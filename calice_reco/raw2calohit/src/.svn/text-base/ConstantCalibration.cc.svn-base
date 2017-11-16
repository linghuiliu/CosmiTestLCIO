#include "ConstantCalibration.hh"
#include "CalibrationKit.hh"
#include "CalibrationFactory.hh"
#include <iostream>
#include <cstdlib>

/** Create a calibration object which just returns the uncalibrated values.
 * The collection name of the calibration constant collection
 * is abused to pass the scale paramer to the 
 * ConstantCalibration object.
 * @sa ConstantCalibration.
 */
class ConstantCalibrationKit : public CalibrationKit
{
protected:
  ConstantCalibrationKit() {
    CalibrationFactory *instance=CalibrationFactory::getInstance();
    instance->registerCalibrationKit("ConstantCalibration",this);
  };

  static ConstantCalibrationKit __instance;

public:
  /** Create a constant calibration object.
   * @param module_type_col_name not used.
   * @param module_calibration_col_name converted to a float value and passed as scale to the ConstantCalibration object.
   */
  Calibration *create(const std::string &module_type_col_name,  const std::string &module_calibration_col_name) const {
    Float_t a_scale=static_cast<Float_t>(strtod(module_calibration_col_name.c_str(),0));
    std::cout << "CalibrationKit::create> Create ConstantCalibration (scale=" << a_scale << " ." << std::endl;
    if (a_scale<=0.) {
      throw std::runtime_error("ConstantCalibrationKit::create> The collection name for the calibration constant"
			       " is missused ass the scaling value. The value is not a positive number."
			       " Please set the collection name of the calibration constant collection to a"
			       " positve float value.");
    }
    return new ConstantCalibration(a_scale);
  };
};

ConstantCalibrationKit ConstantCalibrationKit::__instance;

