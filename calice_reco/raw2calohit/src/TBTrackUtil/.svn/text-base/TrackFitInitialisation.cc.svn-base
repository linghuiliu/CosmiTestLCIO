#include <cassert>
#include <iomanip>

#include "TrackFitInitialisation.hh"

namespace TBTrack {

TrackFitInitialisation::TrackFitInitialisation() : _errorMatrix(6) {
}

double TrackFitInitialisation::zBeam() const {
  return _zBeam;
}

void TrackFitInitialisation::zBeam(double z) {
  _zBeam=z;
}

double TrackFitInitialisation::beamCoordinate() const {
  return _beamAverage[0];
}

double TrackFitInitialisation::beamTanAngle() const {
  return _beamAverage[1];
}

void TrackFitInitialisation::beamAverage(double c, double t) {
  _beamAverage[0]=c;
  _beamAverage[1]=t;
}

double TrackFitInitialisation::zLayer(unsigned l) const {
  assert(l<=3);
  return _zLayer[l];
}

void TrackFitInitialisation::zLayer(unsigned l, double z) {
  assert(l<=3);
  _zLayer[l]=z;
}

const TMatrixDSym& TrackFitInitialisation::errorMatrix() const {
  return _errorMatrix;
}

void TrackFitInitialisation::errorMatrix(const TMatrixDSym &e) {
  assert(e.GetNrows()==6);
  _errorMatrix=e;
}

std::ostream& TrackFitInitialisation::print(std::ostream &o, const std::string &s) const {
  o << "TrackFitInitialisation::print()" << std::endl;
  o << " Z origin of beam = " << _zBeam << std::endl;
  /*
  for(unsigned i(0);i<2;i++) {
    if(i==0) o << " X dimension" << std::endl;
    else     o << " Y dimension" << std::endl;

    o << " Beam average position and direction = " << _beamAverage[i][0]
      << ", " <<  _beamAverage[i][1] << std::endl;

    TMatrixDSym b(beamSpread(i));
    o << " Beam spread matrix" << std::endl;
    for(unsigned j(0);j<2;j++) {
	o << "  ";
      for(unsigned k(0);k<2;k++) {
	o << std::setw(13) << b(j,k);
      }
      o << std::endl;
    }

    o << " Tracking layer z values" << std::endl;
    for(unsigned j(0);j<4;j++) {
      o << std::setw(13) << _zLayer[i][j];
    }
    o << std::endl;

    TMatrixDSym e(forwardScattering(i,electron));
    o << "Electron forward error matrix" << std::endl;
    for(unsigned j(0);j<6;j++) {
      for(unsigned k(0);k<6;k++) {
	o << std::setw(13) << e(j,k);
      }
      o << std::endl;
    }
    o << std::endl;

    e.ResizeTo(4,4);
    e=backwardScattering(i,electron);
    o << "Electron backward error matrix" << std::endl;

    o << _backwardScattering[i][0][1] << std::endl;

    for(unsigned j(0);j<4;j++) {
      for(unsigned k(0);k<4;k++) {
	o << std::setw(13) << e(j,k);
      }
      o << std::endl;
    }
    o << std::endl;
  */

      /*
      << std::setw(12) << _errorMatrix[i][0][0] << ", "
      << std::setw(12) << _errorMatrix[i][0][1] << ", "
      << std::setw(12) << _errorMatrix[i][0][2] << ", "
      << std::setw(12) << _errorMatrix[i][0][3] << ", normalised "
      << std::setw(12) << 1.0 << ", "
      << std::setw(12) << _errorMatrix[i][0][1]/sqrt(_errorMatrix[i][0][0]*_errorMatrix[i][0][4]) << ", "
      << std::setw(12) << _errorMatrix[i][0][2]/sqrt(_errorMatrix[i][0][0]*_errorMatrix[i][0][7]) << ", "
      << std::setw(12) << _errorMatrix[i][0][3]/sqrt(_errorMatrix[i][0][0]*_errorMatrix[i][0][9]) << std::endl
      << std::setw(12) << _errorMatrix[i][0][1] << ", "
      << std::setw(12) << _errorMatrix[i][0][4] << ", "
      << std::setw(12) << _errorMatrix[i][0][5] << ", "
      << std::setw(12) << _errorMatrix[i][0][6] << ",            "
      << std::setw(12) << _errorMatrix[i][0][1]/sqrt(_errorMatrix[i][0][4]*_errorMatrix[i][0][0]) << ", "
      << std::setw(12) << 1.0 << ", "
      << std::setw(12) << _errorMatrix[i][0][5]/sqrt(_errorMatrix[i][0][4]*_errorMatrix[i][0][7]) << ", "
      << std::setw(12) << _errorMatrix[i][0][6]/sqrt(_errorMatrix[i][0][4]*_errorMatrix[i][0][9]) << std::endl
      << std::setw(12) << _errorMatrix[i][0][2] << ", "
      << std::setw(12) << _errorMatrix[i][0][5] << ", "
      << std::setw(12) << _errorMatrix[i][0][7] << ", "
      << std::setw(12) << _errorMatrix[i][0][8] << ",            "
      << std::setw(12) << _errorMatrix[i][0][2]/sqrt(_errorMatrix[i][0][7]*_errorMatrix[i][0][0]) << ", "
      << std::setw(12) << _errorMatrix[i][0][5]/sqrt(_errorMatrix[i][0][7]*_errorMatrix[i][0][4]) << ", "
      << std::setw(12) << 1.0 << ", "
      << std::setw(12) << _errorMatrix[i][0][8]/sqrt(_errorMatrix[i][0][7]*_errorMatrix[i][0][9]) << std::endl
      << std::setw(12) << _errorMatrix[i][0][3] << ", "
      << std::setw(12) << _errorMatrix[i][0][6] << ", "
      << std::setw(12) << _errorMatrix[i][0][8] << ", "
      << std::setw(12) << _errorMatrix[i][0][9] << ",            "
      << std::setw(12) << _errorMatrix[i][0][3]/sqrt(_errorMatrix[i][0][9]*_errorMatrix[i][0][0]) << ", "
      << std::setw(12) << _errorMatrix[i][0][6]/sqrt(_errorMatrix[i][0][9]*_errorMatrix[i][0][4]) << ", "
      << std::setw(12) << _errorMatrix[i][0][8]/sqrt(_errorMatrix[i][0][9]*_errorMatrix[i][0][7]) << ", "
      << std::setw(12) << 1.0 << std::endl;
      */
  /*
    e.ResizeTo(6,6);
    e=forwardScattering(i,hadron);

    o << "Hadron forward error matrix" << std::endl;
    for(unsigned j(0);j<6;j++) {
      for(unsigned k(0);k<6;k++) {
	o << std::setw(13) << e(j,k);
      }
      o << std::endl;
    }
    o << std::endl;

    e.ResizeTo(4,4);
    e=backwardScattering(i,hadron);

    o << "Hadron backward error matrix" << std::endl;
    o << _backwardScattering[i][1][1] << std::endl;

    for(unsigned j(0);j<4;j++) {
      for(unsigned k(0);k<4;k++) {
	o << std::setw(13) << e(j,k);
      }
      o << std::endl;
    }
    o << std::endl;
  */
    /*
      << std::setw(12) << _errorMatrix[i][1][0] << ", "
      << std::setw(12) << _errorMatrix[i][1][1] << ", "
      << std::setw(12) << _errorMatrix[i][1][2] << ", "
      << std::setw(12) << _errorMatrix[i][1][3] << std::endl << "  "
      << std::setw(12) << _errorMatrix[i][1][1] << ", "
      << std::setw(12) << _errorMatrix[i][1][4] << ", "
      << std::setw(12) << _errorMatrix[i][1][5] << ", "
      << std::setw(12) << _errorMatrix[i][1][6] << std::endl << "  "
      << std::setw(12) << _errorMatrix[i][1][2] << ", "
      << std::setw(12) << _errorMatrix[i][1][5] << ", "
      << std::setw(12) << _errorMatrix[i][1][7] << ", "
      << std::setw(12) << _errorMatrix[i][1][8] << std::endl << "  "
      << std::setw(12) << _errorMatrix[i][1][3] << ", "
      << std::setw(12) << _errorMatrix[i][1][6] << ", "
      << std::setw(12) << _errorMatrix[i][1][8] << ", "
      << std::setw(12) << _errorMatrix[i][1][9] << std::endl;
    */
  //  }

  return o;
}

}
