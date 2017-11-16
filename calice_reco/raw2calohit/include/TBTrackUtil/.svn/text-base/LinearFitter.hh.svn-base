#ifndef TBTrack_LinearFitter_HH
#define TBTrack_LinearFitter_HH

#include <string>
#include <iostream>

#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"


namespace TBTrack {
  class LinearFitResult;

  class LinearFitter {
    
  public:
    LinearFitter();  
    
    void fitInitialisation(const TMatrixD &z, const TMatrixDSym &e);
    
    const TMatrixDSym&      errorMatrix() const;
    const TMatrixD&      solutionMatrix() const;
    const TMatrixDSym& chiSquaredMatrix() const;
    
    std::ostream& printInitialisation(std::ostream &o=std::cout, const std::string &s="") const;
    
    LinearFitResult fitResult(const TVectorD &x) const;
    /*    
    TVectorD    parameters() const;
    TMatrixDSym parameterErrors() const;
    
    TVectorD    residuals() const;
    
    TVectorD    function() const;
    TVectorD    function(const TMatrixD &z) const;
    
    TMatrixDSym functionErrors() const;
    TMatrixDSym functionErrors(const TMatrixD &z) const;

    double  chiSquared() const;
    int    numberOfDof() const;
    double probability() const;

    std::ostream& printFit(std::ostream &o=std::cout) const;
    */
    
    
  private:
    TMatrixD           _fitValues;
    TMatrixDSym      _errorMatrix;
    TMatrixD      _solutionMatrix;
    TMatrixDSym _chiSquaredMatrix;
    /*    
    TVectorD _functionValues;
    TVectorD _parameters;
    double   _chiSquared;
    */
  };
  
}

#endif
