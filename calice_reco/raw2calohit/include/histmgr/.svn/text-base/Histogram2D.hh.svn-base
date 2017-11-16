#ifndef _HistogramTwoD_hh_
#define _HistogramTwoD_hh_

#include <lcio.h>
#include <IMPL/LCGenericObjectImpl.h>
//#include <config.h>
#include <RtypesSubSet.h>
#include "HistPar.hh"

#include <string>
#include <iostream>

namespace histmgr {

enum EHistogram2DIntType {kHistogram2DXNBins, kHistogram2DYNBins, 
			  kHistogram2DId, kNHistogram2DInts};
enum EHistogram2DFloatType {kHistogram2DXmin, kHistogram2DXmax, 
			    kHistogram2DYmin, kHistogram2DYmax, 
			    kNHistogram2DFloats};
enum EHistogram2DDoubleType {kHistogram2DEntries, kNHistogram2DDoubles};

class Histogram2D : public lcio::LCGenericObject
{
protected:
  Histogram2D(lcio::LCObject* a_obj);
  Histogram2D() : _obj(0) , _createdObject( false) {};

public:
  ~Histogram2D() { 
      if( _createdObject )  delete _obj ; 
  }

  /** Return the id of the underlying LCGenericObjectImpl */
  //CRP 5/1/06 To satisfy redefinition of interface in LCIO
  virtual int id() const { return _obj->id() ; }

  UInt_t xFirstBinIndex() const {return 1;};
  UInt_t xLastBinIndex() const {return xNBins();};
  UInt_t yFirstBinIndex() const {return 1;};
  UInt_t yLastBinIndex() const {return yNBins();};
  UInt_t xUnderflowBinIndex() const {return 0;};
  UInt_t xOverflowBinIndex() const {return xNBins()+1;};
  UInt_t yUnderflowBinIndex() const {return 0;};
  UInt_t yOverflowBinIndex() const {return yNBins()+1;};
  
  UInt_t xNBins() const {
    return (UInt_t)  _obj->getIntVal(kHistogram2DXNBins);
  };

  Float_t xMin() const {
    return _obj->getFloatVal(kHistogram2DXmin);
  }; 

  Float_t xMax() const {
    return _obj->getFloatVal(kHistogram2DXmax);
  };

  UInt_t yNBins() const {
    return (UInt_t)  _obj->getIntVal(kHistogram2DYNBins);
  };

  Float_t yMin() const {
    return _obj->getFloatVal(kHistogram2DYmin);
  }; 

  Float_t yMax() const {
    return _obj->getFloatVal(kHistogram2DYmax);
  };


  Float_t xBinCenter(UInt_t bin_index) 
  {
    Float_t x=xMin() + (bin_index-xFirstBinIndex()+.5) / _xToBin;
    return x;
  }

  Float_t xBinLowEdge(UInt_t bin_index) 
  {
    return xMin() + (bin_index - xFirstBinIndex())/_xToBin;
  }

  Float_t xBinWidth(UInt_t) 
  {
    return 1/_xToBin;
  }


  Float_t yBinCenter(UInt_t bin_index) 
  {
    Float_t y=yMin() + (bin_index-yFirstBinIndex()+.5) / _yToBin;
    return y;
  }

  Float_t yBinLowEdge(UInt_t bin_index) 
  {
    //    Float_t y=yMin();
    //    Float_t y_step=(yMax() - yMin()) / yNBins();
    return yMin() + (bin_index - yFirstBinIndex())/_yToBin;
  }

  Float_t yBinWidth(UInt_t) 
  {
    return 1/_yToBin;
  }


  //CRP 5/1/06 To satisfy redefinition of interface in LCIO
  UInt_t calice_id() {
    return (UInt_t) _obj->getIntVal(kHistogram2DId);
  }

  Double_t entries() const {
    return _obj->getDoubleVal(kHistogram2DEntries);
  };

  void addToEntries(Double_t weight)  {
    _obj->setDoubleVal(kHistogram2DEntries,entries()+weight);
  };


  // ---- need to implement LCGenericObject interface:
    
  int getNInt() const    { return kNHistogram2DInts ; };
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

  void setBinning(const HistPar &x_bins, const HistPar &y_bins) {
    _obj->setIntVal(kHistogram2DXNBins,(Int_t) x_bins.nBins());
    _obj->setFloatVal(kHistogram2DXmin,x_bins.xMin());
    _obj->setFloatVal(kHistogram2DXmax,x_bins.xMax());

    _xToBin=x_bins.nBins() / (x_bins.xMax() - x_bins.xMin() );

    _obj->setIntVal(kHistogram2DYNBins,(Int_t) y_bins.nBins());
    _obj->setFloatVal(kHistogram2DYmin,y_bins.xMin());
    _obj->setFloatVal(kHistogram2DYmax,y_bins.xMax());

    _yToBin=y_bins.nBins() / (y_bins.xMax() - y_bins.xMin() );
  };

  void setXtoBins() {
    _xToBin=xNBins()/(xMax() - xMin()) ;
  }
  void setYtoBins() {
    _yToBin=yNBins()/(yMax() - yMin()) ;
  }

  UInt_t xBinIndex(Float_t value) const 
  {
    Double_t bin_f=(value-xMin())*_xToBin;
    if (bin_f<0.) {return xUnderflowBinIndex();};
    UInt_t bin_i=static_cast<unsigned int>(bin_f);
    if (bin_i>=xNBins()) {return xOverflowBinIndex();};
    
    return bin_i+xFirstBinIndex();
  };

  UInt_t yBinIndex(Float_t value) const 
  {
    Double_t bin_f=(value-yMin())*_yToBin;
    if (bin_f<0.) {return yUnderflowBinIndex();};
    UInt_t bin_i=static_cast<unsigned int>(bin_f);
    if (bin_i>=yNBins()) {return yOverflowBinIndex();};
    
    return bin_i+yFirstBinIndex();
  };

  UInt_t getContainerSize() const {
    return (xNBins()+2)*(yNBins()+2);
  }

  lcio::LCGenericObjectImpl* obj() {return _obj;};
  const lcio::LCGenericObjectImpl* obj() const {return _obj;};

  lcio::LCGenericObjectImpl* _obj;
  bool _createdObject ;
  static const std::string __typeName;
  static const std::string __description;

  Double_t _xToBin;
  Double_t _yToBin;
};

}
#endif
