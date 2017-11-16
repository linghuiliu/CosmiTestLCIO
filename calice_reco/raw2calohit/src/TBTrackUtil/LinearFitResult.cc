#include <cassert>
#include <iomanip>

#include "TMath.h"

#include "LinearFitResult.hh"


namespace TBTrack {

  LinearFitResult::LinearFitResult(double c, int n, const TVectorD &p, const TMatrixDSym &e) :
    _chiSquared(c), _numberOfDof(n), _parameters(p), _errors(e) {
    assert(p.GetNrows()==e.GetNrows());
  }
  
  double LinearFitResult::chiSquared() const {
    return _chiSquared;
  }

  int  LinearFitResult::numberOfDof() const {
    return _numberOfDof;
  }

  double LinearFitResult::probability() const {
    return TMath::Prob(_chiSquared,_numberOfDof);
  }
  
  const TVectorD& LinearFitResult::parameters() const {
    return _parameters;
  }
  
  const TMatrixDSym& LinearFitResult::errors() const {
    return _errors;
  }

  double LinearFitResult::probability(const TVectorD &p) const {
    assert(p.GetNrows()==_parameters.GetNrows());
    return TMath::Prob((_parameters-p)*(_errors*(_parameters-p)),p.GetNrows());
  }
  
  std::ostream& LinearFitResult::print(std::ostream &o, const std::string &s) const {
    o << s << "TBTrack::LinearFitResult::print()" << std::endl;
    
    o << " Chi-squared/NumberOfDof = " << std::setw(13)
      << _chiSquared << "/" << _numberOfDof
      << ", probability = " << std::setw(8) << probability() << std::endl;
     
    o << s << " Parameters" << std::endl;
    for(int i(0);i<_parameters.GetNrows();i++) {
      o << std::setw(13) << _parameters(i);
    }
    o << std::endl;

    o << s << " Error matrix";
    for(int i(0);i<_errors.GetNrows();i++) {
      for(int j(0);j<_errors.GetNcols();j++) {
	o << std::setw(13) << _errors(i,j);
      }
      o << std::endl;
    }

    return o;
  }

}
