#ifndef __AHCEntry_hh__
#define __AHCEntry_hh__ 1

#include <string>
#include <fstream>

class AHCEntry {
 public:
  virtual ~AHCEntry(){}
  virtual void init( const std::string& line ) = 0;
  virtual AHCEntry* clone() const = 0;

  const std::string type() const { return _type; }

protected:
  std::string _type;
};  // class AHCEntry

#endif
