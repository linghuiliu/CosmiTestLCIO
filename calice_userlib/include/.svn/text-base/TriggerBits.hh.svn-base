#ifndef _TriggerBits_hh_
#define _TriggerBits_hh_

#include <iostream>

namespace CALICE {

/** A small class which allow for the easy analysis of core trigger 
  * information. It allows the user to test which trigger was set in 
  * a given event. The information has to be shipped as an event parameter.   
  * @author G. Gaycken LLR, (slight modifs by R. Pöschl DESY)
  * @date Dec 2 2005
  *
  */

class TriggerBits
{
public:

  enum ETriggerBitNr{kUnknownBit=0,
		     kBeam_ExtBit=kUnknownBit,
		     kBeamBit=1, 
		     kPedestalBit=2, 
		     kCosmicsBit=3, 
		     kCalibBit=4, 
		     kVetoBit=5,
		     kCherenkovBit=6,
		     kSC1_3x3Bit=7,
		     kSC2_3x3Bit=8,
		     kSC1_10x10Bit=9,
		     kSC2_10x10Bit=10,
		     kSC1_100x100Bit=11,
		     kSC2_100x100Bit=12,
		     kSpillBit=13,
		     kCrcOscillatorBit=14, 
		     kExternalPulserBit=15, 
		     kExternalLEDBit=16, 
		     kFingerHorizontBit=17, 
		     kFingerVertBit=18, 
		     kVetoULBit=19, 
		     kVetoDRBit=20, 
		     kVetoURBit=21, 
		     kVetoDLBit=22, 
		     kSC1_20x20Bit=23,
		     kVeto_20x20Bit=24,
		     kCherenkov2Bit=25,
		     kSC1_50x50Bit=26,
		     kSC2_50x50Bit=27,
		     kUTA1_19x19Bit=28,
		     kUTA2_19x19Bit=29,
		     kMuon_ExtBit=30,
		     kGenericBit=31
};

  enum ETriggerBits {//kUnknown=(1<<kUnknownBit),
                     kBeam_Ext=(1<<kBeam_ExtBit),
		     kBeam=(1<<kBeamBit), 
		     kPedestal=(1<<kPedestalBit),
		     kCosmics=(1<<kCosmicsBit),
		     kCalib=(1<<kCalibBit),
		     kVeto=(1<<kVetoBit),
		     kCherenkov=(1<<kCherenkovBit),
		     kSC1_3x3=(1<<kSC1_3x3Bit),
		     kSC2_3x3=(1<<kSC2_3x3Bit),
		     kSC1_10x10=(1<<kSC1_10x10Bit),
		     kSC2_10x10=(1<<kSC2_10x10Bit),
		     kSC1_100x100=(1<<kSC1_100x100Bit),
		     kSC2_100x100=(1<<kSC2_100x100Bit),
		     kSpill=(1<<kSpillBit),
		     kCrcOscillator=(1<<kCrcOscillatorBit),
		     kExternalPulser=(1<<kExternalPulserBit),
		     kExternalLED=(1<<kExternalLEDBit),
		     kFingerHorizont=(1<<kFingerHorizontBit),
		     kFingerVert=(1<<kFingerVertBit),
		     kVetoUL=(1<<kVetoULBit),
		     kVetoDR=(1<<kVetoDRBit),
		     kVetoUR=(1<<kVetoURBit),
		     kVetoDL=(1<<kVetoDLBit),
		     kSC1_20x20=(1<<kSC1_20x20Bit),
		     kVeto_20x20=(1<<kVeto_20x20Bit),
		     kCherenkov2=(1<<kCherenkov2Bit),
		     kSC1_50x50=(1<<kSC1_50x50Bit),
		     kSC2_50x50=(1<<kSC2_50x50Bit),
		     kUTA1_19x19=(1<<kUTA1_19x19Bit),
		     kUTA2_19x19=(1<<kUTA2_19x19Bit),
		     kMuon_Ext=(1<<kMuon_ExtBit),
		     kGeneric=(1<<kGenericBit)
  };

