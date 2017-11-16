######################################################
#
# GNUmakefile to build the LCCD library test examples
#
## @author F. Gaede, DESY
## @version $Id: GNUmakefile,v 1.3 2007-08-27 19:27:43 lima Exp $
######################################################
LCCDBASE = $(LCCD)
CALICE_CDDATA_BASE = $(CALICE_CDDATA)

SIOINSTALL = $(LCCDBASE)/sio

# check for private compiler definition
ifdef MY_CXX
  CXX = $(MY_CXX)
else
  CXX = g++
endif

AR = ar

ifdef LCCDDEBUG
 LCCDCPPFLAGS = -g -Wall -ansi
else
 LCCDCPPFLAGS = -O3 -Wall -ansi
endif

# to set the same default as in ConditionsDBMySQL, do
# strings lib/libconddb.so | grep -E ".+:.+:.+:.+"
ifdef LCCD_DB_INIT
  LCCDCPPFLAGS += -D LCCD_DB_INIT_DEFAULT=\"$(LCCD_DB_INIT)\"
else
  LCCDCPPFLAGS += -D LCCD_DB_INIT_DEFAULT=\"localhost:lccd_test:calvin:hobbes\"
endif

ifdef CondDBMySQL
 LCCDCPPFLAGS += -D USE_CONDDB -I $(CondDBMySQL)/include
endif


INSTALL = $(LCCDBASE)/lib
BINDIR = ../bin

LCCDINCLUDE := -I $(LCCDBASE)/source/include -I $(LCIO)/src/cpp/include


CALICE_CDDATA_INCLUDE := -I $(CALICE_CDDATA_BASE)/include
USERINCLUDE := -I $(CALUSER)/include
CPPFLAGS = $(LCCDCPPFLAGS) $(LCCDINCLUDE) $(USERINCLUDE) $(CALICE_CDDATA_INCLUDE)
CPPFLAGS += -I $(LCIO)/src/cpp/include 

LIBS =   -L$(CALUSER)/lib -luserlib -L$(INSTALL) -llccd -L$(LCIO)/lib -llcio -L$(LCIO)/sio/lib -lsio -lz


##BINSRC = readcalfile.cc createcalfile.cc createmapfile.cc testdbfile.cc readdatafile.cc maptodb.cc triggermaptodb.cc
#BINSRC = createmapfile.cc 

ifdef CondDBMySQL
BINSRC += createdbfile.cc triggermaptodb.cc \
triggerfromdb.cc triggerchecktodb.cc hcalmaptodb.cc tagdbfolder.cc \
hcalmapfromdb.cc sipmmaptodb.cc sipmmapfromdb.cc tiles_iteptodb.cc \
hcalboards_conntodb.cc hcalboards_connfromdb.cc \
hcalcassvscrctodb.cc sipmvolcorrtodb.cc connmaptodb.cc \
dacvalcalc.cc \
createTcmtDescription.cc createTcmtLocation.cc createTcmtConnection.cc \
createDetectorTransformationSimple.cc createTcmtMIPCalibration.cc

LIBS += -L $(CondDBMySQL)/lib -lconddb
endif

BINS := $(patsubst %.cc,$(BINDIR)/%,$(BINSRC))


objects := $(patsubst %.cc,%.o,$(filter-out $(BINSRC), $(wildcard *.cc))  )


# --- check dependencies (from S.Aplin) 
dependence = $(patsubst %.cc,%.d,$(wildcard *.cc))

%.d: %.cc
	set -e; $(CC) -MM $(CPPFLAGS) $< | sed -e 's/\($*\)\.o[ :]*/\1.o $*.d : /g' > $@; \
	[ -s $@ ] || rm -f $@

include $(dependence)


bin: $(BINS)

clean:
	rm -f $(BINS) *.o *.d

ifdef CondDBMySQL

#$(BINDIR)/tagdbfolder: tagdbfolder.cc $(INSTALL)/liblccd.a $(objects)
#	$(CXX) -o $(BINDIR)/tagdbfolder $(CPPFLAGS) tagdbfolder.cc   $(objects)  $(LIBS) 


