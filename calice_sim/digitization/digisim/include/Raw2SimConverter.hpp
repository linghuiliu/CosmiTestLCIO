#ifndef _RAW2SIMCONVERTER_HPP_
#define _RAW2SIMCONVERTER_HPP_

#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/RawCalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "IMPL/SimCalorimeterHitImpl.h"
// #include "HitContrib.hpp"
#include <map>

namespace digisim {

/**
 * Utility class to handle the conversion of RawCalorimeterHits to
 * SimCalorimeterHits
 *
 * @author Guilherme Lima
 */

class Raw2SimConverter {

//   typedef std::map<const EVENT::MCParticle*, HitContrib*> ContribMap;
  typedef std::map<const EVENT::RawCalorimeterHit*, IMPL::SimCalorimeterHitImpl*> HitMap;

public: // methods

  // constructor
//   Raw2SimConverter(const EVENT::RawCalorimeterHit* rawhit);
  Raw2SimConverter(double efac, double tfac) {
    _energyFactor = efac;
    _timeFactor = tfac;
  }

  // destructor
  ~Raw2SimConverter();

  void addContribution(const EVENT::MCParticle* mcp, double energy, double time);

  IMPL::SimCalorimeterHitImpl* convertToSimCalorimeterHit();

  void print();

  void process( const EVENT::LCCollection* links );

  std::vector<const IMPL::SimCalorimeterHitImpl*>& getSimCalorimeterHits();

  void reset();

private: // data
//   // converted raw hit
//   const EVENT::RawCalorimeterHit* _rawhit;

//   // MCParticle contributions
//   ContribMap _contribs;

  // conversion factors
  double _energyFactor;
  double _timeFactor;

  std::vector<const IMPL::SimCalorimeterHitImpl*> _outSimHits;
};

} // end ns digisim

#endif // _RAW2SIMCONVERTER_HPP_
