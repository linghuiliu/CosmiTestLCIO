#include <cassert>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "FitConstants.hh"

// new FitConstants structure - d. jeans 12/10

namespace TBTrack {

  FitConstants::FitConstants(unsigned e, double err) {

    assert(sizeof(FitConstants)==
	   sizeof(int)*numberOfInts+
	   sizeof(float)*numberOfFloats+
	   sizeof(double)*numberOfDoubles);

    memset(this,0,sizeof(FitConstants));

    // DESY
    /*
    for(unsigned i(0);i<2;i++) {
      double offset(44.0*(i-0.5));
      _zLayer[i][0]=-  50.0+offset;
      _zLayer[i][1]=-1150.0+offset;
      _zLayer[i][2]=-2060.0+offset;
      _zLayer[i][3]=-3160.0+offset;
    }
    */
    //CERN
    /*
      for(unsigned i(0);i<2;i++) {
      double offset(29.0*(i-0.5));
      _zLayer[i][0]=-  29.0+offset;
      _zLayer[i][1]=- 680.0+offset;
      _zLayer[i][2]=-2528.0+offset;
      _zLayer[i][3]=0.0;
      }

    */

    /*
    //assert(e==1 || e==6);


    for(unsigned i(0);i<2;i++) {
    for(unsigned j(0);j<2;j++) {
    if(i==0) {
    if(e==1) {
    _errorMatrix[i][j][0]=0.0862472;
    _errorMatrix[i][j][1]=0.492796;
    _errorMatrix[i][j][2]=0.825323;
    _errorMatrix[i][j][3]=1.18654;
    _errorMatrix[i][j][4]=7.45158;
    _errorMatrix[i][j][5]=14.023;
    _errorMatrix[i][j][6]=21.3166;
    _errorMatrix[i][j][7]=27.1954;
    _errorMatrix[i][j][8]=41.9325;
    _errorMatrix[i][j][9]=65.6207;
    }

    if(e==6) {
    _errorMatrix[i][j][0]=0.00240193;
    _errorMatrix[i][j][1]=0.0137035;
    _errorMatrix[i][j][2]=0.0228085;
    _errorMatrix[i][j][3]=0.0325186;
    _errorMatrix[i][j][4]=0.211231;
    _errorMatrix[i][j][5]=0.399674;
    _errorMatrix[i][j][6]=0.611703;
    _errorMatrix[i][j][7]=0.792795;
    _errorMatrix[i][j][8]=1.25183;
    _errorMatrix[i][j][9]=2.03755;
    }

    } else {
    if(e==1) {
    _errorMatrix[i][j][0]=0.0573164;
    _errorMatrix[i][j][1]=0.246205;
    _errorMatrix[i][j][2]=0.390155;
    _errorMatrix[i][j][3]=0.53892;
    _errorMatrix[i][j][4]=6.73472;
    _errorMatrix[i][j][5]=12.917;
    _errorMatrix[i][j][6]=19.7901;
    _errorMatrix[i][j][7]=25.5962;
    _errorMatrix[i][j][8]=39.8029;
    _errorMatrix[i][j][9]=62.8816;
    }

    if(e==6) {
    _errorMatrix[i][j][0]=0.00162354;
    _errorMatrix[i][j][1]=0.0066976;
    _errorMatrix[i][j][2]=0.0102734;
    _errorMatrix[i][j][3]=0.0135224;
    _errorMatrix[i][j][4]=0.193088;
    _errorMatrix[i][j][5]=0.372717;
    _errorMatrix[i][j][6]=0.575784;
    _errorMatrix[i][j][7]=0.754782;
    _errorMatrix[i][j][8]=1.20348;
    _errorMatrix[i][j][9]=1.98394;
    }
    }

    // Add on intrinsic resolution
    _errorMatrix[i][j][0]+=err*err;
    _errorMatrix[i][j][4]+=err*err;
    _errorMatrix[i][j][7]+=err*err;
    _errorMatrix[i][j][9]+=err*err;
    }
    }
    */
  }

