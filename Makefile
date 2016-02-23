#Makefile for client, server

#build directory
ROOT_DIR = $(PWD)
OUT_DIR = $(ROOT_DIR)/bin
TEST_DIR = $(ROOT_DIR)/test
CONF_DIR = $(ROOT_DIR)/conf
INC_DIR = $(SRV_SRC_DIR)/include
SRV_SRC_DIR = $(ROOT_DIR)/srv_src
CLT_SRC_DIR = $(ROOT_DIR)/clt_src

#srv_src directory
SERVER_DIR = $(SRV_SRC_DIR)/server
PROCESS_DIR = $(SRV_SRC_DIR)/process
CONN_DIR = $(SRV_SRC_DIR)/conn

#clt_src directory


#process method config file
##chinese citizen id process method configure file
### the *.conf must be XX=NAME
ID_PV_CONF_PATH = $(CONF_DIR)/province.conf
ID_CT_CONF_PATH = $(CONF_DIR)/city.conf
ID_CTR_CONF_PATH = $(CONF_DIR)/country.conf
ID_CTRL_FILES = "$(ID_PV_CONF_PATH) $(ID_CT_CONF_PATH) $(ID_CTR_CONF_PATH)"

#build configure
##build tool configure
CROSS_ARCH = arm-
CROSS_COMPILE = /home/joison/invengo-work/usr/local/arm/3.4.1/bin/arm-linux-
##uncomplete: if cross arch equle arm 
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip

##build flags configure
CFLAGS = -DID_CTRL_FILES=$(ID_CTRL_FILES)
DEBUG = -Wall
IFLAGS = -I$(INC_DIR)

SRV_OBJS = $(SRV_SRC_DIR)/main.o 
CLT_OBJS = 

#sub-makefile
include $(SRV_SRC_DIR)/Makefile
include $(SERVER_DIR)/Makefile
include $(PROCESS_DIR)/Makefile
include $(PROCESS_DIR)/id_process/Makefile
include $(CONN_DIR)/Makefile

#target name
CLIENT_BIN = $(CROSS_ARCH)client
SERVER_BIN = $(CROSS_ARCH)server

$(SERVER_BIN):$(SRV_OBJS)
	$(CC) -o S(SERVER_BIN) $@ $(CFLAGS) $(IFLAGS) $(DEBUG)

#include $(CLT_SRC_DIR)/Makefile


#install:
	
