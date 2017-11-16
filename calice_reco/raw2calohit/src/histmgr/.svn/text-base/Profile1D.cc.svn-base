#include "Profile1D.hh"
#include <cmath>

inline double sqr(double a) {return a*a;}

namespace histmgr {

Profile1D::Profile1D(UInt_t id, const HistPar &binning)
{
  _obj = new lcio::LCGenericObjectImpl(kNHistogram1DInts,kNHistogram1DFloats,kNHistogram1DDoubles+(binning.nBins()+2)*kNBinTypes) ;
  _createdObject=true;
  _obj->setIntVal(kHistogram1DId,id);
  setBinning(binning);
  reset();
}

Profile1D::Profile1D(const Profile1D &a)
{
  _obj = dynamic_cast<lcio::LCGenericObjectImpl*>(a.obj()->clone());
  _createdObject=true;

//     new lcio::LCGenericObjectImpl(kNHistogram1DInts,kNHistogram1DFloats+a.nBins()+2,kNHistogram1DDoubles) ;
//   _obj->setIntVal(kHistogram1DId,a.id());
//   setBinning(HistPar(a.nBins(),a.xMin(),a.xMax()));
  
//   UInt_t bin_i=a.underflowBinIndex();
//   setBinContent(bin_i,a.binContent(bin_i));
//   for (bin_i=a.firstBinIndex(); bin_i<=a.lastBinIndex(); bin_i++) {
//     setBinContent(bin_i,a.binContent(bin_i));
//   }
//   bin_i=a.overflowBinIndex();
//   setBinContent(bin_i,a.binContent(bin_i));
}

void Profile1D::reset()
{
  _obj->setDoubleVal(kHistogram1DEntries,0.);
  for (UInt_t bin_type_i=0; bin_type_i<kNBinTypes; bin_type_i++) {
    for (UInt_t bin_i=0; bin_i<=overflowBinIndex(); bin_i++) {
      setBinContent(bin_type_i,bin_i,0.);
    }
  }
}

Float_t Profile1D::mean() const 
{
  Double_t a_sum=0; 
  Double_t weight_sum=0;
  UInt_t bin_i;
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/nBins();
  x+=x_step*.5;
  for (bin_i=firstBinIndex(); bin_i<=lastBinIndex(); bin_i++) {
    a_sum+=sum(bin_i);
    weight_sum+=sumOfWeights(bin_i);
  }
  if (weight_sum>0) {
    a_sum/=weight_sum;
  }
  return a_sum;
}

Float_t Profile1D::rms() const 
{
  Double_t a_sum=0; 
  Double_t a_sum2=0; 
  Double_t weight_sum=0;
  UInt_t bin_i;
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/nBins();
  x+=x_step*.5;
  for (bin_i=firstBinIndex(); bin_i<=lastBinIndex(); bin_i++) {
    a_sum+=sum(bin_i);
    a_sum2+=sum2(bin_i);
    weight_sum+=sumOfWeights(bin_i);
  }
  if (weight_sum>0) {
      Double_t a_mean=a_sum/weight_sum;
    if (weight_sum>1) {
      a_sum2=(a_sum2-a_mean*a_sum)/(weight_sum-1);
      return sqrt(a_sum2);
    }
    else {
      a_sum2=(a_sum2-a_mean*a_sum)/weight_sum;
      return sqrt(a_sum2);
    }
    
  }
  else {
    return 0.;
  }
}

Float_t Profile1D::variance() const 
{
  Double_t a_sum=0; 
  Double_t a_sum2=0; 
  Double_t weight_sum=0;
  UInt_t bin_i;
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/nBins();
  x+=x_step*.5;
  for (bin_i=firstBinIndex(); bin_i<=lastBinIndex(); bin_i++) {
    a_sum+=sum(bin_i);
    a_sum2+=sum2(bin_i);
    weight_sum+=sumOfWeights(bin_i);
  }
  if (weight_sum>0) {
    Double_t a_mean=a_sum/weight_sum;
    a_sum2=(a_sum2-a_mean*a_sum)/weight_sum;
    return a_sum2;
  }
  else {
    return 0.;
  }
}

void Profile1D::calculate() {
  for (UInt_t bin_i=underflowBinIndex(); bin_i<overflowBinIndex(); bin_i++) {
    if (!isCalculated(bin_i)) {
      Double_t a_mean=mean(bin_i);
      Double_t a_sigma=sigma(bin_i);
      setBinContent(kWeightSum,bin_i,-fabs(sumOfWeights(bin_i)));
      setBinContent(kSum,bin_i,a_mean);
      setBinContent(kSum2,bin_i,a_sigma);
    }
  }
}

Double_t Profile1D::sum2(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
  if (isCalculated(bin_index)) {
    Double_t a_var=binContent(kSum2,bin_index);
    a_var*=a_var;
    Double_t a_weight_sum=sumOfWeights(bin_index);
    if (a_weight_sum>1.) {
      a_var*=(a_weight_sum-1.);
    }
    else {
      a_var*=a_weight_sum;
    }
    Double_t a_mean=mean(bin_index);
    a_var+=a_mean*a_mean*a_weight_sum;
    return a_var;
  }
  else return binContent(kSum2,bin_index);
}

}
