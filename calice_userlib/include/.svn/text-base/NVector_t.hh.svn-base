#ifndef NVector_t_hh
#define NVector_t_hh

#include <RtypesSubSet.h>
#ifdef BOUNDARY_CHECK
#include <stdexcept>
#endif
#include <math.h>

namespace CALICE {

  template <class T, int dimension> class  NVector_t;

  /** A three dimensional single precision floating point vector.
   */
  typedef NVector_t<Float_t, 3> ThreeVector_t;

  /** Simple n-dimensional vector (size of the vector is defined at compile time)
   * Provides methods to add and subtract vectors or multiply and divide by scalars.
   * There are also functions to calculate the scalar and cross products (the latter 
   * for three dimensional vectors only).
   * A flat array (currently floats only) can be casted to an NVector_t:
   * <code>
   * lcio::CalorimeterHit *a_hit;
   * NVector_t<Float_t,3> *vector=static_cast<NVector_t<Float_t,3> *>(a_hit->getPosition());
   * </code>
   */
  template <class T, int dimension>
  class  NVector_t
  {
    template <class _T, int _dimension> friend  NVector_t<_T,_dimension> operator-(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b);
    template <class _T, int _dimension> friend  NVector_t<_T,_dimension> operator+(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b);
    template <class _T, int _dimension> friend  NVector_t<_T,_dimension> operator*(const NVector_t<_T,_dimension> &a, float c);
    template <class _T, int _dimension> friend  NVector_t<_T,_dimension> operator*(float c, const NVector_t<_T,_dimension> &a);
    template <class _T, int _dimension> friend  NVector_t<_T,_dimension> operator/(const NVector_t<_T,_dimension> &a, float c);
    template <class _T, int _dimension> friend  Double_t norm(const NVector_t<_T,_dimension> &a);
    template <class _T, int _dimension> friend  Double_t dot(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b);
    friend ThreeVector_t cross(const ThreeVector_t &a, const ThreeVector_t &b);

  public:

    /** Set all elements to zero.
     * @todo this only works for simple types like int, float, double.
     */
    NVector_t &clear() {
      for (UInt_t  i=0; i<dimension; i++) {_x[i]=0;};
      return *this;
    };

    /** Get the dimension of the vector. 
     */
    UInt_t n() const {return dimension;};

    /** Set the elements of the vector from the given array.
     * It is assumed that the array contains the correct amount of elements (dimension of the vector).
     * If the dimension of the given array is smaller than the dimension of this vector
     * then the result will be undefined.
     */
    NVector_t &set(const T *array) {
      for (UInt_t  i=0; i<dimension; i++) {_x[i]=array[i];};
      return *this;
    };

    /** get the element of the given index (read and write access).
     * @param index the index of the element (0 to dimension - 1).
     * @return reference to the element of the given index (read and write access).
     * When the preprocessor variable BOUNDARY_CHECK is defined
     * the validity of index is checked and an exception std::runtime_error 
     * is thrown in case of violations.
     */
    Float_t &operator[](UInt_t index) 
#ifdef BOUNDARY_CHECK
      throw(std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (index>=dimension) {
        throw std::range_error("NVector_t> out of range error");
      }
#endif
      return _x[index];
    };

    /** get the element of the given index (read access only).
     * @param index the index of the element (0 to dimension - 1).
     * @return reference to the element of the given index (read access only).
     * When the preprocessor variable BOUNDARY_CHECK is defined
     * the validity of index is checked and an exception std::runtime_error 
     * is thrown in case of violations.
     */
    const Float_t &operator[](UInt_t index) const
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (index>=dimension) {
        throw std::range_error("NVector_t> out of range error");
      }
#endif
      return _x[index];
    };
 
    /** Add a vector having the same dimension to this vector.
     * FIXME: The compiler should create an error message if vectors of different dimensions are added 
     *        Does this happen ?
     */
    NVector_t &operator+=(const NVector_t &a) {
      for (UInt_t  i=0; i<dimension; i++) {_x[i]+=a._x[i];};
      return *this;
    };

    /** Subtract a vector having the same dimension to this vector.
     * FIXME: The compiler should create an error message if vectors of different dimensions are added 
     *        Does this happen ?
     */
    NVector_t &operator-=(const NVector_t &a) {
      for (UInt_t  i=0; i<dimension; i++) {_x[i]-=a._x[i];};
      return *this;
    };

    /** Multiply this vector by a scalar.
     * @todo the argument should not be a float but T ?
     */
    NVector_t &operator*=(const float c) {
      for (UInt_t  i=0; i<dimension; i++) {_x[i]*=c;};
      return *this;
    };

