#ifndef InterConstants_h
#define InterConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject.hh>

using namespace lcio;

namespace CALICE {

class InterConstants;

/**
  * Class to store the results of a inter calibration measurement, i.e. conversion factor between
  * calibration mode and physics mode plus the error of it for a certain chip/channel for a fixed moduleID
  * @author S. Schmidt DESY
  * @date Aug 24 2006
  */

class InterConstants: public LCHcalCalibrationObject{

public:
  
  InterConstants() {};
  
  InterConstants(unsigned chip, unsigned channel, float value, float error) {
    unsigned _cellId = (chip << 8) + channel;
    setCellKey(_cellId);
    obj()->setFloatVal(0, value);
    obj()->setFloatVal(1, error);
  };
  
  InterConstants(LCObject* obj): LCHcalCalibrationObject(obj) {};
  
  virtual ~InterConstants() {};

  /** get conversion factor value 
    */
  float getInterValue() const {
    return getFloatVal(0);
  };  

  /** get conversion factor error
    */
  float getInterError() const {  
    return getFloatVal(1);
  };
  
  /** apply calibration to energy value 
    */
  float applyCalibration(float inputValue) const {
    return (float) (inputValue * getInterValue());
  };
  
  /** cancel an applied calibration from energy value
    */
  float cancelCalibration(float outputValue) const {
    return (float) (outputValue / getInterValue());
  };  
  
  /** apply calibration error to current error
    */
  float applyCalibrationError(float inputValue, float inputError) const {
    return (float) (inputError * getInterValue() + inputValue * getInterError());
  };

  /** apply error due to canceling a calibration to current error
    */
  float cancelCalibrationError(float outputValue, float outputError) const {
    return (float) (sqrt(pow(outputError/outputValue,2)+pow(getInterError()/getInterValue(),2))*(outputValue/getInterValue()));
  };      

  /** check if a valid calibration is available
    */
  bool calibrationValid() const {
    return (!std::isnan(getInterValue()) && (getInterValue()>0));
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
    return "InterConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:InterValue,f:InterError";
  };  
    
};

}

#endif  
