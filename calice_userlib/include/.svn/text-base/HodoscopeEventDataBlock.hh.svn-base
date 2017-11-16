#ifndef HodoscopeEventDataBlock_hh
#define HodoscopeEventDataBlock_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
//CRP Use 'local' interface to LCGenericObjects
#include "LCGenericObjectImplExt.hh"
#define HODCHANNELSHIFT 16


#ifdef BOUNDARY_CHECK
#  include <cassert>
#endif

using namespace lcio ;
//using namespace CALICE;

namespace CALICE {

  /** The indices of all the stored values.
   */
  enum ERoConfIntValues {kHodEvtStatus,
			 kHodEvtEvtCounter,
			 kHodEvtTimeCounter,
			 kHodEvtNumHitsX,                      
			 kHodEvtNumHitsY                      
  };

typedef std::map<int,int> HodoscopeHitMap_t;

 /** Interface class to the (LAL) Hodoscope data.
   * From the user point of view the class returns maps 
   * containing the hits in x and y  respectively, but also other
   * values like the timecounter and eventcounter are returned.
   * The latter is useful to check whether the hodoscope has been
   * in synch with the daq.
   * @author R. Poeschl LAL (based on the other interface classes) 
   * 
   * @date Aug  2007
   */
class HodoscopeEventDataBlock : public LCGenericObjectImplExt {


public:



  /** Default Constructor */
      HodoscopeEventDataBlock(); 

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      HodoscopeEventDataBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { 
        createHitMaps();
      } 

  /* Set the status word */ 
    HodoscopeEventDataBlock &setStatusWord(int status) {
     obj()->setIntVal(kHodEvtStatus, status); 
     return *this;
    }    

    /** Get the Hodoscope Status*/
    int getStatusWord() const {return getIntVal(kHodEvtStatus);} 



  /* Set the (hodoscope internal) event counter */ 
    HodoscopeEventDataBlock &setEventCounter(int counter) {
     obj()->setIntVal(kHodEvtEvtCounter, counter); 
     return *this;
    }    


    /** Get the Event counter */
    int getEventCounter() const {
       return getIntVal(kHodEvtEvtCounter);
    } 


    /** Set time counter */
    HodoscopeEventDataBlock &setTimeCounter(int timecounter) {
     obj()->setIntVal(kHodEvtTimeCounter, timecounter); 
     return *this;
      
    }    
 
    /** Get the time counter */
  int getTimeCounter() const {
       return getIntVal(kHodEvtTimeCounter);
    } 
  
  void storeHitMaps(std::vector<int>&, std::vector<int>&);

  /** Get the Number of Hits in X direction */
  unsigned int getNumHitsX() const {return static_cast<unsigned int>(getIntVal(kHodEvtNumHitsX));}

  /** Get the Number of Hits in Y direction */
  unsigned int getNumHitsY() const {return static_cast<unsigned int>(getIntVal(kHodEvtNumHitsY));}

  /** Give the hit maps to the user*/
  HodoscopeHitMap_t getHitMapX() const{
    return _hitMapX;
  }
  
  HodoscopeHitMap_t getHitMapY() const {
    return _hitMapY;
  }

  /** Convenient print method*/
  //void print();
  void print(std::ostream& os);



  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"HodoscopeEventDataBlock" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:event_counter,i:time_counter,i:numHits,i:numhitsX,i:numHitsY,i[numhitsX]:hitsX,i[numHitsY]:hitsY" ;}
  
private: 
  /** The hitmaps measured by the hodoscope*/
  HodoscopeHitMap_t _hitMapX;
  HodoscopeHitMap_t _hitMapY;
  /** A method to create the hitmaps (to be called by the constructor at access time*/
  void createHitMaps();
  /** A pool which checks whetherthe hit map has already been recreated*/
  bool _hitMapsCreated;
 };


}

#endif
