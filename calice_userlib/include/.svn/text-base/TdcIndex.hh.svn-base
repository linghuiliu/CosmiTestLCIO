#ifndef __TdcIndex_hh__
#define __TdcIndex_hh__

#ifdef RANGE_CHECK
#ifndef TDCINDEX_RANGE_CHECK
#define TDCINDEX_RANGE_CHECK 1
#endif
#endif

#define TDCINDEX_SHIFT_MODULE 8
#define TDCINDEX_MASK_MODULE 0xFFFFFF
#define TDCINDEX_MASK_CHANNEL 0xFF

#include <iostream>

class TBTrackMapper;

namespace CALICE {
  
  class TdcIndex {
    friend class TdcConnection;
  public:
    TdcIndex();
    TdcIndex( const unsigned int, const unsigned int );
    ~TdcIndex();

    const unsigned int tdcModule() const;
    const unsigned int tdcChannel() const;

    TdcIndex( const unsigned int );
    const unsigned int index() const { return _index; }

  protected:
    void set( const unsigned int, const unsigned int );

  private:
    unsigned int _index;
  };  // class TdcIndex

};  // namespace CALICE

#endif
