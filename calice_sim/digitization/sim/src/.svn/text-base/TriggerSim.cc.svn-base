#include "TriggerSim.hh"

#include <iostream>
#include <sstream>
#include <assert.h>
#include <time.h>

#include <marlin/Exceptions.h>

#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/LCParameters.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/TrackerHit.h"
#include "EVENT/Track.h"
#include "EVENT/MCParticle.h"
#include "IMPL/LCFlagImpl.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "AdcBlock.hh"
#include "EventHeader.hh"
#include "collection_names.hh"
#include "TriggerBits.hh"
#include "ErrorBits.hh"

using namespace lcio;
using namespace marlin;
using namespace CALICE;


TriggerSim aTriggerSim;


TriggerSim::TriggerSim() : Processor("TriggerSim")
{
  _description = "TriggerSim processor sets trigger bits for MC events" ;

  registerProcessorParameter ( "TriggerMode",
                               "The trigger mode used for data taking. Possible values: "
                               "10 for 10x10 2 fold coincidence, "
                               "20 for 20x20 singlee running, "
                               "100 for 100x100 2 fold coincidence",
                               _simTriggerModeParameter,
                               10 );
}


void TriggerSim::init()
{
  /* usually a good idea to*/
  printParameters() ;

  _nRun         = 0;
  _nEvt         = 0;
  _nSc1_10x10   = 0;
  _nSc2_10x10   = 0;
  _nSc1_3x3     = 0;
  _nSc2_3x3     = 0;
  _nSc1_100x100 = 0;
  _nSc2_100x100 = 0;
  _nVetoTrigger = 0;
  _nVeto_20x20  = 0;
  _nBeamTrigger = 0;

  if ( _simTriggerModeParameter == 10 ) _simTriggerMode = kSimTriggerMode10x10Coincidence;
  else if ( _simTriggerModeParameter == 20 ) _simTriggerMode = kSimTriggerMode20x20;
  else if ( _simTriggerModeParameter == 100 ) _simTriggerMode = kSimTriggerMode100x100Coincidence;
  else {
    streamlog_out(ERROR) << "Undefined trigger mode: " << _simTriggerModeParameter << " -- STOP" << std::endl;
    throw marlin::SkipEventException(this);
  }

}

void TriggerSim::processRunHeader( LCRunHeader* run)
{
  _nRun++ ;
  _DetectorName = run->getDetectorName();
  cout << " TriggerSim : Detector Name <<" << _DetectorName << ">>" << endl;

}

