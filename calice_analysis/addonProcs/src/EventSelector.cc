#include <EventSelector.hh>


#include <TriggerBits.hh>
#include <EVENT/LCParameters.h>
#include <collection_names.hh>
#include <marlin/Exceptions.h>

namespace CALICE {

  EventSelector aEventSelector;

  EventSelector::EventSelector() : Processor("eventSelector")
  {
    _description = "This processor selects events with special triggers. \n\
                  pedestal=P, cosmic=Co, calibration=Ca, beam=B, cherenkov=Ch, cherenkov2=Ch2, veto=V, vetoUpLeft=Vul, vetoUpRight=Vur, \
                  vetoDownLeft=Vdl, vetoDownRight=Vdr, multiplicityBit = multiBit, t3x3a=t3a, t3x3b=t3b, t10x10a=t10a, \
                  t10x10b=t10b, t100x100a=t100a, t100x100b=t100b, spill=S, generic=G, oscillator=O, external=X, \
                  EMC-TrackBit=TrackEmc, auxiliary bit 1 = aux1, auxiliary bit 2 = aux2, auxiliary bit 3 = aux3, auxiliary bit 4 = aux4, auxiliary bit 5 = aux5";

    registerProcessorParameter("TriggerConfigurationName",
                               "Name of the event parameter name which contains the trigger configuration bits.",
                               _parNameTriggerConf,
                               std::string(PAR_TRIGGER_CONF));

    registerProcessorParameter("TriggerEventName",
                               "Name of the event parameter name which contains the current trigger main word .",
                               _parNameTriggerEvent,
                               std::string(PAR_TRIGGER_EVENT));

    registerProcessorParameter("MultiBitName",
                               "Name of the event parameter name which contains the current multiplicity bit .",
                               _parNameMultiBit,
                               std::string(PAR_MULTI_BIT));

    registerProcessorParameter("EmcTrackBitName",
                               "Name of the event parameter name which contains the current ECAL track bit .",
                               _parNameEmcTrackBit,
                               std::string("EmcTrackBit"));

    registerProcessorParameter("Aux_1_BitName",
                               "Name of the event parameter (must be of type INT) which contains the current auxiliary bit (1).",
                               _parNameAuxBit_1,
                               std::string(""));

    registerProcessorParameter("Aux_2_BitName",
                               "Name of the event parameter (must be of type INT) which contains the current auxiliary bit (2).",
                               _parNameAuxBit_2,
                               std::string(""));

    registerProcessorParameter("Aux_3_BitName",
                               "Name of the event parameter (must be of type INT) which contains the current auxiliary bit (3).",
                               _parNameAuxBit_3,
                               std::string(""));

    registerProcessorParameter("Aux_4_BitName",
                               "Name of the event parameter (must be of type INT) which contains the current auxiliary bit (4).",
                               _parNameAuxBit_4,
                               std::string(""));

    registerProcessorParameter("Aux_5_BitName",
                               "Name of the event parameter (must be of type INT) which contains the current auxiliary bit (5).",
                               _parNameAuxBit_5,
                               std::string(""));

    registerProcessorParameter("SkipEventNumber", 
                               "skip the first pedestal configuration event, default event number is 500",
                               _skipEventNumber,
                               (int) 0);

    registerProcessorParameter("doSkipEvent", "flag for Skip the events, the Number was given by SkipEventNumber",
                               _doSkipEvent,
                               (bool) false);

    StringVec defaultStringVec;
    defaultStringVec.push_back("");

    registerOptionalParameter("withTrigger","trigger flags which are required",_withTriggers,defaultStringVec);
    registerOptionalParameter("withoutTrigger","trigger flags which are forbidden",_withoutTriggers,defaultStringVec);


  }

  void EventSelector::init() {

    printParameters();
    _nEvent = 0;
  }

  void EventSelector::checkTrigger(bool trigger,std::string triggerString) {
    for (unsigned i=0;i<_withTriggers.size();i++)
      if (_withTriggers[i]==triggerString && !trigger)
        throw marlin::SkipEventException(&aEventSelector);
    for (unsigned i=0;i<_withoutTriggers.size();i++)
      if (_withoutTriggers[i]==triggerString && trigger)
        throw marlin::SkipEventException(&aEventSelector);
  }


