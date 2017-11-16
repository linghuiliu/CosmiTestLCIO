#ifndef EUDAQBlock2016_h
#define EUDAQBlock2016_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 77
#define NFLOAT 0
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

  class EUDAQBlock ;

  /**
   * Class for the SLCIO EUDAQ Data as acquired by the EUDAQ system.
   * @author A. Irles, based on the EUDAQBlock writen by A. Irles DESY Hamburg
   * @date January 12 2016
   * Created for 2016 testbeams EUDAQ data format.
   */
  class EUDAQBlock2016 : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
  public: 
  
    EUDAQBlock2016() {};

    /** Very ugly constructor because we use a Generic Object with many integers
     */
    EUDAQBlock2016(int CycleNr, int BunchXID, int EvtNr, int ChipID, int NChannels, 
	       int TDC_0, int TDC_1, int TDC_2, int TDC_3, int TDC_4, int TDC_5,  int TDC_6, int TDC_7, int TDC_8, int TDC_9, int TDC_10, int TDC_11, int TDC_12,int TDC_13, int TDC_14, int TDC_15, int TDC_16, int TDC_17,
	       int TDC_18, int TDC_19, int TDC_20, int TDC_21, int TDC_22, int TDC_23,  int TDC_24, int TDC_25, int TDC_26, int TDC_27, int TDC_28, int TDC_29, int TDC_30,int TDC_31, int TDC_32, int TDC_33, int TDC_34, int TDC_35,
	       int ADC_0, int ADC_1, int ADC_2, int ADC_3, int ADC_4, int ADC_5,  int ADC_6, int ADC_7, int ADC_8, int ADC_9, int ADC_10, int ADC_11, int ADC_12,int ADC_13, int ADC_14, int ADC_15, int ADC_16, int ADC_17,
	       int ADC_18, int ADC_19, int ADC_20, int ADC_21, int ADC_22, int ADC_23,  int ADC_24, int ADC_25, int ADC_26, int ADC_27, int ADC_28, int ADC_29, int ADC_30,int ADC_31, int ADC_32, int ADC_33, int ADC_34, int ADC_35)
    {

      obj()->setIntVal( 0 , CycleNr  ) ;
      obj()->setIntVal( 1 , BunchXID  ) ;
      obj()->setIntVal( 2 , EvtNr ) ;  
      obj()->setIntVal( 3 , ChipID  ) ;
      obj()->setIntVal( 4 , NChannels) ;  

      obj()->setIntVal( 5+0 , TDC_0 ) ;  
      obj()->setIntVal( 5+1 , TDC_1 ) ;  
      obj()->setIntVal( 5+2 , TDC_2 ) ;  
      obj()->setIntVal( 5+3 , TDC_3 ) ;  
      obj()->setIntVal( 5+4 , TDC_4 ) ;  
      obj()->setIntVal( 5+5 , TDC_5 ) ;  
      obj()->setIntVal( 5+6 , TDC_6 ) ;  
      obj()->setIntVal( 5+7 , TDC_7 ) ;  
      obj()->setIntVal( 5+8 , TDC_8 ) ;  
      obj()->setIntVal( 5+9 , TDC_9 ) ;  
      obj()->setIntVal( 5+10 , TDC_10 ) ;  
      obj()->setIntVal( 5+11 , TDC_11 ) ;  
      obj()->setIntVal( 5+12 , TDC_12 ) ;  
      obj()->setIntVal( 5+13 , TDC_13 ) ;  
      obj()->setIntVal( 5+14 , TDC_14 ) ;  
      obj()->setIntVal( 5+15 , TDC_15 ) ;  
      obj()->setIntVal( 5+16 , TDC_16 ) ;  
      obj()->setIntVal( 5+17 , TDC_17) ;  
      obj()->setIntVal( 5+18 , TDC_18 ) ;  
      obj()->setIntVal( 5+19 , TDC_19 ) ;  
      obj()->setIntVal( 5+20 , TDC_20 ) ;  
      obj()->setIntVal( 5+21 , TDC_21 ) ;  
      obj()->setIntVal( 5+22 , TDC_22 ) ;  
      obj()->setIntVal( 5+23 , TDC_23 ) ;  
      obj()->setIntVal( 5+24 , TDC_24 ) ;  
      obj()->setIntVal( 5+25 , TDC_25) ;  
      obj()->setIntVal( 5+26 , TDC_26) ;  
      obj()->setIntVal( 5+27 , TDC_27) ;  
      obj()->setIntVal( 5+28 , TDC_28) ;  
      obj()->setIntVal( 5+29 , TDC_29) ;  
      obj()->setIntVal( 5+30 , TDC_30) ;  
      obj()->setIntVal( 5+31 , TDC_31) ;  
      obj()->setIntVal( 5+32 , TDC_32) ;  
      obj()->setIntVal( 5+33 , TDC_33) ;  
      obj()->setIntVal( 5+34 , TDC_34) ;  
      obj()->setIntVal( 5+35 , TDC_35) ;  

      obj()->setIntVal( 5+36+0 , ADC_0 ) ;  
      obj()->setIntVal( 5+36+1 , ADC_1 ) ;  
      obj()->setIntVal( 5+36+2 , ADC_2 ) ;  
      obj()->setIntVal( 5+36+3 , ADC_3 ) ;  
      obj()->setIntVal( 5+36+4 , ADC_4 ) ;  
      obj()->setIntVal( 5+36+5 , ADC_5 ) ;  
      obj()->setIntVal( 5+36+6 , ADC_6 ) ;  
      obj()->setIntVal( 5+36+7 , ADC_7 ) ;  
      obj()->setIntVal( 5+36+8 , ADC_8 ) ;  
      obj()->setIntVal( 5+36+9 , ADC_9 ) ;  
      obj()->setIntVal( 5+36+10 , ADC_10 ) ;  
      obj()->setIntVal( 5+36+11 , ADC_11 ) ;  
      obj()->setIntVal( 5+36+12 , ADC_12 ) ;  
      obj()->setIntVal( 5+36+13 , ADC_13 ) ;  
      obj()->setIntVal( 5+36+14 , ADC_14 ) ;  
      obj()->setIntVal( 5+36+15 , ADC_15 ) ;  
      obj()->setIntVal( 5+36+16 , ADC_16 ) ;  
      obj()->setIntVal( 5+36+17 , ADC_17) ;  
      obj()->setIntVal( 5+36+18 , ADC_18 ) ;  
      obj()->setIntVal( 5+36+19 , ADC_19 ) ;  
      obj()->setIntVal( 5+36+20 , ADC_20 ) ;  
      obj()->setIntVal( 5+36+21 , ADC_21 ) ;  
      obj()->setIntVal( 5+36+22 , ADC_22 ) ;  
      obj()->setIntVal( 5+36+23 , ADC_23 ) ;  
      obj()->setIntVal( 5+36+24 , ADC_24 ) ;  
      obj()->setIntVal( 5+36+25 , ADC_25) ;  
      obj()->setIntVal( 5+36+26 , ADC_26) ;  
      obj()->setIntVal( 5+36+27 , ADC_27) ;  
      obj()->setIntVal( 5+36+28 , ADC_28) ;  
      obj()->setIntVal( 5+36+29 , ADC_29) ;  
      obj()->setIntVal( 5+36+30 , ADC_30) ;  
      obj()->setIntVal( 5+36+31 , ADC_31) ;  
      obj()->setIntVal( 5+36+32 , ADC_32) ;  
      obj()->setIntVal( 5+36+33 , ADC_33) ;  
      obj()->setIntVal( 5+36+34 , ADC_34) ;  
      obj()->setIntVal( 5+36+35 , ADC_35) ;  


    }

    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    EUDAQBlock2016(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

    /** Important for memory handling*/
    virtual ~EUDAQBlock2016() { /* no op*/  }
  
     /** get the CycleNr.
     */ 
    int GetCycleNr() const {
      return getIntVal(0);
    }

    /** get the BunchXID.
     */ 
    int GetBunchXID() const {
      return getIntVal(1);
    }

    /** get the EvtNr.
     */ 
    int GetEvtNr() const {
      return getIntVal(2);
    }

    /** get the ChipID.
     */ 
    int GetChipID() const {
      return getIntVal(3);
    }

    
    /** get the Channel.
     */ 
    int GetNChannels() const {
      return getIntVal(4);
    }

  
    /** get the TDC.
     */ 
    std::vector<int> GetTDC() const {

      std::vector<int> tdc;
      for(int i_tdc =5; i_tdc< 5+getIntVal(4); i_tdc++) 
	tdc.push_back(getIntVal(i_tdc));//%4096);

      return tdc;
    }

    // /** get the HitBit_tdc.
    //  */ 
    // std::vector<int> GetHitBit_tdc() const {

    //   std::vector<int> tdc;
    //   for(int i_tdc =4; i_tdc< 4+getIntVal(3); i_tdc++) 
    // 	tdc.push_back( (getIntVal(i_tdc) & 0x1000) /4096);

    //   return tdc;
    // }

    // /** get the GainBit_tdc.
    //  */ 
    // std::vector<int> GetGainBit_tdc() const {

    //   std::vector<int> tdc;
    //   for(int i_tdc =4; i_tdc< 4+getIntVal(3); i_tdc++) 
    // 	tdc.push_back( (getIntVal(i_tdc) & 0x2000) /8192);

    //   return tdc;
    // }

    /** get the ADC.
     */ 
    std::vector<int> GetADC() const {

      std::vector<int> adc;
      for(int i_adc =5+getIntVal(4); i_adc< 5+2*getIntVal(4); i_adc++) 
	adc.push_back(getIntVal(i_adc));//%4096);

      return adc;
    }

    // /** get the HitBit_adc.
    //  */ 
    // std::vector<int> GetHitBit_adc() const {

    //   std::vector<int> adc;
    //   for(int i_adc =4+getIntVal(3); i_adc< 4+2*getIntVal(3); i_adc++) 
    // 	adc.push_back( (getIntVal(i_adc) & 0x1000) /4096);

    //   return adc;
    // }

    // /** get the GainBit_adc.
    //  */ 
    // std::vector<int> GetGainBit_adc() const {

    //   std::vector<int> adc;
    //   for(int i_adc =4+getIntVal(3); i_adc< 4+2*getIntVal(3); i_adc++) 
    // 	adc.push_back( (getIntVal(i_adc) & 0x2000) /8192);

    //   return adc;
    // }

 


  
    /** Convenient print method 
     */
    void print(  std::ostream& os, int ) ;
  

    // -------- need to implement abstract methods from LCGenericObject

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"EUDAQBlock2016" ;
    } 
  
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:CycleNr:i:BunchXID; i:EvtNr;i:ChipID; i:NChannels; i:TDC_14bit[NChannels]; i:ADC_14bit[NChannels]" ;
    }

  }; // class
}

#endif 
//=============================================================================
