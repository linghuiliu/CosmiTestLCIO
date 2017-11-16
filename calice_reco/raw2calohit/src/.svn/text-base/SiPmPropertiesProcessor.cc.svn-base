#include "SiPmPropertiesProcessor.hh"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <marlin/ConditionsProcessor.h>
#include "marlin/Exceptions.h"
#include <TilesItep.hh>
#include <TilesItepSaturation.hh>
#include <TilesProduction.hh>

#include "streamlog/streamlog.h"

//#define HCALRECO_DEBUG

namespace CALICE {

SiPmPropertiesProcessor aSiPmPropertiesProcessor();

  SiPmPropertiesProcessor::SiPmPropertiesProcessor(const std::string processorName): marlin::Processor(processorName), 



           _defaultPixelScaleFactorsMap(&CALICE::SimpleValue::getCellID),



           _pixelScaleFactorsMap(&CALICE::SimpleValue::getCellID),


	   _SiPmInfoChange(this,&SiPmPropertiesProcessor::SipmInfoChanged),
	   _SiPmSaturationChange(this,&SiPmPropertiesProcessor::SipmSaturationChanged),
           _ModuleProductionChange(this,&SiPmPropertiesProcessor::ModuleProductionChanged),
           _pixelScaleFactorsChange(this,&SiPmPropertiesProcessor::pixelScaleFactorsChanged),
           _defaultPixelScaleFactorsChange(this,&SiPmPropertiesProcessor::defaultPixelScaleFactorsChanged)
                                                                                     



                                                                                     
  {
    _description = "This processor maps the Sipm Properties of two DB tables and allows correlations between the tables elements.";

    registerProcessorParameter("SiPmInfoCollectionName", "Name of the SiPm ITEP collection",
                               _SiPmInfoColName, std::string("SiPmInfo"));
    registerProcessorParameter("SiPmSaturationCollectionName", "Name of the SiPm ITEP Saturation collection",
                               _SiPmSaturationColName, std::string("SipmSaturation"));
    registerProcessorParameter("ModuleProductionCollectionName", "Name of the module production collection",
			       _ModuleProductionColName, std::string("ModuleProduction"));

    registerProcessorParameter("PixelScaleFactorsCollectionName", 
                               "Name of the pixel scale factors collection",
			       _pixelScaleFactorsColName, 
                               std::string("PixelScaleFactors"));

    registerProcessorParameter("DefaultPixelScaleFactorsCollectionName", 
                               "Name of the pixel scale factors collection",
			       _defaultPixelScaleFactorsColName, 
                               std::string("DefaultPixelScaleFactors"));

    registerProcessorParameter("AssumeIncrease",
                               "Assumes an increasing saturation correction curve.",
			       _assumeIncrease,
			       (int)1);	     
    registerProcessorParameter("ReduceFluctuations",
                               "Reduces fluctuations by fixing the saturation correction curve for low pixel rates.",
			       _reduceFluctuations,
			       (int)1);	
    registerProcessorParameter("DefaultPixelScaleFactor",
                               "",
                               _defaultPixelScaleFactor,
                               (float)0.8);

    registerProcessorParameter("AdditionalOverallPixelScaleFactor",
                               "",
                               _additionalOverallPixelScaleFactor,
                               (float)1.0);

}


void SiPmPropertiesProcessor::init() {

  printParameters();
  _SiPmInfoEmpty = true;
  _SiPmSaturationEmpty = true;
  _ModuleProductionEmpty = true;

  _defaultPixelScaleFactorsEmpty = true;

  _pixelScaleFactorsEmpty = true;



  std::stringstream message;
  message << "SipmPropertiesProcessor: undefined conditionsdata: ";
  bool error = false;
  try {
    if (!marlin::ConditionsProcessor::registerChangeListener( &_ModuleProductionChange,_ModuleProductionColName)) {
      message << " " << _ModuleProductionColName;
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_SiPmInfoChange,_SiPmInfoColName)) {
      message << " " << _SiPmInfoColName;
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_SiPmSaturationChange,_SiPmSaturationColName)) {
      message << " " << _SiPmSaturationColName;
    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_pixelScaleFactorsChange,
                                                              _pixelScaleFactorsColName)) {

    }
    if (!marlin::ConditionsProcessor::registerChangeListener( &_defaultPixelScaleFactorsChange,
                                                              _defaultPixelScaleFactorsColName)) {

    }
  }
  catch (ErrorMissingConditionsDataHandler &conddata_error) {
    std::string a(conddata_error.what());
    error = true;
    if (a.size()>0) {
      a.erase(a.size()-1);
      message << a; 
    }
  }
  if (error) { 
    message <<  ".";
    throw ErrorMissingConditionsDataHandler(message.str());
  }

  UpdateVectors();
};


