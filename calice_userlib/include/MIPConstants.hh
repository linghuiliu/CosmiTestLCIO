#ifndef MIPConstants_h
#define MIPConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject.hh>

using namespace lcio;

namespace CALICE {

class MIPConstants;

/**
  * Class to store the results of a MIP calibration measurement, i.e. channels/MIP plus the error of it
  * for a certain chip/channel for a fixed moduleID
  * @author S. Schmidt DESY
  * @date Aug 11 2006
  */

class MIPConstants: public LCHcalCalibrationObject{

public:
  
  MIPConstants() {};
  
  MIPConstants(unsigned chip, unsigned channel, float value, float error): _cutValue(0.5) {
    unsigned _id = (chip << 8) + channel;
    setCellKey(_id);
    obj()->setFloatVal(0, value);
    obj()->setFloatVal(1, error);
  };
  
  MIPConstants(LCObject* obj): LCHcalCalibrationObject(obj), _cutValue(0.5) {};
  
  virtual ~MIPConstants() {};

  /** get channels/MIP value 
    */
  float getMIPValue() const {
    return getFloatVal(0);
  };  

  /** get channels/MIP error
    */
  float getMIPError() const {  
    return getFloatVal(1);
  };
  
  /** apply calibration to energy value 
    */
  float applyCalibration(float inputValue) const {
    return (float) (inputValue / getMIPValue());
  };
  
  /** cancel an applied calibration from energy value
    */
  float cancelCalibration(float outputValue) const {
    return (float) (outputValue * getMIPValue());
  };  
  
  /** apply calibration error to current error
    */
  float applyCalibrationError(float inputValue, float inputError) const {
    return (float) (sqrt(pow(inputError/inputValue,2)+pow(getMIPError()/getMIPValue(),2))*(inputValue/getMIPValue()));
  };
  
  /** apply error due to canceling a calibration to current error
    */
  float cancelCalibrationError(float outputValue, float outputError) const {
    return (float) (outputValue * getMIPError() + getMIPValue() * outputError);
  };      

  /** check if a valid calibration is available
    */
  bool calibrationValid() const {
    return (!std::isnan(getMIPValue()) && (getMIPValue()>0));
  };

  /** no cut for the moment
    */
  bool keepEvent(float resultValue, float resultError) const {
    return resultValue > _cutValue;
  };
  
  float getParam(unsigned paramIndex) {
    if (paramIndex==0) return _cutValue; 
    else {
      throw std::runtime_error("parameter index not valid");
    }  
  };
  
  void setParam(unsigned paramIndex, float paramValue) {
    if (paramIndex==0) _cutValue = paramValue;
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
    return "MIPConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:MIPValue,f:MIPError";
  };
  
protected:
  float _cutValue; 
    
};

}

#endif  
