#ifndef PedestalConstants_h
#define PedestalConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject.hh>

using namespace lcio;

namespace CALICE {

class PedestalConstants;

/**
  * Class to store the results of a pedestal measurement, i.e. a pedestal plus the error of it
  * for a certain chip/channel for a fixed moduleID
  * @author S. Schmidt DESY
  * @date Jun 16 2006
  */

class PedestalConstants: public LCHcalCalibrationObject{

public:
  
  PedestalConstants() {};
  
  PedestalConstants(unsigned chip, unsigned channel, float value, float error): _cutSignificance(3.0) {
    unsigned _cellId = (chip << 8) + channel;
    setCellKey(_cellId);
    obj()->setFloatVal(0, value);
    obj()->setFloatVal(1, error);
  };
  
  PedestalConstants(LCObject* obj): LCHcalCalibrationObject(obj), _cutSignificance(3.0) {};
  
  virtual ~PedestalConstants() {};

  /** get pedestial value 
    */
  float getPedestalValue() const {
    return getFloatVal(0);
  };  

  /** get pedestal error
    */
  float getPedestalError() const {  
    return getFloatVal(1);
  };
  
  /** apply calibration to energy value 
    */
  float applyCalibration(float inputValue) const {
    return (float) (inputValue - getPedestalValue());
  };
  
  float cancelCalibration(float outputValue) const {
    return (float) (outputValue + getPedestalValue());
  };
  
  /** apply calibration error to current error
    */
  float applyCalibrationError(float inputValue, float inputError) const {
    return (float) (sqrt(pow(inputError,2)+pow(getPedestalError(),2)));
  };      

  /** check if a valid calibration is available
    */
  bool calibrationValid() const {
    return (!std::isnan(getPedestalValue()));
  };

  /** accept only events which are distinguishable from pedestals
    */
  bool keepEvent(float resultValue, float resultError) const {
    return (resultValue - resultError*_cutSignificance > 0.);
  };    

  float getParam(unsigned paramIndex) {
    if (paramIndex==0) return _cutSignificance; 
    else {
      throw std::runtime_error("parameter index not valid");
    }  
  };
  
  void setParam(unsigned paramIndex, float paramValue) {
    if (paramIndex==0) _cutSignificance = paramValue;
    else {
      throw std::runtime_error("parameter index not valid");
    }
  };

  /** convenient print method
    */
  void print (std::ostream& os);
  
  /** return type of the class
    */
  const std::string getTypeName() const {
    return "PedestialConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:PedestalValue,f:PedestalError";
  };  
    
protected:
  float _cutSignificance; 
    
};

}

#endif  
