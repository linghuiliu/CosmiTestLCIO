#include "EVENT/LCGenericObject.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
#include "lccd/DBInterface.hh"

#include "RunTimeWhizard.hh"
#include "LCPayload.hh"
#include "SimConstants.hh"

#include <cstdlib>
#include <unistd.h>

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
       << "  -n <string>   file name with the AlnConstants (mandatory)" <<endl
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
  else if (location =="test")
    {
      folder = "/test_al/TBTrack/";
    }
  else
    {
      cout<<"... Sorry, unknown location, aborting..."<<endl;
      return 1;
    }

  std::string dbColName = "TBTrack/SimConstants"; 
  std::string dbFolder  = folder + dbColName;
  cout<<"DB folder: "<<dbFolder<<endl;

  TBTrack::SimConstants tcData;
  
  LCCollectionVec* simColData = new LCCollectionVec( LCIO::LCGENERICOBJECT );

  LCPayload<TBTrack::SimConstants> *qData = new LCPayload<TBTrack::SimConstants>(tcData);
  qData->update(fileName);
  qData->payload().print();

  simColData->addElement(dynamic_cast<LCGenericObject*>(qData)) ;

  if (writeToDB)
    {
      try
	{
	  lccd::DBInterface *dbInterface = new lccd::DBInterface( dbinit, dbFolder, true );
	  cout<<" ---> Database has been accessed, now writing the new collection "
		   <<" between run "<<firstRun
		   <<" and run "<<lastRun
	      <<endl;

	  CALICE::RunTimeWhizard runTimeWhizard("/cd_calice/CALDAQ_RunTimeInfo", dbinit);
	  LCTime startTime = runTimeWhizard.getRunStartTime(firstRun);
	  LCTime stopTime  = runTimeWhizard.getRunStopTime(lastRun);
	  
	  dbInterface->storeCollection(lccd::LCCDTimeStamp(startTime.timeStamp()),
				       lccd::LCCDTimeStamp(stopTime.timeStamp()),
				       simColData, "SimConstants");

	  std::cout<<" Collection successfully written"<<std::endl;
	}
      catch( ... )
	{
	  std::cerr << "Problems opening DB or storing information" << std::endl;
	}
    }/*--------- end if writeToDB ------------*/
  
  return 0;

}
