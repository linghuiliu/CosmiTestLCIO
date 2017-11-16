#include "DeepAnalysisProcessor.hh"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
//#include <IMPL/CalorimeterHitImpl.h>//contains lcio:CalorimeterHit
#include <IMPL/ClusterImpl.h>
#include <marlin/ConditionsProcessor.h>

#include <CellIndex.hh>
#include <TriggerBits.hh>
#include <collection_names.hh>
#include <RunInformation.hh>


//#define CLUSTERING_DEBUG


namespace CALICE {
  
  DeepAnalysisProcessor DeepAnalysisProcessor;
  
  /********************************************************************************/
  /*                                                                              */
  /* constructor                                                                  */
  /*                                                                              */
  /********************************************************************************/
  DeepAnalysisProcessor::DeepAnalysisProcessor(): Processor("DeepAnalysisProcessor"){
    _description = "This processor performs clustering a la Vassilly.";
    _inputCollectionNames.clear();
    _inputCollectionNames.push_back("CalorimeterHits"); //AhcCalorimeterHits
    registerProcessorParameter("HitCollectionName",
			       "Name of the Calorimeter hit collection",
			       _inputCollectionNames,
			       _inputCollectionNames);

    registerProcessorParameter("ClusterCollectionName", 
			       "Name of the Cluster collection",
			       _outputCollectionName,
			       std::string("AhcClusters"));

    registerProcessorParameter("NeutrClusterCollectionName", 
			       "Name of the neutral Cluster collection",
			       _outputNeutrCollectionName,
			       std::string("AhcNeutrClusters"));


    _colNamesModuleConnection.clear();
    _colNamesModuleConnection.push_back("ModuleConnection");
    registerProcessorParameter("ModuleConnectionCollectionName", 
			       "Name of the conditions data collection which describes the connection between modules and the DAQ front-ends (folder /CaliceEcal/module_connection)",
			       _colNamesModuleConnection,
			       _colNamesModuleConnection);

    _colNamesModuleLocation.clear();
    _colNamesModuleLocation.push_back("ModuleLocation");
    registerProcessorParameter("ModuleLocationCollectionName", 
			       "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_location)",
			       _colNamesModuleLocation,
			       _colNamesModuleLocation);

    _colNamesModuleDescription.clear();
    _colNamesModuleDescription.push_back("ModuleDescription");
    registerProcessorParameter("ModuleDescriptionCollectionName", 
			       "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_description)",
			       _colNamesModuleDescription,
			       _colNamesModuleDescription);

    registerProcessorParameter("ViewMapping", 
			       "View the mapping between channels and modules when ever the module location or module connection conditions data change (set to 0 or !=0)",
			       _viewConnectionTree,
			       0);

    _samplingFractionA.clear();
    _samplingFractionA.push_back(28.62);//29.5
    registerProcessorParameter("SamplingFractionA" ,
			       "first part of parametrisation of the sampling fraction." , 
			       _samplingFractionA,
			       _samplingFractionA);

    _samplingFractionB.clear();
    _samplingFractionB.push_back(0.);//0.78
    registerProcessorParameter("SamplingFractionB" ,
			       "second part of parametrisation of the sampling fraction." , 
			       _samplingFractionB, 
			       _samplingFractionB);
    
    _thresholds.clear();
    _thresholds.push_back(0.4);
    _thresholds.push_back(0.5);
    _thresholds.push_back(2.0);
    _thresholds.push_back(4.0);
    registerProcessorParameter("Thresholds",
			       "Thresholds to be put into deep analysis algorithm.",
			       _thresholds,
			       _thresholds);

    registerProcessorParameter("mipThreshold" ,
			       "Minimal energy deposition in units of MIP to keep hit (default 0.5)" , 
			       _mipThreshold,
			       (double) 0.5);

    registerProcessorParameter("switchRotation",
			       "Switch rotation (from Calice to Deep Analysis coordinates) ON (1-default) and OFF (0)",
			       _switchRotation,
			       (int) 1);

    registerProcessorParameter("kevFactor",
			       "Factor (in GeV) for transforming E[GeV] in E[MIP], default value 875 keV",
			       _samplingFactor,
			       (double) 0.000875);
  }

