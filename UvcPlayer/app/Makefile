CURRENT_PATH = $(shell pwd)
CROSS_COMPILE ?=arm-linux-gnueabihf-
CC  = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR  = $(CROSS_COMPILE)ar

#是否使能UVC功能
ENABLE_V4L2 = 1

COM_FLAGS = -Wall -O2 -fPIC -mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -mthumb-interwork -marm -g -funwind-tables -ffunction-sections -rdynamic
COM_FLAGS += -DENABLE_V4L2=$(ENABLE_V4L2)
C_FLAGS   = $(COM_FLAGS) -std=gnu11
CPP_FLAGS = $(COM_FLAGS) -std=gnu++11

INCLUDES  = -I. -I$(CURRENT_PATH)/../libjpeg/include -I$(CURRENT_PATH)/../libyuv/include -I$(CURRENT_PATH)/../sstar/include
INCLUDES += -I$(CURRENT_PATH)/../v4l2/inc -I$(CURRENT_PATH)/../v4l2/src

LIB_PATH  = -L. -L$(CURRENT_PATH)/../libjpeg/lib -L$(CURRENT_PATH)/../libyuv/lib -L$(CURRENT_PATH)/../sstar/lib

TARGET_NAME  = UvcPlayer

CPP_SRCS  =  $(wildcard $(CURRENT_PATH)/*.cpp)
C_SRCS    =  $(wildcard $(CURRENT_PATH)/*.c)
C_SRCS   +=  $(wildcard $(CURRENT_PATH)/../v4l2/src/*.c)


CPP_OBJS  = $(patsubst %.cpp, %.cpp.o, $(CPP_SRCS))
C_OBJS    = $(patsubst %.c, %.c.o, $(C_SRCS))

LIB_NAME  = -lmi_panel -lmi_sys -lmi_divp -lmi_vpe -lmi_isp -lcus3a -lispalgo -lmi_disp -lmi_common -lcam_os_wrapper
LIB_NAME += -lrt -lyuv -lturbojpeg -ljpeg

.PHONY: all prepare clean

all: prepare $(TARGET_NAME) finish

prepare:
	@echo
	@echo ">>>>========================================================"
	@echo "TARGET_NAME = $(TARGET_NAME)"
	@echo

clean:
	@rm -rf $(CPP_OBJS)
	@rm -rf $(C_OBJS)
	@rm -rf $(TARGET_NAME)

finish:
	@echo "make done"
	@echo "<<<<========================================================"
	@echo

$(TARGET_NAME): $(CPP_OBJS) $(CPP_SRCS) $(C_OBJS) $(C_SRCS)
	@echo "generate $@"
	@$(CPP) -o $@ $(C_OBJS) $(CPP_OBJS) $(LIB_PATH) $(LIB_NAME) -ldl -lm -lpthread

%.c.o : %.c
	@echo "compile $@"
	@$(CC) $(C_FLAGS) $(INCLUDES) -c $< -o $@

%.cpp.o : %.cpp
	@echo "compile $@"
	@$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@

