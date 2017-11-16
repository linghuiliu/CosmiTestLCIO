/**
 * A modifier for basic double-threshold transformations.
 *
 * Double here means that the threshold is taken as an absolute value,
 * so all hits with energies in the range (-trheshold, +threshold) are
 * discarded.  The reason for this is that in some cases
 * (e.g. gaussian noise), negative noise can cancel out positive
 * noise.
 *
 * @author Guilherme Lima, C. DeCaro
 * @version $Id: AbsValueDiscrimination.cpp,v 1.3 2008-02-01 16:10:40 lima Exp $
 */
#include "AbsValueDiscrimination.hpp"
#include "CalHitModifier.hpp"
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <iostream>

#include "CLHEP/Random/RandGauss.h"
using std::cout;
using std::endl;
using std::string;

namespace digisim {

  AbsValueDiscrimination::AbsValueDiscrimination() : CalHitModifier() {
    registerModifier("AbsValueDiscrimination", this);
  }


  void AbsValueDiscrimination::init(std::vector<float>& floats) {
    _par = floats;
    if(_debug > 0) {
      for(unsigned int i=0; i<_par.size(); ++i) {
	cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
      }
    }
  }

  void AbsValueDiscrimination::processHits(std::map<long long, TempCalHit>& hitmap) {
    if(_debug > 0) {
      cout << "name: " << _name << ", debug = " << _debug << endl;
    }

    std::vector<long long> toBeErased;
    for(std::map<long long, TempCalHit>::iterator i=hitmap.begin(); i!=hitmap.end(); ++i) {
      TempCalHit ihit = hitmap[i->first];
      double energy = ihit.getTotalEnergy();

      if(isBetweenThresholds(energy) ) {

	toBeErased.push_back(i->first);
      }
    }

    for(std::vector<long long>::iterator i = toBeErased.begin(); i != toBeErased.end(); i++) {
      hitmap.erase(*i);
    }
  }

  void AbsValueDiscrimination::print() const{
    //cout << _name << " a gain+threshold modifier" << endl;
    if(_par.size() != 0) {
      cout << " Parameters:";
      for(unsigned int i = 0; i < _par.size(); ++i){
	cout << " " << _par.at(i);
      }
      cout << endl;
    }
  }

  bool AbsValueDiscrimination::isBetweenThresholds(double energy) {
    const double threshNominal = _par.at(0);
    const double threshWidth = _par.at(1);

    double threshold = threshNominal;

    if(threshWidth > 0.0) threshold = CLHEP::RandGauss::shoot(threshNominal, threshWidth);

    if(std::fabs(energy) < threshold) return true;
    else return false;

  }
}
