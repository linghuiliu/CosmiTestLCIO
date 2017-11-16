#ifndef TBTrackBaseProcessor_hpp
#define TBTrackBaseProcessor_hpp

#include <string>

#include "TFile.h"

#include "lcio.h"

#include "marlin/Processor.h"

#include "LCPayload.hh"
#include "MapConstants.hh"
#include "SimConstants.hh"
#include "AlnConstants.hh"
#include "FitConstants.hh"
#include "RunInformation.hh"


using namespace std;

class TBTrackBaseProcessor : public marlin::Processor {
  
 public:

  /* Not for an abstract class
  virtual marlin::Processor* newProcessor() {
    return new TBTrackBaseProcessor;
  }
  */

  TBTrackBaseProcessor(const std::string &n="TBTrackBaseProcessor");
  virtual ~TBTrackBaseProcessor();
  
  /** Called at the begin of the job before anything is read.
   */
  virtual void init();
  virtual void Init() = 0;
  
  /** Called for every run
   */
  virtual void processRunHeader(EVENT::LCRunHeader *run);
  virtual void ProcessRunHeader(EVENT::LCRunHeader *run) = 0;
  
  /** Called for every event
   */
  virtual void processEvent(EVENT::LCEvent *evt); 
  virtual void ProcessEvent(EVENT::LCEvent *evt) = 0;
  
  /** Called for every event for checking
   */
  virtual void check(EVENT::LCEvent *evt){};
  
  /** Called after data processing for clean up.
   */
  virtual void end();
  virtual void End() = 0;


 protected:

  /** Useful methods
   */
  virtual bool printLevel(int p, bool b=true) const;

  virtual void getEvtConstants(const EVENT::LCEvent *evt);

  virtual const EVENT::LCCollection* 
  getCollection(const EVENT::LCEvent *evt, 
		const std::string &name,
		const std::string &type,
		bool allowPrint=true) const;
  
  virtual bool
  addCollection(EVENT::LCEvent *evt, 
		EVENT::LCCollection *c,
		const std::string &name,
		bool allowPrint=true) const;

  void openHFile(const EVENT::LCRunHeader *run);
  void closeHFile();

  void getSimTrackerHits(EVENT::LCEvent *evt);


  /** The database information objects
   */
  TBTrack::MapConstants _mapConstants;
  bool _mapConstantsUpdated;
  bool _mapConstantsValid;

  TBTrack::SimConstants _simConstants;
  bool _simConstantsUpdated;
  bool _simConstantsValid;

  TBTrack::AlnConstants _alnConstants;
  bool _alnConstantsUpdated;
  bool _alnConstantsValid;

  TBTrack::FitConstants _fitConstants;
  bool _fitConstantsUpdated;
  bool _fitConstantsValid;

  RunInformation _runInformation;
  bool _runInformationUpdated;
  bool _runInformationValid;


  /** Various other items
   */
  std::string _tdcRawDataCollection;
  std::string _simTrackerHitCollection;
  std::string _tdcHitCollection;
  std::string _trackProjectionCollection;

  // to specify the momentum
  //   used by for simulation
  //   for real data momentum is normally taken from the database, 
  //    setting this parameter will overwrite
  float _beamMomentum;

  /** Allow for processing of files produced with multiple DC Collection (obsolete Mokka priot to Mokka6.3)
   */
  int _obsoleteMokkaCollections;  

  int _nRun;
  int _nEvt;
  int _iRun;
  int _iEvt;

  std::string _runString;

private:
  int _iCall;
  int _printLevel;
  int _printEventNumber;
  int _printEventLevel;

  TFile *_hFile;
};

#endif
