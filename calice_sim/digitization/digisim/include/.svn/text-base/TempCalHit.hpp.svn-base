#ifndef _TEMPCALHIT_HPP_
#define _TEMPCALHIT_HPP_

#include "HitContrib.hpp"
#include <map>
#include <vector>

namespace digisim {

  class TempCalHit;
  typedef std::map<long long,TempCalHit> TempCalHitMap;

class TempCalHit {

public:
  // default constructor
  TempCalHit();

  // copy constructor
  TempCalHit(const TempCalHit& oldhit);

  // same-cell constructor
  TempCalHit(long long simid, double energy, double time);

  // destructor
  ~TempCalHit();

  // non-same cell constructor
  TempCalHit(long long rawid, long long simid, double energy, double time);

  // assignment operator
  TempCalHit& operator=(const TempCalHit& rhs);

  // method to get total energy from all contributions
  double getTotalEnergy() const;

  /** Scales the global energy.  This changes the energy of each
   *  contribution by the same factor.
   * @param factor Applied to each contribution separately
   */
  void scaleEnergy(double factor);

  /** Changes global energy.  Actually, this changes the energy of
   *  each contribution by the same factor, so that final energy
   *  gets updated to the value provided.
   * @param energy Final value of total energy
   */
  void setEnergy(double energy);

  /** Changes global time stamp -- use it with care, as it then
   *  becomes different than the timings of any individual
   *  contribution.
   */
  void setTime(double tstamp) { _time = tstamp; }

  /**
   * Primary time: either earliest time or time from largest contribution.
   * It is currently set as the earliest time.
   */
  double getPrimaryTime() const;

  /// Add a contribution from a simulated hit
  void addContribution(long long id, double energy, double time);

  // OBSOLETE: use scaleEnergy() instead
  /// Reduce contribution from original hit, due to crosstalk into neighbors
//   void reduceContribution(double energy);

  /// Return the contributions from simulated hits to the current TempCalHit
  std::map<long long, HitContrib>& getContributions();

  /// Return a vector with the energy contributions to the current TempCalHit
  std::vector<double> getEnergyContributions() const;

  /** Return a vector with the CellIDs for the sim hits' contributions
   * to the current TempCalHit
   */
  std::vector<long long> getContributingIDs() const;

  /// Set cell ID 
  void setCellID0(int id) {
    _rawID = (_rawID & 0xffffffff00000000LL) | ((long long)id);
  }
  void setCellID1(int id) {
    _rawID = (_rawID & 0x00000000ffffffff) | (((long long)id) << 32);
  }
  void setCellID(long long id) {
    _rawID = id;
  }

  // Return cell ID
  int getCellID0() const {
    return (int)(_rawID & 0xffffffff);
  }
  int getCellID1() const {
    return (int)((_rawID >> 32) & 0xffffffff);
  }
  int getCellID() const {
    return _rawID;
  }

private:
  //Method to find contribution to this TempCalHit from a given sim hit
  int findContribution(long long cellID);

  //--Data Members--
  //Temp Hit ID, supposedly same as channel ID
  long long _rawID;
  //Total Energy
  double _energy;
  //Primary hit timing
  double _time;
  //Cell IDs contributing to this TempCalHit object
  std::map<long long, HitContrib> *_simHits;

  //std::vector<double> energies;
  //std::vector<long long> cellIDs;
  std::vector<HitContrib>::const_iterator i;
};

}  //end namespace digisim
#endif
