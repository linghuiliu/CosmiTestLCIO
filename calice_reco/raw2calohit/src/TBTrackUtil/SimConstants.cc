#include <cassert>
#include <iomanip>

#include "SimConstants.hh"


namespace TBTrack {

SimConstants::SimConstants(int p) :
  AlnConstants(p,1), _period(p) {

  assert(sizeof(SimConstants)==
	 sizeof(int)*numberOfInts+
	 sizeof(float)*numberOfFloats+
	 sizeof(double)*numberOfDoubles);

  if(_period==0) {
    for(unsigned xy(0);xy<2;xy++) {
      for(unsigned layer(0);layer<4;layer++) {
	_cEffic[xy][layer]=0.7;
	_cNoise[xy][layer]=0.02;
	_cSmear[xy][layer]=0.4;
	_tTzero[xy][layer]=(9000.0-1000.0*layer)/300.0;
      }
    }

  
  } else {
    for(unsigned xy(0);xy<2;xy++) {
      for(unsigned layer(0);layer<3;layer++) {
	_cEffic[xy][layer]=0.9;
	_cNoise[xy][layer]=0.01;
	_cSmear[xy][layer]=0.2;
	_tTzero[xy][layer]=(40000.0-1000.0*layer)/300.0;
      }
      _cEffic[xy][3]=0.0;
      _cNoise[xy][3]=0.0;
      _cSmear[xy][3]=0.0;
      _tTzero[xy][3]=0.0;
    }
  }
}

double SimConstants::cEffic(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _cEffic[d][l];
}

void SimConstants::cEffic(unsigned d, unsigned l, double e) {
  assert(d<=1 && l<=3);
  _cEffic[d][l]=e;
}

double SimConstants::cSmear(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _cSmear[d][l];
}

void SimConstants::cSmear(unsigned d, unsigned l, double s) {
  assert(d<=1 && l<=3);
  _cSmear[d][l]=s;
}

double SimConstants::cNoise(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _cNoise[d][l];
}

void SimConstants::cNoise(unsigned d, unsigned l, double n) {
  assert(d<=1 && l<=3);
  _cNoise[d][l]=n;
}

double SimConstants::tTzero(unsigned d, unsigned l) const {
  assert(d<=1 && l<=3);
  return _tTzero[d][l];
}

void SimConstants::tTzero(unsigned d, unsigned l, double t) {
  assert(d<=1 && l<=3);
  _tTzero[d][l]=t;
}

std::ostream& SimConstants::print(std::ostream &o, const std::string &s) const {
  o << s << "SimConstants::print()" << std::endl;
  o << s << " Data period = " << _period << std::endl;

  AlnConstants::print(o,s+" ");

  for(unsigned xy(0);xy<2;xy++) {
    if(xy==0) o << s << "  X dimension:" << std::endl;
    else      o << s << "  Y dimension:" << std::endl;
    for(unsigned layer(0);layer<4;layer++) {
      o << s << "   Layer " << layer
	<< ", efficiency = " << std::setw(4) << _cEffic[xy][layer]
	<< ", noise rate = " << std::setw(4) << _cNoise[xy][layer] << "/mm"
	<< ", smearing = " << std::setw(4) << _cSmear[xy][layer] << "mm"
	<< ", t0 = " << std::setw(4) << _tTzero[xy][layer] << "ns"
	<< std::endl;
    }
  }

  return o;
}

const int* SimConstants::intData() const {
  return &_period;
}

int* SimConstants::intData() {
  return &_period;
}

const float* SimConstants::floatData() const {
  return 0;
}

float* SimConstants::floatData() {
  return 0;
}

const double* SimConstants::doubleData() const {
  //return _cEffic[0];
  return (const double*)this;
}

double* SimConstants::doubleData() {
  //return _cEffic[0];
  return (double*)this;
}

}
