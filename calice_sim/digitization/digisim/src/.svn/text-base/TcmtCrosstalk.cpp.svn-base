//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: TcmtCrosstalk.cpp
// Module: DigiSim
//
// Purpose: A modifier for simple crosstalk simulation
//
// 20050307 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#include "TcmtCrosstalk.hpp"
#include "CLHEP/Random/RandGauss.h"
#include "TempCalHit.hpp"
using namespace std;

namespace digisim {

//*** Important:
//   Please add a global instance for this modifier in Globals.cpp

TcmtCrosstalk::TcmtCrosstalk() : CalHitModifier() {
  registerModifier("TcmtCrosstalk", this);
}

void TcmtCrosstalk::init(vector<float>& floats) {
  _par = floats;
  if(_debug>0) {
    for(unsigned int i=0; i<_par.size(); ++i) {
      cout << _name << ".init(): _par["<< i << "] = "<< _par[i] << endl;
    }
  }
}

// this is where real work is performed
// hitmap is both input and output
void TcmtCrosstalk::processHits(TempCalHitMap& hitmap) {

//   cout<< "xtalk: input size="<< hitmap.size() << endl;

  // copy input map, so new hits won't produce secondary crosstalk
  TempCalHitMap origMap( hitmap );

  // loop over temp hits to look for neighbors
  for(TempCalHitMap::iterator it=origMap.begin(); it!=origMap.end(); ++it ) {
    int cellid = it->first;
    TempCalHit& hit = dynamic_cast<TempCalHit&>(it->second);
    double energy = hit.getTotalEnergy();
    double time = hit.getPrimaryTime();

    unsigned I = (cellid>> 6) & 0x3f;
    unsigned J = (cellid>>15) & 0x3f;

    // reduce energy in main hit
    hitmap[ cellid ] . scaleEnergy( 1.0 - _par[0] );
//     cout<<"hit <"<< hex << cellid << dec <<">: I="<< I <<" J="<< J <<", E="<< energy
// 	<<" --> "<< hitmap[cellid].getTotalEnergy() << endl;

    // add crosstalk into neighbor strips (TCMT specific: one lower, one higher)
    if(I>0) {
      unsigned i;

      // lower neighbor
      i = I-1;
      if(i>0) {
	int newcellid = (cellid & 0x3fff8000) | (i<<6);
	// if hit did not exist, a new one is created automatically
	TempCalHit& ihit = hitmap[ newcellid ];
	// set cellID if necessary
	if( ihit.getCellID() == 0 ) ihit.setCellID(newcellid);

// 	cout<<"   lower I: <"<< hex << newcellid << dec <<">: I="<< i <<" J="<< J
// 	    <<", E="<< ihit.getTotalEnergy() <<"+"<< (energy*_par[0]/2);
	ihit.addContribution( cellid, energy*_par[0]/2, time );
// 	cout<<" = "<< ihit.getTotalEnergy() << endl;
      }

      // upper neighbor
      i = I+1;
      if(i<=20) {
	int newcellid = (cellid & 0x3fff8000) | (i<<6);
	// if hit did not exist, a new one is created automatically
	TempCalHit& ihit = hitmap[ newcellid ];
	// set cellID if necessary
	if( ihit.getCellID() == 0 ) ihit.setCellID(newcellid);

// 	cout<<"   higher I: <"<< hex << newcellid << dec <<">: I="<< i <<" J="<< J
// 	    <<", E="<< ihit.getTotalEnergy() <<"+"<< (energy*_par[0]/2);
	ihit.addContribution( cellid, energy*_par[0]/2, time );
// 	cout<<" = "<< ihit.getTotalEnergy() << endl;
      }
    }

    if(J>0) {
      unsigned j;

      // lower neighbor
      j = J-1;
      if(j>0) {
	int newcellid = (cellid & 0x3f007fc0) | (j<<15);
	// if hit did not exist, a new one is created automatically
	TempCalHit& jhit = hitmap[ newcellid ];
	// set cellID if necessary
	if( jhit.getCellID() == 0 ) jhit.setCellID(newcellid);

// 	cout<<"   lower J: <"<< hex << newcellid << dec <<">: I="<< I <<" J="<< j
// 	    <<", E="<< jhit.getTotalEnergy() <<"+"<< (energy*_par[0]/2);
	jhit.addContribution( cellid, energy*_par[0]/2, time );
// 	cout<<" = "<< jhit.getTotalEnergy() << endl;
      }

      // upper neighbor
      j = J+1;
      if(j<=20) {
	int newcellid = (cellid & 0x3f007fc0) | (j<<15);
	// if hit did not exist, a new one is created automatically
	TempCalHit& jhit = hitmap[ newcellid ];
	// set cellID if necessary
	if( jhit.getCellID() == 0 ) jhit.setCellID(newcellid);

// 	cout<<"   higher J: <"<< hex << newcellid << dec <<">: I="<< I <<" J="<< j
// 	    <<", E="<< jhit.getTotalEnergy() <<"+"<< (energy*_par[0]/2);
	jhit.addContribution( cellid, energy*_par[0]/2, time );
// 	cout<<" = "<< jhit.getTotalEnergy() << endl;
      }
    }
  }

//   cout<< "xtalk: output size="<< hitmap.size() << endl;
}

// printout
void TcmtCrosstalk::print() const {
  cout << "TcmtCrosstalk::print(): "<< _name
       <<" - a simple crosstalk modifier"<< endl;
  cout << " Parameters:";
  for(unsigned int i=0; i<_par.size(); ++i) {
    cout << ' '<< _par[i];
  }
  cout << endl;
}

// double TcmtCrosstalk::energyToADC(const double ene) {
//   // assign roles to the parameters
//   const double gainNominal = _par[0];
//   const double gainWidth = _par[1];

//   double gain = gainNominal;
//   if(gainWidth>0.0) gain = RandGauss::shoot(gainNominal, gainWidth);
// //     if(_debug>0) cout << " gain = " << gain << endl;

//   double adc = ene * gain;
//   return adc;
// }

// bool TcmtCrosstalk::isBelowThreshold(const double adc) {
//   // assign roles to the parameters
//   const double threshNominal = _par[2];
//   const double threshWidth = _par[3];

//   double threshold = threshNominal;
//   if(threshWidth>0.0) threshold=RandGauss::shoot(threshNominal, threshWidth);

//   if(adc<threshold) return true;
//   else return false;
// }

} // end namespace
