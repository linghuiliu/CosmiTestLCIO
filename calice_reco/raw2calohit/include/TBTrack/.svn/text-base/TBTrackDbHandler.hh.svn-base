#ifndef TBTrackDbHandler_hpp
#define TBTrackDbHandler_hpp

#include <string>
#include <fstream>

#include "TBTrackBaseProcessor.hh"

using namespace lcio ;
using namespace marlin ;

class TBTrackDbHandler : public TBTrackBaseProcessor 
{
public:
  
  virtual Processor*  newProcessor() { return new TBTrackDbHandler ; }
  
  TBTrackDbHandler() ;
  
  virtual void Init(){} ;
  virtual void ProcessRunHeader( LCRunHeader* run ) ;
  virtual void ProcessEvent( LCEvent * evt ) ; 
  virtual void End(){} ;

 private:

} ;

#endif



