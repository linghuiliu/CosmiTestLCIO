#ifndef LCHCALCALIBRATIONOBJECT_H
#define LCHCALCALIBRATIONOBJECT_H 1

#include "lcio.h"
#include "IMPL/LCGenericObjectImpl.h"

#include <iostream>
#include <map>
#include <stdexcept>

using namespace lcio ;

namespace CALICE{

 /** Object to store information about arbitrary cell wise calibration of the HCAL in LC Objects.
   * @author S. Schmidt DESY
   * @date Jul 2006
   */

  class LCHcalCalibrationObject : public LCGenericObject {

  public:
    
  /** Default c'tor.
   */
    LCHcalCalibrationObject():_createdObject( true )  {
      //create a simple generic object
      _obj = new LCGenericObjectImpl() ;
    }
    

    LCHcalCalibrationObject(LCObject* obj) : 
      _createdObject(false)  {
      
      _obj = dynamic_cast<LCGenericObjectImpl*>( obj )  ;
      

       

      //a sanity check ...

      if( _obj==0 ){

	// could be an instance of LCGenericObjectImplExt !?
	LCHcalCalibrationObject* f = 
	  dynamic_cast< LCHcalCalibrationObject* >( obj ) ;

	if( f != 0 )
	  _obj = f->obj() ;

      }

      //Throw an exception if sthg. goes wrong
      if( _obj==0 ){
        throw Exception("Cannot create LCHcalCalibrationObject from sth."
                        " that is not LCGenericObjectImpl" ) ;
      } 
      
    }
    

    /** The LCGenericObjectImpl . Sublcasses use this to access their data.
     */
    LCGenericObjectImpl* obj() { return _obj ; }
    
    
    /** Clean up if we created a new LCGenericObjectImpl 
     */
    virtual ~LCHcalCalibrationObject() { 
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
    
    float getConstant(){return getFloatVal(0);}
    float getConstantError(){return getFloatVal(1);}
    void setConstant( float c ){_obj->setFloatVal(0, c );}
    void setConstantError( float ce ){_obj->setFloatVal(1, ce );}


    // ---- end of LCGenericObject interface

    /** get cell key of cell
      */    
    unsigned getCellKey() const {
      return (unsigned) (_obj->getIntVal(0));
    };  
    
    /** set cell key of cell 
      */
    void setCellKey(unsigned cellKey) {
      _obj->setIntVal(0, (int) cellKey);   
    }; 

    /** get chip number of cell 
      */
    unsigned getChip() const {
      return (unsigned) ((getCellKey() & 0xFF00)>>8);
    };
  
    /** get channel number of cell  
      */
    unsigned getChannel() const {
      return (unsigned) (getCellKey() & 0x00FF);
    };

    /** let the object apply the calibration stored in the object
      * @param inputValue "energy" value of the cell before the calibration
      * @return "energy" value of the cell after the calibration
      */  
    virtual float applyCalibration(float inputValue) const {
      return 0.;
    };
    
    /** cancel an applied calibration from energy value
      */
    virtual float cancelCalibration(float outputValue) const {
      return 0.;
    };  

    /** let the object calculate the error after applying the calibration stored in the object
      * @param inputValue "energy" value of the cell stored before the calibration
      * @param inputError error of the "energy" value of the cell before the calibration
      * @return error of the "energy" value of the cell after the calibration taking 
      *   into account the error of the "energy" value of the cell before the calibration
      *   and the error of the calibration itself
      */   
    virtual float applyCalibrationError(float inputValue, float inputError) const {
      return 0.;
    };
    
    /** apply error due to canceling a calibration to current error
      */
    virtual float cancelCalibrationError(float outputValue, float outputError) const {
      return 0.;
    };

    /** is the calibration stored in the object valid?
      * @return true if the calibration is valid, false if no valid calibration is available
      */    
    virtual bool calibrationValid() const {
      return true;
    }
    
    /** should the cell been kept in the cell selection after the calibration has been performed?
      * @param resultValue "energy" value of the cell after the calibration
      * @param resultError error on the "energy" value of the cell after the calibration
      */
    virtual bool keepEvent(float resultValue, float resultError) const {
      return true;
    }
    
    virtual float getParam(unsigned paramIndex) const {
      throw std::runtime_error("no parameters available");
    }
    
    virtual void setParam(unsigned paramIndex) {
      throw std::runtime_error("no parameters available");
    }; 

    virtual void setCellTemp(float tempValue, float tempError) {
      return;
    }
    
    virtual float getCellTemp() {
      return 0;
    }
    
    virtual float getCellTempError() {
      return 0;
    }    

    /** return type of the class
      */
    const std::string getTypeName() const {
      return "some HCAL Calibration constant";
    };   

    /** return a brief description of the data member
      */
    const std::string getDataDescription() const {
      return "i:CellKey";
    };
      
  protected:
    
    LCGenericObjectImpl* _obj ;
    bool _createdObject ;
 };

typedef std::map<unsigned,LCHcalCalibrationObject*> HcalCalibrationModuleMap; 
typedef std::pair<lcio::LCCollection*, HcalCalibrationModuleMap*> HcalCalibrationModuleData;

} // namespace
#endif /* ifndef LCHCALCALIBRATIONOBJECT_H */
