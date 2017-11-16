#ifndef LINEAR_FIT_RESULT_HH
#define LINEAR_FIT_RESULT_HH 1

#include "UTIL/LCFixedObject.h"
#include <cmath>

#include <iostream>

namespace CALICE {

  // helper enums:

  // NDF = number of degress of freedom
  enum EIntLFTValues {k_ID_IntLFT, k_NDF_IntLFT, kNIntLFTValues};

  // no floats for the moment
  enum EFloatLFTValues {kNFloatLFTValues};

  // first parameter offset, second slope.
  // error matrix: (A B)
  //               (C D)
  enum EDoubleLFTValues {k_Offset_DoubleLFT, 
                         k_Slope_DoubleLFT,
                         k_Chi2_DoubleLFT,
                         k_ErrorMatrixA_DoubleLFT,
                         k_ErrorMatrixB_DoubleLFT,
                         k_ErrorMatrixC_DoubleLFT,
                         k_ErrorMatrixD_DoubleLFT,
                         kNDoubleLFTValues};

  /** Class to store the result of a linear fit within the lcio framework.
   * 
   * This class can be used for example to store the result of a
   * linear fit, e.g. TF1 root fit.
   *
   * Errors are stored in the error matrix.
   *
   * @author Sebastian Richter (sebastian.richter@desy.de)
   * @version 1.0
   * @date 9 January 2009
   */
  class LinearFitResult : public lcio::LCFixedObject<kNIntLFTValues,
                                                     kNFloatLFTValues,
                                                     kNDoubleLFTValues> {

  public:

    LinearFitResult(lcio::LCObject* obj) : lcio::LCFixedObject<kNIntLFTValues,
                                                               kNFloatLFTValues,
                                                               kNDoubleLFTValues
                                                              >(obj) {}

    LinearFitResult(int ID, double offset, double slope, double chi2, 
                    double eMA, double eMB, double eMC, double eMD, int ndf) {

      obj()->setIntVal(k_ID_IntLFT, ID);

      obj()->setDoubleVal(k_Offset_DoubleLFT, offset);
      obj()->setDoubleVal(k_Slope_DoubleLFT, slope);
      obj()->setDoubleVal(k_Chi2_DoubleLFT, chi2);

      obj()->setDoubleVal(k_ErrorMatrixA_DoubleLFT, eMA);
      obj()->setDoubleVal(k_ErrorMatrixB_DoubleLFT, eMB);
      obj()->setDoubleVal(k_ErrorMatrixC_DoubleLFT, eMC);
      obj()->setDoubleVal(k_ErrorMatrixD_DoubleLFT, eMD);

      obj()->setIntVal(k_NDF_IntLFT, ndf);

    }

    /** Evaluates the fit.
     *
     * @param x - point where the fit is evaluated: getOffset() + x * getSlope()
     *
     */
    double eval(const double x) const {

      return getOffset() + x*getSlope();

    }

    /** Get the identifier.
     */
    int getID() const {

      return getIntVal(k_ID_IntLFT);

    }

    /** Get the number of degress of freedom of the fit.
     */
    int getNDF() const {

      return getIntVal(k_NDF_IntLFT);

    }
    
    /** Get the offset/y-intercept of the fit.
     *
     *  The offset is equivalent to eval(0).
     *
     */
    double getOffset() const {

      return getDoubleVal(k_Offset_DoubleLFT);

    }

    /** Get the slope of the fit.
     */
    double getSlope() const {

      return getDoubleVal(k_Slope_DoubleLFT);

    }

    /** Get the Chi2 of the fit.
     */
    double getChi2() const {

      return getDoubleVal(k_Chi2_DoubleLFT);

    }

    /** Get the error matrix of the fit.
     *
     * The first parameter is the offset, the second one is the slope.
     *
     */ 
    void getErrorMatrix(double errorMatrix[2][2]) const {

      errorMatrix[0][0] = getDoubleVal(k_ErrorMatrixA_DoubleLFT);
      errorMatrix[0][1] = getDoubleVal(k_ErrorMatrixB_DoubleLFT);
      errorMatrix[1][0] = getDoubleVal(k_ErrorMatrixC_DoubleLFT);
      errorMatrix[1][1] = getDoubleVal(k_ErrorMatrixD_DoubleLFT);

    }

    /** Get the error of the offset.
     */
    double getOffsetError() const {

      static double errormatrix[2][2];
      
      getErrorMatrix(errormatrix);
      
      return sqrt(errormatrix[0][0]);

    }

    /** Get the error of the slope.
     */
    double getSlopeError() const {

      static double errormatrix[2][2];

      getErrorMatrix(errormatrix);

      return sqrt(errormatrix[1][1]);

    }
    
    /** Sets an identifier.
     */
    void setID(const int ID) {

      obj()->setIntVal(k_ID_IntLFT, ID);

    }

    /** Sets the number of degress of freedom.
     */
    void setNDF(const int NDF) {

      obj()->setIntVal(k_NDF_IntLFT, NDF);

    }
    
    /** Sets the offset.
     */
    void setOffset(const double offset) {

      obj()->setDoubleVal(k_Offset_DoubleLFT, offset);

    }

    /** Sets the slope.
     */
    void setSlope(const double slope) {

      obj()->setDoubleVal(k_Slope_DoubleLFT, slope);

    }

    /** Sets the chi2.
     */
    void setChi2(const double chi2) {

      obj()->setDoubleVal(k_Chi2_DoubleLFT, chi2);

    }

    /** Sets the full error matrix.
     */
    void setErrorMatrix(const double array[2][2]) {

       obj()->setDoubleVal(k_ErrorMatrixA_DoubleLFT, array[0][0]);
       obj()->setDoubleVal(k_ErrorMatrixB_DoubleLFT, array[0][1]);
       obj()->setDoubleVal(k_ErrorMatrixC_DoubleLFT, array[1][0]);
       obj()->setDoubleVal(k_ErrorMatrixD_DoubleLFT, array[1][1]);

    }

    /** Sets the error of the offset.
     */
    void setOffsetError(double offsetError) {

      obj()->setDoubleVal(k_ErrorMatrixA_DoubleLFT, offsetError * offsetError);

    }

    /** Sets the error of the slope.
     */
    void setSlopeError(double slopeError) {

      obj()->setDoubleVal(k_ErrorMatrixD_DoubleLFT, slopeError * slopeError);

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "LinearFitResult";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID,i:NDF,f:offset,f:slope,f:chi2,f:errorMatrix00,"
                                               ",f:errorMatrix01,"
                                               ",f:errorMatrix10,"
                                               ",f:errorMatrix11";
    }

  private:


  };

inline std::ostream& operator<<(std::ostream& out, 
                         const CALICE::LinearFitResult& lfr) {

  out << "ID: " << lfr.getID()
      << " offset: " << lfr.getOffset()
      << " slope: " << lfr.getSlope();

  return out;

}

}

#endif
