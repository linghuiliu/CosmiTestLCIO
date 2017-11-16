#ifndef _ReadDaqMap_H_
#define _ReadDaqMap_H_

#include <stdexcept>
#include <string>
#include <map>
#include <UTIL/LCTime.h>

using namespace std;
#include <FeConnectionDef_t.hh>

class ReadDaqMap 
{
public:
  ReadDaqMap(const char *file_name) throw(runtime_error);
  void ShowChannelMap() const ;

  typedef map< UInt_t, FeConnectionDef_t> FeMap_t;
  typedef map< UInt_t, FeMap_t > SlotMap_t;
  
  const SlotMap_t &getMapping() const {return _mapping;};
  const UTIL::LCTime &since() const {return _since;};
  const UTIL::LCTime &till() const {return _till;};
  UInt_t crate() const {return _crate;};
private:
  UInt_t _crate;
  SlotMap_t _mapping;
  UTIL::LCTime _since;
  UTIL::LCTime _till;
};

#endif
