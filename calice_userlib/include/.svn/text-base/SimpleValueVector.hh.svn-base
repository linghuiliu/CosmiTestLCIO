#ifndef __SimpleValueVector_hh__
#define __SimpleValueVector_hh__

#include "IMPL/LCGenericObjectImpl.h"
#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <vector>

namespace CALICE {

  class SimpleValueVector : public IMPL::LCGenericObjectImpl {
  public:
    /**  Empty constructor
     */
    SimpleValueVector(){}

    /**  Constructor
     */
    SimpleValueVector( const int id,
		       const unsigned int nMemo,
		       const std::vector<float> val,
		       const std::vector<float> err,
		       const std::vector<int> stat );

    /**  Constructor from LCObject
     */
    SimpleValueVector( EVENT::LCObject* obj );

    /**  Destructor
     */
    ~SimpleValueVector(){}

    /**  Return index identifier
     */
    int getCellID() const {
      return getIntVal( 0 );
    }

    /**  Return size array
     */
    int getSize() const {
      return getIntVal( 1 );
    }

    /**  Return the value
     */
    float getValue( const unsigned int i) const {
      return getFloatVal( 2*i );
    }

    /**  Return the error on the value
     */
    float getError( const unsigned int i) const {
      return getFloatVal( 2*i+1 );
    }

    /**  Return the status
     */
    int getStatus(const unsigned int i) const {
      return getIntVal( i+2 );
    }

    /**  set index identifier
     */
    void setCellID(const int ID) {
      setIntVal( 0, ID );
    }

    /**  set size
     */
    void setSize(const int size) {
      setIntVal( 1, size );
    }

    /** set the value
     */
    void setValue( const unsigned int i,
		   const float val ) { setFloatVal( 2*i, val ); }

    /**  set the error on the value
     */
    void setError(const unsigned int i,
		  const float err ) { setFloatVal( 2*i+1, err ); }

    /**  set the status
     */
    void setStatus(const unsigned int i,
		   const int stat ) { setIntVal( i+2, stat ); }

    /**  Implementation of LCGenericObject::getTypeName
     */
    const std::string getTypeName() const { 
      return "SimpleValueVector"; 
    }

    /**  Implementation of LCGenericObject::getDataDescription
     */
    const std::string getDataDescription() const { 
      return "i:CellID,  i:nMemo,  f:value[nMemo],f:error[nMem],i:status[nMemo]"; 
    }

  protected:

  private:

  };  // classs SimpleValueVector

}  // namespace CALICE

#endif   // __SimpleValueVector_hh__
