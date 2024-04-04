#	Dirs
CWD := $(shell pwd)
export PRJDIR := $(CWD)
export PRJINCDIRS := ./Lib ../sml/include/
export PRJLIBS := -lm ../sml/lib/libsml.a # ../sml/lib/libsml.so.1
export TARGETDIR := $(PRJDIR)/Out
export OBJDIR := $(PRJDIR)/Obj

ifneq ($(shell uname), Darwin)
  PRJLIBS += -luuid
endif

###############################################################################
# RulesMakefike.mk
###############################################################################

# die Zieldatei (ohne Suffix)
TARGET = MySmlTest

# Defines für dieses Project
PRJ_DEFS	= 

# Common Directories
BOOSTDIR := #$(PRJDIR)/../../../../_Boost
SWCOMMONDIR := $(PRJDIR)/Lib

# Source Directories
SRCDIRS = $(PRJDIR) $(SWCOMMONDIR)

# Sourcefiles, die von vornherein ignoriert werden sollen
IGNORE_SOURCES = 
# zusätzliche Include-Directories
INCDIRS = $(PRJINCDIRS) $(PRJDIR) $(SWCOMMONDIR) $(BOOSTDIR) 
# zusätzliche Libraries
LDLIBS = $(PRJLIBS)

###############################################################################

INC_FILES = $(wildcard $(SRCDIRS:%=%/*.h)) $(wildcard $(INCDIRS:%=%/*.h))
C_SOURCES = $(realpath $(wildcard $(SRCDIRS:%=%/*.c)))
CPP_SOURCES = $(realpath $(wildcard $(SRCDIRS:%=%/*.cpp)))
C_BASENAMES = $(notdir $(basename $(C_SOURCES)))
CPP_BASENAMES = $(notdir $(basename $(CPP_SOURCES)))
OBJ_FILES = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(C_BASENAMES) $(CPP_BASENAMES)))
MAKEFILE := 

.SUFFIXES : .o .c
vpath %.c $(subst ,:,$(SRCDIRS))
vpath %.cpp $(subst ,:,$(SRCDIRS))

###############################################################################
# End of RulesMakefike.mk
###############################################################################

.EXTRA_PREREQS += $(foreach mk, ${MAKEFILE_LIST},$(abspath ${mk}))

CFLAGS += -ILib/ -I../sml/include/ -g -std=c99 -Wall -Wextra -pedantic
CXXFLAGS += -ILib/ -I../sml/include/ -Wall -Wextra -pedantic

#OBJ_FILES = SmlCommon.o SmlCrc16.o SmlByteReader.o SmlBuffer.o Decoder.o SmlServer.o

#LIBSML = ../sml/lib/libsml.so.1
#LIBSML = ../sml/lib/libsml.a

INSTALL = /usr/bin/install
prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

all: $(OBJDIR)/. $(TARGETDIR)/. $(TARGETDIR)/$(TARGET)

$(TARGETDIR)/$(TARGET) : $(OBJ_FILES) $(C_SOURCES)
	$(LINK.cc) $(OBJ_FILES) $(LDLIBS) -o $@
	#LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)

$(OBJDIR)/%.o: %.c $(INC_FILES)
	$(COMPILE.c) -o $@ $<
	#COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(OBJDIR)/%.o: %.cpp $(INC_FILES)
	$(COMPILE.cpp) -o $@ $<
	#COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

.PHONY: $(OBJDIR)/.$(TARGETDIR)/. clean install uninstall

$(OBJDIR)/.:
	mkdir $(OBJDIR)

$(TARGETDIR)/.:
	mkdir $(TARGETDIR)

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(TARGETDIR)
	@rm -f sml_server

install: sml_server
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) sml_server $(DESTDIR)$(bindir)

uninstall:
	@rm -f $(DESTDIR)$(bindir)/sml_server
