/*
 * @,@Author: ,: your name
 * @,@Date: ,: 2020-12-21 16:00:23
 * @,@LastEditTime: ,: 2020-12-23 21:00:13
 * @,@LastEditors: ,: Please set LastEditors
 * @,@Description: ,: In User Settings Edit
 * @,@FilePath: ,: /ztt/ShowFileInfo.h
 */
#ifndef   _SHOWFILEINFO_H_
#define   _SHOWFILEINFO_H_
#define DEBUG(...) printf(__VA_ARGS__);

#define SUCCESS 0
#define ERR 11
#define ERR_EXEC_PATH 101
#define ERR_NEW_FILE 102
#define ERR_LOADING_KEY_FILE 103
#define ERR_GET_KEY 104
#define ERR_FILE_ATTRIBUTE_NOTHINGNESS 105
#define ERR_PATH_OPEN 106
#define ERR_PARAMETER_INPUT 107
#define ERR_ICON_PATH 108
#define ERR_SHOW_TYPE 109 
#define ERR_GET_USRAPPLIST 110     //从总线获取用户applist失败
#define ERR_GET_ATTRIBUTE 111
#define ERR_GET_PACKAGE_NAME 112
#define ERR_CALL_SSOMETHOD 113 //从sso获取usr的desktoplist返回err消息，或者sso的getusrdesktoplist不存在

#define TENCENT_APP 501     //腾讯应用
#define SYSTEM_APP  502     //系统应用

#define NO_EXEC_PATH "Fail Get Exec Path"  //找不到该路径
#define DESKTOP_PATH "/usr/share/applications/" //.desktop文件的目录 用于拼接
#define DESKTOP_PATH1 "/usr/share/applications" //.desktop文件的目录 用于定位目录 （opendir）

#define TENCENT_APP_PRE_PATH "/usr/share/txeduplatform-runtime-apps/"
#define TENCENT_APP_LAST_PATH "/manifest.json"

int exec_num;//记录系统环境变量PATH获取到的路径个数
char **exec_path;//定义存储可执行路径的二维数组
int desktop_row;//记录desktop文件个数
char DESKTOP_SESSION[1024]; //记录当前系统的DESKTOP_SESSION环境变量
void desktop_free(char **res);//专属free函数
int usr_appnum;//记录用户个人app数目
int usr_desktopnum;//与usr_appnum相对应，记录根据appid找到的desktop个数

/**
show_all_file_name函数:
    means:获取desktop文件名字
    input args:二维name_output作为函数入参，保存路径下所有.desktop文件的文件名
         传入时为NULL，函数内部malloc，函数调用完成需要使用desktop_free函数释放空间
    output args:二维name_output保存路径下所有.desktop文件的文件名
    return:返回值为0即函数调用成功，其它错误码见宏定义
**/
int show_all_file_name(char ***name_output);

/**
show_app_attribute函数:
    means:根据传入的desktop文件名以及欲获取的属性获取对应的属性值
    input args:name 为欲获得属性的文件名
               attribute 为欲获得的属性,即key值
               attribute_output 一维数组，用于保存获取的属性值，即value值。
    output args:一维attribute_output保存保存获取的属性值
    return:返回值为0即函数调用成功，其它错误码见宏定义
**/
int show_app_attribute(char *name,char *attribute,char **attribute_output);

int show_app_type(char *name,char * res_app_type);//获取应用类型函数

/*
    get_package_name函数：
    根据应用可执行路径找所属包名
    调用实例：
    char *a=NULL;
    get_package_name("/usr/bin/firefox",&a);
    printf("%s\n",a);
*/
int  get_package_name(char * exec_path,char ** res_package_info,int app_from);
//int get_common_attribute(char *name,char *attribute,char *attribute_output);
//get_usr_applist函数：根据之前获取到的usr_appid列表找到对应的desktop
//并保存在output_usrapp_desktop中
int get_usr_applist(int num,char **usr_appid,char ***output_usrapp_desktop);

//获取用户应用的APPID，保存在output_appid中，调用时函数内部malloc，外部free
int string_process(char * str,char ***output_appid);

int get_attribut_assist(char *name,char *attribute,char *attribute_output);

int get_envpath();//获取当前笔记本存在可执行文件的路径

void  get_DESKTOP_SESSION();//获取环境变量  DESKTOP_SESSION在头文件中
#endif
