#ifndef AhcConditions_h
#define AhcConditions_h 1

#ifdef HAVE_CONFIG_H
#include <config.h> 
#endif
 
#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define AHCCONDITIONSNINT 22
#define AHCCONDITIONSNFLOAT 0
#define AHCCONDITIONSNDOUBLE 0

using namespace lcio;

namespace CALICE {

class AhcConditions;

/**
  * Class for the CALICE Ahcal conditions information during the
  * reconstruction. 
  *
  * renamed from CaliceConditions  2007/12/14 B.Lutz
  *
  * Information is stored module wise
  * @author B. Lutz DESY
  * @date Sep 11 2006
  */
  
class AhcConditions: public LCFixedObject<AHCCONDITIONSNINT,AHCCONDITIONSNFLOAT,AHCCONDITIONSNDOUBLE> {

public: 

  /** default constructor
    */
  AhcConditions() {};
  
  /** constructor, AhcConditions is initialized with the parameters given
    * @param moduleNr mdoule "stamp"
    * @param moduleID module ID of the module the hit is in, low byte gives upper or 
    *   lower half, high byte gives module "stamp", used to find correct calibration
    * @param calibStart starting time of Tcalib signal in ticks
    * @param calibWidth duration of the Tcalib signal in ticks
    * @param calibEnable has Tcalib signal been sent at all?
    * @param hold starting time of the hold signal in ticks (6.25ns)
    * @param holdWidth duration of the hold signal in ticks (6.25ns)
    * @param multiplex number of multiplexed signals in the acquisition cycle
    * @param vcalib Vcalib value
    * @param verification shift register verification pattern
    * @param sr shift registers
    */
  AhcConditions(int moduleNr, unsigned moduleID, int calibStart, int calibWidth, bool calibEnable,
                   int hold, int holdWidth, int multiplex, int vcalib, int verification, int sr[12]) {
    
    obj()->setIntVal(0, moduleID);
    obj()->setIntVal(1, moduleNr);
    obj()->setIntVal(2, calibStart);
    obj()->setIntVal(3, calibWidth);
    if (calibEnable)  obj()->setIntVal(4, 0);
    else  obj()->setIntVal(4, 1);
    obj()->setIntVal(5, hold);
    obj()->setIntVal(6, holdWidth);
    obj()->setIntVal(7, multiplex);
    obj()->setIntVal(8, vcalib);
    obj()->setIntVal(9, verification);

    obj()->setIntVal(10, sr[0]);
    obj()->setIntVal(11, sr[1]);
    obj()->setIntVal(12, sr[2]);
    obj()->setIntVal(13, sr[3]);
    obj()->setIntVal(14, sr[4]);
    obj()->setIntVal(15, sr[5]);
    obj()->setIntVal(16, sr[6]);
    obj()->setIntVal(17, sr[7]);
    obj()->setIntVal(18, sr[8]);
    obj()->setIntVal(19, sr[9]);
    obj()->setIntVal(20, sr[10]);
    obj()->setIntVal(21, sr[11]);

  };
  
  AhcConditions(LCObject* obj): LCFixedObject<AHCCONDITIONSNINT,AHCCONDITIONSNFLOAT,AHCCONDITIONSNDOUBLE>(obj) {};

  virtual ~AhcConditions() {};
  
  /** get module ID of config
   */
  unsigned getModuleID() const {
    return (unsigned) (getIntVal(0));
  }; 
  
  /** get module number of config
   */
  unsigned getModuleNr() {
    return (unsigned) (getIntVal(1));
  };
  
  /** get calib start
   */
  unsigned getCalibStart() const {
    return (unsigned) (getIntVal(2));
  };
  
  /** get calib width
    */
  unsigned getCalibWidth() const {
    return (unsigned) (getIntVal(3));
  }  
  
  /** get if calib is enabled
   */
  bool isCalibEnabled() const {
    return (getIntVal(4)==0);
  };
  
  /** get hold
   */
  float getHold() const {
    return (unsigned) getIntVal(5);
  };
  
  /** get hold width
   */
  unsigned getHoldWidth() const {
    return (unsigned) getIntVal(6);
  };      
  
  /** get order of multiplexing
   */
  unsigned getMultiplex() const {
    return (unsigned) (getIntVal(7));
  };  

  /** get vcalib
   */
  unsigned getVcalib() const {
    return (unsigned) getIntVal(8);
  };      

  /** get verification data 
   */
  unsigned getVerification() const {
    return (unsigned) getIntVal(9);
  };      

  /** get hab sr
   */
  unsigned getSR(unsigned hab) const {
    return (unsigned) getIntVal(10+hab);
  };      


  /** convenient print method
   */
  void print (std::ostream& os);
  
  /** return the type of the class
   */
  const std::string getTypeName() const {
    return "AhcConditions";    
  }

  /** return a brief description of the data memeber
   */
  const std::string getDataDescription() const {
    return "i:ModuleID,i:moduleNr,i:calibStart,i:calibWidth,f:calibEnabled,i:hold,i:holdWidth,i:multiplex,i:vcalib,i:verification,i:srHab0,i:srHab1,i:srHab2,i:srHab3,i:srHab4,i:srHab5,i:srHab6,i:srHab7,i:srHab8,i:srHab9,i:srHab10,i:srHab11";
  } 

};   
}  

#endif
  
  
