#ifndef __AHCReader_hh__
#define __AHCReader_hh__ 1

#include <set>
#include "AHCEntry.hh"

class AHCReader : public std::set< AHCEntry* >{
 public:

  AHCReader();
  AHCReader( const char* fname );
  ~AHCReader();

  AHCReader& read( const char* fname );
  AHCReader& reset();

};  // class AHCReader

#endif
