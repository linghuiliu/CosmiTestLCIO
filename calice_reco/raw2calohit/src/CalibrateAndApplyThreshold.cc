#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "TH1F.h"

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/RawCalorimeterHit.h>
#include <EVENT/SimCalorimeterHit.h>
#include <EVENT/LCObject.h>
#include <EVENT/LCParameters.h>
#include <EVENT/LCFloatVec.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCRelation.h>
#include <IMPL/LCFlagImpl.h>
#include "UTIL/LCRelationNavigator.h"
#include "lccd/LCConditionsMgr.hh"
#include <time.h>

#include <ErrorBits.hh>
#include <CalibrationFactory.hh>
#include <NoOpCalibration.hh>
#include <Calibration.hh>

#include <marlin/ConditionsProcessor.h>
#include <marlin/Global.h>
#include <CalibrateAndApplyThreshold.hh>
#include <CellIndex.hh>
#include "NoiseParameter.hh"
#include "LCPayload.hh"
#include "SimpleHitSearch.hh"



using namespace lcio;
using namespace marlin;

namespace CALICE {//namespace CALICE
   CalibrateAndApplyThreshold b;


  CalibrateAndApplyThreshold::CalibrateAndApplyThreshold()
    : VRawADCValueProcessor("CalibrateAndApplyThreshold"),
      _calibration(new NoOpCalibration),
      _cellCol(0),
      _nEventsWithoutRAWHITS(0),
      _nEventsWithoutSIMHITS(0),
      _nEvt(0),
      _hfile(0),
      _detectorTransformationChange(this,&CalibrateAndApplyThreshold::detectorTransformationChanged),
      _referenceTransformationChange(this,&CalibrateAndApplyThreshold::referenceTransformationChanged)
   {

    registerProcessorParameter( "SaveHistograms" ,
			       "Create a rootfile with control histograms" ,
			       _saveHistograms ,
				static_cast<int>(0));

    _rawhitColName=COL_RAWHITS;
    registerProcessorParameter( "RawHitCollectionName" ,
			       "The name of the Rawcalorimeter hit collection (input) to be used" ,
			       _rawhitColName ,
			       _rawhitColName);

    registerProcessorParameter( "InputRelCollections",
				"Names of relation collections with simhits",
				_RawToSimColName,
				std::string("EcalProtoRaw2sim") );

    _hitColName=COL_HITS;
    registerProcessorParameter( "HitCollectionName" ,
			       "The name of the calorimeter hit collection (output) to be used" ,
			       _hitColName ,
			       _hitColName);


    _cellParameterCollectionName="EmcCellParameters";
    registerProcessorParameter( "CellParameterCollectionName" ,
			       "The name of the collection which contains the pedestals, the noise, etc. of all the cells." ,
			       _cellParameterCollectionName ,
			       _cellParameterCollectionName);

    // ---- collection name of calibration constants and name of calibration object
    registerProcessorParameter( "Calibration" ,
    				"Name of object to be used for the calibration"  ,
				_calibrationObjectName ,
    				std::string("NoOpCalibration") ) ;

    registerOptionalParameter( "CalibrationConstants" ,
			       "Name of the conditions data collection which contains the calibration constants"  ,
			       _calibrationConstantColName ,
			       std::string("") );

    // --- detector position
    registerOptionalParameter( "DetectorTransformationCollectionName" ,
    				"Name of the conditions data collection which describes the detector position and rotation (folder e.g. /Calice/ECAL/Transformation)"  ,
				_colNameDetectorTransformation ,
    				std::string("DetectorTransformation") ) ;

    registerOptionalParameter( "ReferenceTransformationCollectionName" ,
    				"Name of the conditions data collection which describes the position and rotation of the reference coordinate system (folder e.g. /Calice/ECAL/Transformation)"  ,
				_colNameReferenceTransformation ,
    				std::string("ReferenceTransformation") ) ;
    _colNameReferenceTransformation="";
    _colNameDetectorTransformation="";

    _signalThreshold = .5;
    registerProcessorParameter( "SignalThreshold" ,
			       "Cells with a signal (after calibration) larger than this threshold are considered to have a hit. " ,
			       _signalThreshold ,
			       _signalThreshold ) ;

    _noiseCutVec.clear();
    _noiseCutVec.push_back( 5.);
    registerProcessorParameter( "NoiseCut" ,
			       "Cells with a signal-to-noise ratio larger than this value are considered to have a hit. The first element is used for hit search the second for hit rejection during noise calculation." ,
			       _noiseCutVec ,
			       _noiseCutVec) ;


    registerProcessorParameter( "LinkRecToSim", "Create link reconstricted -> simulated hit", _linkRecToSim, true);
    registerProcessorParameter( "LinkRecToSimCollectionName", "Name of the link reconstructed -> simulated hit collection", _RecToSimColName, std::string("EmcRecToSim"));

    //CRP 26/11/15 
    _fallbackShift = 10000.;
    registerProcessorParameter( "FallBackShift" ,
			       " Define a shift for cells which cannot be properly handled since they were present in the simulation but not in the data (e.g. DESY 2005, CERN 2006/2007" ,
			       _fallbackShift ,
			       _fallbackShift ) ;


   }