  /********************************************************************************/
  /*                                                                              */
  /* initialize                                                                   */
  /*                                                                              */
  /********************************************************************************/
  void DeepAnalysisProcessor::init() {

    std::stringstream message;
    message << "DeepAnalysisProcessor: undefined conditions data: ";
    bool error = false;
  
    if (_inputCollectionNames.size() != _colNamesModuleConnection.size()) {
      throw std::runtime_error("DeepAnalysisProcessor::init: For each hit collection name "
			       "mapping conditions data is required.");
    }		     
    if (_inputCollectionNames.size() != _colNamesModuleLocation.size()) {
      throw std::runtime_error("DeepAnalysisProcessor::init: For each hit collection name "
			       "module location conditions data is required.");
    }		       
    if (_inputCollectionNames.size() != _colNamesModuleDescription.size()) {
      throw std::runtime_error("DeepAnalysisProcessor::init: For each hit collection name "
			       "module description conditions data is required.");
    }
    if (_inputCollectionNames.size() != _samplingFractionA.size()) {
      throw std::runtime_error("DeepAnalysisProcessor::init: For each hit collection name "
			       "a first part of the sampling fraction paramterisation is required.");
    }
    if (_inputCollectionNames.size() != _samplingFractionB.size()) {
      throw std::runtime_error("DeepAnalysisProcessor::init: For each hit collection name "
			       "a second part of the sampling fraction parametrisation is required.");				
    }
  
    for (unsigned detectorIndex = 0; detectorIndex<_inputCollectionNames.size(); detectorIndex++) {
      _moduleTypeChange.push_back(MultipleConditionsChangeDelegator<DeepAnalysisProcessor>(this, &DeepAnalysisProcessor::moduleTypeChanged, detectorIndex));
      _moduleLocationChange.push_back(MultipleConditionsChangeDelegator<DeepAnalysisProcessor>(this, &DeepAnalysisProcessor::moduleLocationChanged, detectorIndex));
      _moduleConnectionChange.push_back(MultipleConditionsChangeDelegator<DeepAnalysisProcessor>(this, &DeepAnalysisProcessor::moduleConnectionChanged, detectorIndex));								   								   
    }
  
    for (unsigned detectorIndex = 0; detectorIndex<_inputCollectionNames.size(); detectorIndex++) {
      if (!marlin::ConditionsProcessor::registerChangeListener( &(_moduleLocationChange[detectorIndex])
								,_colNamesModuleLocation[detectorIndex])) {
	message << " " << _colNamesModuleLocation[detectorIndex];
	error=true;
      }
      if (!marlin::ConditionsProcessor::registerChangeListener( &(_moduleConnectionChange[detectorIndex]),
								_colNamesModuleConnection[detectorIndex])) {
	message << " " << _colNamesModuleConnection[detectorIndex];
	error=true;
      }
      if (!marlin::ConditionsProcessor::registerChangeListener( &(_moduleTypeChange[detectorIndex]),
								_colNamesModuleDescription[detectorIndex]) ) {
	message << " " << _colNamesModuleDescription[detectorIndex];
	error=true;
      }
    }  
    if (error) { 
      message <<  ".";
      throw std::runtime_error(message.str());
    }
    _mapping.resize(_inputCollectionNames.size());
    for (unsigned detectorIndex = 0; detectorIndex < _inputCollectionNames.size(); detectorIndex++) {
      _mapping[detectorIndex].init();
      _mapping[detectorIndex].setViewConnectionTree(_viewConnectionTree!=0);
    }
  
    if (_thresholds.size() != 4) {
      throw std::runtime_error("DeepAnalysisProcessor::init: Exactly four thresholds are required.");
    }

    printParameters();
  }

