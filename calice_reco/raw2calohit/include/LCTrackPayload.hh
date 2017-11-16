// -*- C++ -*-
#ifndef LCTRACKPAYLOAD_H
#define LCTRACKPAYLOAD_H 1

#include "EVENT/LCGenericObject.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "EVENT/LCObject.h"
#include <cassert>


/** Specific LCIO implementation of the interface to store generic 
 * user data.
 * 
 * @author dauncey
 * @version $Id: LCTrackPayload.hh,v 1.1 2007-04-26 14:34:28 poeschl Exp $
 */

template <class Payload>
class LCTrackPayload : public EVENT::LCGenericObject {

public: 
  
  /** Constructors.
   */
  LCTrackPayload() ;
  LCTrackPayload(const EVENT::LCGenericObject* obj);
  
  LCTrackPayload(const Payload &c) ;
  LCTrackPayload(const Payload &p,IMPL::LCGenericObjectImpl & obj) ;

  /// Destructor.
  virtual ~LCTrackPayload() { /* nop */; }
  
    //virtual int id() { return simpleUID() ; }

  /** Update values within the object from an LCGenericObject
   */
  virtual void update(const EVENT::LCGenericObject* obj);

  /** Create a new LCGenericObject and fill it
   */
  virtual EVENT::LCGenericObject* output() const;
 
  
  /** Number of integer values stored in this object.
   */
  virtual int getNInt() const;
  
  /** Number of float values stored in this object.
   */
  virtual int getNFloat() const;
  
  /** Number of double values stored in this object.
   */
  virtual int getNDouble() const;
  
  /** Returns the integer value for the given index.
   */
  virtual int getIntVal(int index) const;
  
  /** Returns the float value for the given index.
   */
  virtual float getFloatVal(int index) const;
  
  /** Returns the double value for the given index.
   */
  virtual double getDoubleVal(int index) const;
  


  /** Sets the integer value at the given index.
   */
  virtual void setIntVal(unsigned index, int value) ;

  /** Sets the float value at the given index.
   */
  virtual void setFloatVal(unsigned index, float value) ;

  /** Sets the double value at the given index.
   */
  virtual void setDoubleVal(unsigned index, double value) ;

  /** True if objects of the implementation class have a fixed size, i.e
   * getNInt, getNFloat and getNDouble will return values that are constant during 
   * the lifetime of the object.
   */
  virtual bool isFixedSize() const;

  /** The type name of the user class (typically the class name)
   */
  virtual const std::string getTypeName() const ;
  
  /** The description string. A comma separated list of pairs of
   *  type identifier, one of 'i','f','d' followed by ':' 
   *  and an attribute name, e.g. "i:cellId,f:offset,f:gain".
   */
  virtual const std::string getDataDescription() const ;

  /** Access the actual data for read or write
   */
  virtual const Payload& constants() const ;
  virtual void constants(const Payload &p) ;
  virtual const Payload& payload() const ;
  virtual void payload(const Payload &p) ;
  

protected:

  Payload _payload;

}; // class


template <class Payload>
LCTrackPayload<Payload>::LCTrackPayload() {
}

template <class Payload>
LCTrackPayload<Payload>::LCTrackPayload(const EVENT::LCGenericObject* obj) {
  update(obj);
}

template <class Payload>
LCTrackPayload<Payload>::LCTrackPayload(const Payload &p)
  : _payload(p) {
}

template <class Payload>
LCTrackPayload<Payload>::LCTrackPayload(const Payload &p, IMPL::LCGenericObjectImpl & obj)
  : _payload(p) {
   for (int i(0);i<Payload::numberOfInts;i++){
     obj.setIntVal(i,*(_payload.intData()+i));
//     std::cout << "intVal[" << i << "] = " << getIntVal(i) << std::endl;
   }
   assert (obj.getNInt() == Payload::numberOfInts);

   for (int i(0);i<Payload::numberOfFloats;i++){
     obj.setFloatVal(i,*(_payload.floatData()+i));
//     std::cout << "floatVal[" << i << "] = " << getFloatVal(i) << std::endl;
   }
   assert (obj.getNFloat() == Payload::numberOfFloats);

   for (int i(0);i<Payload::numberOfDoubles;i++){
     obj.setDoubleVal(i,*(_payload.doubleData()+i));
   }
   assert (obj.getNDouble() == Payload::numberOfDoubles);

}

