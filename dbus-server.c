#include "dbus-server.h"
#include "ShowFileInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h> /* for glib main loop */
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <gio/gdesktopappinfo.h>

//对外提供的接口列表
static const char *server_introspection_xml =
    DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
    "<node>\n"

    "  <interface name='org.freedesktop.DBus.Introspectable'>\n"
    "    <method name='Introspect'>\n"
    "      <arg name='data' type='s' direction='out' />\n"
    "    </method>\n"
    "  </interface>\n"

    "  <interface name='com.kylin.intel.edu.appsinfo.interface'>\n"
    "   <method name='GetUserAppList'>\n"
    "       <arg name='u_name' type='s' direction='in'/>\n"
    "       <arg name='app_list' type='as' direction='out'/>\n"
    "   </method>\n"

    "   <method name='GetAppDetails'>\n"
    "       <arg name='appid' type='s' direction='in'/>\n"
    "       <arg name='app_name' type='s' direction='out'/>\n"
    "       <arg name='icon_path' type='s' direction='out'/>\n"
    "       <arg name='app_type' type='s' direction='out'/>\n"
    "       <arg name='pkgname' type='s' direction='out'/>\n"  
    "       <arg name='app_path' type='s' direction='out'/>\n"   
    "   </method>\n"
    "   <method name='GetSysAppList'>\n"
    "       <arg name='app_list' type='as' direction='out'/>\n"
    "   </method>\n"
    "   <method name='Quit'>\n"
    "   </method>\n"
    "  </interface>\n"
    "</node>\n";

// server_vtable.message_function = server_message_handler;

