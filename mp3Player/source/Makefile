CURRENT_PATH = $(shell pwd)
CROSS_COMPILE ?=arm-linux-gnueabihf-
CC  = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR  = $(CROSS_COMPILE)ar

GCC_VERSION ?=6.4.0

ENABLE_STATIC ?=0

# config hdmi or panel display
CONFIG_SUPPORT_HDMI =
-march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -marm
#COM_FLAGS = -Wall -O2 -fPIC -mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -mthumb-interwork -marm -g -funwind-tables -ffunction-sections -rdynamic
#COM_FLAGS = -Wall -O2 -fPIC -marm -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -mthumb-interwork -g -funwind-tables -ffunction-sections -rdynamic
#COM_FLAGS = -Wall -O2 -fPIC -marm -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -g
COM_FLAGS = -Wall -O2 -fPIC -marm -march=armv7-a -mtune=cortex-a7 -g
C_FLAGS   = $(COM_FLAGS) -std=c11
CPP_FLAGS = $(COM_FLAGS) -std=c++11

INCLUDES  = -I. -I$(CURRENT_PATH)/libmad/include -I$(CURRENT_PATH)/sstar/include
LIB_PATH  = -L.

TARGET_NAME ?= Mp3Player

ifeq (${ENABLE_STATIC}, 1)
LIB_PATH += -L$(CURRENT_PATH)/libmad/lib/${GCC_VERSION}/static
#LIB_PATH += -L$(CURRENT_PATH)/sstar/lib/${GCC_VERSION}/static
LIB_PATH += -L$(CURRENT_PATH)/sstar/lib/${GCC_VERSION}/dynamic
else
LIB_PATH += -L$(CURRENT_PATH)/libmad/lib/${GCC_VERSION}/dynamic
LIB_PATH += -L$(CURRENT_PATH)/sstar/lib/${GCC_VERSION}/dynamic
endif



CPP_SRCS  =  $(wildcard $(CURRENT_PATH)/*.cpp)
C_SRCS    =  $(wildcard $(CURRENT_PATH)/*.c)

CPP_OBJS  = $(patsubst %.cpp, %.cpp.o, $(CPP_SRCS))
C_OBJS    = $(patsubst %.c, %.c.o, $(C_SRCS))

ifeq (${ENABLE_STATIC}, 1)
#LIB_NAME  = -Wl,-Bstatic -lmi_ao -lmi_sys -lmi_common -lcam_os_wrapper -lmad -Wl,-Bdynamic -lpthread -lm -ldl
LIB_NAME  = -Wl,-Bstatic -lmad -Wl,-Bdynamic -lmi_ao -lmi_sys -lmi_common -lcam_os_wrapper -lpthread -lm -ldl
else
LIB_NAME  = -lpthread -lm -ldl -lmi_ao -lmi_sys -lmi_common -lcam_os_wrapper -lmad
endif


.PHONY: all prepare clean

all: prepare $(TARGET_NAME) finish

prepare:
	@echo
	@echo ">>>>========================================================"
	@echo "TARGET_NAME = $(TARGET_NAME)"
	@echo

clean:
	@rm -Rf $(CPP_OBJS)
	@rm -f $(C_OBJS)
	@rm -Rf $(TARGET_NAME)*

finish: $(TARGET_NAME)
	@echo "make done"
	@echo "<<<<========================================================"
	@echo

$(TARGET_NAME): $(CPP_OBJS) $(CPP_SRCS) $(C_OBJS) $(C_SRCS)
	@echo "generate $@"
	@$(CPP) -o $@ $(C_OBJS) $(CPP_OBJS) $(LIB_PATH) $(LIB_NAME) -lm -lpthread

%.c.o : %.c
	@echo "compile $@"
	@$(CC) $(C_FLAGS) $(INCLUDES) -c $< -o $@

%.cpp.o : %.cpp
	@echo "compile $@"
	@$(CPP) $(CPP_FLAGS) $(INCLUDES) -c $< -o $@

