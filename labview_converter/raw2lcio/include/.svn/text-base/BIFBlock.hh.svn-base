#ifndef BIFBlock_h
#define BIFBlock_h 1

//BIF PARAMETERS
#define BIF_START_ACQ 0x03000000
#define BIF_STOP_ACQ 0x02000000
#define BIF_OFFSET 0
#define BIF_BINNING 5120
#define BIF_RESOLUTION 0.78125

//BIF LEMO Connection definition 
#define BIF_1ST_LEMO 0x1000000
#define BIF_2ND_LEMO 0x1010000
#define BIF_3RD_LEMO 0x1020000
#define BIF_4TH_LEMO 0x1030000

//lcio headers
#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 2
#define NFLOAT 1
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

  class BIFBlock ;

  /**
   * Class for the SLCIO EUDAQ BIF Data as acquired by the EUDAQ system.
   * @author E.Brianne @ DESY Hamburg
   * @date May 2016
   * Created for 2016 testbeams EUDAQ data format.
   */
  class BIFBlock : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
  public: 
  
    /** Constructor 
     */
    BIFBlock() {};

    /** Convenient constructor */
    BIFBlock(int TriggerSource, int BXID_BIF, float Time_BIF) {

      obj()->setIntVal( 0 ,  TriggerSource );
      obj()->setIntVal( 1 ,  BXID_BIF );
      obj()->setFloatVal( 0 ,  Time_BIF );

    }

    /** Destructor
     */
    virtual ~BIFBlock() {};

    /** Convenient print method 
     */
    void print(  std::ostream& os, int ) ;

    /** Set Trigger type */
    void setTriggerSource(int TriggerSource) {
      obj()->setIntVal( 0 ,  TriggerSource );
    }
    
    /** Set BXID of the BIF Trigger */
    void setBXID(int BXID_BIF) {
      obj()->setIntVal( 1 ,  BXID_BIF );
    }

    /** Set Time of the BIF Trigger */
    void setTime(float Time_BIF) {
      obj()->setFloatVal( 0 ,  Time_BIF );
    }
    
    // -------- need to implement abstract methods from LCGenericObject

    /** Get Trigger type */
    const int getTriggerSource() const {
      return getIntVal(0);
    }
    
    /** Get BXID of the BIF Trigger */
    const int getBXID() const {
      return getIntVal(1);
    }

    /** Get Time of the BIF Trigger */
    const float getTime() const {
      return getFloatVal(0);
    }

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return "BIFBlock";
    } 
  
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:InputSource; i:BXID:f:Time";
    }

  };//end class

}//end namespace
#endif
