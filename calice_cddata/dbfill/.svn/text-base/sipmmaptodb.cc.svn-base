#include <stdexcept>
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


#include "SiPMMappingHcal.hh"
#include "time.h"

#define MAPDEB 1

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
    std::cout << " usage: sipmmaptodb <file> <description> <since> <till> [<dbinit>]" << std::endl ;
    std::cout << " Give <since> <till> in the form YYYY:MM:DD:HH:MM:SS" << std::endl;
    exit(1) ;
  }


 string dbinit("localhost:condb_1:condb:condb"); 

 if(argc > 5) {
   dbinit = argv[5];
 }


  //read in the file
  string _filename(argv[1]);
  ifstream fin(_filename.c_str());

  //Initialize database
  string colNameDB("SiPMMapHcal");
  string folder( "/cd_calice/" + colNameDB ) ;

  //The LCIO collection which will be written to the db
  LCCollectionVec* sipm_col = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;

  //The variables which will contain the parameters
  int SIPM, cassette, icoord, jcoord; 
  string remark, dummy;
  //A string vector in which the remarks are stored
  std::vector<string> remarksVec; 
  //An (help) int vector needed to later on relate the remarks to the 
  //SiPM 
  std::vector<int> sipmVec; 
 //The first lines
  getline( fin , dummy ) ;
  getline( fin , dummy ) ;
  getline( fin , dummy ) ;

  //..characters to be removed from the remark string
  char const* delims = " |";

  //parse the file  
  while(true){ 
    fin >> dummy >> SIPM >> dummy >>  cassette >> dummy >> icoord >> dummy >>
      jcoord >> dummy ;
  
    //create a new sipm mapping object
    SiPMMappingHcal* sipmMap = new SiPMMappingHcal(SIPM, cassette,
                                                     icoord,
                                                     jcoord,
                                                     0) ;

    //add it to the collection
    sipm_col->addElement( sipmMap ) ;
    getline( fin , remark ) ;
    if (!fin.good()) break;  
     string::size_type notwhite = remark.find_last_not_of(delims);    
     remark.erase(notwhite+1);
#ifdef MAPDEB
    cout << SIPM << ", " << cassette << ", " << icoord << ", " << jcoord << ", "
    << remark << endl;
#endif
     //Store the remark
     remarksVec.push_back(remark);
     sipmVec.push_back(SIPM);

  } 

  sipm_col->parameters().setValues("Remark", remarksVec);
  sipm_col->parameters().setValues("SIPM", sipmVec);



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

  //bring the time stamps into the correct format
  LCTime begin( sincevec.at(YEAR),sincevec.at(MONTH), sincevec.at(DAY), sincevec.at(HOUR), sincevec.at(MINUTE), sincevec.at(SECONDS)) ;
  lccd::LCCDTimeStamp since   = begin.timeStamp();
  LCTime farFuture( tillvec.at(YEAR), tillvec.at(MONTH), tillvec.at(DAY),  tillvec.at(HOUR), tillvec.at(MINUTE) , tillvec.at(SECONDS) ) ;
  lccd::LCCDTimeStamp till   = farFuture.timeStamp();
  



  //Open the database for writing
  lccd::DBInterface db( dbinit , folder , true ) ;

  //..and finally store the collection
  string description(argv[2]);
  db.storeCollection(since, till, sipm_col, description);


}



