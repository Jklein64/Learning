RACK_DIR ?= ../..

FLAGS += -Idep/vcv-svghelper/include

SOURCES += src/plugin.cpp
SOURCES += src/Blank.cpp
SOURCES += src/Pentagram.cpp

DISTRIBUTABLES += res
# DISTRIBUTABLES += presets
# DISTRIBUTABLES += selections

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk