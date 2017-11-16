#include "AngleTrackFinder.hh"

#include <math.h>
#include <limits>

#include "marlin/Exceptions.h"
#include "UTIL/CellIDDecoder.h"

#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"

#include "AhcAmplitude.hh"
#include "ClusterOrdering.hh"

using std::cout;
using std::endl;
using namespace lcio;
using namespace marlin;
using namespace UTIL;
using namespace EVENT;

namespace CALICE 
{
  
  AngleTrackFinder AngleTrackFinder;
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  AngleTrackFinder::AngleTrackFinder() : Processor("AngleTrackFinder") 
  {
    _description = "Processor to find track from a MIP";
    
    registerProcessorParameter("MappingProcessorName",
			       "Name of the MappingProcessor instance that provides"
			       " the geometry of the detector.", _mappingProcessorName,
			       std::string("MyMappingProcessor"));
    
    registerProcessorParameter("CellDescriptionProcessorName",
			       "name of CellDescriptionProcessor which takes care of the cell description generation",
			       _cellDescriptionProcessorName, std::string("MyCellDescriptionProcessor"));
    
    registerProcessorParameter("AHCALInputCollectionName",
			       "Name of the collection of the AHCAL hits", _ahcalInputColName,
			       std::string("AhcCalorimeter_Hits"));
    
    registerProcessorParameter("AmplitudeHCALInputCollectionName",
			       "Name of the AHCAL Amplitude Collection", _ahcalAmpInputColName,
			       std::string("AhcHitAmplitudeRelation"));
    
    registerProcessorParameter("AHCALMuonOutputCollectionName",
			       "Name of the collection of the tracks found "
			       "in the AHCAL", _ahcalOutputTrackColName, std::string("AhcMuonHits"));

    registerProcessorParameter("TCMTMuonOutputCollectionName",
			       "Name of the collection of the tracks found "
			       "in the TCMT", _tcmtOutputTrackColName, std::string("TcmtMuonHits"));
    
    registerProcessorParameter("UseTCMT", "Use TCMT input hits", _useTCMT,
			       bool(false));
    
    registerProcessorParameter("UseECAL", "Use ECAL input hits", _useECAL,
			       bool(false));
    
    registerProcessorParameter("TcmtStartVertical",
			       "Orientation of first TCMT layer", _tcmtStartVertical, bool(true));
    
    registerProcessorParameter("TcmtStripWidth", "Width of a TCMT strip",
			       _tcmtStripWidth, float(50.0));
    
    registerProcessorParameter("TCMInputCollectionName",
			       "Name of the TCMT input collection", _tcmtInputColName,
			       std::string("TcmtCalorimeter_Hits"));
    
    registerProcessorParameter("TcmtMinActiveParallelStrips", "Minimum number of active parallel strips in TCMT",
			       _minActiveTCMTParallelStrips, int(6));
    
    registerProcessorParameter("ECALInputCollectionName",
			       "Name of the ECAL input collection", _ecalInputColName,
			       std::string("EmcCalorimeter_Hits"));
    
    registerProcessorParameter("IsNonMuonRun", "Is this a non-muon run?",
			       _isNonMuonRun, bool(false));
    
    registerProcessorParameter("MaxAHCALNoHits",
			       "Maximum number of hits in the AHCAL, used for a non-muon run",
			       _ahcalMaxNoHits, int(100));
    
    registerProcessorParameter("MinECALNoHits",
			       "Minimum number of hits in the ECAL", _ecalMinNoHits, int(30));
    
    registerProcessorParameter("MaxECALNoHits",
			       "Maximum number of hits in the ECAL", _ecalMaxNoHits, int(34));
    
    registerProcessorParameter("MinTCMTEnergySum",
			       "Minimum energy sum in the TCMT", _tcmtMinEnergySum, float(10));
    
    registerProcessorParameter("useCoarseSection", "use or not coarse section",
			       _useCoarseSection, int(0));
    
    registerProcessorParameter("MipCutValue", "Minimum energy in MIPs",
			       _mipCutValue, float(0.5));
    
    registerProcessorParameter("MipCutVariable",
			       "use (0) pure raw/MIP or (1) raw/MIP after whole calibration chain",
			       _mipCutVariable, int(1));
    IntVec badAhcalModulesVec;
    registerProcessorParameter("AHCALBadModules",
			       "List of bad AHCAL modules which need to be excluded",
			       _badAhcalModulesVec, badAhcalModulesVec);
    
    IntVec firstAhcalModulesVec;
    registerProcessorParameter("AHCALFirstModules",
			       "List of beginning AHCAL modules which are used"
			       " for perpendicular track finding",
			       _firstAhcalModulesVec,
			       firstAhcalModulesVec);
    
    IntVec lastAhcalModulesVec;
    registerProcessorParameter("AHCALLastModules",
			       "List of ending AHCAL modules which are used"
			       " for perpendicular track finding", 
			       _lastAhcalModulesVec,
			       lastAhcalModulesVec);
    
    registerProcessorParameter("TrackSelectionMode", "Track selection mode: "
			       "NHITS = based on the number of hits; "
			       "PERPENDICULAR = select only perpendicular tracks",
			       _trackSelectionMode, std::string("NHITS"));
    
    registerProcessorParameter("TrackNoHitsThreshold",
			       "Threshold on the number of hits a track "
			       "is considered to be a muon track (used only with TrackSelectionMode=NHITS)",
			       _trackNoHitsThreshold, int(20));
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  AngleTrackFinder::~AngleTrackFinder() 
  {
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::init() 
  {
    printParameters();
    
    _navigator = NULL;
    
    std::stringstream message;
    bool error = false;
    
    _mapper = dynamic_cast<const Mapper*> (MappingProcessor::getMapper(_mappingProcessorName));
    if (!_mapper) 
      {
	message << "MappingProcessor::getMapper(" << _mappingProcessorName
		<< ") did not return a valid mapper." << std::endl;
	error = true;
      }
    
    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if (!_cellDescriptions) 
      {
	streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "
		     << _cellDescriptionProcessorName
		     << ". Maybe, processor is not present" << std::endl;
	error = true;
      }
    
    if (error) 
      {
	streamlog_out(ERROR) << message.str();
	throw marlin::StopProcessingException(this);
      }
    
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::processEvent(LCEvent* evt) 
  {
    streamlog_out(DEBUG4) << "\n\n============================= Processing event "
		  << evt->getEventNumber() << endl;
            
    /* Get the incoming collections*/
    openInputCollections(evt);
    
    /* Start the track finding*/
    findTrack(evt);
    
    /* Clear vectors*/
    _allAhcalHitsInEventVec.clear();
    
    
    /*Delete the navigator, otherwise there is a memory leak.*/
    delete _navigator; 
    
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::openInputCollections(LCEvent* evt) 
  {
    try {
      if (_useECAL == true) 
	{
	  streamlog_out(DEBUG4) << "Opening ECAL collection " << _ecalInputColName
			<< endl;
	  _ecalInputCol = evt->getCollection(_ecalInputColName);
	}
      
      if (_useTCMT == true) 
	{
	  streamlog_out(DEBUG) << "Opening TCMT collection " << _tcmtInputColName
		       << endl;
	  _tcmtInputCol = evt->getCollection(_tcmtInputColName);
	}
      
      /* HCAL collection*/
      _ahcalInputCol = evt->getCollection(_ahcalInputColName);
      _encodingString = _ahcalInputCol->getParameters().getStringVal("CellIDEncoding");
      /*HCAL amplitude relation*/
      if (_mipCutVariable == 0) 
	{
	  _ahcalAmpInputCol = evt->getCollection(_ahcalAmpInputColName);
	  _navigator = new UTIL::LCRelationNavigator(_ahcalAmpInputCol);
	} 
      else 
	{
	  _ahcalAmpInputCol = NULL;
	  _navigator = NULL;
	}
      
      streamlog_out(DEBUG4) << "Opening AHCAL collection " << _ahcalInputColName
		    << " with " << _ahcalInputCol->getNumberOfElements() << " hits"
		    << endl;
      
    } 
    catch (DataNotAvailableException &e) 
      {
	streamlog_out(WARNING) << "At least one required collection is missing:\n "
		       << _ahcalInputColName << ", " << _ecalInputColName << " or "
		       << _tcmtInputColName << " in the event "
		       << evt->getEventNumber() << endl;
	
	throw marlin::SkipEventException(this);
      }
    
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::findTrack(LCEvent* evt) 
  {
    if (this->doFurtherAnalysis() == false) return; 

    streamlog_out(DEBUG4) << "\n\n ============ Start to find track in event "
		  << evt->getEventNumber() << endl;
    
    
    /* All hits above 0.5 MIP are filled into the vector hits.*/
    std::vector<CalorimeterHit*> hcalHitsVec = this->extractAHCALInformation();
    
    if (hcalHitsVec.size() > (unsigned) _ahcalMaxNoHits) 
      {
	streamlog_out(DEBUG) << "Number of hits above mip cut: "
		     << hcalHitsVec.size() << ", skipping event." << endl;
	return;
      }
    
    /*------------------------------------------------------------------- 
       All hits above 0.5 MIP which are recognized as a track are
       in this vector of ClusterVec -fine section-*/
    
    EVENT::ClusterVec trackClusterVec;
    this->findTrackClusters(hcalHitsVec, trackClusterVec);
    
    /*------------------------------------------------------------------- 
       All hits above 0.5 MIP which are recognized as a track are
       in this vector of ClusterVec -coarse section-*/
    EVENT::ClusterVec trackClusterVecCoarse;
    
    if (_useCoarseSection == 1) 
      {
	/*
	  Angela Lucaci: Ugly, I know. But the hcalHitsVec is emptied in the this->findTrackClusters()...
	*/
	hcalHitsVec = this->extractAHCALInformation();
	this->findTrackClustersCoarse(hcalHitsVec, trackClusterVecCoarse);
      }
 
    /* This vector holds the selected tracks */
    this->selectTrackClusters(trackClusterVec, _trackSelectionMode, _goodTrackClusters);
        
    EVENT::ClusterVec selectedFineCoarseVec;

    if (_useCoarseSection == 1) 
      {
	this->mergeCoarseClusters(_goodTrackClusters, 
				  trackClusterVecCoarse,
				  selectedFineCoarseVec,
				  _trackSelectionMode);
      }
    
    EVENT::ClusterVec outputTrackSegments;
    if (_useCoarseSection == 1) 
      {
	outputTrackSegments = this->getRemainingHitsOfTrackSegment(selectedFineCoarseVec);
      } 
    else 
      {
	outputTrackSegments = this->getRemainingHitsOfTrackSegment(_goodTrackClusters);
      }
	
    streamlog_out(DEBUG) << "Preparing the output " << endl;
    
    EVENT::ClusterVec outputTrackClusterVec = this->getFinalCluster(outputTrackSegments);
    
    streamlog_out(DEBUG) << outputTrackClusterVec.size() << " found tracks"
		 << endl;
    streamlog_out(DEBUG) <<"\n UseTCMT: "<<_useTCMT<<endl;

    if (_useTCMT) 
      {
	LCTypedVector<CalorimeterHit> tcmtHits(_tcmtInputCol);
	this->extendTracksToTcmt(tcmtHits, outputTrackClusterVec);
      }
    
    /* Attach the final tracks to the event.*/
    streamlog_out(DEBUG) << "Create the collection " << endl;
    createOutputCollection(evt, outputTrackClusterVec);
    
    streamlog_out(DEBUG) << "Quite in the end!" << endl;
      
    
    /*----------------------------------------------------------------------------------------*/
    /*Don't forget to clear the contents of the used vectors, otherwise you get a memory leak*/
    for (unsigned int i = 0; i < trackClusterVec.size(); ++i)
      delete trackClusterVec[i];
    trackClusterVec.clear();
    
    for (unsigned int i = 0; i < trackClusterVecCoarse.size(); ++i)
      delete trackClusterVecCoarse[i];
    trackClusterVecCoarse.clear();
    
    // 	selectedFineCoarseVec.clear();
    
    hcalHitsVec.clear();
    
    
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  bool AngleTrackFinder::doFurtherAnalysis() 
  {
    /* here apply criteria to use the number of hits in HCAL to  apply the muon*/
    bool analyse = false;
    
    if (_useECAL == true) 
      {
	streamlog_out(DEBUG4) << "Using ECAL and TCMT information" << endl;
	
	/* ECAL information*/
	std::pair<int, float> nhits_and_esum(extractNumberOfHitsAndEnergySum(_ecalInputCol));
	int ecalNumberOfHits = nhits_and_esum.first;
	
	if ((ecalNumberOfHits > _ecalMinNoHits) && (ecalNumberOfHits < _ecalMaxNoHits)) 
	  {
	    analyse = true;
	    streamlog_out(DEBUG4) << "ECAL criteria reached. "
				  << "Going to do further processing." << endl;
	  } 
	else 
	  {
	    analyse = false;
	    streamlog_out(DEBUG4) << "ECAL criteria *not* reached. "
				  << "Event will be skipped." << endl;
	  }
      }
    else if (_useTCMT == true)
      {
	// TCMT information
	std::pair<int, float> nhits_and_esum = extractNumberOfHitsAndEnergySum(_tcmtInputCol);
	float tcmtEnergySum = nhits_and_esum.second;
	
	if((tcmtEnergySum > _tcmtMinEnergySum))
	  {
	    analyse = true;
	    streamlog_out(DEBUG4) << "TCMT criteria reached. "
				  << "Going to do further processing." << endl;
	  }
	else
	  {
	    analyse = false;
	    streamlog_out(DEBUG4) << "TCMT criteria *not* reached. "
				  << "Event will be skipped." << endl;
	  }
      }
    else 
      {
	if (_isNonMuonRun == true) 
	  {
	    /* this is just for non-muon runs case!*/
	    std::pair<int, float> nhits_and_esum(extractNumberOfHitsAndEnergySum(_ahcalInputCol));
	    int nhits_hcal = nhits_and_esum.first;
	    streamlog_out(DEBUG4) << "hcal hits " << nhits_hcal << endl;
	    
	    // FIXME this check makes no sense here since there are always hits in every cell
	    // check is done in findTrack instead
	    if (nhits_hcal < _ahcalMaxNoHits) 
	      {
		analyse = true;
	      } 
	    else 
	      {
		analyse = true;
	      }
	  } 
	else 
	  {
	    analyse = true;
	  }
	streamlog_out(DEBUG4) << "  ->AHCAL only analysis will start." << endl;
      }
    
    return analyse;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  std::pair<int, float> AngleTrackFinder::extractNumberOfHitsAndEnergySum(LCCollection *inputCol) 
  {
    float energySum = 0;
    const int numberOfHits = inputCol->getNumberOfElements();
    
    for (int i = 0; i < numberOfHits; ++i) 
      {
	CalorimeterHit *hit = dynamic_cast<CalorimeterHit*> (inputCol->getElementAt(i));
	energySum += hit->getEnergy();
      }/*end loop over input collection*/
    
    return std::pair<int, float>(numberOfHits, energySum);
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  std::vector<CalorimeterHit*> AngleTrackFinder::extractAHCALInformation() 
  {
    streamlog_out(DEBUG4) << "\n extractAHCALInformation:" << endl;
    /*check if there is a list of bad modules*/
    bool haveListOfBadModules = parameterSet("AHCALBadModules");
    
    streamlog_out(DEBUG4) << "  haveListOfBadModules: " << haveListOfBadModules << endl;
    
    LCTypedVector<CalorimeterHit> hcalVec(_ahcalInputCol);
    _allAhcalHitsInEventVec = hcalVec;
    std::vector<CalorimeterHit*> selectedHcalHitsVec;
    
    /*counter for debug purposes*/
    int counter = 0;
        
    for (unsigned int iHits = 0; iHits < hcalVec.size(); ++iHits) 
      {
	const int cellID = hcalVec[iHits]->getCellID0();
	const int module = _mapper->getModuleFromCellID(cellID);
	
	bool hitIsInBadModule = false;

	if (haveListOfBadModules == true) 
	  {
	    if (std::find(_badAhcalModulesVec.begin(), _badAhcalModulesVec.end(), module) != _badAhcalModulesVec.end())
	      hitIsInBadModule = true;
	    
	    if (hitIsInBadModule) 
	      {
		streamlog_out(DEBUG4) << " -> " << counter << " hit in module "
			      << module << "-> on list of bad modules" << endl;
	      } 
	    else 
	      {
		streamlog_out(DEBUG4) << " -> " << counter << " hit in module "
			      << module << "-> NOT on list of bad modules" << endl;
	      }
	  }

	// get the amplitude without temperature correction
	float ampMipNoTempNoSat = 0.;
	
	if (_mipCutVariable == 0) 
	  {
	    const LCObjectVec &amplVec = _navigator->getRelatedToObjects(hcalVec[iHits]);
	    
	    if (amplVec.size() > 0) 
	      {
		LCGenericObject *obj = dynamic_cast<LCGenericObject*> (amplVec[0]);
		AhcAmplitude *ahcAmpl = new AhcAmplitude(obj);
		ampMipNoTempNoSat = ahcAmpl->getAmplNOTTemperatureCorrMIP();
		delete ahcAmpl;
	      }
	  }
	
	/* get the amplitude after temperature correction & saturated corrected*/      
	float ampMipFinal = hcalVec[iHits]->getEnergy();
	
	/* choose amplitude to be used*/      
	float mipCut = 0;
	if (_mipCutVariable == 1)
	  {
	    mipCut = ampMipFinal;
	  } 
	else if (_mipCutVariable == 0) 
	  {
	    mipCut = ampMipNoTempNoSat;
	  }
	
	// Now do the selection(!)
	bool isBadHit = ((haveListOfBadModules == true && hitIsInBadModule == true) || (mipCut < _mipCutValue));
	if (isBadHit == false) selectedHcalHitsVec.push_back(hcalVec[iHits]);
      }
    
    streamlog_out(DEBUG4) << " -> " << counter
		  << " hits do not fullfill MIP cut and/or are in bad modules"
		  << endl;
    streamlog_out(DEBUG4) << " -> selected AHCAL hits: " << hcalVec.size() << endl;
    
    
    
    return selectedHcalHitsVec;
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::findTrackClusters(std::vector<CalorimeterHit*> &hcalVec, EVENT::ClusterVec &clusterVec) 
  {
    streamlog_out(DEBUG4) << "\n--------------- Start to find track clusters" << endl;
    
    clusterVec.clear();

    LCTypedVector<CalorimeterHit>::iterator seedIter = hcalVec.begin();
    while (seedIter != hcalVec.end()) 
      {
	CalorimeterHit *seedHit = (*seedIter);
	const int cellID = (*seedIter)->getCellID0();
	/*erase seed hit*/
	hcalVec.erase(seedIter);
	
	/*use only hits in fine modules to find tracks*/
	const int K = _mapper->getDecoder()->getKFromCellID(cellID);
	
	if (K <= 30) 
	  {
	    ClusterImpl *cluster = new ClusterImpl;
	    /*first member of the cluster is the seed hit*/
	    cluster->addHit(seedHit, 1.);
	    
	    LCTypedVector<CalorimeterHit>::iterator potentialTrackHitIter = hcalVec.begin();
	    while (potentialTrackHitIter != hcalVec.end()) 
	      {
		float differenceInX = fabs((*potentialTrackHitIter)->getPosition()[0] - seedHit->getPosition()[0]);
		float differenceInY = fabs((*potentialTrackHitIter)->getPosition()[1] - seedHit->getPosition()[1]);
		
		CellDescription* seedHitCellDescription = _cellDescriptions->getByCellID(cellID);
		const float seedHitTileSize             = seedHitCellDescription->getSizeX();
		const float potentialTrackHitTileSize   = _cellDescriptions->getByCellID((*potentialTrackHitIter)->getCellID0())->getSizeX();
		
		float meanCellSize = 0.5 * (potentialTrackHitTileSize + seedHitTileSize);
		
		if (differenceInX < meanCellSize && differenceInY < meanCellSize) 
		  {
		    streamlog_out(DEBUG4) << " New track hit found " << endl;
		    
		    /*add hit to the track cluster*/
		    cluster->addHit((*potentialTrackHitIter), 1.);
		    
		    /*erase track hit from the HCAL seeds vector*/
		    potentialTrackHitIter = hcalVec.erase(potentialTrackHitIter);
		  } 
		else 
		  {
		    ++potentialTrackHitIter;
		  }
	      }/*------------- end loop over potential track hits -------------------*/
	    
	    clusterVec.push_back(cluster);
	    
	  }/*----------------- end loop over fine layers ----------------------------*/
	
	seedIter = hcalVec.begin();
      }/*--------------------- end loop over AHCAL seed hits ------------------------*/
    

    streamlog_out(DEBUG)<<"\n findTrackClusters: found "<<clusterVec.size()<<" track clusters"<<endl;
  }
  
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::findTrackClustersCoarse(std::vector<CalorimeterHit*> &hcalVec, EVENT::ClusterVec &clusterCoarseVec) 
  {
    streamlog_out(DEBUG4) << "\n--------------- Start to find track clusters" << endl;
    
    clusterCoarseVec.clear();
 
    LCTypedVector<CalorimeterHit>::iterator seedIter = hcalVec.begin();
    while (seedIter != hcalVec.end()) 
      {
	CalorimeterHit *seedHit = (*seedIter);
	const int cellID = seedHit->getCellID0();
	/*erase seed hit*/
	hcalVec.erase(seedIter);
	
	/*use only hits in the coarse modules to find the last track segment of the track*/
	const int K = _mapper->getDecoder()->getKFromCellID(cellID);


	if (K > 30) 
	  {
	    ClusterImpl *cluster = new ClusterImpl;
	    /*first member of the cluster is the seed hit*/
	    cluster->addHit(seedHit, 1.);
	    
	    LCTypedVector<CalorimeterHit>::iterator potentialTrackHitIter = hcalVec.begin();
	    
	    while (potentialTrackHitIter != hcalVec.end()) 
	      {
		float differenceInX = fabs((*potentialTrackHitIter)->getPosition()[0]
					   - seedHit->getPosition()[0]);
		float differenceInY = fabs((*potentialTrackHitIter)->getPosition()[1]
					   - seedHit->getPosition()[1]);
		
		CellDescription* seedHitCellDescription =_cellDescriptions->getByCellID(cellID);
		const float seedHitTileSize = seedHitCellDescription->getSizeX();
		const float potentialTrackHitTileSize = _cellDescriptions->getByCellID((*potentialTrackHitIter)->getCellID0())->getSizeX();
		
		float meanCellSize = 0.5 * (potentialTrackHitTileSize + seedHitTileSize);
		
		/* do not take again the layers in fine section*/
		const int cellIDP = (*potentialTrackHitIter)->getCellID0();
		HcalCellIndex geomIndexP(cellIDP);
		int layernum = geomIndexP.getLayerIndex();
		
		if (differenceInX < meanCellSize && differenceInY < meanCellSize && layernum > 30) 
		  {
		    streamlog_out(DEBUG4) << " New track hit found " << endl;
		    
		    /*add hit to the track cluster*/
		    cluster->addHit((*potentialTrackHitIter), 1.);
		    /*erase track hit from the HCAL seeds vector*/
		    potentialTrackHitIter = hcalVec.erase(potentialTrackHitIter);
		  }
		else 
		  {
		    ++potentialTrackHitIter;
		  }
	      }/*------------- end loop over potential track hits -------------------*/
	    
	    clusterCoarseVec.push_back(cluster);
	  }/*----------------- end loop over fine layers ----------------------------*/
	
	seedIter = hcalVec.begin();
      }/*--------------------- end loop over AHCAL seed hits ------------------------*/
    

    streamlog_out(DEBUG)<< "\n findTrackClustersCoarse: Number of tracks in coarse section: " << clusterCoarseVec.size() << endl;

  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec AngleTrackFinder::extendTracksToTcmt(LCTypedVector<CalorimeterHit> tcmtHits, EVENT::ClusterVec trackClusters) 
  {
    streamlog_out(DEBUG0) << "Tail catcher hits: " << tcmtHits.size() << endl;
    streamlog_out(DEBUG0) << "Track candidates to extend: " << trackClusters.size()
		  << endl;
    
    /* get the cellID decoder for the TCMT hits*/
    FastDecoder * decoder_k = FastDecoder::generateDecoder(_tcmtInputCol->getParameters().getStringVal("CellIDEncoding"), "K");
    
    for (ClusterVec::iterator iTrack = trackClusters.begin(); iTrack != trackClusters.end(); ++iTrack) 
      {
	/* make the track modifyable*/
	ClusterImpl* track = (ClusterImpl*) (*iTrack);
	CalorimeterHitVec trackHits = track->getCalorimeterHits();

	streamlog_out(DEBUG0) << "Track size before: " << trackHits.size() << endl;

	if (trackHits.size() == 0) 
	  {
	    streamlog_out(DEBUG0) << "No hits in track. Continue with next track."
			  << endl;
	    continue;
	  }
	
	/* find the first and last hit in the track candidate*/
	CalorimeterHit* firstHit = 0;
	CalorimeterHit* lastHit = 0;
	float zMin = numeric_limits<float>::max();
	float zMax = -numeric_limits<float>::max();

	for (CalorimeterHitVec::iterator iHit = trackHits.begin(); iHit != trackHits.end(); ++iHit) 
	  {
	    if ((*iHit)->getPosition()[2] < zMin) 
	      {
		firstHit = *iHit;
		zMin = firstHit->getPosition()[2];
	      }
	    if ((*iHit)->getPosition()[2] > zMax) 
	      {
		lastHit = *iHit;
		zMax = lastHit->getPosition()[2];
	      }
	  }
	
	streamlog_out(DEBUG0) << "Last track hit: " << lastHit->getCellID0() << ", "
		      << lastHit->getCellID1() << endl;
	
	/* get x-y position of last hit and cell size*/
	const float xPos = lastHit->getPosition()[0];
	const float yPos = lastHit->getPosition()[1];
	
	/* calculate the slope along the track*/
	const float xSlope = (xPos - firstHit->getPosition()[0]) / (zMax - zMin);
	const float ySlope = (yPos - firstHit->getPosition()[1]) / (zMax - zMin);

	streamlog_out(DEBUG0) << "Track: x " << xPos << ", y " << yPos << ", z "
		      << zMax << ", x-slope " << xSlope << ", y-slope " << ySlope
		      << endl;
	
	for (CalorimeterHitVec::iterator iHit = tcmtHits.begin(); iHit != tcmtHits.end(); ++iHit) 
	  {
	    CalorimeterHitImpl* tcmtHit = (CalorimeterHitImpl*) *iHit;
	    const float x = tcmtHit->getPosition()[0];
	    const float y = tcmtHit->getPosition()[1];
	    const float z = tcmtHit->getPosition()[2];
	    
	    /* FIXME should be obtained from the mapping instead*/
	    const float stripWidth = _tcmtStripWidth;
	    
	    /* obtain strip orientation from k position of layer*/
	    /* FIXME should be obtained in a direct way instead of having to know the orientation of the first layer*/
	    bool horizontalStrip = false;
	    unsigned int k_pos = decoder_k->decodeU(tcmtHit->getCellID0());
	    if (_tcmtStartVertical)
	      horizontalStrip = (k_pos % 2 ? true : false);
	    else
	      horizontalStrip = (k_pos % 2 ? false : true);
	    
	    /* extrapolate track to tcmt hit*/
	    const float xExt = (z - zMax) * xSlope + xPos;
	    const float yExt = (z - zMax) * ySlope + yPos;
	    
	    streamlog_out(DEBUG0) << "TailCatcherHit: x " << x << ", y " << y << ", z "
			  << z << " horizontalStrip = "<<horizontalStrip<<" energy="<<tcmtHit->getEnergy()<<endl;
	    streamlog_out(DEBUG0) << "TcmtHitExtrapolation: x' " << xExt << ", y' "
			  << yExt << ", z' " << z << endl;
	    

	    /* Check if hit is compatible with track extrapolation, allow distance of strip width*/
	    bool match = false;
	    
	    if (horizontalStrip) 
	      {
		if (fabs(y - yExt) < 1.5*stripWidth) match = true;
	      } 
	    else 
	      {
		if (fabs(x - xExt) < stripWidth) match = true;
	      }
	    
	    if (match) 
	      {

		streamlog_out(DEBUG0) << " --->Found matching hit! Energy: "
			      << tcmtHit->getEnergy() << " CellId "
			      << tcmtHit->getCellID0() << ", "
			      << tcmtHit->getCellID1() << endl;
		/* change the second cell ID to 1 in order to indicate a tcmt hit*/
		tcmtHit->setCellID1(1);
		/* add the hit to the track*/
		track->addHit(tcmtHit, 1.0);
		/* and remove it from the list of hits*/
		tcmtHits.erase(iHit);
		/* move iterator back, we just deleted the entry*/
		--iHit;
	      }
	  }
	streamlog_out(DEBUG0) << "Tcmt hits left: " << tcmtHits.size() << endl;
	streamlog_out(DEBUG0) << "Track size after: "
		      << track->getCalorimeterHits().size() << endl;
      }
    
    delete decoder_k;
    
    return trackClusters;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::selectTrackClusters(EVENT::ClusterVec clusterVec, 
					     const std::string trackSelectionMode,
					     EVENT::ClusterVec &_goodTrackClusters) 
  {
    if (trackSelectionMode != "ANGLE") 
      {
	streamlog_out(ERROR) << "No TrackSelectionMode chosen, aborting..." << endl;
	exit(1);
      }

    _goodTrackClusters.clear();

    /*order the track segments by size in decreasing order*/
    std::sort(clusterVec.begin(), clusterVec.end(), ClusterOrdering::compareVectBySize);    
    
    if (clusterVec.size() == 0)
      {
	streamlog_out(DEBUG)<<"selectTrackClusters: clusterVec.size()=0"<<endl;
	return;
      }

    streamlog_out(DEBUG)<<"\n-----------------------> Start to selectTrackClusters, size="<<clusterVec.size()<<endl;

    /* here will store the good track segments*/
    CalorimeterHitVec firsthitVec = clusterVec[0]->getCalorimeterHits();/* check the first track segment*/
    
    streamlog_out(DEBUG)<<" firsthitVec.size()="<<firsthitVec.size()<<", threshold: "<<_trackNoHitsThreshold<<endl;
    
    /* use track segments with a minimum number of Hits*/
    if (firsthitVec.size() > (unsigned int) _trackNoHitsThreshold) 
      {	  
	EVENT::ClusterVec subCluster = this->obtainSubClusters(clusterVec[0]);
	streamlog_out(DEBUG)<<" initial subcluster size: "<<subCluster.size()<<endl;
	
	/*But remove first isolated hits*/
	EVENT::ClusterVec subClusterClean = this->removeIsolatedHits(subCluster);
	streamlog_out(DEBUG)<<" after removing isolated hits: "<<subClusterClean.size()<<endl;
	
	/* Add*/
	for (unsigned isubCl = 0; isubCl < subClusterClean.size(); isubCl++) 
	  {
	    _goodTrackClusters.push_back(subClusterClean[isubCl]);
	  }
	
	streamlog_out(DEBUG) << "Go now to the next towers " << endl;
	    
	/* Now check if the next Clusters are close to the seed track segment*/
	unsigned iSearch = 1;
		
	while (iSearch < clusterVec.size()) 
	  {
	    CalorimeterHitVec hitVec = clusterVec[iSearch]->getCalorimeterHits();
	    	    
	    /* and at least they contain 3 hits!*/
	    if (hitVec.size() > 2) 
	      {
		/* order by layer*/
		bool tobeMerged = ClusterOrdering::checkXYAdyacentClusters(clusterVec[0], clusterVec[iSearch], 3);
				
		if (tobeMerged) 
		  {
		    EVENT::ClusterVec nextsubCluster = this->obtainSubClusters(clusterVec[iSearch]);
		    		    
		    EVENT::ClusterVec nextsubClusterClean = this->removeIsolatedHits(nextsubCluster);
		    		    
		    for (unsigned isubCl = 0; isubCl < nextsubClusterClean.size(); isubCl++) 
		      {
			_goodTrackClusters.push_back(nextsubClusterClean[isubCl]);
		      }
		  }
	      }
	    iSearch++;
	  }
      }
    
    streamlog_out(DEBUG)<<"selectTrackClusters: found "<<_goodTrackClusters.size()<<" good track clusters"<<endl;
    

  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::mergeCoarseClusters(EVENT::ClusterVec goodClusterVec, 
					     EVENT::ClusterVec clusterCoarseVec,
					     EVENT::ClusterVec &mergeFineCoarseClusters,
					     const std::string trackSelectionMode) 
  {
    if (trackSelectionMode == "ANGLE") 
      {
	if (goodClusterVec.size() > 0) 
	  {	  
	    streamlog_out(DEBUG) << "Number of clusters in the fine Section " << goodClusterVec.size() << endl;
	    /* get X,Y of the last cluster in the fine section*/
	    float xlastFine = 0;
	    float ylastFine = 0;
	    float zlastFine = 0;
	  
	    for (unsigned iloop = 0; iloop < goodClusterVec.size(); iloop++) 
	      {
		mergeFineCoarseClusters.push_back(goodClusterVec[iloop]);
		CalorimeterHitVec finehitVec = goodClusterVec[iloop]->getCalorimeterHits();
		/* the cluster are arranged in decreasing order*/
		if (iloop == 0) 
		  {
		    xlastFine = finehitVec[0]->getPosition()[0];
		    ylastFine = finehitVec[0]->getPosition()[1];
		    zlastFine = finehitVec[0]->getPosition()[2];
		  }
	    }
	    
	    streamlog_out(DEBUG) << "Before Number of SubClusters: " << mergeFineCoarseClusters.size() << endl;
	    
	    /* display some coarse quantities*/
	    
	    std::sort(clusterCoarseVec.begin(), clusterCoarseVec.end(), ClusterOrdering::compareVectBySize);
	    
	    for (unsigned iloop = 0; iloop < clusterCoarseVec.size(); iloop++) 
	      {
		CalorimeterHitVec coarsehitVec = clusterCoarseVec[iloop]->getCalorimeterHits();
		/* order it by layer*/
		std::sort(coarsehitVec.begin(), coarsehitVec.end(), ClusterOrdering::compareHitByLayer);
		
		/*check the towers which at least have 3 hits in the coarse section*/
		if (coarsehitVec.size() >= 3) 
		  {
		    /* check all the coarse subclusters since x-y is the same,
		       only use the first hit */
		    float dXfine_coarse = fabs(coarsehitVec[0]->getPosition()[0] - xlastFine);
		    float dYfine_coarse = fabs(coarsehitVec[0]->getPosition()[1] - ylastFine);
		    
		    if (dXfine_coarse < 61 && dYfine_coarse < 61) 
		      {
			/* order by layer*/			
			mergeFineCoarseClusters.push_back(clusterCoarseVec[iloop]);
		      }
		  }
	      }
	    streamlog_out(DEBUG) << "After Number of SubClusters: " << mergeFineCoarseClusters.size() << endl;
	  } 
	else 
	  {
	    streamlog_out(DEBUG) << "no track found" << endl;
	  }
	
	/* print out*/
	/*
	  for (unsigned icl=0;icl<mergeFineCoarseClusters.size();icl++){
	  cout << "at the end of fine-coarse section ---- " << icl << endl;
	  CalorimeterHitVec hitVecCl =mergeFineCoarseClusters[icl] ->getCalorimeterHits();
	  for (int il=0;il<hitVecCl.size();il++){
	  const int cellIDA = hitVecCl[il]->getCellID0();
	  HcalCellIndex geomIndex(cellIDA);
	  int iLayer = geomIndex.getLayerIndex();
	  cout << "xxxx having layers: " << iLayer << " x/y " << hitVecCl[il]->getPosition()[0] 
	  << " / " << hitVecCl[il]->getPosition()[1] << " il: " << il << " energy " << hitVecCl[il]->getEnergy() << endl;
	  }
	  }
	*/
      } 
    else 
      {
	streamlog_out(ERROR) << "No TrackSelectionMode chosen, aborting..." << endl;
	exit(1);
      }
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  ClusterImpl* AngleTrackFinder::getWholeTrack(Cluster *trackCluster) 
  {
    float meanXOfTrack = 0;
    float meanYOfTrack = 0;
    
    CalorimeterHitVec hitVec = trackCluster->getCalorimeterHits();
    for (unsigned int iHits = 0; iHits < hitVec.size(); ++iHits) 
      {
	meanXOfTrack += hitVec[iHits]->getPosition()[0];
	meanYOfTrack += hitVec[iHits]->getPosition()[1];
      }/*------------ end loop over track clusters -----------------*/
    
    meanXOfTrack /= hitVec.size();
    meanYOfTrack /= hitVec.size();
    
    streamlog_out(DEBUG4) << " Mean X: " << meanXOfTrack << " mean Y: " << meanYOfTrack
		  << endl;
    
    ClusterImpl *wholeTrack = new ClusterImpl();
    
    EVENT::CalorimeterHitVec::iterator iter;
    for (iter = _allAhcalHitsInEventVec.begin(); iter != _allAhcalHitsInEventVec.end(); ++iter) 
      {
	float differenceInX = fabs((*iter)->getPosition()[0] - meanXOfTrack);
	float differenceInY = fabs((*iter)->getPosition()[1] - meanYOfTrack);
	
// 	streamlog_out(DEBUG4) << " differenceInX: " << differenceInX
// 		      << " differenceInY: " << differenceInY << endl;
	
	/*be aware of the misalignment information in the future*/
	if (differenceInX < 16 && differenceInY < 16) 
	  {
// 	    streamlog_out(DEBUG0) << " Added hit to the track" << endl;
	    wholeTrack->addHit((*iter), 1.);
	  }
      }/*----------------- end loop over all AHCAL hits in the event*/
    
    return wholeTrack;
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  ClusterImpl* AngleTrackFinder::copyACluster(Cluster *trackCluster) 
  {
    ClusterImpl *wholeTrack = new ClusterImpl();
    CalorimeterHitVec hitVec = trackCluster->getCalorimeterHits();
    
    streamlog_out(DEBUG) << "Elements:: " << hitVec.size() << endl;
    for (unsigned int iHits = 0; iHits < hitVec.size(); ++iHits) 
      {
	wholeTrack->addHit(hitVec[iHits], 1.);
      }
    return wholeTrack;
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec AngleTrackFinder::getRemainingHits(EVENT::ClusterVec selectedTrackClusters) 
  {
    EVENT::ClusterVec outputClusterVec;
    
    for (unsigned int iCluster = 0; iCluster < selectedTrackClusters.size(); ++iCluster) 
      {
	outputClusterVec.push_back(this->getWholeTrack(selectedTrackClusters[iCluster]));
      }/*--------------------- end loop over track clusters -----------------*/
    
    return outputClusterVec;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec AngleTrackFinder::getRemainingHitsOfTrackSegment(EVENT::ClusterVec aselectedTrackClusters) 
  {
    EVENT::ClusterVec outputClusterVec;
    
    for (unsigned int iCluster = 0; iCluster < aselectedTrackClusters.size(); ++iCluster) 
      {
	streamlog_out(DEBUG) << "getRemainingHitsOfTrackSegment iCluster: " << iCluster << endl;
	outputClusterVec.push_back(this->getWholeTrackSegment(aselectedTrackClusters[iCluster]));
      }/*--------------------- end loop over track clusters -----------------*/
    
    streamlog_out(DEBUG)<<"\n getRemainingHitsOfTrackSegment: size="<<outputClusterVec.size()<<endl;

    return outputClusterVec;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  ClusterImpl* AngleTrackFinder::getWholeTrackSegment(Cluster *trackCluster) 
  {
    float meanXOfTrack = 0;
    float meanYOfTrack = 0;
    
    int iniLayerSegment;
    int endLayerSegment;
    
    CalorimeterHitVec hitVec = trackCluster->getCalorimeterHits();
    /* order by Layer!*/
    std::sort(hitVec.begin(), hitVec.end(), ClusterOrdering::compareHitByLayer);

    /* find initial and end;*/
    
    const int cellIDAini = hitVec[0]->getCellID0();
    HcalCellIndex geomIndexIni(cellIDAini);
    iniLayerSegment = geomIndexIni.getLayerIndex();
    
    const int cellIDAend = hitVec[hitVec.size() - 1]->getCellID0();
    HcalCellIndex geomIndexEnd(cellIDAend);
    endLayerSegment = geomIndexEnd.getLayerIndex();
    
    /*cout << "getWholeTrackSegment ======= " << iniLayerSegment << "/t" << endLayerSegment << endl;*/
    
    for (unsigned int iHits = 0; iHits < hitVec.size(); ++iHits) 
      {
	meanXOfTrack += hitVec[iHits]->getPosition()[0];
	meanYOfTrack += hitVec[iHits]->getPosition()[1];
      }/*------------ end loop over track clusters -----------------*/
    
    meanXOfTrack /= hitVec.size();
    meanYOfTrack /= hitVec.size();
    
    streamlog_out(DEBUG4) << " Mean X: " << meanXOfTrack << " mean Y: " << meanYOfTrack
		  << endl;
    
    ClusterImpl *wholeTrack = new ClusterImpl();
    
    EVENT::CalorimeterHitVec::iterator iter;
    for (iter = _allAhcalHitsInEventVec.begin(); iter != _allAhcalHitsInEventVec.end(); ++iter) 
      {
	float differenceInX = fabs((*iter)->getPosition()[0] - meanXOfTrack);
	float differenceInY = fabs((*iter)->getPosition()[1] - meanYOfTrack);
	streamlog_out(DEBUG4) << " differenceInX: " << differenceInX
		      << " differenceInY: " << differenceInY << endl;
	
	/*be aware of the misalignment information in the future*/
	
	if (differenceInX < 16 && differenceInY < 16) 
	  {
	    const int cellIDA = (*iter)->getCellID0();
	    HcalCellIndex geomIndex(cellIDA);
	    int iLayer = geomIndex.getLayerIndex();

	    if (iLayer >= iniLayerSegment && iLayer <= endLayerSegment) 
	      {
		streamlog_out(DEBUG) << "coarse difference: " << differenceInX << "\t" 
			     << differenceInY << " layer " <<iLayer << " energy " << (*iter)->getEnergy() << endl;
		streamlog_out(DEBUG0) << " Added hit to the track" << endl;
		wholeTrack->addHit((*iter), 1.);
	      }
	  }
	
      }/*----------------- end loop over all AHCAL hits in the event*/
    
    return wholeTrack;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec AngleTrackFinder::getFinalCluster(EVENT::ClusterVec selectedTrackClusters) 
  {
    EVENT::ClusterVec outputClusterVec;
    
    streamlog_out(DEBUG) << "Number of subcluster: " << selectedTrackClusters.size() << endl;
    
    for (unsigned int iCluster = 0; iCluster < selectedTrackClusters.size(); ++iCluster) 
      {
	outputClusterVec.push_back(this->copyACluster(selectedTrackClusters[iCluster]));
      }/*--------------------- end loop over track clusters -----------------*/
    
    return outputClusterVec;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AngleTrackFinder::createOutputCollection(LCEvent *evt, EVENT::ClusterVec finalVec) 
  {
    LCCollection *muonHcalCol  = new LCCollectionVec(LCIO::CALORIMETERHIT);
    /*we want to save the position, and this can only be done if the CHBIT_LONG bit is set*/
    muonHcalCol->setFlag(muonHcalCol->getFlag() | 1 << LCIO::CHBIT_LONG );
    
    LCCollection *muonTcmtCol  = new LCCollectionVec(LCIO::CALORIMETERHIT);
    /*we want to save the position, and this can only be done if the CHBIT_LONG bit is set*/
    muonTcmtCol->setFlag(muonTcmtCol->getFlag() | 1 << LCIO::CHBIT_LONG );
    
    /*--------------------------------------------------------------------------*/
    CellIDDecoder<CalorimeterHit>* tcmtDecoder = 0;
    if (_useTCMT) 
      {
	tcmtDecoder = new CellIDDecoder<CalorimeterHit> (_tcmtInputCol);
      }
    float energyArr[MAXTCMTSTRIPS][MAXTCMTLAYERS] = {{0}, {0}};
    int   countArr[MAXTCMTSTRIPS][MAXTCMTLAYERS]  = {{0}, {0}};
    unsigned int cellID0Arr[MAXTCMTSTRIPS][MAXTCMTLAYERS] = {{0}, {0}};
    float xArr[MAXTCMTSTRIPS][MAXTCMTLAYERS] = {{0}, {0}};
    float yArr[MAXTCMTSTRIPS][MAXTCMTLAYERS] = {{0}, {0}};
    float zArr[MAXTCMTSTRIPS][MAXTCMTLAYERS] = {{0}, {0}};

    /*--------------------------------------------------------------------------*/
    unsigned int size = finalVec.size();
    for (unsigned int iCluster = 0; iCluster < size; iCluster++) 
      {
	Cluster *trackCluster = finalVec[iCluster];
	const CalorimeterHitVec hitVec = trackCluster->getCalorimeterHits();
	
	/* loop over the Hits in the cluster*/
	for (unsigned int iHit = 0; iHit < hitVec.size(); ++iHit) 
	  {
	    CalorimeterHit *Hit = hitVec[iHit];
	    /* Now we have getCellID0 and we need a Mapper*/
	    const int cellID0 = Hit->getCellID0();
	    const int cellID1 = Hit->getCellID1();
	    
	    CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
	    newHit->setEnergy(Hit->getEnergy());
	    newHit->setCellID0(cellID0);
	    newHit->setCellID1(cellID1);
	    newHit->setPosition(Hit->getPosition());
	    
	    /* check if the hit is coming from tcmt*/
	    if (cellID1 == 1 && _useTCMT == true) 
	      {
		// muonTcmtCol->addElement(newHit);	      
		const int I = (*tcmtDecoder)(Hit)["I"];
		const int J = (*tcmtDecoder)(Hit)["J"];
		const int K = (*tcmtDecoder)(Hit)["K-1"];
		const int layer = K;
		int strip = 0;
		if (I != 0)
		  strip = I - 1;
		if (J != 0)
		  strip = J - 1;
		streamlog_out(DEBUG0) << "Found a TCMT hit. Layer: " << layer << ", "
			      << " Strip: " << strip << endl;
		
		float energy = Hit->getEnergy();
		energyArr[strip][layer-1] = energy;
		countArr[strip][layer-1] += 1;
		
		cellID0Arr[strip][layer-1] = cellID0;
		xArr[strip][layer-1] = Hit->getPosition()[0];
		yArr[strip][layer-1] = Hit->getPosition()[1];
		zArr[strip][layer-1] = Hit->getPosition()[2];
		
	      }
	    else/*hit is from HCAL*/
	      {
		muonHcalCol->addElement(newHit);
	      }
	  }/*end loop over iHit*/
      }/*end loop over iCluster*/
    
    /*----------------------------------------------------------------
      HCAL muon hits*/    
    if (muonHcalCol->getNumberOfElements() > 0)
      {
 	LCParameters &param = muonHcalCol->parameters();
	param.setValue(LCIO::CellIDEncoding, _encodingString);

	evt->addCollection(muonHcalCol, _ahcalOutputTrackColName.c_str());

	streamlog_out(DEBUG)<<"\nFound "<<muonHcalCol->getNumberOfElements()<<" muon hits in HCAL, saved in col "
		    <<_ahcalOutputTrackColName<<" in event "<<evt->getEventNumber()
		    <<endl;

	/*for debug
	  CellIDDecoder<CalorimeterHit>* decoder = new CellIDDecoder<CalorimeterHit> (muonHcalCol);	
	  for (int i = 0; i < muonHcalCol->getNumberOfElements(); ++i)
	  {
	  CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(muonHcalCol->getElementAt(i));
	  const int I = (*decoder)(hit)["I"];
	  const int J = (*decoder)(hit)["J"];
	  const int K = (*decoder)(hit)["K-1"] + 1;
	  
	  cout<<" I/J/K="<<I<<"/"<<J<<"/"<<K<<endl;
	  }
	*/

      }
  

    /*----------------------------------------------------------------
      TCMT muon hits*/
    if (_useTCMT == false) return; 
    
     /*Now keep only the  TCMT hits which belong to a muon, i.e. for a given strip only if there are at least _minActiveTCMTParallelStrips  
      parallel strips active*/

    int minActiveParallelStrips  = _minActiveTCMTParallelStrips;
    
    int evenAr[MAXTCMTLAYERS/2] = {0, 2, 4, 6, 8, 10, 12, 14};
    int oddAr[MAXTCMTLAYERS/2]  = {1, 3, 5, 7, 9, 11, 13, 15};
    
    for (unsigned int iStrip = 0; iStrip < MAXTCMTSTRIPS; ++iStrip)
      {
	for (unsigned int iLayer = 0; iLayer < MAXTCMTLAYERS; ++iLayer)
	  {
	    int counts = 0;
	    
	    for (unsigned int i = 0; i < MAXTCMTLAYERS/2; ++i)
	      {
		unsigned int temp = 0;
		if (iLayer % 2 == 0) /*even layer*/
		  temp = evenAr[i];
		else
		  temp = oddAr[i];
		
		streamlog_out(DEBUG)<<" iStrip="<<iStrip<<" iLayer="<<iLayer<<" i="<<i
		    <<" temp="<<temp
		    <<" countArr[iStrip][temp]="<<countArr[iStrip][temp]
		    <<endl;
		
	      counts += countArr[iStrip][temp];
	      
	      }
	    
	    streamlog_out(DEBUG)<<"  counts="<<counts<<endl;
	    
	    if (counts >= minActiveParallelStrips && energyArr[iStrip][iLayer] > 0)
	      {
		streamlog_out(DEBUG)<<"------> found a muon hit in strip="<<iStrip<<" layer="<<iLayer<<endl;
		
		CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
		newHit->setEnergy(energyArr[iStrip][iLayer]);
		newHit->setCellID0(cellID0Arr[iStrip][iLayer]);
		float position[3] = {xArr[iStrip][iLayer], yArr[iStrip][iLayer], zArr[iStrip][iLayer]};
		newHit->setPosition(position);
		muonTcmtCol->addElement(newHit);	 
	      }
	    
	  }/*end loop over iLayer*/
      }/*end loop over iStrip*/
    
    /*------------------------------------------------------------------------------------*/
    
    if (muonTcmtCol->getNumberOfElements() > 0)
      {
 	LCParameters &param = muonTcmtCol->parameters();
	param.setValue(LCIO::CellIDEncoding, _encodingString);

	evt->addCollection(muonTcmtCol, _tcmtOutputTrackColName.c_str());

	streamlog_out(DEBUG)<<"\nFound "<<muonTcmtCol->getNumberOfElements()<<" muon hits in TCMT, saved in col "
		    << _tcmtOutputTrackColName
		    <<endl;
      }
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec AngleTrackFinder::obtainSubClusters(EVENT::Cluster *clusterA)
  {
    // Get the calorimeter Hits of the initial cluster
    EVENT::CalorimeterHitVec hitVecA = clusterA->getCalorimeterHits();
    
    std::sort(hitVecA.begin(), hitVecA.end(), ClusterOrdering::compareHitByLayer);
    const int numOfHitsInCluster = hitVecA.size();
 
    // first elemement
    const int cellIDA0 = hitVecA[0]->getCellID0();
    HcalCellIndex geomIndexA(cellIDA0);
    int iniVal = geomIndexA.getLayerIndex();
    
    int iNext;
    int iloop     = 1;
    int numOfGaps = 0;
        
    EVENT::ClusterVec allVecSubsample;
    ClusterImpl *aVecSubsample = new ClusterImpl;
    
    // Fill the first element!
    aVecSubsample->addHit(hitVecA[0], 1.);
      
    while(iloop < numOfHitsInCluster)
	{
	  const int cellIDA = hitVecA[iloop]->getCellID0();
	  HcalCellIndex geomIndex(cellIDA);
	  iNext = geomIndex.getLayerIndex();      
	  int deltaElement = iNext - iniVal;
	  
	  // allow gaps with maximum 2 elements missing
	  
	  if(deltaElement < 4)
	    {
	      aVecSubsample->addHit(hitVecA[iloop], 1.);	  
	    }
	  else
	    {	      
	      numOfGaps++;
	      allVecSubsample.push_back(aVecSubsample);
	      aVecSubsample = new ClusterImpl;
	      aVecSubsample->addHit(hitVecA[iloop], 1.);
	    }
	  
	  iniVal = iNext;
	  iloop++;   
	  
	}
      
      /*Fill the last track segment*/
      allVecSubsample.push_back(aVecSubsample);
      
      return allVecSubsample;
      
  }


  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec AngleTrackFinder::removeIsolatedHits(EVENT::ClusterVec subCluster)
  {
    EVENT::ClusterVec goodClusters;
    
    for (unsigned isubCl = 0; isubCl < subCluster.size(); isubCl++)
      {
	EVENT::CalorimeterHitVec hitVecA = subCluster[isubCl]->getCalorimeterHits();
	/* at least 2 hits in the subcluster*/
	if (hitVecA.size() > 1)
	  {
	    goodClusters.push_back(subCluster[isubCl]);
	  }
      }
    
    return goodClusters;
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  bool AngleTrackFinder::checkXYAdjacentClusters(Cluster *clusterA,Cluster *clusterB, int numOfAdjacentCells)
  {
    if (clusterA == NULL || clusterB == NULL) 
      {
	cout<<"\n checkXYAdyacentClusters: NULL pointers, exiting"<<endl;
	exit(1);
      }

    const EVENT::CalorimeterHitVec hitVecA = clusterA->getCalorimeterHits();
    const EVENT::CalorimeterHitVec hitVecB = clusterB->getCalorimeterHits();

    /* each cluster have same x,y*/
    float Delta_x = fabs(hitVecA[0]->getPosition()[0] - hitVecB[0]->getPosition()[0]);
    float Delta_y = fabs(hitVecA[0]->getPosition()[1] - hitVecB[0]->getPosition()[1]);
    
    float distanceInXY = (numOfAdjacentCells * 30) + 1;
    
    return (Delta_x < distanceInXY && Delta_y < distanceInXY);
  }
  







  /******************************************************************************/
}
