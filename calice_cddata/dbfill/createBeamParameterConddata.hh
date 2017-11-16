#ifndef _createRunLocationConddata_hh_
#define _createRunLocationConddata_hh_

#include <string>

namespace EVENT { class LCCollection; }
namespace UTIL  { class LCTime; }

EVENT::LCCollection *createBeamParameterConddata(UTIL::LCTime &since, UTIL::LCTime &till, const std::string &beamparams_file);

#endif
