#include "TempRootTreeGenerator.hh"
#include "TROOT.h"
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "TempSensorBlock2.hh"
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
    
  TempRootTreeGenerator aTempRootTreeGenerator;
    
  map<TTree*, TempRootTreeGenerator*> TempRootTreeGenerator::_treeFillerMap;
  map<TTree*, TempRootTreeGenerator*> TempRootTreeGenerator::_treeOwnerMap;
  map<TFile*, TempRootTreeGenerator*> TempRootTreeGenerator::_fileOwnerMap;
    
  TempRootTreeGenerator::TempRootTreeGenerator() : Processor("TempRootTreeGenerator")
  {
    _description = "Processor to generate root tree with Event built";
        
    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection of TempSensorData",
			       _inputColName,
			       std::string("TemperatureSensor"));
        
    registerProcessorParameter("OutputRootFileName",
			       "Name of the output root file",
			       _rootFileName,
			       std::string("Temperature.root"));

    registerProcessorParameter("BranchPrefix",
			       "Name of the prefix of the branch",
			       _prefix,
			       std::string(""));

        
  }
    
  TempRootTreeGenerator::~TempRootTreeGenerator() {}
    
  void TempRootTreeGenerator::init()
  {
    runNumber = 0;
    eventNumber = 0;
    Timestamp = 0;

    _rootFile = gROOT->GetFile(_rootFileName.c_str());
    if ( _rootFile == NULL )
      {
	_rootFile      = new TFile(_rootFileName.c_str(), "RECREATE");
	_fileOwnerMap[ _rootFile ] = this;
      }
        
    _rootFile->cd();
    _treeTempSensorBlock = dynamic_cast<TTree*>( _rootFile->Get("TemperatureSensor"));
        
    if ( _treeTempSensorBlock != NULL )
      streamlog_out(DEBUG) << "Reuse tree ["<< "TemperatureSensor" <<"] " << endl;
    if ( _treeTempSensorBlock == NULL )
      {
	_treeTempSensorBlock = new TTree( "TemperatureSensor" , "all information");
	_treeOwnerMap[_treeTempSensorBlock] = this;
      }
        
    bool firstProcessor = (_treeFillerMap[ _treeTempSensorBlock ] == NULL );
    _treeFillerMap[ _treeTempSensorBlock ] = this;
        
    if ( firstProcessor )
      {
	_treeTempSensorBlock->Branch( "eventNumber", &eventNumber, "EventNumber/I");
	_treeTempSensorBlock->Branch( "runNumber", &runNumber, "RunNumber/I");
	_treeTempSensorBlock->Branch( "Timestamp", &Timestamp, "Timestamp/L");
      }
        
    registerBranches(_treeTempSensorBlock);
  }
    
  void TempRootTreeGenerator::registerBranches( TTree* hostTree )
  {
    hostTree->Branch( string(_prefix + "nLayers").c_str(), &_hFill.nLayers,  string(_prefix+"nLayers/I").c_str());
    hostTree->Branch( string(_prefix + "T1").c_str(), &_hFill.T1, string(_prefix+"T1["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "T2").c_str(), &_hFill.T2, string(_prefix+"T2["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "T3").c_str(), &_hFill.T3, string(_prefix+"T3["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "T4").c_str(), &_hFill.T4, string(_prefix+"T4["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "T5").c_str(), &_hFill.T5, string(_prefix+"T5["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "T6").c_str(), &_hFill.T6, string(_prefix+"T6["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "TDIF").c_str(), &_hFill.TDIF, string(_prefix+"TDIF["+_prefix+"nLayers]/F").c_str() );
    hostTree->Branch( string(_prefix + "TPWR").c_str(), &_hFill.TPWR, string(_prefix+"TPWR["+_prefix+"nLayers]/F").c_str() );

  }
      
  void TempRootTreeGenerator::processEvent(LCEvent* evt)
  {
    LCCollection* col ;
    try
      {
	//fetch Labview data raw collection
	col = evt->getCollection( _inputColName ) ;

	runNumber   = evt->getRunNumber();
	eventNumber = evt->getEventNumber();
	Timestamp   = evt->getTimeStamp();//needs to be long64
        //Timestamp= col->getParameters().getIntVal("Timestamp_i");//TimeStamp();//airqui                                                                                         
	streamlog_out(MESSAGE) <<"runNumber= "<<runNumber<<" eventNumber= "<<eventNumber<<" Timestamp= "<< Timestamp<<endl;
	nLayers = 0;
	  
	for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
	  {
	    LCObject *obj = col->getElementAt(ielm);
	    TempSensorBlock2 lBlock(obj);
	    //	    std::cout<<col->getNumberOfElements()<<endl;
	    float t1=lBlock.GetT1()/10.;
            float t2=lBlock.GetT2()/10.;
            float t3=lBlock.GetT3()/10.;
            float t4=lBlock.GetT4()/10.;
            float t5=lBlock.GetT5()/10.;
            float t6=lBlock.GetT6()/10.;
            float tdif=lBlock.GetTDIF();
            float tpwr=lBlock.GetTPWR();

	    streamlog_out(MESSAGE) << "Temperatures : " << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6 << " " << tdif << " " << tpwr << endl;

	    _hFill.T1[nLayers] = t1;
	    _hFill.T2[nLayers] = t2;
	    _hFill.T3[nLayers] = t3;
	    _hFill.T4[nLayers] = t4;
	    _hFill.T5[nLayers] = t5;
	    _hFill.T6[nLayers] = t6;
	    _hFill.TDIF[nLayers] = tdif;
	    _hFill.TPWR[nLayers] = tpwr;
	   
	    nLayers ++;

	  }
                  
	_hFill.nLayers = nLayers;
   
      
	if ( _treeFillerMap[_treeTempSensorBlock] == this )
	  {
	    TFile* oldfile = _treeTempSensorBlock->GetCurrentFile();
	    _treeTempSensorBlock->Fill();
	    TFile* newfile = _treeTempSensorBlock->GetCurrentFile();
	    if (oldfile!=newfile)
	      {
		TempRootTreeGenerator* owner = _fileOwnerMap[oldfile];
		_fileOwnerMap.erase( oldfile );
		_fileOwnerMap[newfile]=owner;
	      }
	  }
      }
    catch (  DataNotAvailableException err )
      {
	//	    std::cout <<  "TempRootTreeGenerator WARNING: Collection "<< _inputColName
	//	 << " not available in event "<< evt->getEventNumber() << endl;
	return;
      }
        
  }
    
  void TempRootTreeGenerator::end()
  {
        
    if ( _treeOwnerMap[_treeTempSensorBlock] == this )
      {
	TFile* treeFile = _treeTempSensorBlock->GetCurrentFile();	
	treeFile->Write();
	delete _treeTempSensorBlock;
	_treeTempSensorBlock=NULL;
      }
        
    if ( _fileOwnerMap[_rootFile] == this )
      {
	_rootFile->Write();
	_rootFile->Close();
	//delete _outFile;
	_rootFile=NULL;
      }
  }
    
  const double TempRootTreeGenerator::INVALID = -FLT_MAX;
    
}
