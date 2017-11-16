#ifndef Ahc2OffsetCalibrator_hh
#define Ahc2OffsetCalibrator_hh 1

//#include <marlin/DataSourceProcessor.h>
#include "marlin/Processor.h"

#include <sstream>
#include <map>

using namespace marlin ;

namespace CALICE{
  
  /**
   * Processor to read SLCIO EUDAQ files and sort them according to the BXID 
   * Then do the AHCAL-BIF Offset calibration by counting the number of correlated events
   * (either a separation between ECAL and AHCAL collections is done by EUDAQ) 
   * @author E. Brianne, DESY Hamburg
   * @date May 2016
   * Created for 2016 testbeams EUDAQ data format and AHCAL-BIF Offset calibration.
   */
  
  class Ahc2OffsetCalibrator : public Processor {
    
  public:

    /** Implementation of new Processor 
     * returns pointer to processor  
     */
    virtual Processor*  newProcessor() { return new Ahc2OffsetCalibrator;}
    Ahc2OffsetCalibrator() ;
    ~Ahc2OffsetCalibrator() ;

    /** Creates events with LCGenrealObject collections from the EUDAQ input file and
     *  calls all active processors' processEvent() and processRunHeader Method.
     */
    void processEvent( LCEvent* evt ) ;
    
    /** init method */
    virtual void init() ;

    /** end method */
    virtual void end() ;

    /** print parameters */
    void printParameters();

  protected:

    // INPUT collections
    std::string _inputColName;
    std::string _inputColNameBIF;

    std::string _outputfile;

    StringVec _BifVector;
    
    /** The run number */
    int _runNumber;

    /** The slcio data format. 
     */
    struct rawData2016 {
      int iCycleNr;
      int iBxID;
      int iEvNr;
      int iCpID;
      int iNChan;
      std::vector<int> iTDC ;
      std::vector<int> iADC ;
    };

  private:

    int LcioEventNr; // new event number after reco
    std::map<int, int> nCorrelatedEvt; //Counter for number of correlated events

    //Bif offset scan parameters
    int StartScan;
    int StopScan;
    int StepScan;

  };

} // end namespace marlin 

#endif
