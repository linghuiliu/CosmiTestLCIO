#ifndef __SCECALGAINFIT_HH__
#define __SCECALGAINFIT_HH__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

//#include <stdexcept>
//#include <sstream>

//#include <iostream>

namespace CALICE {


  class ScECALGainfit : public lcio::LCFixedObject<3,0,5> {

  public:

    /**  Empty constructor
     */
    ScECALGainfit(){}

    ScECALGainfit(lcio::LCObject* obj) : lcio::LCFixedObject<3,0,5>(obj){}

    ScECALGainfit(int ID0, int ID1, double dval, double dval_error, double slope, double slope_err, double setTemp, int nSuccess ) {

      obj()->setIntVal(0, ID0);
      obj()->setIntVal(1, ID1);
      obj()->setIntVal(2, nSuccess);
      obj()->setDoubleVal(0, dval);
      obj()->setDoubleVal(1, dval_error);
      obj()->setDoubleVal(2, slope);
      obj()->setDoubleVal(3, slope_err);
      obj()->setDoubleVal(4, setTemp);
    }

    /**  Destructor
     */
    ~ScECALGainfit(){
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
    double getdval() const {

      return getDoubleVal(0);

    }

    /** Get the error of the constant.
     */
    double getdval_error() const {

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
    void setdval(const double dval) {

      obj()->setDoubleVal(0, dval);

    }
    /** Sets the constant.
     */
    void setdval_error(const double dval_error) {

      obj()->setDoubleVal(1, dval_error);

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
      return "ScECALGainfit";
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
             "f:dval,"
             "f:dval_error,"
             "f:slope,"
             "f:slope_err,"
             "f:setTemp";
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::ScECALGainfit& lfr) {

    out << "ID0: " << lfr.getID0()
        << "ID1: " << lfr.getID1()
        << " dval: " << lfr.getdval()
        << " dval_error: " << lfr.getdval_error()
        << " slope: " << lfr.getslope()
        << " slope_err: " << lfr.getslope_err()
        << " setTemp: " << lfr.getsetTemp()
        << " nSuccess: " << lfr.getnSuccess();

    return out;

  }

}


#endif
