

# <center>获取用户应用DBUS接口文档 </center>

## 描述
获取用户可见应用

## 一、Dbus接口信息
|名称|含义|
|--|--|
|BUS类型|SYSTEM BUS|
|DBUS名称|com.kylin.intel.edu.appsinfo|
|OBJECT路径|/com/kylin/intel/edu/appsinfo|
|INTERFACES名称|com.kylin.intel.edu.appsinfo.interface|

## 二、Metohd列表
### 2.1 GetUserAppList

|method name|input args|out args|means|eg|
|--|--|--|--|--|
|GetUserAppList|s|List|获取指定用户的可见应用列表|GetUserAppList(string u_name) -> (Array of [String] app_list)|

*Metohds*：GetUserAppList(string u_name) -> (Array of [String] app_list)
入参：u_name 用户名，如"kylin"
出参：app_list 应用名称列表，如 ['chromium','wechat','chesee']

### 2.2 GetAppDetails
|method name|input args|out args|means|eg|
|--|--|--|--|--|
|GetAppDetails|s|ssss|获取指定应用详情|GetUserAppList(string app_name) -> (string desc,string icon_path,sting app\_type，string app\_id)|


*Metohds*：GetUserAppList(string app\_name) -> (string desc,string icon\_path,sting app\_type，string app\_id)
入参：app\_name 应用名称，如"chromium"
出参：
\* desc 应用描述，如"谷歌浏览器"
\* icon\_path 应用图标对应绝对路径，如"/usr/share/icons/mate/256x256/mimetypes/text-x-preview.icon"
\* app\_type:应用类型，如教育"edu"、休闲"arder"、其他"other"
\* app\_id:应用标识，如"WebBrowser"

### 2.3 GetSysAppList

|method name|input args|out args|means|eg|
|--|--|--|--|--|--|
|GetSysAppList||List|获取系统所有应用列表|GetUserAppList() -> (Array of [String] app\_list)|

*Metohds*：GetUserAppList() -> (Array of [String] app\_list)
入参：无
出参：app\_list 应用名称列表，如 ['chromium','wechat','chesee']