  /********************************************************************************/
  /*                                                                              */
  /* processRunHeader                                                             */
  /* (to be called once per Run)                                                  */
  /*                                                                              */
  /********************************************************************************/
  void DeepAnalysisProcessor::processRunHeader(LCRunHeader* run) 
  {
    RunInformation runInfo(run);
    _fIsMC = runInfo.isMC();

    streamlog_out(DEBUG0)<<"is MC: "<<runInfo.isMC()<<endl;


  }

  /********************************************************************************/
  /*                                                                              */
  /* processEvent                                                                 */
  /* (to be called in every event)                                                */
  /*                                                                              */
  /********************************************************************************/
  void DeepAnalysisProcessor::processEvent(LCEvent* evt) {
#ifdef CLUSTERING_DEBUG
    std::cout<<"\n\n\n runNumber="<<evt->getRunNumber()<<"  eventNumber="<<evt->getEventNumber()<<endl;
#endif

    DeepAnalysis deepAnalysis;

    //  Set thresholds for the reconstruction
    //            The trick here is: 
    //  Low _thresholds[0] will allow us to increase an efficiency 
    //  to MIP registration;  it was put as low as normal 0.5-0.1=0.4[MIPs]
    //  Neutron_thresh was increased by 0.1 =0.6[MIPs]
    //  to better kill noise in the neutron signal 
    deepAnalysis.detector.normal_thresh = _thresholds[1];    //0.5
    deepAnalysis.detector.neut_thresh   = _thresholds[1] + 0.1;//0.6


    try {
      std::vector<LCCollection*> col_hits; //contains input hits collections (for every detector: HCAL, ECAL etc)
      std::vector<unsigned> detector_hits; //contains the number of input hits collections
      std::vector<unsigned> start_hits;    //contains the number of hits per input collection per event

      //accept only beam events in case of DATA
      const TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));

      if (_fIsMC == false)/*DATA!*/
	if (!trigBits.isBeamTrigger()) return;

      //if (_fIsMC == false && !trigBits.isBeamTrigger()) return;

