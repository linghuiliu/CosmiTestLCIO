#ifndef SiPMMapping_h
#define SiPMMapping_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NSIINT 3 
#define NSIFLO 0
#define NSIDBL 0


#define CASSETTESHIFT 24
#define ICOORDSHIFT 16
#define JCOORDSHIFT 8
#define KCOORDSHIFT 0


using namespace lcio ;
using namespace std;


namespace CALICE {

class SiPMMappingHcal ;

/** Mapping class based on the LCFixedObject template.
 *  that maps SiPMs to the the geom. hcal cells.
 */
class SiPMMappingHcal : public LCFixedObject<NSIINT,NSIFLO,NSIDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  SiPMMappingHcal(int sipmID, int cassetteID, int icoord, int jcoord , int kcoord) {

    int cellID = (cassetteID << CASSETTESHIFT ) | (icoord << ICOORDSHIFT) | (jcoord << JCOORDSHIFT) | (kcoord << KCOORDSHIFT) ;

    obj()->setIntVal( 0 , sipmID  ) ;
    obj()->setIntVal( 1 , cassetteID  ) ;
    obj()->setIntVal( 2 , cellID  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  SiPMMappingHcal	(LCObject* obj) : LCFixedObject<NSIINT,NSIFLO,NSIDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~SiPMMappingHcal	() { /* no op*/  }
  

  /** the class interface:
    */
  int getSiPMID()   { return obj()->getIntVal(0) ;  } 
  int getcassetteID()   { return obj()->getIntVal(1) ;  } 
  int getExCellID()   { return obj()->getIntVal(2);  } 
  int getCellID()   { return (obj()->getIntVal(2) & 0x00ffffff);  } 

  /** convenient getter functions
   */ 
  short getCellIndex(std::string);   
  void print(  std::ostream& os , std::string) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  const std::string getTypeName() const { 
    return"SiPM_MappingHcal" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:SiPMID,i:cassetteID,i:cellID" ;
  }

private:
  short _index;

}; // class

}
#endif 
//=============================================================================
