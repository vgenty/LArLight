#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = EventBuilderCint
SHLIB = libEventBuilder.so
SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.hh)
OBJECTS = $(SOURCES:.cc=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/Base
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/DataFormat
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/Analysis

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(DECODER_DIR)/Makefile/Makefile.${OSNAME}

# set compiler options for ROOT
CXXFLAGS +=  `root-config --cflags`

# call the common GNUmakefile
include $(DECODER_DIR)/Makefile/GNUmakefile.CORE