	unsigned hitIndex = 0;//counter for the total number of hits in a collection (per event)
	//(relevant only if there are more than 1 input collections)
	//--------------------------------------------------
	//loop over all input collections
	for (unsigned detectorIndex = 0; detectorIndex < _inputCollectionNames.size(); detectorIndex++) {
	  LCCollection* aHitCol = evt->getCollection(_inputCollectionNames[detectorIndex]); 
	  col_hits.push_back(aHitCol);
	  start_hits.push_back(hitIndex);
 
      
#ifdef CLUSTERING_DEBUG
	  std::cout << "filling hits for clustering" << std::endl;
	  std::cout << detectorIndex << " " << aHitCol->getNumberOfElements() << std::endl;
#endif      

	  //loop over all elements in the input collection
	  for (int element_i = 0; element_i < aHitCol->getNumberOfElements(); element_i++) {
	    CalorimeterHit *a_hit = dynamic_cast<CalorimeterHit *>(aHitCol->getElementAt(element_i));
      
	    //--------------------------------------------------
	    // Classify the hits according to their energy:
	    // E < _threshold[0]=0.4 -> disregard hit
	    //   ( _threshold[1]=0.5)
	    // E < _threshold[2]=2.0 -> TRK (track)
	    // E < _threshold[3]=4.0 -> HAD (hadronic)
	    // E > _threshold[3]=4.0 -> EM (electromagnetic)
	    //--------------------------------------------------
	    DeepAnalysis::KIND type = DeepAnalysis::KIND_COUNT;
	    if (a_hit->getEnergy() < _thresholds[0]) //0.4
	      continue;
	    else if (a_hit->getEnergy() < _thresholds[2])//2.0
	      type = DeepAnalysis::TRK;
	    else if (a_hit->getEnergy() < _thresholds[3])//2.0
	      type = DeepAnalysis::HAD;
	    else
	      type = DeepAnalysis::EM;

	    //------------------------------------------------------------  
	    // Note on the coordinate system:
	    // The coordinate system used in DeepAnalysis is different from
	    // the one used in Calice. The relations are:
	    // x_deepAnalysis = -x_calice
	    // y_deepAnalysis = y_calice
	    // z_deepAnalysis = -z_calice   	  

	    // double x_calice = a_hit->getPosition()[0];
	    // double y_calice = a_hit->getPosition()[1];
	    // double z_calice = a_hit->getPosition()[2];
	  
	    // double x_deepAnalysis = -x_calice;
	    // double y_deepAnalysis = y_calice;
	    // double z_deepAnalysis = -z_calice; 

	    //rotate to the Calice coordinates, if requested
	    std::vector<float> position = this->GetRotatedPositionIfRquested(_switchRotation, a_hit);

	    //------------------------------------------------------------
	    // Give the tagged hits to the DeepAnalysis to be 
	    // reconstructed:
	    // deepAnalysis.add_hit(x,y,z, ampl_gev, ampl_mip, layer, type)
	    // where: x,y,z    = the space coordinate of the hit
	    //        ampl_gev = the energy amplitude of one hit, in GeV
	    //        ampl_mip = the energy amplitude of one hit, in number of MIPs
	    //        type     = a pre-assigned hit type (according to the energy)
	    //        layer    = the layer in which the hitted cell is found
	    deepAnalysis.add_hit(position.at(0), //x
				 position.at(1), //y
				 position.at(2), //z
				 a_hit->getEnergy()*GeVperMip(detectorIndex, 
							      CellIndex(a_hit->getCellID0()).getLayerIndex()),
				 a_hit->getEnergy(),
				 _mipThreshold,
				 PopulatedLayerIndex(detectorIndex,CellIndex(a_hit->getCellID0()).getLayerIndex()),
				 type,
				 element_i);
	      
#ifdef CLUSTERING_DEBUG
	    cout<<"initial x="<<a_hit->getPosition()[0]<<"  y="<<a_hit->getPosition()[1]
		<<"  z="<<a_hit->getPosition()[2]<<endl;
	    cout<<"deepAnalysis.add_hit(x="<<position.at(0)<<", y="<<position.at(1)<<" , z="<<position.at(2)
		<<"  E(GeV)="
		<<a_hit->getEnergy()*GeVperMip(detectorIndex, CellIndex(a_hit->getCellID0()).getLayerIndex())
		<<"  E(MIPs)="<<a_hit->getEnergy()
		<<" type="<<type<<endl;
#endif
	      

	    //fill the vector detector_hits with the number of input collection of hits
	    detector_hits.push_back(detectorIndex);			      
	    hitIndex++;

	  }//end loop over element_i
	}//end loop over detector_index

	//-------------------------------------------------
	// call the main routine of the DeepAnalysis code,
	// i.e. start reconstruction         
	deepAnalysis.reconstruction();
	//-------------------------------------------------
	unsigned int had_ncl  = 0;
	unsigned int had_nhit = 0;
	double had_engySum    = 0;
	deepAnalysis.color_clusters_stat(DeepAnalysis::HAD,had_ncl,had_nhit,had_engySum);
#ifdef CLUSTERING_DEBUG
	cout<<"had_ncl="<<had_ncl<<" had_nhit="<<had_nhit<<" had_engySum="<<had_engySum<<endl;
#endif
	unsigned int elm_ncl  = 0;
	unsigned int elm_nhit = 0;
	double elm_engySum    = 0;
	deepAnalysis.color_clusters_stat(DeepAnalysis::EM,elm_ncl,elm_nhit,elm_engySum);
#ifdef CLUSTERING_DEBUG
	cout<<"elm_ncl="<<elm_ncl<<" elm_nhit="<<elm_nhit<<" elm_engySum="<<elm_engySum<<endl;
#endif

