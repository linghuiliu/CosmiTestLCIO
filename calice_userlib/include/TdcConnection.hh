#ifndef __TdcConnection_hh__
#define __TdcConnection_hh__

#include "UTIL/LCFixedObject.h"

#include "TdcIndex.hh"
#include "DCIndex.hh"

namespace CALICE {
  
  class TdcConnection : public UTIL::LCFixedObject<2,0,0> {
  public:
    TdcConnection();
    TdcConnection( EVENT::LCObject* );
    TdcConnection( const TdcIndex&, const DCIndex& );
    ~TdcConnection();

    unsigned int getTdcIndex();
    int getDCIndex();

    const std::string getTypeName() const { 
      return "TdcConnection"; 
    }

    const std::string getDataDescription() const {
      return "i:TdcIndex, i:DCIndex";
    }

  };  // class TdcConnection

};  // namespace CALICE

#endif
