//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CrossTalkModifier.cpp
// Module: DigiSim
//
// Purpose: A modifier for simple crosstalk simulation
//
// 20050307 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "CrossTalkModifier.hpp"
#include "CLHEP/Random/RandGauss.h"
#include "TempCalHit.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace digisim {

//*** Important:
//   Please add a global instance for this modifier in Globals.cpp

CrossTalkModifier::CrossTalkModifier() : CalHitModifier() {
  registerModifier("CrossTalkModifier", this);
}

void CrossTalkModifier::init(vector<float>& floats) {
  _par = floats;
  if(_debug>0) {
    for(unsigned int i=0; i<_par.size(); ++i) {
      cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
    }
  }

  // ??? we need to load here the number of cells per layer, for quick
  // determination of neighbors
}

// this is where real work is performed
// hitmap is both input and output
void CrossTalkModifier::processHits(TempCalHitMap& hitmap) {

  // loop over temp hits to look for neighbors
//   for(TempCalHitMap::iterator it=hitmap.begin(); it!=hitmap.end(); ++it ) {
//     int cellid = it->first;
// //     TempCalHit& ihit = dynamic_cast<TempCalHit&>(it->second);
//     TempCalHit& ihit = it->second;

//     // only same-layer neighbors are needed
// //     itheta =


//   }

}

// printout
void CrossTalkModifier::print() const {
  cout << "CrossTalkModifier::print(): "<< _name
       <<" - a simple crosstalk modifier"<< endl;
  cout << " Parameters:";
  for(unsigned int i=0; i<_par.size(); ++i) {
    cout << ' '<< _par[i];
  }
  cout << endl;
}

// double CrossTalkModifier::energyToADC(const double ene) {
//   // assign roles to the parameters
//   const double gainNominal = _par[0];
//   const double gainWidth = _par[1];

//   double gain = gainNominal;
//   if(gainWidth>0.0) gain = RandGauss::shoot(gainNominal, gainWidth);
// //     if(_debug>0) cout << " gain = " << gain << endl;

//   double adc = ene * gain;
//   return adc;
// }

// bool CrossTalkModifier::isBelowThreshold(const double adc) {
//   // assign roles to the parameters
//   const double threshNominal = _par[2];
//   const double threshWidth = _par[3];

//   double threshold = threshNominal;
//   if(threshWidth>0.0) threshold=RandGauss::shoot(threshNominal, threshWidth);

//   if(adc<threshold) return true;
//   else return false;
// }

} // end namespace
