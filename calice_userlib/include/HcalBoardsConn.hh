#ifndef HcalBoardsConn_h
#define HcalBoardsConn_h 1

#include <string>
#include <cmath>
#include <cstdlib>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NBCINT 5 
#define NBCFLO 0
#define NBCDBL 0


using namespace lcio ;
using namespace std;


namespace CALICE {

class HcalBoardsConn;

/** Mapping class based on the LCFixedObject template.
 *  for principle Hcal boards connections
 * @author R. Pöschl DESY
 * @date May 005
 */

 class HcalBoardsConn : public LCFixedObject<NBCINT,NBCFLO,NBCDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  HcalBoardsConn(int connector_pin, string hbab, string hab, int asic_input , int analog_output) {





    obj()->setIntVal( 0 , connector_pin  ) ;

    std::cout << "hbab: " << hbab << endl;
    if (hbab != "left" && hbab != "right") {
      std::cout << "Bad hbab string " << hbab << std:: endl; 
      std::cout << "Will exit program now " << std:: endl; 
      exit(1);
    }

    //check for correct value of hbab string
    
    if ( hbab == "left" ) obj()->setIntVal( 1 , 0  ) ;
    if ( hbab == "right" ) obj()->setIntVal( 1 , 1  ) ;
    if (hab.length() != 1) {
      std::cout << "HcalBoardsConn: Bad hab string " << hab << std:: endl; 
      std::cout << "Will exit program now " << std:: endl; 
      exit(1);
    }

    //check for correct value of hab string
    char const* habIDS = "-ABCDEF"; 
    string::size_type habID = hab.find_first_of(habIDS);
    if ( habID == string::npos) {
      std::cout << "HcalBoardsConn: Bad hab ID " << hab << std:: endl; 
      std::cout << "Will exit program now " << std:: endl; 
      exit(1);
    }
 
    //Assign hardcoded the alphanumerical ids to the integer ids
    //Not very nice but what should we de ...
    //As long as it is in the class interface ....
    //for the left hbab
    if (hab == "-") obj()->setIntVal( 2 , -1  ) ;
    if (hbab == "left") {
      if (hab == "A") obj()->setIntVal( 2 , 0  ) ;
      if (hab == "B") obj()->setIntVal( 2 , 1  ) ;
      if (hab == "C") obj()->setIntVal( 2 , 2  ) ;
      if (hab == "D") obj()->setIntVal( 2 , 3  ) ;
      if (hab == "E") obj()->setIntVal( 2 , 4  ) ;
      if (hab == "F") obj()->setIntVal( 2 , 5  ) ;
    }

    //for the right hbab
    if (hbab == "right") {
      if (hab == "A") obj()->setIntVal( 2 , 11  ) ;
      if (hab == "B") obj()->setIntVal( 2 , 10  ) ;
      if (hab == "C") obj()->setIntVal( 2 , 9  ) ;
      if (hab == "D") obj()->setIntVal( 2 , 8  ) ;
      if (hab == "E") obj()->setIntVal( 2 , 7  ) ;
      if (hab == "F") obj()->setIntVal( 2 , 6  ) ;
    }

    obj()->setIntVal( 3 , asic_input  ) ;
    obj()->setIntVal( 4 , analog_output  ) ;


  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  HcalBoardsConn(LCObject* obj) : LCFixedObject<NBCINT,NBCFLO,NBCDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~HcalBoardsConn() { /* no op*/  }
  

  /** the class interface:
    */
  int getConnectorPin()   { return obj()->getIntVal(0) ;  } 
  string getHbabID()   { 
    string HbabID;
    if (obj()->getIntVal(1) == 0) HbabID = "left";
    if (obj()->getIntVal(1) == 1) HbabID = "right";

    return HbabID ;  
   } 
  int getHabID()   { return obj()->getIntVal(2) ;  } 
  int getAsicInput() { return obj()->getIntVal(3) ;  }
  int getAnalogOutput() { return obj()->getIntVal(4) ;  }
  void print(  std::ostream& os , std::string) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  const std::string getTypeName() const { 
    return "HcalBoardsConnections" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:ConnectorPin,i:HBAB,i:HAB:AsicInput,i:AnalogOutput" ;
  }

private:
  short _index;

}; // class

}
#endif 
//=============================================================================