void TriggerSim::processEvent( LCEvent * evt )
{
  static bool firstEvent = true ;

  int NSC1 = 0;
  int NSC2 = 0;
  int NSC3 = 0;
  int NSC4 = 0;
  int NFC1 = 0;
  int NFC2 = 0;
  int NMC1 = 0;
  int NMC2 = 0;
  int Nveto = 0;
  float EFC2 = 0;
  float ESC2 = 0;
  bool isMC = false;

  typedef const std::vector<std::string> StringVec;
  typedef std::vector<const std::string*> StringPVec;
  StringVec* strVec = evt->getCollectionNames();

  for( StringVec::const_iterator name = strVec->begin() ; name != strVec->end() ; name++)
    {
      LCCollection* col = evt->getCollection( *name ) ;
      std::string sss = name->c_str();

      /* Check trigger counters*/
      if(sss == "MCParticle") isMC = true;

      if(sss.find("sciSD") < 99 || sss.find("fcSD") < 99
         || sss.find("mcSD") < 99 || sss.find("vetoSD") < 99)
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

          if(Esum>0.0005 && sss.find("sci")<99 && sss.find("SD1")<99)
            NSC1++;
          if(Esum>0.0005 && sss.find("sci")<99 && sss.find("SD2")<99)
            {
              NSC2++;
              ESC2 += Esum/0.0025;
            }
          if(Esum>0.0005 && sss.find("sci")<99 && sss.find("SD3")<99)
            NSC3++;
          if(Esum>0.0005 && sss.find("sci")<99 && sss.find("SD4")<99)
            NSC4++;
          if(Esum>0.001 && (sss.find("fing")<99 || sss.find("fc")<99) && sss.find("SD1")<99)
            NFC1++;
          if(Esum>0.001 && (sss.find("fing")<99 || sss.find("fc")<99) && sss.find("SD2")<99)
            {
              NFC2++;
              EFC2 += Esum/0.0025;
            }
          if(Esum>0.0005 && (sss.find("muon")<99 || sss.find("mc")<99) && sss.find("SD1")<99)
            NMC1++;
          if(Esum>0.0005 && (sss.find("muon")<99 || sss.find("mc")<99) && sss.find("SD2")<99)
            NMC2++;
          if(Esum>0.0025 && (sss.find("veto")<99 || sss.find("vc")<99) && sss.find("SD1")<99)
            Nveto++;
        }
    }

  evt->parameters().setValue("ReconstructionState",kRecoStateBeam);

  TriggerBits trigger(0);

  /*---------- CERN Aug'06 ------------------*/
  if(_DetectorName.find("Cern0806")<99)
    {
      if(NSC1 > 0)
        {
          trigger.setSc1_10x10();
          _nSc1_10x10++;
        }

      if(NSC2 > 0)
        {
          trigger.setSc2_10x10();
          _nSc2_10x10++;
        }

      if(NFC1 > 0)
        {
          trigger.setSc1_3x3();
          trigger.setSc2_3x3();
          _nSc1_3x3++;
          _nSc2_3x3++;
        }

      if(NFC2 > 0)
        {
          trigger.setVetoTrigger();
          _nVetoTrigger++;
        }

      if(NMC1 > 0)
        {
          trigger.setSc1_100x100();
          _nSc1_100x100++;
        }

      if(NMC2 > 0)
        {
          trigger.setSc2_100x100();
          _nSc2_100x100++;
        }

      evt->parameters().setValue(PAR_MULTI_AMPL, EFC2 * 1500 + 1400);

      if(EFC2 < 1.5) evt->parameters().setValue(PAR_MULTI_BIT, 1);
    }

  /*--------  CERN Oct'06 -------------------*/
  if(_DetectorName.find("Cern1006")<99)
    {
      if(NSC1 > 0)
        {
          trigger.setSc1_10x10();
          _nSc1_10x10++;
        }
      if(NSC2 > 0)
        {
          trigger.setSc2_10x10();
          _nSc2_10x10++;
        }
      if(NFC1 > 0)
        {
          trigger.setSc1_3x3();
          trigger.setSc2_3x3();
          _nSc1_3x3++; _nSc2_3x3++;
        }
      if(NFC2 > 0)
        {
          trigger.setVetoTrigger();
          _nVetoTrigger++;
        }
      if(NMC1 > 0)
        {
          trigger.setSc2_100x100();
          _nSc2_100x100++;
        }

      evt->parameters().setValue(PAR_MULTI_AMPL, EFC2 * 1500 + 1400);
      if(EFC2 < 1.5) evt->parameters().setValue(PAR_MULTI_BIT, 1);
    }

  /*--------- CERN 2007 -------------------*/
  if(_DetectorName.find("Cern0707")<99 || _DetectorName.find("Cern0807")<99)
    {
      if(NSC1 > 0)
        {
          trigger.setSc1_10x10();
          _nSc1_10x10++;
        }
      if(NSC2 > 0)
        {
          trigger.setVeto_20x20();
          _nVeto_20x20++;
        }
      if(NSC3 > 0)
        {
          trigger.setSc2_10x10();
          _nSc2_10x10++;
        }
      if(NMC1 > 0)
        {
          trigger.setSc2_100x100();
          _nSc2_100x100++;
        }
      if(Nveto > 0)
        {
          trigger.setVetoULTrigger();
          trigger.setVetoURTrigger();
          trigger.setVetoDRTrigger();
          trigger.setVetoDLTrigger();
          _nVetoTrigger++;
        }


      evt->parameters().setValue(PAR_MULTI_AMPL, ESC2 * 1500 + 1100);
      if(ESC2 < 1.5) evt->parameters().setValue(PAR_MULTI_BIT, 1);
    }

  /*--------- Fermilab 2008 / 2009 -------------------*/
  if(_DetectorName.find("Fnal0508")<99 || _DetectorName.find("Fnal0508_p0709")<99)
    {
      if(NSC1 > 0)
        {
          trigger.setSc1_10x10();
          _nSc1_10x10++;
        }
      if(NSC2 > 0)
        {
          trigger.setVeto_20x20();
          _nVeto_20x20++;
        }
      if(NSC3 > 0)
        {
          trigger.setSc2_10x10();
          _nSc2_10x10++;
        }
      if(NMC1 > 0)
        {
          trigger.setSc2_100x100();
          _nSc2_100x100++;
        }
      if(Nveto > 0)
        {
          trigger.setVetoULTrigger();
          trigger.setVetoURTrigger();
          trigger.setVetoDRTrigger();
          trigger.setVetoDLTrigger();
          _nVetoTrigger++;
        }


      evt->parameters().setValue(PAR_MULTI_AMPL, ESC2 * 1500 + 1100);
      if(ESC2 < 1.5) evt->parameters().setValue(PAR_MULTI_BIT, 1);
    }

  if (
      ( ( _simTriggerMode == kSimTriggerMode10x10Coincidence ) && ( trigger.isSc1_10x10() && trigger.isSc2_10x10() ) )
      || ( ( _simTriggerMode == kSimTriggerMode20x20 ) && ( trigger.isVeto_20x20() ) )
      || ( ( _simTriggerMode == kSimTriggerMode100x100Coincidence ) &&  ( trigger.isSc1_100x100() && trigger.isSc2_100x100() ) )
      )
    {
      trigger.setBeamTrigger();
      _nBeamTrigger++;
    }

  evt->parameters().setValue(PAR_TRIGGER_EVENT, trigger.getTriggerBits());

  firstEvent = false ;
  _nEvt ++ ;
}


void TriggerSim::end()
{
  std::cout << "TriggerSim::end()  " << name()
            << " processed " << _nEvt << " events in " << _nRun << " runs "
            << std::endl ;
  if(_nSc1_10x10>0)   std::cout << " Sc1_10x10 trigger set " << _nSc1_10x10 << " times" << std::endl;
  if(_nSc2_10x10>0)   std::cout << " Sc2_10x10 trigger set " << _nSc2_10x10 << " times" << std::endl;
  if(_nSc1_3x3>0)     std::cout << " Sc1_3x3 trigger set " << _nSc1_3x3 << " times" << std::endl;
  if(_nSc2_3x3>0)     std::cout << " Sc2_3x3 trigger set " << _nSc2_3x3 << " times" << std::endl;
  if(_nSc1_100x100>0) std::cout << " Sc1_100x100 trigger set " << _nSc1_100x100 << " times" << std::endl;
  if(_nSc2_100x100>0) std::cout << " Sc2_100x100 trigger set " << _nSc2_100x100 << " times" << std::endl;
  if(_nVetoTrigger>0) std::cout << " Veto trigger set " << _nVetoTrigger << " times" << std::endl;
  if(_nVeto_20x20>0)  std::cout << " Veto_20x20 trigger set " << _nVeto_20x20 << " times" << std::endl;
  if(_nBeamTrigger>0) std::cout << " Beam trigger set " << _nBeamTrigger << " times" << std::endl;

}
