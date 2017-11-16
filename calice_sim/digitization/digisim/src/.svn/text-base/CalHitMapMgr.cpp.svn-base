//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CalHitMapMgr.cpp
//
// Purpose:
//   Arranges calorimeter hits in maps which can be quickly accessed using
//   the cellid key.
//
// 20041112 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "CalHitMapMgr.hpp"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/LCCollection.h"
#include "Exceptions.h"
#include <cassert>
#include <string>
using std::string;
using std::pair;
using EVENT::LCCollection;
using EVENT::SimCalorimeterHit;

#include <iostream>
using std::cout;
using std::endl;

CalHitMapMgr* CalHitMapMgr::_me = NULL;

void CalHitMapMgr::fillHitMap( const string& colname,
			       CalHitMap& hitmap )
{
  assert(_event);

  // First fill the hit map without defining densities,
  // as the map may be used for calculating densities
  LCCollection* col = NULL;
  try {
    col = _event->getCollection( colname );
  }
  catch(EVENT::DataNotAvailableException& e) {
    //     cout << e.what() << endl;
    // This happens quite frequently with tail catcher, when no energy
    // leaks into it
  }
  // return if no collection in event
  if(col==NULL) return;

  SimCalorimeterHit* ihit;
  for( int i = 0; i<col->getNumberOfElements(); ++i ) {
    ihit = (SimCalorimeterHit*)col->getElementAt(i);
    long long cellid = ((long long)ihit->getCellID1()) << 32;
    cellid |= ((long long)ihit->getCellID0());

    // fill hit map
    hitmap.insert( pair<const long long,SimCalorimeterHit*>( cellid, ihit ) );
  }
}
