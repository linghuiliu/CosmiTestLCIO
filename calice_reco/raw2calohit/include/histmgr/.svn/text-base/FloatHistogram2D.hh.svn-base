#ifndef _FloatHistogramTwoD_hh_
#define _FloatHistogramTwoD_hh_

#include "Histogram2D.hh"
#include <stdexcept>
#include <sstream>

namespace histmgr {

  /** A 1-dimensional histogram using float values for the bins
   */
  class FloatHistogram2D : public Histogram2D
  {
  public:
    /** Create a histogram with a certain binning
     * @param id a unique histogram ID,
     * @param binning_x number of bins and range of the axis
     * @param binning_y number of bins and range of the axis
     * @todo a name would be better than an ID but a string can not be easily stored in an LCGenericObject
     */
    FloatHistogram2D(UInt_t id, const HistPar &binning_x, const HistPar &binning_y);
    FloatHistogram2D(lcio::LCObject* a_obj) : Histogram2D(a_obj) {};
    FloatHistogram2D(const FloatHistogram2D &a);

    /** fill a value into the histogram
     */
    void fill(Float_t value_x, Float_t value_y, Float_t weight=1.)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    /** reset the histograms and the number of entries
     */
    void reset();

    /** Get the 1d bin index.
     */
    UInt_t binIndex(UInt_t binx_i, UInt_t biny_i) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    /** manipulate a histogram bin
     */
    void addToBinContent(UInt_t bin_index, Float_t value)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    /** manipulate a histogram bin
     */
    void setBinContent(UInt_t bin_index, Float_t value)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Float_t binContent(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Float_t xOverflow(UInt_t biny_i) const {
      return binContent(binIndex(xOverflowBinIndex(),biny_i));
    }

    Float_t xUnderflow(UInt_t biny_i) const {
      return binContent(binIndex(xUnderflowBinIndex(),biny_i));
    }

    Float_t yOverflow(UInt_t binx_i) const {
      return binContent(binIndex(binx_i,yOverflowBinIndex()));
    }

    Float_t yUnderflow(UInt_t binx_i) const {
      return binContent(binIndex(binx_i, yUnderflowBinIndex()));
    }

    inline Float_t xUnderflow() const;
    inline Float_t xOverflow() const;

    inline Float_t yUnderflow() const;
    inline Float_t yOverflow() const;


    Float_t xMean() const;

    /** Root mean square as defined by ROOT.
     * divided by n
     */
    Float_t xRms(bool natural=false) const;

    /** Variance.
     * divided by n-1
     */
    Float_t xVariance() const;


    Float_t yMean() const;

    /** Root mean square as defined by ROOT.
     * divided by n
     */
    Float_t yRms(bool natural=false) const;

    /** Variance.
     * divided by n-1
     */
    Float_t yVariance() const;

    /** sum contents of bins within the given range
     * @param x0_i index of the first bin along x
     * @param x1_i index of the last bin (included in the sum) along x
     * @param y0_i index of the first bin along y
     * @param y1_i index of the last bin (included in the sum) along y
     * @return the sum of the bins
     */
    Double_t integral(UInt_t x0_i, UInt_t x1_i, UInt_t y0_i, UInt_t y1_i) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    lcio::LCObject* clone() const {  return new FloatHistogram2D(*this); };

  protected:
#ifdef BOUNDARY_CHECK
    /** verify that the bin index is in the allowed range
     */
    void rangeCheck(UInt_t bin_index) const throw(std::range_error);
    void yRangeCheck(UInt_t biny_i) const throw(std::range_error);
    void xRangeCheck(UInt_t binx_i) const throw(std::range_error);
#endif


  private:

    UInt_t nXBinsTotal() const {return xNBins()+2;};
    UInt_t nBinsTotal() const {return (xNBins()+2)*(yNBins()+2);};

  };

  /** Get the 1d bin index.
   */
  inline UInt_t FloatHistogram2D::binIndex(UInt_t binx_i, UInt_t biny_i) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    xRangeCheck(binx_i);
    yRangeCheck(biny_i);
#endif
    UInt_t bin_i=binx_i + nXBinsTotal() * biny_i;
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_i);
#endif
    return bin_i;
  }


  inline void FloatHistogram2D::fill(Float_t value_x, Float_t value_y, Float_t weight)
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    UInt_t binx_i=xBinIndex(value_x);
    UInt_t biny_i=yBinIndex(value_y);
    UInt_t bin_i=binIndex(binx_i,biny_i);

    addToBinContent(bin_i,weight);
    addToEntries(weight);
  }

#ifdef BOUNDARY_CHECK
  inline void FloatHistogram2D::rangeCheck(UInt_t bin_index) const
    throw(std::range_error)
  {
    if ( bin_index + kNHistogram2DFloats >= (UInt_t) _obj->getNFloat() ) {
      std::stringstream message;
      message << "Range error accessing bin " << bin_index << " of histogram " << id() << " which has " << nBinsTotal() << " bins.";
      throw std::range_error(message.str());
    }
  }

  inline void FloatHistogram2D::xRangeCheck(UInt_t binx_i) const
    throw(std::range_error)
  {
    if ( binx_i>xOverflowBinIndex() ) {
      std::stringstream message;
      message << "x-bin index out of range : " << binx_i << " > " << xOverflowBinIndex() << "  for histogram " << id() << ".";
      throw std::range_error(message.str());
    }
  }

  inline void FloatHistogram2D::yRangeCheck(UInt_t biny_i) const
    throw(std::range_error)
  {
    if ( biny_i>yOverflowBinIndex() ) {
      std::stringstream message;
      message << "y-bin index out of range : " << biny_i << " > " << yOverflowBinIndex() << "  for histogram " << id() << ".";
      throw std::range_error(message.str());
    }
  }

#endif

  inline void FloatHistogram2D::addToBinContent(UInt_t bin_index, Float_t value)
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_index);
#endif
    _obj->setFloatVal(bin_index+kNHistogram2DFloats,binContent(bin_index)+value);
  }


  inline void FloatHistogram2D::setBinContent(UInt_t bin_index, Float_t value)
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_index);
#endif
    _obj->setFloatVal(bin_index+kNHistogram2DFloats,value);
  }

  inline Float_t FloatHistogram2D::binContent(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_index);
#endif
    return _obj->getFloatVal(bin_index+kNHistogram2DFloats);
  }

  inline Float_t  FloatHistogram2D::yOverflow() const
  {
    Double_t overflow=0.;
    for (UInt_t binx_i=xFirstBinIndex(); binx_i<=xLastBinIndex(); binx_i++) {
      overflow += yOverflow(binx_i);
    }
    return overflow;
  }

  inline Float_t  FloatHistogram2D::yUnderflow() const
  {
    Double_t underflow=0.;
    for (UInt_t binx_i=xFirstBinIndex(); binx_i<=xLastBinIndex(); binx_i++) {
      underflow += yOverflow(binx_i);
    }
    return underflow;
  }

  inline Float_t  FloatHistogram2D::xOverflow() const
  {
    Double_t overflow=0.;
    for (UInt_t biny_i=yFirstBinIndex(); biny_i<=yLastBinIndex(); biny_i++) {
      overflow += xOverflow(biny_i);
    }
    return overflow;
  }

  inline Float_t  FloatHistogram2D::xUnderflow() const
  {
    Double_t underflow=0.;
    for (UInt_t biny_i=yFirstBinIndex(); biny_i<=yLastBinIndex(); biny_i++) {
      underflow += xUnderflow(biny_i);
    }
    return underflow;
  }

}

#endif