  double FitConstants::pBeam() const {
    return _pBeam;
  }

  void FitConstants::pBeam(double p) {
    _pBeam=p;
  }


  void FitConstants::pBeamScale(double p) {
    // rescale the scattering matrix
    // pBeam is the momentum for the calculated matrix
    // p is the new momentum

    // matrix elements go like 1/p*p
    double scale( (_pBeam*_pBeam) / (p*p) );

    // update the momentum in FitConstants, to make sure we know what momentum they are for...
    _pBeam=p;

    for(unsigned xy(0);xy<2;xy++) {
      for(unsigned eh(0);eh<2;eh++) {
        for(unsigned i(0);i<21;i++) {
          if( _forwardScattering[xy][eh][i]>0.0) _forwardScattering[xy][eh][i]*=scale;
          if(_backwardScattering[xy][eh][i]>0.0) _backwardScattering[xy][eh][i]*=scale;
        }
      }
    }
    return;
  }

  double FitConstants::zBeam() const {
    return _zBeam;
  }

  void FitConstants::zBeam(double z) {
    _zBeam=z;
  }

  double FitConstants::zCalorimeter() const {
    return _zCalorimeter;
  }

  void FitConstants::zCalorimeter(double z) {
    _zCalorimeter=z;
  }

  double FitConstants::beamCoordinate(unsigned d) const {
    assert(d<=1);
    return _beamAverage[d][0];
  }

  void FitConstants::beamCoordinate(unsigned d, double c) {
    assert(d<=1);
    _beamAverage[d][0]=c;
  }

  double FitConstants::beamAngle(unsigned d) const {
    assert(d<=1);
    return _beamAverage[d][1];
  }

  void FitConstants::beamAngle(unsigned d, double a) {
    assert(d<=1);
    _beamAverage[d][1]=a;
  }

  void FitConstants::beamAverage(unsigned d, double c, double a) {
    assert(d<=1);
    _beamAverage[d][0]=c;
    _beamAverage[d][1]=a;
  }

  TMatrixDSym FitConstants::beamSpread(unsigned d) const {
    assert(d<=1);
    TMatrixDSym e(2);
    convertErrorMatrix(_beamSpread[d],e);
    return e;
  }

  double FitConstants::zLayer(unsigned d, unsigned l) const {
    assert(d<=1 && l<=3);
    return _zLayer[d][l];
  }

  void FitConstants::zLayer(unsigned d, unsigned l, double z) {
    assert(d<=1 && l<=3);
    _zLayer[d][l]=z;
  }

  void FitConstants::beamSpread(unsigned d, const TMatrixDSym &e) {
    assert(d<=1 && e.GetNrows()==2);
    return convertErrorMatrix(e,_beamSpread[d]);
  }

  TMatrixDSym FitConstants::forwardScattering(unsigned d, Particle p) const {
    assert(d<=1 && p>=electron && p<=hadron);
    TMatrixDSym e(6);
    convertErrorMatrix(_forwardScattering[d][p],e);
    return e;
  }

  void FitConstants::forwardScattering(unsigned d, Particle p, const TMatrixDSym &e) {
    assert(d<=1 && p>=electron && p<=hadron && e.GetNrows()==6);
    return convertErrorMatrix(e,_forwardScattering[d][p]);
  }

  TMatrixDSym FitConstants::backwardScattering(unsigned d, Particle p) const {
    assert(d<=1 && p>=electron && p<=hadron);
    TMatrixDSym e(6);
    convertErrorMatrix(_backwardScattering[d][p],e);
    return e;
  }

  void FitConstants::backwardScattering(unsigned d, Particle p, const TMatrixDSym &e) {
    assert(d<=1 && p>=electron && p<=hadron && e.GetNrows()==6);
    return convertErrorMatrix(e,_backwardScattering[d][p]);
  }
  /*
    TMatrixDSym FitConstants::errorMatrix(unsigned d, Particle p) const {
    assert(d<=1 && p>=electron && p<=hadron);

    // Get forward scattering part
    TMatrixDSym e(6);
    convertErrorMatrix(_forwardScattering[d][p],e);

    // Add on beam spread
    e(4,4)+=_beamSpread[d][0];
    e(4,5)+=_beamSpread[d][1];
    e(5,4)+=_beamSpread[d][1];
    e(5,5)+=_beamSpread[d][2];

    return e;
    }
  */

