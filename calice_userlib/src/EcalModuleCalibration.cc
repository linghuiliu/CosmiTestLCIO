#include "EcalModuleCalibration.hh"
#include <string>
#include <iostream>
#include "stringFromToInts.hh"

namespace CALICE {

  EcalModuleCalibration::

  /** UsefulConstructor
   */
  EcalModuleCalibration::EcalModuleCalibration(const std::string &module_type_name, UInt_t module_id, UInt_t n_cells)
    : _createdObject( true )  {
    UInt_t name_ints=getNeededInts(module_type_name);
    // create generic object of variable size
    _obj = new LCGenericObjectImpl;
    // enforce an allocation of the int and float array of the correct size;
    _obj->setIntVal(kNEcalModuleCalibrationInts+name_ints -1,0);

    if (n_cells>0) {
      _obj->setFloatVal(kNEcalModuleCalibrationFloats+n_cells -1, 0.) ;
    }
    _obj->setIntVal(kEcalModuleCalibrationIntModuleID,static_cast<int>(module_id));

    // since strings are not supported in the LCGenericObject, the string is encoded as integers.
    convertStringToInts(_obj,module_type_name,kNEcalModuleCalibrationInts, kEcalModuleCalibrationIntNameLength);
  }

}
  
