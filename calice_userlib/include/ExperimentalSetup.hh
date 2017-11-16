#ifndef ExperimentalSetup_hh
#define ExperimentalSetup_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <sstream>


using namespace lcio ;

namespace CALICE {

/** 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */

  enum EExperimentalSetupFloatType {kExperimentalSetupFloatPeakEnergy,  /**< maximum of the beam energy distribution */
				    kExperimentalSetupFloatBeamAngleZX, /**< angle between beam axis and z-axis in the ZX-plane*/
				    kExperimentalSetupFloatBeamAngleZY, /**< angle between beam axis and z-axis in the ZY-plane*/
				    kExperimentalSetupFloatBeamImpactPositionX0, /**< mean beam impact position in XY-plane */
				    kExperimentalSetupFloatBeamImpactPositionY0, /**< mean beam impact position in XY-plane */
				    kExperimentalSetupFloatDetectorAngleZX, /**< angle between the detector and z-axis in the ZX-plane*/
				    kExperimentalSetupFloatDetectorRotationX0, /**< origin of the rotation of the detector in  ZX-plane*/
				    kExperimentalSetupFloatDetectorRotationZ0, /**< origin of the rotation of the detector in  ZX-plane*/
				    kExperimentalSetupFloatDetectorX0, /**< origin of the detector x-coordinate*/
				    kExperimentalSetupFloatDetectorY0, /**< origin of the detector y-coordinate*/
				    kExperimentalSetupFloatDetectorZ0, /**< origin of the detector z-coordinate*/
				    kNExperimentalSetupFloats};

  enum EExperimentalSetupIntType {kExperimentalSetupIntBeamType, /**< main particle type.*/
				  kNExperimentalSetupInts};

  /** Define the experimental setup: beam energy, position, angle and detector position, angle.
   * The experimental setup should be written into the conditions database at least twice:
   * <ul>
   *   <li> the nominal values with the tag NOMINAL
   *   <li> the measured values
   * </ul>
   * @todo{Should this class be split into two? One for the beam and one for the detector parameters?
   * Should the nominal and measured values be stored in separate folders?}
   */
class ExperimentalSetup : public LCFixedObject<kNExperimentalSetupInts,kNExperimentalSetupFloats,0> {
public: 
  ExperimentalSetup() : LCFixedObject<kNExperimentalSetupInts,kNExperimentalSetupFloats,0>() { };
    
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  ExperimentalSetup(LCObject* obj) : LCFixedObject<kNExperimentalSetupInts,kNExperimentalSetupFloats,0>(obj) { } 
  
  enum EBeamType {kElectronBeam, kMuonBeam, kPionBeam, kProtonBeam, kCosmics, kUnknown, kMixed, kPionElectronBeam, kPionProtonBeam, kCalibrationPulse, kNoise, kNBeamTypes};
  /** Set the particle type.
   */
  ExperimentalSetup &  setBeamType(EBeamType beam_type) 
  {
    obj()->setIntVal(kExperimentalSetupIntBeamType, (int) beam_type);
    return *this;
  };

  /** Get the particle type.
   */
  EBeamType getBeamType() const 
  {
    return static_cast<EBeamType>(getIntVal(kExperimentalSetupIntBeamType));
  };

  /** Return the beam type name for a given type id.
   */
  static const char *getBeamTypeName(unsigned int beam_type_i)  { return (beam_type_i<kNBeamTypes ? __beamTypeNames[beam_type_i] : __beamTypeNames[kNBeamTypes]);}


  /** Return the beam type name.
   */
  const char *getBeamTypeName()  const  { return getBeamTypeName(getBeamType());}


