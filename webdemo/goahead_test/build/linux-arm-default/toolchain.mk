CROSS_TOOL_PREFIX	:= arm-linux-gnueabihf-
STRIP				:= $(CROSS_TOOL_PREFIX)strip
CC					:= $(CROSS_TOOL_PREFIX)gcc
CXX					:= $(CROSS_TOOL_PREFIX)g++
OBJCOPY			:= $(CROSS_TOOL_PREFIX)objcopy
LD					:= $(CROSS_TOOL_PREFIX)ld
AR					:= $(CROSS_TOOL_PREFIX)ar