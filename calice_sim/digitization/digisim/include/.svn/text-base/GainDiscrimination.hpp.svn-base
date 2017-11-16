//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: GainDiscrimination.hpp
// Module: CalDigi
//
// Purpose: A modifier for basic gain+threshold transformation
//
// 20041122 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _GAINDISCRIMINATION_HPP_
#define _GAINDISCRIMINATION_HPP_

#include "CalHitModifier.hpp"
#include <vector>

namespace digisim {

class GainDiscrimination : public CalHitModifier {

public: // methods
  GainDiscrimination* newInstance(const std::string& modName) {
    return new GainDiscrimination(modName);
  }
  // default constructor is only called for global instance
  GainDiscrimination();
  // destructor
  ~GainDiscrimination() {};

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
  GainDiscrimination(const std::string& modName) : CalHitModifier(modName) {};
  // hidden constructor, not to be defined
  GainDiscrimination(const GainDiscrimination& rhs);

  // transformation functions
  double energyToADC(const double ene);
  bool isBelowThreshold(const double adc);

private: // data
  // gain and threshold parameters
  std::vector<float> _par;
};

}
#endif  //  _GAINDISCRIMINATION_HPP_
