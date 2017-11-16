/* Processor which generate the event tree*/

#ifndef TEMPROOTTREEGENERATOR_h
#define TEMPROOTTREEGENERATOR_h 1
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

  class TempRootTreeGenerator : public Processor{
    
  public:
    virtual Processor* newProcessor() { return new TempRootTreeGenerator;}
    TempRootTreeGenerator();
    ~TempRootTreeGenerator();
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

    TTree *_treeTempSensorBlock;

    const static unsigned int MAXPORTS  = 1000; /*should be big enough for all detectors!*/

    struct
    {
      int nLayers;
      float T1[MAXPORTS];
      float T2[MAXPORTS];
      float T3[MAXPORTS];
      float T4[MAXPORTS];
      float T5[MAXPORTS];
      float T6[MAXPORTS];
      float TDIF[MAXPORTS];
      float TPWR[MAXPORTS];
    } _hFill;

   

  private:

    static std::map<TTree*,TempRootTreeGenerator*> _treeFillerMap;
    static std::map<TTree*,TempRootTreeGenerator*> _treeOwnerMap;
    static std::map<TFile*,TempRootTreeGenerator*> _fileOwnerMap;

    static const double INVALID;

    int nLayers;
    int runNumber;
    int eventNumber;
    long64 Timestamp;
  };
}
#endif