	unsigned int trk_ncl  = 0;
	unsigned int trk_nhit = 0;
	double trk_engySum    = 0;
	deepAnalysis.color_clusters_stat(DeepAnalysis::TRK,trk_ncl,trk_nhit,trk_engySum);
#ifdef CLUSTERING_DEBUG
	cout<<"trk_ncl="<<trk_ncl<<" trk_nhit="<<trk_nhit<<" trk_engySum="<<trk_engySum<<endl;
#endif



	LCCollection* col_cluster = new LCCollectionVec(LCIO::CLUSTER); 
	//----------------------------------------------
	//loop over reconstructed clusters (ELM-, TRK- and HAD-like)
	for (RSIterator<DeepAnalysis::Cluster> clusterIterator(deepAnalysis.clusters); 
	     clusterIterator.next();) {

	  ClusterImpl *a_cluster = new ClusterImpl;
	  a_cluster->setEnergy(clusterIterator->energySum);//ws=sum of energy
	  float pos_deepAnalysis[3];
	  pos_deepAnalysis[0] = clusterIterator->centerOfGravity.x;//s=center of gravity
	  pos_deepAnalysis[1] = clusterIterator->centerOfGravity.y;
	  pos_deepAnalysis[2] = clusterIterator->centerOfGravity.z;
	
	
	  //rotate back to the Calice system of coordinates
	  float pos_calice[3];
	  pos_calice[0] = -pos_deepAnalysis[0];
	  pos_calice[1] = pos_deepAnalysis[1];
	  pos_calice[2] = -pos_deepAnalysis[2];

	  if (_switchRotation == 1) {
	    a_cluster->setPosition(pos_calice);
	  } else if (_switchRotation == 0){
	    a_cluster->setPosition(pos_deepAnalysis);
	  }
	  float pos_err[3];
	  pos_err[0] = 0.;
	  pos_err[1] = 0.;
	  pos_err[2] = 0.;
	  a_cluster->setPositionError(pos_err);

	  if (clusterIterator->col==DeepAnalysis::EM)  a_cluster->setTypeBit(CLUSTER_EM);
	  if (clusterIterator->col==DeepAnalysis::TRK) a_cluster->setTypeBit(CLUSTER_TRK);
	  if (clusterIterator->col==DeepAnalysis::HAD) a_cluster->setTypeBit(CLUSTER_HAD);
	 
#ifdef CLUSTERING_DEBUG
	  std::cout << "found new cluster " << clusterIterator->energySum<< " " 
		    << clusterIterator->centerOfGravity.x << " "
		    << clusterIterator->centerOfGravity.y << " " 
		    << clusterIterator->centerOfGravity.z 
		    << std::endl;
#endif

	  //----------------------------------------------
	  //loop over hits
#ifdef CLUSTERING_DEBUG
	  int countIt = 0;
#endif
	  for (RSIterator<DeepAnalysis::Hit> hitIterator(&clusterIterator); hitIterator.next();) {
#ifdef CLUSTERING_DEBUG
	    std::cout << " hit: " << detector_hits[hitIterator->idx] << " " 
		      << start_hits[detector_hits[hitIterator->idx]] << " " << hitIterator->idx << std::endl;
#endif      
	    //a_cluster->addHit(static_cast<CalorimeterHit*>(col_hits[detector_hits[hitIterator->idx]]->getElementAt(hitIterator->idx - start_hits[detector_hits[hitIterator->idx]])), 1.);
	    int hitIdx  = hitIterator->idx; //hit index
	    int collIdx = detector_hits[hitIdx];//number of input hits collection (0=first, 1=second, etc)
	    CalorimeterHit *hit = 
	      static_cast<CalorimeterHit*>(col_hits[collIdx]->getElementAt(hitIdx - start_hits[collIdx]));
	    float contribution = 1.;
	    a_cluster->addHit(hit, contribution);
	    
#ifdef CLUSTERING_DEBUG
	    double engy = hit->getEnergy();
	    countIt++;
	    std::cout<<"hitIdx="<<hitIdx<<"  collIdx="<<collIdx<<" start_hits[collIdx]="<<start_hits[collIdx]
		     <<"  countIt="<<countIt<<"  ampl="<<engy<<std::endl;
#endif
	  }//end loop over RSIterator

#ifdef CLUSTERING_DEBUG
	    if (a_cluster->getType() >> CLUSTER_EM == 1) 
	      std::cout<<"EM noHits = "<<(a_cluster->getCalorimeterHits()).size()<<std::endl;
	    else if (a_cluster->getType() >> CLUSTER_TRK == 1) 
	      std::cout<<"TRK noHits = "<<(a_cluster->getCalorimeterHits()).size()<<std::endl;
	    else if (a_cluster->getType() >> CLUSTER_HAD == 1) 
	      std::cout<<"HAD noHits = "<<(a_cluster->getCalorimeterHits()).size()<<std::endl;
#endif
	  //----------------------------------------------
	  col_cluster->addElement(a_cluster);
	}//end loop over clusters

