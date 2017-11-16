//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: TcmtCrosstalk.hpp
// Module: DigiSim
//
// Purpose: A modifier for TCMT-specific crosstalk simulation
//
// 20071213 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _TCMTCROSSTALK_HPP_
#define _TCMTCROSSTALK_HPP_

#include "CalHitModifier.hpp"
#include <vector>

namespace digisim {

class TcmtCrosstalk : public CalHitModifier {

public: // methods
  TcmtCrosstalk* newInstance(const std::string& modName) {
    return new TcmtCrosstalk(modName);
  }
  // default constructor is only called for global instance
  TcmtCrosstalk();
  // destructor
  ~TcmtCrosstalk() {};

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
  TcmtCrosstalk(const std::string& modName) : CalHitModifier(modName) {};
  // hidden constructor, not to be defined
  TcmtCrosstalk(const TcmtCrosstalk& rhs);

private: // data
  // gain and threshold parameters
  std::vector<float> _par;
};

}
#endif  //  _TCMTCROSSTALK_HPP_
