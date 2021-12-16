#include <string.h>
#include <stdlib.h>  
#include <malloc.h>

#define ERR 1
#define SUCCESS 0
int appnum=0;
//获取用户应用的APPID，保存在output_appid中，调用时函数内部malloc，外部free
int string_process(char * str,char ***output_appid)
{
    *output_appid=(char**)malloc(1024*sizeof(char*));
    for(int i=0;i<1024;i++)
    {
        (*output_appid)[i]=(char*)malloc(2048);
    }     
    if(str==NULL||output_appid==NULL)
    {
        return ERR;
    }
    int usr_appunm=0;//用户app数量（初始化为0）
    char *usr_ownapp=(char*)malloc(2048*sizeof(char));//避免段错误
    memset(usr_ownapp,0,2048);
    strcpy(usr_ownapp,str);
    const char s1[2] = ":";
    const char s2[2] = "\"";
    char* token;
    token=strtok(usr_ownapp,s1);//获取第一个子字符串
    if(token!=NULL)
    {
        printf("%s\n",token); //"installed_apps"
        token=strtok(NULL,s1);
        printf("%s\n",token);//[{"app_id"
        token=strtok(NULL,s2);
        strcpy((*output_appid)[usr_appunm++],token);
        printf("[res]:%s\n",token);//"06c71dbf873c46229d66cde9c4e8f18e"
        token=strtok(NULL,s2);
        printf("%s\n",token);
    }
    while(token!=NULL)
    {
        token=strtok(NULL,s1);
        printf("%s\n",token); //"installed_apps"
        token=strtok(NULL,s1);
        printf("%s\n",token);//[{"app_id"
        token=strtok(NULL,s2);
        if(token!=NULL)
        { 
            strcpy((*output_appid)[usr_appunm++],token);
            printf("[res]:%s\n",token);//"0779b2dd64bb4d2ea4dfcb48dbb8c491"
            token=strtok(NULL,s2);
            printf("%s\n",token);
        }
        else
        {
            break;
        }
    }
    appnum=usr_appunm;
    return SUCCESS;
}




int main()
{
    char **usr_appid=NULL;
    char *str="\"installed_apps\":[{\"app_id\":\"06c71dbf873c46229d66cde9c4e8f18e\",\"app_name\":\"精准练习\"},{\"app_id\":\"0779b2dd64bb4d2ea4dfcb48dbb8c491\",\"app_name\":\"中文作文批改高级版\"},{\"app_id\":\"f805d30708234d7d7348bdcd23a627fe\",\"app_name\":\"腾讯文档教育版\"}]}";
    string_process(str,&usr_appid);
    for(int i=0;i<appnum;i++)
    {
        printf("用户应用id为：%s\n",usr_appid[i]);
    }
    return 0;
}