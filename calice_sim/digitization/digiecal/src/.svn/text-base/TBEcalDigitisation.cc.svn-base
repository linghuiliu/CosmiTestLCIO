#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "TRandom3.h"
#include "TFile.h"
#include "TH1F.h"

#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/LCRelationImpl.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/LCCollection.h"

//userlib and reco includes
#include "CellIndex.hh"
#include "CalibrationFactory.hh"
#include "lccd/LCConditionsMgr.hh"
#include "LCPayload.hh"
#include "NoiseParameterArray_t.hh"
#include "NoiseParameter.hh"
#include "cloneUtils.hh"

#include "marlin/StringParameters.h"

#include "TBEcalDigitisation.hh"

using namespace IMPL;
using EVENT::FloatVec;
using namespace lcio;
using namespace marlin;
using namespace std;
using namespace CALICE;


namespace CALICE {
// Global instance needed to register with the framework
  TBEcalDigitisation aTBEcalDigitisation;

//.. constructor
  TBEcalDigitisation::TBEcalDigitisation() : Processor("TBEcalDigitisation") {
    _description = " Digitization processor";
    
    registerProcessorParameter( "Debug",
				"Debug mode",
				_debug,
				static_cast<int>(0) );
    
    registerProcessorParameter( "InputCollection",
				"Name of the input SimCalorimeterHit collection",
				_inputColName,
				std::string("ProtoDesy0506_ProtoSD03") );
    
    registerProcessorParameter( "OutputCollection",
				"Name of the output raw hits collection",
				_outputColName,
				std::string("MyRawCalorimeterHit") );
    
    registerProcessorParameter( "Raw2SimLinksCollection",
				"Name of the collection with raw to sim links",
				_relColName,
				std::string("EcalProtoRaw2sim") );
    
    registerProcessorParameter( "MIPEnergy" , 
				"Energy per MIP (in GeV)",
				_mipEnergy ,
				static_cast<double>(0.000147)) ;
    
    registerProcessorParameter( "InputSeed" , 
				"seed number for random noise definition" ,
				_inputSeed ,
				static_cast<int>(-1)) ;


    registerProcessorParameter( "EmcCellParameters" , 
				"The name of the collection which contains the pedestals, the noise, etc. of all the cells." ,
			        _cellParameterCollectionName,
				std::string("EmcCellParametersSaved")) ;


    registerProcessorParameter( "Calibration" , 
				"Name of object to be used for the calibration" ,
				_calibrationObjectName ,
				std::string("CaliceEcalCalibration")) ;

    registerProcessorParameter( "CalibrationConstants" , 
				"Name of the conditions data collection which contains the calibration constants" ,
				_calibrationConstantColName,
				std::string("EmcCalibrationConstants")) ;

    registerProcessorParameter( "DummyCalibrationConst" , 
				"To decalibrate cells declared as dead",
				_dummyCalib ,
				static_cast<double>(1.0/50.0)) ;


    registerProcessorParameter( "ModuleLocationConnectionName" , 
				"Collection containing SiW Ecal module connection" ,
				_moduleConnection,
				std::string("EmcModuleConnection")) ;

    registerProcessorParameter( "ModuleLocationCollectionName" , 
				"Collection containing SiW Ecal module location" ,
				_moduleDescription,
				std::string("EmcModuleDescription")) ;


    registerProcessorParameter( "ModuleDescriptionCollectionName" , 
				"Collection containing SiW Ecal module description" ,
				_moduleLocation,
				std::string("EmcModuleLocation")) ;
        

    
  }
  
  //.. destructor
  TBEcalDigitisation::~TBEcalDigitisation() {
    
  }
  
