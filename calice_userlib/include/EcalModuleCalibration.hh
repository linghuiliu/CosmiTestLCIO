#ifndef EcalModuleCalibration_hh
#define EcalModuleCalibration_hh

#include "lcio.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "stringFromToInts.hh"

#include <sstream>
#ifdef BOUNDARY_CHECK
#include <stdexcept>
#endif
#include <climits>

#include <RtypesSubSet.h>

using namespace lcio;
namespace CALICE {

  enum EEcalModuleCalibrationIntType {kEcalModuleCalibrationIntModuleID,kEcalModuleCalibrationIntNameLength,kNEcalModuleCalibrationInts};
  enum EEcalModuleCalibrationFloatType {kNEcalModuleCalibrationFloats};

  /** Calibration constants for the Calice ECAL detector modules.
   * Based on LCFixedObject.
   * @author Goetz Gayckem, LLR - ecole polytechnique)
   * @version $Id $
   */
  class EcalModuleCalibration : public lcio::LCGenericObject {
    
  public: 
    /** Default c'tor.
     */
    EcalModuleCalibration():
      _createdObject( true )  {
      //create a generic object of variable size
      _obj = new LCGenericObjectImpl;
      _obj->setIntVal(kEcalModuleCalibrationIntNameLength,0);
      _obj->setIntVal(kEcalModuleCalibrationIntModuleID,INT_MAX);
    }

    /** UsefulConstructor
     */
    EcalModuleCalibration(const std::string &module_type_name, UInt_t module_id, UInt_t n_cells);

    /** Get the name assigned to the module type.
     * The module type together with the module ID is considered to be unique.
     * Due to the character encoding in an integer array, the function may perform slowly.
     */
    std::string getModuleTypeName() const {
      return getStringFromInts(_obj,kNEcalModuleCalibrationInts,kEcalModuleCalibrationIntNameLength);
    };

    /** Set the unique module ID.
     * A module is uniquely identified by the module ID (serial number) and the module type. 
     */
    EcalModuleCalibration &setModuleID(UInt_t module_id) {
      _obj->setIntVal(kEcalModuleCalibrationIntModuleID,(int) module_id);
      return *this;
    };
    /** Get the unique module ID.
     * A module is uniquely identified by the module ID (serial number) and the module type. 
     */
    UInt_t getModuleID() const
    { 
      return (UInt_t) _obj->getIntVal(kEcalModuleCalibrationIntModuleID);
    };

    /** Set the calibration constant of thye given cell.
     * @param cell_index the cell index (electrical order: first the values of the first sample from all chips, then the second sample from all chips etc.)
     * @param calibration_constant the calibration constant which is multiplied to the ADC value.
     */
    EcalModuleCalibration &setCalibrationConstant(UInt_t cell_index, float calibration_constant) 
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (cell_index>(UInt_t) getNFloat()) {
	throw std::range_error("EcalModuleCalibration::setCalibrationConstant> Cell index out of bounds!");
      }
#endif
      _obj->setFloatVal(kNEcalModuleCalibrationFloats+cell_index,calibration_constant);
      return *this;
    };
    Float_t getCalibrationConstant(UInt_t cell_index) const
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (cell_index>(UInt_t) getNFloat()) {
	throw std::range_error("EcalModuleCalibration::getCalibrationConstant> Cell index out of bounds!");
      }
#endif
      return _obj->getFloatVal(kNEcalModuleCalibrationFloats+cell_index);
    };
    
    UInt_t getNCells() const 
#ifdef BOUNDARY_CHECK
      throw (std::logic_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if (getNFloat()<kNEcalModuleCalibrationFloats) {
	throw std::logic_error("The number of cells is negativ!");
      }
#endif
      return (getNFloat()-kNEcalModuleCalibrationFloats);
    };


    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return std::string("EcalModuleCalibration");
    };
  
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      std::stringstream message;
      message << "i[1]:type, f[" << getNFloat()/2 << "]: (x,y)";
      return message.str();
    };

    
    /** C'tor to be used for elements of LCGenericObjects read from
     * an LCIO file or the database. Subclasses should 'override' this, e.g.:<br>
     * Myclass(LCObject* obj) : EcalModuleCalibration(obj) {} <br>
     */
    EcalModuleCalibration(LCObject* obj) : 
      _createdObject(false)  {
      
      _obj = dynamic_cast<LCGenericObjectImpl*>( obj )  ;
      
      if( _obj==0 ){

	// could be an instance of EcalModuleCalibration !?
	EcalModuleCalibration* f = 
	  dynamic_cast< EcalModuleCalibration* >( obj ) ;

	if( f != 0 )
	  _obj = f->obj() ;

      }

      // do some sanity checks ...
      if( _obj==0 ){
	throw Exception("Cannot create EcalModuleCalibration from sth."
			" that is not LCGenericObjectImpl" ) ;
      } 
      
      if(  ( _obj->getNInt()    < kNEcalModuleCalibrationInts )   || 
	   ( _obj->getNFloat()  < kNEcalModuleCalibrationFloats)  ||
	   ( _obj->getNDouble() != 0 )   ) {

	throw Exception("EcalModuleCalibration(LCObject* obj): Wrong number of elements in object" ) ;
      }
      // do more sanity checks
      // zero cells is permitted!
      if (_obj->getNFloat() < kNEcalModuleCalibrationFloats) {
	throw Exception("EcalModuleCalibration(LCObject* obj): Number of cells is negative." ) ;
      }

      UInt_t name_ints=getNeededInts(_obj->getIntVal(kEcalModuleCalibrationIntNameLength));
      if ( static_cast<UInt_t>( _obj->getNInt() ) != kNEcalModuleCalibrationInts+name_ints ) {
	throw Exception("EcalModuleCalibration(LCObject* obj): The number of integers (including encoded string differs from the expectation." ) ;
      }

    }
    
    /** The LCGenericObjectImpl . Sublcasses use this to access their data.
     */
    LCGenericObjectImpl* obj() { return _obj ; }


    /** Clean up if we created a new LCGenericObjectImpl 
     */
    virtual ~EcalModuleCalibration() { 
      if( _createdObject )  delete _obj ; 
    }

    /** Return the id of the underlying LCGenericObjectImpl */
    virtual int id() { return _obj->id() ; }

    // ---- need to implement LCGenericObject interface:
    
    int getNInt() const    { return _obj->getNInt() ; };
    int getNFloat() const  { return _obj->getNFloat() ; };
    int getNDouble() const { return 0 ; }; 
    
    int getIntVal(int index) const {
      return _obj->getIntVal( index ) ;
    };
    float getFloatVal(int index) const { 
      return _obj->getFloatVal( index ) ;
    };
    double getDoubleVal(int index) const {
      return  _obj->getDoubleVal( index ) ;
    };
    bool isFixedSize() const { return false ; };
    
    // ---- end of LCGenericObject interface
    
  protected:
    
    LCGenericObjectImpl* _obj ;
    bool _createdObject ;
    
  }; 
  
} // namespace
#endif /* ifndef LCIO_LCFIXEDOBJECT_H */
