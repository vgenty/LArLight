#
# This is an example GNUmakefile for CMToolApp package
#

# specific names for this package
DICT  = CMToolAppCint
SHLIB = libCMToolApp.so
SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.hh)
OBJECTS = $(SOURCES:.cc=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/Base
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/DataFormat
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/LArUtil
INCFLAGS += -I$(LARLIGHT_CORE_DIR)/Analysis
INCFLAGS += -I$(USER_DEV_DIR)/ClusterRecoUtil
INCFLAGS += -I$(USER_DEV_DIR)/FANN
INCFLAGS += -I$(USER_DEV_DIR)/FANN/include
INCFLAGS += -I$(USER_DEV_DIR)/McshowerLookback
INCFLAGS += -I$(USER_DEV_DIR)/CMTool/CMToolBase
INCFLAGS += -I$(USER_DEV_DIR)/CMTool/CMTAlgPriority
INCFLAGS += -I$(USER_DEV_DIR)/CMTool/CMTAlgMerge
INCFLAGS += -I$(USER_DEV_DIR)/CMTool/CMTAlgMatch
# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)


CINTFLAGS += -p
include $(MAKE_TOP_DIR)/Makefile/Makefile.${OSNAME}

# set compiler options for ROOT
CXXFLAGS +=  `root-config --cflags`

# call the common GNUmakefile
include $(MAKE_TOP_DIR)/Makefile/GNUmakefile.CORE



