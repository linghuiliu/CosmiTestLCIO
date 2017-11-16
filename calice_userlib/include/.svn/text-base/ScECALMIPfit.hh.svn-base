#ifndef __SCECALMIPFIT_HH__
#define __SCECALMIPFIT_HH__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

//#include <stdexcept>
//#include <sstream>

//#include <iostream>

namespace CALICE {

  //  int kNIntScEcalMIPValues    = 3;
  //  int kNFloatScEcalMIPValues  = 0;
  //  int kNDoubleScEcalMIPValues = 3; 

// scale_landau,  mpv_landau,  area_langau,  sigma_gauss

//  int k_ID0_IntScEcalMIP = 0;
//  int k_ID1_IntScEcalMIP = 1;
//  int k_area_langau_IntScEcalMIP = 2;
//  int k_mpv_landau_DoubleScEcalMIP = 1;
//  int k_scale_landau_DoubleScEcalMIP = 0;
//  int k_sigma_gauss_DoubleScEcalMIP =2;


  /** Class to store the constant of a ScECAL MIP fit within the lcio framework.
   * 
   * @author 
   * @version 1.0
   * @date 05 May 2011
   */

  class ScECALMIPfit : public lcio::LCFixedObject<3,0,5> {

  public:

    /**  Empty constructor
     */
    ScECALMIPfit(){}

    ScECALMIPfit(lcio::LCObject* obj) : lcio::LCFixedObject<3,0,5>(obj){}

//110513.coterra    ScECALMIPfit(int ID0, int ID1, double mpv_landau, double scale_landau, int area_langau, double sigma_gauss ) {
    ScECALMIPfit(int ID0, int ID1, double mpv_landau, double mpv_error, double slope, double slope_err, double setTemp, int nSuccess ) {

      obj()->setIntVal(0, ID0);
      obj()->setIntVal(1, ID1);
      obj()->setIntVal(2, nSuccess);
      obj()->setDoubleVal(0, mpv_landau);
      obj()->setDoubleVal(1, mpv_error);
      obj()->setDoubleVal(2, slope);
      obj()->setDoubleVal(3, slope_err);
      obj()->setDoubleVal(4, setTemp);
    }

    /**  Destructor
     */
    ~ScECALMIPfit(){
    }

    /** Get the identifier.
     */
    int getID0() const { //Layer 

      return getIntVal(0);

    }

    int getID1() const { //strip

      return getIntVal(1);

    }
    
    /** Get the constant/y-intercept of the fit.
     *
     */
    double getmpv_landau() const {

      return getDoubleVal(0);

    }

    /** Get the error of the constant.
     */
    double getmpv_error() const {

      return getDoubleVal(1);

    }

    /** Get the error of the constant.
     */
    double getslope() const {

      return getDoubleVal(2);

    }

    /** Get the error of the constant.
     */
    double getslope_err() const {

      return getDoubleVal(3);

    }

    /** Get the error of the constant.
     */
    double getsetTemp() const {

      return getDoubleVal(4);
   
    } 

    /** Get the reference point of the constant.
     */
    int getnSuccess() const {

      return getIntVal(2);

    }

    
    /** Sets an identifier.
     */
    void setID0(const int ID0) {

      obj()->setIntVal(0, ID0);

    }
    
    void setID1(const int ID1) {

      obj()->setIntVal(1, ID1);

    }
    
    /** Sets the constant.
     */
    void setmpv_landau(const double mpv_landau) {

      obj()->setDoubleVal(0, mpv_landau);

    }
    /** Sets the constant.
     */
    void setmpv_error(const double mpv_error) {

      obj()->setDoubleVal(1, mpv_error);

    }
    /** Sets the constant.
     */
    void setslope(const double slope) {

      obj()->setDoubleVal(2, slope);

    }
    /** Sets the constant.
     */
    void setslope_err(const double slope_err) {

      obj()->setDoubleVal(3, slope_err);

    }
    /** Sets the constant.
     */
    void setsetTemp(const double setTemp) {

      obj()->setDoubleVal(4, setTemp);

    }

    void setnSuccess(const int nSuccess) {

      obj()->setIntVal(2, nSuccess);

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "ScECALMIPfit";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID0/Layer,"
	     "i:ID1/Strip,"
             "i:nSuccess,"
             "f:mpv_landau,"
             "f:mpv_error,"
             "f:slope,"
             "f:slope_err,"
             "f:setTemp";
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::ScECALMIPfit& lfr) {

    out << "ID0: " << lfr.getID0()
        << "ID1: " << lfr.getID1()
        << " mpv_landau: " << lfr.getmpv_landau()
        << " mpv_error: " << lfr.getmpv_error()
        << " slope: " << lfr.getslope()
        << " slope_err: " << lfr.getslope_err()
        << " setTemp: " << lfr.getsetTemp()
        << " nSuccess: " << lfr.getnSuccess();

    return out;

  }

}


#endif
