#ifndef TrackProjection_HH
#define TrackProjection_HH

#include <string>
#include <iostream>

#include "TMatrixDSym.h"

#include "TrackFitResult.hh"

namespace EVENT {
  class LCGenericObject;
}


namespace TBTrack {

  class TrackProjection : public TrackFitResult {
    
  public:
    TrackProjection();
    TrackProjection(const TBTrack::TrackFitResult &r);
    TrackProjection(const EVENT::LCGenericObject *p);
    
    unsigned xy() const;
    unsigned fb() const;
    unsigned eh() const;
    
    void fitType(unsigned xy, unsigned fb, unsigned eh);

    void hit(unsigned i, int t);
    int  hit(unsigned i) const;

    void set(const EVENT::LCGenericObject *p);
    EVENT::LCGenericObject* get() const;
    
    std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;
    /*
    enum {numberOfInts=1+5};
    const int* intData() const;
    int* intData();
    enum {numberOfFloats=0};
    const float* floatData() const;
    float* floatData();
    enum {numberOfDoubles=6+0};
    const double* doubleData() const;
    double* doubleData();
    */
    
  private:
    int _fitType;
    int _hits[4];
  };
  
}

#endif
