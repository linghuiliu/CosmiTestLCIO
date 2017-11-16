#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>
#include <stdexcept>

// #include "SimpleValue.hh"
// #include "HcalTileIndex.hh"

#include "TilesProduction.hh"

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "lccd.h"
#include "lccd/DBInterface.hh"

const std::string _defaultString = "none";

LCCollection* readFlatFile( const char* fname ){
  std::ifstream fin( fname );
  if (! fin.is_open() ){
    std::stringstream msg;
    msg << "Cannot open file " << fname;
    throw std::runtime_error( msg.str() );
  }

  IMPL::LCCollectionVec* col = 
    new IMPL::LCCollectionVec( LCIO::LCGENERICOBJECT );

  std::string line;
  int n, s, m, a, c;
  while ( fin.good() ){
    getline( fin, line );
    if ( line == "" || line.at(0) == '#' ) continue;
    std::stringstream ln( line );
    ln >> s >> n >> m >> a >> c;
    CALICE::TilesProduction *tp = new CALICE::TilesProduction( s, n, m, a, c );
    col->addElement( tp );
  }

  return col;
}

int printHelp( int ret ){
  std::cout << "Program writeSimpleValues" << std::endl
	    << "--inFile <file>        input flat file" << std::endl
	    << "--outFile <file>       output LCIO file" << std::endl
	    << "--colName <file>       name of LCCollection in file" << std::endl
	    << "--dbinit <string>      database init string" << std::endl
	    << "--folder <string>      database folder" << std::endl
	    << "--description <string> description stored in DB" << std::endl
	    << "--from <string>        start of validity time" << std::endl
	    << "--until <string>       end of validity time" << std::endl
	    << "--write                dry test run otherwise" << std::endl
	    << "--help                 print this text" << std::endl;
  return ret;
}

lccd::LCCDTimeStamp interpreteTimeString( std::string str ){
  lccd::LCCDTimeStamp time;
  if ( str == "past" )
    time = lccd::LCCDMinusInf;
  else if ( str == "future" )
    time = lccd::LCCDPlusInf;
  else{
    int Y, M, D, h, m, s;
    for ( std::string::iterator iter = str.begin(); iter != str.end(); ++iter)
      if ( *iter == '-' || *iter == '_' || *iter == '/' || *iter == ':' )
	*iter = ' ';
    std::stringstream sstr( str );
    sstr >> Y >> M >> D >> h >> m >> s;
    UTIL::LCTime tm( Y, M, D, h, m, s );
    time = tm.timeStamp();
  }
  return time;
}

void screenDump( LCCollection* col ){
  for ( int i = 0; i != col->getNumberOfElements(); ++i ){
    CALICE::TilesProduction* tp = static_cast<CALICE::TilesProduction*>( col->getElementAt( i ) );
    std::cout << tp->getSIPMID() << " "
	      << tp->getTileSize() << " "
	      << tp->getModule() << " "
	      << tp->getChip() << " "
	      << tp->getChan() << std::endl;
  }
}

void writeSimpleFile( LCCollection* col, 
		      std::string fname, std::string cname ){
  IO::LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;
  lcWrt->open( fname, LCIO::WRITE_NEW );
  IMPL::LCEventImpl* evt = new LCEventImpl();
  evt->addCollection( col, cname );
  lcWrt->writeEvent( evt );
  lcWrt->flush();
  lcWrt->close();
}

void writeDBFile( LCCollection* col, std::string fname, std::string cname,
		  std::string fromString, std::string untilString ){
}

void writeDB( LCCollection* col, 
	      std::string dbinit, 
	      std::string folder, 
	      std::string fromString,
	      std::string untilString,
	      std::string description ){
  lccd::LCCDTimeStamp from = interpreteTimeString( fromString );
  lccd::LCCDTimeStamp until = interpreteTimeString( untilString );
  lccd::DBInterface db( dbinit, folder, true );
  if ( until != lccd::LCCDPlusInf ) until += 1;
  db.storeCollection( from, until, col, description );
}

int main( int argc, char** argv ){
  bool write = false;
  bool writedb = false;
  bool writefile = false;
  std::string inName = _defaultString;
  std::string colName = "Production_DB";
  std::string dbinit = _defaultString;
  std::string outName = _defaultString;
  std::string folder = "/cd_calice/Hcal/Production_DB";
  std::string fromString = "past";
  std::string untilString = "future";
  std::string description = "Hcal production table; relates SiPM, tilesize, and mod/chip/chan";

  for ( int i = 1; i != argc; ++i ){
    if ( strcmp( argv[i],"--help" ) == 0 ) 
      return printHelp( 0 );
    else if ( strcmp( argv[i],"--inFile" ) == 0 && i+1<argc ) 
      inName = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--colName" ) == 0 && i+1<argc ) 
      colName = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--outFile" ) == 0 && i+1<argc ) 
      outName = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--dbinit" ) == 0 && i+1<argc ) 
      dbinit = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--folder" ) == 0 && i+1<argc ) 
      folder = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--description" ) == 0 && i+1<argc ) 
      description = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--from" ) == 0 && i+1<argc ) 
      fromString = std::string( argv[++i] );
    else if ( strcmp( argv[i],"--until" ) == 0 && i+1<argc ) 
      untilString = std::string( argv[++i] );
    else if ( strcmp( argv[i], "--write" ) == 0 )
      write = true;
  }

  bool error = false;
  if ( inName == _defaultString ){
    std::cerr << "Mandatory parameter --inFile <file> missing" << std::endl;
    error = true;
  }
  if ( dbinit != _defaultString && folder != _defaultString )
    writedb = true;
  else if ( dbinit != _defaultString || folder != _defaultString )
    std::cerr << "Need both --dbinit and --folder specified in order to write to DB" << std::endl;

  if ( outName != _defaultString )
    writefile = true;

  if ( write && !writefile && !writedb ){
    std::cerr << "--write requested, but neither --outFile or --dbinit plus --folder specified" << std::endl;
    error = true;
  }

  if ( error ) return 1;

  std::cout << "Here are the options: " << std::endl
	    << "inFile      " << inName << std::endl
	    << "outFile     " << outName << std::endl
	    << "colName     " << colName << std::endl
	    << "dbinit      " << dbinit << std::endl
	    << "folder      " << folder << std::endl
	    << "description " << description << std::endl
	    << "from        " << fromString << std::endl
	    << "until       " << untilString << std::endl
    //	    << "will write File "
	    << std::endl;

  LCCollection* col = readFlatFile( inName.c_str() );
  if ( write ){
    if ( writefile ){
      if ( fromString != "past" || untilString != "future" ){
	writeDBFile( col, outName, colName, fromString, untilString );
      } else {
	writeSimpleFile( col, outName, colName );
      }
    }
    if (writedb ){
      writeDB( col, dbinit, folder, 
	       fromString, untilString, description );
    }
  } else {
    screenDump( col );
  }
  
  return 0;
}
