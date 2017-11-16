//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: FunctionModifier.hpp
// Module: CalDigi
//
// Purpose: A modifier for a basic function-based transformation
//
// 20041129 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _FUNCTIONMODIFIER_HPP_
#define _FUNCTIONMODIFIER_HPP_

#include "CalHitModifier.hpp"
#include <vector>

namespace digisim {

class FunctionModifier : public CalHitModifier {

public: // methods
  // default constructor
  FunctionModifier() : CalHitModifier() {};
  // destructor
  ~FunctionModifier() {};

  //*** Interface to be defined in modifiers ***

  // initialize parameters
  virtual void init(std::vector<float>& floats);

  // this is where real work is performed
  // hitmap is both input and output
  virtual void processHits(TempCalHitMap& hitmap);

  // default printout
  virtual void print() const;

protected:
  virtual double transformEnergy(const TempCalHit& hit) const = 0;

  // hidden constructor
  FunctionModifier(const std::string& modName)
    : CalHitModifier(modName) { }

  // hidden copy constructor, not to be defined
  FunctionModifier(const FunctionModifier& rhs);

protected: // data
  // function parameters
  std::vector<float> _par;
};

}
#endif  //  _FUNCTIONMODIFIER_HPP_
