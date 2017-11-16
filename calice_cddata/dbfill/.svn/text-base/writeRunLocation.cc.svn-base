#include <string>
extern "C" {
#include <unistd.h>
}

#include <cstdlib>

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"

#include "lccd.h"
#include "lccd/DBInterface.hh"

#include "RunLocation.hh"

void printHelp( int argc, char** argv ) {
  std::cout << "usage: " << argv[0] << " [options]" << std::endl << std::endl
	    << "Options are: "<< std::endl
	    << "  -i <string>   DBInit string (mandatory)" << std::endl
	    << "  -f <int>      first run (mandatory)" << std::endl
	    << "  -u <int>      last run (mandatory)" << std::endl
	    << "  -l <string>   TB location (mandatory)" << std::endl
	    << "  -t <string>   run type (mandatory)" << std::endl
	    << "  -m <string>   month/year (mandatory)" << std::endl
	    << "  -w            do write to DB" << std::endl;
  return;
}

int main( int argc, char** argv ) {

  unsigned int firstRun = 0;
  unsigned int lastRun = 0;

  const std::string def = "def";

  std::string dbinit = def;
  std::string folder = "/cd_calice/RunLocation";
  std::string location = def;
  std::string type = def;
  std::string month = def;

  bool write = false;

  char opt;
  while ( ( opt = getopt( argc, argv, "h?i:f:u:l:t:m:w" ) ) != -1 ) {
    switch ( opt ) {
    case 'h':
    case '?':
      printHelp( argc, argv ); return 0;
    case 'i':
      dbinit = std::string( optarg ); break;
    case 'f':
      firstRun = atoi( optarg ); break;
    case 'u':
      lastRun = atoi( optarg ); break;
    case 'l':
      location = std::string( optarg ); break;
    case 't':
      type = std::string( optarg ); break;
    case 'm':
      month = std::string( optarg ); break;
    case 'w':
      write = true; break;
    default:
      printHelp( argc, argv ); return 1;
    }
  }
  
  if ( dbinit == def || location == def || type == def || month == def || firstRun == 0 || lastRun == 0 ) {
    printHelp( argc, argv );
    return 1;
  }
  
  CALICE::RunLocation *l = new CALICE::RunLocation();
  l->setRunLocationParameters( location, type, month );
  
  std::cout << "DBInit    : " << dbinit << std::endl
	    << "folder    : " << folder << std::endl
	    << "first Run : " << firstRun << std::endl
	    << "last Run  : " << lastRun << std::endl;
  l->print( std::cout );
  
  if ( write ){
    IMPL::LCCollectionVec *vec = 
      new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );
    vec->addElement( l );
    std::cout << "created LCCollectionVec with " 
	      << vec->getNumberOfElements() << " element(s)" << std::endl;
    
    try {
      lccd::DBInterface* dbi = new lccd::DBInterface( dbinit, folder, write );
      std::cout << "successfully opened DB" << std::endl;
      dbi->storeCollection( (lccd::LCCDTimeStamp) firstRun, 
			    (lccd::LCCDTimeStamp) lastRun, 
			    vec, "RunLocation" );
    } catch ( ... ) {
      std::cerr << "Problems opening DB or storing information" << std::endl;
    }

  }
}
