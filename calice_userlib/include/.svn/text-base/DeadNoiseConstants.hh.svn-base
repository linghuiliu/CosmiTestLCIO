#ifndef DeadNoiseConstants_h
#define DeadNoiseConstants_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include "lcio.h"
#include <LCHcalCalibrationObject.hh>

using namespace lcio;

namespace CALICE {

class DeadNoiseConstants;

/**
  * Class to store the results of a selection of dead or noisy cells
  * for a certain chip/channel for a fixed moduleID
  * @author S. Schmidt DESY
  * @date Sep 25 2006
  */

class DeadNoiseConstants: public LCHcalCalibrationObject{

public:
  
  /** default constructor
    */
  DeadNoiseConstants() {};
  
  /** constructor, initializing with parameters
    * @param chip chip number of cell to be calibrated
    * @param channel channel number of cell to be calibrated
    * @param deadLevel "level" of being dead for a cell, 0: not dead, 1: dead
    * @param noiseLevel "level" of being noisy for a cell, 0: no noise, 1: low noise, 2: high noise
    */
  DeadNoiseConstants(unsigned chip, unsigned channel, int deadLevel, int noiseLevel) {
    unsigned _cellId = (chip << 8) + channel;
    setCellKey(_cellId);
    obj()->setIntVal(1, deadLevel);
    obj()->setIntVal(2, noiseLevel);
  };
  
  DeadNoiseConstants(LCObject* obj): LCHcalCalibrationObject(obj) {};
  
  virtual ~DeadNoiseConstants() {};

  /** get cell dead level 
    */
  int getDeadLevel() const {
    return getIntVal(1);
  };  

  /** get cell noise level
    */
  float getNoiseLevel() const {  
    return getIntVal(2);
  };
  
  /** apply calibration to energy value
    * @param inputValue "energy" value before the calibration 
    * @return as the selection of non-dead/non-noisy cells has no impact on the "energy" the 
    *    inputValue is returned
    */
  float applyCalibration(float inputValue) const {
    return (float) inputValue;
  };
  
  /** apply calibration error to current error
    * @param inputValue "energy" value before the calibration
    * @param inputError error of the "energy" value before the calibration
    * @return as the selection of non-dead/non-noisy cells has no error the inputError is returned
    */
  float applyCalibrationError(float inputValue, float inputError) const {
    return (float) inputError;
  };      

  /** check if a valid calibration is available
    * @return true as the selection of non-dead/non-noisy cells is always valid
    */
  bool calibrationValid() const {
    return true;
  };

  /** accept only non-dead and non-noisy events
    * @param resultValue "energy" value after the calibration
    * @param resultError error of the "energy" value after the calibration
    * @return true if a cell is non-dead and non-noisy
    */
  bool keepEvent(float resultValue, float resultError) const {
    return ((getDeadLevel() == 0) && (getNoiseLevel() == 0));
  };    

  /** convenient print method
    */
  void print (std::ostream& os);
  
  /** return type of the class
    */
  const std::string getTypeName() const {
    return "DeadNoiseConstants";
  };   

  /** return a brief description of the data member
    */
  const std::string getDataDescription() const {
    return "i:CellKey,i:DeadLevel,i:NoiseLevel";
  };  
    
};

}

#endif  
