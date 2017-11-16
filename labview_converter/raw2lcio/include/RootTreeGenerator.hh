/* Processor which generate the event tree*/

#ifndef ROOTTREEGENERATOR_h
#define ROOTTREEGENERATOR_h 1


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
    
    
    class RootTreeGenerator : public Processor{
        
    public:
        virtual Processor* newProcessor() { return new RootTreeGenerator;}
        RootTreeGenerator();
        ~RootTreeGenerator();
        void init();
        void processEvent( LCEvent * evt ) ;
        void end();
        
    protected:
        std::string _inputColName;
        std::string _prefix;
        
        TFile *_rootFile;
        std::string _rootFileName;
        
        TTree *_treeLabviewBlock;
        
        int _nHits;
        
        int _BunchXID;
        int _CycleNr;
        int _ChipID;
        int _ASICNr;
        int _EvtNr;
        int _Channel;
        int _TDC;
        int _ADC;
        int _xPos;
        int _yPos;
        int _HitBit;
        int _GainBit;
        
    private:
        
        static std::map<TTree*,RootTreeGenerator*> _treeFillerMap;
        static std::map<TTree*,RootTreeGenerator*> _treeOwnerMap;
        static std::map<TFile*,RootTreeGenerator*> _fileOwnerMap;
    };
}
#endif
