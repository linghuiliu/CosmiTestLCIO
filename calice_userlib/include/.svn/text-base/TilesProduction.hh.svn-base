#ifndef TilesProduction_h
#define TilesProduction_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include "HcalTileIndex.hh"

#define NPIINT 5
#define NPIFLO 0
#define NPIDBL 0


using namespace lcio ;
using namespace std;


namespace CALICE {
class TilesProduction ;

/** Parameters of the sipm delivered by PRODUCTION
 */
class TilesProduction : public LCFixedObject<NPIINT,NPIFLO,NPIDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  TilesProduction(int SIPM, int TileSize, int Module, int Chip, int Chan) {

    //Set the integer values
    obj()->setIntVal(0, TileSize);
    obj()->setIntVal(1, SIPM);
    obj()->setIntVal(2, Module);
    obj()->setIntVal(3, Chip);
    obj()->setIntVal(4, Chan);
    _hti = HcalTileIndex( Module, Chip, Chan );
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  TilesProduction(LCObject* obj) : LCFixedObject<NPIINT,NPIFLO,NPIDBL>(obj) { 
    _hti = HcalTileIndex( getModule(), getChip(), getChan() );
  } 

  /** Important for memory handling*/
  virtual ~TilesProduction	() { /* no op*/  }

  /** the class interface:
    */
  int getTileSize() { return obj()->getIntVal(0); } 
  int getSIPMID() { return obj()->getIntVal(1); } 
  int getModule() { return obj()->getIntVal(2); } 
  int getChip() { return obj()->getIntVal(3); } 
  int getChan() { return obj()->getIntVal(4); } 

  int getHardwareID(){ return _hti.getIndex(); }

  /** convenient print function
   */ 
  void print(  std::ostream& os) ;
  
  // -------- need to implement abstract methods from LCGenericObject
  const std::string getTypeName() const { 
    return "SiPM_MappingHcal";
  } 
  
  const std::string getDataDescription() const {
    return "i:TileSize,i:SiPM,i:Module,i:Chip,i:Chan"; 
  }

 protected:
  HcalTileIndex _hti;

}; // class

}
#endif 
