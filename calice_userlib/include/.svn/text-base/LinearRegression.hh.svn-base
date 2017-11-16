/* Copyright (c) 2004, Goetz Gaycken,  Institut de Recherches Subatomiques (IReS)
                                      / Universite Louis Pasteur Strasbourg
*/
// LinearRegression:
//   

#ifndef _LinearRegression_hh_
#define _LinearRegression_hh_

#include <math.h>

typedef double Double_t;
inline double sqr(double a) {return a*a;}

/** Linear regression analysis of given value pairs.
 */
class LinearRegression
{
public:
  LinearRegression() {reset();};

  /** initialise for a analysis of a new value set.
   */
  void reset() {_sumOfWeights=0;_sumX=0; _sumY=0; _sumXX=0; _sumXY=0;_sumYY=0.;};
  
  ~LinearRegression() {};

  /** Add a value pair where the second values has the given uncertainty.
   */
  LinearRegression &add(Double_t x, Double_t y, Double_t y_error) {
    Double_t weight=sqr(1/y_error);
    _sumOfWeights+=weight;
    _sumX+=x*weight;
    _sumY+=y*weight;
    _sumXX+=x*x*weight;
    _sumXY+=x*y*weight;
    _sumYY+=y*y*weight;
    return *this;
  };

  /** Add a value pair (if the uncertainty of the y-values is not known or are all the same)
   */
  LinearRegression &add(Double_t x, Double_t y) {
    _sumOfWeights+=1.;
    _sumX+=x;
    _sumY+=y;
    _sumXX+=x*x;
    _sumXY+=x*y;
    _sumYY+=y*y;
    return *this;
  };

  /** Add the values of another regression object to this object.
   */
  LinearRegression &add(const LinearRegression &a) {
    _sumOfWeights+=a._sumOfWeights;
    _sumX+=a._sumX;
    _sumY+=a._sumY;
    _sumXX+=a._sumXX;
    _sumXY+=a._sumXY;
    _sumYY+=a._sumYY;
    return *this;
  };

  /** Remove a value pair from the object.
   * May be called after an object has been added to remove it.
   */
  LinearRegression &subtract(Double_t x, Double_t y, Double_t y_error) {
    Double_t weight=sqr(1/y_error);
    _sumOfWeights-=weight;
    _sumX-=x*weight;
    _sumY-=y*weight;
    _sumXX-=x*x*weight;
    _sumXY-=x*y*weight;
    _sumYY-=y*y*weight;
    return *this;
  };

  /** Remove a value pair from the object (if the value pair has been added without specifying an uncertainty)
   * May be called after an object has been added without specifying the uncertainty to remove it.
   */
  LinearRegression &subtract(Double_t x, Double_t y) {
    _sumOfWeights-=1.;
    _sumX-=x;
    _sumY-=y;
    _sumXX-=x*x;
    _sumXY-=x*y;
    _sumYY-=y*y;
    return *this;
  };

  /** Remove all value pairs added to another regression object
   * from this regression object.
   */
  LinearRegression &Subtract(const LinearRegression &a) {
    _sumOfWeights-=a._sumOfWeights;
    _sumX-=a._sumX;
    _sumY-=a._sumY;
    _sumXX-=a._sumXX;
    _sumXY-=a._sumXY;
    _sumYY-=a._sumYY;
    return *this;
  };

  /** Calculate the ascent and the offset.
   */
  LinearRegression &calc() {
    Double_t delta=_sumOfWeights*_sumXX-_sumX*_sumX;
    delta=1/delta;
    _ascent=(_sumXY*_sumOfWeights-_sumX*_sumY)*delta;
    _offset=(_sumY*_sumXX-_sumX*_sumXY)*delta;
    return *this;
  };

  /** Calculate the ascent and the offset and their errors.
   */
  LinearRegression &calcWithErrors() {
    Double_t delta=_sumOfWeights*_sumXX-_sumX*_sumX;
    delta=1/delta;
    _ascent=(_sumXY*_sumOfWeights-_sumX*_sumY)*delta;
    _offset=(_sumY*_sumXX-_sumX*_sumXY)*delta;
    if (_sumOfWeights>2.) {
      delta*=(_sumOfWeights*_sumYY-_sumY*_sumY)/(_sumOfWeights*(_sumOfWeights-2));
      _ascentError=sqrt(_sumOfWeights*delta);
      _offsetError=sqrt(_sumXX*delta);
    }
    else {
      _ascentError=0.;
      _offsetError=0.;
    }
    return *this;
  };

  /** Return the calculated offset.
   * The result is undefined before @ref calc or @ref calcWithErrors was called.
   */
  Double_t offset() {return _offset;};

  /** Return the calculated ascent.
   * The result is undefined before @ref calc or @ref calcWithErrors was called.
   */
  Double_t ascent() {return _ascent;};

  /** Calculate the best guess of y for the given value of x.
   * Calcualte offset()+ascent()*x;
   * The result is undefined before @ref calc or @ref calcWithErrors was called.
   */
  Double_t eval(Double_t x) {return x*_ascent+_offset;};

  /** Calculate the uncertainty of a certain y for the given value of x.
   * @param x the x value for which the uncertainty on y should be calculated
   * @param x_ref the x value at which the uncertainty is only given by the 
   *         uncertainty of the offset() should be the average x value of the input set.
   * The result is undefined before @ref calc or @ref calcWithErrors was called.
   */
  Double_t evalError(Double_t x, Double_t x_ref) {
    Double_t ascent_error=(x-x_ref)*_ascentError; 
    Double_t offset_error=_offsetError;
    return sqrt(offset_error*offset_error+ascent_error*ascent_error);
  };

  /** Calculate the uncertainty of a certain y for the given value of x.
   * The result is undefined before @ref calc or @ref calcWithErrors was called.
   */
  Double_t evalError(Double_t x) {
    if (_sumOfWeights>0.) {
      Double_t ascent_error=(x-_sumX/_sumOfWeights)*_ascentError; 
      Double_t offset_error=_offsetError;
      return sqrt(offset_error*offset_error+ascent_error*ascent_error);
    }
    else {
      return 0.;
    }
  };

  /** Sum of the inverse uncertainties of the given y-values.
   */
  Double_t sumOfWeights() {
    return _sumOfWeights;
  };
  
  Double_t _ascent;
  Double_t _offset;
  Double_t _ascentError;
  Double_t _offsetError;

  Double_t _sumOfWeights;
  Double_t _sumX;
  Double_t _sumY;
  Double_t _sumXX;
  Double_t _sumXY;
  Double_t _sumYY;
};

#endif
