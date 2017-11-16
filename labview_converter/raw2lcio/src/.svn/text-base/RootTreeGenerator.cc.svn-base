#include "RootTreeGenerator.hh"

#include "TROOT.h"

#include "lcio.h"
#include "EVENT/LCCollection.h"

#include "LabviewBlock2.hh"

#include <iostream>

using namespace std;

namespace CALICE {
    
    RootTreeGenerator aRootTreeGenerator;
    
    map<TTree*, RootTreeGenerator*> RootTreeGenerator::_treeFillerMap;
    map<TTree*, RootTreeGenerator*> RootTreeGenerator::_treeOwnerMap;
    map<TFile*, RootTreeGenerator*> RootTreeGenerator::_fileOwnerMap;
    
    RootTreeGenerator::RootTreeGenerator() : Processor("RootTreeGenerator") {
        _description = "Processor to generate root tree for T0 currently";
        
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
    
    
    RootTreeGenerator::~RootTreeGenerator() {}
    
    void RootTreeGenerator::init() {
        
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
        
        /*
         _treeT0 = new TTree( "T0", "First T0 chip129/channel35 and chip137/channel35 information" );
         _treeT0->Branch( "T01", &_T01, "T01/I" );
         _treeT0->Branch( "T02", &_T02, "T02/I" );
         _treeT0->Branch( "A01", &_A01, "A01/I" );
         _treeT0->Branch( "A02", &_A02, "A02/I" );
         _treeT0->Branch( "HB01", &_HB01, "HB01/I" );
         _treeT0->Branch( "HB02", &_HB02, "HB02/I" );
         _treeT0->Branch( "BX01", &_BX01, "BX01/I" );
         _treeT0->Branch( "BX02", &_BX02, "BX02/I" );
         _treeT0->Branch( "CN01", &_CN01, "CN01/I" );
         _treeT0->Branch( "CN02", &_CN02, "CN02/I" );
         _treeT0->Branch( "nHits", &_nHits, "nHits/I" );
         */
        
        
        if ( firstProcessor )
        {
            _treeLabviewBlock->Branch( (_prefix + "BunchXID").c_str(), &_BunchXID, "BunchXID/I" );
            _treeLabviewBlock->Branch( (_prefix + "CycleNr").c_str(), &_CycleNr, "CycleNr/I" );
            _treeLabviewBlock->Branch( (_prefix + "ChipID").c_str(), &_ChipID, "ChipID/I" );
            //_treeLabviewBlock->Branch( "ASICNr", &_ASICNr, "ASICNr/I" );
            _treeLabviewBlock->Branch( (_prefix + "EvtNr").c_str(), &_EvtNr, "EvtNr/I" );
            _treeLabviewBlock->Branch( (_prefix + "Channel").c_str(), &_Channel, "Channel/I" );
            _treeLabviewBlock->Branch( (_prefix + "TDC").c_str(), &_TDC, "TDC/I" );
            _treeLabviewBlock->Branch( (_prefix + "ADC").c_str(), &_ADC, "ADC/I" );
            //_treeLabviewBlock->Branch( "xPos", &_xPos, "xPos/I" );
            //_treeLabviewBlock->Branch( "yPos", &_yPos, "yPos/I" );
            _treeLabviewBlock->Branch( (_prefix + "HitBit").c_str(), &_HitBit, "HitBit/I" );
            _treeLabviewBlock->Branch( (_prefix + "GainBit").c_str(), &_GainBit, "GainBit/I" );
        }
    }
    
    void RootTreeGenerator::processEvent(LCEvent* evt){
        
        try {
            //fetch Labview data raw collection
            LCCollection* col = evt->getCollection( _inputColName ) ;
            
            //count elements in ChipID 129:0, 130:1, ... 144:15
            //init counter to 0
            /*
             int counter[16];
             for (int i = 0; i<16;i++){
             counter[i]=0;
             }
             */
            //bool _T01_OK = false;
            //bool _T02_OK = false;
            
            _nHits = 0;
            
            //std::cout << "1" << std::endl;
            
            //check all the ChipID, each one has 36 channels,
            
            //std::cout << "static_cast<unsigned int>(col->getNumberOfElements()) = " << static_cast<unsigned int>(col->getNumberOfElements()) <<  std::endl;
            
            for (unsigned int ielm=0; ielm < static_cast<unsigned int>(col->getNumberOfElements()); ielm++) {
                
                //std::cout << "ielm " << ielm <<  std::endl;
                
                LCObject *obj = col->getElementAt(ielm);
                LabviewBlock2 lBlock(obj);
                
                
                _BunchXID = lBlock.GetBunchXID();
                _CycleNr = lBlock.GetCycleNr();
                _ChipID  = lBlock.GetChipID();
                //_ASICNr  = lBlock.GetASICNr();
                _EvtNr   = lBlock.GetEvtNr();
                _Channel = lBlock.GetChannel();
                _TDC     = lBlock.GetTDC();
                _ADC     = lBlock.GetADC();
                //_xPos    = lBlock.GetXPos();
                //_yPos    = lBlock.GetYPos();
                _HitBit  = lBlock.GetHitBit();
                _GainBit = lBlock.GetGainBit();
                
                //std::cout << "_EvtNr : " << _EvtNr <<  std::endl;
                
                _treeLabviewBlock->Fill();
                
                
                //change the ChipID [129,144] to CID [0,15]
                //i.e. 129->0, 130->1, ... 144->15
                //int CID = lBlock.GetChipID() - 129;
                
                //counter[CID]++;
                
                /*
                 if( lBlock.GetChipID() == 129 && lBlock.GetChannel() == 35){
                 _T01 = lBlock.GetTDC();
                 _A01 = lBlock.GetADC();
                 _HB01 = lBlock.GetHitBit();
                 _BX01 = lBlock.GetBunchXID();
                 _CN01 = lBlock.GetCycleNr();
                 if ( _HB01 == 1) _T01_OK = true;
                 }	
                 
                 if( lBlock.GetChipID() == 137 && lBlock.GetChannel() == 35){
                 _T02 = lBlock.GetTDC();
                 _A02 = lBlock.GetADC();
                 _HB02 = lBlock.GetHitBit();
                 _BX02 = lBlock.GetBunchXID();
                 _CN02 = lBlock.GetCycleNr();
                 if ( _HB02 == 1) _T02_OK = true;
                 }	
                 */ 
                int HitBit = lBlock.GetHitBit();
                if ( HitBit == 1 ) _nHits++;
                
            }
            
            //std::cout << "2" << std::endl;
            
            /*
             for (int i = 0; i<16;i++){
             if( counter[i] > 0 && counter[i] != 36 ){
             std::cout<<"Event number: "<< evt->getEventNumber()
             <<"     ChipID: "      << (i+129) //print in ChipID number, i.e. 0->129, 1->130, ... 15->144
             <<"     Entries: "     << counter[i]
             <<std::endl;
             return;
             }
             }
             */
            /*
             if( (_T01_OK == true) && (_T02_OK == true) ) {
             streamlog_out(DEBUG0) << " Filing Tree ......"<<std::endl;
             _treeT0->Fill();
             }
             */
            
        } catch (  lcio::DataNotAvailableException &err ) {
            err.what();
            return;
        }
        
        if ( _treeFillerMap[_treeLabviewBlock] == this )
        {
            TFile* oldfile = _treeLabviewBlock->GetCurrentFile();
            _treeLabviewBlock->Fill();
            TFile* newfile = _treeLabviewBlock->GetCurrentFile();
            if (oldfile!=newfile)
            {
                RootTreeGenerator* owner = _fileOwnerMap[oldfile];
                _fileOwnerMap.erase( oldfile );
                _fileOwnerMap[newfile]=owner;
            }
        }

        
    }
    
    void RootTreeGenerator::end()
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
    
}
