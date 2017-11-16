#ifndef LCPayload_HH
#define LCPayload_HH

#include <cassert>
#include <fstream>
#include <iomanip>

#include "EVENT/LCGenericObject.h"
#include "IMPL/LCGenericObjectImpl.h"


/** Specific LCIO implementation of the interface to store generic 
 * user data.
 * 
 * @author dauncey
 * @version $Id: LCPayload.hh,v 1.3 2009-03-24 11:40:43 dauncey Exp $
 */

template <class Payload> class LCPayload : public EVENT::LCGenericObject {

public: 
    
  /** Constructors.
   */
  LCPayload() ;
  LCPayload(const EVENT::LCGenericObject* obj);
    
  LCPayload(const Payload &c) ;
  LCPayload(const Payload &p,IMPL::LCGenericObjectImpl & obj) ;

  /// Destructor.
  virtual ~LCPayload() {}
    
  //virtual int id() { return simpleUID() ; }
    
  /** Update values within the object from an LCGenericObject
   */
  virtual void update(const EVENT::LCGenericObject* obj);
    
  /** Create a new LCGenericObject and fill it
   */
  virtual EVENT::LCGenericObject* output() const;
    
  /** Update values within the object from a flat file
   */
  virtual void update(const std::string &fileName);
    
  /** Create a new flat file and fill it
   */
  virtual void output(const std::string &fileName) const;
    
    
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
LCPayload<Payload>::LCPayload() {
}
  
template <class Payload>
LCPayload<Payload>::LCPayload(const EVENT::LCGenericObject* obj) {
  update(obj);
}

template <class Payload>
LCPayload<Payload>::LCPayload(const Payload &p)
  : _payload(p) {
}

template <class Payload>
LCPayload<Payload>::LCPayload(const Payload &p, IMPL::LCGenericObjectImpl & obj)
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
    //     std::cout << "doubleVal[" << i << "] = " << getDoubleVal(i) << std::endl;
  }
  assert (obj.getNDouble() == Payload::numberOfDoubles);
    
}

template <class Payload>
void LCPayload<Payload>::update(const EVENT::LCGenericObject* obj) {
  if (getNInt()!=obj->getNInt())
    {
      std::cout<<" \n getNInt()="<<getNInt()<<", obj->getNInt()="<<obj->getNInt()<<std::endl;
    }
  assert(getNInt()==obj->getNInt());
  for(int i(0);i<obj->getNInt();i++) {
    setIntVal(i,obj->getIntVal(i));
  }
    
  if (getNFloat()!=obj->getNFloat())
    {
      std::cout<<"\n getNFloat()="<<getNFloat()<<", obj->getNFloat()="<<obj->getNFloat()<<std::endl;
    }
  assert(getNFloat()==obj->getNFloat());
  for(int i(0);i<obj->getNFloat();i++) {
    setFloatVal(i,obj->getFloatVal(i));
  }
    
  if (getNDouble()!=obj->getNDouble())
    {
      std::cout<<" getNDouble()="<<getNDouble()<<", obj->getNDouble()="<<obj->getNDouble()<<std::endl;
    }
  assert(getNDouble()==obj->getNDouble());
  for(int i(0);i<obj->getNDouble();i++) {
    setDoubleVal(i,obj->getDoubleVal(i));
  }
}

template <class Payload>
EVENT::LCGenericObject* LCPayload<Payload>::output() const {
  IMPL::LCGenericObjectImpl *obj(new IMPL::LCGenericObjectImpl());
    
  for(int i(0);i<getNInt();i++) {
    obj->setIntVal(i,getIntVal(i));
  }
  assert(getNInt()==obj->getNInt());
    
  for(int i(0);i<getNFloat();i++) {
    obj->setFloatVal(i,getFloatVal(i));
  }
  assert(getNFloat()==obj->getNFloat());
    
  for(int i(0);i<getNDouble();i++) {
    obj->setDoubleVal(i,getDoubleVal(i));
  }
  assert(getNDouble()==obj->getNDouble());
    
  return obj;
}