$(BINDIR)/createdbfile: createdbfile.cc $(INSTALL)/liblccd.a $(objects)
	$(CXX) -o $(BINDIR)/createdbfile $(CPPFLAGS) createdbfile.cc   $(objects)  $(LIBS) 


$(BINDIR)/hcalmaptodb: hcalmaptodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/hcalmaptodb $(CPPFLAGS) hcalmaptodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/hcalmapfromdb: hcalmapfromdb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/hcalmapfromdb $(CPPFLAGS) hcalmapfromdb.cc $(objects) $(LIBS) -L /lib -lrt

#$(BINDIR)/hcalvecfromdb: hcalvecfromdb.cc $(INSTALL)/liblccd.a $(objects)
#	mkdir -p $(BINDIR)
#	$(CXX) -o $(BINDIR)/hcalvecfromdb $(CPPFLAGS) hcalvecfromdb.cc $(objects) $(LIBS) -L /lib -lrt


$(BINDIR)/sipmmaptodb: sipmmaptodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/sipmmaptodb $(CPPFLAGS) sipmmaptodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/sipmmapfromdb: sipmmapfromdb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/sipmmapfromdb $(CPPFLAGS) sipmmapfromdb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/tiles_iteptodb: tiles_iteptodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/tiles_iteptodb $(CPPFLAGS) tiles_iteptodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/hcalboards_conntodb: hcalboards_conntodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/hcalboards_conntodb $(CPPFLAGS) hcalboards_conntodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/hcalboards_connfromdb: hcalboards_connfromdb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/hcalboards_connfromdb $(CPPFLAGS) hcalboards_connfromdb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/hcalcassvscrctodb: hcalcassvscrctodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/hcalcassvscrctodb $(CPPFLAGS) hcalcassvscrctodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/sipmvolcorrtodb: sipmvolcorrtodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/sipmvolcorrtodb $(CPPFLAGS) sipmvolcorrtodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/triggermaptodb: triggermaptodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/triggermaptodb $(CPPFLAGS) triggermaptodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/connmaptodb: connmaptodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/connmaptodb $(CPPFLAGS) connmaptodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/dacvalcalc: dacvalcalc.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/dacvalcalc $(CPPFLAGS) dacvalcalc.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/triggerfromdb: triggerfromdb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/triggerfromdb $(CPPFLAGS) triggerfromdb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/triggerchecktodb: triggerchecktodb.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/triggerchecktodb $(CPPFLAGS) triggerchecktodb.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/tagdbfolder: tagdbfolder.cc $(INSTALL)/liblccd.a $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/tagdbfolder $(CPPFLAGS) tagdbfolder.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/createTcmtMIPCalibration: createTcmtMIPCalibration.cc $(INSTALL)/liblccd.a $(objects)
	@mkdir -p $(BINDIR)
	@echo Building $@...
	@$(CXX) -o $(BINDIR)/createTcmtMIPCalibration $(CPPFLAGS) createTcmtMIPCalibration.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/createTcmtDescription: createTcmtDescription.cc $(INSTALL)/liblccd.a $(objects)
	@mkdir -p $(BINDIR)
	@echo Building $@...
	@$(CXX) -o $(BINDIR)/createTcmtDescription $(CPPFLAGS) createTcmtDescription.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/createTcmtLocation: createTcmtLocation.cc $(INSTALL)/liblccd.a $(objects)
	@mkdir -p $(BINDIR)
	@echo Building $@...
	@$(CXX) -o $(BINDIR)/createTcmtLocation $(CPPFLAGS) createTcmtLocation.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/createDetectorTransformationSimple: createDetectorTransformationSimple.cc $(INSTALL)/liblccd.a $(objects)
	@mkdir -p $(BINDIR)
	@echo Building $@...
	@$(CXX) -o $(BINDIR)/createDetectorTransformationSimple $(CPPFLAGS) createDetectorTransformationSimple.cc $(objects) $(LIBS) -L /lib -lrt

$(BINDIR)/createTcmtConnection: createTcmtConnection.cc $(INSTALL)/liblccd.a $(objects)
	@mkdir -p $(BINDIR)
	@echo Building $@...
	@$(CXX) -o $(BINDIR)/createTcmtConnection $(CPPFLAGS) createTcmtConnection.cc $(objects) $(LIBS) -L /lib -lrt

endif