  double FitConstants::cError(unsigned d, unsigned l) const {
    assert(d<=1 && l<=3);
    return _cError[d][l];
  }

  void FitConstants::cError(unsigned d, unsigned l, double z) {
    assert(d<=1 && l<=3);
    _cError[d][l]=z;
  }

  double FitConstants::probabilityCut(unsigned nDof) const {
    assert(nDof>0 && nDof<=4);
    return _probabilityCut[nDof-1];
  }

  void FitConstants::probabilityCut(unsigned nDof, double c) {
    assert(nDof>0 && nDof<=4);
    _probabilityCut[nDof-1]=c;
  }



  TBTrack::TrackFitInitialisation FitConstants::fitInitialisation(unsigned xy, unsigned fb, unsigned eh) const {
    assert(xy<2 && fb<2 && eh<2);

    TBTrack::TrackFitInitialisation fe;
    for(unsigned i(0);i<4;i++) fe.zLayer(i,_zLayer[xy][i]);
    fe.zBeam(_zBeam);
    fe.beamAverage(_beamAverage[xy][0],_beamAverage[xy][1]);

    TMatrixDSym e(6);
    if(fb==0) {

      convertErrorMatrix(_forwardScattering[xy][eh],e);

      TMatrixDSym b(2);
      convertErrorMatrix(_beamSpread[xy],b);
      if(e(4,4)>=0.0 && e(5,5)>=0.0 &&
	 b(0,0)>=0.0 && b(1,1)>=0.0) {
	e(4,4)+=b(0,0);
	e(4,5)+=b(0,1);
	e(5,5)+=b(1,1);

	e(5,4)=e(4,5);

      } else {
	for(unsigned i(4);i<6;i++) {
	  for(unsigned j(0);j<6;j++) {
	    e(j,i)=0.0;
	    e(i,j)=0.0;
	  }
	  e(i,i)=-1.0;
	}
      }

    } else {

      TMatrixDSym b(6);
      convertErrorMatrix(_backwardScattering[xy][eh],b);

      e.Zero();
      for(unsigned i(0);i<4;i++) {
	for(unsigned j(0);j<4;j++) {
	  e(i,j)=b(i+2,j+2);
	}
      }
 
      e(4,4)=-1.0;
      e(5,5)=-1.0;
    }  

    for(unsigned i(0);i<4;i++) {
      if(e(i,i)>=0.0) e(i,i)+=_cError[xy][i]*_cError[xy][i];
    }

    fe.errorMatrix(e);
    return fe;
  }

