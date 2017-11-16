#include <cassert>

#include "EVENT/LCParameters.h"
#include "RunInformation.hh"


RunInformation::RunInformation() {
}

RunInformation::RunInformation(const EVENT::LCRunHeader *run) {
  get(run);
}

void RunInformation::isMC(bool m){
  _isMC = m;
}
void RunInformation::beamEnergyMeV(int b){
  _beamEnergyMeV = b;
}

void RunInformation::location(std::string l){
  _location = l;
}
void RunInformation::runMonth(std::string month){
  _runMonth = month;
}
void RunInformation::runStart(UTIL::LCTime t){
  _runStart = t;
}
void RunInformation::runEnd(UTIL::LCTime t){
  _runEnd = t;
}
bool RunInformation::isMC() const{
  return _isMC;
}
int RunInformation::beamEnergyMeV() const{
  return _beamEnergyMeV;
}
//RunInformation::Location RunInformation::location() const{
//  return _location;
//
std::string RunInformation::location() const{
  return _location;
}
std::string RunInformation::runMonth() const{
  return _runMonth;
}
UTIL::LCTime RunInformation::runStart() const{
  return _runStart;
}
UTIL::LCTime RunInformation::runEnd() const{
  return _runEnd;
}

void RunInformation::get(const EVENT::LCRunHeader *run){
  const EVENT::LCParameters & params = run->getParameters();
  _beamEnergyMeV=params.getIntVal("beamEnergyMeV");
  _isMC=static_cast<bool>(params.getIntVal("isMC"));
  //_location=static_cast<Location>(params.getIntVal("location"));
  _location=params.getStringVal("location");
  _runMonth=params.getStringVal("runMonth");
  _runStart=params.getIntVal("runStartSec")*1000000000LL+params.getIntVal("runStartNs");
  _runEnd=params.getIntVal("runEndSec")*1000000000LL+params.getIntVal("runEndNs");

}
void RunInformation::set(EVENT::LCRunHeader *run) const{
  EVENT::LCParameters & params = run->parameters();
  params.setValue("isMC",static_cast<int>(_isMC));
  params.setValue("beamEnergyMeV",_beamEnergyMeV);
  params.setValue("location",_location);
  params.setValue("runMonth",_runMonth);

  UTIL::LCTime rs=_runStart;
  params.setValue("runStartSec",rs.unixTime());
  params.setValue("runStartNs",rs.ns());

  UTIL::LCTime re=_runEnd;
  params.setValue("runEndSec",re.unixTime());
  params.setValue("runEndNs",re.ns());

}


std::ostream& RunInformation::print(std::ostream &o) const {


  o << std::endl << "#########################" << 
    "RunInformation::print()" << std::endl <<
    "ISMC = " << _isMC << "," << std::endl <<
    "Beam E (MeV) = " << _beamEnergyMeV << "," << std::endl
  //if (_location == RunInformation::desy) o << "location = DESY," << std::endl;
  //else if (_location == RunInformation::cern) o << "location = CERN," << std::endl;
  //else if (_location == RunInformation::fnal) o << "location = FNAL," << std::endl;
  //else o << "location unknown : " << _location << "," << std::endl;
    << "Location= " << _location << std::endl
    << "Run month/year: " << _runMonth << "," 
    << std::endl;

  UTIL::LCTime rs=_runStart;
  UTIL::LCTime re=_runEnd;
  o << "Run valid between = " << rs.getDateString() << " and " << re.getDateString() << std::endl;
  o << std::endl << "#########################" << std::endl;

  return o;
}
