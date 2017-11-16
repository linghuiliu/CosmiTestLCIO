#ifndef TBTrack_SimConstants_HH
#define TBTrack_SimConstants_HH

// Constants needed for simulation of tracking chambers

#include <string>
#include <iostream>

#include "AlnConstants.hh"

//Forces the compiler to aling this class on a 32 bit boundary
//Needed to protect implemented type 'safety' check
#pragma pack (4)
namespace TBTrack {

class SimConstants : public AlnConstants {

public:
  SimConstants(int p=0);

  // d is dimension; 0 = x, 1 = y
  // l is layer; 0-3 for DESY, 0-2 for CERN
  
  // Intrinsic chamber efficiency
  double cEffic(unsigned d, unsigned l) const;
  void   cEffic(unsigned d, unsigned l, double e);

  // Chamber hit smearing
  double cSmear(unsigned d, unsigned l) const;
  void   cSmear(unsigned d, unsigned l, double s);

  // Noise rate per mm per event
  double cNoise(unsigned d, unsigned l) const;
  void   cNoise(unsigned d, unsigned l, double n);

  // Effective t0 in ns
  double tTzero(unsigned d, unsigned l) const;
  void   tTzero(unsigned d, unsigned l, double t);

  std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;

  enum {numberOfInts=1};
  const int* intData() const;
        int* intData();

  enum {numberOfFloats=0};
  const float* floatData() const;
        float* floatData();

  enum {numberOfDoubles=4*2*4 + AlnConstants::numberOfDoubles};
  const double* doubleData() const;
        double* doubleData();

private:

  // Tracking chamber simulation constants
  double _cEffic[2][4];
  double _cSmear[2][4];
  double _cNoise[2][4];
  double _tTzero[2][4];
  int _period;
};

}
//Reset the alignment
#pragma pack()
#endif
