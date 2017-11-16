#ifndef ExtractSaturationCurveProcessor_h
#define ExtractSaturationCurveProcessor_h 1

/*   standard includes*/
#include <string>
#include <map>

#include "lcio.h"
#include "marlin/Processor.h"

#include "AhcMapper.hh"


class ExtractSaturationCurveProcessor : public marlin::Processor 
{

public:
  
  ExtractSaturationCurveProcessor();
  
  ExtractSaturationCurveProcessor* newProcessor(){return new ExtractSaturationCurveProcessor;}

  virtual void init();

  void processEvent(LCEvent* evt);

  virtual void end();



protected:
  struct datapoint{
    double sumPm;
    double sum2Pm;
    double nPm;
    unsigned short modID;
    unsigned short cellK;
    unsigned short noOverAdc;
  };

  struct condpoint{
    unsigned vcalib;
    unsigned shiftreg[12];
  };

  std::map<unsigned,std::map<unsigned,datapoint> > _data;
  std::map<unsigned,condpoint> _cond;

  std::string _inColName;
  std::string _condColName;

  std::string _outFileName;
  std::string _datFileName;

  bool _doFit;
  bool _subtractPedestal;

  int _nPointsUsed; /* for fit*/
  int _ledType;
 
  unsigned int vcell;


  unsigned int _ahcMaxNumberOfChips;    /**<AHCAL maximum number of chips*/
  unsigned int _ahcMaxNumberOfModules;  /**<AHCAL maximum number of modules (39)*/
  unsigned int _ahcMaxNumberOfCells;    /**<AHCAL maximum number of cells*/
  unsigned int _ahcMaxNumberOfChannels; /**<AHCAL maximum number of channels*/

  const CALICE::AhcMapper *_ahcMapper;
  unsigned int _mapperVersion;
  std::string _mappingProcessorName;
  void updateMapper();

};

#endif


