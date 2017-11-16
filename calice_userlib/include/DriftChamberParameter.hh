#ifndef DriftChamberParameter_hh
#define DriftChamberParameter_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include <sstream>
#include <cmath>
#include <cassert>

using namespace lcio ;

namespace CALICE {

/** 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */

  enum EDriftChamberParameterFloatType {kDriftChamberParameterFloatVelocity, /**< The drift velocity.*/
					kDriftChamberParameterFloatDelay,/**< time shift to subtracted from the measured time.*/
					kDriftChamberParameterFloatOffsetZ,  /**< the position in the z-direction (negative) w.r.t. 
									          ECAL front plate.*/
					kDriftChamberParameterFloatOffsetU,  /**< The position offset in the drift-direction.*/
					kDriftChamberParameterFloatError,    /**< The error on the position measurement.*/
					kNDriftChamberParameterFloats};

  enum EDriftChamberParameterIntType {kDriftChamberParameterIntWireType,         /**< The drift direction w.r.t. to the drift axis 
										     Y/X.*/
				      kNDriftChamberParameterInts};

  enum EDriftChamberWireType {kDriftChamberWireMinus,
			      kDriftChamberWirePlus,
			      kDriftChamberWireXMinus,
			      kDriftChamberWireXPlus,
			      kDriftChamberWireYMinus,
			      kDriftChamberWireYPlus,
			      kNWireTypes};

  /** Parameters for one drift chamber.
   */
class DriftChamberParameter : public LCFixedObject<kNDriftChamberParameterInts,kNDriftChamberParameterFloats,0> {
public: 
  DriftChamberParameter() : LCFixedObject<kNDriftChamberParameterInts,kNDriftChamberParameterFloats,0>() { };
    
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  DriftChamberParameter(LCObject* obj) : LCFixedObject<kNDriftChamberParameterInts,kNDriftChamberParameterFloats,0>(obj) { } 
  

  /** Set the drift velocity.
   */
  DriftChamberParameter &  setDriftVelocity(float drift_velocity) 
  {
    obj()->setFloatVal(kDriftChamberParameterFloatVelocity, drift_velocity );
    return *this;
  };

  /** Get the drift velocity.
   */
  float getDriftVelocity() const
  {
    return getFloatVal(kDriftChamberParameterFloatVelocity);
  };

  /** Set the time to be subtracted from the measured time to get the drift time.
   */
  DriftChamberParameter &  setDelay(float time_shift) 
  {
    obj()->setFloatVal(kDriftChamberParameterFloatDelay, time_shift);
    return *this;
  };

  /** Get the time to be subtracted from the measured time to get the drift time.
   */
  float getDelay() const 
  {
    return getFloatVal(kDriftChamberParameterFloatDelay);
  };

#ifdef DEPRECATED
  /** Get the time to be subtracted from the measured time to get the drift time.
   * This method is deprecated please use @ref getDelay
   */
  float getTimeShift() const 
  {
    return getFloatVal(kDriftChamberParameterFloatDelay);
  };
#endif

  /** Set the position offset of the chamber in the z-direction.
   */
  DriftChamberParameter &  setOffsetZ(float offset_z) 
  {
    obj()->setFloatVal(kDriftChamberParameterFloatOffsetZ, offset_z);
    return *this;
  };

  /** Get the position offset of the chamber in the z-direction.
   */
  float getOffsetZ() const 
  {
    return getFloatVal(kDriftChamberParameterFloatOffsetZ);
  };

  /** Set the position offset of the chamber in the drift direction.
   */
  DriftChamberParameter &  setOffsetU(float offset_u) 
  {
    obj()->setFloatVal(kDriftChamberParameterFloatOffsetU, offset_u);
    return *this;
  };

  /** Get the position offset of the chamber in the  drift direction.
   */
  float getOffsetU() const 
  {
    return getFloatVal(kDriftChamberParameterFloatOffsetU);
  };

