#ifndef _createEcalCalibrationConstantsConddata_hh_
#define _createEcalCalibrationConstantsConddata_hh_

#include <string>

namespace EVENT { class LCCollection; }
namespace UTIL  { class LCTime; }

EVENT::LCCollection *createEcalCalibrationConstantsConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::vector<std::string> &fit_result_file_name, 
							    float nominal_mpv,  float min_calibration_constant, float max_calibration_constant,
							    bool verbose);

EVENT::LCCollection *createEcalCalibrationConstantsConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::vector<std::string> &fit_result_file_name) {
  return createEcalCalibrationConstantsConddata(since, till, fit_result_file_name, 1., 0.,1000., true);
}

#endif
