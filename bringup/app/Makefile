TARGET = zkgui

#赋值对应的路径,此路径需要和本地sdk路径一致
#PROJECT_PATH=/home/beal.wu/i2m_i6_master/project
#PROJECT_PATH=/home/koda.xu/I6/master/project
PROJECT_PATH = ../zk_full/jni/sdkdir

ZKGUI_PATH=./zkgui_dir
COMPILE_ROOT := $(shell pwd)
CC = arm-linux-gnueabihf-g++
STRIP = arm-linux-gnueabihf-strip
USE_BOOTUP:=0

CFLAGS = -I$(ZKGUI_PATH)/include -I$(ZKGUI_PATH)/include/freetype2 -I$(ZKGUI_PATH)/include/easyui -I$(PROJECT_PATH)/include
LDFLAGS = -L$(ZKGUI_PATH)/lib -L$(PROJECT_PATH)/lib
CONFIG_PREFIX =.

LOCAL_CFLAGS = -rdynamic

ifeq ($(USE_BOOTUP), 1)
LOCAL_LDFLAGS = -leasyui -lnanovg -lpng12 -ljpeg -lpthread -lfreetype -ldl -lts -lz -lmi_common -lmi_gfx -lmi_sys -lmi_disp -lmi_panel -Wl,--hash-style=gnu
else
#LOCAL_LDFLAGS = -leasyui -lnanovg -lpng12 -ljpeg -lpthread -lfreetype -ldl -lts -lz -lmi_common -lmi_gfx -lmi_sys -lmi_disp -lmi_panel -lmi_ai -lmi_ao -lcam_os_wrapper -Wl,--hash-style=gnu
LOCAL_LDFLAGS = -leasyui -lnanovg -lpng12 -ljpeg -lpthread -lfreetype -ldl -lts -lz -lmi_common -lmi_gfx -lmi_sys -lmi_disp -lmi_panel -lcam_os_wrapper -Wl,--hash-style=gnu
endif

CPPSRCS += ${wildcard *.cpp}
CPPOBJS = $(patsubst %.cpp, %.cpp.o, $(CPPSRCS))

CSRCS += ${wildcard *.c}
COBJS = $(patsubst %.c,%.c.o,$(CSRCS))

all:$(TARGET) install

$(TARGET):$(CPPOBJS) $(COBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^

$(CPPOBJS):%.cpp.o:%.cpp
	$(CC) -DENABLE_HDMI=$(ENABLE_HDMI) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@

$(COBJS):%.c.o:%.c
	$(CC) -DENABLE_HDMI=$(ENABLE_HDMI) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@

install:
	-mkdir -p $(CONFIG_PREFIX)/bin
	-mv $(TARGET) $(CONFIG_PREFIX)/bin/

clean:
	-rm -rf $(CPPOBJS) $(CONFIG_PREFIX)/bin/
	-rm -rf *.o
.PHONY:all clean $(TARGET) install
