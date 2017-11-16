#ifndef __SCECALMAPPING_HH__
#define __SCECALMAPPING_HH__  1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE {

//  int k_ID0_IntScEcalMapping = 0;
//  int k_ID1_IntScEcalMapping = 1;
//  int k_calibconst_DoubleScEcalMapping = 0;
//  int k_calibconst_err_DoubleScEcalMapping = 1;

  class ScECALMapping : public lcio::LCFixedObject<7,0,2> {

  public:

    /**  Empty constructor
     */

    ScECALMapping(){}

    ScECALMapping(lcio::LCObject* obj) : lcio::LCFixedObject<7,0,2>(obj){}

    ScECALMapping(int ID0, int ID1, int SLOT, int FE, int CHIP, int CHANNEL, int DAC, double DACv0, double DACover ) {

      obj()->setIntVal(0, ID0);
      obj()->setIntVal(1, ID1);
      obj()->setIntVal(2, SLOT);
      obj()->setIntVal(3, FE);
      obj()->setIntVal(4, CHIP);
      obj()->setIntVal(5, CHANNEL);
      obj()->setIntVal(6, DAC);
      obj()->setDoubleVal(0, DACv0);
      obj()->setDoubleVal(1, DACover);
    }

    /**  Destructor
     */
    ~ScECALMapping(){
    }

    /** Get the identifier.
     */
    int getID0() const { //Layer 

      return getIntVal(0);

    }

    int getID1() const { //strip

      return getIntVal(1);

    }
     int getSLOT() const { //strip

      return getIntVal(2);

    }
     int getFE() const { //strip

      return getIntVal(3);

    }
     int getCHIP() const { //strip

      return getIntVal(4);

    }
     int getCHANNEL() const { //strip

      return getIntVal(5);

    }
 
     int getDAC() const { //strip

      return getIntVal(6);

    }
   
    /** Get the constant/y-intercept of the fit.
     *
     */
    double getDACv0() const {

      return getDoubleVal(0);

    }

    /** Get the reference point of the constant.
     */
    double getDACover() const {

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
     void setSLOT(const int SLOT) {

      obj()->setIntVal(2, SLOT);

    }
     void setFE(const int FE) {

      obj()->setIntVal(3, FE);

    }
     void setCHIP(const int CHIP) {

      obj()->setIntVal(4, CHIP);

    }
     void setCHANNEL(const int CHANNEL) {

      obj()->setIntVal(5, CHANNEL);

    }
     void setDAC(const int DAC) {

      obj()->setIntVal(6, DAC);

    }
   
    /** Sets the constant.
     */
    void setDACv0(const double DACv0) {

      obj()->setDoubleVal(0, DACv0);

    }


    /** Sets the reference point of the constant.
     */
    void setDACover(const double DACover) {

      obj()->setDoubleVal(1, 
                          DACover);

    }

    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "ScECALMapping";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID0/Layer,"
	     "i:ID1/Strip,"
	     "i:SLOT,"
	     "i:FE,"
	     "i:CHIP,"
	     "i:CHANNEL,"
	     "i:DAC,"
             "f:DACv0,"
             "f:DACover,";
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::ScECALMapping& lfr) {

    out << "ID0: " << lfr.getID0()
        << " ID1: " << lfr.getID1()
        << " SLOT: " << lfr.getSLOT()
        << " FE: " << lfr.getFE()
        << " CHIP: " << lfr.getCHIP()
        << " CHANNEL: " << lfr.getCHANNEL()
        << " DAC: " << lfr.getDAC()
        << " DACv0: " << lfr.getDACv0()
        << " DACover: " << lfr.getDACover();

    return out;

  }

}


#endif









