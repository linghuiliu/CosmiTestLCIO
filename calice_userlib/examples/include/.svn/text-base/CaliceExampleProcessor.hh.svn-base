/* Processor which reads in CALICE Testbeam Data */
#ifdef USE_LCCD

#ifndef CALICEEXAMPLEPROCESSOR_h
#define CALICEEXAMPLEPOCCESSOR_h 1

#ifdef HAVE_ROOT
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#endif



#include <string>
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h" 
#include "IMPL/LCTOOLS.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "UTIL/LCTime.h"
#include "LCIOTypes.h"
#include "marlin/Processor.h"

//LCCD
// -- LCCD headers
#include "lccd.h"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"
#include "lccd/IConditionsHandler.hh"
//My stuff
#include "CellMappingHcal.hh"


#include "TriggerHandlerCalice.hh"
#include <collection_names.hh>
#include "ConditionsChangeDelegator.hh"
#include "MultipleConditionsChangeDelegator.hh"


//class TH1F;
//class TFile;

using namespace lcio;
using namespace marlin;
using namespace lccd;
using namespace CALICE;

// create a conditions map with channel to cell id mapping
typedef lccd::ConditionsMap<int,CellMappingHcal> CellMap;

namespace marlin {

class CaliceExampleProcessor : public Processor {

public:
  virtual Processor* newProcessor() { return new CaliceExampleProcessor;}
  CaliceExampleProcessor();
  ~CaliceExampleProcessor();
  virtual void init();  
  virtual void processEvent( LCEvent * evt ) ;
  virtual void end() {
#ifdef HAVE_ROOT   
  _hfile->Write();
  _hfile->Close();
  //delete _hfile;
  // delete _hpre;
  //delete _hpost;
#endif
  };

private:
  std::string _description;
  std::string _channelMapCol;
  CellMap* _cellMapPtr;
  LCCollection* _adcCol;
  /**A handle method for ADCCollections */
  void HandleADCCollections(LCEvent*);


  /** The delegator which is notified when the Run Info Configuration changes */
  ConditionsChangeDelegator<CaliceExampleProcessor> _runInfoChanged;

  /** The method which is called in case the Run Info Configuration changes */
  void setRunInfoConfigCol(EVENT::LCCollection *runInfoConfCol) {
  _runInfoConfCol = runInfoConfCol;
  } 

  /** The Collection which contains the Run Info Configuration */
  LCCollection* _runInfoConfCol;



  /** The delegator which is notified when the BeTrgConfiguration changes */
  ConditionsChangeDelegator<CaliceExampleProcessor> _beTrgConfigurationChanged;

  /** The method which is called in case the BeTrgConfiguration changes */
  void setBeTrgConfigCol(EVENT::LCCollection *beTrgConfCol) {
  _beTrgConfCol = beTrgConfCol;
  } 

  /** The Collection which contains the BeTrgConfiguration */
  LCCollection* _beTrgConfCol;


  /** The delegator which is notified when the EmcFeConfiguration changes */
  ConditionsChangeDelegator<CaliceExampleProcessor> _emcFeConfigurationChanged;


  /** The method which is called in case the EmcFeConfiguration changes */
  void setEmcFeConfigCol(EVENT::LCCollection *feCol) {
  _emcFeConfCol = feCol;
  } 

  /** The Collection which contains the EmcFeConfiguration */
  LCCollection* _emcFeConfCol;


  /** The delegator which is notified when the AhcFeConfiguration changes */
  ConditionsChangeDelegator<CaliceExampleProcessor> _ahcFeConfigurationChanged;

  /** The method which is called in case the AhcFeConfiguration changes */
  void setAhcFeConfigCol(EVENT::LCCollection *feCol) {
  _ahcFeConfCol = feCol;
  } 

  /** The Collection which contains the FeConfiguration i.e. not related to SiW Ecal or AHcal) */
  LCCollection* _ahcFeConfCol;

  /** The delegator which is notified when the FeConfiguration changes */
  ConditionsChangeDelegator<CaliceExampleProcessor> _feConfigurationChanged;

  /** The method which is called in case the FeConfiguration changes */
  void setFeConfigCol(EVENT::LCCollection *feCol) {
  _feConfCol = feCol;
  } 

