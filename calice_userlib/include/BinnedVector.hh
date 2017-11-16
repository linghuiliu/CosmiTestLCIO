#ifndef BinnedVector_hh
#define BinnedVector_hh 1

#include <vector>
#include <limits>

#include "marlin/Processor.h" // to use streamlog like in processors

namespace CALICE {

  /**
   * Template class to store data via a linear binned keys.
   *
   * This is like a histogram, but without graphical functionallity.
   *
   * @param K type of the key
   * @param D type of the data
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.2
   * @date October 2009
   */
  template <class K, class D>
  class BinnedVector {
  public:
    /**
     * standard constructor
     *
     * @warning Dimensions have to be set with setBinning() before the vector can be used.
     */
    BinnedVector();
    /**
     * constructor which sets dimensions
     *
     * @param N number bins (without under and over range bins)
     * @param minRange lower end of range
     * @param maxRange upper end of range
     * @param initValue value for initialisation of bins
     */
    BinnedVector(const unsigned int N, const K &minRange, const K &maxRange, const D &initValue);
    /**
     * function to set the dimensions
     *
     * @param N number bins (without under and over range bins)
     * @param minRange lower end of range
     * @param maxRange upper end of range
     * @param initValue value for initialisation of bins
     */
    void setBinning(const unsigned int N, const K &minRange, const K &maxRange, const D &initValue);
    /**
     * access operator
     *
     * @param position key to find bin
     * @returns value at key
     */
    D& operator [] (const K& position);
    /**
     * const access operator
     *
     * @param position key to find bin
     * @returns value at key
     */
    const D& operator [] (const K& position) const;
    /**
     * clear content of bins
     *
     * all bins will be set to initValue
     */
    void clear();
    /**
     * get vector of bins
     *
     * the vector starts with the under range
     * bin and ends with the over range bin
     *
     * @returns vector of bins
     */
    const std::vector<D>& getVector() const {return _data;}
    /**
     * get vector of bin centers
     *
     * the vector starts with the under range
     * bin and ends with the over range bin
     *
     * @returns vector of bin centers
     */
    std::vector<K> getBinCenters() const;

  private:

    unsigned int getIndex(const K& position) const;

    K _binWidth;
    K _minRange;
    K _maxRange;
    unsigned int _N;
    std::vector<D> _data;

    D _default;
  };

  template <class K, class D>
  BinnedVector<K,D>::BinnedVector() {
  }

  template <class K, class D>
  BinnedVector<K,D>::BinnedVector(const unsigned int N, const K &minRange, const K &maxRange, const D &initValue) {
    setBinning(N, minRange, maxRange, initValue);
  }

  template <class K, class D>
  void BinnedVector<K,D>::clear() {

    _data.clear();
    _data.resize(_N+2,_default);

  }

  template <class K, class D>
  void BinnedVector<K,D>::setBinning(const unsigned int N, const K &minRange, const K &maxRange, const D &initValue) {

    _default = initValue;

    _minRange = minRange;
    _maxRange = maxRange;
    _N = N;

    _binWidth = (_maxRange-_minRange)/(double)_N;

    streamlog_out(DEBUG) << "binning: " << _N << " bins between " << _minRange << " and " << _maxRange << " which is a bin width of " << _binWidth<< std::endl;

    clear();
  }

  template <class K, class D>
  unsigned int BinnedVector<K,D>::getIndex(const K& position) const {
    unsigned int index = 0;
    if (position >= _minRange) index = (unsigned int)((position - _minRange)/_binWidth) + 1;
    if (index > _N) index = _N+1;

    streamlog_out(DEBUG) <<"requesting element for " << position << " which is mapped to bin " << index << std::endl;
    if (index >= _data.size() ) streamlog_out(ERROR) <<"requesting element for " << position << "which is mapped to non existing bin " << index << std::endl;

    return index;
  }

  template <class K, class D>
  D& BinnedVector<K,D>::operator [] (const K& position) {
    return _data[getIndex(position)];
  }

  template <class K, class D>
  const D& BinnedVector<K,D>::operator [] (const K& position) const {
    return _data[getIndex(position)];
  }

  template <class K, class D>
  std::vector<K> BinnedVector<K,D>::getBinCenters() const {
    std::vector<K> binCenters;
    for (unsigned int i=0; i<_N+2;++i) {
      binCenters.push_back(_minRange + _binWidth * ( (K)i - 0.5));
    }
    return binCenters;
  }
}

#endif
