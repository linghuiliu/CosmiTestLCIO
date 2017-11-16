#include "MipTrackFinder.hh"

#include <cmath>
#include <cstdlib>
#include <algorithm>

#include "marlin/Exceptions.h"
#include "UTIL/LCRelationNavigator.h"

#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"

#include "AhcAmplitude.hh"

using std::cout;
using std::endl;
using namespace lcio;
using namespace marlin;
using namespace UTIL;

namespace CALICE
{

  MipTrackFinder aMipTrackFinder;

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  MipTrackFinder::MipTrackFinder() : Processor("MipTrackFinder")
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

    registerProcessorParameter("MipCutValue", "Minimum energy in MIPs",
                               _mipCutValue, float(0.5));

    registerProcessorParameter("MipCutVariable", "use (0) pure raw/MIP or (1) raw/MIP after whole calibration chain",
                               _mipCutVariable, int(0));
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
  MipTrackFinder::~MipTrackFinder()
  {}

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void MipTrackFinder::init()
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
  void MipTrackFinder::processEvent(LCEvent* evt)
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
  void MipTrackFinder::openInputCollections(LCEvent* evt)
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
        _ahcalAmpInputCol = evt->getCollection(_ahcalAmpInputColName);

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
  void MipTrackFinder::findTrack(LCEvent* evt)
  {
    streamlog_out(DEBUG4) << "\n\n ============ Start to find track in event " << evt->getEventNumber() << endl;
    streamlog_out(DEBUG4)<<" doFurtherAnalysis(): "<<endl;

    if(this->doFurtherAnalysis() == true )
      {
        /* All hits above 0.5 MIP are filled into the vector hits.*/
        LCTypedVector<CalorimeterHit> hcalHitsVec = this->extractAHCALInformation();

        /* All hits above 0.5 MIP which are recognized as a track are
           in this vector of ClusterVec*/
        EVENT::ClusterVec trackClusterVec = this->findTrackClusters(hcalHitsVec);

        /* This vector holds the selected tracks */
        EVENT::ClusterVec selectedClusterVec = this->selectTrackClusters(trackClusterVec, _trackSelectionMode);

        /* This vector has also the remaining hits*/
        EVENT::ClusterVec outputTrackClusterVec = this->getRemainingHits(selectedClusterVec);

        streamlog_out(DEBUG4) << outputTrackClusterVec.size() << " found tracks" << endl;

        /* append event parameters with track information */
        appendParameters(evt, outputTrackClusterVec);

        /* Attach the final tracks to the event.*/
        createOutputCollection(evt, outputTrackClusterVec);
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
  bool MipTrackFinder::doFurtherAnalysis()
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
  std::pair<int, float> MipTrackFinder::extractNumberOfHitsAndEnergySum(LCCollection *inputCol)
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
  LCTypedVector<CalorimeterHit> MipTrackFinder::extractAHCALInformation()
  {
    streamlog_out(DEBUG4)<<"\n extractAHCALInformation:"<<endl;
    /*check if there is a list of bad modules*/
    bool haveListOfBadModules = parameterSet("AHCALBadModules");

    streamlog_out(DEBUG4)<<"  haveListOfBadModules: "<<haveListOfBadModules<<endl;

    LCTypedVector<CalorimeterHit> hcalVec(_ahcalInputCol);


    UTIL::LCRelationNavigator *navigator = NULL;
    if (_ahcalAmpInputCol != NULL)
      {
        navigator = new UTIL::LCRelationNavigator(_ahcalAmpInputCol);
      }
    else {
      std::cout<<" no relation collection, aborting"<<endl;
      exit(1);
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
          if (std::find(_badAhcalModulesVec.begin(), _badAhcalModulesVec.end(), module) != _badAhcalModulesVec.end()){
            hitIsInBadModule = true;
          }

          if ( hitIsInBadModule ){
            streamlog_out(DEBUG4)<<" -> "<<counter<<" hit in module "<< module << "-> on list of bad modules" <<endl;
          }
          else{
            streamlog_out(DEBUG4)<<" -> "<<counter<<" hit in module "<< module << "-> NOT on list of bad modules" <<endl;
          }
        }


        // get the amplitude without temperature correction

        const LCObjectVec &amplVec = navigator->getRelatedToObjects(hcalVec[i]);

        float ampMipNoTempNoSat;
        ampMipNoTempNoSat=0.;
        if (amplVec.size() > 0)
          {
            LCGenericObject *obj = dynamic_cast<LCGenericObject*>(amplVec[0]);
            AhcAmplitude *ahcAmpl = new AhcAmplitude(obj);
            ampMipNoTempNoSat=ahcAmpl->getAmplNOTTemperatureCorrMIP();
            delete ahcAmpl;
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
  EVENT::ClusterVec MipTrackFinder::findTrackClusters(LCTypedVector<CalorimeterHit> hcalVec)
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
  EVENT::ClusterVec MipTrackFinder::selectTrackClusters(EVENT::ClusterVec clusterVec,
                                                        const std::string trackSelectionMode)
  {
    if (trackSelectionMode == "PERPENDICULAR")
      {
        if (_firstAhcalModulesVec.size() > 0 && _lastAhcalModulesVec.size() > 0)
          {
            EVENT::ClusterVec goodClusters;

            for (unsigned int iCluster = 0; iCluster < clusterVec.size(); ++iCluster)
              {
                int ngoodin  = 0;
                int ngoodout = 0;

                CalorimeterHitVec hitVec = clusterVec[iCluster]->getCalorimeterHits();
                for (unsigned int iHits = 0; iHits < hitVec.size(); ++iHits)
                  {
                    const int cellID = hitVec[iHits]->getCellID0();
                    const int module = _mapper->getModuleFromCellID(cellID);

                    LCIntVec::iterator searchIfFirstModule = find(_firstAhcalModulesVec.begin(),
                                                                  _firstAhcalModulesVec.end(),
                                                                  module);
                    LCIntVec::iterator searchIfLastModule = find(_lastAhcalModulesVec.begin(),
                                                                 _lastAhcalModulesVec.end(),
                                                                 module);

                    if (searchIfFirstModule != _firstAhcalModulesVec.end()) ngoodin++;
                    if (searchIfLastModule  != _lastAhcalModulesVec.end())  ngoodout++;
                  }/*-------- end loop over track hits ---------------------*/

                if ((ngoodin > 1) && (ngoodout > 1))
                  goodClusters.push_back(clusterVec[iCluster]);
		else delete clusterVec[iCluster];

              }/*------------ end loop over track clusters -----------------*/

            return goodClusters;
          }
        else
          {
            streamlog_out(ERROR)<<"PERPENDICULAR tracks chosen, but no first/last AHCAL modules list given"<<endl;
            exit(1);
          }
      }/*-------------------------- end of PERPENDICUALR mode --------------------------------------------*/

    else if (trackSelectionMode == "NHITS")
      {
        EVENT::ClusterVec goodClusters;
        for (unsigned int iCluster = 0; iCluster < clusterVec.size(); ++iCluster)
          {
            CalorimeterHitVec hitVec = clusterVec[iCluster]->getCalorimeterHits();
            if (hitVec.size() > (unsigned int)_trackNoHitsThreshold)
              {
                streamlog_out(DEBUG4)<<"The found track reaches the threshold of "<< _trackNoHitsThreshold
                             <<" hits and has "<<hitVec.size()<<endl;
                goodClusters.push_back(clusterVec[iCluster]);
              }
            else
              {
		delete clusterVec[iCluster];
                streamlog_out(DEBUG4)<<"The found track does NOT reach the threshold of "<< _trackNoHitsThreshold
                             <<" hits and has "<<hitVec.size()<<" hits"<<endl;
              }
          }/*----------------------- end loop over track clusters ----------------------------------------*/

        return goodClusters;
      }/*--------------------------- end of NHITS mode ---------------------------------------------------*/
    /*---------------------------- if no TrackSelectionMode ----------------------------------------------*/
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
  ClusterImpl* MipTrackFinder::getWholeTrack(Cluster *trackCluster)
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

    delete trackCluster;

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
  EVENT::ClusterVec MipTrackFinder::getRemainingHits(EVENT::ClusterVec selectedTrackClusters)
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
  void MipTrackFinder::appendParameters(LCEvent *evt, EVENT::ClusterVec finalVec)
  {

    int nTracks = 0; // number of tracks found in event
    int maxTrack_hits = 0; // maximum track length in event

    // get number of found tracks
    nTracks = (int)finalVec.size();

    // loop over all found tracks
    for (int iCluster = 0; iCluster < nTracks; ++iCluster) {

      if ( (int)((finalVec[iCluster])->getCalorimeterHits()).size() > maxTrack_hits ) {
        maxTrack_hits = (int)((finalVec[iCluster])->getCalorimeterHits()).size();
      }

    }

    /* append event parameters */
    evt->parameters().setValue(name()+"_nTracks", nTracks);
    evt->parameters().setValue(name()+"_maxTrack_hits", maxTrack_hits);

  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void MipTrackFinder::createOutputCollection(LCEvent *evt, EVENT::ClusterVec finalVec)
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

    streamlog_out(DEBUG4)<<"\n Found "<<outputCol->getNumberOfElements()<<" tracks."<<endl;
  }

}
