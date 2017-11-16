#ifndef AhcSlowReadoutBlock_hh
#define AhcSlowReadoutBlock_hh 1

#include "lcio.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#ifdef VALUE_CHECK
#  include <cassert>
#endif


using namespace lcio ;

namespace CALICE {
  
  /** The indices of the the stored values.
   */
  enum AhcSroDblValues {kxPosition_mm, kyPosition_mm, kzPosition_mm, kcPosition_deg, kSroDblValues};
  
  /** 
   * Interface Class to access the AhcSlowConfiguration Data 
   * For the time being we treat only the movable stage positions
   * Update: 6/7/08 Added z and rotated position of stage (Maintain backward compatibility)
   * @author: Roman Pöschl DESY
   * @date Nov  2005
   */
  class AhcSlowReadoutBlock : public LCGenericObjectImplExt {
    
    
  public: 
    
    
    /** Simple Constructor.
     */
    AhcSlowReadoutBlock() {};
    
    
    //Set Parameters of the board by which the data were taken  
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    AhcSlowReadoutBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
    
    AhcSlowReadoutBlock & set_xyzPosition_mm(double xpos, double ypos, double zpos) {
      
      obj()->setDoubleVal(kxPosition_mm, xpos);
      obj()->setDoubleVal(kyPosition_mm, ypos);
      obj()->setDoubleVal(kzPosition_mm, zpos);
      return *this;
    }  
    
    AhcSlowReadoutBlock & set_rotPosition_deg(double cpos) {
      obj()->setDoubleVal(kcPosition_deg, cpos);
      return *this;
    }  
    
    
    virtual double get_xPosition_mm() { return obj()->getDoubleVal(kxPosition_mm);} 
    virtual double get_yPosition_mm() { return obj()->getDoubleVal(kyPosition_mm);} 
    virtual double get_zPosition_mm() { return obj()->getDoubleVal(kzPosition_mm);} 
    virtual double get_cPosition_deg() { return obj()->getDoubleVal(kcPosition_deg);} 
    
    /** Convenient print method 
     */
    void print(  std::ostream& os); 
    
  // -------- need to implement abstract methods from LCGenericObject
    
  /** Return the type of the class 
   */
    const std::string getTypeName() const { 
      return"AhcSlowReadoutBlock" ;
    } 
    
    /** Return a brief description of the data members 
   */
    const std::string getDataDescription() const {
      return "d:xPosition_mm, d:yPosition_mm, d:zPosition_mm, d:cPosition_deg" ;
    }
    
  }; // class
  
}
#endif 