  /** The Collection which contains the FeConfiguration */
  LCCollection* _feConfCol;


  /** The delegator which is notified when the AhcVfeConfiguration changes */
  ConditionsChangeDelegator<CaliceExampleProcessor> _ahcVfeConfigurationChanged;

  /** The method which is called in case the AhcVfeConfiguration changes */
  void setAhcVfeConfigCol(EVENT::LCCollection *vfeCol) {
  _ahcVfeConfCol = vfeCol;
  } 

  /** The Collection which contains the AhcVfeConfiguration */
  LCCollection* _ahcVfeConfCol;



  /** The delegator which is notified when the AhcSroData change
    AhcSroData = StagePosition*/

  ConditionsChangeDelegator<CaliceExampleProcessor> _ahcSroDataChanged;

  /** The method which is called in case the AhcSroData change */
  void setAhcSroDataCol(EVENT::LCCollection *ahcSroDataCol) {
  _ahcSroDataCol = ahcSroDataCol;
  } 

  /** The Collection which contains the ahc Sro Data */
  LCCollection* _ahcSroDataCol;

  /** The delegator which is notified when the AhcSroModData change 
    AhcSroModData = Temps, Volts et al.*/
  ConditionsChangeDelegator<CaliceExampleProcessor> _ahcSroModDataChanged;

  /** The method which is called in case the AhcSroModData change */
  void setAhcSroModDataCol(EVENT::LCCollection *ahcSroModDataCol) {
  _ahcSroModDataCol = ahcSroModDataCol;
  } 

  /** The Collection which contains the AhcSroModData */
  LCCollection* _ahcSroModDataCol;


  /** The delegator which is notified when the SceSroModData change 
    SceSroModData = Temps, Volts et al.*/
  ConditionsChangeDelegator<CaliceExampleProcessor> _sceSroModDataChanged;

  /** The method which is called in case the SceSroModData change */
  void setSceSroModDataCol(EVENT::LCCollection *sceSroModDataCol) {
  _sceSroModDataCol = sceSroModDataCol;
  } 

  /** The Collection which contains the SceSroModData */
  LCCollection* _sceSroModDataCol;


  /** The delegator which is notified when the SceSroTempData change 
    SceSroModData = Temps et al.*/
  ConditionsChangeDelegator<CaliceExampleProcessor> _sceSroTempDataChanged;

  /** The method which is called in case the SceSroTempData change */
  void setSceSroTempDataCol(EVENT::LCCollection *sceSroTempDataCol) {
  _sceSroTempDataCol = sceSroTempDataCol;
  } 

  /** The Collection which contains the SceSroTempData */
  LCCollection* _sceSroTempDataCol;





  /** The delegator which is notified when the EmcStageData change */
  ConditionsChangeDelegator<CaliceExampleProcessor> _emcStgDataChanged;

  /** The method which is called in case the EmcStageData change */
  void setEmcStgDataCol(EVENT::LCCollection *emcStgDataCol) {
    _emcStgDataCol = emcStgDataCol;
  } 

  /** The Collection which contains the ahcSroData */
  LCCollection* _emcStgDataCol;


  /** The delegator which is notified when the CrcReadoutConfData change */
  ConditionsChangeDelegator<CaliceExampleProcessor> _roConfDataChanged;

  /** The method which is called in case the Crc ReadoutConfData change */
  void setRoConfDataCol(EVENT::LCCollection *roConfDataCol) {
    _roConfDataCol = roConfDataCol;
  } 

  /** The Collection which contains the CrcReadoutConfData */
  LCCollection* _roConfDataCol;


  /** The delegator which is notified when the (Trigger) TrgReadoutConfData change */
  ConditionsChangeDelegator<CaliceExampleProcessor> _trgroConfDataChanged;

  /** The method which is called in case the TrgReadoutconfData change */
  void setTrgroConfDataCol(EVENT::LCCollection *trgroConfDataCol) {
    _trgroConfDataCol = trgroConfDataCol;
  } 

  /** The Collection which contains the TrgReadoutConfData */
  LCCollection* _trgroConfDataCol;

  /** The delegator which is notified when the Bml Caen ConfData change */
  ConditionsChangeDelegator<CaliceExampleProcessor> _bmlCaenConfDataChanged;

