
#ifndef _SIPMSATURATION_HPP_
#define _SIPMSATURATION_HPP_

#include "FunctionModifier.hpp"
#include <vector>
#include <string>
#include <map>

namespace digisim {

class SiPMSaturation : public FunctionModifier {

public:
  //----------methods-------------------

  /** default constructor is only called for global instance */
  SiPMSaturation();

  /** Returns a reference to a new instance */
  SiPMSaturation* newInstance(const std::string& modName) {
    return new SiPMSaturation(modName);
    }

  /** Smeared linear transformations on energy */
  double transformEnergy(const TempCalHit& hit) const;

private:
  //-------------methods-----------------

  // constructor
  SiPMSaturation(const std::string& modName)
    : FunctionModifier(modName) { }

  // copy constructor, not to be defined
  SiPMSaturation(const SiPMSaturation& rhs);

  //-----------data members-------------

  };
 }  //end namespace digisim
#endif