void SiPmPropertiesProcessor::processEvent(LCEvent* evt) {

  int mod =1;
  int chip=2;
  int chan=3;

  std::cout<<"SiPm ID = "<< getSipmID(mod,chip,chan) <<std::endl;
  cout<<"Voltage = "<< getSipmVoltage(getSipmID(mod,chip,chan)) <<endl;
  cout<<"Temp = "<< getTemp(getSipmID(mod,chip,chan)) <<endl;
  cout<<"XTalk = "<< getXTalk(getSipmID(mod,chip,chan)) <<endl;
  cout<<"TileSize = "<< getTileSize(mod,chip,chan) <<endl;
  
  std::cout<<"SiPm saturation curve: "<<std::endl;
  float sipmSatArray[SATPOINTS];
  float pmtMipSatArray[SATPOINTS];
  for (int point=0; point<SATPOINTS; point++){
    sipmSatArray[point] = getSipmPixelSat(mod,chip,chan,point);
    pmtMipSatArray[point] = getPmtMipSat(mod,chip,chan,point);
    std::cout << pmtMipSatArray[point] << " " << sipmSatArray[point] << " " << std::endl;
  }

  


}


void SiPmPropertiesProcessor::SipmInfoChanged(lcio::LCCollection* col) {

#ifdef HCALRECO_DEBUG
   std::cout << "SiPmPropertiesProcessor::SipmInfoChanged()" << std::endl;
#endif
  if (!col) return;
  _SiPmInfoEmpty = false;
  _SipmVoltageMap.clear() ;
  _SipmVoltageBreakdownMap.clear();
  _Delta_SPEMap.clear();
  _Phe_MIPMap.clear();
  _TempMap.clear();
  _CurrentMap.clear();
  _CurrentRMSMap.clear();
  _DarkRate0Map.clear();
  _DarkRateHalfMap.clear();
  _DarkRateHalfCorrMap.clear();
  _PedRMSMap.clear();
  _PeakWidthMap.clear();
  _XTalkMap.clear();

  for (unsigned i = 0; i < static_cast<unsigned>(col->getNumberOfElements()); i++) {
    TilesItep* sipminfo = new TilesItep (col->getElementAt(i));
    
    _SipmVoltageMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getVoltage()));
    _SipmVoltageBreakdownMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getVoltageBreakdown()));
    _Delta_SPEMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getDelta_SPE()));
    _Phe_MIPMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getPhe_MIP()));
    _TempMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getTemp()));
    _CurrentMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getCurrent()));
    _CurrentRMSMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getCurrentRMS()));
    _DarkRate0Map.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getDarkRate0()));
    _DarkRateHalfMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getDarkRateHalf()));
    _DarkRateHalfCorrMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getDarkRateHalfCorr()));
    _PedRMSMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getPedRMS()));
    _PeakWidthMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getPeakWidth()));
    _XTalkMap.insert(std::make_pair(sipminfo->getSIPMID(),sipminfo->getXTalk()));
  };
  UpdateVectors();
};