  /** Set the maximum of the beam energy distribution.
   */
  ExperimentalSetup &  setPeakEnergy(float peak_energy) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatPeakEnergy, peak_energy);
    return *this;
  };

  /** Get the maximum of the beam energy distribution.
   */
  float getPeakEnergy() const 
  {
    return getFloatVal(kExperimentalSetupFloatPeakEnergy);
  };

  /** Set the angle between the beam and the z-axis in thr ZX plane.
   */
  ExperimentalSetup &  setBeamAngleZX(float beam_angle_zx) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatBeamAngleZX, beam_angle_zx);
    return *this;
  };

  /** Get Set the angle between the beam and the z-axis in thr ZX plane.
   */
  float getBeamAngleZX() const 
  {
    return getFloatVal(kExperimentalSetupFloatBeamAngleZX);
  };

  /** Set the angle between the beam and the z-axis in thr ZY plane.
   */
  ExperimentalSetup &  setBeamAngleZY(float beam_angle_zy) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatBeamAngleZY, beam_angle_zy);
    return *this;
  };

  /** Get the angle between the beam and the z-axis in thr ZY plane.
   */
  float getBeamAngleZY() const 
  {
    return getFloatVal(kExperimentalSetupFloatBeamAngleZY);
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane (convenience method). 
   */
  ExperimentalSetup &  setBeamImpactPosition(float beam_impact_x0, float beam_impact_y0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatBeamImpactPositionX0, beam_impact_x0);
    obj()->setFloatVal(kExperimentalSetupFloatBeamImpactPositionY0, beam_impact_y0);
    return *this;
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane (convenience method). 
   */
  ExperimentalSetup &  setBeamImpactPosition(float *beam_impact_position_tupel) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatBeamImpactPositionX0, beam_impact_position_tupel[0]);
    obj()->setFloatVal(kExperimentalSetupFloatBeamImpactPositionY0, beam_impact_position_tupel[0]);
    return *this;
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane. 
   */
  ExperimentalSetup &  setBeamImpactPositionX0(float beam_impact_x0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatBeamImpactPositionX0, beam_impact_x0);
    return *this;
  };

  /** Get the mean impact position of the beam on the detector in the XY-plane.
   */
  float getBeamImpactPositionX0() const 
  {
    return getFloatVal(kExperimentalSetupFloatBeamImpactPositionX0);
  };

  /** Set the mean impact position of the beam on the detector in the XY-plane. 
   */
  ExperimentalSetup &  setBeamImpactPositionY0(float beam_impact_y0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatBeamImpactPositionY0, beam_impact_y0);
    return *this;
  };

  /** Get the y-coordinate of the mean impact position of the beam on the detector in the XY-plane.
   */
  float getBeamImpactPositionY0() const 
  {
    return getFloatVal(kExperimentalSetupFloatBeamImpactPositionY0);
  };

  /** Set the angle of the detector w.r.t. z-axis in the ZX-plane.
   */
  ExperimentalSetup &  setDetectorAngleZX(float detector_angle_zx) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorAngleZX, detector_angle_zx);
    return *this;
  };

  /** Get the angle of the detector w.r.t. z-axis in the ZX-plane.
   */
  float getDetectorAngleZX() const 
  {
    return getFloatVal(kExperimentalSetupFloatDetectorAngleZX);
  };

  /** Set the origin of the detector rotation in the ZX-plane.
   */
  ExperimentalSetup &  setDetectorRotationX0(float rotation_origin_x0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorRotationX0, rotation_origin_x0);
    return *this;
  };

  /** Get the origin of the detector rotation in the ZX-plane.
   */
  float getDetectorRotationX0() const 
  {
    return getFloatVal(kExperimentalSetupFloatDetectorRotationX0);
  };

  /** Set the origin of the detector rotation in the ZX-plane.
   */
  ExperimentalSetup &  setDetectorRotationZ0(float rotation_origin_z0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorRotationZ0, rotation_origin_z0);
    return *this;
  };

  /** Get the origin of the detector rotation in the ZX-plane.
   */
  float getDetectorRotationZ0() const 
  {
    return getFloatVal(kExperimentalSetupFloatDetectorRotationZ0);
  };

  /** Set the origin of the detector in the 3D-space (convenience method).
   */
  ExperimentalSetup &  setDetectorPosition(float origin_x0, float origin_y0, float origin_z0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorX0, origin_x0);
    obj()->setFloatVal(kExperimentalSetupFloatDetectorY0, origin_y0);
    obj()->setFloatVal(kExperimentalSetupFloatDetectorZ0, origin_z0);
    return *this;
  };

  /** Set the origin of the detector in the 3D-space (convenience method).
   */
  ExperimentalSetup &  setDetectorPosition(float *origin_three_vector) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorX0, origin_three_vector[0]);
    obj()->setFloatVal(kExperimentalSetupFloatDetectorY0, origin_three_vector[1]);
    obj()->setFloatVal(kExperimentalSetupFloatDetectorZ0, origin_three_vector[2]);
    return *this;
  };

  /** Set the origin of the detector in the 3D-space.
   */
  ExperimentalSetup &  setDetectorX0(float origin_x0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorX0, origin_x0);
    return *this;
  };

  /** Get the origin of the detector in the 3D-space.
   */
  float getDetectorX0() const 
  {
    return getFloatVal(kExperimentalSetupFloatDetectorX0);
  };

  /** Set the origin of the detector in the 3D-space.
   */
  ExperimentalSetup &  setDetectorY0(float origin_y0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorY0, origin_y0);
    return *this;
  };

  /** Get the origin of the detector in the 3D-space.
   */
  float getDetectorY0() const 
  {
    return getFloatVal(kExperimentalSetupFloatDetectorY0);
  };

  /** Set the origin of the detector in the 3D-space.
   */
  ExperimentalSetup &  setDetectorZ0(float origin_z0) 
  {
    obj()->setFloatVal(kExperimentalSetupFloatDetectorZ0, origin_z0);
    return *this;
  };

  /** Get the origin of the detector in the 3D-space.
   */
  float getDetectorZ0() const 
  {
    return getFloatVal(kExperimentalSetupFloatDetectorZ0);
  };

  /** Print all members (for debugging)
   */
  void print(std::ostream &os);
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("ExperimentalSetup");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "i[" << kNExperimentalSetupInts << "]: beam  type"
	    << "; f[" << kNExperimentalSetupFloats << "]: beam(E,angle[2],pos[2]), detector(angle,rot_orig[2],pos[3])";
    return message.str();
  };

  static const char *__beamTypeNames[CALICE::ExperimentalSetup::kNBeamTypes+1];

}; // class

}
#endif 
