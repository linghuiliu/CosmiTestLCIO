#ifndef GainConstants_h
#define GainConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject.hh>

using namespace lcio;

namespace CALICE {

class GainConstants;

/**
  * Class to store the results of a Gain calibration measurement, i.e. gain plus the error of it
  * for a certain chip/channel for a fixed moduleID
  * @author S. Schmidt DESY
  * @date Aug 18 2006
  */

class GainConstants: public LCHcalCalibrationObject{

public:
  
  /** contructor
    */
  GainConstants() {};
  
  /** constructor, initialised by parameters
    * @param chip chip number of the cell to be calibrated
    * @param channel channel number of the cell to be calibrated
    * @param value gain value
    * @param error error on the gain value
    */  
  GainConstants(unsigned chip, unsigned channel, float value, float error) {
    unsigned _cellId = (chip << 8) + channel;
    setCellKey(_cellId);
    obj()->setFloatVal(0, value);
    obj()->setFloatVal(1, error);
  };
  
  GainConstants(LCObject* obj): LCHcalCalibrationObject(obj) {};
  
  virtual ~GainConstants() {};

  /** get channels/pixel value 
    */
  float getGainValue() const {
    return getFloatVal(0);
  };  

  /** get channels/pixel error
    */
  float getGainError() const {  
    return getFloatVal(1);
  };
  
  /** apply gain calibration to energy value
    * @param inputValue "energy" value before the gain calibration
    * @return "energy" value after the gain calibration 
    */
  float applyCalibration(float inputValue) const {
    return (float) (inputValue / getGainValue());
  };
  
  /** cancel an applied calibration from energy value
    */
  float cancelCalibration(float outputValue) const {
    return (float) (outputValue * getGainValue());
  };  
  
  /** apply calibration error to current error
    * @param inputValue "energy" value before the gain calibration
    * @param inputError error on the "energy" value before the gain calibration
    * @return error of the "energy" value after the calibration, gained from error propagation of the
    *   error of the "energy" value before the calibration and the error of the gain calibration 
    */
  float applyCalibrationError(float inputValue, float inputError) const {
    return (float) (sqrt(pow(inputError/inputValue,2)+pow(getGainError()/getGainValue(),2))*(inputValue/getGainValue()));
  };      

  /** apply error due to canceling a calibration to current error
    */
  float cancelCalibrationError(float outputValue, float outputError) const {
    return (float) (outputValue * getGainError() + getGainValue() * outputError);
  };      

  /** check if a valid gain calibration is available
    * @return true if the gain value is a valid number  
    */
  bool calibrationValid() const {
    return (!std::isnan(getGainValue()) && (getGainValue()>0));
  };

  /** no cut for the moment
    * @return true, as no cut on cell level is performed after applying the gain calibration 
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
    return "GainConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:GainValue,f:GainError";
  };  
    
};

}

#endif  
