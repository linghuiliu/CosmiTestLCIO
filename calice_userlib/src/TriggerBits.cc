#include "TriggerBits.hh"
#include <iomanip>
#include <cstring>

namespace CALICE {

  int TriggerBits::__nonPedestalTriggerMask= ~(  TriggerBits::kPedestal 
					       | TriggerBits::kCrcOscillator 
					       | TriggerBits::kExternalPulser 
					       | TriggerBits::kSpill 
					       | TriggerBits::kGeneric 
                                               | TriggerBits::kExternalLED);

  int TriggerBits::__mask= ~(  TriggerBits::kSpill 
			     | TriggerBits::kGeneric);

    // ~(TriggerBits::kGeneric | TriggerBits::kSpill);


  const char *CALICE::TriggerBits::__names[32]={
    //"UNKNOWN",    // 0    
    "BEAM_Ext",    // 0    
    "BEAMDATA",   // 1
    "PEDESTAL",   // 2
    "COSMICS",    // 3
    "Calib",      // 4
    "Veto",       // 5
    "Cherenkov",  // 6
    "SC1_3x3",    // 7
    "SC2_3x3",    // 8
    "SC1_10x10",  // 9
    "SC2_10x10",  //10
    "SC1_100x100",//11
    "SC2_100x100",//12
    "Spill",      //13
    "CrcOscillator",     //14
    "ExternalPulser", //15
    "ExternalLED",           //16
    "HorizontalFinger",           //17
    "VerticalFinger",           //18
    "Veto_UL",           //19
    "Veto_DR",           //20
    "Veto_UR",           //21
    "Veto_DL",           //22
    "SC1_20x20",           //23
    "Veto_20x20",           //24
    "Cherenkov2",           //25
    "SC1_50x50",           //26
    "SC2_50x50",           //27
    "UTA1_19x19",           //28
    "UTA2_19x19",           //29
    "MUON_EXT",           //30
    "GENERICBIT"  //31
  };

  std::ostream &CALICE::TriggerBits::print(std::ostream &out) const
  {
    if (_bits) {
      out << "Triggers: ";
      if (isSpill())            out << __names[kSpillBit]         << "; ";
      if (isGeneric())          out << __names[kGenericBit]       << "; ";
      if (isPedestalTrigger())  { if (isPedestalTrigger() && (_bits & __nonPedestalTriggerMask)==0) {out << "pure ";} out << __names[kPedestalBit]      << "; ";}
      if (isCalibTrigger())     { if (isPureCalibTrigger())    {out << "pure ";} out << __names[kCalibBit]         << "; ";}
      if (isCrcOscillator())    out << __names[kCrcOscillatorBit] << "; ";
      if (isExternalPulser()) out << __names[kExternalPulserBit] << "; ";
      if (isBeamTrigger())      { if (isPureBeamTrigger())     {out << "pure ";} out << __names[kBeamBit]          << "; ";}
      if (isCosmicsTrigger())   { if (isPureCosmicsTrigger())  {out << "pure ";} out << __names[kCosmicsBit]       << "; ";}

      if (isSc1_3x3() && isSc2_3x3()) out << " coincidence 3x3; ";
      else {
	if (isSc1_3x3()) out << __names[kSC1_3x3Bit] << "; ";
	if (isSc2_3x3()) out << __names[kSC2_3x3Bit] << "; ";
      }

      if (isSc1_10x10() && isSc2_10x10()) out << " coincidence 10x10; ";
      else {
	if (isSc1_10x10()) out << __names[kSC1_10x10Bit] << "; ";
	if (isSc2_10x10()) out << __names[kSC2_10x10Bit] << "; ";
      }

      if (isSc1_100x100() && isSc2_100x100()) out << " coincidence 100x100; ";
      else {
	if (isSc1_100x100()) out << __names[kSC1_100x100Bit] << "; ";
	if (isSc2_100x100()) out << __names[kSC2_100x100Bit] << "; ";
      }


      if (isUta1_19x19() && isUta2_19x19()) out << " coincidence UTA 19x19; ";
      else {
	if (isUta1_19x19()) out << __names[kUTA1_19x19Bit] << "; ";
	if (isUta2_19x19()) out << __names[kUTA2_19x19Bit] << "; ";
      }

      if (isVetoTrigger())      out << __names[kVetoBit]          << "; ";
      if (isCherenkovTrigger()) out << __names[kCherenkovBit]     << "; ";
      if (isCherenkov2Trigger()) out << __names[kCherenkov2Bit]     << "; ";
      if (isExternalLED())      out << __names[kExternalLEDBit]     << "; ";      
      if (isVetoUL())      out << __names[kVetoULBit]     << "; ";      
      if (isVetoDR())      out << __names[kVetoDRBit]     << "; ";      
      if (isVetoUR())      out << __names[kVetoURBit]     << "; ";      
      if (isVetoDL())      out << __names[kVetoDLBit]     << "; ";      
      if (isSc1_20x20()) out << __names[kSC1_20x20Bit] << "; ";    
      if (isSc1_50x50()) out << __names[kSC1_50x50Bit] << "; ";    
      if (isSc2_50x50()) out << __names[kSC2_50x50Bit] << "; ";    
      if (isVeto_20x20()) out << __names[kVeto_20x20Bit] << "; "; 
      if (isMuon_Ext()) out << __names[kMuon_ExtBit] << "; "; }
    return out;
  }

  unsigned int TriggerBits::getBit(const char *name) {
    
    for (unsigned int bit_i=0;bit_i<32; bit_i++) {
      if (strcmp(name,__names[bit_i])==0) {
	return bit_i;
      }
    }
    return 32;
  }
    

}
