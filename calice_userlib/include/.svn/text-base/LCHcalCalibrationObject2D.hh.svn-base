#ifndef LCHCALCALIBRATIONOBJECT2D_H
#define LCHCALCALIBRATIONOBJECT2D_H 1

#include "lcio.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "LCHcalCalibrationObject.hh"

#include <iostream>
#include <map>

using namespace lcio ;

namespace CALICE{

  /** The results of the calibrations done by LCHcalCalibrationObject2D 
   * can depend on two sets of input values (like saturation correction) 
   * @author S. Schmidt, DESY
   * @date September 2006
   */

  class LCHcalCalibrationObject2D : public LCHcalCalibrationObject {

  public:
  
    LCHcalCalibrationObject2D() {};
  
    LCHcalCalibrationObject2D(LCObject* obj): LCHcalCalibrationObject(obj) {};
      
    virtual float applyCalibration(float inputValue, float inputParameter) const {
      return 0.;
    };
  
    virtual float applyCalibrationError(float inputValue, float inputError, float inputParameter, float inputParameterError) const {
      return 0.;
    };
    
    virtual float applyCalibration(float inputValue) const {
      return applyCalibration(inputValue, inputValue);
    };
  
    virtual float applyCalibrationError(float inputValue, float inputError) const {
      return applyCalibrationError(inputValue, inputError, inputValue, inputError);
    };    
    
 };

typedef std::map<unsigned,LCHcalCalibrationObject2D*> HcalCalibration2DModuleMap; 
typedef std::pair<lcio::LCCollection*, HcalCalibration2DModuleMap*> HcalCalibration2DModuleData;

} // namespace
#endif /* ifndef LCHCALCALIBRATIONOBJECT2D_H */
