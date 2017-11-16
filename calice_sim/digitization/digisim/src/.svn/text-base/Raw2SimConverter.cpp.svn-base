
#include "Raw2SimConverter.hpp"
#include "EVENT/LCRelation.h"
#include <cmath>
#include <cassert>
#include <iostream>
using IMPL::SimCalorimeterHitImpl;
using namespace std;
using namespace EVENT;
using namespace digisim;

Raw2SimConverter::~Raw2SimConverter() {
  this->reset();
}

void Raw2SimConverter::process( const LCCollection* links ) {

  map<const RawCalorimeterHit*, SimCalorimeterHitImpl*> hitmap;

  // loop over links
  for(int i=0; i<links->getNumberOfElements(); ++i) {
    // get link info
    const LCRelation* rel = (LCRelation*)links->getElementAt(i);
    const RawCalorimeterHit* rawhit = (const RawCalorimeterHit*)rel->getFrom();
    const SimCalorimeterHit* simhit = (const SimCalorimeterHit*)rel->getTo();
    double weight = rel->getWeight();

    SimCalorimeterHitImpl* newhit = hitmap[rawhit];
    if(newhit==NULL) {
      newhit = new SimCalorimeterHitImpl();
      newhit->setCellID0( rawhit->getCellID0() );
      newhit->setCellID1( rawhit->getCellID1() );
//       newhit->setPosition( rawhit->getPosition() );
      // insert new hit into hitmap
      hitmap[rawhit] = newhit;
    }
    double rawHitEne = _energyFactor * rawhit->getAmplitude();

    if(simhit!=NULL) {
      // a SimCalHit contributing to this RawCalHit (either
      // cell itself or a neighbor via crosstalk).
      // The energy contribution from this cell (weight*rawHitEne)
      // will be shared between the MCParticles according to their
      // contributions in the original SimCalHit.
      double eSimHit = simhit->getEnergy();

      // loop over contribs to simhit, adding contribs to rawHit
      int nmc = simhit->getNMCContributions();
      for(int imc=0; imc<nmc; ++imc) {
	MCParticle* imcp = simhit->getParticleCont(imc);
	double imcEne = simhit->getEnergyCont(imc);
	double imcTime = simhit->getTimeCont(imc);

	// add this MC contribution to the SimCalHit
	double eneContr = (imcEne/eSimHit) * weight * rawHitEne;
	newhit->addMCParticleContribution(imcp, eneContr, imcTime, 0);

//  	if(nmc>1) {
// 	  if(imc==0) {
// 	    cout<<"*** new confusion rawhit: nmc="<<nmc
// 		<<", E="<<rawhit->getAmplitude()
// 		<<", Time="<<rawhit->getTimeStamp()
// 		<<", simE="<<simhit->getEnergy()*1e8
// 		<<endl;
// 	  }
//  	  cout<<"nmc>1! mcp="<<imcp->getPDG()<<", Econtr="<<imcEne*1e8<<endl;
//  	}
      }
    }
    else {
      // no valid simhit: noise
      double noiseE = weight * rawHitEne;
      double noiseT = _timeFactor * rawhit->getTimeStamp();
      newhit->addMCParticleContribution( 0, noiseE, noiseT, 0 );
    }
  }

  // prepare output
  this->reset();
  for(HitMap::const_iterator iter=hitmap.begin(); iter!=hitmap.end(); ++iter) {
    _outSimHits.push_back( iter->second );
  }
}

std::vector<const IMPL::SimCalorimeterHitImpl*>& Raw2SimConverter::getSimCalorimeterHits()
{
  return _outSimHits;
}

void Raw2SimConverter::reset() {
  // elements will be appended to the event, ownership is transfered
  _outSimHits.clear();
}
