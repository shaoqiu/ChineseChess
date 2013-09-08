AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=client
client_SOURCES=chessbord.c chessbord.h \
			   common.h \
			   login_dialog.c login_dialog.h \
			   main.c \
			   main_dialog.c main_dialog.h \
			   net.c net.h \
			   register_dialog.c register_dialog.h \
			   room_dialog.c room_dialog.h \
			   rules.c rules.h \
			   services.c services.h \
			   sound.c sound.h \
			   ui.c ui.h
INCLUDES=`pkg-config --cflags gtk+-3.0 gthread-2.0`
LIBS=`pkg-config --libs gtk+-3.0 gthread-2.0`
