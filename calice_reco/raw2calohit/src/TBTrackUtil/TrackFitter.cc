#include <cassert>

#include "TMatrixD.h"

#include "LinearFitResult.hh"
#include "LinearFitter.hh"
#include "TrackFitInitialisation.hh"
#include "TrackFitResult.hh"

#include "TrackFitter.hh"

namespace TBTrack {

TrackFitter::TrackFitter() {
  for(unsigned hp(0);hp<64;hp++) _linearFitter[hp]=0;
}

TrackFitter::~TrackFitter() {
  for(unsigned hp(0);hp<64;hp++) delete _linearFitter[hp];
}

void TrackFitter::fitInitialisation(const TrackFitInitialisation &fc) {
  TMatrixDSym em(fc.errorMatrix());

  unsigned mask(0);
  for(int i(0);i<6 && i<em.GetNrows();i++) {
    if(em(i,i)>0.0) mask|=(1<<i);
  }
  
  for(unsigned hp(0);hp<64;hp++) {
    unsigned hitPattern(hp&mask);
    if(hitPattern==hp) {
      unsigned n(0);
      for(unsigned i(0);i<6;i++) if((hitPattern&(1<<i))!=0) n++;
      if(n>=3) {
	
	TMatrixD zf(n,2);
	
	unsigned m(0);
	for(unsigned i(0);i<4;i++) {
	  if((hitPattern&(1<<i))!=0) {
	    zf(m,0)=1.0;
	    zf(m,1)=fc.zLayer(i);
	    m++;
	  }
	}
	
	if((hitPattern&(1<<4))!=0) {
	  zf(m,0)=1.0;
	  zf(m,1)=fc.zBeam();
	  m++;
	}
	
	if((hitPattern&(1<<5))!=0) {
	  zf(m,0)=0.0;
	  zf(m,1)=1.0;
	  m++;
	}
	
	assert(m==n);
	
	TMatrixDSym ef(n);
	
	unsigned mi(0);
	for(unsigned i(0);i<6;i++) {
	  if((hitPattern&(1<<i))!=0) {
	    unsigned mj(0);
	    for(unsigned j(0);j<6;j++) {
	      if((hitPattern&(1<<j))!=0) {
		ef(mi,mj)=em(i,j);
		mj++;
	      }
	    }
	    mi++;
	  }
	}
	
	_linearFitter[hitPattern]=new LinearFitter;
	_linearFitter[hitPattern]->fitInitialisation(zf,ef);
      }
    }
  }

  _beam[0]=fc.beamCoordinate();
  _beam[1]=fc.beamTanAngle();
}

TrackFitResult TrackFitter::fitResult(unsigned h, const TVectorD &c) const {
  assert(h<64);

  if(_linearFitter[h]==0) return TrackFitResult();

  assert(c.GetNrows()==4);

  unsigned n(0);
  for(unsigned i(0);i<6;i++) if((h&(1<<i))!=0) n++;
  TVectorD cf(n);
  
  n=0;
  for(unsigned i(0);i<4;i++) {
    if((h&(1<<i))!=0) {
      cf(n)=c(i);
      n++;
    }
  }

  for(unsigned i(0);i<2;i++) {
    if((h&(1<<(4+i)))!=0) {
      cf(n)=_beam[i];
      n++;
    }
  }

  LinearFitResult r(_linearFitter[h]->fitResult(cf));
  TVectorD p(r.parameters());

  return TrackFitResult(p(0),p(1),
			  //			  _linearFitter[h]->parameterErrors(),
			  r.errors(),
			  r.chiSquared(),h);
}

std::ostream& TrackFitter::print(std::ostream &o, const std::string &s) const {
  o << s << "TrackFitter::print()" << std::endl;
  /*
  for(unsigned xy(0);xy<2;xy++) {
    if(xy==0) o << " X fit: ";
    else      o << " Y fit: ";
    _fitter[xy][0].printInitialisation(o);
  }
  */
  return o;
}

}