	evt->addCollection(col_cluster, _outputCollectionName);


	//============================================================================================
	//============================================================================================
	//============================================================================================
	LCCollectionVec *neutrHitsCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
	//Neutral clusters are just isolated hits
	int noNeutrHits=0;
	double neutrEngySum=0;
  	for (RSIterator<DeepAnalysis::Hit> isolHitIterator(deepAnalysis.neutrons); isolHitIterator.next(); ){
	  noNeutrHits++;
	  
 	  CalorimeterHitImpl *anIsolatedHit = new CalorimeterHitImpl;
 	  double engy = isolHitIterator->amplGeV;
 	  anIsolatedHit->setEnergy(engy);
	  
	  //===================================================
	  //FIXME: need to consider the case with more than 1 input collection?
	  //int isolHitIdx = isolHitIterator->idx;
	  //int collIdx    = detector_hits[isolHitIdx];
	  //===================================================
	  neutrHitsCol->addElement(anIsolatedHit);

	  neutrEngySum += engy;
   	}
#ifdef CLUSTERING_DEBUG
	std::cout<<"neutrHitsCol noOfElements: "<<neutrHitsCol->getNumberOfElements()
		 <<"  noNeutrHits="<<noNeutrHits<<"  neutrEngySum="<<neutrEngySum
		 <<std::endl;
#endif
	evt->addCollection(neutrHitsCol, _outputNeutrCollectionName);

