#include "BeamMetaData.hh"


namespace CALICE {

  BeamMetaData::BeamMetaData() 
    : IMPL::LCGenericObjectImpl( kNInts, kNFloats, kNDoubles ) 
  {
  }


  BeamMetaData::BeamMetaData( const int& pdgCode, const float& energy ) 
    : IMPL::LCGenericObjectImpl( kNInts, kNFloats, kNDoubles ) 
  {
    setPdgCode( pdgCode );
    setEnergy( energy );
  }
  BeamMetaData::BeamMetaData( EVENT::LCGenericObject* obj ) 
    : IMPL::LCGenericObjectImpl( obj->getNInt(), obj->getNFloat(), obj->getNDouble() ) 
  {
    set( obj );
  }
    

  const int BeamMetaData::getPdgCode() const {
    int pdg = getIntVal( kPdgCode );
    return pdg;
  }


  const float BeamMetaData::getEnergy() const {
    float energy = getFloatVal( kEnergy );
    return energy;
  }


  void BeamMetaData::set( LCGenericObject* obj ) {
    setPdgCode( obj->getIntVal( kPdgCode ) );
    setEnergy( obj->getFloatVal( kEnergy ) );
  }


  void BeamMetaData::setPdgCode( const int& pdgCode ) {
    setIntVal( kPdgCode, pdgCode );
  }


  void BeamMetaData::setEnergy( const float& energy ) {
    setFloatVal( kEnergy, energy );
  }


}  // namespace CALICE
