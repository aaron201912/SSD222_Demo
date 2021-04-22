include $(pes_parent_dir)/build/linux-arm-default/toolchain.mk
include $(pes_parent_dir)/build/linux-arm-default/build_option.mk
BIN := ./bin
OBJ := ./obj
LIB := ./lib
TARGET_OUT := $(pes_parent_dir)/out
OUT_LIB := $(TARGET_OUT)/lib
OUT_DATA := $(TARGET_OUT)/data
OUT_BIN := $(TARGET_OUT)/bin
OUT_ETC := $(TARGET_OUT)/etc
OUT_CFG := $(TARGET_OUT)/cfg
OBJECTS := $(addsuffix .o,$(addprefix $(OBJ)/,$(basename $(notdir $(SOURCES)))))
DEPENDS := $(addsuffix .d,$(OBJECTS))
include $(pes_parent_dir)/build/linux-arm-default/current.configs

hide?=@
ifeq "$(BUILDTYPE)" "lib"
SOLIBNAME  = $(LIBNAME).so
TARGET := $(SOLIBNAME)
else ifeq "$(BUILDTYPE)" "bin"
TARGET := $(BINNAME)
endif

C_FLAGS += $(LOCAL_CFLAGS)
BIN_FLAGS += $(LOCAL_CFLAGS)
ifneq "$(FORCE_CXX)" ""
	CXX := $(FORCE_CXX)
endif

.PHONY : all clean release
 
all: hello prepare build post-build release
release:
	$(hide)echo "-----------release----------"
ifeq "$(BUILDTYPE)" "lib"
	cp -rf $(LIB)/$(TARGET) $(OUT_LIB)
else ifeq "$(BUILDTYPE)" "bin"
	cp -rf $(BIN)/$(TARGET) $(OUT_BIN)
endif
ifneq "$(CP_LIB_FILE)" ""
	$(hide)-cp $(CP_LIB_FILE) $(OUT_LIB)
endif
ifneq "$(CP_DATA_FILE)" ""
	$(hide)-cp $(CP_DATA_FILE) $(OUT_DATA)
endif
ifneq "$(CP_BIN_FILE)" ""
	$(hide)-cp $(CP_BIN_FILE) $(OUT_BIN)
endif
ifneq "$(CP_ETC_FILE)" ""
	$(hide)-cp $(CP_ETC_FILE) $(OUT_ETC)
endif
ifneq "$(CP_CFG_FILE)" ""
	$(hide)-cp $(CP_CFG_FILE) $(OUT_CFG)
endif
clean:
	$(hide)echo "-----------clean----------"
	$(hide)rm -rf $(BIN)
	$(hide)rm -rf $(OBJ)
	$(hide)rm -rf $(LIB)
ifeq "$(BUILDTYPE)" "lib"
	rm -rf $(OUT_LIB)/$(TARGET)
else ifeq "$(BUILDTYPE)" "bin"
	rm -rf $(OUT_BIN)/$(TARGET)
endif
hello: ;$(hide) echo ==== start, $(shell date) ====
	$(hide)echo "parent path:$(pes_parent_dir),$(LOCAL_CFLAGS)"
	@echo "$(AI_AUDIO),$(AI_AEC),$(AI_CUS),$(AI_TEST)"
prepare:
	$(hide)mkdir -p $(OUT_LIB)
	$(hide)mkdir -p $(OUT_DATA)
	$(hide)mkdir -p $(OUT_BIN)
	$(hide)mkdir -p $(OUT_ETC)
	$(hide)mkdir -p $(OUT_CFG)	
ifeq "$(BUILDTYPE)" "lib"
	$(hide)mkdir -p $(LIB)
	$(hide)mkdir -p $(OBJ)
else ifeq "$(BUILDTYPE)" "bin"
	$(hide)mkdir -p $(BIN)
endif
	
post-build: ;$(hide) echo ==== done, $(shell date) ====
ifeq "$(BUILDTYPE)" "lib"
build: $(TARGET)
$(TARGET) : $(OBJECTS)
	$(info CXX $@)
	$(hide)$(CXX) $(OBJECTS) $(SHARED) $(LIB)/$@ $(LDFLAGS)
else ifeq "$(BUILDTYPE)" "bin"
LDFLAGS += -L$(pes_parent_dir)/build/linux-arm-default/bin -lgoahead
build: $(TARGET)
$(TARGET):
	$(CXX) $^ -o $(BIN)/$@ $(SOURCES) $(BIN_FLAGS) $(INCLUDES) $(LDFLAGS) -std=c++11 -O3	
endif

 
define make-cmd-cc
$2 : $1
	$(CXX) $(C_FLAGS) $(INCLUDES) -std=c++11 -O3 -MMD -MT $$@ -MF $$@.d -c -o $$@ $$<   
endef

$(foreach afile,$(SOURCES),\
    $(eval $(call make-cmd-cc,$(afile),\
        $(addsuffix .o,$(addprefix $(OBJ)/,$(basename $(notdir $(afile))))))))
