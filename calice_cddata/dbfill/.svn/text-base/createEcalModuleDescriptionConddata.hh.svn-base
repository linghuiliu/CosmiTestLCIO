#ifndef _createEcalModuleDescriptionConddata_hh_
#define _createEcalModuleDescriptionConddata_hh_

namespace UTIL { class LCTime; }
namespace EVENT { class LCCollection; }

EVENT::LCCollection *createEcalModuleDescriptionConddata(UTIL::LCTime &since, UTIL::LCTime &till, bool verbose=false);

EVENT::LCCollection *createEcalModuleDescriptionConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &dummy) {
  return createEcalModuleDescriptionConddata(since,till,false);
}

#endif
