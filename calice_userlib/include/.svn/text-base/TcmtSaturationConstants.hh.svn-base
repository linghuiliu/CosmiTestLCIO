#ifndef TcmtSaturationConstants_h
#define TcmtSaturationConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject.hh>

using namespace lcio;

namespace CALICE {

class TcmtSaturationConstants;

/**
  * Class to store the estimates for the Cpix parameters for saturation corrections of TCMT hits.
  * The correction factor Cpix = f(pedSubADC) = 1/(1-pedSubADC*Cpix).  For more details, see
  *   http://...
  * @author G.Lima, K.Francis (NIU)
  * @date March 15 2008
  */

class TcmtSaturationConstants: public LCHcalCalibrationObject{

public:
  
  TcmtSaturationConstants() {};
  
  TcmtSaturationConstants(unsigned chip, unsigned channel, float value, float error) {
    unsigned _cellId = (chip << 8) + channel;
    setCellKey(_cellId);
    obj()->setFloatVal(0, value);
    obj()->setFloatVal(1, error);
  };
  
  TcmtSaturationConstants(LCObject* obj): LCHcalCalibrationObject(obj) {};
  
  virtual ~TcmtSaturationConstants() {};

  /** get conversion factor value 
    */
  float getCpixValue() const {
    return getFloatVal(0);
  };  

  /** get conversion factor error
    */
  float getCpixError() const {  
    return getFloatVal(1);
  };
  
  /** apply calibration to energy value 
    */
  float applyCalibration(float inputValue) const {
    double cpix = getCpixValue();
    float corrFactor = 1.0 / (1.0 - inputValue * cpix );
    return (float) (inputValue * corrFactor);
  };
  
  /** cancel an applied calibration from energy value
    */
  float cancelCalibration(float outputValue) const {
    double cpix = getCpixValue();
    float cancelFactor = 1.0 / (1.0 + outputValue * cpix );
    return (float) (outputValue * cancelFactor);
  };
  
  /** apply calibration error to current error
    */
  float applyCalibrationError(float inputValue, float inputError) const {
    // z = x/(1-xy) ==> dZ = z^2 * sqrt[dx^2/x^4 + dy^2]
    double x = inputValue;
    double dx = inputError;
    double dy = getCpixError();
    double z = applyCalibration(inputValue);
    double dz = pow(z,2) * sqrt(pow(dx/(x*x),2) + pow(dy,2));
    return (float)dz;
  };

  /** apply error due to canceling a calibration to current error
    */
  float cancelCalibrationError(float outputValue, float outputError) const {
    // x = z/(1+zy) ==> dx = x^2 * sqrt[dz^2/z^4 + dy^2]
    double z = outputValue;
    double dz = outputError;
    double dy = getCpixError();
    double x = cancelCalibration(outputValue);
    double dx = pow(x,2) * sqrt(pow(dz/(z*z),2) + pow(dy,2));
    return (float)dx;
  };

  /** check if a valid calibration is available
    */
  bool calibrationValid() const {
    return (!std::isnan(getCpixValue()) && (getCpixValue()>0));
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
    return "TcmtSaturationConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,f:CpixValue,f:CpixError";
  };  
    
};

}

#endif  
