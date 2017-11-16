#ifndef ExtractIntercalibrationProcessor_h
#define ExtractIntercalibrationProcessor_h 1

/*standard includes*/
#include <string>
#include <map>

/*LCIO and Marlin includes*/
#include "lcio.h"
#include "marlin/Processor.h"

/*CALICE includes*/
#include "AhcConditions.hh"
#include "AhcMapper.hh"

/**
   Processor calculating the electronics intercalibration for each channel from a set of 
   PmLedVcalibScan and CmLedVcalibScan run.
   
   At least one run of each type is needed as input files. The order is not important, 
   the processor separates CM data from PM data.
   
   Intercalibration factor ic = signal (CM) / signal (PM)
   
   @author N. Meyer
 
 */

class ExtractIntercalibrationProcessor : public marlin::Processor 
{
  
public:
  
  ExtractIntercalibrationProcessor();
  
  ExtractIntercalibrationProcessor* newProcessor(){return new ExtractIntercalibrationProcessor;}
  
  virtual void init();

  void processEvent(LCEvent* evt);

  virtual void end();

protected:
  void updateMapper();

  struct datapoint{
    double sumPm;
    double sum2Pm;
    double nPm;
    double sumCm;
    double sum2Cm;
    double nCm;
  };

  struct condpoint{
    unsigned vcalib;
    unsigned shiftreg[12];
  };

  std::map<unsigned, std::map<unsigned, datapoint> > _data;
  std::map<unsigned, condpoint> _cond;

  std::string _inColName;
  std::string _condColName;
  std::string _outFileName;
  std::string _mappingProcessorName;

  unsigned int _ahcMaxNumberOfChips;    /**<AHCAL maximum number of chips*/
  unsigned int _ahcMaxNumberOfModules;  /**<AHCAL maximum number of modules (39)*/
  unsigned int _ahcMaxNumberOfCells;    /**<AHCAL maximum number of cells*/
  unsigned int _ahcMaxNumberOfChannels; /**<AHCAL maximum number of channels*/

  const CALICE::AhcMapper *_ahcMapper;
  unsigned int _mapperVersion;

};

#endif