  /** 
   *  usage:
   *
   * TriggerBits trigger_event(((LCEvent *) evt)->getParameters().getIntVal(PAR_TRIGGER_EVENT));
   * if (trigger_event.isBeamTrigger()) {
   * }
   */

public:

  //TriggerBits() : _bits(kUnknown) {};
  TriggerBits() : _bits(0) {};
  TriggerBits(int trigger_bits) : _bits(trigger_bits) {};

  ~TriggerBits(){};

  bool isBeamTrigger() const {
    return _bits & kBeam;
  };

  bool isPureBeamTrigger() const {
    return isBeamTrigger() && !isPedestalTrigger();
  };

  void setBeamTrigger(){
    _bits |= kBeam;
  };

  bool isPedestalTrigger() const {
    return _bits & (kPedestal | kCrcOscillator | kExternalPulser);
  };

  bool isPurePedestalTrigger() const {
    return isPedestalTrigger() && (_bits & __nonPedestalTriggerMask)==0 ;
  }

  void setPedestalTrigger(){
    _bits |= kPedestal;
  };


  bool isCosmicsTrigger() const {
    return _bits & kCosmics;
  };

  bool isPureCosmicsTrigger() const {
    return (_bits & __mask) == kCosmics;
  };

  void setCosmicsTrigger(){
    _bits |= kCosmics;
  };

  bool isCalibTrigger() const {
    return _bits & kCalib;
  };

  bool isPureCalibTrigger() const {
    return ( _bits & kCalib && isPedestalTrigger()) && ( _bits & __nonPedestalTriggerMask & ~kCalib)==0 ;
  };

  void setCalibTrigger(){
    _bits |= kCalib;
  };
  
  bool isVetoTrigger() const {
    return _bits & kVeto;
  };

  void setVetoTrigger(){
    _bits |= kVeto;
  };

  bool isVetoUL() const {
    return _bits & kVetoUL;
  };

  void setVetoULTrigger(){
    _bits |= kVetoUL;
  };

  bool isVetoDR() const {
    return _bits & kVetoDR;
  };

  void setVetoDRTrigger(){
    _bits |= kVetoDR;
  };

  bool isVetoUR() const {
    return _bits & kVetoUR;
  };

  void setVetoURTrigger(){
    _bits |= kVetoUR;
  };

  bool isVetoDL() const {
    return _bits & kVetoDL;
  };

  void setVetoDLTrigger(){
    _bits |= kVetoDL;
  };


  bool isSc1_20x20() const {
    return _bits & kSC1_20x20;
  }

  void setSc1_20x20()  {
    _bits |= kSC1_20x20;
  }


  bool isUta1_19x19() const {
    return _bits & kUTA1_19x19;
  }

  void setUta1_19x19()  {
    _bits |= kUTA1_19x19;
  }

  bool isUta2_19x19() const {
    return _bits & kUTA2_19x19;
  }

  void setUta2_19x19()  {
    _bits |= kUTA2_19x19;
  }


  bool isMuon_Ext() const{
    return _bits & kMuon_Ext;
   }


  void setMuon_Ext() {
    _bits |= kMuon_Ext;
   }

  bool isBeam_Ext() const {
    return _bits & kBeam_Ext;
   }


  void setBeam_Ext() {
    _bits |= kBeam_Ext;
   }



  bool isVeto_20x20() const {
    return _bits & kVeto_20x20;
  }

  void setVeto_20x20()  {
    _bits |= kVeto_20x20;
  }


  bool isCherenkovTrigger() const {
    return _bits & kCherenkov;
  };

  void setCherenkovTrigger(){
    _bits |= kCherenkov;
  };

  bool isCherenkov2Trigger() const {
    return _bits & kCherenkov2;
  };

