#ifndef TBTrack_FitConstants_HH
#define TBTrack_FitConstants_HH

#include <string>
#include <iostream>

#include "TMatrixDSym.h"

#include "TrackFitInitialisation.hh"

// new FitConstants structure - d. jeans 12/10

//Forces the compiler to aling this class on a 32 bit boundary
//Needed to protect implemented type 'safety' check
#pragma pack (4)
namespace TBTrack {

class FitConstants {

public:
  enum Particle {
    electron,
    hadron
  };

  FitConstants(unsigned e=1, double err=0.0);  

  // Nominal momentum of beam
  double pBeam() const;
  void   pBeam(double p);

  void pBeamScale(double p);
 
  // z position of first calorimeter
  double zCalorimeter() const;
  void   zCalorimeter(double z);
 
  // z position of beam origin
  double zBeam() const;
  void   zBeam(double z);
 
  // Average beam coordinate
  double beamCoordinate(unsigned d) const;
  void   beamCoordinate(unsigned d, double c);
 
  // Average beam angle
  double beamAngle(unsigned d) const;
  void   beamAngle(unsigned d, double a);

  // Insert beam position and angle at once
  void beamAverage(unsigned d, double c, double a);

   // Beam spread error matrix
  TMatrixDSym beamSpread(unsigned d) const;
  void        beamSpread(unsigned d, const TMatrixDSym &e);

  // z of each layer
  double zLayer(unsigned d, unsigned l) const;
  void   zLayer(unsigned d, unsigned l, double z);

  // Forward scattering error matrix  
  // 6x6; layers 0-3, beam coordinate, beam angle
  TMatrixDSym forwardScattering(unsigned d, Particle p) const;
  void        forwardScattering(unsigned d, Particle p, const TMatrixDSym &e);

  // Backward scattering error matrix  
  // 6x6; cal coordinate, cal angle, layers 0-3
  TMatrixDSym backwardScattering(unsigned d, Particle p) const;
  void        backwardScattering(unsigned d, Particle p, const TMatrixDSym &e);

  // Intrinsic error
  double cError(unsigned d, unsigned l) const;
  void   cError(unsigned d, unsigned l, double e);

  // Probability cuts for track finding for nDof
  double probabilityCut(unsigned nDof) const;
  void   probabilityCut(unsigned nDof, double c);

  // z values and total error matrix for fit
  // 6x6; layers 0-3, beam coordinate, beam angle
  TrackFitInitialisation fitInitialisation(unsigned xy, unsigned fb, unsigned eh) const;

  // Error matrix for alignment using beam
  //TMatrixDSym alignmentMatrix(unsigned xy, unsigned eh, unsigned fb) const;

  // Write out a file for inclusion in code
  void writeIcc() const;

  std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;


  enum {numberOfInts=0};
  const int* intData() const;
        int* intData();
  enum {numberOfFloats=0};
  const float* floatData() const;
        float* floatData();
  enum {numberOfDoubles=3 + 2*2 + 2*3 + 2*4 + 2*2*21 + 2*2*21 + 2*4 + 4};
  const double* doubleData() const;
        double* doubleData();

private:
  void convertErrorMatrix(const double *d, TMatrixDSym &e) const;
  void convertErrorMatrix(const TMatrixDSym &e, double *d);
  //  void pBeamScale(double p);
  void pBeamScale(double* mat, double p);

  double _pBeam;
  double _zCalorimeter;
  double _zBeam;
  double _beamAverage[2][2];
  double _beamSpread[2][3];

  double _zLayer[2][4];
  double _forwardScattering[2][2][21];
  double _backwardScattering[2][2][21];

  double _cError[2][4];
  double _probabilityCut[4];
};

}
//Reset the alignment
#pragma pack()
#endif
