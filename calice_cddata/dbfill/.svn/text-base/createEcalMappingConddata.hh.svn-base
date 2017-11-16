#ifndef _createEcalMappingConddata_hh_
#define _createEcalMappingConddata_hh_

#include <string>
#include <climits>

namespace UTIL { class LCTime; }
namespace EVENT { class LCCollection; }

EVENT::LCCollection *createEcalMappingConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &map_file, unsigned int crate_nr);

inline EVENT::LCCollection *createEcalMappingConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &map_file) {
  return createEcalMappingConddata(since,till,map_file,UINT_MAX);
}

#endif
