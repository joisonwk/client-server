#Makefile
#CC = /home/joison/invengo-work/usr/local/arm/3.4.1/bin/arm-linux-gcc
CC = gcc
#target name
TARGET = server-bin

CFLAGS = -pthread -I./include
DEBUG = -Wall

DEPENDS = main.o \
	process/process_core.o process/id_process/chs_ct.o \
	process/id_process/chs_ctr.o process/id_process/chs_pv.o \
	process/id_process/id_chs.o server/clt_item.o server/server_core.o \
	conn/conn_tcp.o

$(TARGET):$(DEPENDS)
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

clean:
	rm -rf $(DEPENDS) $(TARGET)
