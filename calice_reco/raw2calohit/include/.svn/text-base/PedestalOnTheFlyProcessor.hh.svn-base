#ifndef PedestlOnTheFlyProcessor_h
#define PedestalOnTheFlyProcessor_h 1

#include <lcio.h>
#include <marlin/Processor.h>

// FIXME: no magic numbers please
#define HCAL_N_MOD 39
#define HCAL_N_CELL 216

namespace CALICE {
  
  /** Processor which calulates the pedestals from pedestal events and subtracts these values
   * from following non-pedestal events.
   * @author S. Schmidt
   * @date Dec 2006
   */
  
  class PedestalOnTheFlyProcessor : public marlin::Processor{
  public:
    PedestalOnTheFlyProcessor();
    ~PedestalOnTheFlyProcessor(){};
    
    PedestalOnTheFlyProcessor* newProcessor(){return new PedestalOnTheFlyProcessor;}
    
    virtual void init();
    void processEvent(lcio::LCEvent* evt);
    virtual void end(){};
    
  protected:
    std::string _inputColName;
    std::string _outputColName;
    
    double _pedSum[HCAL_N_MOD+1][HCAL_N_CELL];
    double _pedSumSquare[HCAL_N_MOD+1][HCAL_N_CELL];
    unsigned _pedNum[HCAL_N_MOD+1][HCAL_N_CELL];
    
    float _ped[HCAL_N_MOD+1][HCAL_N_CELL];
    float _pedError[HCAL_N_MOD+1][HCAL_N_CELL];
    
    float _significanceCut;
    
    int _skipPedestals;
    int _skipStartUpPedestals;
    int _pedCounter;
    int _minPedNumber;
    
    bool _throwSkipEventException;
    
  };
  
}

#endif
