#ifndef TcmtHit_h
#define TcmtHit_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "lcio.h"
#include "IMPL/RawCalorimeterHitImpl.h"

#include <vector>
#include <iostream>
#include <cstdlib>

namespace CALICE {

struct TcmtHitMemoryPool {
  ~TcmtHitMemoryPool();
  std::vector<void*> mempool;
};

extern TcmtHitMemoryPool theTHMemPool;


/** Using RawCalorimeterHits as TcmtHits
 * 
 * @author S. Schmidt DESY
 * @date 1 Oct 2006
 */

class TcmtHit : public IMPL::RawCalorimeterHitImpl {

public: 

// at least in gcc we can work around the aliasing problems with this union
  union flt_or_int {
    int i;
    float f;
  };

  TcmtHit() {};
  
  inline TcmtHit(unsigned short moduleID, unsigned short chip, unsigned short channel, float valueEnergy,
                float errorEnergy, int timeStamp) {
    
    _timeStamp = timeStamp;
    _cellID0 = (moduleID << 16) | 
               ((chip & 0x00FF) << 8) |
	       (channel & 0x00FF);
    flt_or_int t;
    t.f = valueEnergy;
    _amplitude = t.i;
    t.f = errorEnergy;
    _cellID1 = t.i;
  };
  
  inline TcmtHit(int cellID, float valueEnergy, float errorEnergy, int timeStamp) {
    
    _timeStamp = timeStamp;
    _cellID0 = cellID;
    flt_or_int t;
    t.f = valueEnergy;
    _amplitude = t.i;
    t.f = errorEnergy;
    _cellID1 = t.i;
  };
  
  inline TcmtHit(RawCalorimeterHit* aRawCalorimeterHit) {
    _timeStamp = aRawCalorimeterHit->getTimeStamp();
    _cellID0 = aRawCalorimeterHit->getCellID0();
    _amplitude = aRawCalorimeterHit->getAmplitude();
    _cellID1 = aRawCalorimeterHit->getCellID1();
  };
  
  inline void* operator new( size_t size )
  {
    // fixme: set lock to be threadsafe
    // fixme: handle "out of memory" correctly
    void * memory;
    size_t poolsize = theTHMemPool.mempool.size();
    if ( poolsize == 0 )
      {
      memory = malloc( size );
      }
    else
      {
      memory = theTHMemPool.mempool[ poolsize-1 ];
      theTHMemPool.mempool.pop_back();
      }
    return memory;
  };

  inline void operator delete(void* mem)
  {
    theTHMemPool.mempool.push_back(mem);
  };  

  /** get module number of hit
   */
  inline unsigned short getModuleID() const {
    return (unsigned short) ((_cellID0 & 0xFFFF0000) >> 16);
  };

  /** get chip number of hit
   */
  inline unsigned short getChip() const {
    return (unsigned short ) ((_cellID0 & 0x0000FF00) >> 8);
  };
  
  /** get channel number of hit
   */
  inline unsigned short getChannel() const {
    return (unsigned short) ((_cellID0 & 0x000000FF));
  };
  
  /** get cellKey
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

};

}

#endif
