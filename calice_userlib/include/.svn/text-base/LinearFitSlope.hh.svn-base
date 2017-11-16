#ifndef LINEAR_FIT_SLOPE
#define LINEAR_FIT_SLOPE

#include "UTIL/LCFixedObject.h"

#include <iostream>

namespace CALICE {

  // helper enums:

  enum EIntLFSValues {k_ID_IntLFS, kNIntLFSValues};

  // no floats for the moment
  enum EFloatLFSValues {kNFloatLFSValues};

  enum EDoubleLFSValues {k_Slope_DoubleLFS, 
                         k_Slope_Error_DoubleLFS,
                         kNDoubleLFSValues};

  /** Class to store the slope of a linear fit within the lcio framework.
   * 
   * @author Sebastian Schmitt (sebastian.richter@desy.de)
   * @version 1.0
   * @date 16 February 2009
   */
  class LinearFitSlope : public lcio::LCFixedObject<kNIntLFSValues,
                                                       kNFloatLFSValues,
                                                       kNDoubleLFSValues> {

  public:

    LinearFitSlope(lcio::LCObject* obj) : lcio::LCFixedObject<kNIntLFSValues,
                                                                 kNFloatLFSValues,
                                                                 kNDoubleLFSValues
                                                                >(obj) {}

    LinearFitSlope(int ID, double slope, double slope_error ) {

      obj()->setIntVal(k_ID_IntLFS, ID);

      obj()->setDoubleVal(k_Slope_DoubleLFS, slope);
      obj()->setDoubleVal(k_Slope_Error_DoubleLFS, slope_error);

    }

    /** Get the identifier.
     */
    int getID() const {

      return getIntVal(k_ID_IntLFS);

    }
    
    /** Get the slope/y-intercept of the fit.
     *
     *  The slope is equivalent to eval(0).
     *
     */
    double getSlope() const {

      return getDoubleVal(k_Slope_DoubleLFS);

    }

    /** Get the error of the slope.
     */
    double getSlopeError() const {

      return getDoubleVal(k_Slope_Error_DoubleLFS);

    }
    
    /** Sets an identifier.
     */
    void setID(const int ID) {

      obj()->setIntVal(k_ID_IntLFS, ID);

    }
    
    /** Sets the slope.
     */
    void setSlope(const double slope) {

      obj()->setDoubleVal(k_Slope_DoubleLFS, slope);

    }

    /** Sets the error of the slope.
     */
    void setSlopeError(double slopeError) {

      obj()->setDoubleVal(k_Slope_Error_DoubleLFS, slopeError);

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "LinearFitSlope";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID,f:slope,f:slope_error";
    }

  private:

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::LinearFitSlope& lfr) {

    out << "ID: " << lfr.getID()
        << " slope: " << lfr.getSlope()
        << " slope error: " << lfr.getSlopeError();

    return out;

  }

}


#endif