DBusHandlerResult server_message_handler(DBusConnection *conn, DBusMessage *message, void *data)
{
    DBusHandlerResult result;
    DBusMessage *reply = NULL;//创建消息对象指针

    DBusError err;
   // bool quit = false;
    dbus_error_init(&err);
    fprintf(stderr, "Got D-Bus request: %s.%s on %s,destination is:%s,sender is:%s\n",
            dbus_message_get_interface(message),
            dbus_message_get_member(message),
            dbus_message_get_path(message),
            dbus_message_get_destination(message),
            dbus_message_get_sender(message));

    if (!(reply = dbus_message_new_method_return(message))) //申请内存适配则事先退出(创建返回消息)
    {
        result = DBUS_HANDLER_RESULT_NEED_MEMORY;
        return result;
    }

    //漫长的调用接口判断，归类调用的功能
    if (dbus_message_is_method_call(message, DBUS_INTERFACE_INTROSPECTABLE, METHOD_LST_INTROSPECT))
    {
        dbus_message_append_args(reply,
                                 DBUS_TYPE_STRING, &server_introspection_xml,
                                 DBUS_TYPE_INVALID);   //将server_introspection_xml定义为消息  
     
    }
     else if(dbus_message_is_method_call(message, KYLIN_INTEL_APPSINFO__INTERFACE, METHOD_QUIT))
     {
         
         g_main_loop_quit(mainloop);     //用于退出主循环 
     }
//3///
    else if(dbus_message_is_method_call(message, KYLIN_INTEL_APPSINFO__INTERFACE, METHOD_GET_USER_APP_LIST))
    {
        DBusMessageIter args;
        char * param="";//?
        char **sys_desktopName=NULL;
       // char **usr_desktopName=NULL;
        const char *msg;
        char *str="";
    
        if(!dbus_message_get_args(message,&err,DBUS_TYPE_STRING,&msg,DBUS_TYPE_INVALID))
        {
            goto fail;
        }

        if (!dbus_message_iter_init(message, &args))
        {
            fprintf(stderr, "Message has no arguments!\n");
        }
 
        else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
            fprintf(stderr, "Argument is not string!\n");
        else //新加外层括号
        {
            dbus_message_iter_get_basic(&args, &param);
            if(strcmp(param,str)==0)
            {
                goto fail;
            }
        }
        printf("Method called with %s\n", param);
        show_all_file_name(&sys_desktopName);//系统应用
        dbus_message_iter_init_append(reply, &args);//在消息中加入参数
        for(int i=0;i<desktop_row;i++)//加入系统应用的desktop
        {
            char *tmp=sys_desktopName[i];
            dbus_message_iter_append_basic(&args,DBUS_TYPE_STRING,&tmp);
        }
        desktop_free(sys_desktopName);
        goto send;
    }

    else if(dbus_message_is_method_call(message, KYLIN_INTEL_APPSINFO__INTERFACE, METHOD_GET_SYS_APP_LIST))
    {
        DBusMessageIter iter;
        char **sys_app_name=NULL;
        show_all_file_name(&sys_app_name);
        dbus_message_iter_init_append(reply, &iter);//在消息中加入参数

        for(int i=0;i<desktop_row;i++)
        {
            char *tmp=sys_app_name[i];
            dbus_message_iter_append_basic(&iter,DBUS_TYPE_STRING,&tmp);
        }
        desktop_free(sys_app_name);
        goto send;
    }
////////
    else if(dbus_message_is_method_call(message, KYLIN_INTEL_APPSINFO__INTERFACE, METHOD_GET_APP_DETAILS))
    {
        DBusMessageIter iter;
        int get_execcode,get_iconcode,get_catecode;
        int app_from=0;   //用于应用区分的标记位
        char *test=(char*)malloc(1024*sizeof(char));//用于测试appname是否存在
        char *app_name="";//接收到的入参
        //描述  Name or Name[zh_CN]
        char *desc=(char*)malloc(1024*sizeof(char));
        char *tmp_desc=(char*)malloc(1024*sizeof(char));
        char * app_id=(char*)malloc(1024*sizeof(char));//可执行路径包名
        char *icon_path=(char*)malloc(1024*sizeof(char));//图标路径
        char *res_type=(char*)malloc(1024*sizeof(char));//应用类型
        char *app_path=(char*)malloc(1024*sizeof(char));//exec 可执行路径
        char *res_app_path=(char*)malloc(1024*sizeof(char));//获取包名函数可能会改变app_path的值（可执行文件为连接文件时，app_path会变为连接文件路径）
        char *assist=app_path;//记住地址，下次释放
        
        if(!dbus_message_get_args(message,&err,
                                    DBUS_TYPE_STRING,&app_name,
                                    DBUS_TYPE_INVALID))
        {   
            goto free;
            goto fail;
        }
        if (!dbus_message_iter_init(message, &iter))
        {
            fprintf(stderr, "Message has no arguments!\n");
        }
        else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter))
            fprintf(stderr, "Argument is not string!\n");
        else
        {
            dbus_message_iter_get_basic(&iter, &app_name);
            printf("接收到的入参(AppName)为：%s\n",app_name);
        }
        if(show_app_attribute(app_name,NAME,&test)==ERR_LOADING_KEY_FILE)
        {
            goto free;
            goto fail;
        }
        printf("name属性为:%s \n",test);

        // 应用区分-begin（系统应用TENCENT_APP、系统应用SYSTEM_APP）
        char value_forappid[1024]={0};
        if(SUCCESS==get_attribut_assist(app_name,"Appid",value_forappid))
        {
            printf("value_forappid:%s",value_forappid);
            app_from=TENCENT_APP;
        }
        else
        {
            app_from=SYSTEM_APP;
        }
        //应用区分-end

        if(!show_app_attribute(app_name,NAME_zh_CN,&tmp_desc))
        {
            strcpy(desc,tmp_desc);
        }
        else
        {
            show_app_attribute(app_name,NAME,&desc);
        }
        printf("【desc】=【%s】\n",desc);

        //可执行路径-begin(腾讯应用为"/usr/share/txeduplatform-runtime-apps/"+APPID+/manifest.json)
        if(app_from==TENCENT_APP)
        {
            sprintf(res_app_path,"%s%s%s",TENCENT_APP_PRE_PATH,value_forappid,TENCENT_APP_LAST_PATH);
        }
        else
        {
            get_execcode=show_app_attribute(app_name,EXEC,&app_path);
            if(SUCCESS!=get_execcode)
            {
                strcpy(app_path,NO_EXEC_PATH);
            }
            strcpy(res_app_path,app_path); 
        }
        printf("【app_path】=【%s】\n",res_app_path);
        ///可执行路径-end

        get_iconcode=show_app_attribute(app_name,ICON,&icon_path);
        //No need to judge==begin
        if(get_iconcode==ERR_FILE_ATTRIBUTE_NOTHINGNESS)//文件不存在Icon属性
        {    
            strcpy(icon_path,"Icon attribute does not exist");
        }
        else if(get_iconcode!=SUCCESS)
        {
            strcpy(icon_path,"Failed to find the path of icon");
        }
        printf("【icon_path】=【%s】\n",icon_path);
        //No need to judge==end    

        //获取包名、版本--begin
        if(app_from==TENCENT_APP)   //腾讯app：根据desktop文件路径去找包
        {
            sprintf(app_path,"%s%s%s",DESKTOP_PATH,app_name,".desktop");
        }
        if(SUCCESS==get_execcode||app_from==TENCENT_APP)//腾讯应用或者查找exec成功就找对应的包
        {
            if(ERR_GET_PACKAGE_NAME==get_package_name(app_path,&app_id,app_from))
            {
                strcpy(app_id,"Failed to get package name");
            }
            printf("【app_id】=【%s】\n",app_id);
        }
        else
        {
            strcpy(app_id,"Failed to get package name");
        }
        //获取包名、版本--end
       
        //应用类型
        get_catecode=show_app_attribute(app_name,CATEGORIES,&res_type);
        printf("CATEGORIE函数获取返回值为：%d\n",get_catecode);
        if(get_catecode)//不存在Categories属性则判断是否存在MimeType属性
        {           
			if(show_app_attribute(app_name,MIMETYPE,&res_type)!=SUCCESS)
			{strcpy(res_type,OTHER);}  //这两个属性都不存在，就返回other
        }
        if(!strcmp(res_type,""))//如果所有的CATEGORIES属性都被过滤掉了，就返回other
        {
            strcpy(res_type,OTHER);
        }
        printf("【res_type】=【%s】\n",res_type);
        
        dbus_message_iter_init_append(reply, &iter);//在消息中加入参数
        dbus_message_iter_append_basic(&iter,DBUS_TYPE_STRING,&desc);
        dbus_message_iter_append_basic(&iter,DBUS_TYPE_STRING,&icon_path);
		dbus_message_iter_append_basic(&iter,DBUS_TYPE_STRING,&res_type);
        dbus_message_iter_append_basic(&iter,DBUS_TYPE_STRING,&app_id);
		dbus_message_iter_append_basic(&iter,DBUS_TYPE_STRING,&res_app_path);
        goto free;
        goto send;

    free:
        free(app_id);
        free(assist);
        free(res_app_path);
        free(tmp_desc);
        free(test);
        free(desc);
        free(icon_path);
        free(res_type);
    }

    else if(dbus_message_is_method_call(message, KYLIN_INTEL_APPSINFO__INTERFACE, METHOD_QUIT))
    {
        g_main_loop_quit(mainloop);
    }
    else
    {
        result =  DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

fail:
    if(dbus_error_is_set(&err))
    {
        if(reply)
        {
            reply=dbus_message_new_error(message,err.name,err.message);
            dbus_error_free(&err);
        }      
    }

    if(!reply)
    {
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }


send:
        //消息回复、分发
    if (!dbus_connection_send(conn, reply, NULL)) // 发送该消息  
    result = DBUS_HANDLER_RESULT_NEED_MEMORY; 
    dbus_message_unref(reply); // 释放消息对象
    return result;  

}
