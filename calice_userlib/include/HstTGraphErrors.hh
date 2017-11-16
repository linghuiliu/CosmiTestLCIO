#ifndef HstTGraphErrors_HH
#define HstTGraphErrors_HH

#include "TGraphErrors.h"


class HstTGraphErrors : public TGraphErrors {

public:
  HstTGraphErrors() : TGraphErrors() {
    SetMarkerColor(1);
    SetMarkerStyle(20);
  }

  virtual ~HstTGraphErrors() {
  }

  void AddPoint(Double_t x, Double_t y, Double_t ex, Double_t ey) {
    Int_t n(GetN());
    Set(n+1);
    SetPoint(n,x,y);
    SetPointError(n,ex,ey);
  }
};

#endif