void SiPmPropertiesProcessor::SipmSaturationChanged(lcio::LCCollection* col) {

  float pix[ 20 ] = { 0, 2.917, 5.246, 8.014, 13.633, 24.965, 45.18, 75.394, 123.473, 185.76, 362.292, 658.245, 992.597, 1320.47, 1514.55, 1645.65, 1727.02, 1766.99, 1797.09, 1810.79 };
  float pmt[ 20 ] = { 0, 2.917, 5.246, 8.014, 13.6075, 25.4428, 46.1734, 77.9442, 131.121, 203.782, 430.603, 895.825, 1525.21, 2422.46, 3177.46, 3894.37, 4519.95, 4954.43, 5459.21, 5884.61 };

#ifdef HCALRECO_DEBUG
   std::cout << "SiPmPropertiesProcessor::SipmSaturationChanged()" << std::endl;
#endif
  if (!col) return;
  _SiPmSaturationEmpty = false;
  for (unsigned point = 0; point < SATPOINTS; point++) {
    _SipmPixelSatMap[point].clear();
    _PmtMipSatMap[point].clear();
  }  

  for (unsigned i = 0; i < static_cast<unsigned>(col->getNumberOfElements()); i++) {
    TilesItepSaturation* sipmsaturation = new TilesItepSaturation (col->getElementAt(i));


    if ( sipmsaturation->getSipmPixelSat( 3 ) == 0 ){
      //  empty curve, use default
      for(int point = 0; point < 20; point++){
	_SipmPixelSatMap[point].insert(std::make_pair(sipmsaturation->getSIPMID(), 
                                                      pix[point]));

	_PmtMipSatMap[point].insert(std::make_pair(sipmsaturation->getSIPMID(), pmt[point]));
      }
    } else {
    
      for(int point = 0; point < SATPOINTS; point++){

        _SipmPixelSatMap[point].insert(std::make_pair(sipmsaturation->getSIPMID(), 
                                                      sipmsaturation->getSipmPixelSat(point)));

	_PmtMipSatMap[point].insert(std::make_pair(sipmsaturation->getSIPMID(), 
                                                   sipmsaturation->getPmtMipSat(point)));          
        }


      }


  }

  UpdateVectors();
};


void SiPmPropertiesProcessor::ModuleProductionChanged(lcio::LCCollection* col) {

#ifdef HCALRECO_DEBUG
   std::cout << "SiPmPropertiesProcessor::ModuleProductionChanged()" << std::endl;
#endif
  if (!col) return;
  _ModuleProductionEmpty = false; 
  _SIPMMap.clear();
  _TileSizeMap.clear();

  for (unsigned i = 0; i < static_cast<unsigned>(col->getNumberOfElements()); i++) {
    TilesProduction* moduleinfo = new TilesProduction (col->getElementAt(i));
    
    const unsigned _ModChipChannel = (moduleinfo->getModule()<<16) + (moduleinfo->getChip() << 8) + (moduleinfo->getChan());

    _SIPMMap.insert(std::make_pair(_ModChipChannel,moduleinfo->getSIPMID())) ;
    _TileSizeMap.insert(std::make_pair(_ModChipChannel,moduleinfo->getTileSize()));

  /* --------------------------------------------------------------- */

  };
  UpdateVectors();
};


  void SiPmPropertiesProcessor::pixelScaleFactorsChanged(lcio::LCCollection* col) {

    _pixelScaleFactorsMap.conditionsChanged(col);

    _pixelScaleFactorsEmpty = false;

    streamlog_out(DEBUG0) << _pixelScaleFactorsMap.map().size() 
                          << " pixel scale factors available\n";

    UpdateVectors();

  }
  
  void SiPmPropertiesProcessor::defaultPixelScaleFactorsChanged(lcio::LCCollection* col) {

    _defaultPixelScaleFactorsMap.conditionsChanged(col);

    _defaultPixelScaleFactorsEmpty = false;

    streamlog_out(DEBUG0) << _defaultPixelScaleFactorsMap.map().size() 
                          << " default pixel scale factors available\n";

    UpdateVectors();

  }


