AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=server
server_SOURCES=main.c \
			   common.h \
			   net.h \
			   net.c \
			   database.h \
			   database.c \
			   services.h \
			   services.c
INCLUDES=`mysql_config --cflags`
LIBS=`mysql_config --libs`
