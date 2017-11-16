#include <cassert>
#include <iomanip>

#include "TMath.h"

#include "TrackProjection.hh"


namespace TBTrack {

  TrackProjection::TrackProjection() {
    /*
      assert(sizeof(TrackProjection)==
      sizeof(int)*numberOfInts+
      sizeof(float)*numberOfFloats+
      sizeof(double)*numberOfDoubles);

      memset(this,0,sizeof(TrackProjection));
    */
  }

  TrackProjection::TrackProjection(const TBTrack::TrackFitResult &r) :
    TBTrack::TrackFitResult(r) {
    /*
      assert(sizeof(TrackProjection)==
      sizeof(int)*numberOfInts+
      sizeof(float)*numberOfFloats+
      sizeof(double)*numberOfDoubles);
    */
  }

  unsigned TrackProjection::xy() const {
    return (_fitType)&0x1;
  }

  unsigned TrackProjection::fb() const {
    return (_fitType>>1)&0x1;
  }

  unsigned TrackProjection::eh() const {
    return (_fitType>>2)&0x1;
  }

  void TrackProjection::fitType(unsigned xy, unsigned fb, unsigned eh) {
    assert(xy<2 && fb<2 && eh<2);
    _fitType=4*eh+2*fb+xy;
  }

  int TrackProjection::hit(unsigned i) const {
    assert(i<4);
    return _hits[i];
  }

  void TrackProjection::hit(unsigned i, int t) {
    assert(i<4);
    _hits[i]=t;
  }

  std::ostream& TrackProjection::print(std::ostream &o, const std::string &s) const {
    o << s << "TrackProjection::print()" << std::endl;

    TBTrack::TrackFitResult::print(o,s+" ");

    o << s << " Fit type = ";
    if(xy()==0) o << "x, ";
    else        o << "y, ";
    if(fb()==0) o << "f, ";
    else        o << "b, ";
    if(eh()==0) o << "e";
    else        o << "h";
    o << std::endl;

    for(unsigned i(0);i<4;i++) {
      o << s << "  Layer " << i << ", TDC value = "
	<< std::setw(6) << _hits[i] << std::endl;
    }

    return o;
  }
  /*
    const int* TrackProjection::intData() const {
    return &_hitPattern;
    }

    int* TrackProjection::intData() {
    return &_hitPattern;
    }

    const float* TrackProjection::floatData() const {
    return 0;
    }

    float* TrackProjection::floatData() {
    return 0;
    }

    const double* TrackProjection::doubleData() const {
    return _parameters;
    }

    double* TrackProjection::doubleData() {
    return _parameters;
    }
  */
}

// There is a reason for doing this here; do not move it!

#ifndef DISABLE_LCIO
#include "IMPL/LCGenericObjectImpl.h"

namespace TBTrack {
  
  TrackProjection::TrackProjection(const EVENT::LCGenericObject *p) {
    set(p);
  }
  
  void TrackProjection::set(const EVENT::LCGenericObject *p) {
    assert(p->getNInt()==6);
    assert(p->getNDouble()==6);
    assert(p->isFixedSize());

    _hitPattern=p->getIntVal(0);
    _fitType=p->getIntVal(1);
    _hits[0]=p->getIntVal(2);
    _hits[1]=p->getIntVal(3);
    _hits[2]=p->getIntVal(4);
    _hits[3]=p->getIntVal(5);

    _parameters[0]=p->getDoubleVal(0);
    _parameters[1]=p->getDoubleVal(1);
    _errorMatrix[0]=p->getDoubleVal(2);
    _errorMatrix[1]=p->getDoubleVal(3);
    _errorMatrix[2]=p->getDoubleVal(4);
    _chiSquared=p->getDoubleVal(5);
  }
  
  EVENT::LCGenericObject* TrackProjection::get() const {
    IMPL::LCGenericObjectImpl *p(new IMPL::LCGenericObjectImpl(6,0,6));

    p->setIntVal(0,_hitPattern);
    p->setIntVal(1,_fitType);
    p->setIntVal(2,_hits[0]);
    p->setIntVal(3,_hits[1]);
    p->setIntVal(4,_hits[2]);
    p->setIntVal(5,_hits[3]);
    
    p->setDoubleVal(0,_parameters[0]);
    p->setDoubleVal(1,_parameters[1]);
    p->setDoubleVal(2,_errorMatrix[0]);
    p->setDoubleVal(3,_errorMatrix[1]);
    p->setDoubleVal(4,_errorMatrix[2]);
    p->setDoubleVal(5,_chiSquared);

    return p;
  }

}

#endif
