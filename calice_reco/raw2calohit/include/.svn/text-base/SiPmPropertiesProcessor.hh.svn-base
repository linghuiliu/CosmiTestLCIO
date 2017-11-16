#ifndef SiPmPropertiesProcessor_h
#define SiPmPropertiesProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif
 
#include <TGraph.h>
#include "lcio.h"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include <ConditionsChangeDelegator.hh>
#include <VRawADCValueProcessor.hh>
#include <map>
#include <stdexcept>

#include "lccd/ConditionsMap.hh"
#include "SimpleValue.hh"

#define SATPOINTS 20


namespace CALICE { 

class SiPmPropertiesProcessor : public marlin::Processor {
 
 public: 
  SiPmPropertiesProcessor* newProcessor() {return new SiPmPropertiesProcessor(); }
  
  SiPmPropertiesProcessor(const std::string processorName="SiPmPropertiesProcessor");
  
  ~SiPmPropertiesProcessor() {};
  
  void init();
  
  void processEvent(LCEvent* evtP);  
  
  void processRunHeader(LCRunHeader* run) {};
  
  bool empty() const;
  
 protected:
  std::string _SiPmInfoColName;
  std::string _SiPmSaturationColName;
  std::string _ModuleProductionColName;

  std::string _pixelScaleFactorsColName;
  std::string _defaultPixelScaleFactorsColName;

  int _assumeIncrease;
  int _reduceFluctuations;

  float _defaultPixelScaleFactor;
  float _additionalOverallPixelScaleFactor;

  bool _defaultPixelScaleFactorsEmpty;
  lccd::ConditionsMap<int, CALICE::SimpleValue> _defaultPixelScaleFactorsMap;

  bool _pixelScaleFactorsEmpty;
  lccd::ConditionsMap<int, CALICE::SimpleValue> _pixelScaleFactorsMap;
 

  ConditionsChangeDelegator<SiPmPropertiesProcessor> _SiPmInfoChange;
  ConditionsChangeDelegator<SiPmPropertiesProcessor> _SiPmSaturationChange;
  ConditionsChangeDelegator<SiPmPropertiesProcessor> _ModuleProductionChange;

  ConditionsChangeDelegator<SiPmPropertiesProcessor> _pixelScaleFactorsChange;
  ConditionsChangeDelegator<SiPmPropertiesProcessor> _defaultPixelScaleFactorsChange;


  bool _SiPmInfoEmpty;
  bool _SiPmSaturationEmpty;
  bool _ModuleProductionEmpty;

  // A map from SiPM-ID the individual scale factor which will be
  // used
  std::map<unsigned int, float> _SIPMID_to_scaleFactor;

  std::map<unsigned,float> _SipmVoltageMap;
  std::map<unsigned,float> _SipmVoltageBreakdownMap;
  std::map<unsigned,float> _Delta_SPEMap;
  std::map<unsigned,float> _Phe_MIPMap; 
  std::map<unsigned,float> _TempMap; 
  std::map<unsigned,float> _CurrentMap; 
  std::map<unsigned,float> _CurrentRMSMap;
  std::map<unsigned,float> _DarkRate0Map; 
  std::map<unsigned,float> _DarkRateHalfMap;
  std::map<unsigned,float> _DarkRateHalfCorrMap; 
  std::map<unsigned,float> _PedRMSMap; 
  std::map<unsigned,float> _PeakWidthMap;
  std::map<unsigned,float> _XTalkMap;
  
  std::vector<float> _SipmVoltageVector;
  std::vector<float> _SipmVoltageBreakdownVector;
  std::vector<float> _Delta_SPEVector;
  std::vector<float> _Phe_MIPVector; 
  std::vector<float> _TempVector; 
  std::vector<float> _CurrentVector; 
  std::vector<float> _CurrentRMSVector;
  std::vector<float> _DarkRate0Vector; 
  std::vector<float> _DarkRateHalfVector;
  std::vector<float> _DarkRateHalfCorrVector; 
  std::vector<float> _PedRMSVector; 
  std::vector<float> _PeakWidthVector;
  std::vector<float> _XTalkVector;
   
