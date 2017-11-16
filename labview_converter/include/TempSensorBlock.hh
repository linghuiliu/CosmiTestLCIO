#ifndef TempSensorBlock_h
#define TempSensorBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define TSNINT 1
#define TSNFLOAT 1
#define TSNDOUBLE 0

using namespace lcio ;

namespace CALICE {
    
    class TempSensorBlock ;
    
    /**
     * Class for the Labview Data as acquired by the AHCAL Labview.
     * The class reflects that the data are received in the Labview
     * @author S. Lu DESY Hamburg
     * @date Dec 17 2012
     */
    class TempSensorBlock : public LCFixedObject<TSNINT,TSNFLOAT,TSNDOUBLE> {
        
    public:
        
        TempSensorBlock() {};
        
        /** Convenient c'tor.
         */
        TempSensorBlock(int TempSensorNumber, float TempSensorValue) {
            
            obj()->setIntVal(0, TempSensorNumber);
            obj()->setFloatVal(0, TempSensorValue);

        }
        
        /** 'Copy constructor' needed to interpret LCCollection read from file/database.
         */
        TempSensorBlock(LCObject* obj) : LCFixedObject<TSNINT,TSNFLOAT,TSNDOUBLE>(obj) { }
        
        /** Important for memory handling*/
        virtual ~TempSensorBlock() { /* no op*/  }
        
        int GetTempSensorNumber() const {
            return getIntVal(0);
        }
        
        float GetTempSensorValue() const {
            return getFloatVal(0);
        }
        
        void print(  std::ostream& os, int ) ;
        
        // -------- need to implement abstract methods from LCGenericObject
        
        /** Return the type of the class
         */
        const std::string getTypeName() const {
            return"TempSensorBlock" ;
        }
        
        /** Return a brief description of the data members
         */
        const std::string getDataDescription() const {
            return "i:TempSensorNr; f:TempSensorValue" ;
        }
        
    };
    
}

#endif
//=============================================================================
