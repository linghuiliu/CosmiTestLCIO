/**
 * A modifier for basic gain transformations
 *
 * @author Guilherme Lima, C.DeCaro
 * @version $Id: SmearedTiming.cpp,v 1.4 2008-02-01 16:10:40 lima Exp $
 */
#include "SmearedTiming.hpp"
#include "TempCalHit.hpp"

#include "CLHEP/Random/RandGauss.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace digisim {

  SmearedTiming::SmearedTiming() : CalHitModifier() {
    registerModifier("SmearedTiming", this);
  }

  void SmearedTiming::init(std::vector<float>& floats) {
    _par = floats;
    if(_debug>0) {
      for(unsigned int i=0; i<_par.size(); ++i) {
	cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
      }
    }
  }

  /*
   * This is where real work is performed.
   * @param hitmap map with transient hits.  Both input and output.
   */
  void SmearedTiming::processHits(std::map<long long, TempCalHit>& hitmap) {
    for(std::map<long long, TempCalHit>::iterator i=hitmap.begin(); i!=hitmap.end(); ++i) {
      TempCalHit ihit = hitmap[i->first];
      double timing = transformTime( ihit );
      ihit.setTime(timing);
    }
  }

  /* Smeared linear transformations on time */
  double SmearedTiming::transformTime(const TempCalHit& hit) {
    //assign roles to the parameters
    const double smearNominal = _par.at(0);
    const double smearWidth = _par.at(1);

    double smear = smearNominal;
    if(smearWidth > 0.0) smear = CLHEP::RandGauss::shoot(smearNominal, smearWidth);

    double newTime = hit.getPrimaryTime() * smear;
    return newTime;
  }

  /* debugging printout */
  void SmearedTiming::print() const{
    cout << _name << " a gain+threshold modifier" << endl;
    if(_par.size() != 0) {
      cout << " Parameters:";
      for(unsigned int i = 0; i < _par.size(); ++i){
	cout << " " << _par.at(i);
      }
      cout << endl;
    }
  }
}