	//============================================================================================
	//============================================================================================
	//============================================================================================

    }//end of try()
    catch (lcio::DataNotAvailableException err) {}
  }

  /********************************************************************************/
  /*                                                                              */
  /* GeVperMip                                                                    */
  /*                                                                              */
  /********************************************************************************/
  double DeepAnalysisProcessor::GeVperMip(unsigned detectorIndex, unsigned layerIndex) {
    return (_gevPerMip[detectorIndex])[layerIndex];
  }

  /********************************************************************************/
  /*                                                                              */
  /* PopulatedLayerIndex                                                          */
  /*                                                                              */
  /********************************************************************************/
  unsigned DeepAnalysisProcessor::PopulatedLayerIndex(unsigned detectorIndex, unsigned layerIndex) {
    return (_populatedLayerIndex[detectorIndex])[layerIndex];
  }

  /********************************************************************************/
  /*                                                                              */
  /* updateCorrelations                                                           */
  /*                                                                              */
  /********************************************************************************/
  void DeepAnalysisProcessor::updateCorrelations() {

    int populatedLayer = 0;
  
    std::vector<std::vector<unsigned> > layerOccupation;
    layerOccupation.resize(_inputCollectionNames.size());
    _gevPerMip.resize(_inputCollectionNames.size());
    _populatedLayerIndex.resize(_inputCollectionNames.size());

    for (unsigned detectorIndex = 0; detectorIndex < _inputCollectionNames.size(); detectorIndex++) {
      for (unsigned moduleIndex = 0; moduleIndex < _mapping[detectorIndex].getNModules(); moduleIndex++) {
	unsigned layerIndex = 
	  CellIndex(_mapping[detectorIndex].getGeometricalCellIndex(moduleIndex, 0)).getLayerIndex();

	if (layerIndex + 1 > layerOccupation[detectorIndex].size()) {
	  layerOccupation[detectorIndex].resize(layerIndex+1);
	}	
#ifdef HCALRECO_DEBUG
	std::cout << "DeepAnalysisProcessor::updateCorrelations: found layer " 
		  << layerIndex << " in detector " << detectorIndex << std::endl;
#endif
	(layerOccupation[detectorIndex])[layerIndex]++;
      }

      _populatedLayerIndex[detectorIndex].clear();
      _gevPerMip[detectorIndex].clear();
      for (unsigned layer = 0; layer < layerOccupation[detectorIndex].size(); layer++) {
	if (!(layerOccupation[detectorIndex])[layer]) {
	  _populatedLayerIndex[detectorIndex].push_back(-1);
	  _gevPerMip[detectorIndex].push_back(0.);
	}
	else {
	  _populatedLayerIndex[detectorIndex].push_back(populatedLayer);
	  int i;
	  for (i = layer-1; i >= 0; i--) {
	    if ((_populatedLayerIndex[detectorIndex])[i]>=0) break;
	  }
	  if (layer==0)
	    _gevPerMip[detectorIndex].push_back(_samplingFractionA[detectorIndex] * _samplingFactor);
	  else 
	    _gevPerMip[detectorIndex].push_back(_samplingFractionA[detectorIndex]*
						(1+_samplingFractionB[detectorIndex]*(layer-i-1))
						*_samplingFactor);     
	  populatedLayer++;
	}
      }
#ifdef HCALRECO_DEBUG
      std::cout << "DeepAnalysisProcessor::updateCorrelations: results for detector " 
		<< detectorIndex << " :" << std::endl;

      for (int layer = 0; layer < _populatedLayerIndex[detectorIndex].size(); layer++) {
	if ((_populatedLayerIndex[detectorIndex])[layer] >= 0) {
	  std::cout << " " << layer << ": " << (_populatedLayerIndex[detectorIndex])[layer] 
		    << "/" << (_gevPerMip[detectorIndex])[layer] << std::endl;
	}
      }
      std::cout << std::endl;
#endif      
    }  
  }

  /********************************************************************************/
  /*                                                                              */
  /* check                                                                        */
  /*                                                                              */
  /********************************************************************************/
  void DeepAnalysisProcessor::check(LCEvent* evt) {}
  
  /********************************************************************************/
  /*                                                                              */
  /* rotateCoordinates                                                            */
  /*                                                                              */
  /********************************************************************************/
  std::vector<float> DeepAnalysisProcessor::GetRotatedPositionIfRquested(bool switchRotation, 
									 CalorimeterHit* a_hit) 
  {    
    float x_calice = a_hit->getPosition()[0];
    float y_calice = a_hit->getPosition()[1];
    float z_calice = a_hit->getPosition()[2];
    
    float x_deepAnalysis = -x_calice;
    float y_deepAnalysis = y_calice;
    float z_deepAnalysis = -z_calice; 
    
    std::vector<float> position(3,0); //vector for storing x, y and z of the particle
    
    if (switchRotation == 1){
      position.at(0) = x_deepAnalysis;
      position.at(1) = y_deepAnalysis;
      position.at(2) = z_deepAnalysis;
    } else if (switchRotation == 0){
      position.at(0) = x_calice;
      position.at(1) = y_calice;
      position.at(2) = z_calice;
    }
    
    //double finalZ = position.at(2);
    //std::cout<<"initial z: "<<a_hit->getPosition()[2]<<" final z: "<<finalZ<<std::endl;
    return position;
  }
  
  /********************************************************************************/
  /*                                                                              */
  /* end                                                                          */
  /*                                                                              */
  /********************************************************************************/
  void DeepAnalysisProcessor::end() {}
  
}
