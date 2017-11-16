#ifndef __BeamMetaData_hh__
#define __BeamMetaData_hh__

#include "EVENT/LCGenericObject.h"
#include "IMPL/LCGenericObjectImpl.h"

namespace CALICE {
  
  class BeamMetaData : public IMPL::LCGenericObjectImpl {
  public:
    BeamMetaData( const int& pdgCode, const float& energy );
    BeamMetaData( LCGenericObject* obj );

    const int getPdgCode() const;
    const float getEnergy() const;

    void setPdgCode( const int& pdgCode );
    void setEnergy( const float& energy );

  protected:
    enum Ints { kPdgCode, kNInts };
    enum Floats { kEnergy, kNFloats };
    enum Doubless { kNDoubles };
    BeamMetaData();
    void set( LCGenericObject* obj );

  private:
    int _pdgType;
    float _energy;
    
  };  // class BeamMetaData
  
}  // namespace CALICE

#endif
