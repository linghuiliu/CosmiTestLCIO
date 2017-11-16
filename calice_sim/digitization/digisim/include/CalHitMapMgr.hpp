//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CalHitMapMgr.hpp
//
// Purpose:
//   Arranges calorimeter hits in maps keyed by cellID.
//   Hits can then be randomly accessed using their cellID.
//
// Once filled, the maps can be accessed using 
//   string colName("MyLcioCollection");
//   Map hitmap = CalHitMapMgr::getInstance()->getCollHitMap(colName);
//
// 20041112 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _CALHITMAPMGR_HPP_
#define _CALHITMAPMGR_HPP_

#include <iostream>
#include <map>
#include "EVENT/LCEvent.h"
#include "EVENT/SimCalorimeterHit.h"

// Map of SimCalorimeterHits keyed by cellID
typedef std::map<long long,EVENT::SimCalorimeterHit*> CalHitMap;
// Map of CalHitMaps keyed by collection name
typedef std::map<const std::string, CalHitMap> CalHitCollections;

class CalHitMapMgr {

public:
  // To be used instead of constructor
  static CalHitMapMgr* getInstance() {
    if( !_me ) _me = new CalHitMapMgr();
    return _me;
  }

  static void destroy() {
    if(_me) delete _me;
  }

  void setEvent(EVENT::LCEvent& event) {
//  std::cout<< " HitMapMgr.setEvent: "<< &event <<' '<< _event << std::endl;
    if( _event!=&event || event.getEventNumber()!=_evtno
	|| event.getRunNumber()!=_runno ) {
      _me->reset();
      _me->_event = &event;
      _me->_runno = event.getRunNumber();
      _me->_evtno = event.getEventNumber();
    }
  }

  const CalHitMap& getCollHitMap(const std::string& colName) {
    // if it does not exist, create it now
    CalHitMap& retColl = _collMap[ colName ];
    if( retColl.size()==0 ) fillHitMap( colName, retColl );
    return retColl;
  }

private:
  // Constructor
  CalHitMapMgr() : _event(0) {}
  // Destructor
  ~CalHitMapMgr() {
    reset();
  }

  void reset() {
    CalHitCollections::iterator icol = _collMap.begin();
    while( icol!=_collMap.end() ) {
      icol->second.clear();
      ++icol;
    }
  }

  void fillHitMap( const std::string& colName, CalHitMap& hitmap );

private:
  // ***** Member data  *****
  static CalHitMapMgr* _me;
  EVENT::LCEvent* _event;
  int _runno, _evtno;
  CalHitMap _emHits;
  CalHitMap _hadHits;
  CalHitCollections _collMap;
};

#endif // _CALHITMAPMGR_HPP_
