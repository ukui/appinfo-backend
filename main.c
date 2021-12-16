/*
 * @,@Author: ,: your name
 * @,@Date: ,: 2020-12-23 11:20:14
 * @,@LastEditTime: ,: 2020-12-23 20:07:10
 * @,@LastEditors: ,: Please set LastEditors
 * @,@Description: ,: In User Settings Edit
 * @,@FilePath: ,: /ztt/main.c
 */

#include "dbus-server.h"
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
#include <signal.h>

const struct DBusObjectPathVTable server_vtable = {.message_function = server_message_handler};
void resource_recovery(int sig)
{
    exit(sig);
}
int main()
{
    DBusConnection *conn;
    DBusError err;
    int rv;
    void * in_data=0;
    signal(SIGTERM,resource_recovery);
    signal(SIGKILL,resource_recovery);
    signal(SIGINT,resource_recovery);
    
    dbus_error_init(&err);//将错误对象连接到dbus
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (!conn)
    {
        goto fail;
    }
    rv = dbus_bus_request_name(conn,
                               THIS_DBUS_NAME,
                               DBUS_NAME_FLAG_REPLACE_EXISTING, //one of the standard flag
                               &err);//为这个dbus连接(DbusConnection)起名
    if (rv != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
        printf("fail to request a dbus name!,err name=%s,err msg = %s\n",err.name,err.message);
        goto fail;
    }
    if (!dbus_connection_try_register_object_path(conn,
                                                  THIS_DBUS_OBJECT_PATH,
                                                  &server_vtable,
                                                  in_data,
                                                  &err))
    {
        printf("fail to register a object path!,err name=%s,err msg = %s\n",err.name,err.message);
        goto fail;
    }
    printf("DBUS服务启动成功，进入DBUS消息循环监听状态...\n");
    
    mainloop = g_main_loop_new(NULL, false);
    dbus_connection_setup_with_g_main(conn, NULL);
    g_main_loop_run(mainloop);
    return 0;
fail:
    dbus_error_free(&err);
    return 11;
}
