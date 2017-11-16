#ifndef HodoscopeBlock_h
#define HodoscopeBlock_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NINT 69
#define NFLOAT 0
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

  class HodoscopeBlock ;

  /**
   * Class for the SLCIO hododscope as acquired by the EUDAQ system.
   * @author L. Liu, based on the EUDAQBlock writen by A. Irles DESY Hamburg
   * @date November 9 2017
   * Created for cosmic test hodoscope data format.
   */
  class HodoscopeBlock : public LCFixedObject<NINT,NFLOAT,NDOUBLE> {
  
  public: 
  
    HodoscopeBlock() {};

    /** Very ugly constructor because we use a Generic Object with many integers
     */
    HodoscopeBlock(int header, int size, int trigN, int cycleN, int tdc, 
	       int data_0, int data_1, int data_2, int data_3, int data_4, int data_5,  int data_6, int data_7, int data_8, int data_9, int data_10, int data_11, int data_12,int data_13, int data_14, int data_15,
	       int data_16, int data_17, int data_18, int data_19, int data_20, int data_21,  int data_22, int data_23, int data_24, int data_25, int data_26, int data_27, int data_28,int data_29, int data_30, int data_31,
	       int data_32, int data_33, int data_34, int data_35, int data_36, int data_37,  int data_38, int data_39, int data_40, int data_41, int data_42, int data_43, int data_44,int data_45, int data_46, int data_47,
	       int data_48, int data_49, int data_50, int data_51, int data_52, int data_53,  int data_54, int data_55, int data_56, int data_57, int data_58, int data_59, int data_60,int data_61, int data_62, int data_63)
    {

      obj()->setIntVal( 0 , header ) ;
      obj()->setIntVal( 1 , size   ) ;
      obj()->setIntVal( 2 , trigN  ) ;  
      obj()->setIntVal( 3 , cycleN ) ;
      obj()->setIntVal( 4 , tdc    ) ;  

      obj()->setIntVal( 5+0 , data_0 ) ;  
      obj()->setIntVal( 5+1 , data_1 ) ;  
      obj()->setIntVal( 5+2 , data_2 ) ;  
      obj()->setIntVal( 5+3 , data_3 ) ;  
      obj()->setIntVal( 5+4 , data_4 ) ;  
      obj()->setIntVal( 5+5 , data_5 ) ;  
      obj()->setIntVal( 5+6 , data_6 ) ;  
      obj()->setIntVal( 5+7 , data_7 ) ;  
      obj()->setIntVal( 5+8 , data_8 ) ;  
      obj()->setIntVal( 5+9 , data_9 ) ;  
      obj()->setIntVal( 5+10 , data_10 ) ;  
      obj()->setIntVal( 5+11 , data_11 ) ;  
      obj()->setIntVal( 5+12 , data_12 ) ;  
      obj()->setIntVal( 5+13 , data_13 ) ;  
      obj()->setIntVal( 5+14 , data_14 ) ;  
      obj()->setIntVal( 5+15 , data_15 ) ;  
      obj()->setIntVal( 5+16 , data_16 ) ;  
      obj()->setIntVal( 5+17 , data_17 ) ;  
      obj()->setIntVal( 5+18 , data_18 ) ;  
      obj()->setIntVal( 5+19 , data_19 ) ;  
      obj()->setIntVal( 5+20 , data_20 ) ;  
      obj()->setIntVal( 5+21 , data_21 ) ;  
      obj()->setIntVal( 5+22 , data_22 ) ;  
      obj()->setIntVal( 5+23 , data_23 ) ;  
      obj()->setIntVal( 5+24 , data_24 ) ;  
      obj()->setIntVal( 5+25 , data_25 ) ;  
      obj()->setIntVal( 5+26 , data_26 ) ;  
      obj()->setIntVal( 5+27 , data_27 ) ;  
      obj()->setIntVal( 5+28 , data_28 ) ;  
      obj()->setIntVal( 5+29 , data_29 ) ;  
      obj()->setIntVal( 5+30 , data_30 ) ;  
      obj()->setIntVal( 5+31 , data_31 ) ;  
      obj()->setIntVal( 5+32 , data_32 ) ;  
      obj()->setIntVal( 5+33 , data_33 ) ;  
      obj()->setIntVal( 5+34 , data_34 ) ;  
      obj()->setIntVal( 5+35 , data_35 ) ;  
      obj()->setIntVal( 5+36 , data_36 ) ;  
      obj()->setIntVal( 5+37 , data_37 ) ;  
      obj()->setIntVal( 5+38 , data_38 ) ;  
      obj()->setIntVal( 5+39 , data_39 ) ;  
      obj()->setIntVal( 5+40 , data_40 ) ;  
      obj()->setIntVal( 5+41 , data_41 ) ;  
      obj()->setIntVal( 5+42 , data_42 ) ;  
      obj()->setIntVal( 5+43 , data_43 ) ;  
      obj()->setIntVal( 5+44 , data_44 ) ;  
      obj()->setIntVal( 5+45 , data_45 ) ;  
      obj()->setIntVal( 5+46 , data_46 ) ;  
      obj()->setIntVal( 5+47 , data_47 ) ;  
      obj()->setIntVal( 5+48 , data_48 ) ;  
      obj()->setIntVal( 5+49 , data_49 ) ;  
      obj()->setIntVal( 5+50 , data_50 ) ;  
      obj()->setIntVal( 5+51 , data_51 ) ;  
      obj()->setIntVal( 5+52 , data_52 ) ;  
      obj()->setIntVal( 5+53 , data_53 ) ;  
      obj()->setIntVal( 5+54 , data_54 ) ;  
      obj()->setIntVal( 5+55 , data_55 ) ;  
      obj()->setIntVal( 5+56 , data_56 ) ;  
      obj()->setIntVal( 5+57 , data_57 ) ;  
      obj()->setIntVal( 5+58 , data_58 ) ;  
      obj()->setIntVal( 5+59 , data_59 ) ;  
      obj()->setIntVal( 5+60 , data_60 ) ;  
      obj()->setIntVal( 5+61 , data_61 ) ;  
      obj()->setIntVal( 5+62 , data_62 ) ;  
      obj()->setIntVal( 5+63 , data_63 ) ;  


    }

    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
    HodoscopeBlock(LCObject* obj) : LCFixedObject<NINT,NFLOAT,NDOUBLE>(obj) { } 

    /** Important for memory handling*/
    virtual ~HodoscopeBlock() { /* no op*/  }
  
     /** get the header (0xffff7368 = -35992).
     */ 
    int GetHeader() const {
      return getIntVal(0);
    }

    /** get the size.
     */ 
    int GetSize() const {
      return getIntVal(1) & 0xFFF;
    }

    /** get the trigger number (up to 4095).
     */ 
    int GetTrigN() const {
      return (getIntVal(2)>>16) & 0xFFF;
    }

    /** get the cycle number.
     */ 
    int GetCycleN() const {
      return getIntVal(3) & 0xFFFFFF;
    }

    /** get the tdc.
     */ 
    int GetTDC() const {
      return getIntVal(4) & 0xFFFFFF;
    }
    
    /** get the accept.
     */ 
    int GetAccept() const {
      return (getIntVal(4)>>31)&0x1;
    }

    /** get the ADC.
     */ 
    std::vector<int> GetADC() const {

      std::vector<int> adc;
      for(int i_data=5;i_data<5+64;i_data++) 
	adc.push_back(getIntVal(i_data)&0xFFF);

      return adc;
    }

    /** get the channel.
     */ 
    std::vector<int> GetChannel() const {

      std::vector<int> channel;
      for(int i_data=5;i_data<5+64;i_data++) {
        int ASICid = (getIntVal(i_data)>>24)&0xFF;
        int chn = (getIntVal(i_data)>>16)&0x1F;
        if (ASICid==81) {
        } else if (ASICid==60) {
          chn += 32;
        } else {
          chn = 65;
        }
	channel.push_back(chn);
      }
      return channel;
    }
  
    /** Convenient print method 
     */
    void print(  std::ostream& os, int ) ;
  

    // -------- need to implement abstract methods from LCGenericObject

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return "hodoscopeBlock" ;
    } 
  
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:header,i:size,i:trigN,i:cycleN,i:tdc,i:data[64]";
    }

  }; // class
}

#endif 
//=============================================================================
