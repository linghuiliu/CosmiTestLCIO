#ifndef __AHCPinEntry_hh__
#define __AHCPinEntry_hh__ 1

#include "AHCEntry.hh"

class AHCPinEntry : public AHCEntry {
public:
  AHCPinEntry( const std::string& line );
  ~AHCPinEntry();

  virtual void init( const std::string& line );
  virtual AHCPinEntry* clone() const { return new AHCPinEntry( *this ); }

protected:
  unsigned int _slot;
  unsigned int _fe;
  unsigned int _hold;
  unsigned int _hbab;

};  // class AHCPinEntry

#endif