void SiPmPropertiesProcessor::UpdateVectors() {


    

  if (empty()) return;

  _SipmVoltageVector.clear();
  _SipmVoltageVector.resize(40*256);
  _SipmVoltageBreakdownVector.clear();
  _SipmVoltageBreakdownVector.resize(40*256);
  _Delta_SPEVector.clear();
  _Delta_SPEVector.resize(40*256);
  _Phe_MIPVector.clear(); 
  _Phe_MIPVector.resize(40*256); 
  _TempVector.clear(); 
  _TempVector.resize(40*256); 
  _CurrentVector.clear(); 
  _CurrentVector.resize(40*256); 
  _CurrentRMSVector.clear();
  _CurrentRMSVector.resize(40*256);
  _DarkRate0Vector.clear(); 
  _DarkRate0Vector.resize(40*256); 
  _DarkRateHalfVector.clear();
  _DarkRateHalfVector.resize(40*256);
  _DarkRateHalfCorrVector.clear(); 
  _DarkRateHalfCorrVector.resize(40*256); 
  _PedRMSVector.clear(); 
  _PedRMSVector.resize(40*256); 
  _PeakWidthVector.clear();
  _PeakWidthVector.resize(40*256);
  _XTalkVector.clear();
  _XTalkVector.resize(40*256);
  for (unsigned point=0; point < SATPOINTS; point++) {
    _SipmPixelSatVector[point].clear();
    _PmtMipSatVector[point].clear();
    _SipmPixelSatVector[point].resize(40*256);
    _PmtMipSatVector[point].resize(40*256);
    _InterpolationA[point].resize(40*256);
    _InterpolationA[point].clear();
    _InterpolationB[point].resize(40*256);
    _InterpolationB[point].clear();
  }
  _TileSizeVector.clear();
  _TileSizeVector.resize(40*256);
  for (unsigned i=0; i < _corGraphVector.size(); i++) {
    if (_corGraphVector[i]) delete _corGraphVector[i];
    _corGraphVector[i] = 0;
  }
  _corGraphVector.clear();
  _corGraphVector.resize(40*256);
  for (unsigned i=0; i < _satGraphVector.size(); i++) {
    if (_satGraphVector[i]) delete _satGraphVector[i];
    _satGraphVector[i] = 0;
  }
  _satGraphVector.clear();
  _satGraphVector.resize(40*256);  
  for (std::map<unsigned,unsigned>::iterator it=_SIPMMap.begin(); it != _SIPMMap.end(); ++it) {
    const unsigned module = (it->first & 0xFF0000) >> 16;
    const unsigned chip = (it->first & 0x00FF00) >> 8;
    const unsigned channel = it->first & 0x0000FF;
    const unsigned ModChipChanKey = module*256+chip*32+channel;

    const int currentSIPMID = it->second;

    /* ------------------- get pixel scale factor ------------------- */

    float currentScaleFactor(1);

  if(parameterSet("PixelScaleFactorsCollectionName") == true &&
       _pixelScaleFactorsEmpty == false ) { 

    }

    /* ------------------- pixel scale factors ------------------- */

    int hti(CALICE::HcalTileIndex(module,
                                  chip,
                                  channel
                                  ).getIndex());

    /* (1) individual factors are requested and available
     * (2) individual default factors are requested and available
     */  

    if(parameterSet("PixelScaleFactorsCollectionName") == true &&
       _pixelScaleFactorsEmpty == false ) { 
    // (1) is true

      // try to get the individual factor
      try {

        currentScaleFactor = _pixelScaleFactorsMap.find(hti).getValue();
        streamlog_out(DEBUG0) << "Using individual factor.\n";
      } catch(lcio::Exception &e) {
        // but we haven't found an individual factor

        if(parameterSet("DefaultPixelScaleFactorsCollectionName") == true &&
           _defaultPixelScaleFactorsEmpty == false ) {
          // but (2) is true: plan: take the individual default factor
          
          // try to get the individual default factor
          try {

            currentScaleFactor = _defaultPixelScaleFactorsMap.find(hti).getValue();
            streamlog_out(DEBUG0) << "Using individual default factor.\n";
          } catch(lcio::Exception &e) {
            // but we haven't found an individual default factor, 
            // so we take the global factor

            currentScaleFactor = _defaultPixelScaleFactor;
            streamlog_out(DEBUG0) << "Using global default factor.\n";
          }

        } else {
          // (2) is not true, take the global default factor
          currentScaleFactor = _defaultPixelScaleFactor;
          streamlog_out(DEBUG0) << "Using global default factor.\n";
        }

      }
    } 
    else if(parameterSet("DefaultPixelScaleFactorsCollectionName") == true &&
            _defaultPixelScaleFactorsEmpty == false ) {
      // (1) is false, but (2) is true

      // try to get the individual default factor
      try {
        
        currentScaleFactor = _defaultPixelScaleFactorsMap.find(hti).getValue();
        streamlog_out(DEBUG0) << "Using individual default factor.\n";
      } catch(lcio::Exception &e) {
        // but we haven't found an individual default factor, 
        // so we take the global factor
        
        currentScaleFactor = _defaultPixelScaleFactor;
        streamlog_out(DEBUG0) << "Using global default factor.\n";
      }
      
    } else {
      // (1) and (2) are not true, take the global default factor
      currentScaleFactor = _defaultPixelScaleFactor;
      streamlog_out(DEBUG0) << "Using global default factor.\n";
    }
    
    // Do we need safety net?
    // if( currentScaleFactor < 0.5 ) {
    //   
    //   currentScaleFactor = _additionalOverallPixelScaleFactor * 
    //                        _defaultPixelScaleFactor;
    // 
    // }

    streamlog_out(DEBUG0) << "module: " << module
                          << " chip: " << chip
                          << " channel: " << channel 
                          << " SiPMID: " << currentSIPMID << " scale factor: " 
                          << currentScaleFactor << std::endl;

    /* --------------------------------------------------------------- */


#ifdef HCALRECO_DEBUG
//    std::cout << module << " " << chip << " " << channel << std::endl;
#endif
    _SipmVoltageVector[ModChipChanKey] = getSipmVoltage(it->second);
    _SipmVoltageBreakdownVector[ModChipChanKey] = getSipmVoltageBreakdown(it->second);
    _Delta_SPEVector[ModChipChanKey] = getDelta_SPE(it->second);
    _Phe_MIPVector[ModChipChanKey] = getPhe_MIP(it->second); 
    _TempVector[ModChipChanKey] = getTemp(it->second); 
    _CurrentVector[ModChipChanKey] = getCurrent(it->second); 
    _CurrentRMSVector[ModChipChanKey] = getCurrentRMS(it->second);
    _DarkRate0Vector[ModChipChanKey] = getDarkRate0(it->second); 
    _DarkRateHalfVector[ModChipChanKey] = getDarkRateHalf(it->second);
    _DarkRateHalfCorrVector[ModChipChanKey] = getDarkRateHalfCorr(it->second); 
    _PedRMSVector[ModChipChanKey] = getPedRMS(it->second); 
    _PeakWidthVector[ModChipChanKey] = getPeakWidth(it->second);
    _XTalkVector[ModChipChanKey] = getXTalk(it->second);
    for (unsigned point=0; point < SATPOINTS; point++) {      
      _SipmPixelSatVector[point][ModChipChanKey] = currentScaleFactor * getSipmPixelSat(it->second, point);
      _PmtMipSatVector[point][ModChipChanKey] = getPmtMipSat(it->second, point);
    }
    for (unsigned point=1; point < SATPOINTS; point++) {
      double c = getPmtMipSat(it->second, point) - getPmtMipSat(it->second, point-1);
      c /= currentScaleFactor * getSipmPixelSat(it->second, point) - currentScaleFactor * getSipmPixelSat(it->second, point-1);
      _InterpolationA[point][ModChipChanKey] = c;
      _InterpolationB[point][ModChipChanKey] = -c * currentScaleFactor * getSipmPixelSat(it->second, point-1) + getPmtMipSat(it->second, point-1);
    }
    float p[SATPOINTS+1], q[SATPOINTS+1], pix[SATPOINTS+1], pmt[SATPOINTS+1];
    pix[0] = 0;
    pmt[0] = 0;
    p[0] = 1;
    q[0] = 1;
    const float linearisationFactor = currentScaleFactor * getSipmPixelSat(it->second, 3) / getPmtMipSat(it->second, 3);
    float maxx = 0, maxy = 0;
    bool errorOccured = false;
    int imax = 0;
    for (unsigned point=0; point<SATPOINTS; point++) {
      pix[point] = currentScaleFactor * getSipmPixelSat(it->second, point);
      pmt[point] = getPmtMipSat(it->second, point);      
      if ((pix[point] > maxx) || !_assumeIncrease) {
        maxx = pix[point];
	maxy = pmt[point];
	imax = point;
      } else {
        pix[point] = maxx;
	pmt[point] = maxy;
      }
      pmt[point] *= linearisationFactor;
      if (((pix[point]==0) || (pmt[point]==0)) && (point>=3)) {
        if (getSipmID(module, chip, channel)==0) {
	  std::cout << "missing SiPm for module: " << module << ", chip: " << chip << ", channel: " << channel << std::endl;
	} else {
          streamlog_out(WARNING) << "SiPmPropertiesProcessor: invalid saturation curve for " 
                                 << getSipmID(module, chip, channel)
                                 << ", module: " << module << " chip: " << chip 
                                 << " channel: " << channel << std::endl;
	}
        errorOccured = true;
	p[point]=0;
	q[point]=0;
	break;
      } else {
        p[point] = pmt[point]/pix[point];
        q[point] = pix[point]/pmt[point];
      }	
      if (_reduceFluctuations) {
	for (unsigned point=0; point<3; point++) {
	  p[point] = 1;
	  q[point] = 1;
	}  
      }      
    }
    if (!errorOccured) {
//      //   set 'infinity point' for correction curve: linear extrapolation
//      pmt[SATPOINTS] = 10000;
//      pix[SATPOINTS] = pix[imax] + 
//	(10000 - pmt[imax]) * (pix[imax]-pix[imax-1])/(pmt[imax]-pmt[imax-1]);
      pmt[SATPOINTS] = pmt[imax];
      pix[SATPOINTS] = 1500;
      if ( _corGraphVector[ModChipChanKey] ) delete _corGraphVector[ModChipChanKey];
      _corGraphVector[ModChipChanKey] = new TGraph( SATPOINTS + 1, pix, pmt );

      //   set 'infinity point' for saturation curve: last measurement
      pix[SATPOINTS] = pix[imax];
      if ( _satGraphVector[ModChipChanKey] ) delete _satGraphVector[ModChipChanKey];
      _satGraphVector[ModChipChanKey] = new TGraph( SATPOINTS + 1, pmt, pix );
      /*
	_corGraphVector[ModChipChanKey] = new TGraph(SATPOINTS+1, pix, p);
	_satGraphVector[ModChipChanKey] = new TGraph(SATPOINTS+1, pmt, q);
      */
    } else {
      _corGraphVector[ModChipChanKey] = 0;
      _satGraphVector[ModChipChanKey] = 0;
    }    
  }
}; 


