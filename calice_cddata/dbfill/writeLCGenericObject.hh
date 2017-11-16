#ifndef WRITE_LCGENERICOBJECT_HH
#define WRITE_LCGENERICOBJECT_HH

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "lccd.h"
#include "lccd/DBInterface.hh"

#include "IMPL/LCEventImpl.h" 

#include "collectionParameterHelper.hh"

const std::string _defaultString = "none";
/*string needed in the getTimes method, to get the time based on the run number*/
const std::string _timeFolderName = "/cd_calice/CALDAQ_RunTimeInfo";

class writeLCGenericObject {

public:

  writeLCGenericObject(std::string description) : _description(description) {}

  virtual ~writeLCGenericObject() {}

  int mymain(int argc, char** argv);

private:

  virtual lcio::LCCollectionVec* readFlatFile( const char* fname ) = 0;

  virtual int printHelp( int ret ) = 0;

  virtual void screenDump( lcio::LCCollection* col )= 0 ;

  lccd::LCCDTimeStamp interpreteTimeString( const std::string& str, 
                                            bool isTimeStamp );

  void writeSimpleFile( lcio::LCCollection* col, 
                        std::string fname, 
                        std::string cname );

  void writeDB( lcio::LCCollection* col, 
                std::string dbinit, 
                std::string folder, 
                std::string fromString,
                std::string untilString,
                bool isTimeStamp,
                std::string description );

  /**Angela Lucaci: This function is written by B. Lutz, 
     see calice_db_tools/writeElogInfo
     (should go into a class which can be used both by calice_cddata, and
     by calice_db_tools)
  */
  bool getTimes(std::string dbInit, const int runnumber, lcio::LCTime &from, lcio::LCTime &till );

  /**Angela Lucaci: Write into the data base using the run number to set the validity time
   */
  void writeDBUsingRunNumberAsTime( lcio::LCCollection* col, 
				    std::string dbinit, 
				    std::string folder, 
				    std::string fromString,
				    std::string untilString,
				    std::string description );
  

  std::string _description;
};

#endif
