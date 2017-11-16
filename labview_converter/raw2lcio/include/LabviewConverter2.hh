#ifndef LabviewConverter2_h
#define LabviewConverter2_h 1

#include <marlin/DataSourceProcessor.h>

#include <sstream>

namespace marlin{
  
  /** Processor for reading the AHCAL Labview asscii files.   
   *  It processes the input data to create events with LCIO collections
   *  of the AHCAL Labview ascii output.
   *  Based on F. Gaede's StdHepReader to be found in the marlin package. 
   *  @author: S. Lu (DESY Hamburg)
   *  @date Aug 15 2012
   */
  
  class LabviewConverter2 : public DataSourceProcessor {
    
  public:

    LabviewConverter2() ;

    /** Implementation of new Processor 
      * returns pointer to processor  
      */
    virtual LabviewConverter2*  newProcessor() ;


    /** Creates events with LCGenrealObject collections from the Labview input file and
     *  calls all active processors' processEvent() and processRunHeader Method.
     */
    virtual void readDataSource( int numEvents ) ;
    

    /** work on slow control data, ASIC */
    void writeSlowControl() ;

    /** work on slow control data,  Temperature*/
    //void writeTemperature() ;
    
    /** init method */
    virtual void init() ;

    /** end method */
    virtual void end() ;
    
  protected:
    
    /** The string variable to access the data. Requires
	"yourpath/Run" */
    //std::string _dataPath ;
    std::string _ofile ;

    /** The string variable to access the data. 
     */
    std::string _data ;

    /** The run number */
    int _runNumber;

    /** Slow control line number, HBU: 120, EPT: 1920 */
    int _SlowControlLineNumber;

    /** The HBU number: HBUVI: 6; HBUVI: 7, HBUVIII: 8, HBUIX: 9. */
    int _HBUNumber;

    /** The EPT model number: CERN November testbeam EPT: 1 */
    int _EPTModelNr;

    /** Counter for number of corrupted ROC */
    int nCorruptedCycle;
    
    /** Slow Control data block */
    std::stringstream SlowControl;

    /** The string variable of the detector type name, AHC AEC. 
     */
    std::string _detectorTypeName;
 

    /** The labview data format. 
     */
    struct rawData {
      int iCyNr;
      int iBxID;
      int iCpID;
      int iEvNr;
      int iChan;
      int iTDC ;
      int iADC ;
      int iHBit;
      int iGBit;
    };


  private:
    
 
  };
 
} // end namespace marlin 

#endif
