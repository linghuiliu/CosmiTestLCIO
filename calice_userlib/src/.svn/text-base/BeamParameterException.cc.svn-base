#include <BeamParameterException.hh>
namespace CALICE {

  /*  const char *ExperimentalSetup::__beamTypeNames[CALICE::ExperimentalSetup::kNBeamTypes+1]={
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
    };*/

  void BeamParameterException::print(std::ostream &os) 
  {
    os << " energy=" << getBeamEnergy() << " MeV" << std::endl;
  }
}
