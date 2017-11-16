#include "RootTreeGeneratorEUDAQ2016.hh"
#include "TROOT.h"
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EUDAQBlock2016.hh"
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
    
  RootTreeGeneratorEUDAQ2016 aRootTreeGeneratorEUDAQ2016;
    
  map<TTree*, RootTreeGeneratorEUDAQ2016*> RootTreeGeneratorEUDAQ2016::_treeFillerMap;
  map<TTree*, RootTreeGeneratorEUDAQ2016*> RootTreeGeneratorEUDAQ2016::_treeOwnerMap;
  map<TFile*, RootTreeGeneratorEUDAQ2016*> RootTreeGeneratorEUDAQ2016::_fileOwnerMap;
    
  RootTreeGeneratorEUDAQ2016::RootTreeGeneratorEUDAQ2016() : Processor("RootTreeGeneratorEUDAQ2016")
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
    
  RootTreeGeneratorEUDAQ2016::~RootTreeGeneratorEUDAQ2016() {}
    
  void RootTreeGeneratorEUDAQ2016::init()
  {
        
    _rootFile = gROOT->GetFile(_rootFileName.c_str());
    if ( _rootFile == NULL )
      {
	_rootFile      = new TFile(_rootFileName.c_str(), "RECREATE");
	_fileOwnerMap[ _rootFile ] = this;
      }
        
    _rootFile->cd();
    _treeEUDAQBlock2016 = dynamic_cast<TTree*>( _rootFile->Get("LabviewData"));
        
    if ( _treeEUDAQBlock2016 != NULL )
      streamlog_out(DEBUG) << "Reuse tree ["<< "LabviewData" <<"] " << endl;
    if ( _treeEUDAQBlock2016 == NULL )
      {
	_treeEUDAQBlock2016 = new TTree( "LabviewData" , "all information");
	_treeOwnerMap[_treeEUDAQBlock2016] = this;
      }
        
    bool firstProcessor = (_treeFillerMap[ _treeEUDAQBlock2016 ] == NULL );
    _treeFillerMap[ _treeEUDAQBlock2016 ] = this;
        
    if ( firstProcessor )
      {
	_treeEUDAQBlock2016->Branch( "eventNumber", &eventNumber, "EventNumber/I");
	_treeEUDAQBlock2016->Branch( "runNumber", &runNumber, "RunNumber/I");
	_treeEUDAQBlock2016->Branch( "Timestamp", &Timestamp, "Timestamp/I");
      }
        
    registerBranches(_treeEUDAQBlock2016);
  }
    
  void RootTreeGeneratorEUDAQ2016::registerBranches( TTree* hostTree )
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
    
  void RootTreeGeneratorEUDAQ2016::FillVariable(LCEvent* evt)
  {
        
    LCCollection* col ;
    try
      {
	//fetch Labview data raw collection
	col = evt->getCollection( _inputColName ) ;
            
	//            cout <<"\n Event "<<evt->getEventNumber()<<", loop over collection "<< _inputColName
	//		 <<" with "<< col->getNumberOfElements()<<" elements "<<endl;



	nHits = 0;

	for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
	  {
	    LCObject *obj = col->getElementAt(ielm);
	    EUDAQBlock2016 lBlock(obj);

	    std::vector<int> tdc = lBlock.GetTDC();
	    std::vector<int> adc = lBlock.GetADC();
	    int bxid= lBlock.GetBunchXID();
	    int cycle = lBlock.GetCycleNr();
	    int chipid = lBlock.GetChipID();
	    int evtnr = lBlock.GetEvtNr();

	    for(int ichan = 0; ichan < lBlock.GetNChannels();  ichan++) {

	      streamlog_out(DEBUG)<< "loop into the " << _inputColName<<" , chip="<<chipid<< " channel="<<ichan<< std:: endl;

	      // data sanity checks
	      // In principle, we should never get a warning at this step since the checks should be already done in the EventBuilder
	      double adc_v=0, tdc_v=0, hb_adc=-1, hb_tdc=-1, gb_adc=-1, gb_tdc=-1;

	      adc_v=adc[ichan]%4096;// adc, tdc
	      tdc_v=tdc[ichan]%4096;

	      hb_adc=(adc[ichan]& 0x1000)?1:0;
	      hb_tdc=(tdc[ichan]& 0x1000)?1:0;

	      gb_adc=(adc[ichan]& 0x2000)?1:0;
	      gb_tdc=(tdc[ichan]& 0x2000)?1:0;

	      if(adc_v >4096 || tdc_v>4096) {
		streamlog_out(WARNING)<< "Wrong adc or tdc value: ADC=" <<adc_v<<" TDC="<<tdc_v<<" cycleNr="<<cycle <<std:: endl;
		continue;
	      }

	      if(hb_adc != hb_tdc)  {
		streamlog_out(WARNING)<< "Uncoherent hit bit value: HB_ADC=" <<hb_adc<<" HB_TDC="<<hb_tdc<<" cycleNr="<<cycle <<std:: endl;
		continue;
	      }

	      if(gb_adc != gb_tdc)  {
		streamlog_out(WARNING)<< "Uncoherent gain bit value:  GB_ADC=" <<gb_adc<<" HB_TDC="<<gb_tdc<<" cycleNr="<<cycle <<std:: endl;
		continue;
	      }

	      if( hb_adc<0 || hb_adc>1 || gb_adc<0 || gb_adc>1 ) {
		streamlog_out(WARNING)<< "Wrong bit value (but equal in ADC/TDC):  HB=" <<hb_adc<<" GB="<<gb_adc<<" cycleNr="<<cycle <<std:: endl;
		continue;
	      }
	      //--------------------------


	      _hFill.BunchXID[nHits] = bxid;
	      _hFill.CycleNr[nHits] = cycle;
	      _hFill.ChipID[nHits]  = chipid;
	      _hFill.EvtNr[nHits]   = evtnr;
	      _hFill.Channel[nHits] = ichan;
		  
	      _hFill.TDC[nHits]     = tdc_v;
	      _hFill.ADC[nHits]     = adc_v;

	      _hFill.HitBit[nHits]  = hb_adc;
	      _hFill.GainBit[nHits] = gb_adc;

	      nHits ++;
	      ievt ++;
	    }
		
	  }
                  
	_hFill.iEvt = ievt;
	_hFill.nHits = nHits;
   
	
      }
    catch (  DataNotAvailableException err )
      {
	//	cout <<  "RootTreeGeneratorEUDAQ2016 WARNING: Collection "<< _inputColName
	//	     << " not available in event "<< evt->getEventNumber() << endl;
	return;
      }
        
  }
    
  void RootTreeGeneratorEUDAQ2016::processEvent(LCEvent* evt)
  {
        
    runNumber   = evt->getRunNumber();
    eventNumber = evt->getEventNumber();
    Timestamp   = evt->getTimeStamp();

    FillVariable(evt);

      
    if ( _treeFillerMap[_treeEUDAQBlock2016] == this )
      {
	TFile* oldfile = _treeEUDAQBlock2016->GetCurrentFile();
	_treeEUDAQBlock2016->Fill();
	TFile* newfile = _treeEUDAQBlock2016->GetCurrentFile();
	if (oldfile!=newfile)
	  {
	    RootTreeGeneratorEUDAQ2016* owner = _fileOwnerMap[oldfile];
	    _fileOwnerMap.erase( oldfile );
	    _fileOwnerMap[newfile]=owner;
	  }
      }
        
  }
    
  void RootTreeGeneratorEUDAQ2016::end()
  {
        
    if ( _treeOwnerMap[_treeEUDAQBlock2016] == this )
      {
	TFile* treeFile = _treeEUDAQBlock2016->GetCurrentFile();	
	treeFile->Write();
	delete _treeEUDAQBlock2016;
	_treeEUDAQBlock2016=NULL;
      }
        
    if ( _fileOwnerMap[_rootFile] == this )
      {
	_rootFile->Write();
	_rootFile->Close();
	//delete _outFile;
	_rootFile=NULL;
      }
  }
    
  const double RootTreeGeneratorEUDAQ2016::INVALID = -FLT_MAX;
    
}
