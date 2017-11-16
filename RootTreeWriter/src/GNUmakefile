##################################################################
#
# Makefile for using Umbrella as a Marlin package, based on 
# example Makefile by Frank Gaede
#
# @author Frank Gaede
# @version $Id: GNUmakefile,v 1.1 2008/08/14 21:47:20 meyern Exp $
#
##################################################################

# requires pathes to MARLIN and LCIO  !
# can be called from top level makefile

#---------- user variables ---------------------------------------------------

# name of the program
PROGNAME = RootTreeWriter



#----  additional user include/libraries
#-- optionally can be specified in the to level GNUmakefile of $MARLIN

ifndef USERINCLUDES
  #USERINCLUDES = $(ROOTSYS)/include
  USERINCLUDES = `root-config --cflags`
endif

ifndef USERLIBS
  #USERLIBS = $(ROOTSYS)/lib
  USERLIBS = `root-config --libs`
endif

USERINCLUDES += -I$../LCIOTools/include
USERINCLUDES += -I$(LCIOTOOLSDIR)/include

#----------------------------------------------------------------------------
#        do not modify anything below in order to use top level makefile
#----------------------------------------------------------------------------

# the base directory of your application
# this is where you  would normaly have the lib, bin and source directories
BASEDIR = ..
INSTALL = $(BASEDIR)/lib
BINDIR = $(BASEDIR)/bin
BUILDDIR = ../build
#SRCDIR2 = LCIOTools

UMBRELLADIR = ../..

ifdef MARLINDEBUG
 CPPFLAGS = -g -Wall -ansi
else
 CPPFLAGS = -O3 -Wall -ansi
endif


# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

AR = ar

#--------------------------------------------------------------------------------------
LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive
#--------------------------------------------------------------------------------------

INCLUDE +=  -I $(BASEDIR)/include
INCLUDE +=  -I $(BASEDIR)/include/LCIOTools
INCLUDE += $(USERINCLUDES)
CPPFLAGS += $(INCLUDE)
CPPFLAGS += `. $(MARLIN)/bin/marlin_includes.sh`

CPPFLAGS += $(HSCPPFLAGS)

LIBS += $(USERLIBS)
LIBS += `. $(MARLIN)/bin/marlin_libs.sh`

bin:  lib $(BINDIR)/$(PROGNAME)

$(BINDIR)/$(PROGNAME): $(INSTALL)/lib$(PROGNAME).a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(MARLIN)/src/Marlin.cc $(LIBS) 

#objects = $(patsubst %.cc,%.o,$(filter-out $(BINS), $(wildcard *.cc))  )
sources= $(wildcard *.cc)
#sources+=$(wildcard $(SRCDIR2)/*.cc)
objects = $(patsubst %.cc,$(BUILDDIR)/%.o, $(sources)  )
dependence = $(patsubst %.cc,$(BUILDDIR)/%.d, $(sources))

BUILDDIRESC=$(subst /,\/,$(@D))



$(BUILDDIR)/%.o: %.cc
	@echo [$(CXX) ... $(OUTPUT_OPTION) ...]
	@$(COMPILE.cc) $(OUTPUT_OPTION) $<
$(BUILDDIR)/%.o: %.cpp
	@$(COMPILE.cc) $(OUTPUT_OPTION) $<


#include $(dependence)
-include $(dependence)


OSUBST=$(subst /,\/,$(BUILDDIR)/$*.o)
DSUBST=$(subst /,\/,$(BUILDDIR)/$*.d)
#SSUBST=$(subst /,\/,$*)
#Fixme! don't hardcode SRCDIR2 ?!?
SSUBST=$(subst $(SRCDIR2)\/,,$(subst /,\/,$*))

$(BUILDDIR)/%.d: %.cc
	@echo Building Dependencies: $@
	@mkdir -p $(@D)
	@set -e; $(CC) -MM $(CPPFLAGS) $< | sed -e 's/\($(SSUBST)\)\.o[ :]*/$(OSUBST) $(DSUBST) : /g' > $@; \
        [ -s $@ ] || rm -f $@





$(INSTALL)/lib$(PROGNAME).a: $(objects) 
	@echo [ar cr $@ ... ]
	@mkdir -p $(INSTALL)
	@$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?



lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f $(BUILDDIR)/*.d $(BUILDDIR)/*.o 
	rm -f $(BUILDDIR)/$(SRCDIR2)/*.d  $(BUILDDIR)/$(SRCDIR2)/*.o
	rm -f $(INSTALL)/lib$(PROGNAME).a 
	rm -f $(BINDIR)/$(PROGNAME)

doc:
	cd $(BASEDIR)/doc ; doxygen ;


