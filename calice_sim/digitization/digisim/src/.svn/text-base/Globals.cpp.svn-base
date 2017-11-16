//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File:   Globals.cpp
// Module: daqsim
//
// Purpose:  Force correct instanciation order of static/global symbols.
//           The map must be instanciated before any available modifiers
//
// 2004-12-01 - G.Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "Globals.hpp"
#include "CalHitModifier.hpp"
#include "GainDiscrimination.hpp"
#include "SmearedGain.hpp"
#include "SmearedTiming.hpp"
#include "AbsValueDiscrimination.hpp"
#include "SiPMSaturation.hpp"
// #include "RandomNoiseModifier.hpp"
#include "TcmtGangingModifier.hpp"
#include "TcmtCrosstalk.hpp"
#include <string>
using std::string;
#include <map>
using std::map;

namespace digisim {

  // static map of available modifiers
  map<string,CalHitModifier*> CalHitModifier::_modifiersAvailable;

  // global instance of all available modifiers
  GainDiscrimination      globalGainDiscrimination;
  SmearedGain             globalSmearedGain;
  SmearedTiming           globalSmearedTiming;
  AbsValueDiscrimination  globalAbsValueDiscrimination;
  SiPMSaturation          globalSiPMSaturation;
//   RandomNoiseModifier     globalRandomNoiseModifier;
  TcmtGangingModifier     globalTcmtGangingModifier;
  TcmtCrosstalk           globalCrosstalk;

} // end namespace
