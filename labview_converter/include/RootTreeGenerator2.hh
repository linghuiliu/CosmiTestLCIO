/* Processor which generate the event tree*/

#ifndef ROOTTREEGENERATOR2_h
#define ROOTTREEGENERATOR2_h 1


#include "marlin/Processor.h"

#include "TFile.h"
#include "TTree.h"

#define MAXCELLS 400000

using namespace marlin ;

namespace CALICE {

/** Class to process Labview raw 
  * @author: Shaojun Lu DESY
  * @date Nov 15 2012
  *
  */


  class RootTreeGenerator2 : public Processor{
    
  public:
    virtual Processor* newProcessor() { return new RootTreeGenerator2;}
    RootTreeGenerator2();
    ~RootTreeGenerator2();
    void init();  
    void processEvent( LCEvent * evt ) ;
    void end();
    
  protected:
    std::string _inputColName;

    TFile *_rootFile;
    std::string _rootFileName;

    TTree *_tree;

    double _Esum_Layer[500];
    double _Esum;
    int _event_num;
    int nHits;
    int _nHits_Layer[500];
    int K[MAXCELLS];
    int I[MAXCELLS];
    int J[MAXCELLS];
    float hitEnergy[MAXCELLS];
    float hitPos[MAXCELLS][3];
    float hitTimestamp[MAXCELLS];

  private:
  };
}
#endif
