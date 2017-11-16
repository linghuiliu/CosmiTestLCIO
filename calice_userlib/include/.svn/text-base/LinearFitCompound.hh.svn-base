#ifndef LINEAR_FIT_COMPOUND
#define LINEAR_FIT_COMPOUND

#include <stdexcept>
#include <sstream>
#include <math.h>
#include <iostream>

#include "UTIL/LCFixedObject.h"
#include "LinearFitConstant.hh"
#include "LinearFitSlope.hh"


namespace CALICE {

  // helper enums:

  enum EIntLFValues {k_ID_IntLF, kNIntLFValues};

  // no floats for the moment
  enum EFloatLFValues {kNFloatLFValues};

  enum EDoubleLFValues {k_Constant_DoubleLF,
                        k_Constant_Error_DoubleLF,
                        k_Constant_Reference_Point_DoubleLF,
                        k_Constant_Reference_Point_Error_DoubleLF,
                        k_Slope_DoubleLF,
                        k_Slope_Error_DoubleLF,
                        kNDoubleLFValues};

  /** Class to store the constant of a linear fit within the lcio framework.
   *
   * @author LNTAI
   * @version 1.0
   * @date 27 October 2009
   */
  class LinearFitCompound : public lcio::LCFixedObject<kNIntLFValues,
                            kNFloatLFValues,
                            kNDoubleLFValues> {

  public:

    LinearFitCompound(lcio::LCObject* obj) : lcio::LCFixedObject<kNIntLFValues,
                                                                 kNFloatLFValues,
                                                                 kNDoubleLFValues
                                                                 >(obj){}

    LinearFitCompound(const LinearFitConstant* lfc, const LinearFitSlope* lfs)
    {
      const int ID = lfc->getID();
      obj()->setIntVal(k_ID_IntLF, ID);

      const double constant       = lfc->getConstant();
      const double constant_error = lfc->getConstantError();
      obj()->setDoubleVal(k_Constant_DoubleLF, constant);
      obj()->setDoubleVal(k_Constant_Error_DoubleLF, constant_error);

      const double constant_reference_point       = lfc->getConstantReferencePoint();
      const double constant_reference_point_error = lfc->getConstantReferencePointError();
      obj()->setDoubleVal(k_Constant_Reference_Point_DoubleLF, constant_reference_point);
      obj()->setDoubleVal(k_Constant_Reference_Point_Error_DoubleLF, constant_reference_point_error);

      const double slope       = lfs->getSlope();
      const double slope_error = lfs->getSlopeError();
      obj()->setDoubleVal(k_Slope_DoubleLF, slope);
      obj()->setDoubleVal(k_Slope_Error_DoubleLF, slope_error);

    }

    LinearFitCompound(int ID,
                      double constant,
                      double constant_error,
                      double constant_reference_point,
                      double constant_reference_point_error,
                      double slope,
                      double slope_error)
    {

      obj()->setIntVal(k_ID_IntLF, ID);

      obj()->setDoubleVal(k_Constant_DoubleLF, constant);
      obj()->setDoubleVal(k_Constant_Error_DoubleLF, constant_error);

      obj()->setDoubleVal(k_Slope_DoubleLF, slope);
      obj()->setDoubleVal(k_Slope_Error_DoubleLF, slope_error);

      obj()->setDoubleVal(k_Constant_Reference_Point_DoubleLF, constant_reference_point);
      obj()->setDoubleVal(k_Constant_Reference_Point_Error_DoubleLF, constant_reference_point_error);

    }

    /**
     *  Evaluates the linear fit at the given point.
     *
     *  @param x - the point at which the fit is evaluated
     */
    double eval(double x)
    {
      const double y = getConstant() + (x - getConstantReferencePoint()) * getSlope();
      return y;
    }

    double evalErr(double x, double x_error)
    {
      const double error2 = pow(getConstantError(), 2)
        + pow(getSlope(), 2) * (pow(x_error, 2) + pow(getConstantReferencePointError(), 2))
        + (x*x + pow(getConstantReferencePoint(), 2)) * pow(getSlope(), 2);

      return sqrt(error2);
    }



    /** Get the identifier.
     */
    int getID() const {

      return getIntVal(k_ID_IntLF);

    }

    /** Get the constant/y-intercept of the fit.
     *
     */
    double getConstant() const {

      return getDoubleVal(k_Constant_DoubleLF);

    }

    /** Get the error of the constant.
     */
    double getConstantError() const {

      return getDoubleVal(k_Constant_Error_DoubleLF);

    }

    /** Get slope of the linear fit
     */
    double getSlope() const {

      return getDoubleVal(k_Slope_DoubleLF);

    }

    /** Get error of the slope
     */
    double getSlopeError() const {

      return getDoubleVal(k_Slope_Error_DoubleLF);

    }

    /** Get the reference point of the constant.
     */
    double getConstantReferencePoint() const {

      return getDoubleVal(k_Constant_Reference_Point_DoubleLF);

    }

    /** Get the error of the reference point of the constant.
     */
    double getConstantReferencePointError() const {

      return getDoubleVal(k_Constant_Reference_Point_Error_DoubleLF);

    }




    /** Sets an identifier.
     */
    void setID(const int ID) {

      obj()->setIntVal(k_ID_IntLF, ID);

    }

    /** Sets the constant.
     */
    void setConstant(const double constant) {

      obj()->setDoubleVal(k_Constant_DoubleLF, constant);

    }

    /** Sets the error of the constant.
     */
    void setConstantError(double constantError) {

      obj()->setDoubleVal(k_Constant_Error_DoubleLF, constantError);

    }

    /** Sets the slope.
     */
    void setSlope(const double slope) {

      obj()->setDoubleVal(k_Slope_DoubleLF, slope);

    }

    /** Sets the error of the slope.
     */
    void setSlopeError(const double slope_error) {

      obj()->setDoubleVal(k_Slope_Error_DoubleLF, slope_error);

    }


    /** Sets the reference point of the constant.
     */
    void setConstantReferencePoint(const double constantReferencePoint) {

      obj()->setDoubleVal(k_Constant_Reference_Point_DoubleLF,
                          constantReferencePoint);

    }

    /** Sets the error of the reference point of the constant.
     */
    void setConstantReferencePointError(double constantReferencePointError) {

      obj()->setDoubleVal(k_Constant_Reference_Point_Error_DoubleLF,
                          constantReferencePointError);

    }

    /** Get the type name.
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "LinearFitCompound";
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
        "f:slope,"
        "f:slope_error,"
        "f:constant_reference_point,"
        "f:constant_reference_point_error";
    }

  private:



  };

  inline std::ostream& operator<<(std::ostream& out,
                                  const CALICE::LinearFitCompound& lfr) {

    out << "ID: " << lfr.getID()
        << " constant: " << lfr.getConstant()
        << " constant error: " << lfr.getConstantError()
        << " constant reference point: " << lfr.getConstantReferencePoint()
        << " constant reference point error: " << lfr.getConstantReferencePointError()
        << " slope: " << lfr.getSlope()
        << " slope error: " << lfr.getSlopeError();

    return out;

  }

}


#endif
