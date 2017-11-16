#include <cloneUtils.hh>

#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCObject.h>
#include <LCGenericObjectImplCloner.hh> 
#include <stdexcept>

namespace CALICE {
/** Implementation of cloning functions */

  EVENT::LCGenericObject *cloneLCGenericObject(EVENT::LCObject *obj) {
    EVENT::LCGenericObject *generic_object=dynamic_cast<LCGenericObject *>(obj);
    if (!generic_object) {
      throw std::runtime_error( "ERROR:For the time being only objects of type LCGenericObject can be cloned." );
    }

    IMPL::LCGenericObjectImpl *clone=new LCGenericObjectImplCloner(*generic_object);
    return clone;
  }

  EVENT::LCGenericObject *cloneLCGenericObject(EVENT::LCGenericObject *generic_object) {
    if (!generic_object) {
      throw std::runtime_error( "ERROR:For the time being only objects of type LCGenericObject can be cloned." );
    }

    IMPL::LCGenericObjectImpl *clone=new LCGenericObjectImplCloner(*generic_object);
    return clone;
  }

  EVENT::LCCollection *cloneCollection(EVENT::LCCollection *col)
  {
    // Clone collection
    // ---
    // Unfortunately, the collection has to be cloned because the original collection 
    // will be deleted  when the conditions data changes which is just before it would 
    // be written.
    
    IMPL::LCCollectionVec *a_col=new IMPL::LCCollectionVec(col->getTypeName());
    // clone the elements

    if (col->getNumberOfElements()>0) {
      bool type_names_are_identical=true;
      bool objects_are_fixed_size=true;
      EVENT::LCGenericObject *first_object=dynamic_cast<EVENT::LCGenericObject *>(col->getElementAt(0));
      if (!first_object) {
	throw std::runtime_error( "ERROR:For the time being only objects of type LCGenericObject can be cloned." );
      }
      
      for (UInt_t element_i=0; element_i<static_cast<UInt_t>(col->getNumberOfElements()); element_i++) {
	EVENT::LCGenericObject *new_object=dynamic_cast<EVENT::LCGenericObject *>(col->getElementAt(element_i));
	a_col->addElement(cloneLCGenericObject(new_object));
	if (!new_object->isFixedSize()) {
	  objects_are_fixed_size=false;
	}
	if (first_object->getTypeName()!=new_object->getTypeName()) {
	  type_names_are_identical=false;
	} 
      }

      if (!type_names_are_identical) {
	throw std::logic_error("ERROR:cloneCollection> No support for collection which contain objects of different types.");
      }
      if(  col->parameters().getStringVal( "TypeName" ).size() ==  0 )
	a_col->parameters().setValue( "TypeName", first_object->getTypeName() ) ;
      if (objects_are_fixed_size) {
	if(  col->parameters().getStringVal( "DataDescription" ).size() ==  0 )
	  a_col->parameters().setValue( "DataDescription", first_object->getDataDescription() ) ;
      }
    }
    
    // clone the parameters elements
    // Unfortunately, this requires a lot of pointless copying.
    StringVec keys;
    // copy int parameters
    col->getParameters().getIntKeys(keys);
    for (StringVec::const_iterator key_iter=keys.begin();
	 key_iter!= keys.end();
	 key_iter++) {
      IntVec values;
      a_col->parameters().setValues(*key_iter,col->getParameters().getIntVals(*key_iter,values));
    }

    // copy float parameters
    keys.clear();
    col->getParameters().getFloatKeys(keys);
    for (StringVec::const_iterator key_iter=keys.begin();
	 key_iter!= keys.end();
	 key_iter++) {
      FloatVec values;
      a_col->parameters().setValues(*key_iter,col->getParameters().getFloatVals(*key_iter,values));
    }

    // copy string parameters
    keys.clear();
    col->getParameters().getStringKeys(keys);
    for (StringVec::const_iterator key_iter=keys.begin();
	 key_iter!= keys.end();
	 key_iter++) {
      StringVec values;
      a_col->parameters().setValues(*key_iter,col->getParameters().getStringVals(*key_iter,values));
      //      std::cout << *key_iter << ":"; 
      //      for(StringVec::const_iterator value_iter=values.begin(); value_iter!=values.end(); value_iter++) {
      //	std::cout << " " << *value_iter;
      //      }
      //      std::cout << std::endl;
    }
    return a_col;
  }

}
