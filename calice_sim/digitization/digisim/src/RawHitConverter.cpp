
#include "RawHitConverter.hpp"
#include "EVENT/LCRelation.h"
#include "EVENT/LCIO.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <cstdlib>

using IMPL::CalorimeterHitImpl;
using namespace std;
using namespace EVENT;
using namespace digisim;


RawHitConverter::~RawHitConverter() {
  this->reset();
}

void RawHitConverter::process( const LCCollection* rawhits ) {

  this->reset();

  // loop over raw hits
  for(int i=0; i<rawhits->getNumberOfElements(); ++i) {
    const RawCalorimeterHit* rawhit = (const RawCalorimeterHit*)rawhits->getElementAt(i);

    // calibration parameters
    double energy = _energyFactor * ((double)rawhit->getAmplitude());
    double time = _timeFactor * ((double)rawhit->getTimeStamp());

    CalorimeterHitImpl* calhit = new CalorimeterHitImpl();
    calhit->setCellID0( rawhit->getCellID0() );
    calhit->setCellID1( rawhit->getCellID1() );
    calhit->setEnergy( energy );
    calhit->setTime( time );
//    calhit->setPosition(); // can do without it?
    calhit->setType(-1); // what to set this to?
    calhit->setRawHit( const_cast<RawCalorimeterHit*>(rawhit) );

    _outCalHits.push_back( calhit );
  }
}

void RawHitConverter::process( const LCCollection* rawhits, LCCollection* relcol ) {

  this->reset();
  if( relcol->getTypeName() != LCIO::LCRELATION ) {
    cout << " collection not of type " << LCIO::LCRELATION << endl ;
    exit(0) ;
  }
  assert( relcol->getNumberOfElements() == rawhits->getNumberOfElements() );
  //std::cout << " Number of elements : " << relcol->getNumberOfElements() << std::endl;

  // loop over raw hits
  for(int i=0; i<rawhits->getNumberOfElements(); ++i) {

    const RawCalorimeterHit* rawhit = (const RawCalorimeterHit*)rawhits->getElementAt(i);
    const LCRelation* rel = (const LCRelation*)relcol->getElementAt(i) ;

    //std::cout << "--> Element : " << i << " ID= " << rawhit->getCellID0() << std::endl;

    float pos[3] = {0,0,0};
    if (rel->getFrom() == NULL) {
      std::cout << " ERROR ! Object has no FROM object! Exiting..." << std::endl;
      exit(0);
    }
    else if (((RawCalorimeterHit*)rel->getFrom())->getCellID0() == rawhit->getCellID0()){
      if ((SimCalorimeterHit*)rel->getTo() != NULL){
      pos[0] = ((SimCalorimeterHit*)rel->getTo())->getPosition()[0];
      pos[1] = ((SimCalorimeterHit*)rel->getTo())->getPosition()[1];
      pos[2] = ((SimCalorimeterHit*)rel->getTo())->getPosition()[2];
      }
      else {
	//cout << " No Sim hits associated !!! But ID = " << rawhit->getCellID0() << endl;
	pos[0] = 0;
	pos[1] = 0;
	pos[2] = 0;
      }
    }
    else {
      std::cout << " ERROR ! Raw Object not in LCRelation input collection ! ID= " << ((RawCalorimeterHit*)rel->getFrom())->getCellID0() <<
	". Exiting.... " << std::endl ;
      exit(0) ;
    }

    // calibration parameters
    double energy = _energyFactor * ((double)rawhit->getAmplitude());
    double time = _timeFactor * ((double)rawhit->getTimeStamp());

    CalorimeterHitImpl* calhit = new CalorimeterHitImpl();
    calhit->setCellID0( rawhit->getCellID0() );
    calhit->setCellID1( rawhit->getCellID1() );
    calhit->setEnergy( energy );
    calhit->setTime( time );
    calhit->setPosition(pos); // can do without it?
    calhit->setType(-1); // what to set this to?
    calhit->setRawHit( const_cast<RawCalorimeterHit*>(rawhit) );

    _outCalHits.push_back( calhit );
  }
}//process method


std::vector<const IMPL::CalorimeterHitImpl*>& RawHitConverter::getCalorimeterHits()
{
  return _outCalHits;
}

void RawHitConverter::reset() {
  _outCalHits.clear();
}