  void setCherenkov2Trigger(){
    _bits |= kCherenkov2;
  };


  bool isSc1_3x3() const {
    return _bits & kSC1_3x3;
  }

  void setSc1_3x3()  {
    _bits |= kSC1_3x3;
  }

  bool isSc2_3x3() const {
    return _bits & kSC2_3x3;
  }

  void setSc2_3x3()  {
    _bits |= kSC2_3x3;
  }

  bool isSc1_10x10() const {
    return _bits & kSC1_10x10;
  }

  void setSc1_10x10()  {
    _bits |= kSC1_10x10;
  }

  bool isSc2_10x10() const {
    return _bits & kSC2_10x10;
  }

  void setSc2_10x10()  {
    _bits |= kSC2_10x10;
  }

  //
  bool isSc1_50x50() const {
    return _bits & kSC1_50x50;
  }

  void setSc1_50x50()  {
    _bits |= kSC1_50x50;
  }

  bool isSc2_50x50() const {
    return _bits & kSC2_50x50;
  }

  void setSc2_50x50()  {
    _bits |= kSC2_50x50;
  }
  //

  bool isSc1_100x100() const {
    return _bits & kSC1_100x100;
  }

  void setSc1_100x100()  {
    _bits |= kSC1_100x100;
  }

  bool isSc2_100x100() const {
    return _bits & kSC2_100x100;
  }

  void setSc2_100x100()  {
    _bits |= kSC2_100x100;
  }

  bool isSpill() const {
    return _bits & kSpill;
  }

  void setSpill()  {
    _bits |= kSpill;
  }

  bool isGeneric() const {
    return _bits & kGeneric;
  }

  void setGeneric() {
    _bits |= kGeneric;
  };

  bool isCrcOscillator() const {
    return _bits & kCrcOscillator;
  }

  void setCrcOscillator() {
    _bits |= kCrcOscillator;
  };

  bool isExternalPulser() const {
    return _bits & kExternalPulser;
  }

  void setExternalPulser() {
    _bits |= kExternalPulser;
  };

  bool isExternalLED() const {
    return _bits & kExternalLED;
  }

  void setExternalLED() {
    _bits |= kExternalLED;
  };


  bool isFingerHorizont() const {
    return _bits & kFingerHorizont;
  }


  void setFingerHorizont() {
    _bits |= kFingerHorizont;
  };

  bool isFingerVert() const {
    return _bits & kFingerVert;
  }

  void setFingerVert() {
    _bits |= kFingerVert;
  };


  int getTriggerBits() const {
    return _bits;
  }

  void reset() {_bits = 0;}

  /** Overload the == operator for this class*/
  bool operator==( const TriggerBits &inBits ) const {
    if (_bits != inBits.getTriggerBits()) return false;
    return true;
  }

  /** Overload the != operator for this class*/
  bool operator!=( const TriggerBits &inBits ) const {
    return !(*this == inBits);
  }

  /** print the current errors.
   */
  std::ostream &print(std::ostream &out) const;

  /** Get the bit number assigned to the given name.
   * @param name the name of the trigger.
   * @return the bit number 0-31 assigned to the given trigger.
   */
  static unsigned int getBit(const char *name);

  /** Get the name assinged to the given trigger bit.
   * @param bit_i the bit_i number 0-31.
   * @return the name assinged to the bit
   * The parameter bit_i must be smaller than 32 otherwise the result is undefined 
   * and may result in a segmentation fault.
   */
  static const char *getName(unsigned int bit_i) {
#ifdef BOUNDARY_CHECK
    assert ( bit_i <32 );
#endif    
    return __names[bit_i];
  }

private:
  int _bits;
  static int __mask;
  static int __nonPedestalTriggerMask;
  static const char *__names[32];
};

  inline std::ostream &operator <<(std::ostream &os, const TriggerBits &bits) {
    return bits.print(os);
  }

}
#endif
