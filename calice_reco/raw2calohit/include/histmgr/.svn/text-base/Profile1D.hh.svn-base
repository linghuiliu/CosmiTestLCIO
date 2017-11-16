#ifndef _ProfileD_hh_
#define _ProfileD_hh_

#include "Histogram1D.hh"
#include <stdexcept>
#include <sstream>
#include <cmath>

namespace histmgr {

  /** A 1-dimensional histogram using float values for the bins
   */
  class Profile1D : public Histogram1D
  {
  public:
    /** Create a histogram with a certain binning
     * @param id a unique histogram ID,
     * @param binning number of bins and range of the axis
     * @todo a name would be better than an ID but a string can not be easily stored in an LCGenericObject
     */
    Profile1D(UInt_t id, const HistPar &binning);
    Profile1D(lcio::LCObject* a_obj) : Histogram1D(a_obj) {};
    Profile1D(const Profile1D &a);

    /** fill a value into the histogram
     */
    void fill(Float_t x_value, Double_t value, Double_t weight=1.)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;


    /** reset the histograms and the number of entries
     */
    void reset();

    //  /** manipulate a histogram bin
    //   */
    //  void addToBinContent(UInt_t bin_index, Float_t value)
    //#ifdef BOUNDARY_CHECK
    //  throw(std::range_error)
    //#endif
    //    ;

    //  /** manipulate a histogram bin
    //   */
    //  void setBinContent(UInt_t bin_index, Float_t value)
    //#ifdef BOUNDARY_CHECK
    //    throw(std::range_error)
    //#endif
    //    ;

    //  Float_t binContent(UInt_t bin_index) const
    //#ifdef BOUNDARY_CHECK
    //    throw(std::range_error)
    //#endif
    //    ;

    Double_t sum(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Double_t sum2(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Double_t mean(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    /** Return the rms of the bin.
        deprecated.
    */
    Double_t sigma(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {return rms(bin_index);};

    Double_t rms(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Double_t min(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Double_t max(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Double_t sumOfWeights(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    void calculate();

    //  Float_t overflow() const {
    //    return binContent(overflowBinIndex());
    //  }

    //  Float_t underflow() const {
    //    return binContent(underflowBinIndex());
    //  }

    Float_t mean() const;

    /** Root mean square as defined by ROOT.
     * divided by n
     */
    Float_t rms() const;

    /** Variance.
     * divided by n-1
     */
    Float_t variance() const;

    //   sum contentes of bins within the given range
    //   @param first_bin index of the first bin
    //   @param last_bin index of the last bin (included in the sum)
    //   @return the sum of the bins
    //
    //  Double_t integral(UInt_t first_bin, UInt_t last_bin) const
    //#ifdef BOUNDARY_CHECK
    //    throw(std::range_error)
    //#endif
    //    ;

    lcio::LCObject* clone() const {  return new Profile1D(*this); };

  protected:
#ifdef BOUNDARY_CHECK
    /** verify that the bin index is in the allowed range
     */
    void rangeCheck(UInt_t bin_index) const throw(std::range_error);
#endif

    enum EBinType {kSum,kSum2, kMin,kMax,kWeightSum,kNBinTypes};
    void addToBinContent(UInt_t type_i, UInt_t bin_index, Double_t value)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    void setBinContent(UInt_t type_i, UInt_t bin_index, Double_t value)
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    Double_t binContent(UInt_t type_i, UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
      ;

    bool isCalculated(UInt_t bin_index) const;

    UInt_t getValueIndex(UInt_t type_i, UInt_t bin_index) const {
      //  return type_i*(nBins()+2) + bin_index+kNHistogram1DDoubles;
      return type_i+bin_index * kNBinTypes +kNHistogram1DDoubles;
    };

  private:

  };


  inline void Profile1D::fill(Float_t x_value, Double_t value, Double_t weight)
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    UInt_t bin_index=binIndex(x_value);
#ifdef BOUNDARY_CHECK
    assert ( !isCalculated(bin_index) );
#endif
    addToBinContent(kSum, bin_index,value);
    addToBinContent(kSum2, bin_index,value*value);
    addToBinContent(kWeightSum, bin_index,weight);
    if (value>max(bin_index)) setBinContent(kMax, bin_index,value);
    if (value<min(bin_index)) setBinContent(kMin,bin_index,value);
    addToEntries(weight);
  }

  inline Double_t Profile1D::sum(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    if (isCalculated(bin_index)) return binContent(kSum,bin_index)*sumOfWeights(bin_index);
    else return binContent(kSum,bin_index);
  }


  inline Double_t Profile1D::mean(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    // check whether the mean value has been calculated in is stored in the bin?
    if (!isCalculated(bin_index)) return ( sumOfWeights(bin_index) >0. ? binContent(kSum,bin_index)/sumOfWeights(bin_index) : binContent(kSum,bin_index) ) ;
    else return binContent(kSum,bin_index);
  }

  inline Double_t Profile1D::rms(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    // check whether the mean value has been calculated in is stored in the bin?
    if (!isCalculated(bin_index)) {
      if (sumOfWeights(bin_index)>1.) {
        return sqrt((binContent(kSum2,bin_index)-binContent(kSum,bin_index)*binContent(kSum,bin_index)/sumOfWeights(bin_index))/(sumOfWeights(bin_index)-1.));
      }
      else if (sumOfWeights(bin_index)>0) {
        return sqrt((binContent(kSum2,bin_index)-binContent(kSum,bin_index)*binContent(kSum,bin_index)/sumOfWeights(bin_index))/(sumOfWeights(bin_index)));
      }
      else {
        return 0.;
      }
    }
    else return binContent(kSum2,bin_index);
  }

  inline Double_t Profile1D::min(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    return binContent(kMin,bin_index);
  }

  inline Double_t Profile1D::max(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    return binContent(kMax,bin_index);
  }

  inline Double_t Profile1D::sumOfWeights(UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
    return fabs(binContent(kWeightSum,bin_index));
  }

  inline bool Profile1D::isCalculated(UInt_t bin_index) const {
    return (binContent(kWeightSum,bin_index)<=0.);
  }




#ifdef BOUNDARY_CHECK
  inline void Profile1D::rangeCheck(UInt_t bin_index) const
    throw(std::range_error)
  {
    if ( bin_index + kNHistogram1DDoubles >= (UInt_t) _obj->getNDouble() ) {
      std::stringstream message;
      message << "Range error accessing bin " << bin_index << " of histogram " << id() << "which has" << nBins() << ".";
      throw std::range_error(message.str());
    }
  }
#endif

  inline void Profile1D::addToBinContent(UInt_t type_i, UInt_t bin_index, Double_t value)
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_index);
#endif
    UInt_t value_index=getValueIndex(type_i,bin_index);
    _obj->setDoubleVal(value_index, _obj->getDoubleVal(value_index)+value );
  }

  inline void Profile1D::setBinContent(UInt_t type_i, UInt_t bin_index, Double_t value)
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_index);
#endif
    _obj->setDoubleVal(getValueIndex(type_i,bin_index),value);
  }

  inline double Profile1D::binContent(UInt_t type_i, UInt_t bin_index) const
#ifdef BOUNDARY_CHECK
    throw(std::range_error)
#endif
  {
#ifdef BOUNDARY_CHECK
    rangeCheck(bin_index);
#endif
    return _obj->getDoubleVal(getValueIndex(type_i,bin_index));
  }

}
#endif