    /** Divide this vector by a scalar.
     * @todo the argument should not be a float but T ?
     */
    NVector_t &operator/=(const float c) {
      for (UInt_t  i=0; i<dimension; i++) {_x[i]/=c;};
      return *this;
    };

    /** get a pointer to the underlying array (read and write access).*/
    Float_t *data() {return _x;};               /**< @todo float ? not T ?*/

    /** get a pointer to the underlying array (read only access).*/
    const Float_t *data() const {return _x;};   /**< @todo float ? not T ?*/
  private:
    Float_t _x[dimension]; /**< @todo float ? not T ?*/
  };

  /** Subtract two vectors of the same dimension.
   */
  template <class _T, int _dimension>
  inline NVector_t<_T,_dimension> operator-(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b)
  {
    NVector_t<_T,_dimension> res;
    for (UInt_t  i=0; i<_dimension; i++) {res._x[i]=a._x[i]-b._x[i];};
    return res;
  }

  /** Add two vectors of the same dimension.
   */
  template <class _T,int _dimension>
  inline NVector_t<_T,_dimension> operator+(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b)
  {
    NVector_t<_T,_dimension> res;
    for (UInt_t  i=0; i<_dimension; i++) {res._x[i]=a._x[i]+b._x[i];};
    return res;
  }

  /** Multiply a vector by a scalar.
   * @todo replace float by _T?
   */
  template <class _T,int _dimension>
  inline NVector_t<_T,_dimension> operator*(const float c, const NVector_t<_T,_dimension> &a)
  {
    NVector_t<_T,_dimension> res;
    for (UInt_t  i=0; i<_dimension; i++) {res._x[i]=c*a._x[i];};
    return res;
  }

  /** Multiply a vector by a scalar.
   * @todo replace float by _T?
   */
  template <class _T,int _dimension>
  inline NVector_t<_T,_dimension> operator*(const NVector_t<_T,_dimension> &a, const float c)
  {
    NVector_t<_T,_dimension> res;
    for (UInt_t  i=0; i<_dimension; i++) {res._x[i]=c*a._x[i];};
    return res;
  }

  /** Divide a vector by a scalar.
   * @todo replace float by _T?
   */
  template <class _T,int _dimension>
  inline NVector_t<_T,_dimension> operator/(const NVector_t<_T,_dimension> &a, const float c)
  {
    NVector_t<_T,_dimension> res;
    for (UInt_t  i=0; i<_dimension; i++) {res._x[i]=a._x[i]/c;};
    return res;
  }

  /** Scalar product of two vectors of the same dimension.
   */
  template <class _T,int _dimension>
  inline Double_t dot(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b)
  {
    Double_t sum=0.;
    for (UInt_t  i=0; i<_dimension; i++) {sum+=a._x[i]*b._x[i];};
    return sum;
  }

  /** Calculate the norm squared of the difference of the two given vectors.
   */
  template <class _T,int _dimension>
  inline Double_t dotOfDifference(const NVector_t<_T,_dimension> &a, const NVector_t<_T,_dimension> &b)
  {
    Double_t sum=0.;
    for (UInt_t  i=0; i<_dimension; i++) {_T temp=a[i]-b[i]; sum+=temp*temp;};
    return sum;
  }

  /** Euclidean Norm of a vector.
   */
  template <class _T,int _dimension>
  inline Double_t norm(const NVector_t<_T,_dimension> &a)
  {
    Double_t sum=0.;
    for (UInt_t  i=0; i<_dimension; i++) {sum+=a._x[i]*a._x[i];};
    return sqrt(sum);
  }

  /** Calculate the perpendicular distance between the line and the point.
   * @param line_origin the origin of the line
   * @param line_direction the direction of the line must have norm 1.
   * @param point position of the point.
   * @return the distance between the line and the point (along the perpendicular).
   */
  template <class _T,int _dimension>
  Float_t distance(const NVector_t<_T,_dimension> &line_origin, const NVector_t<_T,_dimension> &line_direction, const NVector_t<_T,_dimension> &point) {
    //_axis has norm 1
    NVector_t<_T,_dimension> delta=point-line_origin;
    Double_t r=dot(delta,line_direction);
    return norm(delta-r*line_direction);
  }


  /** cross product of two three dimensional vectors.
   */
  inline ThreeVector_t cross(const ThreeVector_t &a, const ThreeVector_t &b)
  {
    const UInt_t cross_index[3][2]={{1,2},{2,3},{3,1}};
    ThreeVector_t res;
    for (UInt_t  i=0; i<3; i++) {res[i]+=a._x[cross_index[i][0]]*b._x[cross_index[i][1]]-a._x[cross_index[i][1]]*b._x[cross_index[i][0]];};
    return res;
  }

}
#endif
