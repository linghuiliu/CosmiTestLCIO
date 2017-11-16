#include "NoOpCalibration.hh"
#include "CalibrationKit.hh"
#include "CalibrationFactory.hh"
#include <iostream>

/** Create a calibration object which just returns the uncalibrated values.
 * @sa NoOpCalibration.
 */
class NoOpCalibrationKit : public CalibrationKit
{
protected:
  NoOpCalibrationKit() {
    CalibrationFactory *instance=CalibrationFactory::getInstance();
    instance->registerCalibrationKit("NoOpCalibration",this);
  };

  static NoOpCalibrationKit __instance;

public:
  Calibration *create(const std::string &module_type_col_name,  const std::string &module_calibration_col_name) const {
    std::cout << "CalibrationKit::create> Create NoOpCalibration." << std::endl;
    return new NoOpCalibration;
  };
};

NoOpCalibrationKit NoOpCalibrationKit::__instance;

