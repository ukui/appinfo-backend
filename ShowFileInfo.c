#include <string.h>
#include <stdio.h>
#include <glib-2.0/glib.h>
#include <unistd.h>    
#include <dirent.h>   
#include <sys/stat.h>
#include <stdlib.h>  
#include <malloc.h>
#include <libgen.h>
#include <gio/gdesktopappinfo.h>
#include "ShowFileInfo.h"

char res[40]={0};
char exce_path_res[200]={0};
char icon_path_res[200]={0};

char *Icon_path[]={"/usr/share/icons/hicolor/scalable/apps/",
                    "/usr/share/icons/ukui-icon-theme-default/scalable/apps/",
                    "/usr/share/icons/hicolor/32x32/apps/",
                    "/usr/share/icons/ukui-icon-theme-default/256x256/categories/"};//Icon属性特殊处理，	

int get_envpath()//获取当前笔记本存在可执行文件的路径
{
    int tmp = 0;
    char *token;
    int colon_num = 0;//初始化：个数，路径个数=冒号个数+1
    char *env_path = getenv("PATH");//echo $PATH = /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
    char *envpath_str = (char*)malloc(1024*sizeof(char));
    strcpy(envpath_str,env_path);
    if(env_path == NULL)
    {
        return ERR_EXEC_PATH;
    }
    //查找：的个数，以得到exec个数。for malloc
    for(int j = 0; env_path[j] != '\0'; j++)
    {  
        if(env_path[j] == ':')
            colon_num++;
    }

    //为可执行路径二维数组malloc
    exec_path = (char**)malloc(sizeof(int*)*(colon_num+1));//行：colon_num+1，列：200
    for(int i = 0; i < (colon_num+1); i++) 
    {
        exec_path[i] = (char*)malloc(sizeof(char)*200);  
    } 

    if(envpath_str != NULL)
    {
        token=strtok(envpath_str,":");  
    }
    while(token!=NULL)
    {  
        strcpy(exec_path[tmp++],token);
        printf("path=[%s]\n",token);
        token = strtok(NULL,":");
    }
    free(envpath_str);
    return SUCCESS;  
}

void  get_DESKTOP_SESSION()//获取环境变量
{
    strcpy(DESKTOP_SESSION,"");
    char *curr_desktop = getenv("XDG_CURRENT_DESKTOP");
    if(curr_desktop)
        strcpy(DESKTOP_SESSION,curr_desktop);
}

char* get_exce_path(char *path,char *name)//获取exec绝对路径
{
    if(NULL==path||NULL==name)
    {
        return NULL;
    }
    DIR *dirptr;
	struct dirent *entry;//读readdir函数
	dirptr=opendir(path);
	if(dirptr==NULL)
	{
		printf("路径打开失败!\n");
        return NULL;
	}	
	chdir(path);//修改当前目录
    int i=0;
	while ((entry = readdir(dirptr)) != NULL) 
	{ 
			if(strcmp(entry->d_name,name)==0)//传入的path中有这个文件
            {
                memset(exce_path_res,0,200);
                sprintf(exce_path_res,"%s%s%s",path,"/",name);
                i++;
            }
	}
    if(i == 0)
    {
        closedir(dirptr);
        return NULL;
    }

    closedir(dirptr);
    return exce_path_res;
}

char* get_icon_path(char *path,char *name)//获取icon绝对路径
{
    if(NULL==path||NULL==name)
    {
        return NULL;
    }
    DIR *dirptr;
	struct dirent *entry;//读readdir函数
	dirptr=opendir(path);
	if(dirptr==NULL)
	{
		printf("路径打开失败!\n");
        return NULL;
	}	
    memset(icon_path_res,0,200);
	chdir(path);//修改当前目录
	char name1[1024]={0};
	sprintf(name1,"%s%s",name,".svg");
    printf("【函数接受的name1】=%s \n",name1);
	while ((entry = readdir(dirptr)) != NULL) 
	{ 
			if(strcmp(entry->d_name,name1)==0)//传入的path中有这个文件
            {
                strcat(icon_path_res,path);
                strcat(icon_path_res,name1);
                closedir(dirptr);
                return icon_path_res;
            }
	}
    closedir(dirptr);
	char name2[1024]={0};
    chdir(path);//修改当前目录
    strcat(name2,name);
    strcat(name2,".png");
    DIR *dirptr2;
	struct dirent *entry2;//读readdir函数
	dirptr2=opendir(path);
	while ((entry2 = readdir(dirptr2)) != NULL) 
	{ 
			if(strcmp(entry2->d_name,name2)==0)//传入的path中有这个文件
            {
                strcat(icon_path_res,path);
                strcat(icon_path_res,name2);
                closedir(dirptr2);
                return icon_path_res;
            }
	}
    closedir(dirptr2);
    printf("icon_path:%s\n",icon_path_res);
    return icon_path_res;
}

