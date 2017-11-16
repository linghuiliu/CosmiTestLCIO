#ifndef __AHCTcmtEntry_hh__
#define __AHCTcmtEntry_hh__ 1

#include "AHCEntry.hh"

class AHCTcmtEntry : public AHCEntry {
public:
  AHCTcmtEntry( const std::string& line );
  ~AHCTcmtEntry();

  virtual void init( const std::string& line );
  virtual AHCTcmtEntry* clone() const { return new AHCTcmtEntry( *this ); }

protected:
  unsigned int _slot;
  unsigned int _fe;
  std::string _dacFileName;
  unsigned int _holdExt;
  unsigned int _holdLedCm;
  unsigned int _holdLedPm;
  unsigned int _vcalibCm;
  unsigned int _vcalibPm;

};  // class AHCTcmtEntry

#endif

