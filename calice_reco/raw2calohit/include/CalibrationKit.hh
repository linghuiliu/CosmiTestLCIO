#ifndef _CalibrationKit_hh_
#define _CalibrationKit_hh_

#include <string>

//#include <Calibration.hh>
class Calibration;

/** Abstract interface of a calibration kit which can create calibration objects.
 */
class CalibrationKit
{
public:

  virtual ~CalibrationKit() {};

  /** create calibration object.
   * @param module_type_col_name the name of the conditions data collection with the module types.
   * @param module_calibration_col_name the name of the conditions data collection which contains the calibration constants.
   * FIXME: There should be a better method to hand parameters to Calibration objects.
   */
  virtual Calibration *create(const std::string &module_type_col_name,  const std::string &module_calibration_col_name) const = 0;
  
};
#endif
