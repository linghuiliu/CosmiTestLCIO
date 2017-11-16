#include "angleTrackFinder.hh"

#include <math.h>

#include "marlin/Exceptions.h"
#include "UTIL/LCRelationNavigator.h"

#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"

#include "AhcAmplitude.hh"
#include "myClusterOrdering.hh"

using std::cout;
using std::endl;
using namespace lcio;
using namespace marlin;
using namespace UTIL;
using namespace EVENT;

namespace CALICE
{

  angleTrackFinder aangleTrackFinder;

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  angleTrackFinder::angleTrackFinder() : Processor("angleTrackFinder")
  {
    _description = "Processor to find track from a MIP";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides"
                                " the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "name of CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("MyCellDescriptionProcessor") ) ;

    registerProcessorParameter("AHCALInputCollectionName",
                               "Name of the collection of the AHCAL hits",
                               _ahcalInputColName,
                               std::string("AhcCalorimeter_Hits"));

    registerProcessorParameter("AmplitudeHCALInputCollectionName",
                               "Name of the AHCAL Amplitude Collection",
                               _ahcalAmpInputColName,
                               std::string("AhcHitAmplitudeRelation"));

    registerProcessorParameter("AHCALTrackOutputCollectionName",
                               "Name of the collection of the tracks found "
                               "in the AHCAL",
                               _ahcalOutputTrackColName,
                               std::string("AhcTrackClusters"));

    registerProcessorParameter("UseECALandTCMT", "Use ECAL and TCMT input hits",
                               _useECALandTCMT, bool(false));

    registerProcessorParameter("TCMInputCollectionName", "Name of the TCMT input collection",
                               _tcmtInputColName, std::string("TcmtCalorimeter_Hits"));

    registerProcessorParameter("ECALInputCollectionName", "Name of the ECAL input collection",
                               _ecalInputColName,
                               std::string("EmcCalorimeter_Hits"));

    registerProcessorParameter("IsNonMuonRun", "Is this a non-muon run?",
                               _isNonMuonRun, bool(false));

    registerProcessorParameter("MaxAHCALNoHits", "Maximum number of hits in the AHCAL, used for a non-muon run",
                               _ahcalMaxNoHits, int(100));

    registerProcessorParameter("MinECALNoHits", "Minimum number of hits in the ECAL",
                               _ecalMinNoHits, int(30));

    registerProcessorParameter("MaxECALNoHits", "Maximum number of hits in the ECAL",
                               _ecalMaxNoHits, int(34));

    registerProcessorParameter("MinTCMTEnergySum", "Minimum energy sum in the TCMT",
                               _tcmtMinEnergySum, float(10));

    registerProcessorParameter("useCoarseSection", "use or not coarse section",
                               _useCoarseSection, int(0));

    registerProcessorParameter("MipCutValue", "Minimum energy in MIPs",
                               _mipCutValue, float(0.5));

    registerProcessorParameter("MipCutVariable", "use (0) pure raw/MIP or (1) raw/MIP after whole calibration chain",
                               _mipCutVariable, int(1));
    IntVec badAhcalModulesVec;
    registerProcessorParameter("AHCALBadModules", "List of bad AHCAL modules which need to be excluded",
                               _badAhcalModulesVec, badAhcalModulesVec);

    IntVec firstAhcalModulesVec;
    registerProcessorParameter("AHCALFirstModules", "List of beginning AHCAL modules which are used"
                               " for perpendicular track finding",
                               _firstAhcalModulesVec, firstAhcalModulesVec);

    IntVec lastAhcalModulesVec;
    registerProcessorParameter("AHCALLastModules", "List of ending AHCAL modules which are used"
                               " for perpendicular track finding",
                               _lastAhcalModulesVec, lastAhcalModulesVec);

    registerProcessorParameter("TrackSelectionMode", "Track selection mode: "
                               "NHITS = based on the number of hits; "
                               "PERPENDICULAR = select only perpendicular tracks",
                               _trackSelectionMode,
                               std::string("NHITS"));

