#include "RootTreeGenerator3.hh"
#include "TROOT.h"
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "LabviewBlock2.hh"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cfloat>

#include "TFile.h"
#include "TTree.h"

using namespace std;
using namespace lcio;

namespace CALICE {
    
    RootTreeGenerator3 aRootTreeGenerator3;
    
    map<TTree*, RootTreeGenerator3*> RootTreeGenerator3::_treeFillerMap;
    map<TTree*, RootTreeGenerator3*> RootTreeGenerator3::_treeOwnerMap;
    map<TFile*, RootTreeGenerator3*> RootTreeGenerator3::_fileOwnerMap;
    
    RootTreeGenerator3::RootTreeGenerator3() : Processor("RootTreeGenerator3")
    {
        _description = "Processor to generate root tree with Event built";
        
        registerProcessorParameter("InputCollectionName",
                                   "Name of the input collection of Labview raw data",
                                   _inputColName,
                                   std::string("LabviewData"));
        
        registerProcessorParameter("OutputRootFileName",
                                   "Name of the output root file",
                                   _rootFileName,
                                   std::string("LabviewDataCERN.root"));
        
        registerProcessorParameter("BranchPrefix",
                                   "Name of the prefix of the branch",
                                   _prefix,
                                   std::string("ahc"));
        
    }
    
    RootTreeGenerator3::~RootTreeGenerator3() {}
    
    void RootTreeGenerator3::init()
    {
        
        _rootFile = gROOT->GetFile(_rootFileName.c_str());
        if ( _rootFile == NULL )
        {
            _rootFile      = new TFile(_rootFileName.c_str(), "RECREATE");
            _fileOwnerMap[ _rootFile ] = this;
        }
        
        _rootFile->cd();
        _treeLabviewBlock = dynamic_cast<TTree*>( _rootFile->Get("LabviewData"));
        
        if ( _treeLabviewBlock != NULL )
            streamlog_out(DEBUG) << "Reuse tree ["<< "LabviewData" <<"] " << endl;
        if ( _treeLabviewBlock == NULL )
        {
            _treeLabviewBlock = new TTree( "LabviewData" , "all information");
            _treeOwnerMap[_treeLabviewBlock] = this;
        }
        
        bool firstProcessor = (_treeFillerMap[ _treeLabviewBlock ] == NULL );
        _treeFillerMap[ _treeLabviewBlock ] = this;
        
        if ( firstProcessor )
        {
            _treeLabviewBlock->Branch( "eventNumber", &eventNumber, "EventNumber/I");
            _treeLabviewBlock->Branch( "runNumber", &runNumber, "RunNumber/I");
            _treeLabviewBlock->Branch( "Timestamp", &Timestamp, "Timestamp/I");
        }
        
        registerBranches(_treeLabviewBlock);
    }
    
    void RootTreeGenerator3::registerBranches( TTree* hostTree )
    {
        
        hostTree->Branch( string(_prefix + "nHits").c_str(), &_hFill.nHits,  string(_prefix+"nHits/I").c_str());
        hostTree->Branch( string(_prefix+"iEvt").c_str(), &_hFill.iEvt, string(_prefix+"iEvt/I").c_str());
        
        hostTree->Branch( string(_prefix + "BunchXID").c_str(), &_hFill.BunchXID, string(_prefix+"BunchXID["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "CycleNr").c_str(), &_hFill.CycleNr, string(_prefix+"CycleNr["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "ChipID").c_str(), &_hFill.ChipID, string(_prefix+"ChipID["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "EvtNr").c_str(), &_hFill.EvtNr, string(_prefix+"EvtNr["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "Channel").c_str(), &_hFill.Channel, string(_prefix+"Channel["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "TDC").c_str(), &_hFill.TDC, string(_prefix+"TDC["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "ADC").c_str(), &_hFill.ADC, string(_prefix+"ADC["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "HitBit").c_str(), &_hFill.HitBit, string(_prefix+"HitBit["+_prefix+"nHits]/I").c_str() );
        hostTree->Branch( string(_prefix + "GainBit").c_str(), &_hFill.GainBit, string(_prefix+"GainBit["+_prefix+"nHits]/I").c_str() );
        
        ievt = 0;
        
    }
    
    void RootTreeGenerator3::FillVariable(LCEvent* evt)
    {
        
        LCCollection* col ;
        try
        {
            //fetch Labview data raw collection
            col = evt->getCollection( _inputColName ) ;
            
            //cout <<"\n Event "<<evt->getEventNumber()<<", loop over collection "<< _inputColName
            //	     <<" with "<< col->getNumberOfElements()<<" elements "<<endl;
            
            nHits = 0;
            
            //check all the ChipID, each one has 36 channels,
            for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
            {
                LCObject *obj = col->getElementAt(ielm);
                LabviewBlock2 lBlock(obj);
                
		_hFill.BunchXID[nHits] = lBlock.GetBunchXID();
		_hFill.CycleNr[nHits] = lBlock.GetCycleNr();
		_hFill.ChipID[nHits]  = lBlock.GetChipID();
		_hFill.EvtNr[nHits]   = lBlock.GetEvtNr();
		_hFill.Channel[nHits] = lBlock.GetChannel();
		_hFill.TDC[nHits]     = lBlock.GetTDC();
		_hFill.ADC[nHits]     = lBlock.GetADC();
		_hFill.HitBit[nHits]  = lBlock.GetHitBit();
		_hFill.GainBit[nHits] = lBlock.GetGainBit();
                    
		nHits ++;
		ievt ++;
            }
            
            _hFill.iEvt = ievt;
            _hFill.nHits = nHits;
        }
        catch (  DataNotAvailableException err )
        {
            //	cout <<  "RootTreeGenerator3 WARNING: Collection "<< _inputColName
            //	     << " not available in event "<< evt->getEventNumber() << endl;
            return;
        }
        
    }
    
    void RootTreeGenerator3::processEvent(LCEvent* evt)
    {
        
        runNumber   = evt->getRunNumber();
        eventNumber = evt->getEventNumber();
        Timestamp   = evt->getTimeStamp();
        
        FillVariable(evt);
        
        if ( _treeFillerMap[_treeLabviewBlock] == this )
        {
            TFile* oldfile = _treeLabviewBlock->GetCurrentFile();
            _treeLabviewBlock->Fill();
            TFile* newfile = _treeLabviewBlock->GetCurrentFile();
            if (oldfile!=newfile)
            {
                RootTreeGenerator3* owner = _fileOwnerMap[oldfile];
                _fileOwnerMap.erase( oldfile );
                _fileOwnerMap[newfile]=owner;
            }
        }
        
    }
    
    void RootTreeGenerator3::end()
    {
        
        if ( _treeOwnerMap[_treeLabviewBlock] == this )
        {
            TFile* treeFile = _treeLabviewBlock->GetCurrentFile();	
            treeFile->Write();
            delete _treeLabviewBlock;
            _treeLabviewBlock=NULL;
        }
        
        if ( _fileOwnerMap[_rootFile] == this )
        {
            _rootFile->Write();
            _rootFile->Close();
            //delete _outFile;
            _rootFile=NULL;
        }
    }
    
    const double RootTreeGenerator3::INVALID = -FLT_MAX;
    
}
