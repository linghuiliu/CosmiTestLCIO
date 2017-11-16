#ifndef __AverageValue_hh__
#define __AverageValue_hh__ 1

#include <cmath>

/**  Collect input values and calculate mean/RMS upon request
 *
 *   This class keeps sum, sum-of-squares and number of input 
 *   values to calculate mean/RMS. Templated value data type 
 *   allows for flexible memory managemant, while internal
 *   calculations are done in double precision to avoid numerical
 *   cut-offs.
 *
 *   @author Niels.Meyer@desy.de
 *   @date   January 2008
 */

template<class T>
class AverageValue{
 public:
  AverageValue():_num(0), _sum(0), _sum2(0){}
  ~AverageValue(){}


  //  add value to be considered for next call to mean/RMS calculation
  AverageValue& addValue(T e){
    _sum += e;
    _sum2 += e*e;
    ++_num;
    return (*this);
  }


  //  reset members
  AverageValue& reset(){
    _sum = 0;
    _sum2 = 0;
    _num = 0;
    return (*this);
  }


  //  return functions for private members
  const unsigned int getNumberOfValues() const { return _num; }

  const T getSum() const { return _sum; }

  const T getSumOfSquares() const { return _sum2; }


  //  value and uncertainty of mean
  const T getMean() const { 
    return _num>0 
      ? _sum / (T)_num 
      : 0; 
  }

  const T getMeanUncertainty() const {
    return _num>0 
      ? (T)getRMS() / sqrt( (T)_num ) 
      : 0; 
  }

  //  value and uncertainty of RMS
  const T getRMS() const { 
    return getMeanOfSquares() <= getMeanSquared() ? 0 : 
      (T)sqrt( getMeanOfSquares() - getMeanSquared() ); 
  }
  const T getRMSUncertainty() const {
    return 2*getMeanUncertainty();
  }

 protected:
  //  internal calculations done in double precision independent of T
  const double getSumSquared() const { 
    return (double)_sum*(double)_sum; 
  }

  const double getMeanSquared() const { 
    return _num>0 ? getSumSquared()/(double)(_num*_num) : 0; 
  }

  const double getMeanOfSquares() const { 
    return _num>0 ? _sum2/(double)_num : 0; 
  }
  
 private:
  //  data members
  unsigned int _num;
  T _sum;
  T _sum2;

}; // class ConfigurationAverage

#endif
