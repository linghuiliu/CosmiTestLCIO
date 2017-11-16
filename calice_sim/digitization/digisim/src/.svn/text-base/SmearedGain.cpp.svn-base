/**
 * A simple modifier for function-based smeared linear transformations
 * on energy.
 *
 * @author Guilherme Lima, C.DeCaro
 * @version $Id: SmearedGain.cpp,v 1.3 2008-02-01 16:10:40 lima Exp $
 */
#include "FunctionModifier.hpp"
#include "SmearedGain.hpp"
#include <vector>
#include <string>

#include "CLHEP/Random/RandGauss.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace digisim {

  SmearedGain::SmearedGain() : FunctionModifier() {
    registerModifier("SmearedGain", this);
  }

  /** Smeared linear transformations on energy */
  double SmearedGain::transformEnergy(const TempCalHit& hit) const {
    // assign roles to the parameters
    const double gainNominal = _par.at(0);
    const double gainWidth = _par.at(1);

    double gain = gainNominal;
    if(gainWidth > 0.0) gain = CLHEP::RandGauss::shoot(gainNominal, gainWidth);

    if(_debug > 10) cout << "gain: " << gain << endl;

    double adc = hit.getTotalEnergy() * gain;
    return adc;
  }
}
