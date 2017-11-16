#include "TConvolution.h"

ClassImp(TConvolution)  // magic ROOT stuff

  TConvolution::TConvolution() {
}

TConvolution::TConvolution(const char* name, TF1* f1, TF1* f2, bool addScale, Int_t stepsPerPoint): _convolution(0) {
  Init(name,f1,f2,addScale,stepsPerPoint);
}

TConvolution::~TConvolution() {
  if (_convolution) delete _convolution;
  if (_f1) delete _f1;
  if (_f2) delete _f2;
}


void TConvolution::Init(const char* name, TF1* f1, TF1* f2, bool addScale, Int_t stepsPerPoint) {

  _name = std::string(name);

  _f1 = (TF1*) f1->Clone();
  _nPar1 = _f1->GetNpar();
  _f1->GetRange(_f1Rmin,_f1Rmax);

  _f2 = (TF1*) f2->Clone();
  _nPar2 = _f2->GetNpar();
  _f2->GetRange(_f2Rmin,_f2Rmax);

  _baseNpar = _nPar1+_nPar2;

  _Rmin = _f1Rmin + _f2Rmin;
  _Rmax = _f1Rmax + _f2Rmax;

  _stepsPerPoint = stepsPerPoint;

  _addScale = addScale;

}


TF1* TConvolution::GetConvolutedFunction() {

  int extraNpar=0;
  if (_addScale) _scaleParNumber = _baseNpar + extraNpar++;

  if (_convolution) delete _convolution;
  _convolution = new TF1(_name.c_str(),this,_Rmin,_Rmax,_baseNpar+extraNpar,"TConvolution");

  for (Int_t i=0; i< _nPar1;i++) {
    _convolution->SetParameter(i,_f1->GetParameter(i));
    _convolution->SetParName(i,((std::string(_f1->GetName())) + "_" + _f1->GetParName(i) ).c_str());
  }
  for (Int_t i=0; i< _nPar2;i++) {
    _convolution->SetParameter(i+_nPar1,_f2->GetParameter(i));
    _convolution->SetParName(i+_nPar1,((std::string(_f2->GetName()))+"_"+_f2->GetParName(i) ).c_str());
  }
  if (_addScale) {
    _convolution->SetParameter(_scaleParNumber,1.);
    _convolution->SetParName(_scaleParNumber,"C");
  }
  return _convolution;
}


Double_t TConvolution::integrand(Double_t t, Double_t x0) {
  return  _f1->Eval(t) *  _f2->Eval(x0-t);
}

Double_t TConvolution::convolutionFkt(Double_t *x, Double_t *par){
  _f1->SetParameters(par);
  _f2->SetParameters(&(par[_nPar1]));

  Double_t stepSize = ( _f1Rmax - _f1Rmin ) / _stepsPerPoint;

  Double_t integral = 0;
  for (Double_t t=_f1Rmin;t <= _f1Rmax; t+=stepSize) integral += integrand(t,x[0]);
  integral *= stepSize;

  if (_addScale) return  par[_scaleParNumber] * integral;
  return integral;
}

