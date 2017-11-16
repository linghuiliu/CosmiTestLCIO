#ifndef __SCECALONEPEADC_HH__
#define __SCECALONEPEADC_HH__  1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE {

  int k_ID0_IntScEcalOnePe = 0;
  int k_ID1_IntScEcalOnePe = 1;
  int k_calibconst_DoubleScEcalOnePe = 0;
  int k_calibconst_err_DoubleScEcalOnePe = 1;

  class ScECALOnePeADC : public lcio::LCFixedObject<2,0,2> {

  public:

    /**  Empty constructor
     */

    ScECALOnePeADC(){}

    ScECALOnePeADC(lcio::LCObject* obj) : lcio::LCFixedObject<2,0,2>(obj){}

    ScECALOnePeADC(int ID0, int ID1, double calibconst, double calibconst_err ) {

      obj()->setIntVal(k_ID0_IntScEcalOnePe, ID0);
      obj()->setIntVal(k_ID1_IntScEcalOnePe, ID1);
      obj()->setDoubleVal(k_calibconst_DoubleScEcalOnePe, calibconst);
      obj()->setDoubleVal(k_calibconst_err_DoubleScEcalOnePe, calibconst_err);
    }

    /**  Destructor
     */
    ~ScECALOnePeADC(){
    }

    /** Get the identifier.
     */
    int getID0() const { //Layer 

      return getIntVal(k_ID0_IntScEcalOnePe);

    }

    int getID1() const { //strip

      return getIntVal(k_ID1_IntScEcalOnePe);

    }
    
    /** Get the constant/y-intercept of the fit.
     *
     */
    double getcalibconst() const {

      return getDoubleVal(k_calibconst_DoubleScEcalOnePe);

    }

    /** Get the reference point of the constant.
     */
    double getcalibconsterr() const {

      return getDoubleVal(k_calibconst_err_DoubleScEcalOnePe);

    }

    
    /** Sets an identifier.
     */
    void setID0(const int ID0) {

      obj()->setIntVal(k_ID0_IntScEcalOnePe, ID0);

    }
    
    void setID1(const int ID1) {

      obj()->setIntVal(k_ID1_IntScEcalOnePe, ID1);

    }
    
    /** Sets the constant.
     */
    void setcalibconst(const double calibconst) {

      obj()->setDoubleVal(k_calibconst_DoubleScEcalOnePe, calibconst);

    }


    /** Sets the reference point of the constant.
     */
    void setcalibconsterr(const double calibconst_err) {

      obj()->setDoubleVal(k_calibconst_err_DoubleScEcalOnePe, 
                          calibconst_err);

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "ScECALOnePeADC";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID0/Layer,"
	     "i:ID1/Strip,"
             "f:calibconst,"
             "f:calibconst_err,";
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::ScECALOnePeADC& lfr) {

    out << "ID0: " << lfr.getID0()
        << "ID1: " << lfr.getID1()
        << " calibconst: " << lfr.getcalibconst()
        << " calibconst_err: " << lfr.getcalibconsterr();

    return out;

  }

}


#endif









