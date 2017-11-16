#ifndef TBTrack_TrackFitResult_HH
#define TBTrack_TrackFitResult_HH

#include <string>
#include <iostream>

#include "TMatrixDSym.h"


namespace TBTrack {

  class TrackFitResult {
    
  public:
    TrackFitResult();  
    TrackFitResult(double p0, double p1, const TMatrixDSym &e,
		   double c, unsigned h);
    
    void parameters(double p0, double p1);
    
    // The intercept and gradient of the track on a sloped plane
    // in the coordinate system within the plane
    // The plane intersection at coordinate = 0 is at z
    // The plane makes an angle a to the coordinate axis
    // so a plane normal to the z axis has a = 0
    
    double intercept(double z=0.0, double a=0.0) const;
    double  gradient(              double a=0.0) const;
    
    // The error matrix of the above quantities
    
    TMatrixDSym errorMatrix(double z=0.0, double a=0.0) const;
    void        errorMatrix(const TMatrixDSym &e);
    
    double interceptError(double z=0.0, double a=0.0) const;
    double  gradientError(              double a=0.0) const;
    
    // The fit chi-squared
    
    double chiSquared() const;
    void   chiSquared(double c);
    
    int numberOfDof() const;
    double probability() const;
    
    unsigned hitPattern() const;
    void     hitPattern(unsigned h);
    
    unsigned numberOfHits() const;
    
    std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;
    
    enum {numberOfInts=0};
    const int* intData() const;
    int* intData();
    
    enum {numberOfFloats=0};
    const float* floatData() const;
    float* floatData();
    
    enum {numberOfDoubles=6};
    const double* doubleData() const;
    double* doubleData();
    
  protected:
    double _parameters[2];
    double _errorMatrix[3];
    double _chiSquared;
    int _hitPattern;
  };

}

#endif
