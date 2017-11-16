#ifndef BeamParameterException_hh
#define BeamParameterException_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include <sstream>


using namespace lcio ;

namespace CALICE {


  //enum EBeamParameterExceptionFloatType {kBeamParameterExceptionFloatBeamEnergy,  /**< maximum of the beam energy distribution */
  //				    kNBeamParameterExceptionFloats};

  enum EBeamParameterExceptionIntType {kBeamParameterExceptionIntBeamEnergy, /**< main particle type.*/
  				  kNBeamParameterExceptionInts};

   
  /** This class handles beam parameters which have been written by hand into the database.
    * It is to be used if beam parameters are either not available at all or if the analysis of the r/o 
    * has revealed some odds.
    * This class might evolve with time, for the time being it constains only the beam energy.
    * @author R. Pöschl LAL 
    * @date Oct  2007
    */
class BeamParameterException : public LCGenericObjectImplExt{
public: 
  BeamParameterException(){};
    
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  BeamParameterException(LCObject* obj) :  LCGenericObjectImplExt(obj) { } 
  
  //  enum EBeamType {kElectronBeam, kMuonBeam, kPionBeam, kProtonBeam, kCosmics, kUnknown, kMixed, kPionElectronBeam, kPionProtonBeam, kCalibrationPulse, kNoise, kNBeamTypes};
  /** Set the particle type.
   */
  //Might be used later 
  //BeamParameterException &  setBeamType(EBeamType beam_type) 
  //{
  //  obj()->setIntVal(kBeamParameterExceptionIntBeamType, (int) beam_type);
  //  return *this;
  //};

  /** Get the particle type.
   */
  //EBeamType getBeamType() const 
  //{
  //  return static_cast<EBeamType>(getIntVal(kBeamParameterExceptionIntBeamType));
  //};

  /** Return the beam type name for a given type id.
   */
  //static const char *getBeamTypeName(unsigned int beam_type_i)  { return (beam_type_i<kNBeamTypes ? __beamTypeNames[beam_type_i] : __beamTypeNames[kNBeamTypes]);}


  /** Return the beam type name.
   */
  //const char *getBeamTypeName()  const  { return getBeamTypeName(getBeamType());}


  /** Set the nominal beam energy.
   */
  BeamParameterException &  setBeamEnergy(unsigned int energy) 
  {
    obj()->setIntVal(kBeamParameterExceptionIntBeamEnergy, static_cast<int>(energy));
    return *this;
  };

  /** Get the maximum of the beam energy distribution.
   */
  unsigned int getBeamEnergy() const 
  {
    return static_cast<unsigned int>(getIntVal(kBeamParameterExceptionIntBeamEnergy));
  };

  /** Print all members (for debugging)
   */
  void print(std::ostream &os);
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("BeamParameterException");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    //message << "i[" << kNBeamParameterExceptionInts << "]: beam  type"
    //	    << "; f[" << kNBeamParameterExceptionFloats << "]: beam(E)";
     message << "; i[" << kNBeamParameterExceptionInts << "]: beam(E)";
    return message.str();
  };

  //static const char *__beamTypeNames[CALICE::BeamParameterException::kNBeamTypes+1];

}; // class

}
#endif 
