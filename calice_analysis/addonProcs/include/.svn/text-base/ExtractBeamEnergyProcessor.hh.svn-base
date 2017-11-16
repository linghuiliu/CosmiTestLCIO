#ifndef ExtractBeamEnergyProcessor_HH
#define ExtractBeamEnergyProcessor_HH 1

//*******************************************
// This is a processor to look at PandoraPFA particle flow objects
//
//  I look at the reconstructed particles and clusters at fill properties to histos//
//
//********************************************



// Marlin & LCIO
#include "marlin/Processor.h"
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h" 

#include <EVENT/LCCollection.h>
#include <EVENT/LCGenericObject.h>
#include <EVENT/LCIO.h>

#include "BeamMetaData.hh"

#include "lccd/IConditionsChangeListener.hh"


using namespace marlin;
using namespace lcio;
using namespace std;
using namespace EVENT;
using namespace CALICE;


class ExtractBeamEnergyProcessor : public marlin::Processor, public lccd::IConditionsChangeListener
{
public:
  ExtractBeamEnergyProcessor();
  virtual ~ExtractBeamEnergyProcessor();
  
  ExtractBeamEnergyProcessor* newProcessor(){return new ExtractBeamEnergyProcessor();}

  virtual void init();
  virtual void processEvent(lcio::LCEvent*);
  virtual void end();


private:

  void conditionsChanged(LCCollection *col); 
 
  string _BeamMetaDataColName;
  BeamMetaData *beamMetaData; 

  
  float BeamEnergy ; 
  int ParticleTyePDG;
  bool _beamMetaDataChanged;
  LCCollection* _colBeamMetaData;

  
};

#endif