  CalibrateAndApplyThreshold::~CalibrateAndApplyThreshold() {
    delete _calibration;
  }

  void CalibrateAndApplyThreshold::init()
  {//init
    std::stringstream message;
    bool error=false;
    try {
      VRawADCValueProcessor::init();
    }
    catch (ErrorMissingConditionsDataHandler &conddata_error) {
      // --- catch conditions data handler registration errors
      //   ... and build a combined error message.
      std::string a(conddata_error.what());
      error=true;
      if (a.size()>0) {
	a.erase(a.size()-1);
	message << a;
      }
    }

    // --- register conditions data handler

    if (!error) {
      message << "CalibrateAndApplyThreshold::init> no conditions data handler for the collections:";
    }

    if (!_colNameDetectorTransformation.empty()) {
      if (!marlin::ConditionsProcessor::registerChangeListener( &_detectorTransformationChange ,_colNameDetectorTransformation) ) {
	message << " " << _colNameDetectorTransformation;
	error=true;
      }
    }

    if (!_colNameReferenceTransformation.empty()) {
      if (!marlin::ConditionsProcessor::registerChangeListener( &_referenceTransformationChange ,_colNameReferenceTransformation) ) {
	message << " " << _colNameReferenceTransformation;
	error=true;
      }
    }

    if (error) {
      // also throw an exception if the registration of conditions data handlers failed in VRawADCValueProcessor::init
      message <<  ".";
      throw ErrorMissingConditionsDataHandler(message.str());
    }

    //_cellParameter.clear();

    printParameters();
    //_nEventsWithoutRAWHITS = 0;
    //_nEvt = 0;

    // --- Calibration object
    delete _calibration;
    CalibrationFactory::getInstance()->listKits();
    _calibration=CalibrationFactory::getInstance()->createCalibrationObject(_calibrationObjectName, _colNameModuleDescription, _calibrationConstantColName);

    if (!_calibration) {
      std::stringstream message;
      message << "SimpleHitSearch::init> Failed to create calibration object \"" << _calibrationObjectName << "\".";
      throw std::runtime_error(message.str());
    }

    // --- initialise the analysis
    if (_noiseCutVec.size()<1) {
      _noiseCut=5;
    }
    else {
      _noiseCut=_noiseCutVec[0];
    }

    std::cout << "NoiseCut = " << _noiseCut << std::endl;


  }//init


  void CalibrateAndApplyThreshold::processRunHeader( LCRunHeader* run){

    std::cout << "CalibrateAndApplyThreshold::processRu()  " << name()
	 << " in run " << run->getRunNumber()
	      << std::endl ;

    _runInfo.get(run);
    _runInfo.print();

    //fill an LCParameter with the list of cellIDs of the dead cells
    //dead = not connected (in connected PCBs) or calibration constant is not valid
    //obtained from the Calice database
    EVENT::IntVec Vec;
    if (_mapping.isModuleConditionsDataComplete()) {
      unsigned int n_modules = _mapping.getNModules();
      for (UInt_t module_i=0; module_i<n_modules; module_i++) {
	UInt_t cells_per_module=_mapping.getNCellsPerModule(module_i);
	for (UInt_t cell_i=0; cell_i<cells_per_module; cell_i++) {
	  if (_mapping.isCellDead(module_i,cell_i) || !_calibration->isValid(_mapping.getModuleID(module_i),_mapping.getModuleType(module_i),cell_i)){
	    Vec.push_back(_mapping.getGeometricalCellIndex(module_i,cell_i));
	  }
	}
      }
    }

    (run->parameters()).setValues("DeadCells",Vec);

  }


