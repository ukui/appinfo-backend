PKG-CONFIG=`pkg-config --cflags --libs dbus-1 glib-2.0 dbus-glib-1 gio-2.0 gio-unix-2.0`

appsinfo: main.c dbus-server.c ShowFileInfo.c 
	gcc $^ -o $@ -g -Wall ${PKG-CONFIG} -I/usr/include/glib-2.0 -I/usr/include/glib-2.0/glib -I/usr/include/glib-2.0/glib/include 
#-I/usr/lib/x86_64-linux-gnu/glib-2.0/include

clean:
	-rm appsinfo
