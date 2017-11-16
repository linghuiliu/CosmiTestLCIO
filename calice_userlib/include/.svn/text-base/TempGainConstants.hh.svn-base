#ifndef TempGainConstants_h
#define TempGainConstants_h 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <GainConstants.hh>

using namespace lcio;

namespace CALICE {

class TempGainConstants;

/** = class GainConstants but with temperature
  * @author S. Schaetzel DESY
  * @date Apr 26 2007
  */

class TempGainConstants: public GainConstants {

private:
  float cellTemp, cellTempError, corrGain, corrGainError;

public:
  
  /** constructor
    */
  TempGainConstants() {};
  
  /** constructor, initialised by parameters
    * @param chip chip number of the channel calibrated
    * @param channel channel number of the channel calibrated
    * @param value gain value of the cannel calibrated
    * @param error error on the gain value of the cannel calibrated
    * @param gainTemp temperature for which the gain was determined
    * @param gainTempError uncertainty of temperature for which the gain was determined
    * @param Tslope temperature slope of gain (dG/dT) in ADC ch./K
    * @param TslopeError uncertainty of temperature slope of gain (dG/dT) in ADC ch./K
    */  
  TempGainConstants(unsigned chip, unsigned channel, float value, float error,
		    float gainTemp, float gainTempError, float Tslope, float TslopeError):
 	GainConstants(chip,channel,value,error) {
    obj()->setFloatVal(2, gainTemp);
    obj()->setFloatVal(3, gainTempError);
    obj()->setFloatVal(4, Tslope);
    obj()->setFloatVal(5, TslopeError);
    cellTemp=0.;
    cellTempError=0.;
    corrGain=0.;
    corrGainError=0.;
  };
  
  TempGainConstants(LCObject* obj): GainConstants(obj) {};
  
  virtual ~TempGainConstants() {};

  /** set cell temperature
    */
  void setCellTemp(float tempValue, float tempError) {
    cellTemp=tempValue;
    cellTempError=tempError;

    corrGain=getGainValue()+(cellTemp-getGainTemp())*getTslope();
    corrGainError=sqrt(pow(getTslope()*cellTempError,2)+pow(getTslope()*getGainTempError(),2)+
		       pow(getGainError(),2)+pow((cellTemp-getGainTemp())*getTslopeError(),2));
    return;
  }

  /** get temperature-corrected gain
    */
  float getCorrGain() const {
    return corrGain;
  };  

  /** get uncertainty on temperature-corrected gain
    */
  float getCorrGainError() const {
    return corrGainError;
  };  

  /** get cell temperature
    */
  float getCellTemp() const {
    return cellTemp;
  };  

  /** get cell temperature uncertainty
    */
  float getCellTempError() const {
    return cellTempError;
  };  

  /** set gain temperature
    */
  void setGainTemp(float value, float error) {
    obj()->setFloatVal(2, value);
    obj()->setFloatVal(3, error);
    return;
  }

  /** get gain temperature
    */
  float getGainTemp() const {
    return getFloatVal(2);
  };  

  /** get gain temperature uncertainty
    */
  float getGainTempError() const {
    return getFloatVal(3);
  };  

  /** set temperature slope
    */
  void setTslope(float value, float error) {
    obj()->setFloatVal(4, value);
    obj()->setFloatVal(5, error);
    return;
  };  

  /** get temperature slope
    */
  float getTslope() const {
    return getFloatVal(4);
  };  

  /** get temperature slope uncertainty
    */
  float getTslopeError() const {
    return getFloatVal(5);
  };  

  /** apply gain calibration to input value
   * @param inputValue SiPM amplitude in ADC ch. 
   * @return SiPM amplitude in pixels
   */
  float applyCalibration(float inputValue) const {
    return inputValue/corrGain;
  };
  
  /** cancel an applied calibration from energy value
    */
  float cancelCalibration(float outputValue) const {
    return outputValue*corrGain;
  };  

  /** apply calibration error to current error
   * @param inputValue SiPM amplitude in ADC ch. 
   * @param inputError SiPM amplitude error in ADC ch.
   * @return error of SiPM amplitude in pixels
   */
  float applyCalibrationError(float inputValue, float inputError) const {
    return (float) (sqrt(pow(inputError/inputValue,2)+pow(corrGainError/corrGain,2))*applyCalibration(inputValue));

  };

  /** apply error due to canceling a calibration to current error
    */
  float cancelCalibrationError(float outputValue, float outputError) const {
    return (float) (outputValue * corrGainError + corrGain * outputError);
  };      


  bool calibrationValid() const {
    return (GainConstants::calibrationValid() && (corrGain>0));
  };

  /** convenient print method
    */
  void print (std::ostream& os);

  /** return type of the class
    */
  const std::string getTypeName() const {
    return "TempGainConstants";
  };

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:GainValue,f:GainError,f:gainTemp,f:gainTempError,f:Tslope,f:TslopeError";
  };

      
};

}

#endif  
