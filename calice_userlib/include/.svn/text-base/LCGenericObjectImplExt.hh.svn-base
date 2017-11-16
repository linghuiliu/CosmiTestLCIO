#ifndef LCGENERICOBJECTIMPLEXT_H
#define LCGENERICOBJECTIMPLEXT_H 1

#include "lcio.h"
#include "IMPL/LCGenericObjectImpl.h"

#include <iostream>

using namespace lcio ;

namespace CALICE{

  /** Extends functionality of the LCGenericObjectImpl Class
   *  It simply adds a new constructor to the LCGenericObject Class  
   *  such that we can construct an interface class around without
   *  being restricted by the LCFixedObject
   *  It uses an instance of LCGenericObjectImpl that holds the data, thus 
   *  there is no overhead when the data is read from a database or file
   *  for copying it  to some local structure (Decorator pattern).<br>
   *  This is still an abstract class: subclasses have to implement 
   *  LCGenericObj ect::getTypeName() 
   *  and LCGenericObject::getDataDescription().
   *  It is basically a copy of the lcio::UTIL::LCFixedObject class
   *  I don't know whether this is the best solution, maybe it is
   *  better to use later on a real extension of the LCGenericObjectImpl 
   * @author Roman Poeschl, LAL Orsay
   * @date Feb. 2006
   */

  class LCGenericObjectImplExt : public LCGenericObject {

  public:
    
  /** Default c'tor.
   */
    LCGenericObjectImplExt():_createdObject( true )  {
      //create a simple generic object
      _obj = new LCGenericObjectImpl() ;
    }
    

    LCGenericObjectImplExt(LCObject* obj) : 
      _createdObject(false)  {
      
      _obj = dynamic_cast<LCGenericObjectImpl*>( obj )  ;
      

       

      //a sanity check ...

      if( _obj==0 ){

	// could be an instance of LCGenericObjectImplExt !?
	LCGenericObjectImplExt* f = 
	  dynamic_cast< LCGenericObjectImplExt* >( obj ) ;

	if( f != 0 )
	  _obj = f->obj() ;

      }

      //Throw an exception if sthg. goes wrong
      if( _obj==0 ){
        throw Exception("Cannot create LCGenericObjectImplExt from sth."
                        " that is not LCGenericObjectImpl" ) ;
      } 
      
    }
    

    /** The LCGenericObjectImpl . Sublcasses use this to access their data.
     */
    LCGenericObjectImpl* obj() { return _obj ; }
    
    
    /** Clean up if we created a new LCGenericObjectImpl 
     */
    virtual ~LCGenericObjectImplExt() { 
      if( _createdObject )  delete _obj ; 
    }
    
    
    /** Return the id of the underlying LCGenericObjectImpl */
    virtual int id() { return _obj->id() ; }
    
    
    // ---- need to implement LCGenericObject interface:
    
    int getNInt() const    { return _obj->getNInt() ; } 
    int getNFloat() const  { return _obj->getNFloat() ; }  
    int getNDouble() const { return _obj->getNDouble() ; } 
    
    int getIntVal(int index) const {
      return _obj->getIntVal( index ) ;
    }
    float getFloatVal(int index) const { 
      return _obj->getFloatVal( index ) ;
    }
    double getDoubleVal(int index) const {
      return  _obj->getDoubleVal( index ) ;
    }
    bool isFixedSize() const { return false ; }
    
    // ---- end of LCGenericObject interface
    
  protected:
    
    LCGenericObjectImpl* _obj ;
    bool _createdObject ;
 };

} // namespace
#endif /* ifndef LCGENERICOBJECTIMPLEXT_H */