template <class Payload>
void LCPayload<Payload>::update(const std::string &fileName) {

  std::ifstream fin(fileName.c_str());
  assert(fin);

  int n(0);

  fin >> n;
  assert(fin);
  assert(n==getNInt());

  fin >> n;
  assert(fin);
  assert(n==getNFloat());

  fin >> n;
  assert(fin);
  assert(n==getNDouble());

  int intValue(0);
  for(int i(0);i<getNInt();i++) {
    fin >> intValue;
    assert(fin);
    setIntVal(i,intValue);
  }

  float floatValue(0.0);
  for(int i(0);i<getNFloat();i++) {
    fin >> floatValue;
    assert(fin);
    setFloatVal(i,floatValue);
  }

  double doubleValue(0.0);
  for(int i(0);i<getNDouble();i++) {
    fin >> doubleValue;
    assert(fin);
    setDoubleVal(i,doubleValue);
  }

  fin.close();
}

template <class Payload>
void LCPayload<Payload>::output(const std::string &fileName) const {

  std::ofstream fout(fileName.c_str());
  assert(fout);

  fout << getNInt() << " " 
       << getNFloat() << " "
       << getNDouble() << std::endl;

  for(int i(0);i<getNInt();i++) {
    fout << std::setw(10) << getIntVal(i) << std::endl;
  }
  
  for(int i(0);i<getNFloat();i++) {
    fout << std::setw(15)<< std::setprecision(8) << getFloatVal(i) << std::endl;
  }
  
  for(int i(0);i<getNDouble();i++) {
    fout << std::setw(20) << std::setprecision(12) << getDoubleVal(i) << std::endl;
  }

  fout.close();
}

template <class Payload>
int LCPayload<Payload>::getNInt() const{
  return Payload::numberOfInts ;
}
  
template <class Payload>
int LCPayload<Payload>::getNFloat() const{
  return Payload::numberOfFloats ;
}
  
template <class Payload>
int LCPayload<Payload>::getNDouble() const{
  return Payload::numberOfDoubles ;
}
  
template <class Payload>
int LCPayload<Payload>::getIntVal(int index) const{
  assert(index<Payload::numberOfInts);
  return *(_payload.intData()+index) ;
}
  
template <class Payload>
float LCPayload<Payload>::getFloatVal(int index) const{
  assert(index<Payload::numberOfFloats);
  return *(_payload.floatData()+index) ;
}
  
template <class Payload>
double LCPayload<Payload>::getDoubleVal(int index) const{
  assert(index<Payload::numberOfDoubles);
  return *(_payload.doubleData()+index) ;
}

template <class Payload>
bool LCPayload<Payload>::isFixedSize() const{
  return true ;
}
  
template <class Payload>
void LCPayload<Payload>::setIntVal(unsigned index, int value) {
  if(index < Payload::numberOfInts)
    *(_payload.intData()+index)=value ;
}
  
template <class Payload>
void LCPayload<Payload>::setFloatVal(unsigned index, float value){
  if(index < Payload::numberOfFloats)
    *(_payload.floatData()+index)=value ;
}
  
template <class Payload>
void LCPayload<Payload>::setDoubleVal(unsigned index, double value){
  if(index < Payload::numberOfDoubles)
    *(_payload.doubleData()+index)=value ;
}
  
template <class Payload>
const std::string  LCPayload<Payload>::getTypeName() const {
  return "LCPayload" ;
}

template <class Payload>
const std::string  LCPayload<Payload>::getDataDescription() const {
  return "Constants needed for TBTrack";
}
  
template <class Payload>
const Payload& LCPayload<Payload>::constants() const {
  return _payload ;
}
  
template <class Payload>
void LCPayload<Payload>::constants(const Payload &p) {
  _payload=p ;
}
  
template <class Payload>
const Payload& LCPayload<Payload>::payload() const {
  return _payload ;
}
  
template <class Payload>
void LCPayload<Payload>::payload(const Payload &p) {
  _payload=p ;
}
  

#endif