char* suffix_process(char *desktopName) //aaa.desktop → aaa
{
    if(NULL==desktopName)
    {
        return NULL;
    }
    int count = 0, n = 0;
    char *head = desktopName;
    while(*desktopName != '\0')
    {
        if(*desktopName == '.')
        {
            count = n;
        }
        n++;
        desktopName++;
    }
    memset(res,0,sizeof(res));
    strncpy(res,head,count);
    *(res+count+1)='\0';
    return res;
}

int get_attribut_assist(char *name,char *attribute,char *attribute_output)
{
    if(name==NULL||attribute==NULL)
    {
        return ERR_PARAMETER_INPUT;
    }
    char *value = NULL;
    char tmp[300];
    if(strstr(name,".desktop"))  //name eg:info.desktop 
    {
        sprintf(tmp,"%s%s",DESKTOP_PATH,name);
    }
    else                         //name eg:info
    {
        sprintf(tmp,"%s%s%s",DESKTOP_PATH,name,".desktop");
    }
    
    char *path=tmp;
    //GDesktopAppInfo *file_name = g_desktop_app_info_new(name);
    GDesktopAppInfo *file_name= g_desktop_app_info_new_from_filename (path);
    if(file_name==NULL)
    {
        g_object_unref(file_name);
        printf("path = %s\n",path);
        return ERR_LOADING_KEY_FILE;
    }
    
    value= g_desktop_app_info_get_locale_string (file_name,attribute);
    if(value==NULL)
    {
        g_object_unref(file_name);
        return ERR;
    }
    strcpy(attribute_output,value);
    g_object_unref(file_name);
    free(value);
    return SUCCESS;
}

