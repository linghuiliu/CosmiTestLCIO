#ifndef EventSelector_h
#define EventSelector_h 1

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <lcio.h>
#include "marlin/Processor.h"

using namespace lcio;

namespace CALICE {


  class EventSelector : public marlin::Processor {

  public:
    EventSelector();
    virtual Processor* newProcessor() {return new EventSelector();}
    virtual void init();
    void processEvent(LCEvent *evt);
    void end();

  protected:
    std::string _parNameTriggerConf;
    std::string _parNameTriggerEvent;
    std::string _parNameMultiBit;
    std::string _parNameEmcTrackBit;
    std::string _parNameAuxBit_1;
    std::string _parNameAuxBit_2;
    std::string _parNameAuxBit_3;
    std::string _parNameAuxBit_4;
    std::string _parNameAuxBit_5;

    int _skipEventNumber;
    bool  _doSkipEvent;

    StringVec _withTriggers, _withoutTriggers;


  private:
    void checkTrigger(bool trigger,std::string triggerString);
    unsigned _nEvent;

  };

}

#endif