  void TBEcalDigitisation::init() {
    // initialization from steering file
    //printParameters();
    
    _nEvt = 0;
    _nRun = 0;
    
    std::cout << "Initializing the mapping" << std::endl;
    _mapping.init();
    _mapping.rebuildConnectionTree();
    
    //TRandom randnumPed;
    //randnumPed.SetSeed(_randnum.GetSeed()+1);
    //std::cout << " Initializing Pedestal root random number. Has to be done only once !! Seed = " << randnumPed.GetSeed() << std::endl;
    
    //this is hardcoded for sanity checks and being sure we loop over all cells.
    //FIXME: if there is the corresponding parameters in the database: would be better.
    _maxK = 30;
    _maxS = 3;
    _maxM = 3;
    _maxI = 6;
    _maxJ = 6;
    
    
  }
  
  void TBEcalDigitisation::processRunHeader( LCRunHeader* run) {
    cout << "TBEcalDigitisation::processRun()  " << name()
	 << " in run " << run->getRunNumber()
	 << endl ;
    
    _runnum = run->getRunNumber();
    
    if (_inputSeed != -1) {
      _randnum.SetSeed(_inputSeed);
      std::cout << " Initializing root random number from input parameter. Has to be done only once !! Seed = " << _randnum.GetSeed() << std::endl;
    }
    _nRun++ ;
  }
  
  
  void TBEcalDigitisation::processEvent( LCEvent * evt) {//process event
    
    //if no input seed given, give the evt number seed.
    if (_inputSeed == -1) _randnum.SetSeed(evt->getEventNumber()+1);
    
    static bool firstEvent = true;
    
    if (_debug) {
      std::cout << " ------------ TBEcalDigitisation::Evt " << _nEvt << "------------" << std::endl;
      if (evt->getTimeStamp() != 0) std::cout << "            timestamp: " << evt->getTimeStamp() << std::endl;
    }
    
    if (_debug == 0 && _nEvt%1000 == 0) std::cout << " ------------ TBEcalDigitisation::Evt " << _nEvt << "------------" << std::endl;
    
    //LCCollection *newcol = new LCCollectionVec(LCIO::LCGENERICOBJECT);
    //evt->addCollection(newcol,"EmcCellParametersSaved");
    
    
    if (firstEvent){//first event
      //LCCollection *newcol = new LCCollectionVec(LCIO::LCGENERICOBJECT);
      std::cout << " Accessing collections in first event" << std::endl;
      
      //CRP 25/11/15 Protect the database requests against missing handler, a missing handler is a serious problem and
      //should stop the programme
      //CRP 25/11/15 Here col -> colcalib to avoid confusion with object named col further downstream
      LCCollection* colcalib(NULL);
      if(lccd::LCConditionsMgr::instance()->getHandler(_calibrationConstantColName)) {
	colcalib = (lccd::LCConditionsMgr::instance()->getHandler(_calibrationConstantColName))->currentCollection();
      } else {
        throw lccd::LCCDException(" CALICE::TBEcalDigitisation: no handler "
				  " for name: " + _calibrationConstantColName) ; 
      }
      
      //LCCollection* col1(NULL);
      if(lccd::LCConditionsMgr::instance()->getHandler(_moduleLocation)) {
	//col1 = (lccd::LCConditionsMgr::instance()->getHandler(_moduleLocation))->currentCollection();
	_mapping.moduleLocationChanged((lccd::LCConditionsMgr::instance()->getHandler(_moduleLocation))->currentCollection());
	std::cout << " Locations have been updated ! " << std::endl;
      } else {
	throw lccd::LCCDException(" CALICE::TBEcalDigitisation: no handler "
				  " for name: " + _moduleLocation) ; 
      }

      //LCCollection* col2(NULL);
      if(lccd::LCConditionsMgr::instance()->getHandler(_moduleConnection)) {
	//col2 = (lccd::LCConditionsMgr::instance()->getHandler(_moduleConnection))->currentCollection();
	_mapping.moduleConnectionChanged((lccd::LCConditionsMgr::instance()->getHandler(_moduleConnection))->currentCollection());
	std::cout << " Connections have been updated ! " << std::endl;
      } else {
        throw lccd::LCCDException(" CALICE::TBEcalDigitisation: no handler "
				  " for name: " + _moduleConnection) ; 
      }
      
      LCCollection* colmoddesc(NULL);
      if(lccd::LCConditionsMgr::instance()->getHandler(_moduleDescription)) {
	colmoddesc = (lccd::LCConditionsMgr::instance()->getHandler(_moduleDescription))->currentCollection();
	_mapping.moduleTypeChanged(colmoddesc);
	std::cout << " Types have been updated ! " << std::endl;
      } else {
        throw lccd::LCCDException(" CALICE::TBEcalDigitisation: no handler "
				  " for name: " + _moduleDescription) ; 
      }

      LCCollection* cellCol(NULL); 
      if(lccd::LCConditionsMgr::instance()->getHandler(_cellParameterCollectionName)) {
	cellCol = (lccd::LCConditionsMgr::instance()->getHandler(_cellParameterCollectionName))->currentCollection();
	std::cout << "Cell parameter collection contains " << cellCol->getNumberOfElements() << " elements." << std::endl;
        //CRP 25/11/15 Remark no further check for non null pointer needed since LCCD either returns a collection or
        //throws an exception, see e.g. DBCondHandler.cc in LCCD 
      } else {
        throw lccd::LCCDException(" CALICE::TBEcalDigitisation: no handler "
				  " for name: " + _cellParameterCollectionName) ; 
      }  
      
      
      _mapping.rebuildConnectionTree();
      _mapping.print(std::cout);
      std::cout << " Number of modules in the mapping : " << _mapping.getNModules() << std::endl;
      std::cout << " Is Cond data complete for mapping ?? " << _mapping.isModuleConditionsDataComplete() << std::endl;
      std::cout << " Creating the reverse look-up table. " << std::endl;
      _convert.createIndexReverseLookup(_mapping);
      
      std::cout << " Accessing the calibration constants. " << std::endl;
      CalibrationFactory::getInstance()->listKits();
      _calib=CalibrationFactory::getInstance()->createCalibrationObject(_calibrationObjectName, _moduleDescription, _calibrationConstantColName);
      
      if (!_calib) {
	std::stringstream message;
	message << "<Digitizer::process> Failed to create calibration object CaliceEcalCalibration";
	throw std::runtime_error(message.str());
      }
      
      
      CALICE::CaliceEcalCalibration *testcalib = dynamic_cast<CALICE::CaliceEcalCalibration*>(_calib);
      if (testcalib){
	dynamic_cast<CALICE::CaliceEcalCalibration*>(_calib)->moduleTypeChanged(colmoddesc);
	std::cout << " Calibration constants have been notified of the current mapping configuration." << std::endl;
	dynamic_cast<CALICE::CaliceEcalCalibration*>(_calib)->calibrationConstantChanged(colcalib);
	std::cout << " Calibration constants have been properly updated." << std::endl;
      }
      else std::cout << " Dynamic_cast is null !!" << std::endl;
      
      CALICE::NoiseParameterArrayofArray_t & cellParameter = getCellParameters();
      cellParameter.clear();
      if (cellParameter.size()!=_maxK) cellParameter.resize(_maxK);
      for (unsigned int l = 0; l < _maxK; l++){
	cellParameter[l].resize(_maxS*_maxM);
      }
      //std::cout << " Filling cellParameter array : " << std::endl;
      //std::cout << " --> Ncells for the first module = " << index[module_i+1]-index[module_i] << std::endl;
      
      //if(_cellCol) {
	assert (static_cast<unsigned int>(cellCol->getNumberOfElements()) == _maxK*_maxS*_maxM*_maxI*_maxJ);
	
	for (UInt_t element_i=0; element_i < static_cast<UInt_t>(cellCol->getNumberOfElements()); element_i++) {
	  //Need to clone the object prior to refilling it into the new collection which will added to the event
	  //newcol->addElement(  cloneLCGenericObject(dynamic_cast<LCGenericObject*>(_cellCol->getElementAt(element_i))) );
	  const LCPayload<NoiseParameter> p(dynamic_cast<LCGenericObject*>(cellCol->getElementAt(element_i)));
	  NoiseParameter noisePar = p.payload();
	  CALICE::CellIndex mydec(noisePar.getGeomCellIndex());
	  unsigned int layer = mydec.getLayerIndex()-1;
	  unsigned int waf = (mydec.getWaferColumn()-1)*_maxM + mydec.getWaferRow() - 1;
	  assert (waf >= 0);
	  assert (waf < _maxS*_maxM);
	  assert (layer >= 0);
	  assert (layer < _maxK);
	  //std::cout << "K=" << layer << " and waf=" << waf << std::endl;
	  cellParameter[layer][waf].push_back(noisePar);
	}
	//evt->addCollection(newcol,"EmcCellParametersSaved");
	assert (cellParameter.size() == _maxK);
	
	TFile *file(0);
	std::ostringstream rootname;
	rootname << "histos/ControlDigi_Run" << _runnum << ".root";
	file = TFile::Open(rootname.str().c_str(),"RECREATE");
	
	TH1F *p_noise[_maxK];
	
	
	for (unsigned int l = 0; l < _maxK; l++){
	  assert (cellParameter[l].size() == _maxS*_maxM);
	  ostringstream title;
	  ostringstream histname;
	  histname << "p_noise_"<<l;
	  title << "Noise for layer " << l << ";(6*J+I)+36*(3*S+M);noise";
	  p_noise[l] = new TH1F(histname.str().c_str(),title.str().c_str(),324,0,324);
	  for (unsigned int w = 0; w < cellParameter[l].size(); w++){
	    assert (cellParameter[l][w].size() == _maxI*_maxJ);
	    for (unsigned int cell = 0; cell < cellParameter[l][w].size(); cell++){
	      CALICE::NoiseParameter &cellPars = cellParameter[l][w][cell];
	      _noise[l][w][cell] = cellPars.getNoise();
	      _pedestal[l][w][cell] = cellPars.getPedestal();
	      //AM:coherent noise ???
	    }
	  }
	  for (unsigned int cell = 0; cell < 324; cell++){
	    p_noise[l]->SetBinContent(cell,_noise[l][static_cast<int>(cell/36.)][cell%36]);
	  }
	  
	}
	std::cout << " cellParameter have been filled. " << std::endl;
	//save the noise per layer to check what has been put in the database...
	
	if(file) {
	  file->Write();
	  file->Close();
	  delete file;
	}
	//}
      firstEvent = false;
    }//first event
    

    //will fill arrays for noise and pedestal with one value per cell randomly according to values in cellParameter.
    
    
    LCCollection *col=0;
    LCCollection *relVec=0;
    LCCollection *rawVec = new LCCollectionVec( LCIO::RAWCALORIMETERHIT );
    // assume that the Ecal is not hit
    unsigned int nExistingHits = 0;
    for (unsigned int l = 0; l < _maxK; l++){
      for (unsigned int w = 0; w < _maxS*_maxM; w++){
	for (unsigned int ch = 0; ch < _maxI*_maxJ; ch++){
	  _there[l][w][ch] = false;
	}
      }
    }
    //fill an array with the noise for this event
    FillRandomNoise();

    //Check whether there is a collection of sim hit for this event
    try {//try
      col = evt->getCollection(_inputColName);

      //LCCollection *rawVec = new LCCollectionVec( LCIO::RAWCALORIMETERHIT );
      LCFlagImpl rawFlag(0) ;
      rawFlag.setBit( LCIO::RCHBIT_TIME );
      rawFlag.setBit( LCIO::CHBIT_ID1);
      rawFlag.setBit( LCIO::CHBIT_LONG);
      rawVec->setFlag( rawFlag.getFlag() );
      
      // and another collection to save the LCRelations between raw and sim hits
      relVec = new LCCollectionVec( LCIO::LCRELATION );
      relVec->parameters().setValue( "FromType", LCIO::RAWCALORIMETERHIT );
      relVec->parameters().setValue( "ToType", LCIO::SIMCALORIMETERHIT );
      LCFlagImpl relFlag(0);
      relFlag.setBit( LCIO::LCREL_WEIGHTED );
      relVec->setFlag( relFlag.getFlag()  );
      

      //loop on existing hits to add noise on top of the signal
      for(int i=0; i < col->getNumberOfElements(); i++) {//loop on hits
	//loop on hits
      if (_debug > 2) std::cout << "Processing hit #" << i << std::endl;
      SimCalorimeterHit *simHit = dynamic_cast<SimCalorimeterHit*>(col->getElementAt(i));
      
      double hitenergy = static_cast<double>(simHit->getEnergy());
      int cellID0 = simHit->getCellID0();
      int cellID1 = simHit->getCellID1();
      
      DecalibrateHits(hitenergy,cellID0,cellID1);

      CALICE::CellIndex dec(cellID0,cellID1);

      unsigned int lay = dec.getLayerIndex()-1;
      unsigned int waf = (dec.getWaferColumn()-1)*_maxM + dec.getWaferRow() - 1;
      unsigned int ch = (dec.getPadColumn()-1)*_maxI + dec.getPadRow() - 1;
      assert(lay >= 0);
      assert(waf >= 0);
      assert(ch >= 0);
      assert(lay < _maxK);
      assert(waf < _maxM*_maxS);
      assert(ch < _maxI*_maxJ);
      //A quick check for Mokka and reco cell positions  
      

     
      _there[lay][waf][ch] = true;
      //Protect against missing Noise Collection
      //CRP 25/11/15 not needed, see Remark above
      hitenergy += _randNoise[lay][waf][ch];
      //else hitenergy +=0;
      nExistingHits++;
      
      // Create collection for writing raw hits into LCIO file
      int adc = 0;
      if (hitenergy >= 0.0) adc = static_cast<int>(hitenergy+0.5);
      else adc = static_cast<int>(hitenergy-0.5);
      
      //create a raw hit for each temp hit
      RawCalorimeterHitImpl* rawhit = new RawCalorimeterHitImpl();
      rawhit->setCellID0(cellID0);
      rawhit->setCellID1(cellID1);
      if (_debug > 1) std::cout << " rawhit: " << i << ", cellID0 = " << rawhit->getCellID0() << ", cellID1 = " << rawhit->getCellID1() << std::endl;
      rawhit->setAmplitude( adc );
      rawhit->setTimeStamp(0);
      rawVec->addElement( rawhit );

      // create LCRelation object
      LCRelation* relobj = new LCRelationImpl(rawhit,simHit,1.);
      relVec->addElement(relobj);

    }//loop on hits


    //assert (relVec->getNumberOfElements() == rawVec->getNumberOfElements());

    if (_debug > 0) std::cout << " End of event loop, collections have been added to the event." << std::endl;
  }//try
  catch(EVENT::DataNotAvailableException& e) {
    cout << e.what() << endl;
    std::cout << " TBEcalDigitisation::ProcessEvent, evt #" << evt->getEventNumber() << ", collection " << _inputColName << " not in the event " << std::endl;
    //exit(0);
  }
  
    //create noise only hits and add them to the collection
    //CreateNoiseHits(nExistingHits,*rawVec);
    if (_debug > 0) std::cout << "Creating noise only hits for this event, for all cells having no signal hits. Number of existing hits = " << nExistingHits << std::endl;


    unsigned int nNoiseOnlyHits = 0;
    for (unsigned int K =1; K < _maxK+1; K++)
      {//loop on K
	for (unsigned int M=1; M < _maxM+1; M++)
	  {//loop on M
	    for (unsigned int S=1; S < _maxS+1; S++)
	      {//loop on S
		for (unsigned int I=1; I < _maxI+1; I++)
		  {//loop on I
		    for (unsigned int J=1; J < _maxJ+1; J++)
		      {//loop on J
			unsigned int lay = K-1;
			unsigned int waf = (S-1)*_maxM + M - 1;
			unsigned int ch = (J-1)*_maxI + I - 1;		    

			CALICE::CellIndex mydec(M,S,I,J,K);
			int cellID0 = mydec.getCellIndex();
			int cellID1 = 0;
			setNoiseCellIDs(cellID0,cellID1,lay,waf,ch);


			if (!_there[lay][waf][ch])
			  {
			    RawCalorimeterHitImpl *noisehit = new RawCalorimeterHitImpl();
			    noisehit->setCellID0(cellID0);
                            noisehit->setCellID1(cellID1);
			    int ampl = 0;
			    //Protect execution against non existing cellCol collection
                            double hitenergy;
			    //if(_cellCol) hitenergy = _randNoise[lay][waf][ch];
                            //CRP 26/11/15 no need to check for existing _cellCol anymore, if it wouldn'y exist we wouldn't be here
                            hitenergy = _randNoise[lay][waf][ch];
                            //else hitenergy = 0;
			    if (hitenergy >= 0.0) ampl = static_cast<int>(hitenergy+0.5);
			    else ampl = static_cast<int>(hitenergy-0.5);
			    noisehit->setAmplitude(ampl);
			    noisehit->setTimeStamp(0);
			    rawVec->addElement(noisehit);
			    //LCRelation* relnoise = new LCRelationImpl(noisehit,0,1.);
			    //relVec->addElement(relnoise);
			    nNoiseOnlyHits++;
			  }
		      }//loop on J
		  }//loop on I
	      }//loop on S
	  }//loop on M
      }//loop on K

    
    if (_debug > 0) std::cout << "---> Number of noise only hits created : " << nNoiseOnlyHits << std::endl;
    assert ((nNoiseOnlyHits + nExistingHits) == _maxK*_maxS*_maxI*_maxM*_maxJ);

    evt->addCollection(rawVec,_outputColName);
    if(relVec) evt->addCollection(relVec,_relColName);

    ++_nEvt;
}//process event

void TBEcalDigitisation::end() {
  cout << "TBEcalDigitisation::end()  " << name()
       << " processed " << _nEvt << " events in " << _nRun << " runs "
       << endl ;
}

void TBEcalDigitisation::DecalibrateHits(double& hitenergy, int cellID0, int& cellID1) {

  CALICE::CellIndex dec(cellID0,cellID1);
  unsigned int K = dec.getLayerIndex();
  unsigned int M = dec.getWaferRow();
  unsigned int S = dec.getWaferColumn();
  unsigned int I = dec.getPadRow();
  unsigned int J = dec.getPadColumn();

  if ( !(K >= 1 && K <= _maxK) ||
       !(M >= 1 && M <= _maxM) ||
       !(S >= 1 && S <= _maxS)||
       !(I >= 1 && I <= _maxI)||
       !(J >= 1 && J <= _maxJ))
    {
      std::cout << " DecalibrateHits::WRONG decoding of indices ! " << std::endl <<
	" Layer = " << K << std::endl <<
	" Wafer Row (M) = " << M << std::endl <<
	" Wafer Column (S) = " << S << std::endl <<
	" Pad Row (I) = " << I << std::endl <<
	" Pad Column (J) = " << J <<  std::endl <<
	" ...........Exiting........." << std::endl;
      exit(0);
    }

  unsigned int lay =  K-1;
  unsigned int waf = (S-1)*_maxM + M - 1;
  unsigned int ch = (J-1)*_maxI + I - 1;


  bool isBad = false;
  //CRP 25/11/15 Introduce a bool for a bad mapping in order to avoid to case everything on is dead
  //One thing is a dysfunctionnning of a cell that is still a well defined object in the other case the cell  
  //didn't physically exist during the data taking
  bool isMappingProblem(false);

  //CRP Patch this part to protect from missing noise collections
  //Can maybe be done more elegant
  //CRP 26/11/15 Not needed anymore, see above, since otherwise we wouldn't be here
  bool isDead = false;
  std::pair<unsigned, unsigned> ModuleAndCell;
  //if(_cellCol) {
    CALICE::NoiseParameterArrayofArray_t & cellParameter = getCellParameters();
    CALICE::NoiseParameter &cellPar =  cellParameter[lay][waf][ch];
    if (cellPar.isDead()) isDead=true;
    //} //else {
    try{
      ModuleAndCell = _convert.getModuleAndCellIndex(_mapping, dec);
    } catch(std::range_error) {
      //CRP modified 25/11/15 isDead=true;
      isMappingProblem=true;
    } 
//  }
// kkk fill index also if noise is available


  
  //bool isBad = false;
  //if (_debug > 1) std::cout << " Layer " << lay << " (K=" << K << "), wafer " << waf << "(M=" << M << ",S=" << S << "), cell " << ch << " (I=" << I << ",J=" << J << "), isDead=" << cellPar.isDead() << std::endl;
if (_debug > 1) std::cout << " Layer " << lay << " (K=" << K << "), wafer " << waf << "(M=" << M << ",S=" << S << "), cell " << ch << " (I=" << I << ",J=" << J << "), isDead=" << isDead << std::endl;

//CRP Modified 25/11/15 if (!isDead){//cell not dead
 if (!isMappingProblem){//Mapping ok
    //std::pair<unsigned, unsigned> ModuleAndCell = _convert.getModuleAndCellIndex(_mapping, dec);
    UInt_t cell_index   = ModuleAndCell.second;
    UInt_t module_index = ModuleAndCell.first;
    UInt_t module_type = 0;
    UInt_t module_id = 0;
    //Flag a dead cell as bad
    if(isDead) isBad=true;
    //CRP 27/11/15 FIXME: I think that the following check is not needed  
    //Leave it for the time being ... . check for futur versions
    if (_mapping.isValid(module_index)){//valid mapping
      module_type = _mapping.getModuleType(module_index);
      module_id   = _mapping.getModuleID(module_index);

      if (_debug>1) std::cout << "--->cell_index=" << cell_index << ", module_index=" << module_index << ", module_type=" << module_type << ", module_id=" << module_id << std::endl;

      //bool valid = _calib->checkForCalibrationConstantsOfModule(module_id,module_type,216);
      double calibconst = _calib->getCalibratedValue(module_id,module_type,cell_index,1);
      if (_debug>1) std::cout << "---> Constant = " << calibconst << std::endl;

      if (calibconst != 0) hitenergy = hitenergy/(static_cast<double>(_mipEnergy)*calibconst);
      else {
	if (_debug>1) std::cout << "---> Constant is null !! Tagging the hit bad." << std::endl;
	hitenergy = hitenergy/(static_cast<double>(_mipEnergy)*_dummyCalib);
	isBad = true;
      }
	
    }//valid mapping
    else {
      if (_debug) std::cout << "Module_index is not valid ! Dead cell or not connected ??? Tagging the hit bad." << std::endl;
      hitenergy = hitenergy/(static_cast<double>(_mipEnergy)*_dummyCalib);
      isBad = true;
    }

    CALICE::CellIndex newdec(cellID0,module_index,module_type,module_id,cell_index,static_cast<int>(isBad));
    cellID1 = newdec.getSecondIndex();

   }//cell not dead -> 25/11/15 no mapping problem
  else {
    //putting a dummy calibration constant.
    isBad = true;
    hitenergy = hitenergy/(static_cast<double>(_mipEnergy)*_dummyCalib);
    dec.setBad(1);
    cellID1 = dec.getSecondIndex();
  }
}


CALICE::NoiseParameterArrayofArray_t & TBEcalDigitisation::getCellParameters() {
  static CALICE::NoiseParameterArrayofArray_t params;
  return params;
}

void TBEcalDigitisation::FillRandomNoise() {

  if (_debug > 0) std::cout << "Filling the array of noise for this event, using values saved in cellParameters." << std::endl;

  for (unsigned int K =0; K<_maxK; K++){//loop on K
    for (unsigned int w = 0; w < _maxS*_maxM; w++){
      for (unsigned int cell = 0; cell < _maxI*_maxJ; cell++){
	_pedestal[K][w][cell] = _randnum.Rndm() - 0.5;
	_randNoise[K][w][cell] = _randnum.Gaus(_pedestal[K][w][cell],_noise[K][w][cell]);
      }
    }
  }//loop on K
}

void TBEcalDigitisation::setNoiseCellIDs(int & cellID0, int & cellID1, int lay, int waf, int ch) {
  CALICE::NoiseParameterArrayofArray_t & cellParameter = getCellParameters();
  CALICE::NoiseParameter &cellPar =  cellParameter[lay][waf][ch];

  CALICE::CellIndex mydec(cellID0|0x80000000);
  UInt_t cell_index   = 0;
  UInt_t module_index = 0;
  UInt_t module_type = 0;
  UInt_t module_id = 0;
  UInt_t isBad = 0;

  bool isDead = false;
  std::pair<unsigned, unsigned> ModuleAndCell;
  //CRP 26/11/15 Not needed anymore, see above, since otherwise we wouldn't be here
  //if(_cellCol) {
    //CALICE::NoiseParameterArrayofArray_t & cellParameter = getCellParameters();
    //CALICE::NoiseParameter &cellPar =  cellParameter[lay][waf][ch];
    if (cellPar.isDead()) isDead=true;
    //} //else {
  //
  try{
    ModuleAndCell = _convert.getModuleAndCellIndex(_mapping, mydec);
  } catch(std::range_error) {
    isDead=true;
  } 
//  }
// kkk fill index also if noise is available


  //if (!cellPar.isDead()){//cell not dead
  //if (!isDead){//cell not dead
    //std::pair<unsigned, unsigned> ModuleAndCell = _convert.getModuleAndCellIndex(_mapping, mydec);
    cell_index   = ModuleAndCell.second;
    module_index = ModuleAndCell.first;
    if (_mapping.isValid(module_index)){//valid mapping
      module_type = _mapping.getModuleType(module_index);
      module_id   = _mapping.getModuleID(module_index);
    }//valid mapping
    else {
      std::cout << " Not valid mapping, don't know what to do ?? Exiting..." << std::endl;
      exit(0);
    }
    
    //}
    //else isBad = 1;
   if (isDead) isBad = 1;
    

  mydec.setModuleIndex(module_index);
  mydec.setModuleType(module_type);
  mydec.setModuleID(module_id);
  mydec.setCellID(cell_index);
  mydec.setBad(isBad);

  cellID0 = mydec.getCellIndex();
  cellID1 = mydec.getSecondIndex();
  //if (_debug > 2) std::cout << " Check : cellID0 = " << std::hex << cellID0 << " cellID1 = " << cellID1 << " decoded as K=" << std::dec << mydec.getLayerIndex() << ", M=" << mydec.getWaferRow() << ", S=" << mydec.getWaferColumn() << ", I=" << mydec.getPadRow() << ", J=" << mydec.getPadColumn() << ", type=" << mydec.getModuleType() << ", ID=" << mydec.getModuleID() << ", index=" << mydec.getModuleIndex() << ", cell=" << mydec.getCellID() << ", isBad=" << mydec.isBad() << std::endl;
  //if (isBad) { std::cout << " Check : cellID0 = " << std::hex << cellID0 << " cellID1 = " << cellID1 << " decoded as K=" << std::dec << mydec.getLayerIndex() << ", M=" << mydec.getWaferRow() << ", S=" << mydec.getWaferColumn() << ", I=" << mydec.getPadRow() << ", J=" << mydec.getPadColumn() << ", type=" << mydec.getModuleType() << ", ID=" << mydec.getModuleID() << ", index=" << mydec.getModuleIndex() << ", cell=" << mydec.getCellID() << ", isBad=" << mydec.isBad() << std::endl;
  //std::cout << "x-position from mapping: " << (_mapping.getPosition(mydec.getModuleIndex(),mydec.getCellID()).data())[0] << std::endl;  
  //}
}
}// end namespace CALICE
