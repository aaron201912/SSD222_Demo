CROSS_COMPILE ?=arm-linux-gnueabihf-
CC  := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)g++
AR  := $(CROSS_COMPILE)ar

#PROJECT_PATH = /users/shaojun.ke/work/SDK/SSC2XX/Linux/v015
PROJECT_PATH = ../../../sourcecode/project

INTERNAL_MODULE := common vif vpe venc

COM_FLAGS = -Wall -O2 -fPIC -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -marm
C_FLAGS    = $(COM_FLAGS)
CPP_FLAGS  = $(COM_FLAGS)

INCLUDES  := -I. -I$(PROJECT_PATH)/release/include -I$(PROJECT_PATH)/release/include/isp
INCLUDES  += $(foreach dir,$(INTERNAL_MODULE), -I./internal/$(dir))

TARGET_NAME  = SnapPic

CPP_SRCS  := $(foreach dir, $(INTERNAL_MODULE), $(wildcard internal/$(dir)/*.cpp))
CPP_SRCS  += $(wildcard *.cpp)
C_SRCS    := $(foreach dir, $(INTERNAL_MODULE), $(wildcard internal/$(dir)/*.c))
C_SRCS    += $(wildcard *.c)

CPP_OBJS = $(patsubst %.cpp, %.cpp.o, $(CPP_SRCS))
C_OBJS = $(patsubst %.c, %.c.o, $(C_SRCS))

LIB_PATH  = -L. -L$(PROJECT_PATH)/release/dispcam/p3/common/glibc/9.1.0/mi_libs/dynamic -L$(PROJECT_PATH)/release/dispcam/p3/common/glibc/9.1.0/ex_libs/dynamic/
LIB_NAME  = -lm -lpthread -ldl
#LIB_PATH += -lmi_sys -lmi_disp -lmi_common -lmi_vif -lmi_vpe -lmi_venc -lcus3a -lmi_isp -lispalgo -lmi_iqserver -lmi_sensor -lcam_os_wrapper -lmi_divp -lfbc_decode
#LIB_NAME += -lmi_panel

LIB_PATH += -lmi_sys -lmi_common -lmi_vif -lmi_vpe -lmi_venc -lcus3a -lmi_isp -lispalgo -lmi_iqserver -lmi_sensor -lcam_os_wrapper -lmi_divp -lfbc_decode
#LIB_PATH += -L$(PROJECT_PATH)/release/dispcam/p3/common/glibc/9.1.0/ex_libs/static/

.PHONY: all prepare clean

all: prepare $(TARGET_NAME) finish

prepare:
	@echo
	@echo ">>>>========================================================"
	@echo $(INCLUDES)
	@echo $(CPP_SRCS)
	@echo $(C_SRCS)
	@echo $(LIB_PATH)
	@echo $(LIB_NAME)
	@echo $(DEFINES)
	@echo "TARGET_NAME = $(TARGET_NAME)"
	@echo


clean:
	@rm -Rf $(CPP_OBJS)
	@rm -Rf $(C_OBJS)
	@rm -Rf $(TARGET_NAME)

finish:
	@echo "make done"
	@echo "<<<<========================================================"
	@echo

$(TARGET_NAME): $(CPP_OBJS) $(CPP_SRCS) $(C_OBJS) $(C_SRCS)
	@echo "generate $@"
	@$(CPP) -o $@ $(C_OBJS) $(CPP_OBJS) $(LIB_PATH) $(LIB_NAME)

%.c.o : %.c
	@echo "compile $@"
	@$(CC) $(C_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

%.cpp.o : %.cpp
	@echo "compile $@"
	@$(CPP) $(CPP_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@
