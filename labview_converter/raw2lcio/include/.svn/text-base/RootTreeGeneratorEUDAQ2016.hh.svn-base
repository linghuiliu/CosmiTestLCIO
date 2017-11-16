/* Processor which generate the event tree*/

#ifndef ROOTTREEGENERATOREUDAQ2016_h
#define ROOTTREEGENERATOREUDAQ2016_h 1
#include "marlin/Processor.h"
#include "TFile.h"
#include "TTree.h"
#include <map>

using namespace marlin ;

namespace CALICE {

/** Class to process Labview raw 
  * @author: Shaojun Lu DESY
  * @date Nov 15 2012
  *
  */


  class RootTreeGeneratorEUDAQ2016 : public Processor{
    
  public:
    virtual Processor* newProcessor() { return new RootTreeGeneratorEUDAQ2016;}
    RootTreeGeneratorEUDAQ2016();
    ~RootTreeGeneratorEUDAQ2016();
    void init();  
    void processEvent( LCEvent * evt ) ;
    void end();
    void registerBranches( TTree* hostTree );
    void FillVariable(LCEvent* evt);
    
  protected:
    std::string _inputColName;
    std::string _prefix;

    TFile *_rootFile;
    std::string _rootFileName;

    TTree *_treeEUDAQBlock2016;

    const static unsigned int MAXCELLS  = 7609; /*should be big enough for all detectors!*/

    struct
    {
      int iEvt;
      int nHits;
      int BunchXID[MAXCELLS];
      int CycleNr[MAXCELLS];
      int ChipID[MAXCELLS];
      int EvtNr[MAXCELLS];
      int Channel[MAXCELLS];
      int TDC[MAXCELLS];
      int ADC[MAXCELLS];
      int HitBit[MAXCELLS];
      int GainBit[MAXCELLS];
    } _hFill;

   

  private:

    static std::map<TTree*,RootTreeGeneratorEUDAQ2016*> _treeFillerMap;
    static std::map<TTree*,RootTreeGeneratorEUDAQ2016*> _treeOwnerMap;
    static std::map<TFile*,RootTreeGeneratorEUDAQ2016*> _fileOwnerMap;

    static const double INVALID;

    int nHits;
    int ievt;
    int runNumber;
    int eventNumber;
    int Timestamp;
  };
}
#endif
