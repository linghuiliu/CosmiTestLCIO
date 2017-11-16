#ifndef RunInformation_HH
#define RunInformation_HH

#include <iostream>
#include "UTIL/LCTime.h"
#include "EVENT/LCRunHeader.h"

/** Class which decodes information about a run stored in the run header.
 *
 * @author (probably) Roman Poeschl
 *
 */

class RunInformation {

public:
  enum Location {
    desy, cern, fnal
  };

  RunInformation();  
 RunInformation(const EVENT::LCRunHeader *run);  

  bool isMC() const;
  void isMC(bool m);
  int beamEnergyMeV() const;
  void beamEnergyMeV(int b);
  //Location location() const;
  //void location(Location l);
  std::string location() const;
  void location(std::string);
  std::string runMonth() const;
  void runMonth(std::string month);

  UTIL::LCTime runStart() const;
  void runStart(UTIL::LCTime t);
  UTIL::LCTime runEnd() const;
  void runEnd(UTIL::LCTime t);

  void get(const EVENT::LCRunHeader *run);
  void set(EVENT::LCRunHeader *run) const;

  std::ostream& print(std::ostream &o=std::cout) const;


private:
  bool _isMC;
  int _beamEnergyMeV;
  //Location _location;
  std::string _location;
  //std::string _cernMonth;
  std::string _runMonth;
  UTIL::LCTime _runStart;
  UTIL::LCTime _runEnd;
};

#endif