  void EventSelector::processEvent(LCEvent *evt) {

    int evtNumber = evt->getEventNumber();
    //Skip the first 500 empty noise event 
    if( _doSkipEvent && evtNumber < _skipEventNumber ) throw marlin::SkipEventException(this);

    TriggerBits triggerConf = evt->getParameters().getIntVal(_parNameTriggerConf);
    TriggerBits triggerEvent = evt->getParameters().getIntVal(_parNameTriggerEvent);
    int multiBit = evt->getParameters().getIntVal(_parNameMultiBit);
    int emcTrackBit = evt->getParameters().getIntVal(_parNameEmcTrackBit);

    // get auxiliary bits from event parameters if defined
    int auxBit_1 = 0;
    int auxBit_2 = 0;
    int auxBit_3 = 0;
    int auxBit_4 = 0;
    int auxBit_5 = 0;

    if ( parameterSet("Aux_1_BitName") )
      auxBit_1 = evt->getParameters().getIntVal(_parNameAuxBit_1);
    if ( parameterSet("Aux_2_BitName") )
      auxBit_2 = evt->getParameters().getIntVal(_parNameAuxBit_2);
    if ( parameterSet("Aux_3_BitName") )
      auxBit_3 = evt->getParameters().getIntVal(_parNameAuxBit_3);
    if ( parameterSet("Aux_4_BitName") )
      auxBit_4 = evt->getParameters().getIntVal(_parNameAuxBit_4);
    if ( parameterSet("Aux_5_BitName") )
      auxBit_5 = evt->getParameters().getIntVal(_parNameAuxBit_5);


    // the trigger handler knows what the bits mean --> so lets do a little effort to decouple our index from the trigger bits
    bool pedestal = triggerEvent.isPedestalTrigger();
    bool cosmic = triggerEvent.isCosmicsTrigger();
    bool calibration = triggerEvent.isCalibTrigger();
    bool beam = triggerEvent.isBeamTrigger();
    bool cherenkov = triggerEvent.isCherenkovTrigger();
    bool cherenkov2 = triggerEvent.isCherenkov2Trigger();
    bool veto = triggerEvent.isVetoTrigger();
    bool t3x3a = triggerEvent.isSc1_3x3();
    bool t3x3b = triggerEvent.isSc2_3x3();
    bool t10x10a = triggerEvent.isSc1_10x10();
    bool t10x10b = triggerEvent.isSc2_10x10();
    bool t20x20a = triggerEvent.isSc1_20x20();
    bool t100x100a = triggerEvent.isSc1_100x100();
    bool t100x100b = triggerEvent.isSc2_100x100();
    bool spill = triggerEvent.isSpill();
    bool generic = triggerEvent.isGeneric();
    bool oscillator = triggerEvent.isCrcOscillator();
    bool external = triggerEvent.isExternalPulser();
    bool multipl  = (bool)multiBit;
    bool vetoUL = triggerEvent.isVetoUL();
    bool vetoUR = triggerEvent.isVetoUR();
    bool vetoDL = triggerEvent.isVetoDL();
    bool vetoDR = triggerEvent.isVetoDR();
    bool emcTrack = (bool)emcTrackBit;
    bool aux1 = (bool)auxBit_1;
    bool aux2 = (bool)auxBit_2;
    bool aux3 = (bool)auxBit_3;
    bool aux4 = (bool)auxBit_4;
    bool aux5 = (bool)auxBit_5;


    checkTrigger( pedestal,    "P");
    checkTrigger( cosmic,      "Co");
    checkTrigger( calibration, "Ca");
    checkTrigger( beam,        "B");
    checkTrigger( cherenkov,   "Ch");
    checkTrigger( cherenkov2,  "Ch2");
    checkTrigger( veto,        "V");
    checkTrigger( t3x3a,       "t3a");
    checkTrigger( t3x3b,       "t3b");
    checkTrigger( t10x10a,     "t10a");
    checkTrigger( t10x10b,     "t10b");
    checkTrigger( t20x20a,     "t20a");
    checkTrigger( t100x100a,   "t100a");
    checkTrigger( t100x100b,   "t100b");
    checkTrigger( spill,       "S");
    checkTrigger( generic,     "G");
    checkTrigger( oscillator,  "O");
    checkTrigger( external,    "X");
    checkTrigger( multipl,     "multiBit");
    checkTrigger( vetoUL,      "Vul");
    checkTrigger( vetoUR,      "Vur");
    checkTrigger( vetoDL,      "Vdl");
    checkTrigger( vetoDR,      "Vdr");
    checkTrigger( emcTrack,    "TrackEmc");
    checkTrigger( aux1,        "aux1");
    checkTrigger( aux2,        "aux2");
    checkTrigger( aux3,        "aux3");
    checkTrigger( aux4,        "aux4");
    checkTrigger( aux5,        "aux5");


    _nEvent++;
  }



  void EventSelector::end() {
    std::cout << name() << " -- Report: " << std::endl;
    std::cout << "  found " << _nEvent << " which matched the trigger requirements" << std::endl;
  }


}
