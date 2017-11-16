#include "Ahc2TriggerSim.hh"

#include <iostream>
#include <sstream>

#include <marlin/Exceptions.h>

#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/LCParameters.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/MCParticle.h"
#include "IMPL/LCFlagImpl.h"
#include "UTIL/LCTypedVector.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE
{

  Ahc2TriggerSim aAhc2TriggerSim;

  Ahc2TriggerSim::Ahc2TriggerSim() : Processor("Ahc2TriggerSim")
  {

    // register steering parameters: name, description, class-variable, default value

    registerProcessorParameter("DetectorName",
    "Name of the detector",
    _DetectorName,
    std::string("Ahc2"));

    registerProcessorParameter("TriggerMode",
    "Mode for trigger MC, 100 for 100x100 2 fold coincidence, 500 for 500x500 2 fold coincidence",
    _simTriggerModeParameter,
    int(100));

  }

  /************************************************************************************/

  void Ahc2TriggerSim::init()
  {
    printParameters();

    _nRun = 0 ;
    _nEvt = 0 ;

    _nSc1_100x100   = 0;
    _nSc2_100x100   = 0;
    _nSc1_500x500   = 0;
    _nSc2_500x500   = 0;
    _nBeamTrigger   = 0;

    if ( _simTriggerModeParameter == 100 ) _simTriggerMode = ETriggerSimMode::kSimTriggerMode100x100Coincidence;
    else if ( _simTriggerModeParameter == 500 ) _simTriggerMode = ETriggerSimMode::kSimTriggerMode500x500Coincidence;
    else {
      streamlog_out(ERROR) << "Undefined trigger mode: " << _simTriggerModeParameter << " -- STOP" << std::endl;
      throw marlin::SkipEventException(this);
    }

    std::cout << " _simTriggerMode " << _simTriggerMode << std::endl;
  }

  /************************************************************************************/
  void Ahc2TriggerSim::processRunHeader( LCRunHeader* run)
  {
    _nRun++ ;
  }

  void Ahc2TriggerSim::processEvent( LCEvent * evt )
  {
    bool isMC = false;

    int NSC1 = 0;
    int NSC2 = 0;
    int NSC3 = 0;
    int NSC4 = 0;

    typedef const std::vector<std::string> StringVec ;
    StringVec* strVec = evt->getCollectionNames();
    for( StringVec::const_iterator name = strVec->begin() ; name != strVec->end() ; name++)
    {
      LCCollection* col = evt->getCollection( *name ) ;
      std::string sss = name->c_str();

      /* Check trigger counters*/
      if(sss == "MCParticle") isMC = true;

      if(sss.find("scintSD") < 99)
      {
        float Esum = 0;
        int nHits = col->getNumberOfElements();

        for( int i = 0; i < nHits; i++ )
        {
          if(  col->getTypeName() == LCIO::SIMCALORIMETERHIT )
          {
            SimCalorimeterHit* hit = dynamic_cast<SimCalorimeterHit*>( col->getElementAt( i ) ) ;
            Esum += hit->getEnergy();
          }
        }

        if(Esum > 0.0005 && sss.find("scintSD_100x100_front") < 99)
        NSC1++;
        if(Esum > 0.0005 && sss.find("scintSD_100x100_back") < 99)
        NSC2++;
        if(Esum > 0.0005 && sss.find("scintSD_500x500_front") < 99)
        NSC3++;
        if(Esum > 0.0005 && sss.find("scintSD_500x500_back") < 99)
        NSC4++;
      }
    }

    Ahc2TriggerBits trigger;

    if(_DetectorName.find("Ahc2_July15") < 99)
    {
      if(NSC1 > 0)
      {
        trigger.setSc1_100x100();
        _nSc1_100x100++;
      }

      if(NSC2 > 0)
      {
        trigger.setSc2_100x100();
        _nSc2_100x100++;
      }

      if(NSC3 > 0)
      {
        trigger.setSc1_500x500();
        _nSc1_500x500++;
      }

      if(NSC4 > 0)
      {
        trigger.setSc2_500x500();
        _nSc2_500x500++;
      }
    }

    if ( ( ( _simTriggerMode == ETriggerSimMode::kSimTriggerMode100x100Coincidence ) && ( trigger.isSc1_100x100() && trigger.isSc2_100x100() ) ) || ( ( _simTriggerMode == ETriggerSimMode::kSimTriggerMode500x500Coincidence ) && ( trigger.isSc1_500x500() && trigger.isSc2_500x500() ) ) )
    {
      trigger.setBeamTrigger();
      _nBeamTrigger++;
    }

    streamlog_out(MESSAGE) << "Trigger Bits " << trigger << "(" << trigger.getInt() << ")" << endl;

    evt->parameters().setValue("BeamTrigger", trigger.isBeamTrigger());
    evt->parameters().setValue("TriggerValue", trigger.getInt());
    _nEvt ++ ;
  }

  /************************************************************************************/

  void Ahc2TriggerSim::check( LCEvent * evt )
  {
    // nothing to check here - could be used to fill checkplots in reconstruction processor
  }

  /************************************************************************************/
  void Ahc2TriggerSim::end()
  {
    std::cout << "TriggerSim::end()  " << name()
    << " processed " << _nEvt << " events in " << _nRun << " runs "
    << std::endl ;

    if(_nSc1_100x100>0) std::cout << " Sc1_100x100 trigger set " << _nSc1_100x100 << " times" << std::endl;
    if(_nSc2_100x100>0) std::cout << " Sc2_100x100 trigger set " << _nSc2_100x100 << " times" << std::endl;
    if(_nSc1_500x500>0) std::cout << " Sc1_500x500 trigger set " << _nSc1_500x500 << " times" << std::endl;
    if(_nSc2_500x500>0) std::cout << " Sc2_500x500 trigger set " << _nSc2_500x500 << " times" << std::endl;
    std::cout << " Beam trigger set " << _nBeamTrigger << " times" << std::endl;
  }


  /************************************************************************************/
  void Ahc2TriggerSim::printParameters(){
    std::cerr << "============= Ahc2TriggerSim Processor =================" <<std::endl;
    std::cerr << "Trigger for MC" <<std::endl;
    std::cerr << "Detector Name: " << _DetectorName <<std::endl;
    std::cerr << "TriggerMode : " << _simTriggerModeParameter << std::endl;
    std::cerr << "=======================================================" <<std::endl;
    return;

  }
}
