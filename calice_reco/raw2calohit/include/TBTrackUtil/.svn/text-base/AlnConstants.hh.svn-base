#ifndef TBTrack_AlnConstants_HH
#define TBTrack_AlnConstants_HH

// Constants needed for conversion of TDC values
// to space coordinates (and vice versa for MC)

#include <string>
#include <iostream>


//Forces the compiler to aling this class on a 32 bit boundary
//Needed to protect implemented type 'safety' check
#pragma pack (4)
namespace TBTrack {

class AlnConstants {

public:
  //AlnConstants(bool desy=true, bool data=true);
  AlnConstants(int period=0, int dataType=2);

  // TDC unit in ns (info only; not needed for alignment)
  double tdcUnit() const;
  void   tdcUnit(double t);

  // d is dimension; 0 = x, 1 = y
  // l is layer; 0-3 for DESY, 0-2 for CERN

  // Coordinate when TDC value = 0 in mm
  double cTzero(unsigned d, unsigned l) const;
  void   cTzero(unsigned d, unsigned l, double c);

  // Drift velocity in mm/TDC units
  double vDrift(unsigned d, unsigned l) const;
  void   vDrift(unsigned d, unsigned l, double v);
  
  // Drift velocity quadratic term in mm/TDC units^2
  double vDquad(unsigned d, unsigned l) const;
  void   vDquad(unsigned d, unsigned l, double q);
  
  // Conversion from TDC value to coordinate in mm
  double coordinate(unsigned d, unsigned l, int t) const;

  // Conversion from coordinate (with time) to TDC value (for MC)
  int tdcValue(unsigned d, unsigned l, double c, double t=0.0) const;

  std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;

  enum {numberOfInts=0};
  const int* intData() const;
        int* intData();

  enum {numberOfFloats=0};
  const float* floatData() const;
        float* floatData();

  enum {numberOfDoubles=1+3*2*4};
  const double* doubleData() const;
        double* doubleData();

protected:

  // Tracking chamber constants
  double _tdcUnit;
  double _cTzero[2][4];
  double _vDrift[2][4];
  double _vDquad[2][4];
};

}

//Reset the alignment
#pragma pack()
#endif
