#ifndef _SimpleArray_t_H_
#define _SimpleArray_t_H_

#include <vector>
#ifdef BOUNDARY_CHECK
#include <stdexcept>
#include <cassert>
#endif

/** Wrapper around std::vector which adds optional boundary check (at compile time).
 * To have boundary checks whenever elements are accessed with the operator [],
 * define the preprocessor variable BOUNDARY_CHECK:
 * <code>
 *    e.g: make CXXFLAGS="-g -DBOUNDARY_CHECK"
 * </code>
 * NOTE: the validity of iterators is <b>never</b> verified.
 */
template <class T>
class SimpleArray_t : public std::vector<T>
{
public:
  T &operator[](UInt_t index) 
#ifdef BOUNDARY_CHECK
    throw (std::range_error)
#endif    
  {
#ifdef BOUNDARY_CHECK
    if (index>=std::vector<T>::size()) {
      *((char *) 0)='a';
      throw std::range_error(__ASSERT_FUNCTION);
    }
#endif
    return std::vector<T>::operator[](index);
  };

  const T &operator[](UInt_t index) const
#ifdef BOUNDARY_CHECK
    throw (std::range_error)
#endif    
  {
#ifdef BOUNDARY_CHECK
    if (index>=std::vector<T>::size()) {
      *((char *) 0)='a';
      throw std::range_error(__ASSERT_FUNCTION);
    }
#endif
    return std::vector<T>::operator[](index);
  };
};
#endif
