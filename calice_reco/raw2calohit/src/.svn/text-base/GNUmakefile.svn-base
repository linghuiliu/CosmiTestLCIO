###########################################################
#
# Example GNUmakefile to build an Application with Marlin
#
# @author Frank Gaede
# @version $Id: GNUmakefile,v 1.9 2007-10-05 13:20:15 poeschl Exp $
#
###########################################################

# requires path to MARLIN and Marlin v00-09 or higher
# can be called from top level makefile


#----------------------------------------------------------------------------
#                user variables 
#----------------------------------------------------------------------------


# name of the program/library
PROGNAME = raw2calohit

#----  additional user include/libraries
#-- optionally can be specified in the to level GNUmakefile of $MARLIN

ifndef USERINCLUDES
# USERINCLUDES = ...your user includes
endif

ifndef USERLIBS
# USERLIBS = ... your user libs
endif



#----------------------------------------------------------------------------
#        do not modify anything below in order to use top level makefile
#----------------------------------------------------------------------------

# the base directory of your application
# this is where you  would normaly have the lib, bin and source directories
BASEDIR = ..
INSTALL = $(BASEDIR)/$(LIBDIR)
BINDIR = $(BASEDIR)/$(BINARYDIR)
DOCDIR = $(BASEDIR)/doc




ifdef DEBUG
 CPPFLAGS = -g -Wall -ansi 
else
 CPPFLAGS = -O3 -Wall -ansi
endif

ifdef X86_64
  CPPFLAGS += -m32
endif


# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++ 
endif

AR = ar


#--------------------------------------------------------------------------------------
INCLUDE += -I $(BASEDIR)/include -I $(BASEDIR)/include/histmgr -I $(BASEDIR)/include/utilities -I $(BASEDIR)/include/gui -I $(BASEDIR)/include/TBTrack -I $(BASEDIR)/include/TBTrackUtil
INCLUDE += -I $(CALUSER)/include 
INCLUDE += $(USERINCLUDES)
INCLUDE += -D WITH_CONTROL_HISTOGRAMS -D HAVE_ROOT
#INCLUDE += -D WITH_CONTROL_HISTOGRAMS 
#--------------------------------------------------------------------------------------

CPPFLAGS += $(INCLUDE)
CPPFLAGS += `. $(MARLIN)/bin/marlin_includes.sh`

#--------------------------------------------------------------------------------------
LIBS += -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive
LIBS += -Wl,-whole-archive -L $(CALUSER)/$(LIBDIR) -luserlib -Wl,-no-whole-archive
LIBS += $(USERLIBS)
LIBS += -L$(LAPACK_DIR) -llapack -lblas -L/usr/$(LIBDIR) -lg2c 
LIBS += `. $(MARLIN)/bin/marlin_libs.sh`
#--------------------------------------------------------------------------------------

#include Root Stuff
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDE += $(ROOTCFLAGS)
LIBS += $(ROOTLIBS)
ifdef ROOTSPECTRUM
ROOTLIBS += $(ROOTSPECTRUM) 
endif

bin:  lib $(BINDIR)/$(PROGNAME)

dependence = $(patsubst %.cc,%.d,$(wildcard *.cc))
dependence += $(patsubst %.cc,%.d,$(wildcard gui/*.cc))
dependence += $(patsubst %.cc,%.d,$(wildcard histmgr/*.cc))
dependence += $(patsubst %.cc,%.d,$(wildcard utilities/*.cc))
dependence += $(patsubst %.cc,%.d,$(wildcard TBTrackUtil/*.cc))

%.d: %.cc
	set -e; $(CC) -MM $(CPPFLAGS) $< | sed -e 's![[:lower:][:upper:][:digit:]]*\.o[ :] \([[:lower:][:upper:][:digit:]/]*\)[[:lower:][:upper:][:digit:]]*\.cc!\1.o \1.d : \1.cc!g' > $@; \
	[ -s $@ ] || rm -f $@

include $(dependence)

#Dependences via sed this works, too
#	set -e; $(CC) -MM $(CPPFLAGS) $< | sed -e 's![a-zA-Z0-9]*\.o[ :] \([a-zA-Z0-9/]*\)[a-zA-Z0-9]*\.cc!\1.o \1.d : \1.cc!g' > $@; \

$(BINDIR)/$(PROGNAME): $(INSTALL)/lib$(PROGNAME).a  $(CALUSER)/$(LIBDIR)/libuserlib.a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(MARLIN)/src/Marlin.cc $(LIBS) 

objects = $(patsubst %.cc,%.o, $(wildcard *.cc)  )
objects += $(patsubst %.cc,%.o, $(wildcard histmgr/*.cc)  )
objects += $(patsubst %.cc,%.o, $(wildcard utilities/*.cc)  )
objects += $(patsubst %.cc,%.o, $(wildcard gui/*.cc)  )
objects += $(patsubst %.cc,%.o, $(wildcard TBTrack/*.cc)  )
objects += $(patsubst %.cc,%.o, $(wildcard TBTrackUtil/*.cc)  )

$(INSTALL)/lib$(PROGNAME).a: $(objects) 
	mkdir -p $(INSTALL)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?

lib: $(INSTALL)/lib$(PROGNAME).a

clean:
	rm -f *.o *.d histmgr/*.o histmgr/*.d utilities/*.o utilities/*.d gui/*.o gui/*.d TBTrack/*.o TBTrack/*.d TBTrackUtil/*.o TBTrackUtil/*.d $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)

doc:
	cd $(DOCDIR) ; doxygen ;

