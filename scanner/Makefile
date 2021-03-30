#依赖本地sdk路径
#PROJECT_PATH=/home/koda.xu/P3/test_stable_p3/project
SDKDIR=/home/koda.xu/demo_test/scanner/sdkdir/

#Toolchain设置
CC = arm-linux-gnueabihf-g++
STRIP = arm-linux-gnueabihf-strip

#配置源文件目录
PROJ_ROOT_PATH :=./

#源文件后缀名
PROJ_SRC_SUFFIX := %.cpp %.c
PROJ_HEAD_SUFFIX := %.h

#递归遍历目录下的所有的文件
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#获取相应的源文件
PROJ_ALL_FILES := $(foreach src_path,$(PROJ_ROOT_PATH), $(call rwildcard,$(src_path),*.*) )   
PROJ_SRC_FILES := $(filter $(PROJ_SRC_SUFFIX),$(PROJ_ALL_FILES))   
PROJ_HEAD_FILES := $(filter $(PROJ_HEAD_SUFFIX),$(PROJ_ALL_FILES)) 

# 获取相应的源文件
LOCAL_SRC_FILES  := $(PROJ_SRC_FILES) 
LOCAL_HEAD_FILES := $(PROJ_HEAD_FILES)

#源文件目录结构
PROJ_DIR_TREE := $(dir $(foreach src_path, $(LOCAL_SRC_FILES), $(call rwildcard,$(src_path),*/) ) )  
PROJ_DIR_TREE := $(sort $(PROJ_DIR_TREE)) 

#obj文件目录结构
OBJS_OUTPUT_DIR= $(PROJ_ROOT_PATH)obj/
OBJS_1 = $(patsubst %.cpp,%.o,$(LOCAL_SRC_FILES))
OBJS_2 = $(patsubst %.c,%.o,$(OBJS_1))
OBJS   = $(addprefix $(OBJS_OUTPUT_DIR),$(OBJS_2))
OBJS_DIR_TREE=$(addprefix $(OBJS_OUTPUT_DIR),$(PROJ_DIR_TREE))

#bin文件目录
BIN_OUTPUT_DIR= $(PROJ_ROOT_PATH)bin

TARGET = scanner
CFLAGS = -Wno-write-strings
LDFLAGS =
CONFIG_PREFIX =.

LOCAL_CFLAGS = -I$(SDKDIR)/include -I. -I./osd -I./qrcode/include
LOCAL_LDFLAGS = -L$(SDKDIR)/lib/mi_libs -L$(SDKDIR)/lib/ex_libs \
				-L ./qrcode/lib
#LOCAL_LDFLAGS += -lpthread -lm -ldl -lmi_common -lmi_sys -lmi_disp -lmi_panel -lmi_divp -lmi_vpe -lmi_sensor -lmi_vif -lmi_rgn -lmi_isp -lispalgo -lcus3a -lcam_os_wrapper -lZXing

LOCAL_LDFLAGS += -lpthread -lm -ldl -lmi_common -lmi_sys -lmi_disp -lmi_panel -lmi_divp -lmi_vpe -lmi_sensor -lmi_vif -lmi_isp -lispalgo -lcus3a -lcam_os_wrapper -lZXing

all: prepare $(TARGET) install
	@echo genrate $(TARGET) done!

$(TARGET):$(OBJS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^

$(OBJS_OUTPUT_DIR)%.o: %.cpp $(LOCAL_HEAD_FILES)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $< -o $@ 

$(OBJS_OUTPUT_DIR)%.o: %.c $(LOCAL_HEAD_FILES)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $< -o $@

prepare: 
	@echo generating target ...
	@-mkdir -p $(OBJS_DIR_TREE)
	@-mkdir -p $(BIN_OUTPUT_DIR)

install:
	@-mv $(TARGET) $(BIN_OUTPUT_DIR)

clean:
	@echo clean ...
	@-rm -rf $(OBJS_OUTPUT_DIR)
	@-rm -rf $(BIN_OUTPUT_DIR)
	@echo clean done!

.PHONY:all prepare clean $(TARGET)
