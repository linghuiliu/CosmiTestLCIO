#ifndef LINEAR_FIT_CONSTANT
#define LINEAR_FIT_CONSTANT

#include "UTIL/LCFixedObject.h"

#include <stdexcept>
#include <sstream>

#include <iostream>

namespace CALICE {

  // helper enums:

  enum EIntLFCValues {k_ID_IntLFC, kNIntLFCValues};

  // no floats for the moment
  enum EFloatLFCValues {kNFloatLFCValues};

  enum EDoubleLFCValues {k_Constant_DoubleLFC, 
                         k_Constant_Error_DoubleLFC,
                         k_Constant_Reference_Point_DoubleLFC,
                         k_Constant_Reference_Point_Error_DoubleLFC,
                         kNDoubleLFCValues};

  /** Class to store the constant of a linear fit within the lcio framework.
   * 
   * @author Sebastian Schmitt (sebastian.richter@desy.de)
   * @version 1.0
   * @date 16 February 2009
   */
  class LinearFitConstant : public lcio::LCFixedObject<kNIntLFCValues,
                                                       kNFloatLFCValues,
                                                       kNDoubleLFCValues> {

  public:

    LinearFitConstant(lcio::LCObject* obj) : lcio::LCFixedObject<kNIntLFCValues,
                                                                 kNFloatLFCValues,
                                                                 kNDoubleLFCValues
								 >(obj),
					     _linearFitSlopeAvailable(false),
                                             _linearFitSlope( 1 ) {}

    LinearFitConstant(int ID,
                      double constant, 
                      double constant_error, 
                      double constant_reference_point,
                      double constant_reference_point_error ) {

      obj()->setIntVal(k_ID_IntLFC, ID);

      obj()->setDoubleVal(k_Constant_DoubleLFC,
                          constant);

      obj()->setDoubleVal(k_Constant_Error_DoubleLFC, 
                          constant_error);

      obj()->setDoubleVal(k_Constant_Reference_Point_DoubleLFC, 
                          constant_reference_point);

      obj()->setDoubleVal(k_Constant_Reference_Point_Error_DoubleLFC, 
                          constant_reference_point_error);

    }

    /** In the case of a given corresponding slope this functions
     *  evaluates the linear fit at the given point.
     *
     *  @param x - the point at which the fit is evaluated
     */
    double eval(double x) { 

      if(_linearFitSlopeAvailable) {

        const double y_intercept = 
          getConstant() - _linearFitSlope*getConstantReferencePoint();

        return y_intercept + _linearFitSlope * x;

      } else {

	return getConstant();

//        std::stringstream errorMessage("Slope missing for evaluating fit");
//
//        errorMessage << " with ID: " << getID() << '\n';
//        
//        throw(std::runtime_error(errorMessage.str()));

      }

    }

    /** Get the identifier.
     */
    int getID() const {

      return getIntVal(k_ID_IntLFC);

    }
    
    /** Get the constant/y-intercept of the fit.
     *
     */
    double getConstant() const {

      return getDoubleVal(k_Constant_DoubleLFC);

    }

    /** Get the error of the constant.
     */
    double getConstantError() const {

      return getDoubleVal(k_Constant_Error_DoubleLFC);

    }

    /** Get the reference point of the constant.
     */
    double getConstantReferencePoint() const {

      return getDoubleVal(k_Constant_Reference_Point_DoubleLFC);

    }

    /** Get the error of the reference point of the constant.
     */
    double getConstantReferencePointError() const {

      return getDoubleVal(k_Constant_Reference_Point_Error_DoubleLFC);

    }

    
    /** Sets an identifier.
     */
    void setID(const int ID) {

      obj()->setIntVal(k_ID_IntLFC, ID);

    }
    
    /** Sets the constant.
     */
    void setConstant(const double constant) {

      obj()->setDoubleVal(k_Constant_DoubleLFC, constant);

    }

    /** Sets the error of the constant.
     */
    void setConstantError(double constantError) {

      obj()->setDoubleVal(k_Constant_Error_DoubleLFC, constantError);

    }


    /** Sets the reference point of the constant.
     */
    void setConstantReferencePoint(const double constantReferencePoint) {

      obj()->setDoubleVal(k_Constant_Reference_Point_DoubleLFC, 
                          constantReferencePoint);

    }

    /** Sets the error of the reference point of the constant.
     */
    void setConstantReferencePointError(double constantReferencePointError) {

      obj()->setDoubleVal(k_Constant_Reference_Point_Error_DoubleLFC, 
                          constantReferencePointError);

    }

    /** Sets a corresponding slope.
     *
     *  It is _bad_ design to have this function here. But I really
     *  want to avoid another wrapping function combining the constant
     *  and the slope. So, sorry!
     *
     */
    void setSlope(double lfs) {

      _linearFitSlope = lfs;

      _linearFitSlopeAvailable = true;

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "LinearFitConstant";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID,"
             "f:constant,"
             "f:constant_error,"
             "f:constant_reference_point,"
             "f:constant_reference_point_error";
    }

  private:

    bool _linearFitSlopeAvailable;
    double _linearFitSlope;

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::LinearFitConstant& lfr) {

    out << "ID: " << lfr.getID()
        << " constant: " << lfr.getConstant()
        << " constant error: " << lfr.getConstantError()
        << " constant reference point: " << lfr.getConstantReferencePoint()
        << " constant reference point error: " << lfr.getConstantReferencePointError();

    return out;

  }

}


#endif
