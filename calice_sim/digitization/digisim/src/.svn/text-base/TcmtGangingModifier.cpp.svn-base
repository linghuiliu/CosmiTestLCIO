//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: TcmtGangingModifier.cpp
// Module: DigiSim
//
// Purpose: A modifier for ganging of MC tail catcher hits
//
// 20061106 - G.Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "TcmtGangingModifier.hpp"
#include "TempCalHit.hpp"
#include "CellIDDecoder.hpp"
#include "NoiseDef.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;
// using std::string;
using std::vector;
using std::pair;
using std::hex;
using std::dec;

namespace digisim {

  TcmtGangingModifier::TcmtGangingModifier() : CalHitModifier() {
    registerModifier("TcmtGangingModifier", this);
  }

  void TcmtGangingModifier::init(vector<float>& floats) {
    _par = floats;
    if(_debug>0) {
      for(unsigned int i=0; i<_par.size(); ++i) {
	cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
      }
    }
  }

  // this is where real work is performed
  // hitmap is both input and output
  void TcmtGangingModifier::processHits(TempCalHitMap& hitmap) {//process hits

    //assign roles to the parameters
    //double TimeMean = _par[62];
    //double TimeSigma = _par[63];

    // duplicate input collection
    TempCalHitMap input( hitmap );
    hitmap.clear();

    // loop over simHits, creating new ganged hits
    for(TempCalHitMap::iterator it=input.begin(); it!=input.end(); ++it ) {
      TempCalHit& ihit = dynamic_cast<TempCalHit&>(it->second);
      unsigned int simid = ihit.getCellID0();
      unsigned int ilayer = simid & 0xff;  // BL: K and not K-1
      unsigned int iy = (simid >> 8) & 0xff;
      unsigned int ix = (simid >> 16) & 0xff;
      double energy = ihit.getTotalEnergy();
      double time = ihit.getPrimaryTime();

      // even (odd) layers contain horizontal (vertical) strips
      unsigned strip = ix;
      if(ilayer%2==0) strip = iy;
//       unsigned newid = (strip << 12) | ilayer;

      // GL: conform with Mokka CellIDEncoding: M:3,S-1:3,I:9,J:9,K-1:6 
      /* BL: no not true. Mokka CellIDEncoding is K:8,J:8,I:8 as used above,
       *     this is standard Marlin CellID encoding for CalorimeterHits.
       *     Need to subtract 1 from K or add proper encoding string to the event.
       *     Unfortunately, the design of digisim does not respect encoding
       *     strings. Therefore, I added substraction of 1 here, to be
       *     conform with the standard encoding.
       *     to-do: make digisim aware of encoding strings, to prevent
       *            problems when standards are changed.
       */
      unsigned newid = (ilayer - 1 << 24);
      if(ilayer%2!=0) newid |= (strip << 6);  // vertical strip
      if(ilayer%2==0) newid |= (strip << 15); // horizontal strip

      // create new hit
      TempCalHitMap::iterator it2 = hitmap.find( newid );
      if(it2==hitmap.end()) {
	// a new hit is needed
	TempCalHit newhit( newid, simid, energy, time );
	hitmap.insert( pair<long long,TempCalHit>(newid,newhit) );
      }
      else {
	TempCalHit& newhit = it->second;
	newhit.addContribution( simid, energy, time );
      }

    }
  }//process hits

  // printout
  void TcmtGangingModifier::print() const {
    cout << "TcmtGangingModifier::print(): "<< _name
	 <<" - a TCMT ganging modifier"<< endl;
    cout << " Parameters:";
    for(unsigned int i=0; i<_par.size(); ++i) {
      cout << ' '<< _par[i];
    }
    cout << endl;
  }

} // end namespace digisim
