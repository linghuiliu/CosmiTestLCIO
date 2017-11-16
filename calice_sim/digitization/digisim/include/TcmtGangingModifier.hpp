//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: TcmtGangingModifier.hpp
// Module: CalDigi
//
// Purpose: A modifier for ganging of MC tail catcher hits
//
// 20061106 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _TCMTGANGINGMODIFIER_HPP_
#define _TCMTGANGINGMODIFIER_HPP_

#include "CalHitModifier.hpp"
#include <vector>

namespace digisim {

class TcmtGangingModifier : public CalHitModifier {

public: // methods
  TcmtGangingModifier* newInstance(const std::string& modName) {
    return new TcmtGangingModifier(modName);
  }
  // default constructor is only called for global instance
  TcmtGangingModifier();
  // destructor
  ~TcmtGangingModifier() {};

  //*** Interface to be defined in modifiers ***

  // initialize parameters
  void init(std::vector<float>& floats);

  // this is where real work is performed
  // hitmap is both input and output
  void processHits(TempCalHitMap& hitmap);

  // default printout
  void print() const;

  /** Called after data processing for clean up.
   */
  //void end() ;


private:
  // constructors
  TcmtGangingModifier(const std::string& modName) : CalHitModifier(modName) {};

  // hidden constructor, not to be defined
  TcmtGangingModifier(const TcmtGangingModifier& rhs);

private: // data
  // gain and threshold parameters
  std::vector<float> _par;
};

}//namespace digisim

#endif  //  _TCMTGANGINGMODIFIER_HPP_
