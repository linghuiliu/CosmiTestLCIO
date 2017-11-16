#include <ExperimentalSetup.hh>
namespace CALICE {

  const char *ExperimentalSetup::__beamTypeNames[CALICE::ExperimentalSetup::kNBeamTypes+1]={
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

  void ExperimentalSetup::print(std::ostream &os) 
  {
    os << "type=" << getBeamTypeName()
       << " energy=" << getPeakEnergy()
       << " beam: angle ZX=" << getBeamAngleZX()
       << " ZY=" << getBeamAngleZY()
       << " position x=" << getBeamImpactPositionX0()
       << " position y=" << getBeamImpactPositionY0()
       << " detector angle" << getDetectorAngleZX()
       << " rot. orig. x=" << getDetectorRotationX0()
       << " z=" << getDetectorRotationZ0()
       << " pos x=" << getDetectorX0()
       << " y=" << getDetectorY0()
       << " z=" << getDetectorZ0();
  }
}
