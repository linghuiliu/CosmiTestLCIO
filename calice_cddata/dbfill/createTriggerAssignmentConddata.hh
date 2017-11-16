#ifndef _createTriggerAssignmentConddata_hh_
#define _createTriggerAssignmentConddata_hh_

#include <string>

namespace EVENT { class LCCollection; }
namespace UTIL  { class LCTime; }

EVENT::LCCollection *createTriggerAssignmentConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &input_file);

#endif
