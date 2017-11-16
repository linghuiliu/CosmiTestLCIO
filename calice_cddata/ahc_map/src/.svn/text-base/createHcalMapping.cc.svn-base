//  @todo:  test db entries (run reco job?)
//  @todo:  change hard coded folders
//  @todo:  clean, tag, install on caliceserver
//  @todo:  liason with Beni

//#define USE_CONDDB

#include "AHCReader.hh"
#include "AHCHcalEntry.hh"

#include "DBInitString.hh"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCEventImpl.h"
#include "UTIL/LCTime.h"
#include "lccd.h"
#include "lccd/DBInterface.hh"

#include <cstdlib>

using namespace std;
using namespace lcio;
using namespace lccd;
using namespace CALICE;

/************************************************************/
/*                                                          */
/* @brief Interprete date/time string "YYYY-MM-DD hh:mm:ss" */
/*                                                          */
/************************************************************/
LCTime readTime(const string& timeString)
{
  if ( timeString == "past" ) return LCTime( LCCDMinusInf );
  if ( timeString == "future" ) return LCTime( LCCDPlusInf );
  if ( timeString == "now" )
    {
      time_t t;
      t = time( &t );
      return LCTime( (int)t );
    }

  int y = 1970, m = 1, d = 1, hr = 0, min = 0, sec = 0;
  char cdummy;
  stringstream timeStream(timeString.c_str());
  timeStream >> y >> cdummy >> m >> cdummy >> d >> cdummy >> hr >> cdummy >> min
	     >> cdummy >> sec;
  LCTime t(y,m,d,hr,min,sec);
  return t;
}

/************************************************************/
/*                                                          */
/*                                                          */
/*                                                          */
/************************************************************/
void collection2LCIOfile( string fname, string cname, LCCollection* col )
{
  cout << "Writing collection " << cname << " to file " << fname << endl;
  
  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;
  
  lcWrt->open(fname);
  LCEventImpl*  evt = new LCEventImpl();
  evt->addCollection( col, cname );
  lcWrt->writeEvent( evt );
  lcWrt->flush();
  lcWrt->close();
}

/************************************************************/
/*                                                          */
/*                                                          */
/*                                                          */
/************************************************************/
int main( int argc, char** argv )
{
  string dbinit         = DBInitString();
  string infile         = "";
  string validFrom      = "now";
  string validUntil     = "2009-12-31 23:59:59.99999999";
  string parentFolder   = "";
  string absorberType   = "";

  string folderMapping  = "";
  string folderLocation = "";

  bool simple = false;
  bool write = false;

  LCTime from = readTime( validFrom );
  LCTime until = readTime( validUntil );

  if ( argc < 2 ){
    cout << "  usage: " << argv[0] 
	 << " [--simple] [--write] [--from <from-timestamp>] [--until <until-timestamp>] "
	 <<" --pfolder <parent folder set in DB> --file <AHC.cfg>" 
	 <<" --absorber <Fe/W>" << endl;
    return 1;
  }

  /*  all options are interpreted here*/
  for ( int i=1; i<argc; i++ )
    {
      if ( strcmp( argv[i],"--from") == 0 && i+1 < argc )
	from = LCTime( atoi( argv[++i] ) );
      else if ( strcmp( argv[i],"--until") == 0 && i+1 < argc )
	until = LCTime( atoi( argv[++i] ) );
      else if ( strcmp( argv[i],"--file") == 0 && i+1 < argc )
	infile = string( argv[++i] );
      else if ( strcmp( argv[i],"--pfolder") == 0 && i+1 < argc )
	parentFolder = string( argv[++i] );
      else if ( strcmp( argv[i],"--absorber") == 0 && i+1 < argc )
	absorberType = string( argv[++i] );
      else if ( strcmp( argv[i],"--simple") == 0 )
	simple = true;
      else if ( strcmp( argv[i],"--write") == 0 )
	write = true;
      else
	cout << "ignoring unrecognized option " << argv[i] << endl;
    }
  
  /* create DB folder names*/
  folderMapping.append(parentFolder);
  folderMapping.append("HcalMapping");
  folderLocation.append(parentFolder);
  folderLocation.append("HcalModuleLocationReference");// New database folder for 2006-2011
  //folderLocation.append("HcalModuleLocation"); // Old database folder

  /* check for required parameters*/
  if ( infile == "" )
    {
      cout << "ERROR: input file not specified! " << endl;
      return 1;
    }
  if ( parentFolder == "" )
    {
      cout << "ERROR: DB parent folder not specified! " << endl;
      return 1;
    }

  if (absorberType == "")
    {
      cout<<" ERROR: absorber type not specified!"<<endl;
      return 1;
    }


  /* print parameters*/
  cout << " " << endl;
  cout << "input file:           " << infile << endl;
  cout << "valid from:           " << from.getDateString() << endl;
  cout << "valid until:          " << until.getDateString() << endl;
  cout << "DB parent folder set: " << parentFolder << endl;
  cout << "DB folder 1:          " << folderMapping << endl;
  cout << "DB folder 2:          " << folderLocation << endl;
  cout << "absorber type:        " << absorberType << endl;
  cout << "output to file:       " << simple << endl;
  cout << "write to DB:          " << write << endl;
  cout << " " << endl;

  AHCReader reader( infile.c_str() );

  LCCollectionVec* conCol = new LCCollectionVec( LCIO::LCGENERICOBJECT );
  LCCollectionVec* locCol = new LCCollectionVec( LCIO::LCGENERICOBJECT );

  for( AHCReader::iterator iter = reader.begin(); iter != reader.end(); ++iter )
    {
      AHCHcalEntry* ahcEntry = dynamic_cast< AHCHcalEntry* >(*iter);

      if (ahcEntry == NULL)
	{
	  cout<<" could not find AHCHcalEntry, continue to read the file"<<endl;
	  continue;
	}

      if ( ahcEntry )
	{
	  ahcEntry->setAbsorberType( absorberType );
	  conCol->addElement( new ModuleConnection( ahcEntry->getConnection( true ) ) );
	  conCol->addElement( new ModuleConnection( ahcEntry->getConnection( false ) ) );
	  locCol->addElement( new ModuleLocation( ahcEntry->getLocation( true ) ) );
	  locCol->addElement( new ModuleLocation( ahcEntry->getLocation( false ) ) );
	}
    }
  
  cout << "conCol has " << conCol->getNumberOfElements() << " elements" << endl;
  cout << "locCol has " << locCol->getNumberOfElements() << " elements" << endl;
  
  if ( simple )
    {
      collection2LCIOfile( "HcalMapping.slcio", "HcalMapping", conCol );
      collection2LCIOfile( "HcalModuleLocationReference.slcio", "HcalModuleLocationReference", locCol );// New database folder for 2006-2011
      //collection2LCIOfile( "HcalModuleLocation.slcio", "HcalModuleLocation", locCol ); // Old database folder
    }

  if ( write ) 
    {
      DBInterface conFld( dbinit, 
			  folderMapping, true );
      conFld.storeCollection( from.timeStamp(), until.timeStamp(), conCol, 
			      "Hcal cable mapping, written automatically" );
      DBInterface locFld( dbinit, 
			  folderLocation, true );
      locFld.storeCollection( from.timeStamp(), until.timeStamp(), locCol,
			      "Hcal module locations, written automatically" );
    }
  
  return 0;
}
