#ifndef _createEcalModuleLocationConddata_hh_
#define _createEcalModuleLocationConddata_hh_

#include <string>

namespace EVENT { class LCCollection; }
namespace UTIL  { class LCTime; }

EVENT::LCCollection *createEcalModuleLocationConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &geometry_file);

#endif
