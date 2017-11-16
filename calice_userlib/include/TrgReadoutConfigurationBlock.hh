#ifndef TrgReadoutConfigurationBlock_hh
#define TrgReadoutConfigurationBlock_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

  /** The indices of all the stored values.
   */
enum ETrgConfIntValues {kTrgConfIntNumbers,
                            kTrgConfIntEnable,
                            kTrgConfIntclearBeTrg,
                            kTrgConfIntbeTrgSoftTrg,
                            kTrgConfIntbeTrgSoftSpill,
                            kTrgConfIntbeTrgSquirt,
                            kTrgConfIntbeTrgVlink,
                            kTrgConfIntspillInvert,
                            kTrgConfIntreadPeriod,
                            kTrgConfIntreadcPeriod,
                            kTrgConfIntbeTrgPollNumber,
			kTrgConfIntbeTrgPollTimesec, kTrgConfIntbeTrgPollTimemus,
                            kTrgConfIntbeTrgSpillNumber,
			kTrgConfIntbeTrgSpillTimesec, kTrgConfIntbeTrgSpillTimemus, 
                            kNTrgConfIntValues=kTrgConfIntbeTrgSpillTimemus};


 /** Stores the general readout configuration.
   * e.g. to decide whether  a softtrigger was -generally- enabled
   * in contrast to the CrcReadoutConfiguration where the condig
   * per board is given
   * In addition information about general hardware
   * configuration and run-conditions related items can be retrieved.  
   * 
   * @sa ConditionsChangeDelegator
   * @author R. Poeschl LAL (based on the other interface classes) 
   * 
   * @date Jul  2006
   */

class TrgReadoutConfigurationBlock : public LCGenericObjectImplExt{


public:

  /** Default Constructor */
      TrgReadoutConfigurationBlock() {/*no op*/}; 

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      TrgReadoutConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 


     /** Set the enable of TrgReadConfiguration, i.e 
       * i.e. to decide whether the parameters here were active (?)
       */ 
    TrgReadoutConfigurationBlock &setEnable(int enable) {
     obj()->setIntVal(kTrgConfIntEnable, enable); 
     return *this;

    }    
 
    /** Get the enable value*/
    int getEnable() const {
       return getIntVal(kTrgConfIntEnable);
    } 


    /** Set the clear BeTrig indicator 
      * Note this a some of the following values are only
      * bools so the setting of an integer is a wast of memory
      * but anyway */
    TrgReadoutConfigurationBlock &setClearBeTrg(int clearbetrg) {
     obj()->setIntVal(kTrgConfIntclearBeTrg, clearbetrg); 
     return *this;
      
    }    
 
    /** Get the clear BeTrg indicator */
    int getClearBeTrg() const {
       return getIntVal(kTrgConfIntclearBeTrg);

    } 

    /** Set the beTrgSoftTrg indicator */
    TrgReadoutConfigurationBlock &setBeTrgSoftTrg(int betrgsofttrg) {
       obj()->setIntVal(kTrgConfIntbeTrgSoftTrg, betrgsofttrg); 
      return *this;

    }    
 
    /** Get the beTrgSoftTrg indicator  */
    int getBeTrgSoftTrg() const {
        return getIntVal(kTrgConfIntbeTrgSoftTrg);
    } 


    /** Set the beTrg soft spill indicator */
    TrgReadoutConfigurationBlock &setBeTrgSoftSpill(int betrgsoftspl) {
     obj()->setIntVal(kTrgConfIntbeTrgSoftSpill, betrgsoftspl); 
     return *this;


    }    
 
    /** Get the betrg soft spill indicator */

    int getBeTrgSoftSpill() const {
       return getIntVal(kTrgConfIntbeTrgSoftSpill);
    } 

    /** Set the beTrg squirt indicator */
    TrgReadoutConfigurationBlock &setBeTrgSquirt(int betrgsquirt) {
     obj()->setIntVal(kTrgConfIntbeTrgSquirt, betrgsquirt); 
     return *this;
    }    
 
    /** Get the beTrg squirt indicator */
    int getBeTrgSquirt() const {
       return getIntVal(kTrgConfIntbeTrgSquirt);

    } 

    /** Set the beTrg Vlink indicator */
    TrgReadoutConfigurationBlock &setBeTrgVlink(int betrgvlink) {
     obj()->setIntVal(kTrgConfIntbeTrgVlink, betrgvlink); 
     return *this;
    }    
 
    /** Get the beTrg Vlink indicator */
    int getBeTrgVlink() const {
       return getIntVal(kTrgConfIntbeTrgVlink);

    } 

    /** Set the spill Invert indicator */
    TrgReadoutConfigurationBlock &setSpillInvert(int spillinvert) {
     obj()->setIntVal(kTrgConfIntspillInvert, spillinvert); 
     return *this;
    }    
 
    /** Get the spill invert indicator */
    int getSpillInvert() const {
       return getIntVal(kTrgConfIntspillInvert);
    } 

    /** Set the read period */
    TrgReadoutConfigurationBlock &setReadPeriod(int readperiod) {
     obj()->setIntVal(kTrgConfIntreadPeriod, readperiod); 
     return *this;
    }    
 
