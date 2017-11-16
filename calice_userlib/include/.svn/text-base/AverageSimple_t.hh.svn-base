#ifndef _AverageSimple_t_H_
#define _AverageSimple_t_H_
#include <iostream>
// FLT_MAX
//#include <values.h>
#include <cfloat>
#include <climits>
#include <RtypesSubSet.h>
#include <math.h>

/** Calculate mean and standard deviation.
 */
class AverageSimple_t {
public:
  friend std::ostream &operator <<(std::ostream &os, AverageSimple_t &a);

  /** Default constructor initialses all arrays to be ready to gather statastics of a variable.
   */
  AverageSimple_t() : _n(0),
		_sum(0.),
		_sum2(0.) {};

  /** build the sum and the sum of squares. 
   * The sums can be used later to calculate the mean and the standard deviation 
   * @param value the value to be added to the sum and the sum of squares
   * (@sa calculate)
   */
  void add(Float_t value) {
    _n++; 
    _sum+=value; 
    _sum2+=value*value; 
  };

  /** calculate the standard deviation and the mean.
   * The mean and the standard deviation are stored in the locations which are also used for the sums. 
   * Therefore if further values are added after calculate() was called, the result is undefined.
   */
  void calculate() {
    if (_n>1) {
      Double_t mean=_sum/_n;
      Double_t sigma=sqrt((_sum2-mean*_sum)/(_n-1));
      _mean=mean;
      _sigma=sigma;
    }
  };

  /** return the mean value.
   * returns only a correct result after at least 1 value was added and calculate was called.
   */
  Double_t mean() const {return _mean;};

  /** return the standard deviation.
   * returns only a correct result after at least 1 value was added and calculate was called.
   */
  Double_t sigma() const {return _sigma;};

  /** return the sample size.
   */
  UInt_t n() const {return _n;};
  
  /** Get the total accumulated data
   */
  Double_t sum() const {return _sum;};

  void operator+=(Float_t val) { this->add(val); };

protected:
  UInt_t _n;
  union { 
    Double_t _sum;
    Double_t _mean;
  };
  union {
    Double_t _sum2;
    Double_t _sigma;
  };
};

inline std::ostream &operator <<(std::ostream &os, AverageSimple_t &a) {
  os << a._mean << "+-" 
     << a._sigma 
    ;
  return os;
}

#endif
