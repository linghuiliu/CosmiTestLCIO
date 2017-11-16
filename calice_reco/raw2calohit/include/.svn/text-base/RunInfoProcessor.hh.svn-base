#ifndef _RunInfoProcessor_hpp
#define _RunInfoProcessor_hpp 1

#include <string>
#include <fstream>

// -- LCIO headers
#include "lcio.h"
#include "UTIL/LCTime.h"
#include "EVENT/LCRunHeader.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/EventModifier.h"

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"

// -- userlib headers
#include <collection_names.hh>
#include "RunInformation.hh"
#include "RunLocationWhizard.hh"

// seconds to nano seconds
#define NPS 1000000000LL


using namespace std;
using namespace lcio ;
using namespace lccd ;
using namespace marlin ;
using namespace CALICE;


namespace marlin {

  /** Preprocessor which is supposed to run at the !very! beginning of the processor chain. 
    * Adding information to the Run and EventHeader, for data and MC.  
    * In partcular the beam energy is extracted from the database for a given run
    * and the MC events get assigned a timestamp.
    * @author: A.M. Magnan Imperial College, modifs R.Poeschl LAL
    *
    * @date Apr  2007
    * @todo reduce even more the dependency on hardcoded, god given parameters in the code (valid for many processors)     
    * 
    */

class RunInfoProcessor : public Processor, public EventModifier {
  
 public:
  
  virtual Processor*  newProcessor() { return new RunInfoProcessor ; }
  
  
  RunInfoProcessor() ;

  ~RunInfoProcessor();
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  /** Needed for modifyEvent*/
  virtual const std::string & name() const { return Processor::name(); }
  virtual void modifyEvent( LCEvent* evt );
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;

  /** Method to write the histograms in a ROOT file.
   */
  /* virtual void SaveHistograms();*/

  void printRunParameters(const LCRunHeader *run);

 private:

  /** dbinit string, note that in this Processor we cannot make use of the ConditionsDataProcessor Mechanism as 
      it may run also on MC where the event time has to be set by this processor*/
  std::string _dbInit;
  /** The variable holding the folder name from which we fetch the runinfo */
  std::string _folderRunTime;
  /** The folder which holds the information on the beam parameter */
  std::string _folderBeamParameter; 
  /** Tolerance in % allowed until current reading at run begin and run end is considered to be inconsinstent */
  float _currentTolerance;
  /** Tolerance in s allowed until reading of beam parameters is considered to be outdated */
  float _timeTolerance;
  /** The variable which holds the folder name for the BeamParameterException, i.e. to be queried in case of pbs. with 
   * automatic r/o of beam parameters. 
   */
  std::string _folderBeamParameterException;
  /** The variable which holds the name of the folder to look up the run location */
  std::string _folderLocation;
  /** The object which holds the information on the run location*/
  RunLocationWhizard *_runlocationwhizard;
  /**A string which contains the version of the converter and therefore of the db entry*/
  std::string _vers_string;
  /** The function which sets the version string*/
  void setVersString(std::string);
  /** Handling of beam parameters to extract energy
   *@todo extend scope of function and maybe turn into dedicated class
   */
  unsigned int handleBeamParameters(LCCDTimeStamp);
  /** A helper function which determines corrections to be applied when check for r/o time of (cern) database 
   *  of cern beam parameters
   *@todo remove hardcoded times
   */ 
  float timeCor();
  /**The length of the version string*/
  int _vers_length;

  /**Obvious variables */
  int _runNumber;
  unsigned int _beamEnergy;

  /** variables needed to convert current to energy*/
  int _bendingCurrentToCheck;
  float _currentToEnergy;
  float _toMeV;

  /** Variable which can be used to ensure validity of conditions data in case of MC*/
  int _safetyMargin;

  /** Obvious counter variables, needed? */
  //CAMM: just as sanity check by printing the processed number of events in the end() method
  int _nRun ;
  int _nEvt ;

  /** Method to get the beam energy from the database for a given time stamp (default)*/
  unsigned int getBeamEnergy(const LCCDTimeStamp);
  /** Method to get the beam energy from the database for a given time stamp (user defined settings)*/
  unsigned int getBeamEnergyUser(const LCCDTimeStamp);

  /** An object which assembles run information */
  RunInformation *_runInformation;
  /** The event time, created for MC, not in use for real data */
  UTIL::LCTime _eventTime;

  bool _overwriteRunNumber; /*<flag to overwrite existing run number*/

  /** Helper class to for the modification of the Run Header */
  class LCRunModifier : public IMPL::LCRunHeaderImpl {
  public:
    static void setRun(LCRunHeader *r, int n, std::string description) {
      LCRunModifier *m((LCRunModifier*)r);
      m->setReadOnly(false);
      m->setRunNumber(n);
      m->setDescription(description);
      m->setReadOnly(true);
    }
  };

} ;

}
#endif