    /** Get the read period*/
    int getReadPeriod() const {
       return getIntVal(kTrgConfIntreadPeriod);
    } 


    /** Set the readc period*/
    TrgReadoutConfigurationBlock &setReadCounterPeriod(int readcperiod) {
     obj()->setIntVal(kTrgConfIntreadcPeriod, readcperiod); 
     return *this;
    }    
 
    /** Get the readc period*/
    int getReadCounterPeriod() const {
       return getIntVal(kTrgConfIntreadcPeriod);
    } 

 
    /** Set the BeTrg Pollnumber */
    TrgReadoutConfigurationBlock &setBeTrgPollNumber(int betrgpollnum) {
     obj()->setIntVal(kTrgConfIntbeTrgPollNumber, betrgpollnum); 
     return *this;
    }    
 
    /** Get the BeTrg Poll Number*/
    int getBeTrgPollNumber() const {
       return getIntVal( kTrgConfIntbeTrgPollNumber);
    } 


    /** Set the BetrgPoll time */
    TrgReadoutConfigurationBlock &setBeTrgPollTime(int
    betrgpolltimesec, int betrgpolltimemus) {

     //Set secs and musecs seperately
     obj()->setIntVal(kTrgConfIntbeTrgPollTimesec, betrgpolltimesec); 
     obj()->setIntVal(kTrgConfIntbeTrgPollTimemus, betrgpolltimemus); 
     return *this;
    }    
 
    /** Get the BeTrgPoll time Sec.Part*/
    int getBeTrgPollTimeSec() const {
       return getIntVal(kTrgConfIntbeTrgPollTimesec);
    } 

    /** Get the BeTrgPoll time mus.Part*/
    int getBeTrgPollTimeMus() const {
       return getIntVal(kTrgConfIntbeTrgPollTimemus);
    } 

    /** Set the BeTrgSpill number */
    TrgReadoutConfigurationBlock &setBeTrgSpillNumber(int betrgspillnum) {
     obj()->setIntVal(kTrgConfIntbeTrgSpillNumber, betrgspillnum); 
     return *this;
    }    
 
    /** Get the BeTrgSpill number*/
    int getBeTrgSpillNumber() const {
       return getIntVal(kTrgConfIntbeTrgSpillNumber);
    } 

    /** Set the BeTrgSpill time */
    TrgReadoutConfigurationBlock &setBeTrgSpillTime(int
    betrgspilltimesec, int betrgspilltimemus) {

     //Set secs and musecs seperately
     obj()->setIntVal(kTrgConfIntbeTrgSpillTimesec, betrgspilltimesec); 
     obj()->setIntVal(kTrgConfIntbeTrgSpillTimemus, betrgspilltimemus); 
     return *this;
    }    
 
    /** Get the BeTrgSpill time Sec. Part*/
    int getBeTrgSpillTimeSec() const {
       return getIntVal(kTrgConfIntbeTrgSpillTimesec);
    } 

    /** Get the BeTrgSpill time Mus. Part*/
    int getBeTrgSpillTimeMus() const {
       return getIntVal(kTrgConfIntbeTrgSpillTimesec);
    } 

    /** Convenient print method 
     */
  /**dumps data content 
    */
   std::ostream& print(std::ostream &ostrm)  
   {
      
     ostrm << " TrgReadoutConfigurationData: "  << std::endl;
     ostrm << " is Enable: " << getEnable() << std::endl;
     ostrm << " ClearBeTrg: " << getClearBeTrg() << std::endl;
     ostrm << " BeTrgSoftTrg: " << getBeTrgSoftTrg() << std::endl; 
     ostrm << " BeTrgSoftSpill: " << getBeTrgSoftSpill() << std::endl;
     ostrm << " BeTrgSquirt: " << getBeTrgSquirt() << std::endl;
     ostrm << " BeTrgVlink: " << getBeTrgVlink() << std::endl;
     ostrm << " SpillInvert: " << getSpillInvert() << std::endl;
     ostrm << " ReadPeriod: " << getReadPeriod() << std::endl;
     ostrm << " ReadCPeriod: " << getReadCounterPeriod() << std::endl;
     ostrm << " BeTrgPollNumber: " << getBeTrgPollNumber() << std::endl;
     ostrm << " BeTrgPollTime: " << getBeTrgPollTimeSec() << "." << getBeTrgPollTimeMus()<< std::endl; 
     ostrm << " BeTrgPollNumber: " << getBeTrgSpillNumber() << std::endl;
     ostrm << " BeTrgSpillTime: " << getBeTrgSpillTimeSec() << "." << getBeTrgSpillTimeMus()<< std::endl; 
     return ostrm;
   }
//............................................................................      





   // -------- need to implement abstract methods from LCGenericObject

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return"TrgReadoutConfigurationBlock" ;
    } 

     /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      return "i:clearbetrg, i:betrgsoft, i:betrgsoftspill, i:betrgsquirt, i:betrgvlink, i:spillinvert, i:readperiod, i:readcperiod, i:betrgpollnumber, i:betrgpolltime, i:betrgpollnumber, betrgspillnumber, betrgspilltime" ;}



 };


}

#endif
