#include "BeamMomentum.hh"

#include <vector>
#include <cmath>
#include <iostream>


namespace CALICE {

  BeamMomentum::BeamMomentum(const CALICE::BmlSlowRunDataBlock& cernBeamData) {
    std::vector<double> bendMagnets =  cernBeamData.getBendCurrents();
    std::vector<double> collimators =  cernBeamData.getCollimatorPositions();

    //  std::cout << bendMagnets.size() << std::endl;
    //  std::cout << collimators.size() << std::endl;

    // size check
    if (bendMagnets.size() != 9*2) {
      throw new BeamMomentumException("CERN beamline expects 9 bend magnets");
    }
    if (collimators.size() != 11*4) {
      throw new BeamMomentumException("CERN beamline expects 11 collimators");
    }


    /*
     * Prepare variables that follow the numbering scheme of the beam documents.
     * If the read value is 0, try the set value.
     * For the collimators this might be overdone, as only 3 & 8 are of interest, but let's stay general.
     */
    std::vector<double> B;
    std::vector<double> C;

    B.resize((bendMagnets.size()/2)+1);
    for (unsigned int i=0; i< bendMagnets.size(); i+=2) {
      int I = i/2+1;
      if (I == 8 ) continue; // BEND8 does not exist
      if (bendMagnets[i] !=0 )  B[I] = bendMagnets[i];
      else {
        B[I] = bendMagnets[i+1];
        _warningText << "Bend" << I << " actual value is 0 - reading error? --> using set value instead" << std::endl;
      }
      if ( bendMagnets[i+1] != 0 &&
           fabs((bendMagnets[i] - bendMagnets[i+1])/bendMagnets[i+1]) > 0.01 )
        _warningText << "Bend"<< I <<" actual value " << bendMagnets[i]<< " but ref " << bendMagnets[i+1] << std::endl;
    }

    C.resize((collimators.size()/4)+1);
    for (unsigned int i=0; i< collimators.size(); i+=4) {
      int I = i/4+1;
      if ( I == 4 || I == 7 ) continue; // COLLIMATOR 4 & 7 do not exist
      if (collimators[i] != 0 && collimators[i+2] != 0 )  C[I] = collimators[i+2] - collimators[i];
      else {
        C[I] = collimators[i+3] - collimators[i+1];
        _warningText << "Collimator" << I <<" has 0 value (" << collimators[i] << "," << collimators[i+2] <<")" << std::endl;
      }
      if ( collimators[i+1] !=0 &&
           collimators[i+3] !=0 &&
           ( fabs((collimators[i] - collimators[i+1])/collimators[i+1]) > 0.1 ||
             fabs((collimators[i+2] - collimators[i+3])/collimators[i+3]) > 0.1 ) )
        _warningText << "Collimator"<<I<<" actual value (" << collimators[i] << "," << collimators[i+2] <<") but ref "<< collimators[i+1] << "," << collimators[i+3] << std::endl;
    }

    _momentum = B[5]/-4.72;  // official momentum defining bend

    /*
     * To reduce problems with failed readouts of single magnets
     * only magnets with more than 3GeV are accounted.
     * This should be save, as the minimum energy of the beamline is 6 GeV.
     */
    _secondaryMomentum = 0;
    int goodMagnets = 0;
    if ( fabs(B[2]/7.25) > 3 ) {
      _secondaryMomentum += B[2]/7.25;
      goodMagnets++;
    }
    if ( fabs(B[3]/-4.7) > 3 ) {
      _secondaryMomentum += B[3]/-4.7;
      goodMagnets++;
    }
    if ( fabs(B[4]/4.84) > 3 ) {
      _secondaryMomentum += B[4]/4.84;
      goodMagnets++;
    }
    _secondaryMomentum /= (double)goodMagnets;


    _tertiaryMomentum = 0;
    goodMagnets = 0;
    if ( fabs(B[5]/-4.72) > 3 ) {
      _tertiaryMomentum += B[5]/-4.72;
      goodMagnets++;
    }
    if ( fabs(B[6]/4.95) > 3 ) {
      _tertiaryMomentum += B[6]/4.95;
      goodMagnets++;
    }
    if ( fabs(B[7]/5.02) > 3 ) {
      _tertiaryMomentum += B[7]/5.02;
      goodMagnets++;
    }
    _tertiaryMomentum /= (double)goodMagnets;
    _relativeSpread = sqrt(pow(C[3],2.) + pow(C[8],2.))/19.4/100. / (2.*sqrt(2.*log(2.)));

    if ( fabs((_secondaryMomentum - _tertiaryMomentum)/_tertiaryMomentum ) < 0.1 && C[8] > 4.) {
      _limitedRelativeSpread = sqrt(pow(C[3],2.) + pow(4.,2.))/19.4/100. / (2.*sqrt(2.*log(2.)));
      _warningText << "This looks like secondary beam (p_{ter}=" << _tertiaryMomentum << " p_{sec}=" << _secondaryMomentum << ") and C8 = "<< C[8] <<" > 4mm " <<std::endl
                   << "Relative spread might be overestimated." << std::endl
                   << " alternative relative spread: " << _limitedRelativeSpread << std::endl;

    }
    else _limitedRelativeSpread = _relativeSpread;


  }

  double BeamMomentum::getMomentum() const {
    return _momentum;
  }

  double BeamMomentum::getSecondaryMomentum() const {
    return _secondaryMomentum;
  }

  double BeamMomentum::getTertiaryMomentum() const {
    return _tertiaryMomentum;
  }

  double BeamMomentum::getRelativeSpread() const {
    return _relativeSpread;
  }

  double BeamMomentum::getMomentumSpread() const {
    return _relativeSpread*_momentum;
  }

  double BeamMomentum::getLimitedRelativeSpread() const {
    return _relativeSpread;
  }

  double BeamMomentum::getLimitedMomentumSpread() const {
    return _limitedRelativeSpread*_momentum;
  }

  const std::string BeamMomentum::getWarnings() const {
    return _warningText.str();
  }
}
