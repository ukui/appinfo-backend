#ifndef __MDM_DBUS_SER__
#define __MDM_DBUS_SER__

#include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h> /* for glib main loop */
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#define EXEC "Exec"
#define ICON "Icon"
#define NAME "Name"
#define NAME_zh_CN "Name[zh_CN]"
#define CATEGORIES "Categories"
#define MIMETYPE "MimeType"
#define OTHER "other"

//腾讯应用与系统应用的exec、app_path查找方式不同，需要加以区分

#define THIS_DBUS_NAME "com.kylin.intel.edu.appsinfo"
#define THIS_DBUS_OBJECT_PATH "/com/kylin/intel/edu/appsinfo"
#define KYLIN_INTEL_APPSINFO__INTERFACE "com.kylin.intel.edu.appsinfo.interface"

#define CALL_OBJECT_PATH    "/cn/kylinos/SSOBackend"
#define CALL_INTERFACE_PATH "cn.kylinos.SSOBackend.applications"
#define CALL_METHOD         "GetDesktopAppList"

#define METHOD_LST_INTROSPECT   "Introspect"    
#define METHOD_GET_USER_APP_LIST "GetUserAppList"     //获取用户级应用列表
#define METHOD_GET_APP_DETAILS   "GetAppDetails"         //获取应用详情
#define METHOD_GET_SYS_APP_LIST "GetSysAppList"        //获取系统级服务列表
#define METHOD_QUIT     "Quit"  //自我脱离Dbus服务


GMainLoop *mainloop;

DBusHandlerResult server_message_handler(DBusConnection *conn, DBusMessage *message, void *data);



// const struct DBusObjectPathVTable server_vtable = {.message_function = server_message_handler};

// struct DBusObjectPathVTable server_vtable;

#endif
