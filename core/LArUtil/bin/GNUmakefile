# Include your header file location
CXXFLAGS += -I$(LARLIGHT_CORE_DIR)/Base
CXXFLAGS += -I$(LARLIGHT_CORE_DIR)/DataFormat
CXXFLAGS += -I$(LARLIGHT_CORE_DIR)/LArUtil
CXXFLAGS += -I. $(shell root-config --cflags) -g

# Include your shared object lib location
LDFLAGS += -L$(LARLIGHT_LIBDIR) -lBase -lLArUtil
LDFLAGS += $(shell root-config --libs) -lPhysics -lMatrix -g

# platform-specific options
OSNAME = $(shell uname -s)
include $(MAKE_TOP_DIR)/Makefile/Makefile.${OSNAME}

# Add your program below with a space after the previous one.
# This makefile compiles all binaries specified below.
PROGRAMS = print_constants gen_tree_Geometry gen_tree_DetectorProperties gen_tree_LArProperties

all:		$(PROGRAMS)

$(PROGRAMS): *.cc
	@echo '<<compiling' $@'>>'
	@$(CXX) $@.cc -o $@ $(CXXFLAGS) $(LDFLAGS)
	@rm -rf *.dSYM
clean:	
	rm -f $(PROGRAMS)
