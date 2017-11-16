#ifndef SaturationConstants_h
#define SaturationConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject2D.hh>

using namespace lcio;

namespace CALICE {

class SaturationConstants;

/**
  * Class to store the parametrisation of the saturation curve (no error yet)
  * for a certain chip/channel for a fixed moduleID
  * @author S. Schmidt DESY
  * @date Aug 18 2006
  */

class SaturationConstants: public LCHcalCalibrationObject2D{

public:
  
  SaturationConstants() {};
  
  SaturationConstants(unsigned chip, unsigned channel, float value, float error) {
    unsigned _cellId = (chip << 8) + channel;
    setCellKey(_cellId);
    obj()->setFloatVal(0, value);
  };
  
  SaturationConstants(LCObject* obj): LCHcalCalibrationObject2D(obj) {};
  
  virtual ~SaturationConstants() {};

  /** get parametrisation value 
    */
  float getSaturationValue() const {
    return getFloatVal(0);
  };  

  /** get parametrisation error
    */
  float getSaturationError() const {  
    return 0.;
  };
  
  /** apply calibration to energy value 
    */
  float applyCalibration(float inputValue, float inputParameter) const {
    return (float) (inputValue / (1 - getSaturationValue() * inputParameter));
  };
  
  /** cancel an applied calibration from energy value
    */
  float cancelCalibration(float outputValue, float inputParameter) const {
    return (float) (outputValue * (1 - getSaturationValue() * inputParameter));
  };  
  
  /** apply calibration error to current error
    */
  float applyCalibrationError(float inputValue, float inputError, float inputParameter, float inputParameterError) const {
    return (float) (inputError);
  };
  
   /** apply error due to canceling a calibration to current error
    */
  float cancelCalibrationError(float outputValue, float outputError, float inputParameter, float inputParameterError) const {
    return (float) (outputError);
  };        

  /** check if a valid calibration is available
    */
  bool calibrationValid() const {
    return (!std::isnan(getSaturationValue()));
  };

  /** no cut for the moment
    */
  bool keepEvent(float resultValue, float resultError) const {
    return true;
  };    

  /** convenient print method
    */
  void print (std::ostream& os);
  
  /** return type of the class
    */
  const std::string getTypeName() const {
    return "SaturationConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:SaturationValue";
  };  
    
};

}

#endif  
