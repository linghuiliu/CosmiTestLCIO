#include "HCalPIDProcessor.hh"

#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "EVENT/LCCollection.h"
#include "CaliceElogInfo.hh"

#include <collection_names.hh>
#include <TriggerBits.hh>
#include <ctype.h>
#include <cmath>
#include <cstdlib>

using namespace marlin;
using namespace lcio;
using namespace std;

namespace HCalPID {
  /** e, mu, pi, K, p */
  enum Particle { electron, muon, pion, kaon, proton, Nparticles };
  /** e, mu, pi, K, p strings */
  const char* particleName[5] = {"electron", "muon", "pion", "kaon", "proton" };
  /** e, mu, pi, K, p masses in GeV */
  float particleMass[5] = {0.5e-3, 0.105, 0.140, 0.494, 0.938};  
  
  /** Return the square of the Lorentz factor gamma
    @param momentum in GeV
    @param mass in GeV */
  float gamma2(float momentum, float mass)
  {
    return 1 + momentum*momentum/(mass*mass);
  }
}

namespace CALICE {

	HCalPIDProcessor aHCalPIDProcessor;

	/*******************************************************************************************/
	HCalPIDProcessor::HCalPIDProcessor() : Processor("HCalPIDProcessor")
	{
	    _description = "Selects events from HCal testbeam with the particles requested by the user. \n\
	                  The selection is based on the information given by the Cherenkov counters A and B.";

	    registerProcessorParameter("TriggerEventName",
	                               "Name of the event parameter name which contains the current trigger main word .",
	                               _parNameTriggerEvent,
	                               std::string(PAR_TRIGGER_EVENT));

	    registerProcessorParameter("Particle",
	                               "Particle to be selected (string)",
	                               _particleStr,
	                               std::string("Electron"));

      registerProcessorParameter("ElogCollectionName",
                                "Name of Elog collection in database",
                                _elogColName,
                                std::string("Elog"));
                                
      registerProcessorParameter("Veto",
                                 "To veto particles instead of selecting them (boolean)",
                                 _veto,
                                 false);
	  
	}

	/*******************************************************************************************/
	HCalPIDProcessor::~HCalPIDProcessor()
	{}


	/*******************************************************************************************/
	void HCalPIDProcessor::init()
	{
	  _elogColChanged = false;

	  if (!ConditionsProcessor::registerChangeListener(this, _elogColName))
	  {
	    streamlog_out(ERROR) << "Undefined conditions: " << _elogColName << endl;
	    throw marlin::StopProcessingException(this);
	  }
		  
		  // Define the particle to be selected from the "particle" string
		  defineParticle(_particleStr);

		  // Hardcoded table with the minimum pressure needed on the Cherenkov counter
		  // for each particle to produce Cherenkov radiation for a given beam energy
		  // key: [beamE - 1, Particle enum] -> value: pressure
		  float cherenkovThresholds[10][5] = { 
		    {0.0004, 13.23, 23.02, 348.82, 880.9}, 
		    {0.0001, 3.31, 5.78, 76.13, 248.1}, 
		    {0.0000, 1.47, 2.57, 33.06, 113.3}, 
		    {0.0000, 0.83, 1.45, 18.45, 64.4}, 
		    {0.0000, 0.53, 0.93, 11.76, 41.4}, 
		    {0.0000, 0.37, 0.64, 8.15, 28.8}, 
		    {0.0000, 0.27, 0.47, 5.98, 21.2}, 
		    {0.0000, 0.21, 0.36, 4.58, 16.3}, 
		    {0.0000, 0.16, 0.29, 3.61, 12.9}, 
		    {0.0000, 0.13, 0.23, 2.93, 10.4} };
		  
		  for (unsigned int i = 0; i < 10; ++i)
		    for (unsigned int j = 0; j < 5; ++j)
		      _cherenkovThresholds[i][j] = cherenkovThresholds[i][j];
		 
	}

	/***************************************************************************************/
	void HCalPIDProcessor::conditionsChanged( LCCollection * col ) 
	{
	  std::string colName = col->getParameters().getStringVal("CollectionName") ;
	  
	  if (colName == _elogColName)
    {
      _elogColChanged = true;
    }
	}     

	/*******************************************************************************************/
	void HCalPIDProcessor::processRunHeader(LCRunHeader *runHeader)
	{
	}


	/*******************************************************************************************/
	void HCalPIDProcessor::processEvent(LCEvent* evt)
	{
	  // Read the run info from the database if the conditions change (e.g.: at the first event of each run)
    if (_elogColChanged)
    {
      getRunInfo(evt);
      _elogColChanged = false;
    }
	   
	  // Read the trigger information for Cherenkov counters
	  TriggerBits triggerEvent = evt->getParameters().getIntVal(_parNameTriggerEvent);
	  bool cherenkov = triggerEvent.isCherenkovTrigger();
	  bool cherenkov2 = triggerEvent.isCherenkov2Trigger();
	   
	  // PID based on Cherenkov counters with shap cut:
	  // radiation expected and observed OR not expected and not observed
 	  streamlog_out(DEBUG) << "Cherenkov expected in A: " << _kCherenkov << ". Observed: " << cherenkov << endl;
	  streamlog_out(DEBUG) << "Cherenkov expected in B: " << _kCherenkov2 << ". Observed: " << cherenkov2 << endl;

	  if ( ((_kCherenkov == cherenkov) && (_kCherenkov2 == cherenkov2)) == _veto )
	  {
	    streamlog_out(DEBUG) << "Skipping event" << endl;
  	  throw marlin::SkipEventException(this);
  	}
	}