  /*  
  TMatrixDSym FitConstants::alignmentMatrix(unsigned xy, unsigned eh, unsigned fb) const {
    assert(xy<2 && eh<2 && fb<2);

    if(fb==0) {
      TMatrixDSym b(6);
      convertErrorMatrix(_forwardScattering[xy][eh],b);
      return b;

    } else { 
      TMatrixDSym b(4);
      convertErrorMatrix(_backwardScattering[xy][eh],b);
      
      TMatrixDSym e(2);
      convertErrorMatrix(_beamSpread[xy],e);
      
      if(e(0,0)>=0.0 && e(1,1)>=0.0) {
	for(unsigned i(0);i<4;i++) {
	  if(b(i,i)>=0.0) {
	    for(unsigned j(0);j<4;j++) {
	      if(b(j,j)>=0.0) {
		b(i,j)+=e(0,0)
		  +e(0,1)*(_zLayer[xy][i]-_zBeam)
		  +e(1,0)*(_zLayer[xy][j]-_zBeam)
		  +e(1,1)*(_zLayer[xy][i]-_zBeam)*(_zLayer[xy][j]-_zBeam);
	      }
	    }
	  }
	}
      }

      for(unsigned i(0);i<4;i++) {
	if(b(i,i)>=0.0) b(i,i)+=_cError[xy][i]*_cError[xy][i];
      }

      return b;
    }
  }
  */  
  void FitConstants::writeIcc() const {
    std::ostringstream sout;
    sout << "FitConstants" << (unsigned)(10.0*_pBeam+0.5) << "GeV.icc";
    std::ofstream fout(sout.str().c_str());
 
    fout << "      pBeam(" << _pBeam << ");" << std::endl;
    fout << "      f.zBeam(" << _zBeam << ");" << std::endl;
    fout << std::endl;
 
    for(unsigned xy(0);xy<2;xy++) {
      fout << "      f.beamAverage(" << xy << "," << _beamAverage[xy][0]
	   << "," << _beamAverage[xy][1] << ");" << std::endl;
      fout << std::endl;
 
      for(unsigned i(0);i<4;i++) {
	fout << "      f.zLayer(" << xy << "," << i << ","
	     << _zLayer[xy][i] << ");" << std::endl;
      }
      fout << std::endl;
 
      for(unsigned i(0);i<4;i++) {
	fout << "      f.cError(" << xy << "," << i << ","
	     << _cError[xy][i] << ");" << std::endl;
      }
      fout << std::endl;
 
      TMatrixDSym bp(beamSpread(xy));
      fout << "      TMatrixDSym bp" << xy << "(2);" << std::endl;
      for(unsigned i(0);i<2;i++) {
	for(unsigned j(0);j<2;j++) {
	  fout << "      bp" << xy << "(" << i << "," << j << ")="
	       << bp(i,j) << ";" <<std::endl;
	}
      }
      fout << "      f.beamSpread(" << xy << ",bp" << xy << ");" << std::endl;
      fout << std::endl;
 
      for(unsigned eh(0);eh<2;eh++) {
	TBTrack::FitConstants::Particle p((TBTrack::FitConstants::Particle)eh);
	
	TMatrixDSym fs(forwardScattering(xy,p));
	fs.Zero(); // CERN!
	fout << "      TMatrixDSym fs" << xy << eh << "(6);" << std::endl;
	for(unsigned i(0);i<6;i++) {
	  for(unsigned j(0);j<6;j++) {
	    fout << "      fs" << xy << eh << "(" << i << "," << j << ")=";
	    //if(eh==0) {
	    fout << fs(i,j) << ";" <<std::endl;
	    //} else {
	    //  if(i==j) fout << "-1.0;" << std::endl;
	    // else     fout << " 0.0;" << std::endl;
	    // }
	  }
	}
	fout << "      f.forwardScattering(" << xy << ",";
	if(eh==0) fout << "TBTrack::FitConstants::electron";
	else      fout << "TBTrack::FitConstants::hadron";
	fout << ",fs" << xy << eh << ");" << std::endl;
	fout << std::endl;
	
	TMatrixDSym bs(backwardScattering(xy,p));
	bs.Zero(); // CERN!
	fout << "      TMatrixDSym bs" << xy << eh << "(4);" << std::endl;
	for(unsigned i(0);i<4;i++) {
	  for(unsigned j(0);j<4;j++) {
	    fout << "      bs" << xy << eh << "(" << i << "," << j << ")=";
	    //if(eh==0) {
	    fout << bs(i,j) << ";" <<std::endl;
	    //} else {
	    //if(i==j) fout << "-1.0;" << std::endl;
	    //else     fout << " 0.0;" << std::endl;
	    //}
	  }
	}
	fout << "      f.backwardScattering(" << xy << ",";
	if(eh==0) fout << "TBTrack::FitConstants::electron";
	else      fout << "TBTrack::FitConstants::hadron";
	fout << ",bs" << xy << eh << ");" << std::endl;
	fout << std::endl;
      }
    }
 
    for(unsigned i(0);i<4;i++) {
      fout << "      f.probabilityCut(" << i+1 << ","
	   << _probabilityCut[i] << ");" << std::endl;
    }
    fout << std::endl;
  }
  
