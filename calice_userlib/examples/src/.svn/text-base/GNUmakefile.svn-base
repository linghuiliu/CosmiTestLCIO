######################################################
#
# Example GNUmakefile to build an Application with Marlin
#
# @author Frank Gaede
# @date   Dec, 02 2004
######################################################

# requires pathes to MARLIN and LCIO  !


#---------- user variables ---------------------------------------------------

# name of the program
PROGNAME = Dataproc


# the base directory of your application
# this is where you  would normaly have the lib, bin and source directories
BASEDIR = ..
INSTALL = $(BASEDIR)/lib
BINDIR = $(BASEDIR)/bin


#---------- user variables ---------------------------------------------------

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

#INCLUDE =  -I $(LCIO)/src/cpp/include
INCLUDE  =  -I $(BASEDIR)/include
INCLUDE +=  -I $(CALUSER)/include
#------ add additional include files here
#USERINCLUDES = ...
#--------------------------------------------------------------------------------------


CPPFLAGS += -g -Wall -ansi
CPPFLAGS += -D TRIGGER_HANDLER_IS_SINGLETON -D DEPRECATED $(INCLUDE)
CPPFLAGS += $(USERINCLUDES)
#LCCD and CONDDB should be included in marlin !!!!
CPPFLAGS += `. $(MARLIN)/bin/marlin_includes.sh`

#----- add libraries needed --------
LIBS +=   -Wl,-whole-archive  -L $(CALUSER)/lib  -luserlib -Wl,-no-whole-archive
#Needed in case everything was linked statically e.g. for debugging
#LIBS += -L /usr/lib/mysql -lmysqlclient
LIBS +=   -Wl,-whole-archive  -L $(BASEDIR)/lib  -l$(PROGNAME) -Wl,-no-whole-archive
#LCCD and CONDDB should be included in marlin !!!!
LIBS += `. $(MARLIN)/bin/marlin_libs.sh`
#------ add additional libraries with marlin processors here, e.g.
#USERLIBS =
#LIBS += $(USERLIBS)
#--------------------------------------------------------------------------------------


ifdef USE_ROOT
INCLUDE += -D HAVE_ROOT
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS  = $(shell root-config --glibs)
INCLUDE += $(ROOTCFLAGS)
LIBS += $(ROOTLIBS)
endif

bin:  lib $(BINDIR)/$(PROGNAME) 


$(BINDIR)/$(PROGNAME): $(INSTALL)/lib$(PROGNAME).a
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(PROGNAME) $(CPPFLAGS) $(MARLIN)/src/Marlin.cc $(LIBS) 

objects = $(patsubst %.cc,%.o, $(wildcard *.cc)  )

$(INSTALL)/lib$(PROGNAME).a: $(objects)
	mkdir -p $(INSTALL)
	$(AR) cr $(INSTALL)/lib$(PROGNAME).a  $?


lib: $(INSTALL)/lib$(PROGNAME).a


clean:
	rm -f *.o $(INSTALL)/lib$(PROGNAME).a $(BINDIR)/$(PROGNAME)

doc:
	cd $(BASEDIR)/doc ; doxygen ;

