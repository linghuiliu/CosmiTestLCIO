#ifndef _RAWHITCONVERTER_HPP_
#define _RAWHITCONVERTER_HPP_

#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/RawCalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/CalorimeterHitImpl.h"
// #include "HitContrib.hpp"
#include <map>

namespace digisim {

/**
 * Utility class to handle the conversion of RawCalorimeterHits to
 * SimCalorimeterHits
 *
 * @author Guilherme Lima
 */

class RawHitConverter {

//   typedef std::map<const EVENT::MCParticle*, HitContrib*> ContribMap;
  typedef std::map<const EVENT::RawCalorimeterHit*, IMPL::CalorimeterHitImpl*> HitMap;

public: // methods

  // constructor
//   RawHitConverter(const EVENT::RawCalorimeterHit* rawhit);
  RawHitConverter(double efac, double tfac) {
    _energyFactor = efac;
    _timeFactor = tfac;
  }

  // destructor
  ~RawHitConverter();

  void addContribution(const EVENT::MCParticle* mcp, double energy, double time);

  IMPL::CalorimeterHitImpl* convertToCalorimeterHit();

  void print();

  // Recommended for CALICE: hit positions will be defined at the rawHit->CaloHit step
  void process( const EVENT::LCCollection* links);

  // Magnan's tests: hit positions will be defined at the rawHit level
  void process( const EVENT::LCCollection* links, EVENT::LCCollection* relcol);

  std::vector<const IMPL::CalorimeterHitImpl*>& getCalorimeterHits();

  void reset();

private: // data
//   // converted raw hit
//   const EVENT::RawCalorimeterHit* _rawhit;

//   // MCParticle contributions
//   ContribMap _contribs;

  // conversion factors
  double _energyFactor;
  double _timeFactor;

  std::vector<const IMPL::CalorimeterHitImpl*> _outCalHits;
};

} // end ns digisim

#endif // _RAWHITCONVERTER_HPP_
