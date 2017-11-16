#ifndef __SCECALINTERCALIB_HH__
#define __SCECALINTERCALIB_HH__  1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE {

//These are confrict when we call them in two modules.
//  int k_ID0_IntScEcalInter = 0;
//  int k_ID1_IntScEcalInter = 1;
//  int k_calibconst_DoubleScEcalInter = 0;
//  int k_calibconst_err_DoubleScEcalInter = 1;

  class ScECALIntercalib : public lcio::LCFixedObject<2,0,2> {

  public:

    /**  Empty constructor
     */

    ScECALIntercalib(){}

    ScECALIntercalib(lcio::LCObject* obj) : lcio::LCFixedObject<2,0,2>(obj){}

    ScECALIntercalib(int ID0, int ID1, double calibconst, double calibconst_err ) {

      obj()->setIntVal(0, ID0);
      obj()->setIntVal(1, ID1);
      obj()->setDoubleVal(0, calibconst);
      obj()->setDoubleVal(1, calibconst_err);
    }

    /**  Destructor
     */
    ~ScECALIntercalib(){
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
    double getcalibconst() const {

      return getDoubleVal(0);

    }

    /** Get the reference point of the constant.
     */
    double getcalibconsterr() const {

      return getDoubleVal(1);

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
    void setcalibconst(const double calibconst) {

      obj()->setDoubleVal(0, calibconst);

    }


    /** Sets the reference point of the constant.
     */
    void setcalibconsterr(const double calibconst_err) {

      obj()->setDoubleVal(1, 
                          calibconst_err);

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "ScECALIntercalib";
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
                                  const CALICE::ScECALIntercalib& lfr) {

    out << "ID0: " << lfr.getID0()
        << "ID1: " << lfr.getID1()
        << " calibconst: " << lfr.getcalibconst()
        << " calibconst_err: " << lfr.getcalibconsterr();

    return out;

  }

}


#endif