  /** Set the error on the position measurement
   */
  DriftChamberParameter &  setError(float error) 
  {
    obj()->setFloatVal(kDriftChamberParameterFloatError, error);
    return *this;
  };

  /** Get the error on the position measurement
   */
  float getError() const 
  {
    return getFloatVal(kDriftChamberParameterFloatError);
  };

  /** Set the sign of the drift direction 
   */
  DriftChamberParameter &  setDirectionSign(int sign) 
  {
    obj()->setIntVal(kDriftChamberParameterIntWireType, getIntVal(kDriftChamberParameterIntWireType) & (~1) + sign % 2);
    return *this;
  };

  /** Get the sign of the drift direction
   */
  int getDirectionSign() const 
  { 
    int wire_type= getIntVal(kDriftChamberParameterIntWireType);
    if (wire_type<0) return -1;
    return (wire_type%2==0 ? -1 : 1);
  };

  /** Calculate from the measured time the postion along the drift direction.*/
  float calcPos(float time) const {
    return getOffsetU()+getDirectionSign() * ( time - getDelay()) * getDriftVelocity();
  };

  /** Calculate from the measured time the postion along the drift direction.*/
  float calcTime(float pos) const {
    return getDirectionSign()*(pos-getOffsetU()) / getDriftVelocity() + getDelay() ;
  };

  /** Calculate from the measured residual the shift of the offset to be subtracted from the measured time.*/
  float calcDelay(float residual) const {
    return residual*getDirectionSign()/getDriftVelocity()+getDelay();
  };

  /** Return the type of the wire.
   * Wires are distinguished by their orientation (x.y) and the position with respect 
   * to the beam axis (left or right of the beam axis for x; below or above the beam
   * axis for y wires).
   */
  EDriftChamberWireType getWireType() const {
    int wire_type= getIntVal(kDriftChamberParameterIntWireType);
    if (wire_type<0) return kDriftChamberWireMinus;
    return static_cast<EDriftChamberWireType>(wire_type);
  };

  /** Set the wire type.
   */
  void setWireType(int wire_type)  {
#ifdef BOUNDARY_CHECK
    assert (wire_type<=kNWireTypes) ;
#endif    
    obj()->setIntVal(kDriftChamberParameterIntWireType, wire_type);
  }

  /** Print all members (for debugging)
   */
  void print(std::ostream &os);
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return std::string("DriftChamberParameter");
  };
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    std::stringstream message;
    message << "i[" << kNDriftChamberParameterInts << "]: sign of drift direction"
	    << "f[" << kNDriftChamberParameterFloats << "]: velovity, time shift, z, u offsets, error.";
    return message.str();
  };

}; // class

  /** Calculate the drift velocity from the measured drift time of two adjacent chambers (same axis).
   * Assuming that the particle track is reasonably parallel to the z-axis, the electrons generated
   * in two adjacent chambers (containing wires pointing in the same direction) will travel in total
   * the distance (in the drift direction) between the adjacent wires. The drift velocity is then:
   * L/(t1+t2), where L is the distance of the wires and t1 and t2 the drift times measured at the 
   * two wires.
   */
  inline float calculateDriftVelocity(float drift_time_u, float drift_time_next_u,
				      const DriftChamberParameter &axis_u,  
				      const DriftChamberParameter &next_axis_u) {
    float total_drift_time=
        drift_time_u      - axis_u.getDelay()
      + drift_time_next_u - next_axis_u.getDelay();
    
    // TODO: apply correction due to track inclination ? 
    
    float total_drift_distance=
        fabs( axis_u.getOffsetU() - next_axis_u.getOffsetU() );
    
    float drift_velocity=total_drift_distance/total_drift_time;
    
    //    std::cout << "drift_time_tot=" << drift_time_u << "+" << drift_time_next_u << " = " << total_drift_time
    //    	      << " distance=" <<     axis_u.getOffsetU() << " +  " << next_axis_u.getOffsetU()
    //    	      << " = " << total_drift_distance 
    //    	      << " -> " << drift_velocity << std::endl;
    return drift_velocity;
  }

}
#endif 
