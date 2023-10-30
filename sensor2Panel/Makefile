TARGET = zkgui

#赋值对应的路径,此路径需要和本地sdk路径一致
PROJECT_PATH = sstar

COMPILE_ROOT := $(shell pwd)
CC = arm-linux-gnueabihf-g++
STRIP = arm-linux-gnueabihf-strip

CFLAGS = -I$(PROJECT_PATH)/include -I$(PROJECT_PATH)/include/isp -I$(PROJECT_PATH)/include/isp/ikayaki
LDFLAGS = -L$(PROJECT_PATH)/lib
CONFIG_PREFIX =.

#LOCAL_CFLAGS = -rdynamic
LOCAL_LDFLAGS = -ldl -lmi_common -lmi_sys -lmi_disp -lmi_panel -lcam_os_wrapper -Wl,--hash-style=gnu

CPPSRCS += ${wildcard *.cpp}
CPPOBJS = $(patsubst %.cpp, %.cpp.o, $(CPPSRCS))

CSRCS += ${wildcard *.c}
COBJS = $(patsubst %.c,%.c.o,$(CSRCS))

all:$(TARGET) install

$(TARGET):$(CPPOBJS) $(COBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^

$(CPPOBJS):%.cpp.o:%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@

$(COBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@

install:
	-mkdir -p $(CONFIG_PREFIX)/bin
	-mv $(TARGET) $(CONFIG_PREFIX)/bin/

clean:
	-rm -rf $(CPPOBJS) $(CONFIG_PREFIX)/bin/
	-rm -rf *.o
.PHONY:all clean $(TARGET) install
