#ifndef BitSet_hh
#define BitSet_hh 1

#include <vector>
#include <ostream>

namespace CALICE {

  /**
   * Base class for easy definition of bit sets.
   *
   * Derived classes have to define the meaning of the bits. No instantiation possible.
   *
   * @author Benjamin.Lutz@desy.de
   * @date November 2009
   * @version 1.0
   */
  class BitSet {

  public:

    /**
     * get integer corresponding to the bits
     *
     * @return integer with the bits set
     */
    int getInt() const { return _bits; }

    /**
     * set all bits with an integer
     *
     * @param[in] value integer with the bits
     */
    void setBits(const int value) {
      _bits = value;
    }

    /**
     * get vector of ints with elements corresponding
     * to the single bits (1 for true and 0 for false)
     *
     * @return vector of integers of single bits
     */
    std::vector<int> getIntVec() const {
      std::vector<int> result;
      for (unsigned int i=_maxBits; i>0; --i) {
        result.push_back(getBit(i-1));
      }
      return result;
    }

    /**
     * clear all bits to 0
     */
    void clear() {
      _bits = 0x0;
    }

  protected:

    /**
     * constructur
     *
     * maximum number of bits defines the size of the returned vector
     *
     * @param[in] maxBits maximum number of bits
     * @param[in] value to initialise to different than 0
     */
    BitSet(const unsigned int maxBits, const int value = 0x0) {
      _bits = value;
      _maxBits = maxBits;
    }

    /**
     * get function for bit number
     *
     * @param bitNo number of the bit to return
     * @returns state of bit
     */
    bool getBit(unsigned int bitNo ) const { int bits = _bits; return ( (bits >> bitNo) & 0x01 ); }
    /**
     * set function for bit number
     *
     * @param bitNo number of the bit to set
     * @param state state to witch the bit gets set
     */
    void setBit(unsigned int bitNo, bool state) {
      int value = (0x01 << bitNo);
      if (state) _bits = _bits | value;
      else _bits = _bits & ~value;
    }

  private:
    int _bits;
    unsigned int _maxBits;
  };

  std::ostream & operator << (std::ostream &os, const BitSet &bits);

} // end namespace CALICE
#endif