int show_app_attribute(char *name,char *attribute,char **attribute_output)
{
    if(name==NULL||attribute==NULL)
    {
        return ERR_PARAMETER_INPUT;
    }
    char path[300];
    sprintf(path,"%s%s%s",DESKTOP_PATH,name,".desktop");
	GError * err = NULL;

	GKeyFileFlags flags = G_KEY_FILE_KEEP_TRANSLATIONS;
    GKeyFile * key_file = g_key_file_new ();
    if(!key_file)
    {
        DEBUG("New file Failed\n");
        return ERR_NEW_FILE;        
    }
     
    gboolean ret = g_key_file_load_from_file(key_file,path,flags,&err);
    if(!ret)
    {
        DEBUG ("Error loading key file: %s", err->message);
        g_key_file_free(key_file);
        return ERR_LOADING_KEY_FILE;
    }
    if(!g_key_file_has_key(key_file,"Desktop Entry",attribute,&err))//文件不存在该属性
    {
        printf("文件不存在该属性!!\n");
        g_key_file_free(key_file);
        return ERR_FILE_ATTRIBUTE_NOTHINGNESS;
    }

    char * date = g_key_file_get_locale_string(key_file,"Desktop Entry",attribute,"zh_CN",&err);
    if(!date)
    {
        DEBUG ("Error get  key value: %s", err->message);
        g_key_file_free(key_file);
        return ERR_GET_KEY;
    }
    if(!strcmp(date,""))
    {
        printf ("key value is NULL");
        g_key_file_free(key_file);
        return ERR_GET_KEY;
    }

    //属性的value处理：
    if(!strcmp(attribute,"Exec"))
    {
        date=strtok(date," ");
        if(date[0]=='/')//绝对路径，验证该路径是否存在
        {
            if(access(date,F_OK)==0)      //判断路径下文件是否存在
            {
                strcpy(*attribute_output,date);    
                g_key_file_free(key_file);
                g_free (date);      
                return SUCCESS;                    
            }
            else
            {
                DEBUG("The file under the path shown by exec does not exist \n"); 
                g_key_file_free(key_file);
                g_free (date); 
                return ERR_EXEC_PATH;
            }
        }
        else //相对路径
        {
            int flage=0;
            if(get_envpath()==ERR_EXEC_PATH)
            {
                g_key_file_free(key_file);
                g_free (date);    
                return ERR_EXEC_PATH;
            }
            for(int i=0;i<exec_num;i++)
            {
                char * exec_res=get_exce_path(exec_path[i],date);
                if(exec_res!=NULL)
                {  
                    strcpy(*attribute_output,exec_res);
                    flage=1;
                    for(int j=0;j<exec_num;j++)  
                    {
                        free(exec_path[j]); 
                    }
                    free(exec_path);
                    g_key_file_free(key_file);
                    g_free (date);   
                    return SUCCESS;
                }   
            }
            if(!flage)          //这几个路径下都无该文件，则文件路径查找失败
            {
                printf("Exec属性绝对路径查找失败\n");
                for(int j=0;j<exec_num;j++)  
                {
                    free(exec_path[j]); 
                }
                free(exec_path); 
                g_key_file_free(key_file);
                g_free (date);   
                return ERR_EXEC_PATH;
            }  
        }  
    }

    else if(!strcmp(attribute,"Icon"))
    {
        date=strtok(date," ");
        if(date[0]=='/')
        {
            if(access(date,F_OK)==0)      //判断路径下文件是否存在
            {
                strcpy(*attribute_output,date);   
                g_key_file_free(key_file);
                g_free (date);    
                return SUCCESS;                           
            }
            else
            {
                DEBUG("The file under the path shown by Icon does not exist \n");
                g_key_file_free(key_file);
                g_free (date);  
                return ERR_ICON_PATH;
            }
        }
        else
        {
            int buf=0;
            for(int i=0;i<4;i++)
            {
                if(!get_icon_path(Icon_path[i],date))
                {
                    continue;
                }
                char *res_icon_path=get_icon_path(Icon_path[i],date);
                if(strcmp(res_icon_path,"")!=0)//成功找到该icon.svg/png
                {
                    buf++;						
                    strcpy(*attribute_output,res_icon_path);
                    g_key_file_free(key_file);
                    g_free (date);     
                    return SUCCESS;
                }                           
            } 
            if(!buf)          //这4个路径下都无该文件，则文件路径查找失败
            {
                printf("Icon属性绝对路径查找失败\n");   
                g_key_file_free(key_file);
                g_free (date); 
                return ERR_ICON_PATH;
            }     
        }    
    }
    else if(!strcmp(attribute,"Categories"))
    {
        char *str1=NULL;
        char res[2048]={0};
        str1=strtok(date,";");
        if((!strstr(str1,"GTK"))&&(!strstr(str1,"Utility"))&&(!strstr(str1,"GNOME"))&&(!strstr(str1,"Settings")))//不是这其中的某一个就可以作为结果
        {
            sprintf(res,"%s%s",str1,";");     
        }
        while(str1!=NULL)
        {
            str1=strtok(NULL,";");
            if(str1==NULL)
            {
                break;
            }						
            if((!strstr(str1,"GTK"))&&(!strstr(str1,"Utility"))&&(!strstr(str1,"GNOME"))&&(!strstr(str1,"Settings")))//不是这其中的某一个就可以作为结果
            {				
                strcat(res,str1);
                if(str1!=NULL)
                {
                    strcat(res,";"); 
                }	     
            }
        }
        strcpy(*attribute_output,res);
        g_key_file_free(key_file);
        g_free (date);    
        return SUCCESS;
    }
    else if(!strcmp(attribute,"MimeType"))
    {
        char *token1=NULL;
        char *token2=NULL;
        char res[2048]={0};
        token1=strtok(date,"/");
        strcpy(res,token1);
        token2=strtok(NULL,";");
        while(token2!=NULL)
        {
            token1=strtok(NULL,"/");
            if(token1!=NULL&&strstr(res,token1)==NULL)//结果中不包含这个字符串 需在res中添加该字符串
            {    strcat(res,";");            
                strcat(res,token1);          
            }
            else
            {
                break;
            }
            token2=strtok(NULL,";");
        }
        strcpy(*attribute_output, res);     //  处理字符串 
        g_key_file_free(key_file);
        g_free (date);  
        return SUCCESS;
    }
    else //其它属性
    {
        strcpy(*attribute_output,date);
        g_key_file_free(key_file);
        g_free (date);  
        return SUCCESS;  
    }
    return SUCCESS; 
}

//show_attribute对比属性值是否存在
int show_attribute(char *name,char *attribute)
{
    if(name==NULL||attribute==NULL)
    {
        return ERR_PARAMETER_INPUT;
    }
    char *tmp=(char*)malloc(1024*sizeof(char));
    memset(tmp,0,1024);
    sprintf(tmp,"%s%s",DESKTOP_PATH,name);
    GDesktopAppInfo *file_name= g_desktop_app_info_new_from_filename (tmp);
    if(file_name==NULL)
    {
        g_object_unref(file_name);
	    free(tmp);
        return ERR_LOADING_KEY_FILE;
    }
    
    if(g_desktop_app_info_has_key(file_name,attribute))//属性值存在：g_desktop_app_info_has_key()==TRUE
    {
        g_object_unref(file_name);
     	free(tmp);
        return SUCCESS;
    }
    else
    {
        g_object_unref(file_name);
	    free(tmp);
        return ERR;
    }
}

