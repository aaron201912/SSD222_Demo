##############################parent floder
pes_parent_dir:=$(shell pwd)/$(lastword)
pes_parent_dir:=$(shell dirname $(pes_parent_dir))
#pes_parent_dir:=$(shell dirname $(pes_parent_dir))
#pes_parent_dir:=$(shell dirname $(pes_parent_dir))
##############################target define
#BUILDTYPE := lib
BUILDTYPE := bin

LIBNAME	:= libhttpserver
BINNAME	:= httpserver

SOURCES := $(wildcard ./*.cpp)

INCLUDES := -I$(pes_parent_dir)/work/include 

include $(pes_parent_dir)/build/linux-arm-default/rule.mk

all:
	cp -rf ./data/* $(pes_parent_dir)/out/bin
