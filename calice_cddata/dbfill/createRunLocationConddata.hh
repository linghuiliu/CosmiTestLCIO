#ifndef _createRunLocationConddata_hh_
#define _createRunLocationConddata_hh_

#include <string>

namespace EVENT { class LCCollection; }
namespace UTIL  { class LCTime; }

EVENT::LCCollection *createRunLocationConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &geometry_file);

#endif
