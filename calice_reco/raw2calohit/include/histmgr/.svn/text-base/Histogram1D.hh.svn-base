#ifndef _Histogram_hh_
#define _Histogram_hh_

#include <lcio.h>
#include <IMPL/LCGenericObjectImpl.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <RtypesSubSet.h>
#include "HistPar.hh"

#include <string>
#include <iostream>

namespace histmgr { 

enum EHistogram1DIntType {kHistogram1DNBins, kHistogram1DId, kNHistogram1DInts};
enum EHistogram1DFloatType {kHistogram1DXmin, kHistogram1DXmax, kNHistogram1DFloats};
enum EHistogram1DDoubleType {kHistogram1DEntries, kNHistogram1DDoubles};

/** Equidistant binned 1D histogram without data container.
 * The histogram is kept very simple and does not make use of 
 * virtual functions in order to be very fast. 
 * Note: the histogram does not have a data container. A usable histogram
 * is the @ref FloatHistogram1D.
 */
class Histogram1D : public lcio::LCGenericObject
{
protected:
  Histogram1D(lcio::LCObject* a_obj);
  Histogram1D() : _obj(0) , _createdObject( false) {};

public:
  ~Histogram1D() { 
      if( _createdObject )  delete _obj ; 
  }

  /** Return the id of the underlying LCGenericObjectImpl */
  //CRP 5/1/06 To satisfy redefinition of interface in LCIO
  virtual int id() const { return _obj->id() ; }

  /** Return the index of the first bin not below the minimum axis value.
   */
  UInt_t firstBinIndex() const {return 1;};

  /** Return the index of the last bin not above the maximum axis value.
   */
  UInt_t lastBinIndex() const {return nBins();};

  /** Return the index of the bin below the minimum axis value (underflow bin).
   */
  UInt_t underflowBinIndex() const {return 0;};

  /** Return the index of the bin above the maximum axis value (overflow bin).
   */
  UInt_t overflowBinIndex() const {return nBins()+1;};
  
  /** Return the number of bins.
   */
  UInt_t nBins() const {
    return (UInt_t)  _obj->getIntVal(kHistogram1DNBins);
  };

  /** The minimum axis value (lower edge of the histogram range).
   */
  Float_t xMin() const {
    return _obj->getFloatVal(kHistogram1DXmin);
  }; 

  /** The maximum axis value (upper edge of the histogram range).
   */
  Float_t xMax() const {
    return _obj->getFloatVal(kHistogram1DXmax);
  };

  /** Return the axis value of the centre of the given bin.
   */
  Float_t binCenter(UInt_t bin_index) const
  {
    Float_t x=xMin() + (xMax()-xMin()) * (bin_index-firstBinIndex()+.5) / nBins();
    return x;
  }

  /* Return the axis value of the low edge of the given bin.
   */
  Float_t binLowEdge(UInt_t bin_index) const
  {
    Float_t x=xMin();
    Float_t x_step=(xMax()-xMin())/nBins();
    x+=x_step*(bin_index-firstBinIndex());
    return x;
  }

  /** Return the width of the given bin.
   */
  Float_t binWidth(UInt_t) const
  {
    Float_t x_step=(xMax()-xMin())/nBins();
    return x_step;
  }

  /** Return the id of the histogram.
   */
  //CRP 5/1/06 To satisfy redefinition of interface in LCIO
  UInt_t calice_id() {
    return (UInt_t) _obj->getIntVal(kHistogram1DId);
  }

  /** Return the number of entries of the histogram.
   */
  Double_t entries() const {
    return _obj->getDoubleVal(kHistogram1DEntries);
  };

protected:
  /** Add weight to the number of entries.
   * Weight is usually 1, but  may be a fractional value in the interval (0,oo).
   * NOTE: No range check of the weight is performed.
   */
  void addToEntries(Double_t weight)  {
    _obj->setDoubleVal(kHistogram1DEntries,entries()+weight);
  };


  // ---- need to implement LCGenericObject interface:
    
  int getNInt() const    { return kNHistogram1DInts ; };
  int getNFloat() const  { return _obj->getNFloat() ; };
  int getNDouble() const { return _obj->getNDouble(); }; 
    
  int getIntVal(int index) const {
    return _obj->getIntVal( index ) ;
  };
  float getFloatVal(int index) const { 
    return _obj->getFloatVal( index ) ;
  };
  double getDoubleVal(int index) const {
    return  _obj->getDoubleVal( index ) ;
  };
  bool isFixedSize() const { return false ; };

  const std::string getTypeName() const {
    return __typeName;
  }

  const std::string getDataDescription() const {
    return __description;
  }
  
  // ---- end of LCGenericObject interface

protected:

  /** Store the binning of the histogram.
   */
  void setBinning(const HistPar &bins) {
    _obj->setIntVal(kHistogram1DNBins,(Int_t) bins.nBins());
    _obj->setFloatVal(kHistogram1DXmin,bins.xMin());
    _obj->setFloatVal(kHistogram1DXmax,bins.xMax());
  };

  /** Calculate and return the bin index for the given axis value.
   */
  UInt_t binIndex(Float_t value) const 
  {
    Float_t x_min=xMin();
    Float_t x_max=xMax();
    UInt_t n_bins=nBins();

    if (value>=x_max) {return overflowBinIndex();};
    if (value<x_min) {return underflowBinIndex();};
    return (UInt_t) (((value-x_min)/(x_max-x_min))*n_bins)+firstBinIndex();
  };

  lcio::LCGenericObjectImpl* obj() {return _obj;};
  const lcio::LCGenericObjectImpl* obj() const {return _obj;};

  lcio::LCGenericObjectImpl* _obj;
  bool _createdObject ;
  static const std::string __typeName;
  static const std::string __description;
};

}
#endif
