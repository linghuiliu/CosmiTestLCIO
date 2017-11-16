#include "TdcConnection.hh"

namespace CALICE {

  TdcConnection::TdcConnection() {
  }

  TdcConnection::TdcConnection( const TdcIndex& tdcIndex, 
				const DCIndex& dcIndex ){
    obj()->setIntVal( 0, (int)tdcIndex.index() );
    obj()->setIntVal( 1, dcIndex.index() );
  }

  TdcConnection::TdcConnection( EVENT::LCObject* obj ) : UTIL::LCFixedObject<2,0,0>(obj) {
  }

  TdcConnection::~TdcConnection(){
  }

  unsigned int TdcConnection::getTdcIndex() { 
    return (unsigned int) obj()->getIntVal(0); }

  int TdcConnection::getDCIndex() { 
    return obj()->getIntVal(1); }

}  // namespace CALICE
