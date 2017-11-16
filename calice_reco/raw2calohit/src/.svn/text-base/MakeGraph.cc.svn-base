#include <TGraph.h>
#include <TGraphErrors.h>
#include "MakeGraph.hh"

TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<unsigned int> &y_arr) {
  TGraph *a_gr=0;
  if (x_arr.size()>0 && x_arr.size()==y_arr.size()) {
    a_gr=new TGraph;
    if (a_gr) {
      a_gr->SetName(name);
      a_gr->SetTitle(name);
      a_gr->Set(x_arr.size());
      for (UInt_t gr_i=0; gr_i<x_arr.size(); gr_i++) {
	a_gr->SetPoint(gr_i,x_arr[gr_i],y_arr[gr_i]);
      } 
    }
  }
  return a_gr;
}

TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<int> &y_arr) {
  TGraph *a_gr=0;
  if (x_arr.size()>0 && x_arr.size()==y_arr.size()) {
    a_gr=new TGraph;
    if (a_gr) {
      a_gr->SetName(name);
      a_gr->SetTitle(name);
      a_gr->Set(x_arr.size());
      for (UInt_t gr_i=0; gr_i<x_arr.size(); gr_i++) {
	a_gr->SetPoint(gr_i,x_arr[gr_i],y_arr[gr_i]);
      } 
    }
  }
  return a_gr;
}

TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<Float_t> &y_arr) {
  TGraph *a_gr=0;
  if (x_arr.size()>0 && x_arr.size()==y_arr.size()) {
    a_gr=new TGraph;
    if (a_gr) {
      a_gr->SetName(name);
      a_gr->SetTitle(name);
      a_gr->Set(x_arr.size());
      for (UInt_t gr_i=0; gr_i<x_arr.size(); gr_i++) {
	a_gr->SetPoint(gr_i,x_arr[gr_i],y_arr[gr_i]);
      } 
    }
  }
  return a_gr;
}

TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<Double_t> &y_arr) {
  TGraph *a_gr=0;
  if (x_arr.size()>0 && x_arr.size()==y_arr.size()) {
    a_gr=new TGraph(x_arr.size(),&(x_arr[0]),&(y_arr[0]));
    if (a_gr) {
      a_gr->SetName(name);
      a_gr->SetTitle(name);
    }
  }
  return a_gr;
}

TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<Float_t> &y_arr, const std::vector<Float_t> &y_error_arr) {
  TGraphErrors *a_gr=0;
  if (x_arr.size()>0 && x_arr.size()==y_arr.size() && x_arr.size()==y_error_arr.size()) {
    a_gr=new TGraphErrors;
    if (a_gr) {
      a_gr->SetName(name);
      a_gr->SetTitle(name);
      a_gr->Set(x_arr.size());
      for (UInt_t gr_i=0; gr_i<x_arr.size(); gr_i++) {
	a_gr->SetPoint(gr_i,x_arr[gr_i],y_arr[gr_i]);
	a_gr->SetPointError(gr_i,0,y_error_arr[gr_i]);
      } 
    }
  }
  return a_gr;
}
