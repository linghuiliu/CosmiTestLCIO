#include <cassert>
#include <iomanip>

#include "TMath.h"

#include "LinearFitResult.hh"
#include "LinearFitter.hh"

namespace TBTrack {

  LinearFitter::LinearFitter() {
  }

  void LinearFitter::fitInitialisation(const TMatrixD &z, const TMatrixDSym &e) {
    assert(e.GetNcols()==z.GetNrows());

    const bool print(false);
    
    const unsigned nLayers(z.GetNrows());
    const unsigned nParams(z.GetNcols());
    
    _fitValues.ResizeTo(z.GetNrows(),z.GetNcols());
    _fitValues=z;
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "z matrix" << std::endl;
      for(unsigned i(0);i<nLayers;i++) {
	for(unsigned j(0);j<nParams;j++) {
	  std::cout << std::setw(13) << z(i,j);
	}
	std::cout << std::endl;
      }
    }
    
    TMatrixD zt(nParams,nLayers); // p x l
    zt.Transpose(z);
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Transposed z matrix" << std::endl;
      for(unsigned i(0);i<nParams;i++) {
	for(unsigned j(0);j<nLayers;j++) {
	  std::cout << std::setw(13) << zt(i,j);
	}
	std::cout << std::endl;
      }
    }
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Layer error matrix" << std::endl;
      for(unsigned i(0);i<nLayers;i++) {
	for(unsigned j(0);j<nLayers;j++) {
	  std::cout << std::setw(13) << e(i,j);
	}
	std::cout << std::endl;
      }
    }
    
    TMatrixDSym Wc(e); // l x l
    Wc.Invert();
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Layer weight matrix" << std::endl;
      for(unsigned i(0);i<nLayers;i++) {
	for(unsigned j(0);j<nLayers;j++) {
	  std::cout << std::setw(13) << Wc(i,j);
	}
	std::cout << std::endl;
    }
    }
    
    TMatrixD v(Wc*z); // l x p
    TMatrixD vt(nParams,nLayers); // p x l
    vt.Transpose(v);
    
    _errorMatrix.ResizeTo(nParams,nParams); // p x p
    _errorMatrix.Zero();
    
    TMatrixD ep(zt*v); // p x p
    //_errorMatrix=zt*v;
    /*
      for(unsigned m(0);m<nParams;m++) {
      for(unsigned n(0);n<nParams;n++) {
      for(unsigned l(0);l<nLayers;l++) {
      _errorMatrix(m,n)+=zt(m,l)*v(l,n);
      }
      }
      }
    */
    for(unsigned m(0);m<nParams;m++) {
      for(unsigned n(0);n<nParams;n++) {
	_errorMatrix(m,n)=ep(m,n);
      }
    }
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Parameter weight matrix" << std::endl;
      for(unsigned i(0);i<nParams;i++) {
	for(unsigned j(0);j<nParams;j++) {
	  std::cout << std::setw(13) << _errorMatrix(i,j);
	}
	std::cout << std::endl;
      }
    }
    
    _errorMatrix.Invert();
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Parameter error matrix" << std::endl;
      for(unsigned i(0);i<nParams;i++) {
	for(unsigned j(0);j<nParams;j++) {
	  std::cout << std::setw(13) << _errorMatrix(i,j);
	}
	std::cout << std::endl;
      }
    }
    
    _solutionMatrix.ResizeTo(nParams,nLayers); // p x l
    _solutionMatrix=_errorMatrix*vt;
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Solution matrix" << std::endl;
      for(unsigned i(0);i<nParams;i++) {
	for(unsigned j(0);j<nLayers;j++) {
	  std::cout << std::setw(13) << _solutionMatrix(i,j);
	}
	std::cout << std::endl;
      }
    }
    
    _chiSquaredMatrix.ResizeTo(nLayers,nLayers); // l x l
    _chiSquaredMatrix=Wc;
    
    //_chiSquaredMatrix-=v*_solutionMatrix;
    
    TMatrixD dc(v*_solutionMatrix);
    for(unsigned i(0);i<nLayers;i++) {
      for(unsigned j(0);j<nLayers;j++) {
	_chiSquaredMatrix(i,j)-=dc(i,j);
      }
    }
    
    if(print) {
      std::cout << "LinearFitter::fitErrors() "
		<< "Chi-squared matrix" << std::endl;
      for(unsigned i(0);i<nLayers;i++) {
	for(unsigned j(0);j<nLayers;j++) {
	  std::cout << std::setw(13) << _chiSquaredMatrix(i,j);
	}
	std::cout << std::endl;
      }
    }
  }
  
  const TMatrixDSym& LinearFitter::errorMatrix() const {
    return _errorMatrix;
  }
  
  const TMatrixD& LinearFitter::solutionMatrix() const {
    return _solutionMatrix;
  }
  
  const TMatrixDSym& LinearFitter::chiSquaredMatrix() const {
    return _chiSquaredMatrix;
  }

  std::ostream& LinearFitter::printInitialisation(std::ostream &o, const std::string &s) const {
    o << "LinearFitter::printInitialisation()" << std::endl;
    
    o << " Fit value matrix" << std::endl;
    for(int i(0);i<_fitValues.GetNrows();i++) {
      o << "  ";
      for(int j(0);j<_fitValues.GetNcols();j++) {
	o << std::setw(13) << _fitValues(i,j);
      }
      o << std::endl;
    }
    
    o << " Parameter error matrix" << std::endl;
    for(int i(0);i<_errorMatrix.GetNrows();i++) {
      o << "  ";
      for(int j(0);j<_errorMatrix.GetNcols();j++) {
	o << std::setw(13) << _errorMatrix(i,j);
      }
      o << std::endl;
    }
    
    o << " Parameter normalised error matrix" << std::endl;
    for(int i(0);i<_errorMatrix.GetNrows();i++) {
      o << "  ";
      for(int j(0);j<_errorMatrix.GetNcols();j++) {
	if(j==i) o << std::setw(13) << sqrt(_errorMatrix(i,i));
	else     o << std::setw(13) << _errorMatrix(i,j)/sqrt(_errorMatrix(i,i)*_errorMatrix(j,j));
      }
      o << std::endl;
    }
    
    o << " Solution matrix" << std::endl;
    for(int i(0);i<_solutionMatrix.GetNrows();i++) {
      o << "  ";
      for(int j(0);j<_solutionMatrix.GetNcols();j++) {
	o << std::setw(13) << _solutionMatrix(i,j);
      }
      o << std::endl;
    }
    
    o << " Chi-squared matrix" << std::endl;
    for(int i(0);i<_chiSquaredMatrix.GetNrows();i++) {
      o << "  ";
      for(int j(0);j<_chiSquaredMatrix.GetNcols();j++) {
	o << std::setw(13) << _chiSquaredMatrix(i,j);
      }
      o << std::endl;
    }
    
    o << std::endl;
    
    return o;
  }
  
  LinearFitResult LinearFitter::fitResult(const TVectorD &x) const {
    assert(_solutionMatrix.GetNcols()  ==x.GetNrows());
    assert(_chiSquaredMatrix.GetNcols()==x.GetNrows());
    
    return LinearFitResult(x*(_chiSquaredMatrix*x),
			   _solutionMatrix.GetNcols()-_solutionMatrix.GetNrows(),
			   _solutionMatrix*x,
			   _errorMatrix);
  }

  /*
  TVectorD LinearFitter::function(const TMatrixD &z) const {
    assert(z.GetNcols()==_parameters.GetNrows());
    return z*_parameters;
  }
  
  TMatrixDSym LinearFitter::functionErrors() const {
    return functionErrors(_fitValues);
  }
  
  TMatrixDSym LinearFitter::functionErrors(const TMatrixD &z) const {
    assert(_errorMatrix.GetNcols()==z.GetNrows());
    
    TMatrixD zt(z.GetNcols(),z.GetNrows());
    zt.Transpose(z);
    
    TMatrixD zez(zt*_errorMatrix*z);
    
    TMatrixDSym e(zez.GetNcols());
    for(int i(0);i<e.GetNrows();i++) {
      for(int j(0);j<e.GetNcols();j++) {
	e(i,j)=zez(i,j);
      }
    }
    
    return e;
  }

  double LinearFitter::chiSquared() const {
    return _chiSquared;
  }

  int LinearFitter::numberOfDof() const {
    return _solutionMatrix.GetNcols()-_solutionMatrix.GetNrows();
  }

  double LinearFitter::probability() const {
    return TMath::Prob(_chiSquared,numberOfDof());
  }

  std::ostream& LinearFitter::printFit(std::ostream &o) const {
    o << "LinearFitter::printFit()" << std::endl;
    
    o << " Parameters = ";
    for(unsigned i(0);i<static_cast<unsigned int>(_parameters.GetNrows());i++) {
      o << std::setw(13) << _parameters(i);
    }
    o << std::endl;
    
    o << " Chi-squared/numberOfDoF = " << std::setw(13)
      << _chiSquared << "/" << numberOfDof()
      << ", probability = " << std::setw(8) << probability() << std::endl;
    o << " Function values, fitted values and residuals" << std::endl;
    
    for(unsigned i(0);i<static_cast<unsigned int>(_functionValues.GetNrows());i++) {
      o << "  Point" << std::setw(6) << i
	<< std::setw(13) << _functionValues(i)
	<< std::setw(13) << function()(i)
	<< std::setw(13) << residuals()(i) << std::endl;
    }
    
    o << std::endl;
    
    return o;
  }
  */

}
