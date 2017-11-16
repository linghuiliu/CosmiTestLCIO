######################################################
#
# GNUmakefile to build calice userlib
#
# @author Roman Poeschl (based on example by Frank Gaede)
# @date   Jan 26 2005 (start of project)
######################################################

# name of the program

PROGNAME = userlib

BASEDIR = ..
INSTALL = $(BASEDIR)/$(LIBDIR)
DOCDIR  = $(BASEDIR)/doc

# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

AR = ar

INCLUDE = -D TRIGGER_HANDLER_IS_SINGLETON -D DEPRECATED 
INCLUDE += -I $(BASEDIR)/include

CPPFLAGS += $(INCLUDE)

ifdef X86_64
  CPPFLAGS += -m32
endif

ifdef DEBUG
 CPPFLAGS += -g -Wall -ansi
else
 CPPFLAGS += -O3 -Wall -ansi
endif


CPPFLAGS += `. $(MARLIN)/bin/marlin_includes.sh`




LIBS +=   -Wl,-whole-archive  -L $(INSTALL)  -l$(PROGNAME) -Wl,-no-whole-archive
LIBS +=   -Wl,--demangle
LIBS += `. $(MARLIN)/bin/marlin_libs.sh`


LDFLAGS += --demangle



lib: $(INSTALL)/lib$(PROGNAME).a

dependence = $(patsubst %.cc,%.d,$(wildcard *.cc))

%.d: %.cc
	set -e; $(CC) -MM $(CPPFLAGS) $< | sed -e 's![[:lower:][:upper:][:digit:]]*\.o[ :] \([[:lower:][:upper:][:digit:]/]*\)[[:lower:][:upper:][:digit:]]*\.cc!\1.o \1.d : \1.cc!g' > $@; \
        [ -s $@ ] || rm -f $@

include $(dependence)


objects = $(patsubst %.cc,%.o, $(wildcard *.cc))

$(INSTALL)/lib$(PROGNAME).a: $(objects)
	mkdir -p $(INSTALL)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?

clean:
	rm -f *.o *.d $(INSTALL)/lib$(PROGNAME).a 

doc:	
	cd $(DOCDIR) ; doxygen ;
