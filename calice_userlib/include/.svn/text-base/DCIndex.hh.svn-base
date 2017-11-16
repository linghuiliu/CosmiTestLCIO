#ifndef __DCIndex_hh__
#define __DCIndex_hh__

#define DCINDEX_MASK_XY 0x1
#define DCINDEX_MASK_LAYER 0x7FFFFFF
#define DCINDEX_SHIFT_LAYER 1

#ifdef RANGE_CHECK
#ifndef DCINDEX_RANGE_CHECK
#define DCINDEX_RANGE_CHECK 1
#endif
#endif

namespace CALICE {

  class DCIndex {
    friend class TdcConnection;
  public:
    DCIndex();
    DCIndex( const unsigned int, const unsigned int, const bool );
    ~DCIndex();

    const unsigned int DClayer() const;
    const unsigned int DCxy() const;
    const bool DCnegative() const { return ( _index < 0 ); }

    DCIndex( const int );
    const int index() const { return _index; }

  protected:
    void set( const unsigned int, const unsigned int, const bool );

  private:
    int _index;

  };  // class DCIndex

}  // namespace CALICE

#endif
