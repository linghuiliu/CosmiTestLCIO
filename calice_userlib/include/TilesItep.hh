#ifndef TilesItep_h
#define TilesItep_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NTIINT 2
#define NTIFLO 13
#define NTIDBL 0


using namespace lcio ;
using namespace std;


namespace CALICE {
class TilesItep ;

/** Parameters of the sipm delivered by ITEP
 */
class TilesItep : public LCFixedObject<NTIINT,NTIFLO,NTIDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  TilesItep(int TileSize, int SIPM, float Voltage, float VoltageBreakdown, float Delta_SPE, 
            float Phe_MIP , float Temp, float Current, float CurrentRMS, float DarkRate0, 
            float DarkRateHalf, float DarkRateHalfCorr, float PedRMS, float PeakWidth, float XTalk) {

    //Set the integer values
    obj()->setIntVal( 0 , TileSize  ) ;
    obj()->setIntVal( 1 , SIPM  ) ;
    //Set the floating values
    obj()->setFloatVal( 0, Voltage ) ; 
    obj()->setFloatVal( 1, VoltageBreakdown);
    obj()->setFloatVal( 2, Delta_SPE ) ; 
    obj()->setFloatVal( 3, Phe_MIP ) ; 
    obj()->setFloatVal( 4, Temp ) ;
    obj()->setFloatVal( 5, Current );
    obj()->setFloatVal( 6, CurrentRMS );
    obj()->setFloatVal( 7, DarkRate0 );
    obj()->setFloatVal( 8, DarkRateHalf );
    obj()->setFloatVal( 9, DarkRateHalfCorr ); 
    obj()->setFloatVal( 10, PedRMS ); 
    obj()->setFloatVal( 11, PeakWidth ); 
    obj()->setFloatVal( 12, XTalk );
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  TilesItep(LCObject* obj) : LCFixedObject<NTIINT,NTIFLO,NTIDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~TilesItep	() { /* no op*/  }

  /** the class interface:
    */
  int getTileSize() { return obj()->getIntVal(0); } 
  int getSIPMID() { return obj()->getIntVal(1); } 
  
  float getVoltage() { return obj()->getFloatVal(0); }
  float getVoltageBreakdown() { return obj()->getFloatVal(1); }
  float getDelta_SPE() { return obj()->getFloatVal(2); }
  float getPhe_MIP() { return obj()->getFloatVal(3); }
  float getTemp() { return obj()->getFloatVal(4); }
  float getCurrent() { return obj()->getFloatVal(5); }  
  float getCurrentRMS() { return obj()->getFloatVal(6); }  
  float getDarkRate0() { return obj()->getFloatVal(7); }  
  float getDarkRateHalf() { return obj()->getFloatVal(8); }  
  float getDarkRateHalfCorr() { return obj()->getFloatVal(9); }  
  float getPedRMS() { return obj()->getFloatVal(10); }  
  float getPeakWidth() { return obj()->getFloatVal(11); }  
  float getXTalk() { return obj()->getFloatVal(12); }  


  /* ITEP FILES
  curmean.dat - sipm current (uA)
  currms.dat - rms of multiple measurement of sipm current
  ddelta.dat - sipm gain (adc channels)
  ==> NO eeff.dat - number of p.e. for low level light
  hhv.dat - chosen hv (V)
  hvthr.dat - breakdown point (V)
  nnoise.dat - noise at "0" level (kHz)
  nnoiset.dat - noise at 1/2 mip level (Hz)
  nnoiset_corr.dat - noise at 1/2 mip level corrected to temp=22 C (Hz)
  pedrms.dat - pedestal rms (adc channels)
  ==> NO pixs.dat - number of pixels for saturation curve measurements
  ==> NO pmts.dat - amount of light for corresponding pixs.dat (mip)
  ==> NO ppix.dat - number of pixels at low light
  ppixm.dat - number of pixels per mip at low light
  ssigma.dat - single pe peak width in interpixel distance units
  temp1.dat - temperature during test
  xxt.dat - sipm cross talk
  */

  /** convenient print function
   */ 
  void print(std::ostream& os);
  
  // -------- need to implement abstract methods from LCGenericObject
  const std::string getTypeName() const { 
    return "SiPM_MappingHcal";
  } 
  
  const std::string getDataDescription() const {
    return "i:TileSize,i:SiPM,f:Voltage,f:VoltageBreakdown,f:Delta_SPE,f:Phe/Mip,f:Temperature,f:CurrentRMS,f:DarkRate0,f:DarkRateHalf,f:DarkRateHalfCorr,f:PedRMS,f:PeakWidth,f:XTalk"; 
  }

}; // class

}
#endif 