  std::map<unsigned,float> _SipmPixelSatMap[SATPOINTS];
  std::map<unsigned,float> _PmtMipSatMap[SATPOINTS];

  std::vector<float> _SipmPixelSatVector[SATPOINTS];
  std::vector<float> _PmtMipSatVector[SATPOINTS];
  std::vector<float> _InterpolationA[SATPOINTS];
  std::vector<float> _InterpolationB[SATPOINTS];

  std::map<unsigned,unsigned> _SIPMMap;
  std::map<unsigned,unsigned> _TileSizeMap;

  std::vector<unsigned> _SIPMVector;
  std::vector<unsigned> _TileSizeVector;
  
  std::vector<TGraph*> _corGraphVector;
  std::vector<TGraph*> _satGraphVector;

  virtual void SipmInfoChanged(lcio::LCCollection* col);
  virtual void SipmSaturationChanged(lcio::LCCollection* col);
  virtual void ModuleProductionChanged(lcio::LCCollection* col);

  virtual void pixelScaleFactorsChanged(lcio::LCCollection* col);
  virtual void defaultPixelScaleFactorsChanged(lcio::LCCollection* col);
  
  virtual void UpdateVectors();

  // get functions according to db-tools/src/TilesProduction.hh 
  // and db-tools/src/TilesItep.hh

public:
  float getSipmVoltage(const unsigned sipm) const;
  float getSipmVoltage(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getSipmVoltageBreakdown(const unsigned sipm) const;
  float getSipmVoltageBreakdown(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getDelta_SPE(const unsigned sipm) const;
  float getDelta_SPE(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getPhe_MIP(const unsigned sipm) const;
  float getPhe_MIP(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getTemp(const unsigned sipm) const;
  float getTemp(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getCurrent(const unsigned sipm) const;
  float getCurrent(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getCurrentRMS(const unsigned sipm) const;
  float getCurrentRMS(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getDarkRate0(const unsigned sipm) const;
  float getDarkRate0(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getDarkRateHalf(const unsigned sipm) const;
  float getDarkRateHalf(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getDarkRateHalfCorr(const unsigned sipm) const;
  float getDarkRateHalfCorr(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getPedRMS(const unsigned sipm) const;
  float getPedRMS(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getPeakWidth(const unsigned sipm) const;
  float getPeakWidth(const unsigned module, const unsigned chip, const unsigned chan) const;
  float getXTalk(const unsigned sipm) const;
  float getXTalk(const unsigned module, const unsigned chip, const unsigned chan) const;

  unsigned getSatPointNr(const unsigned sipm) const;
  unsigned getSatPointNr(const unsigned module, const unsigned chip, const unsigned chan) const;
  
  float getSipmPixelSat(const unsigned sipm, const unsigned point) const;
  float getSipmPixelSat(const unsigned module, const unsigned chip, const unsigned chan, const unsigned point) const;
  float getPmtMipSat(const unsigned sipm, const unsigned point) const;
  float getPmtMipSat(const unsigned module, const unsigned chip, const unsigned chan, const unsigned point) const;

  float getSiPmSaturationCorrection(const unsigned module, const unsigned chip, const unsigned chan, const float pixel) const;
  float getSiPmSaturationCorrection(const unsigned module, const unsigned cell, const float pixel) const;
  float getInverseSiPmSaturationCorrection(const unsigned module, const unsigned chip, const unsigned chan, const float pixel) const;
  float getInverseSiPmSaturationCorrection(const unsigned module, const unsigned cell, const float pixel) const;

  unsigned getSipmID(const unsigned module, const unsigned chip, const unsigned chan) const;
  unsigned getTileSize(const unsigned module, const unsigned chip, const unsigned chan) const;
   
}; 

}

#endif

