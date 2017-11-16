#ifndef TBTrackScatter_hpp
#define TBTrackScatter_hpp

#include "TH2F.h"
#include "TVectorD.h"

#include "TBTrackBaseProcessor.hh"

#include "TMath.h"

Double_t TBTrackScatter_twodgausFcn(Double_t *xin, Double_t *params) {
  float x = xin[0];
  float y = xin[1];

  float norm  = params[0];
  float meanx = params[1];
  float meany = params[2];
  float sigx  = params[3];
  float sigy  = params[4];
  float corr  = params[5];

  return (norm/(2.*TMath::Pi()*sigx*sigy*TMath::Sqrt(1.-corr*corr))) *
    TMath::Exp( -(1./(2*(1-corr*corr)))*(TMath::Power((x-meanx)/sigx,2) + TMath::Power((y-meany)/sigy,2)
                                         - 2.*corr/(sigx*sigy)*(x-meanx)*(y-meany) ) );
}

class TBTrackScatter : public TBTrackBaseProcessor {
  
 public:
  
  virtual Processor*  newProcessor() {
    return new TBTrackScatter;
  }
  
  
  TBTrackScatter() ;
  
  virtual void Init() ;
  virtual void ProcessRunHeader(LCRunHeader *run);
  virtual void ProcessEvent(LCEvent *evt); 
  virtual void End() ;

  TMatrixDSym findError(const std::vector<TVectorD> &v, double cutProb=0.1, bool mean=true);
  void findError2();
  
  virtual void initHists() ;


 private:

  int _oldRun;

  static const float _rotationAngle;

  std::string _simFakeTrackerHitCollection;

  enum {
    NLAYER=4,
    NPM=14, NDM=6,
    NEXTRAP=3
  };

  class TBEvent {
  public:
    TBEvent() {
      for (int xy=0; xy<2; xy++) {
	mcProdPos[xy]=-999;
	fakePos[xy]=-999;
	mcProdDir[xy]=-999;
	fakeDir[xy]=-999;
	for (int il=0; il<NLAYER; il++) {
	  hitPos[xy][il]=-999;
	  hitDir[xy][il]=-999;
	  fakeExtrapPos[xy][il]=-999;
	  mcExtrapPos[xy][il]=-999;
	}
      }
    }

    double hitPos[2][NLAYER];
    double mcProdPos[2];
    double fakePos[2];

    double hitDir[2][NLAYER];
    double mcProdDir[2];
    double fakeDir[2];

    double fakeExtrapPos[2][NLAYER];
    double mcExtrapPos[2][NLAYER];
  };

  void getHitInfo( LCEvent * evt );
  void fillHistograms(TBEvent* tbEvent);
  void fillHistograms();

  double getTruncatedRMS(TH1F* h, float factor = 20.);

  std::vector<TVectorD> _vDelta[2];

  std::vector <TBEvent> _vTBEvent;

  TMatrixD _rotation;

  /*
  TH1F* _hPos1d[2][NPM];
  TH2F* _hPos2d[2][NPM*(NPM-1)/2];
  TH2F* _hPos2dRot[2][NPM*(NPM-1)/2];
  TH1F* _hDir1d[2][NDM];
  TH2F* _hDir2d[2][NDM*(NDM-1)/2];
  TH2F* _hDir2dRot[2][NDM*(NDM-1)/2];
  */

  TH1F* _hZpos[2][NLAYER+1];


  TH1F* _NEW_hPos1d_fake[2];
  TH1F* _NEW_hPos1d_mc[2];
  TH1F* _NEW_hPos1d[2][NEXTRAP][NLAYER];

  TH2F* _NEW_hPos2d[2][NEXTRAP][NLAYER*(NLAYER-1)/2];
  TH2F* _NEW_hPos2d_rot[2][NEXTRAP][NLAYER*(NLAYER-1)/2];


} ;

#endif



