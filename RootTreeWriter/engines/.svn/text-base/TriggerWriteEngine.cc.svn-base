#include "TriggerWriteEngine.hh"

#include "collection_names.hh"
#include "TriggerBits.hh"
#include "TcmtEventBits.hh"

#include <cfloat>

using namespace CALICE;
using namespace std;


#define DDEBUG(name) ;std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) ;std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  void TriggerWriteEngine::registerParameters()
  {
    
    _hostProcessor.relayRegisterProcessorParameter("TriggerConfigurationName",
						   "Name of the event parameter name which contains the trigger configuration bits.",
						   _parNameTriggerConf,
						   std::string(PAR_TRIGGER_CONF));

    _hostProcessor.relayRegisterProcessorParameter("TriggerEventName",
						   "Name of the event parameter name which contains the current trigger main word .",
						   _parNameTriggerEvent,
						   std::string(PAR_TRIGGER_EVENT));

    _hostProcessor.relayRegisterProcessorParameter("MultiBitName",
						   "Name of the event parameter name which contains the current veto bit .",
						   _parNameMultiBit,
						   std::string(PAR_MULTI_BIT));

    _hostProcessor.relayRegisterProcessorParameter("MultiAmplitudeName",
						   "Name of the event parameter name which contains the current veto adc counts .",
						   _parNameMultiAmplitude,
						   std::string(PAR_MULTI_AMPL));

  }

  void TriggerWriteEngine::registerBranches( TTree* hostTree )
  {

    hostTree->Branch("multiADC",    &_trigBits.multiADC,  "multiADC/F");
    hostTree->Branch("multiBit",    &_trigBits.multi,    "multiBit/I");
    hostTree->Branch("beamBit",     &_trigBits.beam,     "beamBit/I");
    hostTree->Branch("spillBit",    &_trigBits.spill,    "spillBit/I");
    hostTree->Branch("pedestalBit", &_trigBits.pedestal, "pedestalBit/I");
    hostTree->Branch("purePedBit",  &_trigBits.purePed,  "purePedBit/I");
    hostTree->Branch("cosmicsBit",  &_trigBits.cosmics,  "cosmicsBit/I");
    hostTree->Branch("calibBit",    &_trigBits.calib,    "calibBit/I");
    hostTree->Branch("cherenkowBit",&_trigBits.cherenkow,"cherenkowBit/I");
    hostTree->Branch("cherenkow2Bit",&_trigBits.cherenkow2,"cherenkow2Bit/I");
    hostTree->Branch("a3x3Bit",     &_trigBits.a3x3,     "a3x3Bit/I");
    hostTree->Branch("b3x3Bit",     &_trigBits.b3x3,     "b3x3Bit/I");
    hostTree->Branch("a10x10Bit",   &_trigBits.a10x10,   "a10x10Bit/I");
    hostTree->Branch("a20x20Bit",   &_trigBits.a20x20,   "a20x20Bit/I");
    hostTree->Branch("b10x10Bit",   &_trigBits.b10x10,   "b10x10Bit/I");
    hostTree->Branch("a100x100Bit", &_trigBits.a100x100, "a100x100Bit/I");
    hostTree->Branch("b100x100Bit", &_trigBits.b100x100, "b100x100Bit/I");
    hostTree->Branch("vetoBit",     &_trigBits.veto,     "vetoBit/I");
    hostTree->Branch("tcmtEventBit",&_trigBits.tcmtEventBit,"tcmtEventBit/I");

  }


  void TriggerWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    /// \todo fixme: check whether tigger paramter is attached to the event.
    //    TriggerBits triggerConf = evt->getParameters().getIntVal(_parNameTriggerConf);
    TriggerBits triggerEvent = evt->getParameters().getIntVal(_parNameTriggerEvent);        

    _trigBits.multiADC  = evt->getParameters().getFloatVal(_parNameMultiAmplitude);
    _trigBits.multi     = evt->getParameters().getIntVal(_parNameMultiBit);
    _trigBits.beam      = triggerEvent.isBeamTrigger();
    _trigBits.spill     = triggerEvent.isSpill();
    _trigBits.pedestal  = triggerEvent.isPedestalTrigger();
    _trigBits.purePed   = triggerEvent.isPurePedestalTrigger();
    _trigBits.calib     = triggerEvent.isCalibTrigger();
    _trigBits.cosmics   = triggerEvent.isCosmicsTrigger();
    _trigBits.cherenkow = triggerEvent.isCherenkovTrigger();
    _trigBits.cherenkow2 = triggerEvent.isCherenkov2Trigger();
    _trigBits.a3x3      = triggerEvent.isSc1_3x3();
    _trigBits.b3x3      = triggerEvent.isSc2_3x3();
    _trigBits.a10x10    = triggerEvent.isSc1_10x10();
    _trigBits.a20x20    = triggerEvent.isSc1_20x20();
    _trigBits.b10x10    = triggerEvent.isSc2_10x10();
    _trigBits.a100x100  = triggerEvent.isSc1_100x100();
    _trigBits.b100x100  = triggerEvent.isSc2_100x100();
    _trigBits.veto      = triggerEvent.isVetoUL()+triggerEvent.isVetoUR()
                         +triggerEvent.isVetoDL()+triggerEvent.isVetoDR();

    /*---------------------------------------------------------------------*/
    const int tcmtTriggerValue = evt->getParameters().getIntVal("TcmtEventBit");
    TcmtEventBits tcmtBit(tcmtTriggerValue);
    _trigBits.tcmtEventBit = tcmtBit.isMuon();
    /*---------------------------------------------------------------------*/
    
    
  }

}//namespace marlin