  /** The method which is called in case the TrgReadoutconfData change */
  void setBmlCaenConfDataCol(EVENT::LCCollection *bmlCaenConfDataCol) {
    _bmlCaenConfDataCol = bmlCaenConfDataCol;
  } 

  /** The Collection which contains the TrgReadoutConfData */
  LCCollection* _bmlCaenConfDataCol;

  /** The delegator which is notified when the Bml Caen RoConfData change */
  ConditionsChangeDelegator<CaliceExampleProcessor> _bmlCaenRoConfDataChanged;

  /** The method which is called in case the TrgReadoutconfData change */
  void setBmlCaenRoConfDataCol(EVENT::LCCollection *bmlCaenRoConfDataCol) {
    _bmlCaenRoConfDataCol = bmlCaenRoConfDataCol;
  } 

  /** The Collection which contains the TrgReadoutConfData */
  LCCollection* _bmlCaenRoConfDataCol;

  //Note that the following classes functions and methods will be put into a BeamLineHandler soon
  /** The delegator which is notified when the BmlSlowRundata (=beam parameters (CERN)) change */
  ConditionsChangeDelegator<CaliceExampleProcessor> _bmlSroRunDataChanged;

  /** The method which is called in case the BmlSlowRundata change */
  void setBmlSroRunDataCol(EVENT::LCCollection *bmlSroRunDataCol) {
    _bmlSroRunDataCol = bmlSroRunDataCol;
  } 

  /** The Collection which contains the VBmlSlowRunData */
  LCCollection* _bmlSroRunDataCol;



  /** The array of delegators which are notified when the VBmlSlowRundata change*/
  std::vector<CALICE::MultipleConditionsChangeDelegator<CaliceExampleProcessor> > _bmlParamChangeDelegatorVec;

  /** The method which is called in case the VBmlSlowRundata change */
  void setVBmlSroRunDataCol(EVENT::LCCollection*, unsigned int); 

  /** The map which holds the relation between the delegator (index) and the type of parameters */
  typedef std::map<unsigned, std::string> BmlParamsIndexMap_t;
  BmlParamsIndexMap_t _bmlParamsIndexMap;
  /** The map which holds the relation between the type of parameters and the pointer to the corresponding Conditionsdata Collection*/
  typedef std::map<std::string, LCCollection*> BmlParamsCollectionMap_t;
  BmlParamsCollectionMap_t _bmlParamsCollectionMap;


  //string for collection names
  string _colRunInfo;
  string _colAdcVals;
  //string _colTdcVals;  
  lcio::StringVec _colTdcVals;
  string _colAhcFeConf;
  string _colBeTrgConf;
  string _colEmcFeConf;
  string _colFeConf;
  string _colAhcVfeConf;
  string _colAhcSroData;
  string _colAhcSroModData;
  string _colSceSroModData;
  string _colSceSroTempData;
  string _colEmcStgData;
  string _colRoConfData;
  string _colTrgRoConfData;
  string _colBmlCaenConfData;
  string _colBmlCaenRoConfData;
  //Note that the following three collections will soon be handled in a BeamLineHandler
  string _colBmlSroRunDataCern;
  lcio::StringVec _bmlCollections; 
  string _parNameTriggerConf;
  string _parNameTriggerAndEnable;
  string _parNameTriggerEvent;
  string _parNameTriggerMainWord;
  string _parNameTriggerPreHistory;
  string _parNameTriggerPostHistory;
  string _parNameErroBits;

   //The TriggerHandler
  TriggerHandlerCalice* _theTrigHandler;

  //method which demonstrates  the access to the calice data types 
  void printMiscInformation(LCEvent*);

  /** string to initialize the database */
  std::string _dbInit;

  /** Folder Name of Run Summary */
  std::string _runsumfolder; 

#ifdef HAVE_ROOT
  //===== Root stuff =====
  int _blockMin, _blockMax;
  TFile* _hfile;
  TH1F* _hpre;
  TH1F* _hpost;
  TH2F* _hbit;
  //TH1F* _hall;
  //TH1F* _hlimits;
  //TH1F* _hpeak;
  //std::map<int,TH1F*> _hmap;
  int _debug; 
#endif  

};
}
#endif

#endif