  void CalibrateAndApplyThreshold::processEvent( LCEvent * evtP )
  {//processEvt
    //static bool firstEvent = true;
    try {//try
      //FIXME: check there is rawhits in the collection, else, do nothing, a mssing raw collection might indicate a  serious
      //problem, error catching to be updated.
      LCCollection* col_rawhits = evtP->getCollection( _rawhitColName ) ;
      unsigned int n_modules = _mapping.getNModules();
      //then, access the cell parameters. For data, it will be the ones saved in SimpleHitSearch or in the cellParam collection, but for MC it will be created for the first event only from the database collection.
      NoiseParameterArray_t &noiseArray = SimpleHitSearch::getCellParameters();

      if (!_runInfo.isMC() && noiseArray.size() == 0) {//data when SimpleHitSearch is not run before
	_cellCol = evtP->getCollection( _cellParameterCollectionName );
	noiseArray.clear();
	if (noiseArray.size()!=n_modules) noiseArray.resize(n_modules);
	unsigned int nHits = static_cast<unsigned int>(_cellCol->getNumberOfElements());
	unsigned int element = 0;
	for (UInt_t mod = 0; mod < n_modules; mod++){//loop on modules
	  for (UInt_t cell = 0; cell < _mapping.getNCellsPerModule(mod); cell++){

	    if (element >= nHits) {
	      std::stringstream message;
	      message << "CalibrateAndApplyThreshold::ProcessEvent> ERROR: found more elements than saved in the cell parameter collection ! n_modules = " << n_modules << ", and number of elements in collection " << _cellParameterCollectionName << " is " << nHits << std::endl;
	      throw runtime_error(message.str());
	    }
	    const LCPayload<NoiseParameter> p(dynamic_cast<LCGenericObject*>(_cellCol->getElementAt(element)));
	    NoiseParameter noisePar = p.payload();
	    noiseArray[mod].push_back(noisePar);
	    element++;
	  }
	}

      }//data
      else if (_runInfo.isMC()) {//isMC
	//need to copy the parameters from the collection saved with digi step. takeCollection allows the same parameters to be read for each events, as the collection will exist only in the first event.
	//if (firstEvent) _cellCol = evtP->takeCollection( _cellParameterCollectionName );

     	noiseArray.clear();
	if (noiseArray.size()!=30) noiseArray.resize(30);

        //CRP 25/11/15 Take out the try block as a missing handler in the following is not correctly caught by lccd 
        //try {
          //Protect against missing handler (happens if you don't use existing tools!!!!!)
          if(lccd::LCConditionsMgr::instance()->getHandler(_cellParameterCollectionName)){
	    _cellCol = (lccd::LCConditionsMgr::instance()->getHandler(_cellParameterCollectionName))->currentCollection();
           //CRP 25/11/15 Remark no further check for non null pointer needed since LCCD either returns a collection or
           //throws and exception, see e.g. DBCondHandler.cc in LCCD 
           //Hardcoded and ugly!!!!!!
	    assert (_cellCol->getNumberOfElements() == 30*324);
	    for (UInt_t element_i=0; element_i < static_cast<UInt_t>(_cellCol->getNumberOfElements()); element_i++) {
	      const LCPayload<NoiseParameter> p(dynamic_cast<LCGenericObject*>(_cellCol->getElementAt(element_i)));
	      NoiseParameter noisePar = p.payload();
	      CellIndex dec(noisePar.getGeomCellIndex());
	      assert (dec.getLayerIndex() > 0 && dec.getLayerIndex() < 31);
	      noiseArray[dec.getLayerIndex()-1].push_back(noisePar);
	    }
            //CRP 25/11/15 Not very nice to throw an exception that normally lccd should throw (in LCConditionsMgr::getHandler) 
	    //but it's the cleanest way here 
            //since after all a request for a conditions data collection has failed
	  } else {throw lccd::LCCDException(" CALICE::CalibrateAndApplyThreshold: no handler "
                            " for name: " + _cellParameterCollectionName) ; }
          //Leave the catch block in for future correction of lccd
	  //} catch ( DataNotAvailableException &err ){
	  //std::cout << "No cell parameter collection for event" << std::endl;
	  //}

      }//isMC
      else if (noiseArray.size() < n_modules ) {
	std::stringstream message;
	message << " ERROR : wrong access to the cell parameters in data ! Size of the array should be at least " << _mapping.getNModules() << " and is : " << noiseArray.size() << ". Exiting....";
	throw std::runtime_error(message.str());
      }

      //next step, loop over rawHits, calibrate and copy the ones above threshold in the output collection.

      //the relation collection is needed for MC events, to copy the position from the simhits for not connected layers where the mapping doesn't exist.
      LCCollection* col_RawToSim = 0;
      if (_runInfo.isMC()) {
        try {col_RawToSim = evtP->getCollection(_RawToSimColName); }
        catch ( DataNotAvailableException &err ){
	  streamlog_out(DEBUG) << "CalibrateAndApplyThreshold>>processEvent: No relation rawhits <-> Simhits. Missing SimHits?" << endl;
          _nEventsWithoutSIMHITS++;
        }
      }

      LCCollection* col_hit=new LCCollectionVec( LCIO::CALORIMETERHIT );
      EVENT::LCParameters & theParam = col_hit->parameters();
      //Set the cell decoder which might be useful in event displays
      //suggested by Allister
      //hardcoded, no good, should be created somehow automatically
      //theParam.setValue(LCIO::CellIDEncoding,"M:3,S-1:3,I:9,J:9,K-1:6");
      theParam.setValue(LCIO::CellIDEncoding,_mapping.getCellIDEncoding());

#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
      LCFloatVec signal_over_noise_ratio;
      signal_over_noise_ratio.clear();
#endif
      // write 3d coordinates
      LCFlagImpl hitFlag(col_hit->getFlag()) ;
      hitFlag.setBit( LCIO::RCHBIT_LONG) ;
      hitFlag.setBit( LCIO::RCHBIT_ID1) ;
      col_hit->setFlag( hitFlag.getFlag()  ) ;

      Int_t minAdcSignalThreshold = _calibration->getMiniumADCForMipThreshold(_signalThreshold);

      if (col_rawhits && col_rawhits->getNumberOfElements()>0) {//if input col

	LCRelationNavigator nav_RecToSim(LCIO::CALORIMETERHIT, LCIO::SIMCALORIMETERHIT);

	for (int ele = 0; ele < col_rawhits->getNumberOfElements(); ele++)
	  {//loop on raw hits
	    RawCalorimeterHit *hit_raw = dynamic_cast<RawCalorimeterHit*>(col_rawhits->getElementAt(ele));
	    CellIndex mydec(hit_raw->getCellID0(),hit_raw->getCellID1());
	    UInt_t K = mydec.getLayerIndex();

#ifdef BOUNDARY_CHECK
	      if ( !(mydec.getLayerIndex() >= 1 && mydec.getLayerIndex() <= 30) ||
		   !(mydec.getWaferRow() >= 1 && mydec.getWaferRow() <= 3) ||
		   !(mydec.getWaferColumn() >= 1 && mydec.getWaferColumn() <= 3)||
		   !(mydec.getPadRow() >= 1 && mydec.getPadRow() <= 6)||
		   !(mydec.getPadColumn() >= 1 && mydec.getPadColumn() <= 6))
		{
		  std::cout << " CalibrateAndApplyThreshold::WRONG decoding of indices ! " << std::endl <<
		    " CellID0 = " << hit_raw->getCellID0() << std::endl <<
		    " CellID1 = " << hit_raw->getCellID1() << std::endl <<
		    " Layer = " << mydec.getLayerIndex() << std::endl <<
		    " Wafer Row (M) = " << mydec.getWaferRow() << std::endl <<
		    " Wafer Column (S) = " << mydec.getWaferColumn() << std::endl <<
		    " Pad Row (I) = " << mydec.getPadRow() << std::endl <<
		    " Pad Column (J) = " << mydec.getPadColumn() <<  std::endl <<
		    " ...........Exiting........." << std::endl;
		  exit(0);
		}
#endif

	    assert (K > 0 && K < 31);
	    UInt_t cell_i = mydec.getCellID();
            //FIXME: What happens if we simulate a different number of
            //Modules than were actually connected
            //I think than we find nothing in the lookup table
            //I believe that a simple modif of the following function can remedy the
            //situation. At the moment it is ok that it throws a run time error
            //To be verified !!!!!!!!
	    //CAMM: it's much faster to use already the value saved instead of recalculating it from scratch... the method _mapping.getModuleIndexFromCellIndex() is quite time consuming.
	    UInt_t module_index = mydec.getModuleIndex();//_mapping.getModuleIndexFromCellIndex(hit_raw->getCellID0());
#ifdef RECO_DEBUG
	    std::cout << "Found module index: " << module_index << " from cell: " << std::hex << hit_raw->getCellID0() << std::dec << std::endl;
#endif
            //CRP The check on a valid module index has to be revised
	    //assert (module_index >= _mapping.getNModules());
	    UInt_t module_id = mydec.getModuleID();
	    UInt_t module_type = mydec.getModuleType();
	    UInt_t isBad = mydec.isBad();
	    assert (cell_i < 324);

	    UInt_t layer = 0;
	    UInt_t cell=0;
            //CRP Rearrange for the tme being to be save for
            //data -> to be verified for MC!!!!!
	    //CAMM: OK for MC!
            UInt_t access_parameter;
	    if (!_runInfo.isMC()) {
	      access_parameter = module_index;
	      cell = cell_i;
	    }
	    else {
	      layer = K-1;
              access_parameter = layer;
	      cell = 36*(3*(mydec.getWaferColumn()-1)+mydec.getWaferRow()-1)+6*(mydec.getPadColumn()-1)+mydec.getPadRow()-1;
	      assert (layer >= 0);
	      assert (layer < 30);
	      assert (cell >= 0);
	      assert (cell < 324);
	    }

	    //If at all
	    NoiseParameter &cell_parameter=noiseArray[access_parameter][cell];
	    Float_t noise = cell_parameter.getNoise();

	    // buffer the calibrated value, to avoid a recalculation
	    Float_t raw_value = 0;
	    if (!_runInfo.isMC()) raw_value = static_cast<float>(hit_raw->getAmplitude())/10000.;
	    else raw_value = static_cast<float>(hit_raw->getAmplitude());

	    //std::cout << " Hit : " << ele << ", module_id = " << module_id << ", module_type = " << module_type << ", cell_i = " << cell_i << ", layer " << K << " S,M,I,J : " << mydec.getWaferRow()<<","<<mydec.getWaferColumn()<<","<<mydec.getPadRow()<<","<<mydec.getPadColumn() << " ==> raw value = " << raw_value << std::endl;


	    Float_t calibrated_value = 0;
	    if (((_runInfo.isMC() && !cell_parameter.isDead()) || !_runInfo.isMC()) && isBad == 0) {
	      calibrated_value = _calibration->getCalibratedValue(module_id,module_type,cell_i,raw_value);
	    }
	    else if (_runInfo.isMC()) calibrated_value = 1./50*raw_value; //dummy value for the MC layers.

	    if (raw_value>minAdcSignalThreshold && raw_value>noise*_noiseCut && calibrated_value>_signalThreshold)
	      {// -- signal

		
#ifdef RECO_DEBUG
                //CRP 25/11/15 some sanity checks
                if(cell_parameter.isDead()) { 
                  std::cout << "is dead: ?" << cell_parameter.isDead() << std::endl;
                  std::cout << "is Bad: ?" << isBad << std::endl;
                  std::cout << "Noise: " <<  noise<< std::endl; 
                  std::cout << "Raw value: " << raw_value << std::endl;
                  std::cout << "Calibrated value: " << calibrated_value << std::endl;
                  std::cout << "Check cell: " << cell_i << std::endl;
                  std::cout << "CellID0 = " << std::hex << hit_raw->getCellID0() << std::endl <<" CellID1 = " << hit_raw->getCellID1() << std::dec << std::endl;
                  std::cout << "Check dead cells: " << mydec.getLayerIndex() << ", M=" << mydec.getWaferRow() << ", S=" << mydec.getWaferColumn() << ", I=" << mydec.getPadRow() << ", J=" << mydec.getPadColumn() << ", type=" << mydec.getModuleType() << ", ID=" << mydec.getModuleID() << ", index=" << mydec.getModuleIndex() << ", cell=" << mydec.getCellID() << ", isBad=" << mydec.isBad() << std::endl;
                } 
#endif
		IMPL::CalorimeterHitImpl *hit=new IMPL::CalorimeterHitImpl;
		hit->setCellID0(hit_raw->getCellID0());
		hit->setCellID1(hit_raw->getCellID1());
		hit->setTime(hit_raw->getTimeStamp());
		hit->setEnergy( calibrated_value );

		//FIXME: create a table in the database for the MC with the position of ALL cells saved, instead of using the data mapping...
		if (!_runInfo.isMC()){
		  hit->setPosition(_mapping.getPosition(module_index,cell_i).data());
		}
		else {//if MC
		  //if (!cell_parameter.isDead()) hit->setPosition(_mapping.getPosition(module_index,cell_i).data());
		  float pos[3] = {0,0,0};

		  if ((hit_raw->getCellID0()&0x80000000)  == 0) {//signal hit
		    //set the link to the initial simhit
		    if(col_RawToSim) {
		      for (int rela = 0; rela < col_RawToSim->getNumberOfElements(); rela++){
			const LCRelation* rel = (const LCRelation*)col_RawToSim->getElementAt(rela) ;
			if (((RawCalorimeterHit*)rel->getFrom())->getCellID0() == hit_raw->getCellID0()){
			  assert ((SimCalorimeterHit*)rel->getTo() != NULL);
			  hit->setRawHit((SimCalorimeterHit*)rel->getTo());
			  pos[0] = ((SimCalorimeterHit*)rel->getTo())->getPosition()[0];
			  pos[1] = ((SimCalorimeterHit*)rel->getTo())->getPosition()[1];
			  pos[2] = ((SimCalorimeterHit*)rel->getTo())->getPosition()[2];
			}
		      }
		    } else std::cout << "Signal hit but no relation to simhit???? " << std::endl;
		  }
		  if (isBad == 0) hit->setPosition(_mapping.getPosition(module_index,cell_i).data());
		  else { 
                    //If this cell has a valid module index then fill the positions from the mapping
                    if (_mapping.isValid(module_index)) hit->setPosition(_mapping.getPosition(module_index,cell_i).data());
                    else {
		      //CRP 26/11/15 We have now here a cell that was flagged bad somewhere upstream,
                      //maybe because it was physically not there inthe experimental setup but still
                      //got simulated. Early CALICE convention to simulate everything
                      //This happens for DESY 2005 and CERN 2006/2007  
                      //Since the CellID1 has no more any bit free to signal this we choose to modify the Mokka x,y-positions
		      //such that for all God's sake these cells can be easily spotted by the analyser 
		      float pos_fallback[3] = {0,0,0};
                      pos_fallback[0] = pos[0] - _fallbackShift;
                      pos_fallback[1] = pos[1] - _fallbackShift;
                      pos_fallback[2] = pos[2] - _fallbackShift;
                      hit->setPosition(pos_fallback);
#ifdef RECO_DEBUG
		      std::cout <<"CALICE::CalibrateAndApplyThreshold: Warning fallback shift needed: " << std::endl; 
#endif
		    }
		  }
		  
		  
		  if (col_RawToSim) {//kkk links hit to simhit

		       LCRelationNavigator nav_RawToSim(col_RawToSim);
		       const LCObjectVec & vec_hit_sim = nav_RawToSim.getRelatedToObjects(hit_raw);
		       if (vec_hit_sim.size() == 1) {
			    SimCalorimeterHit * hit_sim = dynamic_cast<SimCalorimeterHit*>(vec_hit_sim[0]);
			    nav_RecToSim.addRelation(hit, hit_sim);
		       } else if (vec_hit_sim.size() > 1) {
			    cout << "ERROR vec_hit_sim.size() = " << vec_hit_sim.size() << endl;
		       } else
			    /* kkk: There is no linked simulated hit.            */
			    /* This happens.                                     */
			    /* Probably noise hits have been added by digitizer. */
			    ;

		  } else { 
#ifdef RECO_DEBUG
                   std::cout << "Missing raw to sim relation " << std::endl;
#endif
                  } 


		}//if MC

		col_hit->addElement(hit);
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
		if (noise !=0) signal_over_noise_ratio.push_back(raw_value/noise);
#endif
	      }//signal

	    else {//noise
	    }

	  }//loop on raw hits
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
	if (col_hit->getNumberOfElements()>0) {
	  if (!signal_over_noise_ratio.empty()) col_hit->parameters().setValues(std::string("SN"),signal_over_noise_ratio);
	}
#endif
	  evtP->addCollection(col_hit,_hitColName);
	  //}
          //else {
	  //delete col_hit;
	  //}

	  if (_linkRecToSim) evtP->addCollection(nav_RecToSim.createLCCollection(), _RecToSimColName);


      }//if input col

    }//try
    catch ( DataNotAvailableException &err ){
      streamlog_out(DEBUG) << "Counting failing events: " << endl; 
      _nEventsWithoutRAWHITS++;
    }

    //CRP Suppressed firstEvent variable
    //firstEvent = false;
    _nEvt++;

  }//processEvt

  void CalibrateAndApplyThreshold::end() {
    std::cout << " Processor " << name() << " summary : " << endl <<
      "---> dropped " << _nEventsWithoutRAWHITS << " events without RAW hits," << endl <<
      "---> found " << _nEventsWithoutSIMHITS << " events without SIM hits," << endl <<
      "---> Total events processed : " << _nEvt << std::endl;

  }


}//namespace CALICE



