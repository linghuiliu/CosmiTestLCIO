#include <lcio.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCObject.h>
#include <RtypesSubSet.h>

using namespace lcio;

namespace CALICE {

/** Utility class to clone a LCGenericObject this is needed is
  * needed since during the writing of CALICE conditions data
  * a full collection has to be memorized until a new set of 
  * this datatype appears.
  * @author Götz Gaycken LLR (Ecole Polytechnique)
  * @date Sep. 2005     
  */

  class LCGenericObjectImplCloner : public LCGenericObjectImpl
  {
  public:
    LCGenericObjectImplCloner(const LCGenericObject &a) : LCGenericObjectImpl(a.getNInt(),a.getNFloat(),a.getNDouble()) {
      for (UInt_t element_i=0; element_i< static_cast<UInt_t>(a.getNInt()); element_i++) {
	setIntVal(element_i,a.getIntVal(element_i));
      }
      for (UInt_t element_i=0; element_i< static_cast<UInt_t>( a.getNFloat()); element_i++) {
	setFloatVal(element_i,a.getFloatVal(element_i));
      }
      for (UInt_t element_i=0; element_i< static_cast<UInt_t>(a.getNDouble()); element_i++) {
	setDoubleVal(element_i,a.getDoubleVal(element_i));
      }

      _isFixedSize=a.isFixedSize();

      //      _typeName=a.getTypeName();
      //      _dataDescription=a.getDataDescription();
    };
  };

}
