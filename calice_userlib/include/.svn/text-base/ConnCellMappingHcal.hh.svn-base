#ifndef ConnCellMappingHcal_h
#define ConnCellMappingHcal_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NCNINT 3 
#define NCNFLO 0
#define NCNDBL 0


#define ICOORDSHIFT 16
#define JCOORDSHIFT 8
#define KCOORDSHIFT 0


using namespace lcio ;
using namespace std;



namespace CALICE {

class ConnCellMappingHcal ;

/** Mapping class based on the LCFixedObject template.
 *  that relates the 
 *  mapping of the hcal connectors to the cellids 
 */
class ConnCellMappingHcal : public LCFixedObject<NCNINT,NCNFLO,NCNDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  ConnCellMappingHcal(int connpin, int icd_fine, int jcd_fine, 
  int kcd_fine , int icd_coarse, int jcd_coarse, int kcd_coarse) {

    int cellID_fine = ( icd_fine << ICOORDSHIFT) | (jcd_fine << JCOORDSHIFT) | (kcd_fine << KCOORDSHIFT) ;
    int cellID_coarse = (icd_coarse << ICOORDSHIFT) | (jcd_coarse << JCOORDSHIFT) | (kcd_coarse << KCOORDSHIFT) ;

    obj()->setIntVal( 0 , connpin  ) ;
    obj()->setIntVal( 1 , cellID_fine  ) ;
    obj()->setIntVal( 2 , cellID_coarse  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  ConnCellMappingHcal(LCObject* obj) : LCFixedObject<NCNINT,NCNFLO,NCNDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~ConnCellMappingHcal() { /* no op*/  }
  

  /** the class interface:
    */
  int getConnpin()   { return obj()->getIntVal(0) ;  } 
  int getCellID_fine()   { return obj()->getIntVal(1);  } 
  int getCellID_coarse()   { return obj()->getIntVal(2);  } 


  /** convenient getter functions
   */ 
  short getCellIndex_fine(std::string);   
  short getCellIndex_coarse(std::string);   
  void print(  std::ostream& os) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  /** Type definition */
  const std::string getTypeName() const { 
    return"SiPM_MappingHcal" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:SiPMID,i:cassetteID,i:cellID" ;
  }

private:
  /** i,j or k Index of the cell id */
  short _index;

}; // class

}
#endif 
//=============================================================================
