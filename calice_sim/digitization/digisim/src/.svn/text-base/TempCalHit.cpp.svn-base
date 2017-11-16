//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: TempCalHit.cpp
// Module: DigiSim
//
// Purpose: Serve as transition from Sim to Raw hits.  Temp Hits
//     both input and output
// 20041112 - Guilherme Lima - Created
// 2005/08/11  - C DeCaro - Updated
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "TempCalHit.hpp"
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
using std::map;
using std::pair;
using std::vector;
using std::cout;
using std::endl;

namespace digisim {

  typedef map<long long, HitContrib> ContribMap;

  //Default Constructor
  TempCalHit::TempCalHit()  {
    _energy = 0;
    _time = 0;
    _simHits = new ContribMap();
    _rawID = 0;
    //cout<< "Default TempCalHit constructor"<< endl;
  }

  TempCalHit::~TempCalHit() {
     delete _simHits;
  }

  //copy constructor
  TempCalHit::TempCalHit(const TempCalHit& oldHit) {
    this->_rawID = oldHit._rawID;
    this->_simHits = new ContribMap();
    //cout << "_simhits="<< _simHits <<' '<< oldHit._simHits << endl;

    // energy and time will be set within addContribution()
    this->_energy = 0;
    this->_time = 0;
    for (ContribMap::const_iterator i= oldHit._simHits->begin(); i != oldHit._simHits->end(); ++i) {
      const HitContrib& contr = i->second;
      this->addContribution(i->first, contr.energy(), contr.time() );
    }
  }

  //same-cell constructor
  TempCalHit::TempCalHit(long long simid, double energy, double time) {
    _simHits = new ContribMap();
    //cout<< " tempCalHit constructor: _simHit="<< _simHits <<' '<< _simHits->size() << endl;
    _rawID = simid;
    // energy and time will be set within addContribution()
    _energy = 0;
    _time = 0;
    addContribution(simid, energy, time);
    //cout<< " tempCalHit constructor: _simHit="<< _simHits <<' '<< _simHits->size() << endl;
  }

  // non-same cell constructor (e.g. crosstalk)
  TempCalHit::TempCalHit(long long rawid, long long simid, double energy, double time) {
    _energy = 0;
    _time = 0;
    _simHits = new ContribMap();
    _rawID = rawid;
    addContribution(simid, energy, time);
  }

  // assignment operator
  TempCalHit& TempCalHit::operator=(const TempCalHit& rhs) {
    //cout<< "assignment operator called"<< endl;
    this->_rawID = rhs._rawID;
    if(_simHits!=NULL) delete _simHits;
    this->_simHits = new ContribMap();
    //cout << "_simhits="<< _simHits <<' '<< rhs._simHits << endl;

    // energy and time will be set within addContribution()
    this->_energy = 0;
    this->_time = 0;
    for (ContribMap::const_iterator i= rhs._simHits->begin(); i != rhs._simHits->end(); i++) {
      const HitContrib& contr = i->second;
      this->addContribution(i->first, contr.energy(), contr.time() );
    }
    return *this;
  }

  void TempCalHit::scaleEnergy(double factor){
    for(ContribMap::iterator i=_simHits->begin(); i != _simHits->end(); i++) {
      i->second.scale(factor);
    }
    _energy *= factor;
  }

  void TempCalHit::setEnergy(double energy) {
    double factor = energy/_energy;
    this->scaleEnergy(factor);
  }


  //method to get total energy from all contributions
  double TempCalHit::getTotalEnergy() const{
    if(_energy > 0.0) {
      return _energy;
    }
    //if not:
    double totEnergy = 0.0;
    //ContribMap::const_iterator i;
    for(ContribMap::const_iterator i=_simHits->begin(); i != _simHits->end(); i++) {
      totEnergy += i->second.energy();
    }
    return totEnergy;
  }

  double TempCalHit::getPrimaryTime() const{
    if(_time > 0.0) {
      return _time;
    }
    //if not
    double largestContribTime;
    double maxEnergy=0, earliestTime=1.0e+12;
    ContribMap::const_iterator i;
    for(i=_simHits->begin(); i != _simHits->end(); i++) {
      double curTime = i->second.time();
      if(curTime < earliestTime ) earliestTime = curTime;

      double curEnergy = i->second.energy();
      if(curEnergy > maxEnergy) {
	maxEnergy = curEnergy;
	largestContribTime = curTime;
      }
    }
    //choose earliestTime or largestContribTime
    return earliestTime;
  }

  //method to add a contribution from a simulated hit
  void TempCalHit::addContribution(long long id, double energy, double time) {
    if( _simHits->count(id)>0 ) {
      // contribution already exists
      HitContrib& contr = _simHits->find(id)->second;
      contr.increment(energy,time);
    }
    else {
      // new contribution
      HitContrib contr(energy,time);
      _simHits->insert( pair<long long, HitContrib>(id, contr) );
    }
    _energy += energy;
    if( time < _time ) _time = time;
  }

  // return the contributions from simulated hits to the current TempCalHit
  map<long long, HitContrib>& TempCalHit::getContributions() {
    return *_simHits;
  }

  // return a vector with the energy contributions to the current TempCalHit
  // FIXME: should keep the array internally, and return a const reference
  vector<double> TempCalHit::getEnergyContributions() const{
    vector<double> energies;
    for(ContribMap::const_iterator i=_simHits->begin(); i != _simHits->end(); i++) {
      energies.push_back( i->second.energy() );
    }
    return energies;
  }

  // return a vector with the CellIDs for the sim hits' contributions to the current TempCalHit
  // FIXME: should keep the array internally, and return a const reference
  vector<long long> TempCalHit::getContributingIDs() const {
    vector<long long> cellIDs;
    for(ContribMap::const_iterator i=_simHits->begin(); i != _simHits->end(); i++){
      cellIDs.push_back( i->first );
    }
    return cellIDs;
  }

}
