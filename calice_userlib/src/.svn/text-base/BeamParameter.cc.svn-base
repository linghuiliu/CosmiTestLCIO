#include <BeamParameter.hh>
namespace CALICE {

  const char *BeamParameter::__beamTypeNames[CALICE::BeamParameter::kNBeamTypes+1]={
    "electron",
    "muon",
    "pion",
    "proton",
    "cosmics",
    "unknown",
    "mixed", 
    "pion+electron",
    "pion+proton",
    "calibration",
    "noise",
    "(out of range)",
  };

  void BeamParameter::print(std::ostream &os) 
  {
    os << "type=" << getBeamTypeName()
       << " energy=" << getPeakEnergy()
       << " beam: angle ZX=" << getBeamAngleZX()
       << " ZY=" << getBeamAngleZY()
       << " position x=" << getBeamImpactPositionX0()
       << " position y=" << getBeamImpactPositionY0();
  }
}
