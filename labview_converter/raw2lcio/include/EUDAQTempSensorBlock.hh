#ifndef EUDAQTempSensorBlock_h
#define EUDAQTempSensorBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define TS2NINT 10
#define TS2NFLOAT 0
#define TS2NDOUBLE 0

using namespace lcio ;

namespace CALICE {
    
    class EUDAQTempSensorBlock ;
    
    /**
     * Class for the temperature sensor data  as acquired by the AHCAL EUDAQ.
     * The class reflects that the data are received in the EUDAQ
     * @author A. Irles DESY Hamburg, based on TempSensorBlock by S. Lu (date Dec 17 2012)
     * @date 25june 2015
     */
    class EUDAQTempSensorBlock : public LCFixedObject<TS2NINT,TS2NFLOAT,TS2NDOUBLE> {
        
    public:
        
        EUDAQTempSensorBlock() {};
        
        /** Convenient c'tor.
         */
        EUDAQTempSensorBlock(int LDANumber, int PortNumber, int T1, int T2, int T3, int T4, int T5, int T6, int TDIF, int TPWR) {
            
            obj()->setIntVal(0, LDANumber);
            obj()->setIntVal(1, PortNumber);
            obj()->setIntVal(2, T1);
            obj()->setIntVal(3, T2);
            obj()->setIntVal(4, T3);
            obj()->setIntVal(5, T4);
            obj()->setIntVal(6, T5);
            obj()->setIntVal(7, T6);
            obj()->setIntVal(8, TDIF);
            obj()->setIntVal(9, TPWR);
        }
        
        /** 'Copy constructor' needed to interpret LCCollection read from file/database.
         */
        EUDAQTempSensorBlock(LCObject* obj) : LCFixedObject<TS2NINT,TS2NFLOAT,TS2NDOUBLE>(obj) { }
        
        /** Important for memory handling*/
        virtual ~EUDAQTempSensorBlock() { /* no op*/  }
        
        int GetLDANumber() const {
            return getIntVal(0);
        }
        
        int GetPortNumber() const {
            return getIntVal(1);
        }
        
        int GetT1() const {
            return getIntVal(2);
        }
        
        int GetT2() const {
            return getIntVal(3);
        }
        
        int GetT3() const {
            return getIntVal(4);
        }
        
        int GetT4() const {
            return getIntVal(5);
        }
        
        int GetT5() const {
            return getIntVal(6);
        }
        
        int GetT6() const {
            return getIntVal(7);
        }
        
        int GetTDIF() const {
            return getIntVal(8);
        }
        
        int GetTPWR() const {
            return getIntVal(9);
        }
        
        void print(  std::ostream& os, int ) ;
        
        // -------- need to implement abstract methods from LCGenericObject
        
        /** Return the type of the class
         */
        const std::string getTypeName() const {
            return"EUDAQTempSensorBlock" ;
        }
        
        /** Return a brief description of the data members
         */
        const std::string getDataDescription() const {
            return "i:LDA; i:port; i:T1; i:T2; i:T3; i:T4; i:T5; i:T6; i:TDIF; i:TPWR;" ;
        }
        
    };
    
}

#endif
//=============================================================================
