#ifndef DetectorTransformation_hh
#define DetectorTransformation_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <sstream>


using namespace lcio ;

namespace CALICE {

/** 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */

  enum EDetectorTransformationFloatType {kDetectorTransformationFloatDetectorAngleZX, /**< angle between the detector and z-axis in the ZX-plane*/
					 kDetectorTransformationFloatDetectorRotationX0, /**< origin of the rotation of the detector in  ZX-plane*/
					 kDetectorTransformationFloatDetectorRotationZ0, /**< origin of the rotation of the detector in  ZX-plane*/
					 kDetectorTransformationFloatDetectorX0, /**< origin of the detector x-coordinate*/
					 kDetectorTransformationFloatDetectorY0, /**< origin of the detector y-coordinate*/
					 kDetectorTransformationFloatDetectorZ0, /**< origin of the detector z-coordinate*/
					 kNDetectorTransformationFloats};

  enum EDetectorTransformationIntType {kNDetectorTransformationInts};

  /** Define the experimental setup: beam energy, position, angle and detector position, angle.
   * The experimental setup should be written into the conditions database at least twice:
   * <ul>
   *   <li> the nominal values with the tag NOMINAL
   *   <li> the measured values
   * </ul>
   * @todo{Should this class be split into two? One for the beam and one for the detector parameters?
   * Should the nominal and measured values be stored in separate folders?}
   */
class DetectorTransformation : public LCFixedObject<kNDetectorTransformationInts,kNDetectorTransformationFloats,0> {
public: 
  DetectorTransformation() : LCFixedObject<kNDetectorTransformationInts,kNDetectorTransformationFloats,0>() { };
    
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  DetectorTransformation(LCObject* obj) : LCFixedObject<kNDetectorTransformationInts,kNDetectorTransformationFloats,0>(obj) { } 


  /** Set the angle of the detector w.r.t. z-axis in the ZX-plane.
   */
  DetectorTransformation &  setDetectorAngleZX(float detector_angle_zx) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorAngleZX, detector_angle_zx);
    return *this;
  };

  /** Get the angle of the detector w.r.t. z-axis in the ZX-plane.
   */
  float getDetectorAngleZX() const 
  {
    return getFloatVal(kDetectorTransformationFloatDetectorAngleZX);
  };

  /** Set the origin of the detector rotation in the ZX-plane.
   */
  DetectorTransformation &  setDetectorRotationX0(float rotation_origin_x0) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorRotationX0, rotation_origin_x0);
    return *this;
  };

  /** Get the origin of the detector rotation in the ZX-plane.
   */
  float getDetectorRotationX0() const 
  {
    return getFloatVal(kDetectorTransformationFloatDetectorRotationX0);
  };

  /** Set the origin of the detector rotation in the ZX-plane.
   */
  DetectorTransformation &  setDetectorRotationZ0(float rotation_origin_z0) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorRotationZ0, rotation_origin_z0);
    return *this;
  };

  /** Get the origin of the detector rotation in the ZX-plane.
   */
  float getDetectorRotationZ0() const 
  {
    return getFloatVal(kDetectorTransformationFloatDetectorRotationZ0);
  };

  /** Set the origin of the detector in the 3D-space (convenience method).
   */
  DetectorTransformation &  setDetectorTransformation(float origin_x0, float origin_y0, float origin_z0) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorX0, origin_x0);
    obj()->setFloatVal(kDetectorTransformationFloatDetectorY0, origin_y0);
    obj()->setFloatVal(kDetectorTransformationFloatDetectorZ0, origin_z0);
    return *this;
  };

  /** Set the origin of the detector in the 3D-space (convenience method).
   */
  DetectorTransformation &  setDetectorTransformation(float *origin_three_vector) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorX0, origin_three_vector[0]);
    obj()->setFloatVal(kDetectorTransformationFloatDetectorY0, origin_three_vector[1]);
    obj()->setFloatVal(kDetectorTransformationFloatDetectorZ0, origin_three_vector[2]);
    return *this;
  };

  /** Set the origin of the detector in the 3D-space.
   */
  DetectorTransformation &  setDetectorX0(float origin_x0) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorX0, origin_x0);
    return *this;
  };

  /** Get the origin of the detector in the 3D-space.
   */
  float getDetectorX0() const 
  {
    return getFloatVal(kDetectorTransformationFloatDetectorX0);
  };

  /** Set the origin of the detector in the 3D-space.
   */
  DetectorTransformation &  setDetectorY0(float origin_y0) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorY0, origin_y0);
    return *this;
  };

  /** Get the origin of the detector in the 3D-space.
   */
  float getDetectorY0() const 
  {
    return getFloatVal(kDetectorTransformationFloatDetectorY0);
  };

  /** Set the origin of the detector in the 3D-space.
   */
  DetectorTransformation &  setDetectorZ0(float origin_z0) 
  {
    obj()->setFloatVal(kDetectorTransformationFloatDetectorZ0, origin_z0);
    return *this;
  };

  /** Get the origin of the detector in the 3D-space.
   */
  float getDetectorZ0() const 
  {
    return getFloatVal(kDetectorTransformationFloatDetectorZ0);
  };

  /** Print all members (for debugging)
   */
  void print(std::ostream &os);
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("DetectorTransformation");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "f[" << kNDetectorTransformationFloats << "]: detector(angle,rot_orig[2],pos[3])";
    return message.str();
  };

}; // class

}
#endif 
