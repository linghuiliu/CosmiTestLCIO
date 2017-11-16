#ifndef TBTrack_LinearFitResult_HH
#define TBTrack_LinearFitResult_HH

#include <string>
#include <iostream>

#include "TVectorD.h"
#include "TMatrixDSym.h"


// Class contains the result of a linear fit

namespace TBTrack {

  class LinearFitResult {

  public:
    LinearFitResult(double c, int n, const TVectorD &p, const TMatrixDSym &e);  

    // Returns the chi-squared value of the fit
    double chiSquared() const;

    // Returns the number of degrees of freedom
    int numberOfDof() const;
    
    // Returns the probability of the fit
    double probability() const;

    // Returns the N parameters resulting from the fit
    const TVectorD& parameters() const;

    // Returns the NxN error matrix of the fit
    const TMatrixDSym& errors() const;
    
    // Returns the probability comparing the fit result to parameters
    double probability(const TVectorD &p) const;

    std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;
    
    
  private:
    double   _chiSquared;
    int     _numberOfDof;
    TVectorD _parameters;
    TMatrixDSym  _errors;
  };
  
}

#endif
