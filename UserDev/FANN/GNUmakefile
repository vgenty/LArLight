#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = FANNCint
SHLIB = libFANN.so
SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.hh)
OBJECTS = $(SOURCES:.cc=.o)

# include options for this package
INCFLAGS  = -I. 
INCFLAGS += -I$(USER_DEV_DIR)/FANN/include
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/LArUtil
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/Base
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/DataFormat
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/Analysis

# Add -p flag to CINT compilation
CINTFLAGS += -p

# linking for the fann packages:
LIBS += -Llib -lfann 

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(MAKE_TOP_DIR)/Makefile/Makefile.${OSNAME}

# set compiler options for ROOT
CXXFLAGS +=  `root-config --cflags`

# call the common GNUmakefile
include $(MAKE_TOP_DIR)/Makefile/GNUmakefile.CORE

pre_build: 
	@echo '<< building FANN-2.2.0-Source >>'
	@python $(USER_DEV_DIR)/FANN/bin/fann_build.py

pre_clean:
	@echo '<< cleaning FANN-2.2.0-Source >>'
	@python $(USER_DEV_DIR)/FANN/bin/fann_clean.py



