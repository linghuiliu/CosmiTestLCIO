#include "RootTreeWriter.hh"

#include "TROOT.h"
#include "marlin/StringParameters.h"

#include <cfloat>
#include <functional>
#include <ext/functional>

#include "EngineRegistrar.hh"

using namespace lcio;
using namespace std;


//****************************************************
// Add header files for new write engines here

#include "EventPropertiesWriteEngine.hh"
#include "EnginesInclude.icc"
//****************************************************



namespace marlin
{

  RootTreeWriter aRootTreeWriter;

  map<TTree*, RootTreeWriter*> RootTreeWriter::_treeFillerMap;
  map<TTree*, RootTreeWriter*> RootTreeWriter::_treeOwnerMap;
  map<TFile*, RootTreeWriter*> RootTreeWriter::_fileOwnerMap;

  RootTreeWriter::RootTreeWriter():Processor("RootTreeWriter")
  {

    //***********************************************************
    // Add new engines here
//    _writeEngineList.push_back( new EventPropertiesWriteEngine( this )    );
#   include "EnginesAdd.icc"
    __RTW::EngineRegistrar::TheInstance().appendAllEngines( _writeEngineList, this );
    //***********************************************************


    _description="Single processor for all the plotting";

    registerProcessorParameter( "OutFileName","Name of the output ROOT file",
				_rootFileName, std::string("plots.root") );

    registerOptionalParameter( "OutFileMode","Mode for opening ROOT file",
			       _rootFileMode, std::string("RECREATE")      );

    registerOptionalParameter( "RootTreeName","Name of the output TTree",
			       _rootTreeName, std::string("bigtree") );

    
    /// \todo: register enable/disable switch for each engine
    /// \todo: register prefix for each engine
    size_t numEngines = _writeEngineList.size();
    //    _engineNames.resize( nu_Engines );
    _engineEnable.resize( numEngines );

    size_t engineIndex=0;

    for ( WriteEngineVIt pWE = _writeEngineList.begin(); 
	  pWE != _writeEngineList.end(); ++pWE          )
      {
	// _engineNames[procInd] = (*pWE)->getEngineName();
	std::string engineName = (*pWE)->getEngineName();
	registerProcessorParameter(engineName+"_enable","enable(1) or disable(0) write engine "+engineName,
				   _engineEnable[ engineIndex ], 0                                         );
	(*pWE)->registerParameters();
	
	engineIndex++;
      }

  }

  bool RootTreeWriter::checkUsedEnginesExist()
  {
    /// \todo fixme: file feature request to Marlin bug-tracker. Need to check for 
    /// "unregistered" parameters.
    StringVec paramkeys;
    parameters()->getStringKeys(paramkeys);
    StringVec enabledEngines;
    // collect all enabled engines;
    for ( StringVec::iterator keyIt = paramkeys.begin(); keyIt != paramkeys.end(); ++keyIt )
      {
	size_t suffixPos =  keyIt->find("_enable");
	// substring "_enable" at the end of key...
	if ( suffixPos != string::npos 
	     && suffixPos == ( keyIt->size()-string("_enable").size()) )
	  {
	    int enabled = parameters()->getIntVal( *keyIt );
	    if ( enabled !=0 )
	      {
		string engineName( *keyIt, 0 ,suffixPos );
		enabledEngines.push_back(engineName);
	      }
	  }
      }
    // check all enabled engines for existence
    for ( StringVec::iterator engIt = enabledEngines.begin();
	  engIt != enabledEngines.end(); ++engIt )
      {

	WriteEngineVec::iterator foundEngIt = find_if( _writeEngineList.begin(),
						       _writeEngineList.end(),
						       RTW::EngineNameIs(*engIt) );
	// not found? that's baaaaaaaaad..
	if ( foundEngIt == _writeEngineList.end() )
	  {
	    cerr << "RootTreeWriter(): Fatal: Engine ["<<*engIt<<"] is enabled but does not exist. "
	      "Don't know what to do, but to panic. Thus: PANIC!..." << endl;
	    cerr << "Exit ungraceful!. (Please complain at Marlin authors for missing "
	      "handling of fatal and non fatal errors... 'kill -9 $$'... )" <<  endl;
	    exit( -1 );
	    
	    return false;
	  }
      }

    return true;
  }


