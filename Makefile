#Makefile for client, server

#build tool configure
CROSS_ARCH = arm
CROSS_COMPILE = /home/joison/invengo-work/usr/local/arm/3.4.1/bin/arm-linux-

#uncomplete: if cross arch equle arm 
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip

#build directory
ROOT_DIR = $(PWD)
OUT_DIR = $(ROOT_DIR)/bin
TEST_DIR = $(ROOT_DIR)/test
SRC_DIR = $(ROOT_DIR)/src
CLIENT_DIR = $(SRC_DIR)/client
SERVER_DIR = $(SRC_DIR)/server
PROCESS_DIR = $(SRC_DIR)/process

#target name
CLIENT_BIN = $(CROSS_ARCH)-client
SERVER_BIN = $(CROSS_ARCH)-server

#sub-makefile
include $(CLIENT_DIR)/Makefile
include $(SERVER_DIR)/Makefile
include $(PROCESS_DIR)/Makefile

#install:
	
