CROSS_COMPILE ?=arm-linux-gnueabihf-
CC  = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR  = $(CROSS_COMPILE)ar

ALKAID_PATH ?= ./../..

COM_FLAGS = -Wall -O2 -fPIC -mtune=cortex-a7 -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -mthumb-interwork -marm -g
C_FLAGS   = $(COM_FLAGS) -std=c11
CPP_FLAGS = $(COM_FLAGS) -std=c++11

INCLUDES  = -I. -I$(ALKAID_PATH)/project/release/include -I../player/include -I../ffmpeg/include
LIB_PATH  = -L../player/lib -L../ffmpeg/lib -L../3rdparty
LIB_PATH += -L$(ALKAID_PATH)/project/release/dispcam/p3/common/glibc/9.1.0/mi_libs/dynamic -L$(ALKAID_PATH)/project/release/dispcam/p3/common/glibc/9.1.0/ex_libs/dynamic

TARGET_NAME  = ssplayer

CPP_SRCS  =  $(wildcard *.cpp */*.cpp)
CPP_SRCS +=  $(wildcard ../src/*.cpp)
C_SRCS    =  $(wildcard *.c */*.c)
C_SRCS   +=  $(wildcard ../src/*.c)

CPP_OBJS  = $(patsubst %.cpp, %.cpp.o, $(CPP_SRCS))
C_OBJS    = $(patsubst %.c, %.c.o, $(C_SRCS))

LIB_NAME  = -lssplayer -lavformat -lavcodec -lavutil -lswscale -lswresample
LIB_NAME += -lmi_panel -lmi_ao -lmi_divp -lmi_disp -lmi_gfx -lmi_sys -lmi_common -lcam_os_wrapper
LIB_NAME += -lssl -lcrypto

.PHONY: all prepare clean

all: prepare $(TARGET_NAME) finish

prepare:
	@echo
	@echo ">>>>========================================================"
	@echo "TARGET_NAME = $(TARGET_NAME)"
	@echo


clean:
	@rm -rf $(CPP_OBJS)
	@rm -ff $(C_OBJS)
	@rm -rf $(TARGET_NAME)

finish:
	@echo "make done"
	@echo "<<<<========================================================"
	@echo

$(TARGET_NAME): $(CPP_OBJS) $(CPP_SRCS) $(C_OBJS) $(C_SRCS)
	@echo "generate $@"
	@$(CC) -o $@ $(C_OBJS) $(CPP_OBJS) $(LIB_PATH) $(LIB_NAME) -ldl -lm -lpthread

%.c.o : %.c
	@echo "compile $@"
	@$(CC) $(C_FLAGS) $(INCLUDES) -c $< -o $@

%.cpp.o : %.cpp
	@echo "compile $@"
	@$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@

