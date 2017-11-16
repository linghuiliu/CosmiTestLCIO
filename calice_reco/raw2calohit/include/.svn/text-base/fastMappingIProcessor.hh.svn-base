#ifndef fastMappingIProcessor_h 
#define fastMappingIProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif
 
#include "lcio.h"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include <VRawADCValueProcessor.hh>

// #define HCALRECO_DEBUG

namespace CALICE { 

/*** A class which converts ADChits into intermediate CaliceHits which can be used
   * for calibration, etc. 
   * @author S.Schmidt DESY
   * @date July 31 2006
   */

class fastMappingIProcessor : public VRawADCValueProcessor {

  public: 
    
    virtual Processor* newProcessor() { return new fastMappingIProcessor; }
    
    fastMappingIProcessor();
    
    virtual void init();
    
    virtual void processRunHeader(LCRunHeader* run);
    
    virtual void processEvent(LCEvent* evt);
    
    virtual void check(LCEvent* evt);
    
    virtual void end();
    
  protected:
      
    std::string _outputColName;
    int _viewConnectionTree;
    int _pickModule;
    
};

}

#endif
