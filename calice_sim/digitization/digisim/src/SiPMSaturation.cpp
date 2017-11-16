/**
 * A simple modifier for simulating the SiPM saturation.
 *
 * @author Guilherme Lima, C.DeCaro
 * @version $Id: SiPMSaturation.cpp,v 1.2 2005-08-31 21:28:05 lima Exp $
 */
#include "FunctionModifier.hpp"
#include "SiPMSaturation.hpp"
#include <vector>
#include <string>

#include "CLHEP/Random/RandGauss.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace digisim {


  SiPMSaturation::SiPMSaturation() : FunctionModifier() {
    registerModifier("SiPMSaturation", this);
  }

  /** Smeared linear transformations on energy */
  double SiPMSaturation::transformEnergy(const TempCalHit& hit) const{
    // assign roles to the parameters
    const double gainNom = _par.at(0);
    const double linearMax = _par.at(1);

    double energy = hit.getTotalEnergy();
    // saturation regime
    double adc = linearMax*gainNom;

    if(energy < linearMax) {
      // linear regime
      adc = energy * gainNom;
    }
    else {
      if(_debug > 0) cout<<"saturation in: "<< energy <<", out="<< adc << endl;
    }

    return adc;
  }
}
