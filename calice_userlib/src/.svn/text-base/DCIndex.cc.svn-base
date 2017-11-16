#include "DCIndex.hh"

namespace CALICE {

  DCIndex::DCIndex() : _index(0) {
  }

  DCIndex::DCIndex( const unsigned int layer, 
		    const unsigned int xy, 
		    const bool negative ){
    set( layer, xy, negative );
  }

  DCIndex::DCIndex( const int index ) : _index( index ){
  }

  DCIndex::~DCIndex(){
  }

  const unsigned int DCIndex::DClayer() const {
    return ( ( _index >> DCINDEX_SHIFT_LAYER ) & DCINDEX_MASK_LAYER );
  }

  const unsigned int DCIndex::DCxy() const {
    return ( _index & DCINDEX_MASK_XY );
  }

  void DCIndex::set( const unsigned int layer,
		     const unsigned int xy,
		     const bool negative ){
#ifdef DCINDEX_RANGE_CHECK
    if ( layer > DCINDEX_MASK_LAYER )
      throw std::range_error( "Value too large for field DClayer in DCIndex" );
    if ( xy > DCINDEX_MASK_XY-1  )
      throw std::range_error( "Value too large for field DCxy in DCIndex" );
#endif
    _index = ( layer << DCINDEX_SHIFT_LAYER ) + xy;
    if ( negative ) {
      _index = -_index;
      if ( _index == 0 ) _index = 0x80000000;
    }
  }  // void DCIndex::set

}  // namespace CALICE
