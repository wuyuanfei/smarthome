TOP_DIR := $(shell pwd)
JSON 	:= /home/ubuntu/json_arm/include/
SQLITE 	:= /home/ubuntu/sqlite3_arm/include/

CROSS_COMPILE ?= arm-none-linux-gnueabi-
CXX = $(CROSS_COMPILE)g++

AR = $(CROSS_COMPILE)ar
CXXFLAGS = -c -g -Wall  -I$(JSON) -I$(SQLITE)
INCLUDE_PATH = -I$(JSON) -I$(SQLITE)
APPNAME = SmartHome 

export TOP_DIR CXX CXXFLAGS AR

C-OBJS += $(TOP_DIR)/main.o
C-OBJS += $(TOP_DIR)/tcpServ.o
C-OBJS += $(TOP_DIR)/jsonMsg.o
C-OBJS += $(TOP_DIR)/linkQueue.o
C-OBJS += $(TOP_DIR)/sqlite3.o
C-OBJS += $(TOP_DIR)/threadLogin.o
C-OBJS += $(TOP_DIR)/threadOperate.o
C-OBJS += $(TOP_DIR)/threadPool.o
C-OBJS += $(TOP_DIR)/uart.o

lib-y  += /home/ubuntu/json_arm/lib/libjson.a
lib-y  += /home/ubuntu/sqlite3_arm/lib/libsqlite3.a

all:$(C-OBJS)
	$(CXX) $(C-OBJS) $(lib-y) -o $(APPNAME) -lpthread -ldl

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAGS) -o "$@" "$<"

clean:
	@rm -rf $(C-OBJS) $(APPNAME) 