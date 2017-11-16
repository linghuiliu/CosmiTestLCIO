#ifndef _Average_t_H_
#define _Average_t_H_

#include <iostream>
#include <iomanip>
// FLT_MAX
//#include <values.h>
#include <climits>
#include <cfloat>
#include <RtypesSubSet.h>
#if ( __GNUC__ < 3 )
#  include <math.h>
#else
#  include <cmath>
#endif

/** Calculate mean and standard deviation and find minimum and maximum value.
 * The minimum and maximum is not searched/memorised if AVERAGE_WITHOUT_MIN_MAX
 * is defined.
 */
class Average_t {
public:
  friend  std::ostream &operator <<(std::ostream &os, const Average_t &a) ;

  /** Default constructor initialses all arrays to be ready to gather statastics of a variable.
   */
  Average_t() : _weight(0.),
		_min(FLT_MAX),
		_max(-FLT_MAX),
		_sum(0.),
		_sum2(0.) {};

  /** Build the sum and the sum of squares and find the minimum and maximum value. 
   * The sums can be used later to calculate the mean and the standard deviation 
   * @param value the value to be added to the sum and the sum of squares
   * @param weight the weight given to this value in the sums.
   * (@sa calculate)
   */
  void add(Float_t value, Float_t weight=1.) {
    _weight+=weight; 
    _sum+=value*weight; 
    _sum2+=value*value*weight; 
    if (value<_min) _min=value; 
    if (value>_max) _max=value;
  };

  /** Calculate the standard deviation and the mean.
   * The mean and the standard deviation are stored in the locations which are also used for the sums. 
   * Therefore if further values are added after calculate() was called, the result is undefined.
   */
  void calculate() {
    if (_weight>1) {
      Double_t mean=_sum/_weight;
      Double_t sigma=sqrt((_sum2-mean*_sum)/(_weight-1));
      _mean=mean;
      _sigma=sigma;
    }
  };

  /** Return the mean value.
   * returns only a correct result after at least 1 value was added and calculate was called.
   */
  Double_t mean() const {return _mean;};

  /** Return the standard deviation.
   * returns only a correct result after at least 1 value was added and calculate was called.
   */
  Double_t sigma() const {return _sigma;};

  /** Return the minimum value.
   */
  Float_t min() const {return _min;};

  /** Return the maximum value.
   */
  Float_t max() const {return _max;};

  /** Get the total accumulated weight.
   */
  Double_t weight() const {return _weight;};
  
  /** Get the total accumulated data
   */
  Double_t sum() const {return _sum;};
  
protected:
  Double_t  _weight;
  Float_t _min;
  Float_t _max;
  union { 
    Double_t _sum;
    Double_t _mean;
  };
  union {
    Double_t _sum2;
    Double_t _sigma;
  };
};

inline std::ostream &operator <<(std::ostream &os, const Average_t &a) {
  os << std::setw(9) << a._weight << ":" 
     << std::setw(9) << a._min << " < " 
     << std::setw(9) << a._mean << "+-" 
     << std::setw(9) << a._sigma 
     << " < " << std::setw(9) << a._max
    ;
  return os;
}

#endif
