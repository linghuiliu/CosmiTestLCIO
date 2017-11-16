#include "writeLCGenericObject.hh"
#include "EVENT/LCIO.h"

#ifdef USE_CONDDB
#include "ConditionsDB/CondDBException.h"
#endif

#include "lccd/DBInterface.hh"
#include "RunTimeInfo.hh"

#include <cstring>
#include <cstdlib>

using std::cout;
using std::endl;

/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
int writeLCGenericObject::mymain( int argc, char** argv )
{
  bool write = false;
  bool writedb = false;
  bool writefile = false;
  std::string inName = _defaultString;
  std::string colName = "COLLECTION_NAME_NOT_SET";
  std::string dbinit = _defaultString;
  std::string outName = _defaultString;
  std::string folder = _defaultString;
  std::string fromString = "past";
  std::string untilString = "future";
  bool isTimeStamp = false;
  std::string parameterFileName = _defaultString;

  bool useRunNumberAsTime = false;

  for ( int i = 1; i != argc; ++i )
    {
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
	_description = std::string( argv[++i] );
      else if ( strcmp( argv[i],"--from" ) == 0 && i+1<argc ) 
	fromString = std::string( argv[++i] );
      else if ( strcmp( argv[i],"--until" ) == 0 && i+1<argc ) 
	untilString = std::string( argv[++i] );
      else if ( strcmp( argv[i],"--isTimeStamp" ) == 0 && i+1<argc ) 
	{
	  if( strcmp( argv[++i],"true" ) == 0 ) 
	    {
	      isTimeStamp = true;
	    }
	}
      else if ( strcmp( argv[i],"--parameterFile" ) == 0 && i+1<argc ) 
	parameterFileName = std::string( argv[++i] );
      else if ( strcmp( argv[i],"--useRunNumberAsTime" ) == 0 && i+1<argc ) 
	useRunNumberAsTime = true;
      else if ( strcmp( argv[i], "--write" ) == 0 )
	write = true;
    }
  
  bool error = false;
  if ( inName == _defaultString )
    {
      std::cerr << "Mandatory parameter --inFile <file> missing" << endl;
      error = true;
    }
  if ( dbinit != _defaultString && folder != _defaultString )
    writedb = true;
  else if ( dbinit != _defaultString || folder != _defaultString )
    std::cerr << "Need both --dbinit and --folder specified in order to write to DB" << endl;

  if ( outName != _defaultString )
    writefile = true;

  if ( write && !writefile && !writedb )
    {
      std::cerr << "--write requested, but neither --outFile or --dbinit plus --folder specified" << endl;
      error = true;
    }


  if ( error ) 
    {
      return printHelp( 1 );
    }
  
  cout << "Here are the options: " << endl
       << "inFile      " << inName << endl
       << "outFile     " << outName << endl
       << "colName     " << colName << endl
       << "paramFile   " <<parameterFileName<<endl
       << "dbinit      " << dbinit << endl
       << "folder      " << folder << endl
       << "description " << _description << endl
       << "from        " << fromString << endl
       << "until       " << untilString << endl
       << "write       " << write << endl
       << endl;
  

   IMPL::LCCollectionVec* col = readFlatFile( inName.c_str() );
   if(parameterFileName != _defaultString) 
    {
      std::ifstream paramFile(parameterFileName.c_str());
      if(!paramFile) 
	{
	  cout<<"\n parameter file does not exist, aborting...\n\n "<<endl;
	  return 1;
	}
      else
	{
	  setCollectionParameters(col, parameterFileName);
	}
    }
 
  if ( write )
    {
      if ( writefile )
	{
	  writeSimpleFile( col, outName, colName );
	}
      if (writedb )
	{
	  if (useRunNumberAsTime == false)
	    writeDB( col, dbinit, folder, fromString, untilString, isTimeStamp, _description );
	  else
	    {
	      writeDBUsingRunNumberAsTime(col, dbinit, folder, fromString, untilString, _description);
	    }
	}
    } 
  else 
    {
      screenDump( col );
    }
  
  return 0;
}


/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
lccd::LCCDTimeStamp writeLCGenericObject::interpreteTimeString(const std::string& str, bool isTimeStamp) 
{
  lccd::LCCDTimeStamp time;

  if(isTimeStamp == true) 
    {
      time = atoll(str.c_str());
    } 
  else 
    {
      if ( str == "past" )
	time = lccd::LCCDMinusInf;
      else if ( str == "future" )
	time = lccd::LCCDPlusInf;
      else
	{
	  std::string istr( str );
	  int Y, M, D, h, m, s;
	  for ( std::string::iterator iter = istr.begin(); iter != istr.end(); ++iter)
	    if ( *iter == '-' || *iter == '_' || *iter == '/' || *iter == ':' )
          *iter = ' ';
	  std::stringstream sstr( istr );
	  sstr >> Y >> M >> D >> h >> m >> s;    
	  UTIL::LCTime tm( Y, M, D, h, m, s );
	  time = tm.timeStamp();
	}
    }
  return time;
}