char invisible_list[1024][1024];
int app_num=0;

void get_invisible_list()//读取文件获取用户不可见列表
{
    app_num=0;
    memset(invisible_list,0,sizeof(invisible_list));
    FILE *fp;
	char line[1000];
    char * file_path = "/etc/invisible_list.txt";     
	fp=fopen(file_path,"r");
	if(fp==NULL)
	{
		printf("can not load file:%s!\n",file_path);
		return;
	}
	while(!feof(fp))
	{
		fgets(line,1000,fp);
        app_num++;
        char *tmp=strtok(line,"");
        strcpy(invisible_list[app_num],tmp);
	}
	fclose(fp);
//   printf("fclose return =%d,err=%s\n",ret,strerror(ret));	
}


//show_all_file_name显示所有的系统desktop文件，根据当前系统环境结合NoDisplay、OnlyShowIn等进行过滤
int show_all_file_name(char ***name_output) //sysytem
{
    get_DESKTOP_SESSION();//获取系统DESKTOP_SESSION的环境变量
    printf("当前环境：%s\n",DESKTOP_SESSION);
    get_invisible_list();       //获取用户不可见列表，保存在invisible_list中
    desktop_row=0;
    *name_output=(char**)malloc(1024*sizeof(char*));
    char *value_forkey=(char*)malloc(1024*sizeof(char));//用于存储某属性的value

    for(int i=0;i<1024;i++)
    {
        (*name_output)[i]=(char*)malloc(2048);
    }     
	DIR *dirptr;
	struct dirent *entry;//读readdir函数 
	char *str=".desktop";
	dirptr=opendir(DESKTOP_PATH1);
	if(dirptr==NULL)
	{
		printf("路径打开失败!\n");
        free(value_forkey);
		return  ERR_PATH_OPEN;
	}	
	chdir(DESKTOP_PATH1);//修改当前目录

	while ((entry = readdir(dirptr)) != NULL) 
	{ 
        memset(value_forkey,0,1024);
        /*列表内不输出  *设置标记位flage
        *flage=0 表示在列表内，不输出 *flage=1 表示可以输出*/
        int flage=1;
        for(int i=0;i<app_num;i++)
        {
            if(strstr(invisible_list[i],entry->d_name)!=NULL)//在列表内
            {
                flage=0;
                break;
            }
        }
        
        if(strstr(entry->d_name,str)==NULL)//不存在该desktop
        {   
            continue;
        }
        /*无Exec、Icon属性的应用不可见*/
        else if((ERR==show_attribute(entry->d_name,"Exec"))||(ERR==show_attribute(entry->d_name,"Icon")))///不显示的应用
        {
            continue;
        }
        else if((SUCCESS==get_attribut_assist(entry->d_name,"NoDisplay",value_forkey)) && (strcasecmp(value_forkey,"true")==0)) //存在NoDisplay属性，并为true就不显示为用户应用
        {
            memset(value_forkey,0,1024);
            continue;
        }
        else if((SUCCESS==get_attribut_assist(entry->d_name,"OnlyShowIn",value_forkey)) && (strstr(value_forkey,DESKTOP_SESSION)==NULL))//不包含当前系统环境，不输出
        {
            memset(value_forkey,0,1024);
            continue;
        }
        else if ((SUCCESS==get_attribut_assist(entry->d_name,"NotShowIn",value_forkey)) && (strstr(value_forkey,DESKTOP_SESSION)!=NULL))
        {
            memset(value_forkey,0,1024);
            continue;
        }  
        else if(flage == 0)
        {
            continue;
        }
        else	
        {    
            memset(value_forkey,0,1024);
            char *res=suffix_process(entry->d_name);        
            strcpy((*name_output)[desktop_row++],res);
        }
	}
	printf("close return =%d\n",closedir(dirptr));
    free(value_forkey);
	return 0;
}