    registerProcessorParameter("TrackNoHitsThreshold","Threshold on the number of hits a track "
                               "is considered to be a muon track (used only with TrackSelectionMode=NHITS)",
                               _trackNoHitsThreshold, int(20));
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  angleTrackFinder::~angleTrackFinder()
  {}

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void angleTrackFinder::init()
  {
    printParameters();

    std::stringstream message;
    bool error = false;

    _mapper = dynamic_cast<const Mapper*>(MappingProcessor::getMapper(_mappingProcessorName));
    if ( !_mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName
                << ") did not return a valid mapper." << std::endl;
        error = true;
      }

    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions )
      {
        streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "
                     <<_cellDescriptionProcessorName<<". Maybe, processor is not present"
                     << std::endl;
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
  void angleTrackFinder::processEvent(LCEvent* evt)
  {

    streamlog_out(DEBUG4) << "\n\n============================= Processing event "<<evt->getEventNumber()<<endl;

    /* Get the incoming collections*/
    openInputCollections(evt);

    /* Start the track finding*/
    findTrack(evt);

    /* Clear vectors*/
    _allAhcalHitsInEventVec.clear();

  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void angleTrackFinder::openInputCollections(LCEvent* evt)
  {
    try
      {
        if(_useECALandTCMT == true)
          {
            streamlog_out(DEBUG4) << "Opening ECAL collection "<<_ecalInputColName
                          << "and TCMT collection " << _tcmtInputColName<< endl;

            _ecalInputCol = evt->getCollection(_ecalInputColName);
            _tcmtInputCol = evt->getCollection(_tcmtInputColName);
          }
	
	// Hcal collection
        _ahcalInputCol = evt->getCollection(_ahcalInputColName);
	// Amplitude relation
	if (_mipCutVariable==0){
	  _ahcalAmpInputCol = evt->getCollection(_ahcalAmpInputColName);
	}
	else{
	  //cout << "do not use relation! " << endl;
	}

        streamlog_out(DEBUG4) << "Opening AHCAL collection " << _ahcalInputColName
                      <<" with "<<_ahcalInputCol->getNumberOfElements() <<" hits"
                      << endl;

      }
    catch(DataNotAvailableException &e)
      {
        streamlog_out(WARNING) << "At least one required collection is missing:\n "
                       <<_ahcalInputColName<<", "<<_ecalInputColName
                       <<" or "<<_tcmtInputColName
                       <<" in the event "<<evt->getEventNumber()
                       << endl;

        throw marlin::SkipEventException(this);
      }

  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void angleTrackFinder::findTrack(LCEvent* evt)
  {
    streamlog_out(DEBUG4) << "\n\n ============ Start to find track in event " << evt->getEventNumber() << endl;
    streamlog_out(DEBUG4)<<" doFurtherAnalysis(): "<<endl;

    if(this->doFurtherAnalysis() == true )
      {

	//cout << "\n\n ============ Start to find track in event " << evt->getEventNumber() << endl;

	//cout << "-------------- Evt Number: " << evt->getEventNumber() << endl;
        /* All hits above 0.5 MIP are filled into the vector hits.*/
        LCTypedVector<CalorimeterHit> hcalHitsVec = this->extractAHCALInformation();

	//cout << "Number of hits above mip cut: " << hcalHitsVec.size() << endl;

        /* All hits above 0.5 MIP which are recognized as a track are
           in this vector of ClusterVec -fine section-*/

        EVENT::ClusterVec trackClusterVec = this->findTrackClusters(hcalHitsVec);

	/* All hits above 0.5 MIP which are recognized as a track are
           in this vector of ClusterVec -coarse section-*/
	//cout << "Number of cluster in the fine section: " << trackClusterVec.size() << endl;

	EVENT::ClusterVec trackClusterVecCoarse;
	if (_useCoarseSection==1){
	  trackClusterVecCoarse = this->findTrackClustersCoarse(hcalHitsVec);
	  //cout << "Number of tracks in coarse section: " << trackClusterVecCoarse.size() << endl;
	}
	
	//cout << "Number of cluster in the fine section: " << trackClusterVecCoarse.size() << endl;	
        /* This vector holds the selected tracks */

        EVENT::ClusterVec selectedClusterVec = this->selectTrackClusters(trackClusterVec,_trackSelectionMode);

	EVENT::ClusterVec selectedFineCoarseVec;
	if (_useCoarseSection==1){
	  selectedFineCoarseVec = this->mergeCoarseClusters(selectedClusterVec,trackClusterVecCoarse,_trackSelectionMode);
	}

	EVENT::ClusterVec outputTrackSegments;
	if (_useCoarseSection==1){
	  outputTrackSegments = this->getRemainingHitsOfTrackSegment(selectedFineCoarseVec);
	}
	else{
	  outputTrackSegments = this->getRemainingHitsOfTrackSegment(selectedClusterVec);
	}

	//cout << "Preparig the output " << endl;

	EVENT::ClusterVec outputTrackClusterVec = this->getFinalCluster(outputTrackSegments);

        streamlog_out(DEBUG4) << outputTrackClusterVec.size() << " found tracks" << endl;

	//cout << "Create the collection " << endl;

        /* Attach the final tracks to the event.*/
        createOutputCollection(evt, outputTrackClusterVec);

	//cout << "Quite in the end!" << endl;

      }
    else
      {
        streamlog_out(DEBUG0) << "Criteria for further analysis not reached." << endl;
        /* Do nothing.*/
      }

  }



  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  bool angleTrackFinder::doFurtherAnalysis()
  {
    /* here apply criteria to use the number of hits in HCAL to  apply the muon*/
    bool analyse = false;

    if(_useECALandTCMT == true)
      {
        streamlog_out(DEBUG4) << "Using ECAL and TCMT information" << endl;

        /* ECAL information*/
        std::pair<int, float> nhits_and_esum(extractNumberOfHitsAndEnergySum(_ecalInputCol));
        int ecalNumberOfHits = nhits_and_esum.first;

        /* TCMT information*/
        nhits_and_esum = extractNumberOfHitsAndEnergySum(_tcmtInputCol);
        float tcmtEnergySum = nhits_and_esum.second;

        if((ecalNumberOfHits > _ecalMinNoHits) && (ecalNumberOfHits < _ecalMaxNoHits)
           && (tcmtEnergySum > _tcmtMinEnergySum))
          {
            analyse = true;
            streamlog_out(DEBUG4) << "ECAL and TCMT criteria reached. "
                                  << "Going to do further processing." << endl;
          }
        else
          {
            analyse = false;
            streamlog_out(DEBUG4) << "ECAL and TCMT criteria *not* reached. "
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

            if (nhits_hcal < _ahcalMaxNoHits)
              {
                analyse = true;
              }
            else
              {
                analyse = false;
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
  std::pair<int, float> angleTrackFinder::extractNumberOfHitsAndEnergySum(LCCollection *inputCol)
  {
    float energySum = 0;
    const int numberOfHits = inputCol->getNumberOfElements();

    for (int i = 0; i < numberOfHits; ++i)
      {
        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputCol->getElementAt(i));
        energySum += hit->getEnergy();
      }/*end loop over input collection*/

    return std::pair<int, float>(numberOfHits, energySum);
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  LCTypedVector<CalorimeterHit> angleTrackFinder::extractAHCALInformation()
  {
    streamlog_out(DEBUG4)<<"\n extractAHCALInformation:"<<endl;
    /*check if there is a list of bad modules*/
    bool haveListOfBadModules = parameterSet("AHCALBadModules");

    streamlog_out(DEBUG4)<<"  haveListOfBadModules: "<<haveListOfBadModules<<endl;

    LCTypedVector<CalorimeterHit> hcalVec(_ahcalInputCol);
    UTIL::LCRelationNavigator *navigator = NULL;

    if (_mipCutVariable==0){      
      if (_ahcalAmpInputCol != NULL)
	{
	  navigator = new UTIL::LCRelationNavigator(_ahcalAmpInputCol);
	}
      else { 
	std::cout<<" no relation collection, aborting"<<endl;
	exit(1);
      }
    }
	
     _allAhcalHitsInEventVec = hcalVec;



    /*counter for debug purposes*/
    int counter = 0;

    for (unsigned int i = 0; i < hcalVec.size(); ++i)
      {
        const int cellID = hcalVec[i]->getCellID0();
        const int module = _mapper->getModuleFromCellID(cellID);

        bool hitIsInBadModule = false;

        if (haveListOfBadModules == true) {
          if (std::find(_badAhcalModulesVec.begin(), _badAhcalModulesVec.end(), module) != _badAhcalModulesVec.end())
            hitIsInBadModule = true;

          if ( hitIsInBadModule )
	    {
	      streamlog_out(DEBUG4)<<" -> "<<counter<<" hit in module "<< module << "-> on list of bad modules" <<endl;
	    }
          else
	    {
	      streamlog_out(DEBUG4)<<" -> "<<counter<<" hit in module "<< module << "-> NOT on list of bad modules" <<endl;
	    }
        }


	// get the amplitude without temperature correction
	float ampMipNoTempNoSat;
	ampMipNoTempNoSat=0.;

	if (_mipCutVariable==0){
	  const LCObjectVec &amplVec = navigator->getRelatedToObjects(hcalVec[i]);
	  	 
	  if (amplVec.size() > 0)
	    {
	      LCGenericObject *obj = dynamic_cast<LCGenericObject*>(amplVec[0]);
	      AhcAmplitude *ahcAmpl = new AhcAmplitude(obj);
	      ampMipNoTempNoSat=ahcAmpl->getAmplNOTTemperatureCorrMIP();
	      delete ahcAmpl;
	    }
	}

	// get the amplitude after temperature correction & saturated corrected

	float ampMipFinal;
	ampMipFinal=hcalVec[i]->getEnergy();

	// choose amplitude to be used
	
	float mipCut;
	mipCut=0;
	if (_mipCutVariable==1){
	  mipCut=ampMipFinal;
	}
	else if (_mipCutVariable==0){
	  mipCut=ampMipNoTempNoSat;
	}

	// Now do the selection(!)

	//cout << "mipValue" <<  mipCut << endl;

        if (
            (haveListOfBadModules == true && hitIsInBadModule == true)
            || (mipCut < _mipCutValue) )
          {
            counter++;
            hcalVec.erase(hcalVec.begin() + i); /*remove this hit from the vector*/
            i--; /* go back one step not to miss the next hit*/
          }

      }

    streamlog_out(DEBUG4)<<" -> "<<counter<<" hits do not fullfill MIP cut and/or are in bad modules"<<endl;
    streamlog_out(DEBUG4)<<" -> selected AHCAL hits: "<<hcalVec.size()<<endl;

    delete navigator;

    return hcalVec;
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec angleTrackFinder::findTrackClusters(LCTypedVector<CalorimeterHit> hcalVec)
  {
    streamlog_out(DEBUG4)<<"\n--------------- Start to find track clusters"<<endl;

    EVENT::ClusterVec clusterVec;

    LCTypedVector<CalorimeterHit>::iterator seedIter = hcalVec.begin();
    while(seedIter != hcalVec.end())
      {
        CalorimeterHit *seedHit = (*seedIter);
        const int cellID = seedHit->getCellID0();
        /*erase seed hit*/
        hcalVec.erase(seedIter);

	/*use only hits in fine modules to find tracks*/
	const int K = _mapper->getDecoder()->getKFromCellID(cellID);
	// <=
	if (K <= 30)
	  {
            ClusterImpl *cluster = new ClusterImpl;
            /*first member of the cluster is the seed hit*/
            cluster->addHit(seedHit, 1.);
	    
            LCTypedVector<CalorimeterHit>::iterator potentialTrackHitIter = hcalVec.begin();
            while(potentialTrackHitIter != hcalVec.end())
              {
                float differenceInX = fabs((*potentialTrackHitIter)->getPosition()[0] - seedHit->getPosition()[0]);
                float differenceInY = fabs((*potentialTrackHitIter)->getPosition()[1] - seedHit->getPosition()[1]);

                CellDescription* seedHitCellDescription = _cellDescriptions->getByCellID(cellID);
                const float seedHitTileSize = seedHitCellDescription->getSizeX();
                const float potentialTrackHitTileSize =
                  _cellDescriptions->getByCellID((*potentialTrackHitIter)->getCellID0())->getSizeX();

                float meanCellSize  = 0.5 * (potentialTrackHitTileSize + seedHitTileSize);

                if (differenceInX < meanCellSize && differenceInY < meanCellSize)
                  {
                    streamlog_out(DEBUG4)<<" New track hit found "<<endl;

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

    return clusterVec;
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  
  EVENT::ClusterVec angleTrackFinder::findTrackClustersCoarse(LCTypedVector<CalorimeterHit> hcalVec)
  {
    streamlog_out(DEBUG4)<<"\n--------------- Start to find track clusters"<<endl;
    
    EVENT::ClusterVec clusterCoarseVec;
    
    LCTypedVector<CalorimeterHit>::iterator seedIter = hcalVec.begin();
    while(seedIter != hcalVec.end())
      {
        CalorimeterHit *seedHit = (*seedIter);
        const int cellID = seedHit->getCellID0();
        /*erase seed hit*/
        hcalVec.erase(seedIter);

	/*use only hits in the coarse modules to find the last track segment of the track*/
	const int K = _mapper->getDecoder()->getKFromCellID(cellID);
	if (K>30)
	  {
            ClusterImpl *cluster = new ClusterImpl;
            /*first member of the cluster is the seed hit*/
            cluster->addHit(seedHit, 1.);

            LCTypedVector<CalorimeterHit>::iterator potentialTrackHitIter = hcalVec.begin();
            while(potentialTrackHitIter != hcalVec.end())
              {
                float differenceInX = fabs((*potentialTrackHitIter)->getPosition()[0] - seedHit->getPosition()[0]);
                float differenceInY = fabs((*potentialTrackHitIter)->getPosition()[1] - seedHit->getPosition()[1]);

                CellDescription* seedHitCellDescription = _cellDescriptions->getByCellID(cellID);
                const float seedHitTileSize = seedHitCellDescription->getSizeX();
                const float potentialTrackHitTileSize =
                  _cellDescriptions->getByCellID((*potentialTrackHitIter)->getCellID0())->getSizeX();

                float meanCellSize  = 0.5 * (potentialTrackHitTileSize + seedHitTileSize);

		// do not take again the layers in fine section
		const int cellIDP = (*potentialTrackHitIter)->getCellID0();
		HcalCellIndex geomIndexP(cellIDP);
		int layernum = geomIndexP.getLayerIndex(); 

                if (differenceInX < meanCellSize && differenceInY < meanCellSize && layernum>30)
                  {
                    streamlog_out(DEBUG4)<<" New track hit found "<<endl;

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

    return clusterCoarseVec;
  }


  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec angleTrackFinder::selectTrackClusters(EVENT::ClusterVec clusterVec,
                                                     const std::string trackSelectionMode)
  {

    if(trackSelectionMode == "ANGLE"){

      //order the track segments by size in decreasing order

      std::sort(clusterVec.begin(), clusterVec.end(),myClusterOrdering::compareVectBySize);
      //cout << "Number of towers: " << clusterVec.size() << endl;

      // here will be store the good track segments
      EVENT::ClusterVec goodClusters;

      // check the first track segment
      CalorimeterHitVec firsthitVec = clusterVec[0]->getCalorimeterHits();

      // use track segments with a minimun number of Hits
      if (firsthitVec.size() > (unsigned int)_trackNoHitsThreshold){

	//cout << "First track with length: " << firsthitVec.size() << endl;
 	  
	EVENT::ClusterVec subCluster=myClusterOrdering::obtainSubClusters(clusterVec[0]);
	// But remove first isolated hits
	EVENT::ClusterVec subClusterClean=myClusterOrdering::removeIsolatedHits(subCluster);
	// Add
	for (unsigned isubCl=0;isubCl<subClusterClean.size();isubCl++){
	  goodClusters.push_back(subClusterClean[isubCl]);
	}	

	//cout << "Go now to the next towers " << endl;

	// Now check if the next Clusters are close to the seed track segment
	unsigned iSearch;
	iSearch=1;

	while (iSearch < clusterVec.size()){

	  CalorimeterHitVec hitVec = clusterVec[iSearch]->getCalorimeterHits();
	  //

	  // and at least they contain 3 hits!
	  if (hitVec.size() > 2){
	    // order by layer
	    //cout << "next tower: " << iSearch << " having # of hits: " << hitVec.size() << endl;
	    bool tobeMerged = false;
	    tobeMerged=myClusterOrdering::checkXYAdyacentClusters(clusterVec[0],clusterVec[iSearch],3);
	    if (tobeMerged){
	      EVENT::ClusterVec nextsubCluster=myClusterOrdering::obtainSubClusters(clusterVec[iSearch]);
	      EVENT::ClusterVec nextsubClusterClean=myClusterOrdering::removeIsolatedHits(nextsubCluster);
	      for (unsigned isubCl=0;isubCl<nextsubClusterClean.size();isubCl++){
		goodClusters.push_back(nextsubClusterClean[isubCl]);
	      }
	    }	 
	  }
	  iSearch++;
	}	      	
      }

      // print the output of goodClusters!
      /*
      for (unsigned icl=0;icl<goodClusters.size();icl++){
	  cout << "yyyyyyy---- " << icl << endl;
	  CalorimeterHitVec hitVecCl =goodClusters[icl] ->getCalorimeterHits();	  
	  for (int il=0;il<hitVecCl.size();il++){
	    const int cellIDA = hitVecCl[il]->getCellID0();
	    HcalCellIndex geomIndex(cellIDA);	  
	    int iLayer = geomIndex.getLayerIndex(); 
	    cout << "---- good clusters having layers: " << iLayer << " x/y " << hitVecCl[il]->getPosition()[0] << " / " << hitVecCl[il]->getPosition()[1] << " il: " << il << endl;
	  }
	}
	  */
      return goodClusters;
    }
    else
      {
        streamlog_out(ERROR)<<"No TrackSelectionMode chosen, aborting..."<<endl;
        exit(1);
      }
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  EVENT::ClusterVec angleTrackFinder::mergeCoarseClusters(EVENT::ClusterVec goodClusterVec, EVENT::ClusterVec clusterCoarseVec, const std::string trackSelectionMode)
  {

    if(trackSelectionMode == "ANGLE"){
  
       EVENT::ClusterVec mergeFineCoarseClusters;

       if (goodClusterVec.size()>0){

	 //cout << "Number of clusters in the fine Section " << goodClusterVec.size() << endl; 
	 // get X,Y of the last cluster in the fine section
	 float xlastFine;
	 float ylastFine;
	 float zlastFine;
	 
	 xlastFine=0;
	 ylastFine=0;
	 
	 for (unsigned iloop=0;iloop<goodClusterVec.size();iloop++){
	   mergeFineCoarseClusters.push_back(goodClusterVec[iloop]);
	   CalorimeterHitVec finehitVec = goodClusterVec[iloop]->getCalorimeterHits();
	   // the cluster are arranged in decreasing order
	   if (iloop==0){
	     xlastFine=finehitVec[0]->getPosition()[0];
	     ylastFine=finehitVec[0]->getPosition()[1];
	     zlastFine=finehitVec[0]->getPosition()[2];
	   }
	 }

	 //cout << "Before Number of SubClusters: " << mergeFineCoarseClusters.size() << endl;

	 // display some coarse quantities

	 std::sort(clusterCoarseVec.begin(), clusterCoarseVec.end(),myClusterOrdering::compareVectBySize);      
	 for (unsigned iloop=0;iloop<clusterCoarseVec.size();iloop++){
	   CalorimeterHitVec coarsehitVec = clusterCoarseVec[iloop]->getCalorimeterHits();
	   // order it by layer
	   std::sort(coarsehitVec.begin(), coarsehitVec.end(),myClusterOrdering::compareHitByLayer);
	   //check the towers which at least have 3 hits in the coarse section
	   //cout << "iloop:-----------" << iloop << endl;
	   //cout << "lenght of coarse track segment: " << coarsehitVec.size() << endl;
	   if (coarsehitVec.size()>=3){
	     // check all the coarse subclusters since x-y is the same, 
	     // only use the first hit
	     float dXfine_coarse=fabs(coarsehitVec[0]->getPosition()[0]-xlastFine);
	     float dYfine_coarse=fabs(coarsehitVec[0]->getPosition()[1]-ylastFine);
	     
	     if (dXfine_coarse<61&&dYfine_coarse<61){
	       //cout << "Filling? at: " << iloop << endl;
	       // order by layer
	       
	       mergeFineCoarseClusters.push_back(clusterCoarseVec[iloop]);
	     }	  
	   }
	 }
	 //cout << "After Number of SubClusters: " << mergeFineCoarseClusters.size() << endl;
       }
       else{
	 //cout << "no track found" << endl;
       }

       // print out
       /*
       for (unsigned icl=0;icl<mergeFineCoarseClusters.size();icl++){
	 cout << "at the end of fine-coarse section ---- " << icl << endl;
	 CalorimeterHitVec hitVecCl =mergeFineCoarseClusters[icl] ->getCalorimeterHits();	  
	 for (int il=0;il<hitVecCl.size();il++){
	   const int cellIDA = hitVecCl[il]->getCellID0();
	   HcalCellIndex geomIndex(cellIDA);	  
	   int iLayer = geomIndex.getLayerIndex(); 
	   cout << "xxxx having layers: " << iLayer << " x/y " << hitVecCl[il]->getPosition()[0] << " / " << hitVecCl[il]->getPosition()[1] << " il: " << il << " energy " << hitVecCl[il]->getEnergy() << endl;
	 }
       }
	   */


       return mergeFineCoarseClusters;
    }
    else{
      streamlog_out(ERROR)<<"No TrackSelectionMode chosen, aborting..."<<endl;
      exit(1);
    }
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  ClusterImpl* angleTrackFinder::getWholeTrack(Cluster *trackCluster)
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

    streamlog_out(DEBUG4)<<" Mean X: "<<meanXOfTrack<<" mean Y: "<<meanYOfTrack<<endl;

    ClusterImpl *wholeTrack = new ClusterImpl();

    EVENT::CalorimeterHitVec::iterator iter;
    for (iter = _allAhcalHitsInEventVec.begin(); iter != _allAhcalHitsInEventVec.end(); ++iter)
      {
        float differenceInX = fabs((*iter)->getPosition()[0] - meanXOfTrack);
        float differenceInY = fabs((*iter)->getPosition()[1] - meanYOfTrack);

        streamlog_out(DEBUG4)<<" differenceInX: "<<differenceInX<<" differenceInY: "<<differenceInY<<endl;

        /*be aware of the misalignment information in the future*/
        if (differenceInX < 16 && differenceInY < 16)
          {
            streamlog_out(DEBUG0)<<" Added hit to the track"<<endl;
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
  ClusterImpl* angleTrackFinder::copyACluster(Cluster *trackCluster)
  {
    ClusterImpl *wholeTrack = new ClusterImpl();
    CalorimeterHitVec hitVec = trackCluster->getCalorimeterHits();

    //cout << "Elements:: " << hitVec.size() << endl;
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
  EVENT::ClusterVec angleTrackFinder::getRemainingHits(EVENT::ClusterVec selectedTrackClusters)
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
  EVENT::ClusterVec angleTrackFinder::getRemainingHitsOfTrackSegment(EVENT::ClusterVec aselectedTrackClusters)
  {
    EVENT::ClusterVec outputClusterVec;

    for (unsigned int iCluster = 0; iCluster < aselectedTrackClusters.size(); ++iCluster)
      {
	//cout << "getRemainingHitsOfTrackSegment iCluster: " << iCluster << endl;
        outputClusterVec.push_back(this->getWholeTrackSegment(aselectedTrackClusters[iCluster]));
      }/*--------------------- end loop over track clusters -----------------*/

    return outputClusterVec;
  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  ClusterImpl* angleTrackFinder::getWholeTrackSegment(Cluster *trackCluster)
  {
    float meanXOfTrack = 0;
    float meanYOfTrack = 0;

    int iniLayerSegment;
    int endLayerSegment;

    CalorimeterHitVec hitVec = trackCluster->getCalorimeterHits();
    // order by Layer!
    std::sort(hitVec.begin(), hitVec.end(),myClusterOrdering::compareHitByLayer);

    // find initial and end;

    const int cellIDAini = hitVec[0]->getCellID0();
    HcalCellIndex geomIndexIni(cellIDAini);	  
    iniLayerSegment = geomIndexIni.getLayerIndex(); 

    const int cellIDAend = hitVec[hitVec.size()-1]->getCellID0();
    HcalCellIndex geomIndexEnd(cellIDAend);	  
    endLayerSegment = geomIndexEnd.getLayerIndex(); 

    //cout << "getWholeTrackSegment ======= " << iniLayerSegment << "/t" << endLayerSegment << endl;

    for (unsigned int iHits = 0; iHits < hitVec.size(); ++iHits)
      {
        meanXOfTrack += hitVec[iHits]->getPosition()[0];
        meanYOfTrack += hitVec[iHits]->getPosition()[1];
      }/*------------ end loop over track clusters -----------------*/

    meanXOfTrack /= hitVec.size();
    meanYOfTrack /= hitVec.size();

    streamlog_out(DEBUG4)<<" Mean X: "<<meanXOfTrack<<" mean Y: "<<meanYOfTrack<<endl;

    ClusterImpl *wholeTrack = new ClusterImpl();

    EVENT::CalorimeterHitVec::iterator iter;
    for (iter = _allAhcalHitsInEventVec.begin(); iter != _allAhcalHitsInEventVec.end(); ++iter)
      {
        float differenceInX = fabs((*iter)->getPosition()[0] - meanXOfTrack);
        float differenceInY = fabs((*iter)->getPosition()[1] - meanYOfTrack);
        streamlog_out(DEBUG4)<<" differenceInX: "<<differenceInX<<" differenceInY: "<<differenceInY<<endl;
	
        /*be aware of the misalignment information in the future*/

	if (differenceInX < 16 && differenceInY < 16){
	  const int cellIDA = (*iter)->getCellID0();
	  HcalCellIndex geomIndex(cellIDA);	  
	  int iLayer = geomIndex.getLayerIndex(); 
	  //cout << "layer: " << iLayer << "\t" << iniLayerSegment << "\t" << endLayerSegment << endl; 
	  //cout <<" differenceInX: "<<differenceInX<<" differenceInY: "<<differenceInY<<endl;
	  if (iLayer>=iniLayerSegment&&iLayer<=endLayerSegment){
	    //cout << "coarse difference: " << differenceInX << "\t" << differenceInY << " layer " <<iLayer << " energy " << (*iter)->getEnergy() << endl;
	    streamlog_out(DEBUG0)<<" Added hit to the track"<<endl;
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
  EVENT::ClusterVec angleTrackFinder::getFinalCluster(EVENT::ClusterVec selectedTrackClusters)
  {
    EVENT::ClusterVec outputClusterVec;

    //cout << "Number of subcluster: " << selectedTrackClusters.size() << endl;

    for (unsigned int iCluster = 0; iCluster < selectedTrackClusters.size(); ++iCluster)
      {
	//outputClusterVec.push_back(selectedTrackClusters[iCluster]);
        outputClusterVec.push_back(this->copyACluster(selectedTrackClusters[iCluster]));
      }/*--------------------- end loop over track clusters -----------------*/

    return outputClusterVec;
  } 

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void angleTrackFinder::createOutputCollection(LCEvent *evt, EVENT::ClusterVec finalVec)
  {
    LCCollection *outputCol = new LCCollectionVec(LCIO::CLUSTER);
    /*Need to set the flag; for more details, see:
      http://forum.linearcollider.org/index.php?t=msg&th=323&rid=0&S=857c8d1096e8bdfbb539fd560de6b468
    */
    outputCol->setFlag(outputCol->getFlag() | (1 << LCIO::CLBIT_HITS));

    for (unsigned int iCluster = 0; iCluster < finalVec.size(); ++iCluster)
      {
        outputCol->addElement(finalVec[iCluster]);
      }
    evt->addCollection(outputCol, _ahcalOutputTrackColName);

   cout <<"\n Found "<<outputCol->getNumberOfElements()<<" track segments"<<endl;
    streamlog_out(DEBUG4)<<"\n Found "<<outputCol->getNumberOfElements()<<" tracks."<<endl;
  }

}
