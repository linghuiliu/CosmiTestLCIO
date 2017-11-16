#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

// --- lcio headers 
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
// ------------------

// -- LCCD headers
#include "lccd.h"
#include "lccd/StreamerMgr.hh"
#include "lccd/VCollectionStreamer.hh"
#include "lccd/DBInterface.hh"


#include "CellMappingHcal.hh"
#include "time.h"

#define MAPDEB 1

//define the positions of the different
//cellid components in the vector created from
//each line of the input file
#define SLOT 0
#define FE 1
#define IADC 2
#define IMUL 3

#define ICOORD 4
#define JCOORD 5


#define TIMEVECSIZE 6
#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECONDS 5

using namespace std ;
using namespace lcio;
using namespace lccd;
using namespace CALICE;

/** Test program that writes the mapping of the hcal cells
 *  to the electronic channel to an LCIO file.<br>
 * 
 */

int main(int argc, char** argv ) {
  

if( argc < 5) {
    std::cout << " usage: hcalmaptodb <file> <description> <since> <till> [<dbinit>]" << std::endl ;
    std::cout << " Give <since> <till> in the form YYYY:MM:DD:HH:MM:SS" << std::endl;
    exit(1) ;
  }


 string dbinit("default"); 
 if(argc > 5) {
   dbinit = argv[5];
 }


  string filename(argv[1]);   
  ifstream fin(filename.c_str());

  //Initialize database
  string colNameDB("CellMapHcal");
  string folder( "/cd_calice/" + colNameDB ) ;

  
  LCCollectionVec* map_col = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;




  string CellMapString;
  std::vector<int> mapvec;

  //simple streamer to read the cell mapping
  while(!fin.eof()) {
    
    //if (fin.eof()) break ;
    getline( fin , CellMapString ) ;
    mapvec.clear();
    if ( CellMapString[0] != '#' && CellMapString.length() > 0) {
      string val_stream;
      for (unsigned i = 0; i <  CellMapString.length(); ++i) {    
	if (CellMapString[i] != ' ' && CellMapString[i] !='\n' ) {
	  val_stream += CellMapString[i];
	} 
	else { 
	  mapvec.push_back( atoi ( val_stream.c_str() ) );
	  val_stream = " ";
	}
      }
      
#ifdef MAPDEB
      std::cout << "After parsing " << std::endl;
      std::cout << "Slot: " << mapvec.at(SLOT) << std::endl; 
      std::cout << "Front End: " << mapvec.at(FE) << std::endl; 
      std::cout << "Multiplex: " << mapvec.at(IMUL) << std::endl; 
      std::cout << "ADC No.: " << mapvec.at(IADC) << std::endl; 
      std::cout << "Index i: " << mapvec.at(ICOORD) << std::endl; 
      std::cout << "Index j: " << mapvec.at(JCOORD) << std::endl; 
      std::cout << "=============================================" << std::endl;
#endif

      CellMappingHcal* cellMap = new CellMappingHcal(172, mapvec.at(SLOT),
						     mapvec.at(FE),
						     mapvec.at(IMUL),
						     mapvec.at(IADC),
						     mapvec.at(ICOORD),
						     mapvec.at(JCOORD),
						     0) ;
      
#ifdef MAPDEB
      cellMap->print(cout);
#endif
      map_col->addElement( cellMap ) ;

    }
  }



  //read the validity range
  //The since time
  string sinceStr(argv[3]);   
  std::vector<int> sincevec;
  string since_stream;
  for (unsigned i = 0; i < sinceStr.length(); ++i) {    
    if ( sinceStr[i] != ':' ) {

      if( !isdigit( sinceStr[i] ) ) {
	std::cout << "Error in reading 'since' value " << std::endl;
	std::cout << "Check format of 'since' value " << std::endl;
	std::cout << "Nothing will be written to db " << std::endl;
	std::cout << "Will leave the program now " << std::endl;
	exit(1);
      }
      
      since_stream += sinceStr[i];
    } 
    else { 
      sincevec.push_back( atoi ( since_stream.c_str() ) );
      since_stream = " ";
    }
    
    if (i == sinceStr.length()-1) {
      sincevec.push_back( atoi ( since_stream.c_str() ) );
    } 


  }

  if ( sincevec.size() < TIMEVECSIZE ) {
    std::cout << "Error in reading 'since' value " << std::endl;
    std::cout << "Don't have the desired six time parameters" << std::endl;
    std::cout << "Will leave the program now " << std::endl;
    exit(1);
  }  



  std::cout << "Since Year: " << sincevec.at(YEAR) << " ,"   
            << "Month: " << sincevec.at(MONTH) << " ,"  
            << "Day: " << sincevec.at(DAY) << " ,"  
            << "Hour: " << sincevec.at(HOUR) << " ,"  
            << "Minute: " << sincevec.at(MINUTE) << " ,"   
            << "Seconds: " << sincevec.at(SECONDS) << std::endl; 


  
  //The till time
  //FIXME: Avoid code duplication !!!!
  string tillStr(argv[4]);   
  std::vector<int> tillvec;
  string till_stream;
  for (unsigned i = 0; i < tillStr.length(); ++i) {    
    if ( tillStr[i] != ':' && tillStr[i] !='\0' ) {
      if( !isdigit( tillStr[i] ) ) {
	std::cout << "Error in reading 'till' value " << std::endl;
	std::cout << "Check format of 'till' value " << std::endl;
	std::cout << "Nothing will be written to db " << std::endl;
	std::cout << "Will leave the program now " << std::endl;
	exit(1);
      }
      
      till_stream += tillStr[i];
    } 
    else { 
      tillvec.push_back( atoi ( till_stream.c_str() ) );
      till_stream = " ";
    }
    
    if (i == tillStr.length()-1) {
      tillvec.push_back( atoi ( till_stream.c_str() ) );
    } 


  }
  
  if ( tillvec.size() < TIMEVECSIZE ) {
    std::cout << "Error in reading 'till' value " << std::endl;
    std::cout << "Don't have the desired six time parameters" << std::endl;
    std::cout << "Will leave the program now " << std::endl;
    exit(1);
  }  
  

  std::cout << "Till Year: " << tillvec.at(YEAR) << " ,"   
            << "Month: " << tillvec.at(MONTH) << " ,"  
            << "Day: " << tillvec.at(DAY) << " ,"  
            << "Hour: " << tillvec.at(HOUR) << " ,"  
            << "Minute: " << tillvec.at(MINUTE) << " ,"   
            << "Seconds: " << tillvec.at(SECONDS) << std::endl; 


  //Now start the storage into the db

  //bring the time staps into the correct format
  LCTime begin( sincevec.at(YEAR),sincevec.at(MONTH), sincevec.at(DAY), sincevec.at(HOUR), sincevec.at(MINUTE), sincevec.at(SECONDS)) ;
  lccd::LCCDTimeStamp since   = begin.timeStamp();
  LCTime farFuture( tillvec.at(YEAR), tillvec.at(MONTH), tillvec.at(DAY),  tillvec.at(HOUR), tillvec.at(MINUTE) , tillvec.at(SECONDS) ) ;
  lccd::LCCDTimeStamp till   = farFuture.timeStamp();
 
  //Open the database for writing
  lccd::DBInterface db( dbinit , folder , true ) ;

  //..and finally store the collection
  string description(argv[2]);
  db.storeCollection(since, till, map_col, description);


}