	/*******************************************************************************************/
	void HCalPIDProcessor::end()
	{

	}


	/*******************************************************************************************/
	void HCalPIDProcessor::defineParticle(std::string particle)
	{
 
	  // Convert all strings to lowercase
	  for (unsigned int i = 0; i < particle.length(); ++i)
	    particle[i] = std::tolower(particle[i]);
	  
	  if ( particle.find("electron") != string::npos )
	    _kParticle = HCalPID::electron;
	  else if ( particle.find("muon") != string::npos )
	    _kParticle = HCalPID::muon;
	  else if ( particle.find("pion") != string::npos )
	    _kParticle = HCalPID::pion;
	  else if ( particle.find("kaon") != string::npos )
	    _kParticle = HCalPID::kaon;
	  else if ( particle.find("proton") != string::npos )
	    _kParticle = HCalPID::proton;    
	  else
	  {
	    streamlog_out(ERROR) << "Unknown particle: " << _particleStr << std::endl;
	    throw marlin::StopProcessingException(this);
	  }

    streamlog_out(DEBUG) << "Processor will " << std::string((!_veto) ? "select " : "veto ")
                 << particle << "s / (code " << _kParticle << ")" << endl;
             
	}

	/*******************************************************************************************/
	void HCalPIDProcessor::getRunInfo(LCEvent* evt)
	{
		streamlog_out(DEBUG) << "RUN: " << evt->getRunNumber() << endl;
	
	  // Try to access conditions database
	  LCCollection* testCol = NULL;
    try
   	{
	     testCol = evt->getCollection( _elogColName );
  	}
      catch ( DataNotAvailableException &e )
	  {
	    streamlog_out(ERROR) << "Collection "<<_elogColName<< " not available, cannot read database"<<endl;
	    throw marlin::StopProcessingException(this);
	  }

		// Read the beam energy and the pressures in the Cherenkov counters from the conditions db
    CaliceElogInfo info(testCol);
    float beamE = info.getEnergy();
    float pA = info.getCherenkovPressure();
    float pB = info.getCherenkov2Pressure();
    
    // Check if pressure and beam energy are valid (1 <= beamE <= 10 and pA, pB > 0 )
    if (fabs(beamE) < 1 || fabs(beamE) > 10)
    {
      streamlog_out(ERROR) << "Invalid beam energy: " << beamE << endl;
	    throw marlin::StopProcessingException(this);
    }
    
    if(pA < 0 || pB < 0)
    {
      streamlog_out(ERROR) << "Invalid pressure in Cherenkov counters: " << pA << " / " << pB << endl;
	    throw marlin::StopProcessingException(this);
    }

	  // Determine if Cherenkov radiation is expected given pressures, beam energy and particle
	  _kCherenkov  = cherenkovExpected(pA, _kParticle, beamE); // counter A
	  _kCherenkov2 = cherenkovExpected(pB, _kParticle, beamE); // counter B  

 	  streamlog_out(DEBUG) << "Beam energy: " << beamE << " GeV" << endl;	  
	  streamlog_out(DEBUG) << "Pressure in Cherenkov counter A: " << pA << " bar" << endl;
	  streamlog_out(DEBUG) << "Pressure in Cherenkov counter B: " << pB << " bar" << endl;
	  streamlog_out(DEBUG) << "Cherenkov expected in A: " << _kCherenkov << endl;
	  streamlog_out(DEBUG) << "Cherenkov expected in B: " << _kCherenkov2 << endl;
	  
	  // Check and warn the user if any other particle would give the same result given the Cherenkov pressures
	  for (unsigned int i = 0; i < HCalPID::Nparticles; ++i)
    {
      if (i != _kParticle && 
          cherenkovExpected(pA, i, beamE) == _kCherenkov &&
          cherenkovExpected(pB, i, beamE) == _kCherenkov2 &&
          (i != HCalPID::proton || beamE > 0) ) // protons only come with positive beam
      {
        streamlog_out(WARNING) << "Selection cannot distinguish chosen particles from " << HCalPID::particleName[i] << "s" << endl;
      }

    }
  }

	/*******************************************************************************************/
  bool HCalPIDProcessor::cherenkovExpected(float pressure, unsigned int particle, float beamE)
  {
    //return pressure > cherenkovThreshold(beamE, particle); // -> function instead of table
    unsigned int bE = (unsigned int) abs((int) beamE); // convert beam energy to unsigned int to use as index in table
 	  streamlog_out(DEBUG) << "Threshold for " << HCalPID::particleName[particle] << "s: " << _cherenkovThresholds[bE - 1][particle] << endl;
    return pressure > _cherenkovThresholds[bE - 1][particle];

  }

	/*******************************************************************************************/
	float HCalPIDProcessor::cherenkovThreshold(float beamE, unsigned int particle)
	{
	  streamlog_out(DEBUG) << "Threshold for " << HCalPID::particleName[particle] << "s: " << 1193/HCalPID::gamma2(beamE, HCalPID::particleMass[particle]) << endl;
	  return 1193/HCalPID::gamma2(beamE, HCalPID::particleMass[particle]);
	}

}
