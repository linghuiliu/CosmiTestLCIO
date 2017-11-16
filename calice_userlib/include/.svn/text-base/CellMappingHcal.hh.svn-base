#ifndef CellMapping_h
#define CellMapping_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NUMINT 2 
#define NFLMAP 0
#define NDOUBLE 0


#define CRATESHIFT 24
#define SLOTSHIFT 16 
#define FESHIFT 12
#define MULSHIFT 4
#define ADCSHIFT 0

#define ICOORDSHIFT 16
#define JCOORDSHIFT 8
#define KCOORDSHIFT 0


using namespace lcio ;
using namespace std;

//class CellMappingHcal ;

namespace CALICE {

/** Example for a simple mapping class based on the LCFixedObject template.
 *  <p>
 *  LCFixedObject uses an instance of LCGenericObjectImpl that holds the data, thus 
 *  there is no overhead when the data is read from a database or file
 *  for copying it  to some local structure (Decorator pattern).<br>
 *  
 */
class CellMappingHcal : public LCFixedObject<NUMINT,NFLMAP,NDOUBLE> {
  
public: 
  
  /** Convenient c'tor.
   */
  CellMappingHcal(int icrate, int islot, int ife , int imul, int iadc, int icoord, int jcoord , int kcoord) {

    int elec_channel = (icrate << CRATESHIFT) | ( islot << SLOTSHIFT ) | 
      ( ife << FESHIFT) | ( imul << MULSHIFT) | (iadc << ADCSHIFT);
    int cellID = (icoord << ICOORDSHIFT) | (jcoord << JCOORDSHIFT) | (kcoord << KCOORDSHIFT) ;

    obj()->setIntVal( 0 , elec_channel  ) ;
    obj()->setIntVal( 1 , cellID  ) ;
    //obj()->setFloatVal( 0 ,  offset ) ;  
    //obj()->setFloatVal( 1 ,  gain ) ;  
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  CellMappingHcal(LCObject* obj) : LCFixedObject<NUMINT,NFLMAP,NDOUBLE>(obj) { } 

  /** Important for memory handling*/
  virtual ~CellMappingHcal() { /* no op*/  }
  

  /** the class interface:
    */
  int getElecChannel()   { return obj()->getIntVal(0) ;  } 
  int getCellID()   { return obj()->getIntVal(1) ;  } 

  /** convenient getter functions
   */ 
  short getCellIndex(std::string);   
  short getCrateID() { return ( ( obj()->getIntVal(0) >> CRATESHIFT ) & 0xff );  }
  short getSlotID() { return ( ( obj()->getIntVal(0) >> SLOTSHIFT ) & 0xff );  }
  //These masks are particularly ugly !!!
  short getFeID() { return ( ( ( obj()->getIntVal(0) & 0xffff) >> FESHIFT ) & 0xff);  }
  short getMulID() { return ( ( ( obj()->getIntVal(0) & 0xfff ) >> MULSHIFT ) & 0xff );  }
  short getAdcID() { return ( ( ( obj()->getIntVal(0) & 0xf ) >> ADCSHIFT ) & 0xff );  }

  
  /** Convenient print method */
  void print(  std::ostream& os ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Type definition */
  const std::string getTypeName() const { 
    return"Cell_MappingHcal" ;
  } 
  
  /** Data description, needed e.g. by graphical interface 
    * to LCCD 
    */
  const std::string getDataDescription() const {
    return "i:elec_channel,i:cellID" ;
  }

private:
  /** i,j or k Index of the cell id */
  short _index;

}; // class

}
#endif 
//=============================================================================