/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
void writeLCGenericObject::writeDB( lcio::LCCollection* col, 
                                    std::string dbinit, 
                                    std::string folder, 
                                    std::string fromString,
                                    std::string untilString,
                                    bool isTimeStamp,
                                    std::string description )
{
  cout << "    writeDB start ---" << endl;
  lccd::LCCDTimeStamp from = interpreteTimeString( fromString, isTimeStamp );
  lccd::LCCDTimeStamp until = interpreteTimeString( untilString, isTimeStamp );
  cout <<"     Will write collection from "<< fromString << " until " << untilString << endl;
  
#ifdef USE_CONDDB
  try {
#endif
    lccd::DBInterface db( dbinit, folder, true );
    if ( until != lccd::LCCDPlusInf ) until += 1;
    db.storeCollection( from, until, col, description );
    cout << "   ----- writeDB end" << endl;
#ifdef USE_CONDDB
  }
  catch(CondDBException &error)
    {
      cout<<"\n writeLCGenericObject::writeDB: Could not create db interface for folder "<<folder<<endl;
    }
#endif
  
}

/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
void writeLCGenericObject::writeSimpleFile( lcio::LCCollection* col, 
                                            std::string fname, std::string cname )
{
  IO::LCWriter* lcWrt = lcio::LCFactory::getInstance()->createLCWriter() ;
  lcWrt->open( fname, lcio::LCIO::WRITE_NEW );
  lcio::LCEventImpl* evt = new lcio::LCEventImpl();
  evt->addCollection( col, cname );
  lcWrt->writeEvent( evt );
  lcWrt->flush();
  lcWrt->close();
}
/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
bool writeLCGenericObject::getTimes(std::string dbInit, const int runnumber, lcio::LCTime &from, lcio::LCTime &till )
{
  lccd::DBInterface dbTime(dbInit,_timeFolderName);
  
  lcio::LCCollection* timeCol = dbTime.findCollection( runnumber );
  if (!timeCol) 
    {
      cout << "ERROR: No time information found for run " << runnumber << endl;
      //exit(1);
      return false;
    }

  CALICE::RunTimeInfo runTimeInfo = CALICE::RunTimeInfo( timeCol->getElementAt( 0 ) );
  from = lcio::LCTime(runTimeInfo.getRunStartTime());
  till = lcio::LCTime(runTimeInfo.getRunStopTime());

  delete timeCol;
  
  return true;
}
/*******************************************************************************/
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/
void writeLCGenericObject::writeDBUsingRunNumberAsTime( lcio::LCCollection* col, 
							std::string dbinit, 
							std::string folder, 
							std::string fromString,
							std::string untilString,
							std::string description )
{
  cout << "    writeDB start ---" << endl;

  std::istringstream from(fromString);
  int firstRun;
  if ( (from >> firstRun).fail() )
    {
      cout<<"\n Could not convert "<<fromString<<" into a run number"<<endl;
      exit(1);
    }
  lcio::LCTime firstRunTimeStart, firstRunTimeEnd;
  this->getTimes(dbinit, firstRun, firstRunTimeStart, firstRunTimeEnd);
 
  std::istringstream until(untilString);
  int lastRun;
  if ( (until >> lastRun).fail() )
    {
      cout<<"\n Could not convert "<<untilString<<" into a run number"<<endl;
      exit(1);
    }
  lcio::LCTime lastRunTimeStart, lastRunTimeEnd;
  this->getTimes(dbinit, lastRun, lastRunTimeStart, lastRunTimeEnd);
 
  cout <<"     Will write collection from "<< firstRunTimeStart.getDateString() 
	    << " until " << lastRunTimeEnd.getDateString() << endl;
  
#ifdef USE_CONDDB
  try {
#endif
    lccd::DBInterface db( dbinit, folder, true );
    db.storeCollection( firstRunTimeStart.timeStamp(), lastRunTimeEnd.timeStamp(), col, description );
    cout << "   ----- writeDB end" << endl;
#ifdef USE_CONDDB
  }
  catch(CondDBException &error)
    {
      cout<<"\n writeLCGenericObject::writeDBUsingRunNumberAsTime:Could not create db interface for folder "<<folder<<endl;
    }
#endif
  
}
