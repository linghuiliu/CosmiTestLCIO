#include <cassert>
#include <iomanip>
#include <cstring>

#include "AlnConstants.hh"

namespace TBTrack {

//AlnConstants::AlnConstants(bool desy, bool data) {
AlnConstants::AlnConstants(int period, int dataType) {

  assert(sizeof(AlnConstants)==
         sizeof(int)*numberOfInts+
         sizeof(float)*numberOfFloats+
         sizeof(double)*numberOfDoubles);

  memset(this,0,sizeof(AlnConstants));

  if(period == 0) {//period
    _tdcUnit=1.0; // 1ns
    
    if(dataType == 2) {//dataType
    /*
    for(unsigned i(0);i<2;i++) {
      double offset(50.0*(i-0.5));
      _zLayer[i][0]=-  50.0+offset;
      _zLayer[i][1]=-1150.0+offset;
      _zLayer[i][2]=-2060.0+offset;
      _zLayer[i][3]=-3160.0+offset;
    }
    */
    
    // X drift velocities per layer
      _vDrift[0][0]= 0.0300;
      _vDrift[0][1]=-0.0312;
      _vDrift[0][2]= 0.0308;
      _vDrift[0][3]=-0.0300;
      
      // Y drift velocities per layer
      _vDrift[1][0]=-0.0300;
      _vDrift[1][1]= 0.0306;
      _vDrift[1][2]=-0.0312;
      _vDrift[1][3]= 0.0300;
      
      /*
	for(unsigned i(0);i<2;i++) {
	for(unsigned j(0);j<4;j++) {
	if(i==0) _vDrift[i][j]*=1.01;
	if(i==1) _vDrift[i][j]*=0.97;
	}
	}
      */
      
      // X corresponding to time = 0
      _cTzero[0][0]=-_vDrift[0][0]*1310.0;
      //_cTzero[0][1]=-_vDrift[0][1]*1116.0;
      _cTzero[0][1]= 34.0;
      //_cTzero[0][2]=-_vDrift[0][2]*1183.0;
      _cTzero[0][2]=-35.8;
      _cTzero[0][3]=-_vDrift[0][3]*1020.0;
      
      // Y corresponding to time = 0
      _cTzero[1][0]=-_vDrift[1][0]*1209.0;
      //_cTzero[1][1]=-_vDrift[1][1]*1165.0;
      _cTzero[1][1]=-35.0;
      //_cTzero[1][2]=-_vDrift[1][2]*1113.0;
      _cTzero[1][2]= 34.3;
      _cTzero[1][3]=-_vDrift[1][3]*1055.0;
      
      for(unsigned i(0);i<2;i++) {
	for(unsigned j(0);j<4;j++) {
	  _cTzero[i][j]/=-_vDrift[i][j];
	}
      }

    } else {

      _vDrift[0][0]= 0.03;
      _vDrift[0][1]=-0.03;
      _vDrift[0][2]= 0.03;
      _vDrift[0][3]=-0.03;
      
      _vDrift[1][0]=-0.03;
      _vDrift[1][1]= 0.03;
      _vDrift[1][2]=-0.03;
      _vDrift[1][3]= 0.03;

      for(unsigned i(0);i<2;i++) {
	for(unsigned j(0);j<4;j++) {
	  _cTzero[i][j]=1200.0;
	}
      }
    }


  } else {
    _tdcUnit=25.0/32.0; // 40MHz clock subdivided by 32


    /*
    for(unsigned i(0);i<2;i++) {
      double offset(29.0*(i-0.5));
      _zLayer[i][0]=-  29.0+offset;
      _zLayer[i][1]=- 680.0+offset;
      _zLayer[i][2]=-2528.0+offset;
      _zLayer[i][3]=0.0;
    }
    */
    
    // X drift velocities per layer
    _vDrift[0][0]=0.2;
    _vDrift[0][1]=0.2;
    _vDrift[0][2]=0.2;
    _vDrift[0][3]=0.0;
    
    // Y drift velocities per layer
    _vDrift[1][0]=0.2;
    _vDrift[1][1]=0.2;
    _vDrift[1][2]=0.2;
    _vDrift[1][3]=0.0;
    
    // X corresponding to time = 0
    _cTzero[0][0]=-_vDrift[0][0]*0.0;
    _cTzero[0][1]=-_vDrift[0][1]*0.0;
    _cTzero[0][2]=-_vDrift[0][2]*0.0;
    _cTzero[0][3]=0.0;
    
    // Y corresponding to time = 0
    _cTzero[1][0]=-_vDrift[1][0]*0.0;
    _cTzero[1][1]=-_vDrift[1][1]*0.0;
    _cTzero[1][2]=-_vDrift[1][2]*0.0;
    _cTzero[1][3]=0.0;
  }

  for(unsigned i(0);i<2;i++) {
    for(unsigned j(0);j<4;j++) {
      _vDquad[i][j]=0.0;
    }
  }
}

double AlnConstants::cTzero(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _cTzero[d][l];
}

void AlnConstants::cTzero(unsigned d, unsigned l, double c) {
  assert(d<=1 && l<=3);
  _cTzero[d][l]=c;
}

double AlnConstants::vDrift(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _vDrift[d][l];
}

void AlnConstants::vDrift(unsigned d, unsigned l, double z) {
  assert(d<=1 && l<=3);
  _vDrift[d][l]=z;
}

double AlnConstants::vDquad(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _vDquad[d][l];
}

void AlnConstants::vDquad(unsigned d, unsigned l, double z) {
  assert(d<=1 && l<=3);
  _vDquad[d][l]=z;
}

double AlnConstants::coordinate(unsigned d, unsigned l, int t) const {
  assert(d<=1 && l<=3);
  //const double tns(t*_tdcUnit-_cTzero[d][l]);
  //const double tns((t-_cTzero[d][l])*_tdcUnit);
  const double tns(t-_cTzero[d][l]);
  return _vDrift[d][l]*tns+_vDquad[d][l]*tns*tns;
}

int AlnConstants::tdcValue(unsigned d, unsigned l, double c, double t) const {
  assert(d<=1 && l<=3);

  // Polynominal expansion; avoids quad=0 problems and wrong solution
  const double s(c/_vDrift[d][l]);
  const double q(_vDquad[d][l]/_vDrift[d][l]);
  //double tdc((t+_cTzero[d][l]+s-q*s*s+2.0*q*q*s*s*s)/_tdcUnit);
  //double tdc(_cTzero[d][l]+(t+s-q*s*s+2.0*q*q*s*s*s)/_tdcUnit);
  double tdc(_cTzero[d][l]+(t+s-q*s*s+2.0*q*q*s*s*s));

  // An int always rounds towards zero so compensate
  if(tdc>=0.0) tdc+=0.5;
  else         tdc-=0.5;

  return (int)tdc;
}

std::ostream& AlnConstants::print(std::ostream &o, const std::string &s) const {
  o << s << "AlnConstants::print()" << std::endl;
  o << s << " TDC unit = " << _tdcUnit << "ns" << std::endl;

  for(unsigned i(0);i<2;i++) {
    if(i==0) o << s << " X dimension:" << std::endl;
    else     o << s << " Y dimension:" << std::endl;
    for(unsigned j(0);j<4;j++) {
      o << s << "  Layer " << j
	<< ", t at coordinate=0 = " << std::setw(6) << _cTzero[i][j]
	<< " TDC units"
	<< ", drift velocity linear = " << std::setw(8) << _vDrift[i][j] 
	<< " mm/TDC unit"
	<< ", quadratic = " << std::setw(6) << _vDquad[i][j] 
	<< " mm/TDC unit^2" << std::endl;
    }
  }

  return o;
}

const int* AlnConstants::intData() const {
  return 0;
}

int* AlnConstants::intData() {
  return 0;
}

const float* AlnConstants::floatData() const {
  return 0;
}

float* AlnConstants::floatData() {
  return 0;
}

const double* AlnConstants::doubleData() const {
  return &_tdcUnit;
}

double* AlnConstants::doubleData() {
  return &_tdcUnit;
}

}
