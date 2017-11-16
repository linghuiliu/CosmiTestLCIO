#ifndef HCAL_PID_PROCESSOR_HH
#define HCAL_PID_PROCESSOR_HH 1

#include <marlin/Processor.h>
#include <EVENT/LCEvent.h>
#include "lccd/IConditionsChangeListener.hh"

namespace CALICE {

  /**
   * Processor to select (or veto) events from HCal testbeam in PS T9 with the particles requested by the user.
   * The steering parameters 'Particle' and 'Veto' define the particle species and if the particle will be selected or vetoed.
   * The selection is based on the information given by Cherenkov counters A and B.
   * In case the selection is not suitable for the requested particle, a warning is printed.
   *
   * @author Bruno Lenzi <Bruno.Lenzi@cern.ch>
   * @date Feb 2011
   */
  
  class HCalPIDProcessor : public marlin::Processor, public lccd::IConditionsChangeListener {
  public:
    
    HCalPIDProcessor();
    virtual ~HCalPIDProcessor();
    
    HCalPIDProcessor* newProcessor(){return new HCalPIDProcessor();}
    
    virtual void init();
    virtual void processRunHeader(LCRunHeader *);
    virtual void processEvent(lcio::LCEvent*);
    virtual void end();
    
    /** Callback function for the condition changes */
    virtual void conditionsChanged(LCCollection *col);
    
  protected:
    
  private:
    /** Convert the string to define the particle selected by the user and store it in _particleStr and _kParticle
	@param particle string containing the particle to be selected
    */
    void defineParticle(std::string particle); 
    
    /** Read the run info from the conditions database: beam energy and pressure in 
     * Cherenkov counters A and B. Called at the beginning of each run (or if run conditions change)
     */
    void getRunInfo(LCEvent* evt);
    
    /** Determine if Cherenkov radiation is expected
	@param pressure in Cherenkov counter
	@param particle being selected (or vetoed), from HCalPID::Particle
	@param beamE energy in GeV
    */
    bool cherenkovExpected(float pressure, unsigned int particle, float beamE);
    
    /** Return the pressure above which the given particle produces Cherenkov light at momentum P
	@param P the momentum of the particle in GeV
	@param particle the particle according to HCalPID::Particle */
    float cherenkovThreshold(float P, unsigned int particle);
    
    /* processor parameters */
    std::string _particleStr;
    std::string _parNameTriggerEvent;
    std::string _elogColName;
    bool _veto;
    
    bool _elogColChanged; /** listener */
    float _cherenkovThresholds[10][5]; /** Hardcoded table with the minimum pressure needed on the Cherenkov counter
					   to produce light, depending on the particle and its momentum
					   (key: momentum -1, particle) -> value: pressure */
    bool _kCherenkov, _kCherenkov2; /** Cherenkov radiation expected in counters A and B */
    unsigned int _kParticle; /** Particle species, defined by the user */
  };
  
}

#endif
