
/* -- CondDB headers*/
#include "ConditionsDB/ICondDBMgr.h"
//#include "exampleObject.h"

/* -- LCIO headers*/
#include "lcio.h"
#include "IO/LCWriter.h"
#include "EVENT/LCGenericObject.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/LCTime.h"
#include "LCIOSTLTypes.h"

/* -- LCCD headers*/
#include "lccd.h"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

/*userlib headers*/
//#include "collection_names.hh" 
#include "ExperimentalSetup.hh"
#include "BmlSlowRunDataBlock.hh"

#include "RunTimeWhizard.hh"
#include "RunTimeInfo.hh"

#include "LCPayload.hh"
#include "FitConstants.hh"

#include <cstdlib>

using std::cout;
using std::endl;


/***************************************************************************************/
/*                                                                                     */
/*   printHelp                                                                         */
/*                                                                                     */
/***************************************************************************************/
void printHelp( int argc, char** argv ) 
{
  cout << "usage: " << argv[0] << " [options]" << endl << endl
       << "Options are: "<< std::endl
       << "  -i <string>   DBInit string (mandatory)" << endl
       << "  -f <int>      first run (mandatory)" << endl
       << "  -u <int>      last run (mandatory)" << endl
       << "  -l <string>   TB location (mandatory)" << endl
       << "  -n <string>   file name with the FitConstants (mandatory)" <<endl
       << "  -w            do write to DB" << endl;

  return;
}

/***************************************************************************************/
/*                                                                                     */
/*   main                                                                              */
/*                                                                                     */
/***************************************************************************************/
int main(int argc, char** argv )
{
unsigned int firstRun = 0;
  unsigned int lastRun = 0;

  const std::string def = "default";
  std::string dbinit    = def;
  std::string location  = def;
  std::string fileName  = def;

  bool writeToDB = false;

  char opt;
  while ( ( opt = getopt( argc, argv, "h?i:f:u:l:n:w" ) ) != -1 ) 
    {
      switch ( opt ) 
        {
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
        case 'n':
          fileName = std::string( optarg ); break;
        case 'w':
          writeToDB = true; break;
        default:
          printHelp( argc, argv ); return 1;
        }
    }
  
  if ( dbinit == def || location == def || firstRun == 0 || lastRun == 0 ) 
    {
      printHelp( argc, argv );
      return 1;
    }

  if (fileName == def)
    {
      cout<<" Sorry, no file named "<<fileName<<" found, exiting"<<endl;
      return 1;
    }

  cout<<"\n"<<endl;
  cout<<"First run: "<<firstRun<<endl;
  cout<<"Last  run: "<<lastRun<<endl;
  cout<<"Location : "<<location<<endl;

  std::string folder = "";
  if (location == "Desy" || location == "DESY")
    {
      folder = "/cd_calice_beam/";
    }
  else if (location == "Cern" || location == "CERN")
    {
      folder = "/cd_calice_cernbeam/";
    }
  else if (location == "Fnal" || location == "FNAL")
    {
      folder = "/cd_calice_fnalbeam/";
    }
  else
    {
      cout<<"... Sorry, unknown location, aborting..."<<endl;
      return 1;
      //folder = "/test_al/";
    }


  std::string dbColName = "TBTrack/FitConstants";
  std::string dbFolder = folder + dbColName;   

  cout<<"DB folder: "<<dbFolder<<endl; 

  std::ifstream fin(fileName.c_str());
  assert(fin);
 
  TBTrack::FitConstants tcData;
  LCCollectionVec* fitColData = new LCCollectionVec( LCIO::LCGENERICOBJECT );
  LCPayload<TBTrack::FitConstants> *qData = new LCPayload<TBTrack::FitConstants>(tcData);
  qData->update(fileName);
  qData->payload().print();

  fitColData->addElement(dynamic_cast<LCGenericObject*>(qData)) ;


if (writeToDB)
    {
      try
        {
          std::cout<<" dbinit: "<<dbinit<<std::endl;
          std::cout<<" dbfolder: "<<dbFolder<<std::endl;
          lccd::DBInterface *dbInterface = new lccd::DBInterface( dbinit, dbFolder, true );
          std::cout<<" ---> Database has been accessed, now writing the new collection "
                   <<" between run "<<firstRun
                   <<" and run "<<lastRun
                   <<std::endl;

          /*No idea why this does not work:
          dbInterface->storeCollection( (lccd::LCCDTimeStamp) firstRun,
          (lccd::LCCDTimeStamp) lastRun,
          alnColData, "AlnConstants");
          */
          CALICE::RunTimeWhizard runTimeWhizard("/cd_calice/CALDAQ_RunTimeInfo", dbinit);
          LCTime startTime = runTimeWhizard.getRunStartTime(firstRun);
          LCTime stopTime  = runTimeWhizard.getRunStopTime(lastRun);
          
          dbInterface->storeCollection(lccd::LCCDTimeStamp(startTime.timeStamp()),
                                       lccd::LCCDTimeStamp(stopTime.timeStamp()),
                                       fitColData, "FitConstants");

          std::cout<<" Collection successfully written"<<std::endl;
        }
      catch( ... )


       {
          std::cerr << "Problems opening DB or storing information" << std::endl;
        }
    }/*--------- end if writeToDB ------------*/
  
 
 return 0;

}
