#ifndef EmcStageDataBlock_hh
#define EmcStageDataBlock_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"
#define POSSIGNMASK 0x8000
#define POSMASK 0x7fff
#define NUMXVALUES 3
#define NUMYVALUES 3

using namespace lcio ;
//using namespace CALICE;

namespace CALICE {
  
  /** The indices of all the stored values.
   */
  enum EEmcStIntValues {kEmcStHeader,
			kEmcStxStatus,
			kEmcStxValue,
			kEmcStyStatus=kEmcStxValue+NUMXVALUES,
			kEmcStyValue,
			kEmcStCheckSum=kEmcStyValue+NUMYVALUES
  };
  
  
  /** Stores information about the Emc stage 
   * Here we need to duplicate a lot of code already
   * written by Paul since we cannot top  
   * the 'intelligence' which is introduced already there.
   * For the time being we return only the (to my taste)
   * most important values. The others can be added easily
   * if really needed.
   * @sa ConditionsChangeDelegator
   * @author R. Poeschl LAL (based on the other interface classes) 
   * 
   * @date June  2006
   */
  
  class EmcStageDataBlock : public LCGenericObjectImplExt{

    
    
  public:
    
    /** Default Constructor */
    EmcStageDataBlock() { 
      //Do some initialization
      setHeader(0);  
      setXYStatus(0, 0);
      int xDummy[NUMXVALUES] = {0,0,0};
      int yDummy[NUMYVALUES] = {0,0,0};
      setXYValues(xDummy, yDummy);
      setCheckSum(0);
    }; 
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      EmcStageDataBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
    
    
    /** Set the header data */
    EmcStageDataBlock &setHeader(int header) {
      obj()->setIntVal(kEmcStHeader, header); 
      return *this;
    }    
    
    /** Set the x and yStatus data */
    EmcStageDataBlock &setXYStatus(int xStatus, int yStatus) {
      obj()->setIntVal(kEmcStxStatus, xStatus); 
      obj()->setIntVal(kEmcStyStatus, yStatus); 
      return *this;
      
    }    
    
    /** Set the x and y Value data */
    EmcStageDataBlock &setXYValues(int* xValue, int* yValue) {
      for (int ipos=0; ipos < NUMXVALUES; ipos++) {
	obj()->setIntVal(kEmcStxValue+ipos, *xValue); 
	xValue++;
      }
      
      for (int ipos=0; ipos < NUMYVALUES; ipos++) {
	obj()->setIntVal(kEmcStyValue+ipos, *yValue); 
	yValue++;
      }
      return *this;
    }    
    
    
    /** Set the checksum */
    EmcStageDataBlock &setCheckSum(int checksum) {
      obj()->setIntVal(kEmcStCheckSum, checksum); 
      return *this;
      
    }    
    
    /**Return the header*/
    int getHeader() {return getIntVal(kEmcStHeader);}
    
    
    /**Return the XIndexerStatus*/
    bool getXIndexerStatus(){ 
      if(getIntVal(kEmcStxStatus)&0x1) return true;
      else return false; 
    }
    
    /**Return the YIndexerStatus*/
    bool getYIndexerStatus(){ 
      if(getIntVal(kEmcStyStatus)&0x1) return true;
      else return false; 
    }
    
    
    /**Return the xStand Position*/
    int getXStandPosition() { return (getIntVal(kEmcStxValue+1))&0xFFFF;}
    /**Return the yStand Position*/
    int getYStandPosition() { return (getIntVal(kEmcStyValue+1))&0xFFFF;}
    
    
    /**Return the xBeam Position*/
    int getXBeamPosition() {
      int xValue = getIntVal(kEmcStxValue+2);
      bool sign = xValue&POSSIGNMASK;
      if(sign) return -(xValue&POSMASK);     
      else return (xValue&POSMASK);
    }
    
    /**Return the yBeam Position*/
    int getYBeamPosition() {
      int yValue = getIntVal(kEmcStyValue+2);
      bool sign = yValue&POSSIGNMASK;
      if(sign) return -(yValue&POSMASK);     
      else return (yValue&POSMASK);
    }
    
    
    /**Return the checksum*/
    int getCheckSum() {return getIntVal(kEmcStCheckSum);}
    
    
    
    /** Convenient print method */
    void print(  std::ostream& os); 

   // -------- need to implement abstract methods from LCGenericObject
    
    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"EmcStageDataBlock" ;
    } 

    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:header,i:xStatus,i[3]:xValues,i:yStatus,i[3]:yValues,i:checkSum";}
    
    

  };


}

#endif
