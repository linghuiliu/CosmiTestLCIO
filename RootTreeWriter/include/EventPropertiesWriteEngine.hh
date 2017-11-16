#ifndef __EVENTPROPERTIESWRITEENGINE_HH__
#define __EVENTPROPERTIESWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"


#include <string>

#include "TTree.h"

namespace marlin
{
  /// This RootTreeWriter engine calculates basic event properties and
  /// adds them to the output tree.
  /// \todo description of event properties calculated in this processor
  class EventPropertiesWriteEngine: public RootWriteEngine
  {
  public:
    
    EventPropertiesWriteEngine( RootTreeWriter* host, std::string engineName = "EventPropertiesWriteEngine" ):
      RootWriteEngine(host), _engineName(engineName) {}


    virtual const std::string& getEngineName()
    { return _engineName; }

//    virtual RootWriteEngine* newEngine( Processor* proc)
//    {
//      mParentProcessor = proc;
//      return new EventPropertiesWriteEngine ;
//    }


    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );

    virtual void fillVariables( EVENT::LCEvent* );


    // tree variables
    struct 
    {
      double sumE;
      double sumP[3];
      double sqrtS;
    } _eventKin;


  private:
    std::string _engineName;
    std::string _enginePrefix;
    // processor parameters
    std::string _recPartColName;
    
  };

}//namespace marlin

#endif // __EVENTPROPERTIESWRITEENGINE_HH__