  std::ostream& FitConstants::print(std::ostream &o, const std::string &s) const {
    o << "FitConstants::print()" << std::endl;
    o << " Nominal momentum of beam = " << _pBeam << " GeV" << std::endl;
    o << " Z of first calorimeter = " << _zCalorimeter << " mm" << std::endl;
    o << " Z of nominal beam origin = " << _zBeam << " mm" << std::endl;

    o << " Probability cut for nDof 1-4 =";
    for(unsigned i(0);i<4;i++) {
      o << std::setw(10) << _probabilityCut[i];
    }
    o << std::endl;

    for(unsigned i(0);i<2;i++) {
      if(i==0) o << " X dimension" << std::endl;
      else     o << " Y dimension" << std::endl;

      o << " Beam average position and tan(angle) = " << _beamAverage[i][0]
	<< " mm, " <<  _beamAverage[i][1] << std::endl;
      
      TMatrixDSym b(beamSpread(i));
      o << " Beam spread matrix" << std::endl;
      for(unsigned j(0);j<2;j++) {
	o << "  ";
	for(unsigned k(0);k<2;k++) {
	  o << std::setw(13) << b(j,k);
	}
	o << std::endl;
      }
	
      o << " Tracking layer z values" << std::endl;
      for(unsigned j(0);j<4;j++) {
	o << std::setw(13) << _zLayer[i][j];
      }
      o << std::endl;

      o << " Tracking layer intrinsic errors" << std::endl;
      for(unsigned j(0);j<4;j++) {
	o << std::setw(13) << _cError[i][j];
      }
      o << std::endl;

      TMatrixDSym e(forwardScattering(i,electron));
      o << " Electron forward error matrix" << std::endl;
      for(unsigned j(0);j<6;j++) {
	for(unsigned k(0);k<6;k++) {
	  o << std::setw(13) << e(j,k);
	}
	o << std::endl;
      }

      e=backwardScattering(i,electron);
      o << " Electron backward error matrix" << std::endl;
      for(unsigned j(0);j<6;j++) {
	for(unsigned k(0);k<6;k++) {
	  o << std::setw(13) << e(j,k);
	}
	o << std::endl;
      }

      e=forwardScattering(i,hadron);
      o << " Hadron forward error matrix" << std::endl;
      for(unsigned j(0);j<6;j++) {
	for(unsigned k(0);k<6;k++) {
	  o << std::setw(13) << e(j,k);
	}
	o << std::endl;
      }

      e=backwardScattering(i,hadron);
      o << " Hadron backward error matrix" << std::endl;
      for(unsigned j(0);j<6;j++) {
	for(unsigned k(0);k<6;k++) {
	  o << std::setw(13) << e(j,k);
	}
	o << std::endl;
      }

      if(i==0) o << std::endl;
    }

    return o;
  }

  void FitConstants::convertErrorMatrix(const double *d, TMatrixDSym &e) const {

    assert(d!=0);

    unsigned n(0);
    for(int i(0);i<e.GetNrows();i++) {
      for(int j(0);j<=i;j++) {
	e(i,j)=d[n];
	e(j,i)=d[n];
	n++;
      }
    }
  }

  void FitConstants::convertErrorMatrix(const TMatrixDSym &e, double *d) {
    assert(d!=0);

    unsigned n(0);
    for(int i(0);i<e.GetNrows();i++) {
      for(int j(0);j<=i;j++) {
	d[n]=e(i,j);
	n++;
      }
    }
  }

  const int* FitConstants::intData() const {
    return 0;
  }
 
  int* FitConstants::intData() {
    return 0;
  }
 
  const float* FitConstants::floatData() const {
    return 0;
  }
 
  float* FitConstants::floatData() {
    return 0;
  }
 
  const double* FitConstants::doubleData() const {
    return &_pBeam;
  }
 
  double* FitConstants::doubleData() {
    return &_pBeam;
  }

}