//get_link_source根据desktop文件exec属性的绝对路径获取其链接到的真实路径
//如果不是ln文件，那src传出来的，还是links
//如果是links，那传出来的就是真实文件
int get_link_source(char * exec_path,char *output_real_path)
{
    //判空
    if(NULL==exec_path||NULL==output_real_path)
    {
        return ERR;
    }    
    struct stat st;
    int flage;
    lstat(exec_path,&st); 
    flage=S_ISLNK(st.st_mode); //符号链接就返回1，否则返回0

    while(flage)//是符号链接
    {
        char buf[200]={0};
        char end_path[200]={0};
        char real_link_path[400]={0};
        char resolved_path[400]={0};
        int size=200;
        //执行成功则返回字符串的字符数，失败返回-1， 错误代码存于errno。
        int rt=readlink(exec_path, buf, size);
        if(rt==-1)     //执行失败
        {
            return ERR;
        }
        else        //执行成功
        {
            if(buf[0]!='/')//连接是一个相对路径
            {
                char *pre_path=dirname(exec_path);//eg:/usr/bin/libreoffice的链接文件是../lib/libreoffice/program/soffice
                                                    //此函数就返回/usr/bin
                memset(end_path,0,200);
                memset(real_link_path,0,400);
                memset(resolved_path,0,400);
                strcpy(end_path,buf);           //end_path:../lib/libreoffice/program/soffice
                sprintf(real_link_path,"%s%s%s",pre_path,"/",end_path);//buf:   /usr/bin../lib/libreoffice/program/soffice
                realpath(real_link_path, resolved_path);
                strcpy(exec_path,resolved_path);
            }
            //printf("！！buf:%s\n",buf);
            else
            {
                strcpy(exec_path,buf);
            }
            lstat(exec_path,&st); 
            flage=S_ISLNK(st.st_mode); //符号链接就返回1，否则返回0
        }   
    }
    strcpy(output_real_path,exec_path);
    return SUCCESS;
}

//get_package_name函数：根据可执行文件路径获取软件包名
int  get_package_name(char * exec_path,char ** res_package_info,int app_from)
{
    //空判断
    if(exec_path==NULL||(!strcmp(NO_EXEC_PATH,exec_path)))
    {
        return 1;//fail
    }
    char *output_real_path=(char*)malloc(200*sizeof(char));
    memset(output_real_path,0,200);
    if(app_from==TENCENT_APP)   //腾讯应用
    {
        strcpy(output_real_path,exec_path);
    }
    else        //系统应用需要判断可执行路径是否为连接
    {
        get_link_source(exec_path,output_real_path);
        printf("exec_path:%s\n",exec_path);
    }

    //dpkg -S 获取包名
    char *cmdstring=(char*)malloc(1024*sizeof(char));
    char nameBuf[1024] = "";
    memset(cmdstring,0,1024);
    strcat(cmdstring,"dpkg -S ");
    strcat(cmdstring,output_real_path);
    printf("cmdstring：!%s！\n",cmdstring);
    FILE * ptr = popen(cmdstring,"r");
    fgets(nameBuf, 1024, ptr);
    char * tmp =strtok(nameBuf,":"); 
    if (tmp)                // tmp：包名
    {                       //dpkg -s xx | grep Version 获取包版本  
        int version_end=0;
        char *pkg_version_string=(char*)malloc(200*sizeof(char));
        memset(pkg_version_string,0,200);
        //eg:dpkg -s vim | grep Version  => Version: 2:8.1.2269-1ubuntu5
		sprintf(pkg_version_string,"%s%s%s","dpkg -s ",tmp,"| grep Version");
		FILE * ptr_pkg_Version = popen(pkg_version_string,"r");
		char *res_version=(char*)malloc(1024*sizeof(char));
        memset(res_version,0,1024);
		fgets(res_version, 1024, ptr_pkg_Version);
        strcpy(*res_package_info,tmp);  //eg: vim的包名 =>  vim
        strcat(*res_package_info," ="); //此时res_package_info => vim =
        //用于判断dpkg命令结果的结束位置，version_end用于记录结束下标
        for(int i=0;i<1024;i++) 
        {
            if(res_version[i]=='\n')
            {
                version_end=i;
                break;
            }   
        }
		strncat(*res_package_info,res_version+8,version_end-8); //将结果“Version:”后（res_version+8）的20（version_end-8）位字符拷贝给res_package_info
        //printf("长度：%d\n",version_end-8);
        pclose(ptr);
        pclose(ptr_pkg_Version);
        free(cmdstring);
        free(output_real_path);
        free(pkg_version_string);
		free(res_version);
        return 0;
    }
    else
    {
        pclose(ptr);
        free(cmdstring);
        free(output_real_path);
        return 1;
    }   
}

void desktop_free(char **res)
{
    for(int i=0;i<1024;i++)
    {
        if(res[i]!=NULL)
        {
            free(res[i]);
        }
    } 
    free(res);
}