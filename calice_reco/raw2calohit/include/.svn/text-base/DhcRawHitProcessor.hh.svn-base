/* Processor which wraps the CaliceTriggerProcessor*/

#ifndef DHCRAWHITPROCESSOR_h
#define DHCRAWHITPROCESSOR_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h" 
#include "IMPL/LCTOOLS.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "UTIL/LCTime.h"
#include "LCIOTypes.h"
#include "LCIOSTLTypes.h"
#include "marlin/Processor.h"
#include "collection_names.hh"
#include "DhcRawChipContent.hh"


#include <string>


using namespace std;
using namespace lcio ;
//using namespace lccd ;
using namespace marlin ;
//using namespace CALICE;



namespace marlin {

/** Class to process dhc raw hits and to write out DhcRawCalorimeter Hits
  * @author: R. Pöschl DESY
  * @date Dec 7 2005
  *
  */


  class DhcRawHitProcessor : public Processor{
    
  public:
    virtual Processor* newProcessor() { return new DhcRawHitProcessor;}
    DhcRawHitProcessor();
    //  ~DhcRawHitProcessor();
    void init();  
    void processEvent( LCEvent * evt ) ;
    void end();
    
  private:
  };
}
#endif
