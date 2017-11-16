#ifndef __AHCPmtEntry_hh__
#define __AHCPmtEntry_hh__ 1

#include "AHCEntry.hh"

class AHCPmtEntry : public AHCEntry {
public:
  AHCPmtEntry( const std::string& line );
  ~AHCPmtEntry();

  virtual void init( const std::string& line );
  virtual AHCPmtEntry* clone() const { return new AHCPmtEntry( *this ); }

protected:
  unsigned int _slot;
  unsigned int _fe;
  unsigned int _hold;

};  // class AHCPmtEntry

#endif

