#include "FloatHistogram2D.hh"
#include <cmath>

inline double sqr(double a) {return a*a;}

namespace histmgr {

FloatHistogram2D::FloatHistogram2D(UInt_t id, const HistPar &binning_x, const HistPar &binning_y )
{
  _obj = new lcio::LCGenericObjectImpl(kNHistogram2DInts,kNHistogram2DFloats+(binning_x.nBins()+2)*(binning_y.nBins()+2),kNHistogram2DDoubles) ;
  _createdObject=true;
  _obj->setIntVal(kHistogram2DId,id);
  setBinning(binning_x, binning_y);
  reset();
}

FloatHistogram2D::FloatHistogram2D(const FloatHistogram2D &a)
{
  _obj = dynamic_cast<lcio::LCGenericObjectImpl*>(a.obj()->clone());
  _createdObject=true;

//     new lcio::LCGenericObjectImpl(kNHistogram2DInts,kNHistogram2DFloats+a.nBins()+2,kNHistogram2DDoubles) ;
//   _obj->setIntVal(kHistogram2DId,a.id());
//   setBinning(HistPar(a.nBins(),a.xMin(),a.xMax()));
  
//   UInt_t bin_i=a.underflowBinIndex();
//   setBinContent(bin_i,a.binContent(bin_i));
//   for (bin_i=a.firstBinIndex(); bin_i<=a.lastBinIndex(); bin_i++) {
//     setBinContent(bin_i,a.binContent(bin_i));
//   }
//   bin_i=a.overflowBinIndex();
//   setBinContent(bin_i,a.binContent(bin_i));
}

void FloatHistogram2D::reset()
{
  _obj->setDoubleVal(kHistogram2DEntries,0.);
  for (UInt_t bin_i=0; bin_i<nBinsTotal(); bin_i++) {
    setBinContent(bin_i,0);
  }
}

Float_t FloatHistogram2D::xMean() const 
{
  Double_t sum=0; 
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/xNBins();
  x+=x_step*.5;
  for (UInt_t binx_i=xFirstBinIndex(); binx_i<=xLastBinIndex(); binx_i++) {
    Double_t projection=0;
    for (UInt_t biny_i=yUnderflowBinIndex(); biny_i<=yOverflowBinIndex(); biny_i++) {
      projection+=binContent(binIndex(binx_i,biny_i));
    }
    sum+=x*projection;
    x+=x_step;
  }

  Double_t n=entries()-xOverflow()-xUnderflow();
  if (n>0) {
    sum/=n;
  }
  return sum;
}

Float_t FloatHistogram2D::xRms(bool natural) const 
{
  Double_t sum=0; 
  Double_t sum2=0; 
  Float_t x=xMin();
  Float_t x_step=(xMax()-xMin())/xNBins();
  x+=x_step*.5;
  for (UInt_t binx_i=xFirstBinIndex(); binx_i<=xLastBinIndex(); binx_i++) {
    Double_t projection=0;
    for (UInt_t biny_i=yUnderflowBinIndex(); biny_i<=yOverflowBinIndex(); biny_i++) {
      projection+=binContent(binIndex(binx_i,biny_i));
    }
    sum+=x*projection;
    sum2+=sqr(x)*projection;
    x+=x_step;
  }

  Double_t n=entries()-xOverflow()-xUnderflow();
  if (n>1) {
    Double_t mean=sum/n;
    sum2=(sum2-mean*sum)/( (natural  ? n-1 : 1));
    return sqrt(sum2);
  }
  else {
    return 0.;
  }
}

Float_t FloatHistogram2D::yMean() const 
{
  Double_t sum=0; 
  Float_t y=yMin();
  Float_t y_step=(yMax()-yMin())/yNBins();
  y+=y_step*.5;
  for (UInt_t biny_i=yFirstBinIndex(); biny_i<=yLastBinIndex(); biny_i++) {
    Double_t projection=0;
    for (UInt_t binx_i=xUnderflowBinIndex(); binx_i<=xOverflowBinIndex(); binx_i++) {
      projection+=binContent(binIndex(binx_i,biny_i));
    }
    sum+=y*projection;
    y+=y_step;
  }

  Double_t n=entries()-yOverflow()-yUnderflow();
  if (n>0) {
    sum/=n;
  }
  return sum;
}

Float_t FloatHistogram2D::yRms(bool natural) const 
{
  Double_t sum=0; 
  Double_t sum2=0; 
  Float_t y=yMin();
  Float_t y_step=(yMax()-yMin())/yNBins();
  y+=y_step*.5;
  for (UInt_t biny_i=yFirstBinIndex(); biny_i<=yLastBinIndex(); biny_i++) {
    Double_t projection=0;
    for (UInt_t binx_i=xUnderflowBinIndex(); binx_i<=xOverflowBinIndex(); binx_i++) {
      projection+=binContent(binIndex(binx_i,biny_i));
    }
    sum+=y*projection;
    sum2+=sqr(y)*projection;
    y+=y_step;
  }

  Double_t n=entries()-yOverflow()-yUnderflow();
  if (n>1) {
    Double_t mean=sum/n;
    sum2=(sum2-mean*sum)/( (natural  ? n-1 : 1));
    return sqrt(sum2);
  }
  else {
    return 0.;
  }
}

Double_t FloatHistogram2D::integral(UInt_t x0_i, UInt_t x1_i, UInt_t y0_i, UInt_t y1_i) const 
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
#ifdef BOUNDARY_CHECK
  xRangeCheck(x0_i);
  xRangeCheck(x1_i);
  yRangeCheck(y0_i);
  yRangeCheck(y1_i);
#endif
  Double_t sum=0; 
  for (UInt_t binx_i=x0_i; binx_i<=x1_i; binx_i++) {
    for (UInt_t biny_i=x0_i; biny_i<=y1_i; biny_i++) {
      sum+=binContent(binIndex(binx_i,biny_i));
    }
  }
  return sum;
}

}

