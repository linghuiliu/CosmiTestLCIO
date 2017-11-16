#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "TFile.h"

#include "lcio.h"
#include "EVENT/LCGenericObject.h"
#include "EVENT/LCCollection.h"

#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/LCCollectionVec.h"

#include "collection_names.hh"

#include "TBTrackBaseProcessor.hh"

using namespace lcio;
using namespace marlin;
using namespace TBTrack;

using std::cout;
using std::endl;

/* Not for an abstract class
TBTrackBaseProcessor aTBTrackBaseProcessor;
*/

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
TBTrackBaseProcessor::TBTrackBaseProcessor(const std::string &n) : Processor(n), _hFile(0) 
{
  _description = "Base class for TBTrack processors";

  _printLevel=0;
  registerProcessorParameter("PrintLevel", 
			     "Print level",
			     _printLevel,_printLevel);

  _printEventNumber=-1;
  registerProcessorParameter("PrintEventNumber", 
			     "Print event number",
			     _printEventNumber,_printEventNumber);

  _printEventLevel=3;
  registerProcessorParameter("PrintEventLevel", 
			     "Print event level",
			     _printEventLevel,_printEventLevel);

  _simTrackerHitCollection="TBSimTrackerHits";
  registerProcessorParameter("SimTrackerHitCollection",
			     "SimTrackerHit collection name",
			     _simTrackerHitCollection,
			     _simTrackerHitCollection);

  _tdcHitCollection="TBTrackTdcHits";
  registerProcessorParameter("TdcHitCollection",
			     "TdcHit collection name",
			     _tdcHitCollection,
			     _tdcHitCollection);

  _trackProjectionCollection="TBTrackProjection";
  registerProcessorParameter("TrackProjectionCollection",
			     "TrackProjection collection name",
			     _trackProjectionCollection,
			     _trackProjectionCollection);

  _obsoleteMokkaCollections=0;
  registerProcessorParameter("ObsoleteMokkaCollections",
			     "For Mokka sim using multiple DC Collections",
			     _obsoleteMokkaCollections,
			     _obsoleteMokkaCollections);

  _beamMomentum=0;
  registerProcessorParameter("beamMomentum",
			     "beam momentum: WARNING will overwrite what is in database for data runs!",
			     _beamMomentum,
			     _beamMomentum);

}


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
TBTrackBaseProcessor::~TBTrackBaseProcessor() { }

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::init() 
{
  _iCall=0;
  
  // Flag constants as invalid
  _mapConstantsUpdated = false;
  _mapConstantsValid   = false;

  _simConstantsUpdated = false;
  _simConstantsValid   = false;

  _alnConstantsUpdated = false;
  _alnConstantsValid   = false;

  _fitConstantsUpdated = false;
  _fitConstantsValid   = false;

  _runInformationUpdated = false;
  _runInformationValid   = false;

  streamlog_out(DEBUG) << "Constants initialised to invalid" << endl;
  // Initialise run and event numbers
  _nRun = 0;
  _nEvt = 0;
  _iRun = -1;
  _iEvt = -1;

  // Print processor parameters
  printParameters();

  // Call inherited method
  Init();
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::processRunHeader(LCRunHeader* run) 
{
  _iRun = run->getRunNumber();
  _iEvt = -1;
  _iCall = 1;

  streamlog_out(DEBUG)<< " Processing run " << _nRun
	      << " with run number " 
	      << run->getRunNumber() << endl;
  
  // Fill the local copies of the constants
  _runInformation.get(run);
  _runInformationValid = true; // Should be dependent on get() above
  if(printLevel(1)) _runInformation.print();

  std::ostringstream sout;
  if(_runInformation.isMC()) sout << "Sim";
  else                       sout << "Dat";
  sout << std::setw(6) << std::setfill('0') << _iRun;
  _runString = sout.str();


  std::string year=_runInformation.runMonth().substr(_runInformation.runMonth().size()-2, 2);

  if (_runInformation.location() == "Desy") 
    { 
      _tdcRawDataCollection=COL_TDC;
    } 
  else if (_runInformation.location() == "Cern") 
    {
      if ( year == "06" || year == "07" ) 
	{
	  _tdcRawDataCollection = COL_CAEN767TDC;
	} 
      else 
	{ // 2010, 2011 WHCAL
	  _tdcRawDataCollection = COL_CAENTDC;
	}
    } 
  else if (_runInformation.location() == "Fnal") 
    {
      _tdcRawDataCollection=COL_CAEN767TDC;
    }
  std::cout << "----  " << name() << ", Name of raw tracks collection: " << _tdcRawDataCollection << std::endl;

  //_runInformation.print();
  // Call inherited method
  ProcessRunHeader(run);

  _nRun++;
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::processEvent(LCEvent *evt) 
{
  const int printLevelSave(_printLevel);
  if(_nEvt == _printEventNumber) _printLevel = _printEventLevel;

  _iEvt  = evt->getEventNumber();
  _iCall = 2;

  streamlog_out(DEBUG) << " TBtrackBaseProcessor:Processing event " << _nEvt
	       << " with event number " 
	       << evt->getEventNumber() << endl;
  
  // Fill the local copies of the constants
  getEvtConstants(evt);

  // Call inherited method
  ProcessEvent(evt);

  streamlog_out(DEBUG) << "--- TBTrackBaseProcessor::processEvent() End of processing : " << name() << endl;

  _printLevel = printLevelSave;
  _nEvt++;
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::end() 
{
  _iRun = -1;
  _iEvt = -1;
  _iCall = 3;

  streamlog_out(DEBUG) << " Processed " << _nEvt << " events in "
	       << _nRun << " runs " << endl;
  
  // Call inherited method
  End();
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
bool TBTrackBaseProcessor::printLevel(int p, bool b) const 
{
  if(p > _printLevel) return false;

  if(b) 
    {
      if(p >= -1) 
	{
	  cout << endl << name() << "::";
	  if(_iCall<=0) cout << "Init()";
	  if(_iCall==1) cout << "ProcessRunHeader()";
	  if(_iCall==2) cout << "ProcessEvent()";
	  if(_iCall>=3) cout << "End()";
	  
	  if(_iRun>=0) cout << " Run " << _iRun;
	  if(_iEvt>=0) cout << " Event " << _iEvt;
	  
	  if(p==-1) cout << " WARNING";
	  cout << endl << " ";
	  
	} 
      else 
	{
	  std::cerr << name() << "::";
	  if(_iCall<=0) std::cerr << "Init()";
	  if(_iCall==1) std::cerr << "ProcessRunHeader()";
	  if(_iCall==2) std::cerr << "ProcessEvent()";
	  if(_iCall>=3) std::cerr << "End()";
	  
	  if(_iRun>=0) std::cerr << " Run " << _iRun;
	  if(_iEvt>=0) std::cerr << " Event " << _iEvt;
	  
	  if(p==-2) std::cerr << " ERROR";
	  if(p<=-3) std::cerr << " FATAL";
	  std::cerr << endl << " ";
	}
    }
  
  return true;
}


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::getEvtConstants(const LCEvent *evt) 
{
  const LCCollection *c(0);

  if( (c = getCollection(evt,"TBTrackMapConstants",LCIO::LCGENERICOBJECT,false)) != 0) 
    {
      assert(c->getNumberOfElements() == 1);
      
      const LCPayload<MapConstants> p(dynamic_cast<LCGenericObject*>(c->getElementAt(0)));
      _mapConstants        = p.payload();
      _mapConstantsUpdated = true;
      _mapConstantsValid   = true;
      
      if(printLevel(1)) _mapConstants.print() << endl;
    } 
  else 
    {
      _mapConstantsUpdated = false;
    }

  if( (c = getCollection(evt,"TBTrackSimConstants",LCIO::LCGENERICOBJECT,false)) != 0) 
    {
      assert(c->getNumberOfElements() == 1);
      const LCPayload<SimConstants> p(dynamic_cast<LCGenericObject*>(c->getElementAt(0)));
      _simConstants        = p.payload();
      _simConstantsUpdated = true;
      _simConstantsValid   = true;
      if(printLevel(1)) _simConstants.print() << endl;
    } 
  else 
    {
      _simConstantsUpdated=false;
    }

  if( (c=getCollection(evt,"TBTrackAlnConstants", LCIO::LCGENERICOBJECT,false)) != 0) 
    {
      assert(c->getNumberOfElements()==1);
      const LCPayload<AlnConstants> p(dynamic_cast<LCGenericObject*>(c->getElementAt(0)));
      _alnConstants        = p.payload();
      _alnConstantsUpdated = true;
      _alnConstantsValid   = true;
      
      if(printLevel(1)) _alnConstants.print() << endl;
  } 
  else 
    {
      _alnConstantsUpdated = false;
    }
  
  if( (c = getCollection(evt,"TBTrackFitConstants",LCIO::LCGENERICOBJECT,false)) != 0) 
    {
      assert(c->getNumberOfElements() == 1);
      const LCPayload<FitConstants> p(dynamic_cast<LCGenericObject*>(c->getElementAt(0)));
      _fitConstants        = p.payload();
      _fitConstantsUpdated = true;
      _fitConstantsValid   = true;
      
      if(printLevel(1)) _fitConstants.print() << std::endl;
    } 
  else 
    {
      _fitConstantsUpdated=false;
    }
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
const EVENT::LCCollection* TBTrackBaseProcessor::getCollection(const LCEvent *evt, 
							       const std::string &name,
							       const std::string &type,
							       bool allowPrint) const 
{
  const EVENT::LCCollection *col = NULL;
  
  try {
    col = evt->getCollection(name);
    if(col != 0) 
      {
	streamlog_out(DEBUG)<<"Got collection "<<col<<" with name "<<name<<" and type "<<col->getTypeName()<<endl;

	if(col->getTypeName() != type) 
	  {
	    if(allowPrint && printLevel(-1)) 
	      {
		cout << "getCollection() Collection name " << name 
		     << " with requested type " << type
		     << " has actual type " << col->getTypeName() << endl;
	      }
	    col = 0;
	  }
      } 
    else 
      {
	if(allowPrint && printLevel(-1)) 
	  {
	    cout << "getCollection() Collection name " << name 
		 << " returns null pointer" << endl;
	  }
      }
    
  } 
  catch (DataNotAvailableException &e) 
    {
      col = 0;
      if(allowPrint && printLevel(-1)) 
	{
	  cout << "getCollection() Collection name " << name 
	       << " not found" << endl;
	}
    }
  
  return col;
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
bool TBTrackBaseProcessor::addCollection(EVENT::LCEvent *evt, 
					 EVENT::LCCollection *c,
					 const std::string &name,
					 bool allowPrint) const 
{
  try 
    {
      evt->addCollection(c,name);
      return true;      
    } 
  catch (EventException &e) 
    {
      if(allowPrint && printLevel(-1)) 
	{
	  cout << "addCollection() Collection name " << name 
	       << " already exists" << endl;
	}
    }
  return false;
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::openHFile(const EVENT::LCRunHeader *run) 
{
  // Define the current directory so that root don't put a mess ....
  //TDirectory* current = gDirectory;
  
  // Create an output file name with the run number
  /*
  std::ostringstream sout;
  sout << name();
  if(_runInformation.isMC()) sout << "Sim";
  else                       sout << "Dat";
  if(run->getRunNumber()==0) sout << "00000";
  sout << run->getRunNumber() << ".root";
  */

  std::string fileName(name()+_runString+".root");
  if(printLevel(1)) std::cout << "Recreating ROOT file "
			      << fileName << std::endl;
  
  // Create the output file
  if((_hFile=TFile::Open(fileName.c_str(),"RECREATE"))==0) {
    if(printLevel(-3)) std::cerr << "Cannot recreate ROOT file "
				 << fileName << std::endl;
    assert(false);
  }
  
  /*by default when a file is created, if the functions hfile->Write() and hfile->Close() are called at the end, 
    every ROOT object defined in the include file will be saved, without having to call individually write().*/
  
  //hfile->mkdir("CHECK histograms")->cd();
}
 
/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::closeHFile() 
{
  if(_hFile != 0) 
    {
      streamlog_out(DEBUG) << "Closing histogram file" << endl;
      
      _hFile->Write();
      _hFile->Close();
      
      delete _hFile;
      _hFile = 0;
    } 
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackBaseProcessor::getSimTrackerHits(LCEvent *evt) 
{
  const LCCollection *c(0);
  
  LCCollectionVec* outdcCol(new LCCollectionVec(LCIO::SIMTRACKERHIT));
  addCollection(evt,outdcCol,_simTrackerHitCollection); // Only if !=0?
  
  std::string _dc0ColName,_dc1ColName,_dc2ColName,_dc3ColName;
  
  // DESY; DC1 and DC2 are swapped
  if (_runInformation.location() == RunInformation::desy)
    {
      _dc0ColName="TBdch02_dchSD2";
      _dc1ColName="TBdch02_dchSD1";
      _dc2ColName="TBdch02_dchSD3";
      _dc3ColName="TBdch02_dchSD4";
    }
  else 
    {//cern
      _dc0ColName="DoesNotExist";
      _dc1ColName="TBdch04_01_dchSD1";
      _dc2ColName="TBdch04_01_dchSD2";
      _dc3ColName="TBdch04_01_dchSD3";
    }
  
  for(unsigned layer(0); layer < 4; layer++) 
    {
      if((layer == 0 && (c = getCollection(evt, _dc0ColName,LCIO::SIMTRACKERHIT)) != 0) ||
	 (layer == 1 && (c = getCollection(evt, _dc1ColName,LCIO::SIMTRACKERHIT)) != 0) ||
	 (layer == 2 && (c = getCollection(evt, _dc2ColName,LCIO::SIMTRACKERHIT)) != 0) ||
	 (layer == 3 && (c = getCollection(evt, _dc3ColName,LCIO::SIMTRACKERHIT)) != 0)) 
	{
	
	for(int i(0); i<c->getNumberOfElements(); i++) 
	  {
	    const SimTrackerHit *p(dynamic_cast<const SimTrackerHit*>(c->getElementAt(i)));
	    SimTrackerHitImpl *q(new SimTrackerHitImpl);
	    
	    const double *r(p->getPosition());

	    double rr[3];
	    rr[0] = r[0];
	    rr[1] = r[1];
	    rr[2] = r[2];
	    q->setPosition(rr);

	    double zx(fabs(rr[2]-_fitConstants.zLayer(0,layer)));
	    double zy(fabs(rr[2]-_fitConstants.zLayer(1,layer)));

	    streamlog_out(DEBUG)<< "Z, DZ for " << i << " = " << rr[2] << ", " << _fitConstants.zLayer(0,layer)
			<< " or " << _fitConstants.zLayer(1,layer) << " so " << zx << ", " << zy << endl;
	    
	    if(zx<zy) q->setCellID0(2*layer  );
	    else      q->setCellID(02*layer+1);
	    
	    q->setdEdx(p->getdEdx());
	    q->setTime(p->getTime());
	    
	    q->setMCParticle(p->getMCParticle());
	    
	    const float *m(p->getMomentum());
	    q->setMomentum(m[0],m[1],m[2]);
	    
	    q->setPathLength(p->getPathLength());
	    
	    outdcCol->addElement(q);
	  }
	}
    }
  
  
}
