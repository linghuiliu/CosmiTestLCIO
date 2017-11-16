#ifndef AHC2MIPTRACKFINDER_HH_
#define AHC2MIPTRACKFINDER_HH_

/** @brief Ahc2MipTrackFinder class used for finding Muon Tracks in the EPT Prototype.

Detailed description follows here.
@author E. Brianne, DESY
@date November 2015
*/

#include "marlin/Processor.h"
#include <map>
#include <utility>
#include <iostream>
#include <TMath.h>
#include "HcalSingleHits.hh"
#include "EcalSingleHits.hh"

using namespace std;
using namespace lcio;
using namespace marlin;

//Declare ECAL/HCAL Tower as a type TowerHitVector
typedef map<float , map<float, vector< HcalSingleHit > > > TowerHitVectorHCAL;
typedef map<float , map<float, vector< EcalSingleHit > > > TowerHitVectorECAL;

class Ahc2MipTrackFinder : public Processor
{
public:

  virtual Processor*  newProcessor() { return new Ahc2MipTrackFinder; }

  /** Default constructor.
  */
  Ahc2MipTrackFinder();

  /** Default destructor.
  */
  ~Ahc2MipTrackFinder(){};

  /** Marlin init() function.
  */
  virtual void init();

  /** Marlin processRunHeader() function.
  */
  virtual void processRunHeader(LCEvent *evt);

  /** Marlin processEvent() function.
  */
  virtual void processEvent(LCEvent *evt);

  /** Marlin check() function.
  */
  virtual void check( LCEvent * evt );

  /** Marlin end() function.
  */
  virtual void end();

  void printParameters();

  /**Fill HCAL Towers
  @param TowerMap - HCAL TowerMap
  @param I - I coordinate of the hit
  @param J - J coordinate of the hit
  @param K - K coordinate of the hit
  @param x - x coordinate of the hit
  @param y - y coordinate of the hit
  @param z - z coordinate of the hit
  @param ahc_hitEnergy - Energy of the hit
  @param ahc_hitTime - Time of the hit
  @param ahc_hitType - Type of the hit
  @param CellID0 - CellID of the hit
  */
  int Fill2DMap(TowerHitVectorHCAL &TowerMap, int I,  int J,  int K, float x, float y, float z, Float_t ahc_hitEnergy,Float_t ahc_hitTime, Int_t ahc_hitType, int CellID0);

  /**Fill ECAL Towers
  @param TowerMap - ECAL TowerMap
  @param I - I coordinate of the hit
  @param J - J coordinate of the hit
  @param K - K coordinate of the hit
  @param x - x coordinate of the hit
  @param y - y coordinate of the hit
  @param z - z coordinate of the hit
  @param emc_hitEnergy - Energy of the hit
  @param emc_hitTime - Time of the hit
  @param emc_hitType - Type of the hit
  @param CellID0 - CellID of the hit
  */
  int Fill2DMap(TowerHitVectorECAL &TowerMap, int I,  int J,  int K, float x, float y, float z, Float_t emc_hitEnergy,Float_t emc_hitTime, Int_t emc_hitType, int CellID0);

  /** Select HCAL Towers with number of hits over MinNHits
  @param towers - HCAL Towers
  @param MinNHits - Minimum on Number of hits in a Tower
  @return - Number of Towers left
  */
  int selectTowerNHits(TowerHitVectorHCAL &towers, unsigned int MinNHits);

  /** Reject T0 hits
  @param Ihit - I coordinate of the hit
  @param Jhit - J coordinate of the hit
  @param Khit - K coordinate of the hit
  @param ahc_hitEnergy - Energy of the hit
  @return - true if T0 Hit otherwise false
  */
  bool Check_T0(int Ihit, int Jhit, int Khit);

  /** Reject Cherenkow hits
  @param Ihit - I coordinate of the hit
  @param Jhit - J coordinate of the hit
  @param Khit - K coordinate of the hit
  @param ahc_hitEnergy - Energy of the hit
  @return - true if Cherenkow Hit otherwise false
  */
  bool Check_Cherenkow(int Ihit, int Jhit, int Khit);

  /** Check ECAL Strip overlap with HCAL Tower
  @param Coordinates_ECAL - x,y coordinates of the ECAL hit
  @param Coordinates_HCAL - x,y coordinates of the HCAL hit
  @return - true if overlap otherwise false
  */
  bool CheckOverlap(pair<float, float> Coordinates_ECAL, pair<float, float> Coordinates_HCAL);

  /** Increment Overlap parameter of a ECAL Hit
  @param ECALHits - vector containing ECAL hits
  */
  int IncrementOverlap(vector<EcalSingleHit> &ECALHits);

protected:

  TowerHitVectorHCAL _towerHCALHits; /**<HCAL Tower*/
  TowerHitVectorECAL _towerECALHits; /**<ECAL Tower*/

  std::string _ahcinputColName; /**<HCAL input Collection*/
  std::string  _emcinputColName; /**<ECAL input Collection*/
  std::string _ahcHitOutputColName; /**<HCAL output Collection*/
  std::string _emcHitOutputColName; /**<ECAL output Collection*/

  std::string _encodingECAL;
  std::string _encodingHCAL;

  bool _keepT0Cherenkow; /**keep T0 and Cherenkow in output collection*/
  int _cutNTowerHits; /**<Cut on Number of hits in a Tower*/
  int _cutNLayerHits; /**<Cut on Number of hits in a Layer*/
  int _nMaxHits; /**<Maximum number of hits per event*/
  int _nMinHits; /**<Minimum number of hits per event*/

  StringVec _T0Vector; /**<Vector containing T0s*/
  StringVec _CherenkowVector; /**<Vector containing Cherenkows*/

  vector<int> _mapT0s; /**<map for T0s*/
  vector<int> _mapCherenkows; /**<map for Cherenkows*/

  vector<int> HitPerlayer; /**<Number of hits per layer before tracking to reject late pion showers*/

  int nHCALtracks;

  int _nEvt;
  int _nRun;
};



#endif /* AHC2MIPTRACKFINDER_H_ */
