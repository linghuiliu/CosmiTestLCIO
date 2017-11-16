#ifndef __TRIGGERWRITEENGINE_HH__
#define __TRIGGERWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"


#include <string>

#include "TTree.h"
#include "TH2.h"

namespace marlin
{

  ///\ingroup calice_engines
  class TriggerWriteEngine: public RootWriteEngine
  {
  public:
    TriggerWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
					    _engineName("TriggerWriteEngine")
    {}
    
    virtual const std::string& getEngineName()
    { return _engineName; }

//    virtual RootWriteEngine* newEngine( Processor* proc)
//    {
//      _parentProcessor = proc;
//      return new TriggerWriteEngine ;
//    }

    virtual void registerParameters();
    virtual void registerBranches( TTree* );
    virtual void fillVariables( EVENT::LCEvent* );


    // tree variables
    struct TrigBits_t
    {
      Float_t multiADC;
      Int_t multi;
      Int_t beam;
      Int_t spill;
      Int_t pedestal;
      Int_t purePed;
      Int_t calib;
      Int_t cosmics;
      Int_t cherenkow;
      Int_t cherenkow2;
      Int_t a3x3;
      Int_t b3x3;
      Int_t a10x10;
      Int_t a20x20;
      Int_t b10x10;
      Int_t a100x100;
      Int_t b100x100;
      Int_t veto;
      Int_t tcmtEventBit;
    } _trigBits;

  private:
    std::string _engineName;
    std::string _enginePrefix;

    // processor parameters

    std::string _parNameTriggerConf;
    std::string _parNameTriggerEvent;
    std::string _parNameMultiBit;
    std::string _parNameMultiAmplitude;

    
    



  };

}//namespace marlin

#endif // __TRIGGERWRITEENGINE_HH__
