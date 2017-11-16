#ifndef HcalTempModel_h
#define HcalTempModel_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include <stdexcept>
#include <cstdlib>

#include <lcio.h>
#include <lccd.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <TempModel.hh>
#include "AhcSlowReadoutModBlock.hh"

#define NPS 1000000000LL

using namespace lcio;

namespace CALICE {

class HcalTempModel : public TempModel {

public: 
  HcalTempModel():TempModel(), _firstWarning(true) {};
  
  virtual ~HcalTempModel() {};
  
  virtual float getTemp(lcio::LCCollection* col, unsigned moduleID, unsigned CellKey) {
    
    if (!col) throw std::runtime_error("HcalTempModel::getTemp: invalid collection");
    unsigned _chip = (CellKey & 0xFF00) >> 8;
    unsigned _channel = CellKey & 0x00FF;
    unsigned _module = (moduleID & 0xFF00) >> 8;

/* some problems in summer 2006 need some fixes: */
    lccd::LCCDTimeStamp _timestamp = std::atoll(col->parameters().getStringVal(lccd::DBSINCE).c_str())/NPS;
    if (!_timestamp && _firstWarning) {
      std::cout << "HcalTempModel: **********************************************************************************************" << std::endl;
      std::cout << "HcalTempModel: during on the fly conversion not all fixes for temerature measurements in 2006 can be applied!" << std::endl;
      std::cout << "HcalTempModel: **********************************************************************************************" << std::endl;
      _firstWarning = false;
    }
    if (((_timestamp>1153E6) && (_timestamp<1164E6)) || (static_cast<unsigned int>(col->getNumberOfElements())<38)) {
/*  for modules 18, 20, 22 the slow control mapping was wrong, let's take the adjacent modules */	      
      if ((_module==18)||(_module==20)||(_module==22)) _module++;
/*  for module 21, chip 11 the temperature is a little bit too hot, let's take another module */       
      if ((_module==21)&&(_chip==11)) _module=23;
/*  for module 14 the temperature got stuck at some time, let's take another module */
      if ((_module==14)&&(_timestamp>1160.5E6)&&(_timestamp<1160.815E6)) _module=15;
    }
    if (col) {
      for (unsigned int imod=0; imod < static_cast<unsigned int>(col->getNumberOfElements()); imod++) {
	AhcSlowReadoutModBlock ahcSroModBlock(col->getElementAt(imod)); 
	if((unsigned) ahcSroModBlock.getModuleNumber() == _module) {
          std::vector<double> cmbTemperatures(ahcSroModBlock.getCmbTemperatures());
          /* quite simple assignment of the various cells to the 5 temperature sensors */	    
          if (_chip == 0) return cmbTemperatures[0];
          if ((_chip >= 1) && (_chip <= 3)) return cmbTemperatures[1];
          if ((_chip >= 4) && (_chip <= 7)) return cmbTemperatures[2];
          if ((_chip >= 8) && (_chip <= 10)) return cmbTemperatures[3];
          if (_chip == 11) return cmbTemperatures[4];
        }
      }
    }
    std::stringstream message;
    message << "HcalTempModel::getTemp: no temperature measurement " << 
      "for module " << _module << ", chip " << _chip << ", channel " << _channel <<
      "at timestamp " << _timestamp << std::endl;
    throw std::runtime_error(message.str());
    return 0;
  };
  
  virtual float getTempError(lcio::LCCollection* col, unsigned moduleID, unsigned CellKey) {
/* assume a precision of 0.1 degrees */
    return 0.1;
  };
  
private: 
  bool _firstWarning;
  
};

}

#endif
