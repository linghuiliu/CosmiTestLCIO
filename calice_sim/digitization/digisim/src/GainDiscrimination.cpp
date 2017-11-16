//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: GainDiscrimination.cpp
// Module: DigiSim
//
// Purpose: A modifier for basic gain+threshold transformation
//
// 20041122 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "GainDiscrimination.hpp"
#include "CLHEP/Random/RandGauss.h"
#include "TempCalHit.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::hex;
using std::dec;

namespace digisim {

  GainDiscrimination::GainDiscrimination() : CalHitModifier() {
    registerModifier("GainDiscrimination", this);
  }

  void GainDiscrimination::init(vector<float>& floats) {
    _par = floats;
    if(_debug>0) {
      for(unsigned int i=0; i<_par.size(); ++i) {
	cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
      }
    }
  }

  // this is where real work is performed
  // hitmap is both input and output
  void GainDiscrimination::processHits(TempCalHitMap& hitmap) {

    double adc;

    // apply gain
    for(TempCalHitMap::iterator it=hitmap.begin(); it!=hitmap.end(); ++it ) {
      TempCalHit& ihit = dynamic_cast<TempCalHit&>(it->second);
      adc = energyToADC( ihit.getTotalEnergy() );
      ihit.setEnergy( adc );
    }

    // apply threshold
    vector<int> toBeErased;
    for(TempCalHitMap::iterator it=hitmap.begin(); it!=hitmap.end(); ++it ) {
      TempCalHit& ihit = dynamic_cast<TempCalHit&>(it->second);
      adc = ihit.getTotalEnergy();

      if( isBelowThreshold( adc ) ) {
	// Tried to erase here, but got problems.  Docs say that iterator
	// are not safe to use after the erase, and it is used in ++it...
	// 	hitmap.erase(it);  //
	// Thus, for now, just save the keys to erase the elements later
	toBeErased.push_back( it->first );
      }
    }

    for(unsigned int i=0; i<toBeErased.size(); ++i) {
      hitmap.erase( toBeErased[i] );
    }
  }

  // printout
  void GainDiscrimination::print() const {
    cout << "GainDiscrimination::print(): "<< _name
	 <<" - a gain+threshold modifier"<< endl;
    cout << " Parameters:";
    for(unsigned int i=0; i<_par.size(); ++i) {
      cout << ' '<< _par[i];
    }
    cout << endl;
  }

  double GainDiscrimination::energyToADC(const double ene) {
    // assign roles to the parameters
    const double gainNominal = _par[0];
    const double gainWidth = _par[1];

    double gain = gainNominal;
    if(gainWidth>0.0) gain = CLHEP::RandGauss::shoot(gainNominal, gainWidth);

    double adc = ene * gain;
    return adc;
  }

  bool GainDiscrimination::isBelowThreshold(const double adc) {
    // assign roles to the parameters
    const double threshNominal = _par[2];
    const double threshWidth = _par[3];

    double threshold = threshNominal;
    if(threshWidth>0.0) threshold = CLHEP::RandGauss::shoot(threshNominal, threshWidth);

    if(adc<threshold) return true;
    else return false;
  }

} // end namespace
