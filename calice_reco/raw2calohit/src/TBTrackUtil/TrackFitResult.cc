#include <cassert>
#include <iomanip>
#include <cmath>
#include <TMath.h>

#include "TrackFitResult.hh"

namespace TBTrack {

  TrackFitResult::TrackFitResult() {
    memset(this,0,sizeof(TrackFitResult));
  }
  
  TrackFitResult::TrackFitResult(double p0, double p1,
				 const TMatrixDSym &e, double c, unsigned h) {
    parameters(p0,p1);
    errorMatrix(e);
    chiSquared(c);
    hitPattern(h);
  }
  
  void TrackFitResult::parameters(double p0, double p1) {
    _parameters[0]=p0;
    _parameters[1]=p1;
  }
  
  double TrackFitResult::intercept(double z, double a) const {
    const double d(std::cos(a)-_parameters[1]*std::sin(a));
    if(d==0.0) return 1.0e12;
    
    return (_parameters[0]+z*_parameters[1])/d;
  }
  
  double TrackFitResult::gradient(double a) const {
    const double d(std::cos(a)-_parameters[1]*std::sin(a));
    if(d==0.0) return 1.0e12;
    
    return (std::sin(a)+_parameters[1]*std::cos(a))/d;
  }
  
  TMatrixDSym TrackFitResult::errorMatrix(double z, double a) const {
    TMatrixDSym e(2);
    const double d(std::cos(a)-_parameters[1]*std::sin(a));
    if(d==0.0) return e;
    
    const double f((z*std::cos(a)+_parameters[0]*std::sin(a))/d);
    
    e(0,0)=(_errorMatrix[0]+2.0*f*_errorMatrix[1]+f*f*_errorMatrix[2])/(d*d);
    e(0,1)=(_errorMatrix[1]+    f*_errorMatrix[2]                    )/(d*d*d);
    e(1,1)=(_errorMatrix[2]                                          )/(d*d*d*d);
    
    e(1,0)=e(0,1); // Redundent?
    
    return e;
  }
  
  void TrackFitResult::errorMatrix(const TMatrixDSym &e) {
    assert(e.GetNrows()==2 || e.GetNcols()==2);
    
    _errorMatrix[0]=e(0,0);
    _errorMatrix[1]=e(0,1);
    _errorMatrix[2]=e(1,1);
  }
  
  double TrackFitResult::interceptError(double z, double a) const {
    TMatrixDSym e(errorMatrix(z,a));
    return std::sqrt(e(0,0));
  }
  
  double TrackFitResult::gradientError(double a) const {
    TMatrixDSym e(errorMatrix(0.0,a));
    return std::sqrt(e(1,1));
  }
  
  double TrackFitResult::chiSquared() const {
    return _chiSquared;
  }
  
  void TrackFitResult::chiSquared(double c) {
    _chiSquared=c;
  }
  
  int TrackFitResult::numberOfDof() const {
    int h(-2);
    for(unsigned i(0);i<6;i++) if((_hitPattern&(1<<i))!=0) h++;
    return h;
  }
  
  double TrackFitResult::probability() const {
    return TMath::Prob(_chiSquared,numberOfDof());
  }
  
  unsigned TrackFitResult::numberOfHits() const {
    unsigned h(0);
    for(unsigned i(0);i<4;i++) if((_hitPattern&(1<<i))!=0) h++;
    return h;
  }
  
  unsigned TrackFitResult::hitPattern() const {
    return (_hitPattern)&0x3f;
  }
  
  void TrackFitResult::hitPattern(unsigned h) {
    assert(h<64);
    _hitPattern=h;
  }
  
  std::ostream& TrackFitResult::print(std::ostream &o, const std::string &s) const {
    o << s << "TrackFitResult::print()" << std::endl;
    
    o << s << " Track = " << _parameters[0];
    if(_parameters[1]>=0.0) o << " + " <<  _parameters[1] << "*z" << std::endl;
    else                    o << " - " << -_parameters[1] << "*z" << std::endl;
    
    o << s << " Error matrix = "
      << std::setw(13) << _errorMatrix[0]
      << std::setw(13) << _errorMatrix[1] << std::endl
      << "                "
      << std::setw(13) << _errorMatrix[1]
      << std::setw(13) << _errorMatrix[2] << std::endl;
    
    o << s << " Chi-squared/nDof = " << _chiSquared << "/" << numberOfDof()
      << ", probability = " << probability() << std::endl;
    
    const int p(hitPattern());
    o << s << " Number of hits = " << numberOfHits()
      << ", pattern = 0x" << std::hex << p << std::dec << " = ";
    if((p&8)==0) {o << "0";} else {o << "1";}
    if((p&4)==0) {o << "0";} else {o << "1";}
    if((p&2)==0) {o << "0";} else {o << "1";}
    if((p&1)==0) {o << "0";} else {o << "1";}
    o << std::endl;
    
    return o;
  }
  
  const int* TrackFitResult::intData() const {
    return &_hitPattern;
  }
  
  int* TrackFitResult::intData() {
    return &_hitPattern;
  }
  
  const float* TrackFitResult::floatData() const {
    return 0;
  }
  
  float* TrackFitResult::floatData() {
    return 0;
  }
  
  const double* TrackFitResult::doubleData() const {
    return _parameters;
  }
  
  double* TrackFitResult::doubleData() {
    return _parameters;
  }
  
}
