//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: FunctionModifier.cpp
// Module: DigiSim
//
// Purpose: A modifier for a basic function-based transformation
//
// 20041129 - Guilherme Lima - Created
// 2005/08/11 - C DeCaro - Updated
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "FunctionModifier.hpp"
#include "CLHEP/Random/RandGauss.h"
#include "TempCalHit.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace digisim {

  static const string description("a simple, function-based modifier");

  void FunctionModifier::init(vector<float>& floats) {
    _par = floats;
    if(_debug>0) {
      for(unsigned int i=0; i<_par.size(); ++i) {
	cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
      }
    }
  }

  // this is where real work is performed
  // hitmap is both input and output
  void FunctionModifier::processHits(TempCalHitMap& hitmap) {

    double adc;

    for(TempCalHitMap::iterator it=hitmap.begin(); it!=hitmap.end(); ++it ) {
      TempCalHit& ihit = dynamic_cast<TempCalHit&>(it->second);

      // function-based energy transformation
      adc = transformEnergy( ihit );
      ihit.setEnergy( adc );
    }
  }

  // printout
  void FunctionModifier::print() const {
    cout<< "FunctionModifier::print(): "<< _name
	<<" - "<< description <<endl;
    cout<< " Parameters:";
    for(unsigned int i=0; i<_par.size(); ++i) {
      cout << ' '<< _par[i];
    }
    cout << endl;
  }
}