template <class Payload>
void LCTrackPayload<Payload>::update(const EVENT::LCGenericObject* obj) {
  assert(getNFloat()==obj->getNFloat());
  for(int i(0);i<obj->getNFloat();i++) {
    setFloatVal(i,obj->getFloatVal(i));
  }
  
  assert(getNInt()==obj->getNInt());
  for(int i(0);i<obj->getNInt();i++) {
    setIntVal(i,obj->getIntVal(i));
  }
  
  assert(getNDouble()==obj->getNDouble());
  for(int i(0);i<obj->getNDouble();i++) {
    setDoubleVal(i,obj->getDoubleVal(i));
    }
}

template <class Payload>
EVENT::LCGenericObject* LCTrackPayload<Payload>::output() const {
  IMPL::LCGenericObjectImpl *obj(new IMPL::LCGenericObjectImpl());
  
  for(int i(0);i<getNFloat();i++) {
    obj->setFloatVal(i,getFloatVal(i));
  }
  assert(getNFloat()==obj->getNFloat());
  
  for(int i(0);i<getNInt();i++) {
    obj->setIntVal(i,getIntVal(i));
  }
  assert(getNInt()==obj->getNInt());
  
  for(int i(0);i<getNDouble();i++) {
    obj->setDoubleVal(i,getDoubleVal(i));
    }
  assert(getNDouble()==obj->getNDouble());
  
  return obj;
}

template <class Payload>
int LCTrackPayload<Payload>::getNInt() const{
  return Payload::numberOfInts ;
}

template <class Payload>
int LCTrackPayload<Payload>::getNFloat() const{
  return Payload::numberOfFloats ;
}

template <class Payload>
int LCTrackPayload<Payload>::getNDouble() const{
  return Payload::numberOfDoubles ;
}

template <class Payload>
int LCTrackPayload<Payload>::getIntVal(int index) const{
  assert(index<Payload::numberOfInts);
  return *(_payload.intData()+index) ;
}

template <class Payload>
float LCTrackPayload<Payload>::getFloatVal(int index) const{
  assert(index<Payload::numberOfFloats);
  return *(_payload.floatData()+index) ;
}

template <class Payload>
double LCTrackPayload<Payload>::getDoubleVal(int index) const{
  assert(index<Payload::numberOfDoubles);
  return *(_payload.doubleData()+index) ;
}

template <class Payload>
bool LCTrackPayload<Payload>::isFixedSize() const{
  return true ;
}

template <class Payload>
void LCTrackPayload<Payload>::setIntVal(unsigned index, int value) {
  if(index < Payload::numberOfInts)
    *(_payload.intData()+index)=value ;
}

template <class Payload>
void LCTrackPayload<Payload>::setFloatVal(unsigned index, float value){
  if(index < Payload::numberOfFloats)
    *(_payload.floatData()+index)=value ;
}

template <class Payload>
void LCTrackPayload<Payload>::setDoubleVal(unsigned index, double value){
  if(index < Payload::numberOfDoubles)
    *(_payload.doubleData()+index)=value ;
}

template <class Payload>
const std::string  LCTrackPayload<Payload>::getTypeName() const {
  return "LCTrackPayload" ;
}

template <class Payload>
const std::string  LCTrackPayload<Payload>::getDataDescription() const {
  return "Constants needed for track fit; chamber positions, alignment, drift velocity and error matrices" ;
}

template <class Payload>
const Payload& LCTrackPayload<Payload>::constants() const {
  return _payload ;
}

template <class Payload>
void LCTrackPayload<Payload>::constants(const Payload &p) {
  _payload=p ;
}

template <class Payload>
const Payload& LCTrackPayload<Payload>::payload() const {
  return _payload ;
}

template <class Payload>
void LCTrackPayload<Payload>::payload(const Payload &p) {
  _payload=p ;
}

#endif /* ifndef LCTRACKPAYLOAD_H */
