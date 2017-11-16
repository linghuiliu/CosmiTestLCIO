#ifndef _HistPar_hh_
#define _HistPar_hh_

#include <RtypesSubSet.h>

/** Class to describe the binning of equidistant histograms.
 */
class HistPar
{
public:
  /** Default constructor. 
   * The binning defined by the default constructor is invalid.
   */
  HistPar() : _nBins(0) {};
  
  /** Constructor. 
   * @sa setNBins,setXMin,setXmax
   */
  HistPar(UInt_t n_bins, Float_t x_min, Float_t x_max) : _nBins(n_bins), _xMin(x_min),_xMax(x_max) {};
  
  /** Set the number of bins.
   * This does not include overflow and underflow bins. Thus, generally two more bins
   * will be used to store the overflow and the underflow.
   */
  HistPar &setNBins(UInt_t n_bins) {_nBins=n_bins; return *this;};

  /** Set the lower edge of the first bin.
   * Everything below this value will be directed to the underflow bin.
   */
  HistPar &setXMin(Float_t x_min)  {_xMin=x_min; return *this;};

  /** set the upper edge of the last bin.
   * Everything above this value will be directed to the overflow bin.
   */
  HistPar &setXMax(Float_t x_max)  {_xMax=x_max; return *this;};

  /** The number of bins to be used.
   * This does not include overflow and underflow bins. Thus, generally two more bins
   * will be used to store the overflow and the underflow.
   */
  UInt_t  nBins() const {return _nBins;};

  /** The lower edge of the first bin.
   * Everything below this value will be directed to the underflow bin.
   */
  Float_t xMin()  const {return _xMin;};

  /** The upper edge of the last bin.
   * Everything above this value will be directed to the overflow bin.
   */
  Float_t xMax()  const {return _xMax;};

protected:
  UInt_t  _nBins;
  Float_t _xMin;
  Float_t _xMax;
};
#endif
