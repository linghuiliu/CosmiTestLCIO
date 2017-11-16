//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CrossTalkModifier.hpp
// Module: CalDigi
//
// Purpose: A modifier for basic crosstalk simulation
//
// 20050307 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _CROSSTALKMODIFIER_HPP_
#define _CROSSTALKMODIFIER_HPP_

#include "CalHitModifier.hpp"
#include <vector>

namespace digisim {

class CrossTalkModifier : public CalHitModifier {

public: // methods
  CrossTalkModifier* newInstance(const std::string& modName) {
    return new CrossTalkModifier(modName);
  }
  // default constructor is only called for global instance
  CrossTalkModifier();
  // destructor
  ~CrossTalkModifier() {};

  //*** Interface to be defined in modifiers ***

  // initialize parameters
  void init(std::vector<float>& floats);

  // this is where real work is performed
  // hitmap is both input and output
  void processHits(TempCalHitMap& hitmap);

  // default printout
  void print() const;

private:
  // constructors
  CrossTalkModifier(const std::string& modName) : CalHitModifier(modName) {};
  // hidden constructor, not to be defined
  CrossTalkModifier(const CrossTalkModifier& rhs);

  // transformation functions
//   double energyToADC(const double ene);
//   bool isBelowThreshold(const double adc);

private: // data
  // gain and threshold parameters
  std::vector<float> _par;
};

}
#endif  //  _CROSSTALKMODIFIER_HPP_
