#include "TdcIndex.hh"

#ifdef TDCINDEX_RANGE_CHECK
#include <stdexcept>
#endif

namespace CALICE {

  TdcIndex::TdcIndex() : _index(0) {
  }

  TdcIndex::TdcIndex( const unsigned int module, 
		      const unsigned int channel ){
    set( module, channel );
  }

  TdcIndex::TdcIndex( const unsigned int index ) : _index( index ) {
  }

  TdcIndex::~TdcIndex(){
  }

  const unsigned int TdcIndex::tdcModule() const {
    return ( _index >> TDCINDEX_SHIFT_MODULE );
  }

  const unsigned int TdcIndex::tdcChannel() const {
    return ( _index & TDCINDEX_MASK_CHANNEL );
  }

  void TdcIndex::set( const unsigned int module,
		      const unsigned int channel ) {
#ifdef TDCINDEX_RANGE_CHECK
    if ( module > TDCINDEX_MASK_MODULE )
      throw std::range_error
	( "Value too large for tdcModule field in TdcIndex" );
    if ( channel > TDCINDEX_MASK_CHANNEL )
      throw std::range_error
	( "Value too large for tdcChannel field in TdcIndex" );
#endif
    _index = ( module << TDCINDEX_SHIFT_MODULE ) + channel;
  }  // void TdcIndex::set

}  // namespace CALICE
