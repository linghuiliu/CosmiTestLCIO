#ifndef FastCaliceHit_h
#define FastCaliceHit_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "lcio.h"
#include "IMPL/RawCalorimeterHitImpl.h"

#include "HcalTileIndex.hh"

#include <vector>
#include <iostream>
#include <cstdlib>

namespace CALICE {

struct FastCaliceHitMemoryPool {
  ~FastCaliceHitMemoryPool();
  std::vector<void*> mempool;
};

extern FastCaliceHitMemoryPool theFCHMemPool;


/** Using RawCalorimeterHits as FastCaliceHits
 * 
 * @author S. Schmidt DESY
 * @date 1 Oct 2006
 *
 *  change log:   Feb 2008 - Niels.Meyer@desy.de:
                             use HcalTileIndex for index decoding
 */

class FastCaliceHit : public IMPL::RawCalorimeterHitImpl {

public: 

// at least in gcc we can work around the aliasing problems with this union
  union flt_or_int {
    int i;
    float f;
  };

  FastCaliceHit() {};
  
  inline FastCaliceHit(unsigned short moduleID, unsigned short chip, unsigned short channel, float valueEnergy,
                float errorEnergy, int timeStamp) {
    
    _timeStamp = timeStamp;
    _hti = HcalTileIndex( moduleID, chip, channel );
    _cellID0 = _hti.getIndex();
    /*  (moduleID << 16) | 
	((chip & 0x00FF) << 8) |
	(channel & 0x00FF);   */
    flt_or_int t;
    t.f = valueEnergy;
    _amplitude = t.i;
    t.f = errorEnergy;
    _cellID1 = t.i;
  };
  
  inline FastCaliceHit(int cellID, float valueEnergy, float errorEnergy, int timeStamp) {
    
    _timeStamp = timeStamp;
    _cellID0 = cellID;
    _hti = HcalTileIndex( _cellID0 );
    flt_or_int t;
    t.f = valueEnergy;
    _amplitude = t.i;
    t.f = errorEnergy;
    _cellID1 = t.i;
  };
  
  inline FastCaliceHit(RawCalorimeterHit* aRawCalorimeterHit) {
    _timeStamp = aRawCalorimeterHit->getTimeStamp();
    _cellID0 = aRawCalorimeterHit->getCellID0();
    _hti = HcalTileIndex( _cellID0 );
    _amplitude = aRawCalorimeterHit->getAmplitude();
    _cellID1 = aRawCalorimeterHit->getCellID1();
  };
 
  inline void* operator new( size_t size )
  {
    // fixme: set lock to be threadsafe
    // fixme: handle "out of memory" correctly
    void * memory;
    size_t poolsize = theFCHMemPool.mempool.size();
    if ( poolsize == 0 )
      {
      memory = malloc( size );
      }
    else
      {
      memory = theFCHMemPool.mempool[ poolsize-1 ];
      theFCHMemPool.mempool.pop_back();
      }
    return memory;
  };

  inline void operator delete(void* mem)
  {
    theFCHMemPool.mempool.push_back(mem);
  };  

  /** get module number of hit
   */
  inline unsigned short getModule() const {
    return _hti.getModule();
  }

  /**  deprecated, try to avoid!
   */
  inline unsigned short getModuleID() const {
    int oldModuleID = ( getModule() << 8 ) | getModuleType();
    return oldModuleID;
  };

  /** get module type
   */
  inline unsigned short getModuleType() const {
    return _hti.getModuleType();
  }

  /** get chip number of hit
   */
  inline unsigned short getChip() const {
    return _hti.getChip();
  };
  
  /** get channel number of hit
   */
  inline unsigned short getChannel() const {
    return _hti.getChannel();
  };
  
  /** deprecated, try to avoid!
    */
  inline unsigned short getCellKey() const {
    return (unsigned short) ((getChip() << 8) | getChannel());
  };

  
  /** get cellID
    */
  inline int getCellID() const {
    return (int) _cellID0;
  };
  
  /** get energy value of hit 
   */
  inline float getEnergyValue() const {
    flt_or_int t;
    t.i = _amplitude;
    return t.f;
  };
  
  /** get energy error of hit
   */
  inline float getEnergyError() const {
    flt_or_int t;
    t.i = _cellID1;
    return t.f;
  };
  
  /** get time stamp
   */
  inline int getTimeStamp() const {
    return _timeStamp;
  };
  
  /** set energy value of hit 
   */
  inline void setEnergyValue(float energyValue) {
    flt_or_int t;
    t.f = energyValue;
    _amplitude = t.i;
  };
  
  /** set energy error of hit
   */
  inline void setEnergyError(float energyError) {
    flt_or_int t;
    t.f = energyError;
    _cellID1 = t.i;
  };
  
  /** set time stamp
   */
  inline void setTimeStamp(unsigned timeStamp) {
    _timeStamp = timeStamp;
  };
  
  /** convenient print method
   */
  void print(std::ostream& os);

 protected:
  HcalTileIndex _hti;

};

  std::ostream& operator<<(std::ostream& out, FastCaliceHit fch);

}

#endif
