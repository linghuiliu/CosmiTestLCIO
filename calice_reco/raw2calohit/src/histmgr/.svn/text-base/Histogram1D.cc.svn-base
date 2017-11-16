#include "Histogram1D.hh"
#include <Exceptions.h>

namespace histmgr {

const std::string Histogram1D::__typeName="Histogram1D";
const std::string Histogram1D::__description="Histogram1D";

Histogram1D::Histogram1D(lcio::LCObject* a_obj) 
  : _obj(dynamic_cast<lcio::LCGenericObjectImpl*>( a_obj )),
    _createdObject( false)
{
    
    if( _obj==0 ){
    
      Histogram1D* f = 
    	dynamic_cast< Histogram1D* >( a_obj ) ;
      
      if( f != 0 )
	_obj = f->obj() ;
    }

    // do some sanity checks ...
    if( _obj==0 ){
      throw lcio::Exception("Cannot create Histogram1D from sth."
		      " that is not LCGenericObjectImpl" ) ;
    } 
    
    if(  ( _obj->getNInt()    != kNHistogram1DInts )    || 
	 ( _obj->getNFloat()  < kNHistogram1DFloats )  || 
	 ( _obj->getNDouble() != kNHistogram1DDoubles )   ) {
      
      throw lcio::Exception("Histogram1D(Histogram1D): Wrong number of elements in object. Cannot be a Histogram1D" ) ;
    }
}

}
