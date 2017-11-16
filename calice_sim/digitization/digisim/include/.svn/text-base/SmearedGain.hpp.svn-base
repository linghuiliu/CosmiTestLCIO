
#ifndef _SMEAREDGAIN_HPP_
#define _SMEAREDGAIN_HPP_

#include "FunctionModifier.hpp"
#include <vector>
#include <string>

namespace digisim {

class SmearedGain : public FunctionModifier {

public:
  //----------methods-------------------
  /** Returns a reference to a new instance */
  SmearedGain* newInstance(const std::string& modName) {
    return new SmearedGain(modName);
    }

  /** default constructor is only called for global instance */
  SmearedGain();

  //bool registerMe();

  /** Smeared linear transformations on time */
  double transformEnergy(const TempCalHit& hit) const;

private:
  //-------------methods-----------------
  // constructor
  SmearedGain(const std::string& modName)
    : FunctionModifier(modName) { }

  // copy constructor, not to be defined
  SmearedGain(const SmearedGain& rhs);


  //-----------data members-------------

  /** Global instance, used only for registration */
  //SmearedTiming _me = new SmearedTiming();

  };
 }  //end namespace digisim
#endif 
