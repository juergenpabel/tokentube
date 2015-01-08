AUTOMAKE_OPTIONS = foreign

libexec_PROGRAMS = sso-greeter-lightdm
libexecdir = $(prefix)/usr/lib/tokentube/libexec

sso_greeter_lightdm_SOURCES = sso-greeter-lightdm.c
sso_greeter_lightdm_CFLAGS  = -I../../../include -I/usr/lib/`uname -i`-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0 -I/usr/include/lightdm-gobject-1
sso_greeter_lightdm_LDFLAGS = -L../../lib/ -L/usr/lib -L/usr/lib/`uname -i`-linux-gnu -llightdm-gobject-1 -lgobject-2.0 -lglib-2.0 -lgtk-3 -lconfuse -ltokentube

