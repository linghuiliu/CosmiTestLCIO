#include "Histogram2D.hh"
#include <Exceptions.h>

namespace histmgr {

const std::string Histogram2D::__typeName="Histogram2D";
const std::string Histogram2D::__description="Histogram2D";

Histogram2D::Histogram2D(lcio::LCObject* a_obj) 
  : _obj(dynamic_cast<lcio::LCGenericObjectImpl*>( a_obj )),
    _createdObject( false)
{
    
    if( _obj==0 ){
    
      Histogram2D* f = 
    	dynamic_cast< Histogram2D* >( a_obj ) ;
      
      if( f != 0 )
	_obj = f->obj() ;
    }

    // do some sanity checks ...
    if( _obj==0 ){
      throw lcio::Exception("Cannot create Histogram2D from sth."
		      " that is not LCGenericObjectImpl" ) ;
    } 
    
    if(  ( _obj->getNInt()    != kNHistogram2DInts )    || 
	 ( _obj->getNFloat()  < kNHistogram2DFloats )  || 
	 ( _obj->getNDouble() != kNHistogram2DDoubles )   ) {
      
      throw lcio::Exception("Histogram2D(Histogram2D): Wrong number of elements in object. Cannot be a Histogram2D" ) ;
    }
    
    setXtoBins();
    setYtoBins();
}

}
