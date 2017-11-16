#include "FloatHistogram1D.hh"
#include <cmath>

inline double sqr(double a) {return a*a;}

namespace histmgr {

FloatHistogram1D::FloatHistogram1D(UInt_t id, const HistPar &binning)
{
  _obj = new lcio::LCGenericObjectImpl(kNHistogram1DInts,kNHistogram1DFloats+binning.nBins()+2,kNHistogram1DDoubles) ;
  _createdObject=true;
  _obj->setIntVal(kHistogram1DId,id);
  setBinning(binning);
  reset();
}

FloatHistogram1D::FloatHistogram1D(const FloatHistogram1D &a)
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

void FloatHistogram1D::reset()
{
  _obj->setDoubleVal(kHistogram1DEntries,0.);
  for (UInt_t bin_i=0; bin_i<=overflowBinIndex(); bin_i++) {
    setBinContent(bin_i,0);
  }
}

Float_t FloatHistogram1D::mean() const 
{
  Double_t sum=0; 
  UInt_t bin_i;
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/nBins();
  x+=x_step*.5;
  for (bin_i=firstBinIndex(); bin_i<=lastBinIndex(); bin_i++) {
    sum+=x*binContent(bin_i);
    x+=x_step;
  }
  Double_t n=entries()-overflow()-underflow();
  if (n>0) {
    sum/=n;
  }
  return sum;
}

Float_t FloatHistogram1D::rms() const 
{
  Double_t sum=0; 
  Double_t sum2=0; 
  UInt_t bin_i;
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/nBins();
  x+=x_step*.5;
  for (bin_i=firstBinIndex(); bin_i<=lastBinIndex(); bin_i++) {
    sum+=x*binContent(bin_i);
    sum2+=sqr(x)*binContent(bin_i);
    x+=x_step;
  }
  Double_t n=entries()-overflow()-underflow();
  if (n>1) {
    Double_t mean=sum/n;
    sum2=(sum2-mean*sum)/(n);
    return sqrt(sum2);
  }
  else {
    return 0.;
  }
}

Float_t FloatHistogram1D::variance() const 
{
  Double_t sum=0; 
  Double_t sum2=0; 
  UInt_t bin_i;
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/nBins();
  x+=x_step*.5;
  for (bin_i=firstBinIndex(); bin_i<=lastBinIndex(); bin_i++) {
    sum+=x*binContent(bin_i);
    sum2+=sqr(x)*binContent(bin_i);
    x+=x_step;
  }
  Double_t n=entries()-overflow()-underflow()-1;
  if (n>1) {
    Double_t mean=sum/n;
    sum2=(sum2-mean*sum)/(n);
    return sum2;
  }
  else {
    return 0.;
  }
}

Double_t FloatHistogram1D::integral(UInt_t first_bin, UInt_t last_bin) const 
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
#ifdef BOUNDARY_CHECK
  rangeCheck(first_bin);
  rangeCheck(last_bin);
#endif
  Double_t sum=0; 
  for (UInt_t bin_i=first_bin; bin_i<=last_bin; bin_i++) {
    sum+=binContent(bin_i);
  }
  return sum;
}

}