  void RootTreeWriter::init()
  {

    //printParameters( clog );
    printParameters();
    checkUsedEnginesExist();

    // ------- open output file ----------
    _outFile = gROOT->GetFile(_rootFileName.c_str());
    if ( _outFile == NULL )
      {
	_outFile      = new TFile(_rootFileName.c_str(),_rootFileMode.c_str());
	_fileOwnerMap[ _outFile ] = this;
      }
    /// \todo check whether file was successfully opened;


    // -------- create tree, register branches -----------------------

    _outFile->cd();
    _bigTree = dynamic_cast<TTree*>( _outFile->Get( _rootTreeName.c_str() ));
    if ( _bigTree != NULL )
      streamlog_out(DEBUG) << "Reuse tree ["<<_rootTreeName<<"] " << endl;
    if ( _bigTree == NULL )
      {
	_bigTree = new TTree( _rootTreeName.c_str(), _rootTreeName.c_str());
	_treeOwnerMap[_bigTree] = this;
	//	_bigTree->SetMaxTreeSize(kMaxLong64);
      }
    
    bool firstProcessor = (_treeFillerMap[ _bigTree ] == NULL );
    _treeFillerMap[ _bigTree ] = this;
    
    if ( firstProcessor )
      {
	_bigTree->Branch("runNumber"  ,&_runEventStruct.runNumber,  "runNumber/I");
	_bigTree->Branch("eventNumber",&_runEventStruct.eventNumber,"eventNumber/I");
	_bigTree->Branch("eventTime"  ,&_runEventStruct.eventTime,  "eventTime/L");
      }

    size_t engineIndex=0;
    for ( WriteEngineVIt pWE = _writeEngineList.begin(); 
	  pWE != _writeEngineList.end(); ++pWE           )
      if ( _engineEnable[engineIndex++] != 0 )
	(*pWE)->registerBranches( _bigTree );
 }
  
  void RootTreeWriter::end()
  {
    if ( _treeOwnerMap[_bigTree] == this )
      {
	TFile* treeFile = _bigTree->GetCurrentFile();	
	treeFile->Write();
	delete _bigTree;
	_bigTree=NULL;
      }

    if ( _fileOwnerMap[_outFile] == this )
      {
	_outFile->Write();
	_outFile->Close();
	//delete _outFile;
	_outFile=NULL;
      }

    for ( WriteEngineVIt pWE = _writeEngineList.begin(); 
	  pWE != _writeEngineList.end(); ++pWE           )
      delete *pWE;

    _writeEngineList.clear();
  }


  void RootTreeWriter::processEvent( LCEvent* evt )
    //  void RootTreeWriter::check( LCEvent* evt )
  {

    _runEventStruct.runNumber   = evt->getRunNumber();
    _runEventStruct.eventNumber = evt->getEventNumber();
    _runEventStruct.eventTime   = evt->getTimeStamp();

    size_t engineIndex=0;
    for ( WriteEngineVIt pWE = _writeEngineList.begin(); 
	  pWE != _writeEngineList.end(); ++pWE           )
      if ( _engineEnable[engineIndex++] != 0 )
	(*pWE)->fillVariables( evt );
    
    if ( _treeFillerMap[_bigTree] == this )
      {
	TFile* oldfile = _bigTree->GetCurrentFile();
	_bigTree->Fill();
	TFile* newfile = _bigTree->GetCurrentFile();
	if (oldfile!=newfile)
	  {
	    RootTreeWriter* owner = _fileOwnerMap[oldfile];
	    _fileOwnerMap.erase( oldfile );
	    _fileOwnerMap[newfile]=owner;
	  }
      }

  }

  //================================================================

  const double RootTreeWriter::INVALID = -FLT_MAX;



}//namespace marlin
