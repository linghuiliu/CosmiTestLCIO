#ifndef BeamParameter_hh
#define BeamParameter_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <sstream>


using namespace lcio ;

namespace CALICE {

/** 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */

  enum EBeamParameterFloatType {kBeamParameterFloatPeakEnergy,  /**< maximum of the beam energy distribution */
				    kBeamParameterFloatBeamAngleZX, /**< angle between beam axis and z-axis in the ZX-plane*/
				    kBeamParameterFloatBeamAngleZY, /**< angle between beam axis and z-axis in the ZY-plane*/
				    kBeamParameterFloatBeamImpactPositionX0, /**< mean beam impact position in XY-plane */
				    kBeamParameterFloatBeamImpactPositionY0, /**< mean beam impact position in XY-plane */
				    kNBeamParameterFloats};

  enum EBeamParameterIntType {kBeamParameterIntBeamType, /**< main particle type.*/
				  kNBeamParameterInts};

  /** Define the experimental setup: beam energy, position, angle and detector position, angle.
   * The experimental setup should be written into the conditions database at least twice:
   * <ul>
   *   <li> the nominal values with the tag NOMINAL
   *   <li> the measured values
   * </ul>
   * @todo{Should this class be split into two? One for the beam and one for the detector parameters?
   * Should the nominal and measured values be stored in separate folders?}
   */
class BeamParameter : public LCFixedObject<kNBeamParameterInts,kNBeamParameterFloats,0> {
public: 
  BeamParameter() : LCFixedObject<kNBeamParameterInts,kNBeamParameterFloats,0>() { };
    
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  BeamParameter(LCObject* obj) : LCFixedObject<kNBeamParameterInts,kNBeamParameterFloats,0>(obj) { } 
  
  enum EBeamType {kElectronBeam, kMuonBeam, kPionBeam, kProtonBeam, kCosmics, kUnknown, kMixed, kPionElectronBeam, kPionProtonBeam, kCalibrationPulse, kNoise, kNBeamTypes};
  /** Set the particle type.
   */
  BeamParameter &  setBeamType(EBeamType beam_type) 
  {
    obj()->setIntVal(kBeamParameterIntBeamType, (int) beam_type);
    return *this;
  };

  /** Get the particle type.
   */
  EBeamType getBeamType() const 
  {
    return static_cast<EBeamType>(getIntVal(kBeamParameterIntBeamType));
  };

  /** Return the beam type name for a given type id.
   */
  static const char *getBeamTypeName(unsigned int beam_type_i)  { return (beam_type_i<kNBeamTypes ? __beamTypeNames[beam_type_i] : __beamTypeNames[kNBeamTypes]);}


  /** Return the beam type name.
   */
  const char *getBeamTypeName()  const  { return getBeamTypeName(getBeamType());}


  /** Set the maximum of the beam energy distribution.
   */
  BeamParameter &  setPeakEnergy(float peak_energy) 
  {
    obj()->setFloatVal(kBeamParameterFloatPeakEnergy, peak_energy);
    return *this;
  };

  /** Get the maximum of the beam energy distribution.
   */
  float getPeakEnergy() const 
  {
    return getFloatVal(kBeamParameterFloatPeakEnergy);
  };

  /** Set the angle between the beam and the z-axis in thr ZX plane.
   */
  BeamParameter &  setBeamAngleZX(float beam_angle_zx) 
  {
    obj()->setFloatVal(kBeamParameterFloatBeamAngleZX, beam_angle_zx);
    return *this;
  };

  /** Get Set the angle between the beam and the z-axis in thr ZX plane.
   */
  float getBeamAngleZX() const 
  {
    return getFloatVal(kBeamParameterFloatBeamAngleZX);
  };

  /** Set the angle between the beam and the z-axis in thr ZY plane.
   */
  BeamParameter &  setBeamAngleZY(float beam_angle_zy) 
  {
    obj()->setFloatVal(kBeamParameterFloatBeamAngleZY, beam_angle_zy);
    return *this;
  };

  /** Get the angle between the beam and the z-axis in thr ZY plane.
   */
  float getBeamAngleZY() const 
  {
    return getFloatVal(kBeamParameterFloatBeamAngleZY);
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane (convenience method). 
   */
  BeamParameter &  setBeamImpactPosition(float beam_impact_x0, float beam_impact_y0) 
  {
    obj()->setFloatVal(kBeamParameterFloatBeamImpactPositionX0, beam_impact_x0);
    obj()->setFloatVal(kBeamParameterFloatBeamImpactPositionY0, beam_impact_y0);
    return *this;
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane (convenience method). 
   */
  BeamParameter &  setBeamImpactPosition(float *beam_impact_position_tupel) 
  {
    obj()->setFloatVal(kBeamParameterFloatBeamImpactPositionX0, beam_impact_position_tupel[0]);
    obj()->setFloatVal(kBeamParameterFloatBeamImpactPositionY0, beam_impact_position_tupel[0]);
    return *this;
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane. 
   */
  BeamParameter &  setBeamImpactPositionX0(float beam_impact_x0) 
  {
    obj()->setFloatVal(kBeamParameterFloatBeamImpactPositionX0, beam_impact_x0);
    return *this;
  };

  /** Get the mean impact position of the beam on the detector in the XY-plane.
   */
  float getBeamImpactPositionX0() const 
  {
    return getFloatVal(kBeamParameterFloatBeamImpactPositionX0);
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane. 
   */
  BeamParameter &  setBeamImpactPositionY0(float beam_impact_y0) 
  {
    obj()->setFloatVal(kBeamParameterFloatBeamImpactPositionY0, beam_impact_y0);
    return *this;
  };

  /** Get the y-coordinate of the mean impact position of the beam on the detector in the XY-plane.
   */
  float getBeamImpactPositionY0() const 
  {
    return getFloatVal(kBeamParameterFloatBeamImpactPositionY0);
  };


  /** Print all members (for debugging)
   */
  void print(std::ostream &os);
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("BeamParameter");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "i[" << kNBeamParameterInts << "]: beam  type"
	    << "; f[" << kNBeamParameterFloats << "]: beam(E,angle[2],pos[2]), detector(angle,rot_orig[2],pos[3])";
    return message.str();
  };

  static const char *__beamTypeNames[CALICE::BeamParameter::kNBeamTypes+1];

}; // class

}
#endif 
