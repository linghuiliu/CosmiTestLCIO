#ifndef _FloatHistogramD_hh_
#define _FloatHistogramD_hh_

#include "Histogram1D.hh"
#include <stdexcept>
#include <sstream>

namespace histmgr {

/** A 1-dimensional histogram using float values for the bins
 */
class FloatHistogram1D : public Histogram1D
{
public:
  /** Create a histogram with a certain binning
   * @param id a unique histogram ID,
   * @param binning number of bins and range of the axis
   * @todo a name would be better than an ID but a string can not be easily stored in an LCGenericObject
   */
  FloatHistogram1D(UInt_t id, const HistPar &binning);
  FloatHistogram1D(lcio::LCObject* a_obj) : Histogram1D(a_obj) {};
  FloatHistogram1D(const FloatHistogram1D &a);

  /** fill a value into the histogram
   */
  void fill(Float_t value, Float_t weight=1.)
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
    ;

  /** reset the histograms and the number of entries
   */
  void reset();

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

  Float_t overflow() const {
    return binContent(overflowBinIndex());
  }

  Float_t underflow() const {
    return binContent(underflowBinIndex());
  }
  
  Float_t mean() const;

  /** Root mean square as defined by ROOT.
   * divided by n
   */
  Float_t rms() const;

  /** Variance.
   * divided by n-1
   */
  Float_t variance() const;

  /** sum contentes of bins within the given range
   * @param first_bin index of the first bin
   * @param last_bin index of the last bin (included in the sum)
   * @return the sum of the bins
   */
  Double_t integral(UInt_t first_bin, UInt_t last_bin) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
    ;

  lcio::LCObject* clone() const {  return new FloatHistogram1D(*this); };
  
protected:
#ifdef BOUNDARY_CHECK
  /** verify that the bin index is in the allowed range
   */
  void rangeCheck(UInt_t bin_index) const throw(std::range_error);
#endif


private:
  
};


inline void FloatHistogram1D::fill(Float_t value, Float_t weight)
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
  UInt_t bin_index=binIndex(value);
  addToBinContent(bin_index,weight);
  addToEntries(weight);
}

#ifdef BOUNDARY_CHECK
inline void FloatHistogram1D::rangeCheck(UInt_t bin_index) const
  throw(std::range_error)
{
  if ( bin_index + kNHistogram1DFloats >= (UInt_t) _obj->getNFloat() ) {
    std::stringstream message;
    message << "Range error accessing bin " << bin_index << " of histogram " << id() << "which has" << nBins() << ".";
    throw std::range_error(message.str());
  }
}
#endif  

inline void FloatHistogram1D::addToBinContent(UInt_t bin_index, Float_t value)
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
#ifdef BOUNDARY_CHECK
  rangeCheck(bin_index);
#endif
  _obj->setFloatVal(bin_index+kNHistogram1DFloats,binContent(bin_index)+value);
}

inline void FloatHistogram1D::setBinContent(UInt_t bin_index, Float_t value)
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
#ifdef BOUNDARY_CHECK
  rangeCheck(bin_index);
#endif
  _obj->setFloatVal(bin_index+kNHistogram1DFloats,value);
}

inline Float_t FloatHistogram1D::binContent(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
  throw(std::range_error)
#endif
{
#ifdef BOUNDARY_CHECK
  rangeCheck(bin_index);
#endif
  return _obj->getFloatVal(bin_index+kNHistogram1DFloats);
}

}
#endif
