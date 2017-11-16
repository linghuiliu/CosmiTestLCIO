#ifndef __SCECALNOISYCHANNEL_HH__
#define __SCECALNOISYCHANNEL_HH__  1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE {

  class ScECALNoisyChannel : public lcio::LCFixedObject<2,0,0> {

  public:

    /**  Empty constructor
     */

    ScECALNoisyChannel(){}

    ScECALNoisyChannel(lcio::LCObject* obj) : lcio::LCFixedObject<2,0,0>(obj){}

    ScECALNoisyChannel(int ID0, int ID1 ) {

      obj()->setIntVal(0, ID0);
      obj()->setIntVal(1, ID1);
    }

    /**  Destructor
     */
    ~ScECALNoisyChannel(){
    }

    /** Get the identifier.
     */
    int getID0() const { //Layer 

      return getIntVal(0);

    }

    int getID1() const { //strip

      return getIntVal(1);

    }
    
    
    /** Sets an identifier.
     */
    void setID0(const int ID0) {

      obj()->setIntVal(0, ID0);

    }
    
    void setID1(const int ID1) {

      obj()->setIntVal(1, ID1);

    }
    
    /** Get the type name.  
     *
     * The type name is used to determine which class interprets the
     * LCGenericObject which holds the pure data.
     */
    const std::string getTypeName() const {
      return "ScECALNoisyChannel";
    }
    
    /** Get the data description.
     *
     * The data description gives the meaning of the data fields.
     *
     */
    const std::string getDataDescription() const {
      return "i:ID0/Layer,"
	     "i:ID1/Strip,";
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                                  const CALICE::ScECALNoisyChannel& lfr) {

    out << "ID0: " << lfr.getID0()
        << "ID1: " << lfr.getID1();

    return out;

  }

}


#endif