unsigned SiPmPropertiesProcessor::getSipmID(const unsigned module, const unsigned chip, const unsigned chan) const {
  const unsigned _ModChipChannel = (module<<16) + (chip << 8) + chan;
  const std::map<unsigned,unsigned>::const_iterator it = _SIPMMap.find(_ModChipChannel);    
  if(it == _SIPMMap.end())
    return 0;
  else
    return it->second;
};


unsigned SiPmPropertiesProcessor::getTileSize(const unsigned module, const unsigned chip, const unsigned chan) const {
  const unsigned _ModChipChannel = (module<<16) + (chip << 8) + chan;
  const std::map<unsigned,unsigned>::const_iterator it = _TileSizeMap.find(_ModChipChannel);    
  if(it == _TileSizeMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getSipmVoltage(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _SipmVoltageMap.find(sipm);
  if(it == _SipmVoltageMap.end())
    return 0;
  else
    return it->second;
};
  
  
float SiPmPropertiesProcessor::getSipmVoltage(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _SipmVoltageVector[module*256+chip*32+chan];
};
  

float SiPmPropertiesProcessor::getSipmVoltageBreakdown(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _SipmVoltageBreakdownMap.find(sipm) ;
  if (it == _SipmVoltageBreakdownMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getSipmVoltageBreakdown(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _SipmVoltageBreakdownVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getDelta_SPE(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _Delta_SPEMap.find(sipm);
  if( it == _Delta_SPEMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getDelta_SPE(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _Delta_SPEVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getPhe_MIP(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _Phe_MIPMap.find(sipm);
  if(it == _Phe_MIPMap.end())
    return 0;
  else
    return it->second;
};

  
float SiPmPropertiesProcessor::getPhe_MIP(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _Phe_MIPVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getTemp(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _TempMap.find(sipm);
  if (it == _TempMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getTemp(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _TempVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getCurrent(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _CurrentMap.find(sipm);
  if (it == _CurrentMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getCurrent(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _CurrentVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getCurrentRMS(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _CurrentRMSMap.find(sipm);
  if(it == _CurrentRMSMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getCurrentRMS(const unsigned module, const unsigned chip, const unsigned chan) const{
  return _CurrentRMSVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getDarkRate0(const unsigned sipm) const {  
  const std::map<unsigned,float>::const_iterator it = _DarkRate0Map.find(sipm);
  if(it == _DarkRate0Map.end())
    return 0;
  else
    return it->second;
};

float SiPmPropertiesProcessor::getDarkRate0(const unsigned module, const unsigned chip, const unsigned chan) const {
   return _DarkRate0Vector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getDarkRateHalf(const unsigned sipm) const {
  std::map<unsigned,float>::const_iterator it = _DarkRateHalfMap.find(sipm);
  if(it == _DarkRateHalfMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getDarkRateHalf(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _DarkRateHalfVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getDarkRateHalfCorr(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _DarkRateHalfCorrMap.find(sipm);
  if(it == _DarkRateHalfCorrMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getDarkRateHalfCorr(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _DarkRateHalfCorrVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getPedRMS(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _PedRMSMap.find(sipm);
  if( it == _PedRMSMap.end())
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getPedRMS(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _PedRMSVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getPeakWidth(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _PeakWidthMap.find(sipm);
  if( it == _PeakWidthMap.end() )
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getPeakWidth(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _PeakWidthVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getXTalk(const unsigned sipm) const {
  const std::map<unsigned,float>::const_iterator it = _XTalkMap.find( sipm ) ;
  if( it == _XTalkMap.end() )
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getXTalk(const unsigned module, const unsigned chip, const unsigned chan) const {
  return _XTalkVector[module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getSipmPixelSat(const unsigned sipm, const unsigned point) const {
  const std::map<unsigned,float>::const_iterator it = _SipmPixelSatMap[point].find(sipm);
  if( it == _SipmPixelSatMap[point].end() )
    return 0;
  else
    return it->second;
};


float SiPmPropertiesProcessor::getSipmPixelSat(const unsigned module, const unsigned chip, const unsigned chan, const unsigned point) const {
  return _SipmPixelSatVector[point][module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getPmtMipSat(const unsigned sipm, const unsigned point) const {
  const std::map<unsigned,float>::const_iterator it = _PmtMipSatMap[point].find(sipm);
  if (it == _PmtMipSatMap[point].end() )
    return 0;
  else
    return it->second;
};


inline float SiPmPropertiesProcessor::getPmtMipSat(const unsigned module, const unsigned chip, const unsigned chan, const unsigned point) const {
  return _PmtMipSatVector[point][module*256+chip*32+chan];
};


float SiPmPropertiesProcessor::getSiPmSaturationCorrection(const unsigned module, const unsigned cell, const float pixel) const {
  const unsigned chip = cell / 18;
  const unsigned chan = cell % 18; 
  return getSiPmSaturationCorrection(module, chip, chan, pixel);
};


float SiPmPropertiesProcessor::getSiPmSaturationCorrection(const unsigned module, const unsigned chip, const unsigned chan, const float pixel) const {
  const unsigned key = module*256+chip*32+chan;
//  int point;
//  for (point=0; point<SATPOINTS; point++)
//    if (pixel < _SipmPixelSatVector[point][key]) break;
//  if (point==0) return _PmtMipSatVector[point][key];
//  if (point==SATPOINTS) return _PmtMipSatVector[point-1][key];
//  float result = pixel*_InterpolationA[point][key]+_InterpolationB[point][key];  
//  return result;
  if ( !_corGraphVector[key] ) return 1.;
  return ( pixel > 0 ) ? _corGraphVector[key]->Eval(pixel) / pixel : 1.;
};


float SiPmPropertiesProcessor::getInverseSiPmSaturationCorrection(const unsigned module, const unsigned chip, const unsigned chan, const float pixel) const {
  const unsigned key = module*256+chip*32+chan;
  if (!_satGraphVector[key]) return 1.;
  return ( pixel > 0 ) ? _satGraphVector[key]->Eval(pixel)/pixel : 1.;
};


float SiPmPropertiesProcessor::getInverseSiPmSaturationCorrection(const unsigned module, const unsigned cell, const float pixel) const {
  const unsigned chip = cell / 18;
  const unsigned chan = cell % 18; 
  return getInverseSiPmSaturationCorrection(module, chip, chan, pixel);
};


unsigned SiPmPropertiesProcessor::getSatPointNr(const unsigned sipm) const {
  return SATPOINTS;
};

unsigned SiPmPropertiesProcessor::getSatPointNr(const unsigned module, const unsigned chip, const unsigned chan) const {
  return SATPOINTS;
};


bool SiPmPropertiesProcessor::empty() const {
  return _SiPmInfoEmpty || _SiPmSaturationEmpty || _ModuleProductionEmpty;
};

}
