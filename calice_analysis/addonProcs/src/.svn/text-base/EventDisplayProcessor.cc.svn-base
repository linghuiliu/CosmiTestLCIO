#include "EventDisplayProcessor.hh"
#include <iostream>
#include <iomanip>
/* ----- include for verbosity dependend logging ---------*/
#include "marlin/VerbosityLevels.h"

#include "marlin/Exceptions.h"

/* LCIO includes*/
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/Cluster.h"
#include "UTIL/CellIDDecoder.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCCollectionVec.h"

/* CED includes*/
#include "ced_cli.h"

/* CALICE includes*/
#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "CellIterator.hh"
#include "ClusterShapesTyped.hh"
#include "CellIndex.hh"

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE {

  EventDisplayProcessor aEventDisplayProcessor;



  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  EventDisplayProcessor::EventDisplayProcessor() : Processor("EventDisplayProcessor"),
                                                   _mapper(0), _cellDescriptions(0)
  {
    /* modify processor description*/
    _description = "processor for displaying CALICE events in CED";

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "Collection_SiWEcalCalorimeterHits" ,
                             "Name of the Si-W ECAL CalorimeterHit collection"  ,
                             _colNameEmc ,
                             std::string("") ) ;

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "Collection_AhcalCalorimeterHits" ,
                             "Name of the AHCAL CalorimeterHit collection"  ,
                             _colNameAhc ,
                             std::string("") ) ;

    registerInputCollection( LCIO::SIMCALORIMETERHIT,
                             "Collection_AhcalSimCalorimeterHits" ,
                             "Name of the Ture MC AHCAL SimCalorimeterHit collection"  ,
                             _colNameAhcSim ,
                             std::string("") ) ;

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "Collection_TcmtCalorimeterHits" ,
                             "Name of the TCMT CalorimeterHit collection"  ,
                             _colNameTcm ,
                             std::string("") ) ;

    registerProcessorParameter( "Collection_MCParticle" ,
                                "Name of the MC particle collection" ,
                                _colNameMCParticle,
                                std::string("") ) ;

    registerProcessorParameter( "Parameter_ShowerStartLayer" ,
                                "name of event parameter with AHCAL shower start layer",
                                _parNameShowerStartLayer,
                                std::string("") ) ;

    registerProcessorParameter( "Parameter_ShowerStartPosition" ,
                                "name of event parameter with shower start position (x,y,z)",
                                _parNameShowerStartPos,
                                std::string("") ) ;

    registerProcessorParameter( "ProcessorName_Mapping" ,
                                "name of Ahcal MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("AhcalMappingProcessor") ) ;

    registerProcessorParameter( "ProcessorName_CellNeighbours" ,
                                "name of Ahcal CellNeighboursProcessor which takes care of the cell neighbours calculation",
                                _cellNeighboursProcessorName,
                                std::string("AhcalCellNeighboursProcessor") ) ;

    registerProcessorParameter( "ProcessorName_CellDescription" ,
                                "name of Ahcal CellDescriptionProcessor which takes care of the cell description generation",
                                _cellDescriptionProcessorName,
                                std::string("AhcalCellDescriptionProcessor") ) ;


    registerProcessorParameter( "CED_WaitForKeyPressed" ,
                                "0 disables to wait for key at end of event" ,
                                _waitForKeyPressed,
                                int(1) ) ;

    registerProcessorParameter( "CED_Draw" ,
                                "0 disables CED drawing" ,
                                _draw,
                                int(1) ) ;

    registerProcessorParameter( "CED_AppendToExistingCED" ,
                                "1 skips opening a new CED connection and appends output to existing CED event" ,
                                _appendToExistingCED,
                                int(0) ) ;

    registerProcessorParameter("ShowerStartCollectionName",
			       "Name of input shower start collection",
			       _showerStartColName,
			       std::string("ShowerStartingLayer"));
        
    registerProcessorParameter( "DrawHCALHistos" ,
                                "1 enables the drawing of the HCAL histos event by event" ,
                                _drawHcalHistos,
                                bool(false) ) ;

    registerProcessorParameter( "TcmtStartVertical" ,
                                "The TCMT starts with vertical strips" ,
                                _tcmtStartVertical,
                                bool(false) ) ;

    registerProcessorParameter("MokkaModelName",
			       "Name of the Mokka model (needed to calculate distance between HCAL and TCMT)",
			       _fMokkaModelName,
			       std::string("TBhcalcatch08_02"));

    registerProcessorParameter( "ShowerShapeColNames" ,
                                "String containing a list of collections to be used",
                                _showerShapeColNames,
                                std::string(""));

 
    registerProcessorParameter( "TracksColName" ,
                                "Name of track collection (LCCluster)",
                                _trackColName,
                                std::string("AhcTracksNN"));

    }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::init() 
  {
    streamlog_out(DEBUG) << "     EventDisplayProcessor::init()  " << std::endl ;

    /* usually a good idea to*/
    printParameters() ;

    if (_drawHcalHistos) _theHcalHistosDrawer = new HcalHistosDrawer();

    bool error = false;

    _mapper = dynamic_cast<const CALICE::AhcMapper*> ( CALICE::MappingProcessor::getMapper(_mappingProcessorName) );
    if ( ! _mapper) {
      streamlog_out(ERROR) << "Cannot obtain AhcMapper from MappingProcessor "<<_mappingProcessorName
		   <<". Mapper not present or wrong type." << std::endl;
      error = true;
    }
    _mapperVersion = _mapper->getVersion();

    _cellNeighbours = CALICE::CellNeighboursProcessor::getNeighbours(_cellNeighboursProcessorName);
    if ( ! _cellNeighbours ) {
      streamlog_out(ERROR) << "Cannot obtain cell neighbours from CellNeighboursProcessor "<<_cellNeighboursProcessorName
		   <<". Maybe, processor is not present" << std::endl;
      error = true;
    }

    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions ) {
      streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "<<_cellDescriptionProcessorName
		   <<". Maybe, processor is not present" << std::endl;
      error = true;
    }

    if (error) throw StopProcessingException(this);

    _nRun = 0 ;
    _nEvt = 0 ;

    /*CED part*/
    if (_draw && !_appendToExistingCED) 
      {
	ced_client_init("localhost",7286);
	ced_register_elements();
      }
    

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::processRunHeader( LCRunHeader* run) {

    _nRun++ ;
  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawCell(const CALICE::CellDescription* description,
				       const unsigned int color, const unsigned int layer, 
				       const bool solid) const 
  {
    if (_draw) {
      double cellSize[3]  = {description->getSizeX(),description->getSizeY(),5};
      double cellPos[3]   = {description->getX(),description->getY(),description->getZ()};
      double cellAngle[3] = {0.,description->getAngle(),0.};
      if (solid) ced_geobox_r_solid(cellSize,cellPos,cellAngle,color,layer << CED_LAYER_SHIFT);
      else ced_geobox_r(cellSize,cellPos,cellAngle,color,layer << CED_LAYER_SHIFT);
    }
  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawHit(const CalorimeterHit* hit,const unsigned int color, const unsigned int layer) const 
  {
    if (_draw) ced_hit(hit->getPosition()[0],hit->getPosition()[1],hit->getPosition()[2],layer << CED_LAYER_SHIFT,2,color);
  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::chooseColorAndLayer(const float energy, unsigned int& color, unsigned int& layer) const
  {
    if (energy < 0.5) {
      color = 0x444444 ;
    }
    else if ( (energy >= 0.5) && (energy < 1.65) ) {
      color = 0x00aa00 ;
      layer += 1;
    }
    else if ( (energy >= 1.65) && (energy < 2.9) ) {
      color = 0xbbbb00;
      layer += 2;
    }
    else if ( (energy >= 2.9) && (energy < 5.4) ) {
      color = 0xdd8800;
      layer += 3;
    }
    else if ( energy >= 5.4 ) {
      color = 0xaa0000 ;
      layer += 4;
    }

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawSiWECAL(LCEvent *evt) 
  {

    try {

      LCCollection* col = evt->getCollection( _colNameEmc );

      LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

      for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

        unsigned int color=0;
        unsigned int layer=1; // layer = 6 -> shift layers by 5, e.g.

        chooseColorAndLayer((*iter)->getEnergy(),color,layer);

        double x = (*iter)->getPosition()[0];
        double y = (*iter)->getPosition()[1];
        double z = (*iter)->getPosition()[2];

        double cellPos[3]  = {x, y, z};
        double cellSize[3] = {10., 10., 1.};
        double cellAngle[3] = {0., 0., 0.};

        if (_draw) {
          ced_geobox_r_solid(cellSize, cellPos, cellAngle, color, layer << CED_LAYER_SHIFT);
          ced_geobox_r(cellSize, cellPos, cellAngle,0x3333ee , 20 << CED_LAYER_SHIFT);
        }

      }

    } catch ( EVENT::DataNotAvailableException &err) {
      streamlog_out(WARNING) << "Si-W ECAL collection " << _colNameEmc << " not available" << std::endl;
    }

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawTCMT(LCEvent *evt) 
  {

    try {

      LCCollection* col = evt->getCollection( _colNameTcm );
      CellIDDecoder<CalorimeterHit> myCellIDDecoder(col);

      // get decoder for correct K values
      CALICE::FastDecoder * decoder_k = CALICE::FastDecoder::generateDecoder( col->getParameters().getStringVal("CellIDEncoding"), "K");

      LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

      for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

        unsigned int k_pos = decoder_k->decodeU( (*iter)->getCellID0() );

	char orient = ' ';
	if (_tcmtStartVertical) orient = ( k_pos %2 ? 'H' :'V' );
	else                    orient = ( k_pos %2 ? 'V' :'H' );

	const int I = myCellIDDecoder(*iter)["I"];
	const int J = myCellIDDecoder(*iter)["J"];
	const int K = myCellIDDecoder(*iter)["K-1"];

        unsigned int color=0;
        unsigned int layer=1; // layer = 6 -> shift layers by 5, e.g.

        chooseColorAndLayer((*iter)->getEnergy(),color,layer);

        float x = (*iter)->getPosition()[0];
        float y = (*iter)->getPosition()[1];
        float z = (*iter)->getPosition()[2];

	streamlog_out(DEBUG)<<" TCMT: I/J/K="<<I<<"/"<<J<<"/"<<K<<" orient="<<orient<<" k_pos="<<k_pos
		    <<", x="<<x<<" y="<<y<<" z="<<z
		    <<" energy="<<(*iter)->getEnergy()
		    <<std::endl;

        CALICE::CellDescription* thisTCMTcell = new CALICE::CellDescription();
        thisTCMTcell->setAngle(0.);
        thisTCMTcell->setPosition(x,y,z);

        if ( orient == 'H' ) {
          thisTCMTcell->setSize(1000.,50.);
        }
        else {
          thisTCMTcell->setSize(50.,1000.);
        }
        drawCell(thisTCMTcell,color,layer);
        drawCell(thisTCMTcell,0x3333ee,20,false);
      }

      streamlog_out(DEBUG)<<"\n TCMT hits: "<<col->getNumberOfElements()<<std::endl;

    } catch ( EVENT::DataNotAvailableException &err) {
      streamlog_out(WARNING) << "TCMT collection " << _colNameTcm << " not available" << std::endl;
    }

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawAHCAL(LCEvent *evt) 
  {
    try {
      LCCollection* col = evt->getCollection( _colNameAhc );

      LCTypedVector<CalorimeterHit> calorimterHitsCol(col);

      // make sure to use the right encoding for the collection just fetched
      _cellDescriptions->getDecoder()->setCellIDEncoding( col->getParameters().getStringVal("CellIDEncoding") );

      float esum = 0;
      float nhits = 0;

      for (LCTypedVector<CalorimeterHit>::iterator iter = calorimterHitsCol.begin();iter != calorimterHitsCol.end();++iter) {

        int cellID = (*iter)->getCellID0();
        float energy = (*iter)->getEnergy();
	if (energy < 0.5) continue;
        CALICE::CellDescription* cellDescription = _cellDescriptions->getByCellID(cellID);

	esum += energy;
	nhits++;
	
	

	streamlog_out(DEBUG)<<setiosflags(ios::right)<< setiosflags(ios::fixed) << setprecision(2)
		    <<"HCAL: x="<< setw(8)<<(*iter)->getPosition()[0]
		    <<", y="<<setiosflags(ios::right)<<setw(8)<<(*iter)->getPosition()[1]
		    <<", z="<<setiosflags(ios::right)<<setw(8)<<(*iter)->getPosition()[2]
		    <<", I/J/K="<<setiosflags(ios::right)<<setw(8)<<_mapper->getDecoder()->getIFromCellID(cellID)
		    <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
		    <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
		    <<" energy="<<setiosflags(ios::right)<<setw(6)<<energy
		    <<std::endl;
	  

        unsigned int color=0;
        unsigned int layer=1;

        chooseColorAndLayer(energy,color,layer);

        drawCell(cellDescription,color,layer);
        drawCell(cellDescription,0x3333ee,20,false);

      }

      streamlog_out(DEBUG)<<"\n energy sum: "<<esum<<", "<<nhits<<" hits"<<std::endl;
    }
    catch ( EVENT::DataNotAvailableException &err) {
      streamlog_out(WARNING) << "AHCAL collection " << _colNameAhc << " not available" << std::endl;
    }
  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawAHCALSim(LCEvent *evt) 
  {
    try {
      LCCollection* col = evt->getCollection( _colNameAhcSim );
      CellIDDecoder<SimCalorimeterHit> myCellIDDecoder(col);

      LCTypedVector<SimCalorimeterHit> simCalorimterHitsCol(col);

      float esum = 0;
      float nhits = 0;

      for (LCTypedVector<SimCalorimeterHit>::iterator iter = simCalorimterHitsCol.begin();iter != simCalorimterHitsCol.end();++iter) {

        float energy = (*iter)->getEnergy(); //return the total energy from all contribution.

	esum += energy;
	nhits++;

	const int I = myCellIDDecoder(*iter)["I"];
	const int J = myCellIDDecoder(*iter)["J"];
	const int K = myCellIDDecoder(*iter)["K"];

        unsigned int color=0;
        unsigned int layer=1;

        chooseColorAndLayer( (energy/0.000846),color,layer);//convert GeV to MIP for color coding. 

        float x = (*iter)->getPosition()[0];
        float y = (*iter)->getPosition()[1];
        float z = (*iter)->getPosition()[2];

	streamlog_out(DEBUG)<<" Ture MC AhcCell: I/J/K="<<I<<"/"<<J<<"/"<<K
		    <<", x="<<x<<" y="<<y<<" z="<<z
		    <<" energy="<<energy
		    <<std::endl;

        CALICE::CellDescription* thisAhcalMCcell = new CALICE::CellDescription();
        thisAhcalMCcell->setAngle(0.);
        thisAhcalMCcell->setPosition(x,y,z);
	thisAhcalMCcell->setSize(10.,10.);

        drawCell(thisAhcalMCcell,color,layer);
        drawCell(thisAhcalMCcell,0x3333ee,20,false);
      }
      

      streamlog_out(DEBUG)<<"\n energy sum: "<<esum<<", "<<nhits<<" hits"<<std::endl;
    }
    catch ( EVENT::DataNotAvailableException &err) {
      streamlog_out(WARNING) << "The True MC AHCAL collection " << _colNameAhc << " not available" << std::endl;
    }
  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  /** @todo count layers from 1 to 38 or from 0 to 37??
   */
  void EventDisplayProcessor::drawStartLayer(LCEvent *evt) 
  {
    int  startLayer = 0;
    if( marlin::Processor::parameters()->isParameterSet("Parameter_ShowerStartLayer") )
      startLayer = evt->getParameters().getIntVal(_parNameShowerStartLayer);
    else if (marlin::Processor::parameters()->isParameterSet("ShowerStartCollectionName") )
       startLayer = this->getMarinaShowerStartHcalLayer(evt);

    if (startLayer < 1 ) 
      {
	streamlog_out(DEBUG)<<" Could not find shower start layer, return "<<endl;
	return;
      }
    if (startLayer > 38 ) startLayer = 38;
    streamlog_out(DEBUG) << "start layer " << startLayer << endl;

    CALICE::CellDescription *middleCell = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(45,45,startLayer)));

    double layerSize[3] = {900.,900.,0.};
    double layerPos[3];
    layerPos[0] = middleCell->getX() + 0.5*middleCell->getSizeX();
    layerPos[1] = middleCell->getY() + 0.5*middleCell->getSizeY();
    layerPos[2] = middleCell->getZ();

    double layerRotation[3] = {0.,middleCell->getAngle(),0.};

    if (_draw) ced_geobox_r_solid(layerSize,layerPos,layerRotation,0xaa00aa,12 << CED_LAYER_SHIFT);

  }
  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawAHCALTracks(LCEvent *evt) 
  {
    try
      {
	LCCollection* lccTracks = evt->getCollection(_trackColName);
	int nTracks = lccTracks->getNumberOfElements();
	streamlog_out(DEBUG)<<"\n drawAHCALTracks ntracks="<<nTracks<<endl;

	int ced_layer = 10;
	int color = 0x000080; // navy

	for (int iTrack = 0; iTrack < lccTracks->getNumberOfElements(); ++iTrack)
	  {
	    if (iTrack == 1) color = 0xffa500; //orange
	    if (iTrack == 2) color = 0x800080; //purple
	    if (iTrack == 3) color = 0x2f4f4f; //darkslategrey
	    if (iTrack > 3)  color = 0x000000; //black
	    
	    Cluster* lcTrack = dynamic_cast<lcio::Cluster*>(lccTracks->getElementAt(iTrack));
	    CalorimeterHitVec hits = lcTrack->getCalorimeterHits();
	    
	    int NHits = hits.size();
	    streamlog_out(DEBUG)<<"\n  nhits="<<NHits<<endl;
	    
	    for (int iHit = 0; iHit < NHits; ++iHit)
	      {
		CalorimeterHit* lcHit = hits.at(iHit);
		
		int cellID = lcHit->getCellID0();
		CALICE::CellIndex idx(lcHit->getCellID0());
		int I = idx.getPadColumn();
		int J = idx.getPadRow();
		int K = idx.getLayerIndex();

		streamlog_out(DEBUG)<<" I/J/K="<<I<<"/"<<J<<"/"<<K<<endl;

		CALICE::CellDescription *description = _cellDescriptions->getByCellID(cellID);
		double cellSize[3]  = {description->getSizeX(),description->getSizeY(),5};
		double cellPos[3]   = {description->getX(),description->getY(),description->getZ()};
		double cellAngle[3] = {0.,description->getAngle(),0.};

		ced_geobox_r(cellSize,cellPos,cellAngle,color, ced_layer << CED_LAYER_SHIFT);
	      }
	  }/*end loop over iTrack*/
      }
    catch (DataNotAvailableException& e)
      {
	streamlog_out(WARNING) << "No cluster with name " << _trackColName << " found" << endl;
      }
   
  }
  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawShowerShapes(LCEvent *evt) 
  {
    /* collect clusters: loop over all input col candidates
     */
    std::vector<ClusterShapesTyped*> allClusters;

    StringVec v_colNames;
    evt->getParameters().getStringVals( _showerShapeColNames , v_colNames );

    streamlog_out(DEBUG)<<"\n drawShowerShapes: Start loop over "<<v_colNames.size()<<" collections named "<<_showerShapeColNames<<std::endl;

    for ( unsigned i = 0; i < v_colNames.size(); i++ )
      {
	try 
	  {
	    LCCollection* incol_i = evt->getCollection( v_colNames[i] );

	    streamlog_out(DEBUG)<<"\n Collection "<<v_colNames[i]<<endl;

	    /*
	      for (int iHit = 0; iHit < incol_i->getNumberOfElements(); ++iHit)
	      {
	      CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(incol_i->getElementAt(i));
	      const float *position = hit->getPosition();
	      float energy   = hit->getEnergy();
	      
	      streamlog_out(DEBUG)<<" hit "<<iHit+1<<" x="<<position[0]<<", y="<<position[1]<<", z="<<position[2]
	      <<", energy="<<energy<<endl;
	      }
	    */


	    ClusterShapesTyped* shape_i = new ClusterShapesTyped;
	    shape_i->fill<CalorimeterHit>(incol_i);	    
	    
	    allClusters.push_back(shape_i);
	  
	  }
	catch ( DataNotAvailableException &err) 
	  {
	    streamlog_out(WARNING) << "collection " << v_colNames[i] << " not available" << std::endl;
	  }
      }/*end loop over input shower shapes collection*/
    
     /* calculate ellipsoid parameters */
    unsigned int nClusters = allClusters.size();
    
    float ene_min = 0.0;
    float ene_max = 100.0;
  
    for ( unsigned iCluster = 0; iCluster <  nClusters; iCluster++ ) 
      {
	float *pos_cog = (allClusters[iCluster])->getClusterShapesPointer()->getCentreOfGravity();
	float clusterEnergy = (allClusters[iCluster])->getClusterShapesPointer()->getTotalAmplitude();

	/*the elipsoid length are only calculated if the inertia vector is calculated*/
	(allClusters[iCluster])->getClusterShapesPointer()->getEigenVecInertia();
	float smallest_axis_length = (allClusters[iCluster])->getClusterShapesPointer()->getElipsoid_r3();
	float medium_axis_length   = (allClusters[iCluster])->getClusterShapesPointer()->getElipsoid_r2();
	float largest_axis_length  = (allClusters[iCluster])->getClusterShapesPointer()->getElipsoid_r1();

	streamlog_out(DEBUG)<<"\n cluster "<<iCluster+1<<" pos_cog=("<<pos_cog[0]<<","<<pos_cog[1]<<","<<pos_cog[2]<<")"<<endl;
	streamlog_out(DEBUG)<<"  nhits               ="<<(allClusters[iCluster])->getClusterShapesPointer()->getNumberOfHits()<<endl;
	streamlog_out(DEBUG)<<"  smallest_axis_length="<<smallest_axis_length<<endl;
	streamlog_out(DEBUG)<<"  medium_axis_length  ="<<medium_axis_length<<endl;
	streamlog_out(DEBUG)<<"  largest_axis_length ="<<largest_axis_length<<endl;

	if (smallest_axis_length == 0 || medium_axis_length == 0) 
	  {
	    streamlog_out(DEBUG)<<"\n Zero length, do not draw this cluster shape"<<endl;
	    continue;
	  }
	
	//double sizes_eli[3]    = {smallest_axis_length, medium_axis_length, largest_axis_length};
	//double center_eli[3]   = {pos_cog[0], pos_cog[1], pos_cog[2]};
	double rotation_eli[3] = {0, 0, 0};
	// 	ced_ellipsoid_r(sizes_eli, center_eli,  rotation_eli, 1, 0x99999999);


	double sizes[] = {100.0, 100.0, 100.0};
	int scale_z = 4;
	sizes[0] = returnClusterSize(clusterEnergy, ene_min, ene_max);
	sizes[1] = sizes[0];
	sizes[2] = scale_z*returnClusterSize(clusterEnergy, ene_min, ene_max);
	//ced_ellipsoid_r(sizes, center_eli,  rotation_eli, 1, 0x99999999);

	float center_eli_r[3]   = {pos_cog[0], pos_cog[1], pos_cog[2]};	
	ced_cluellipse_r((float)sizes[0], (float)sizes[2], center_eli_r, rotation_eli, 1, 0x99999999);

      }/*end loop over clusters*/

    if (nClusters >= 2)
      {
	float *pos_cog_a = (allClusters[0])->getClusterShapesPointer()->getCentreOfGravity();
	float *pos_cog_b = (allClusters[1])->getClusterShapesPointer()->getCentreOfGravity();

	/* calculate distance in r (x-y plane)*/
	float dr_a_b = sqrt( ( ( pos_cog_a[0] - pos_cog_b[0] ) * ( pos_cog_a[0] - pos_cog_b[0] ) ) + 
			     ( ( pos_cog_a[1] - pos_cog_b[1] ) * ( pos_cog_a[1] - pos_cog_b[1] ) ) );
	
	/* calculate distance in z*/
	float dz_a_b = sqrt( ( pos_cog_a[2] - pos_cog_b[2] ) * ( pos_cog_a[2] - pos_cog_b[2] ) );

	streamlog_out(DEBUG)<<"\n Distances between first 2 clusters: "<<endl;
	streamlog_out(DEBUG)<<" in (x,y): "<<dr_a_b<<endl;
	streamlog_out(DEBUG)<<" in z:     "<<dz_a_b<<endl;
      }
    
  }
  

  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  int EventDisplayProcessor::returnClusterSize(float eneCluster, float cutoff_min, float cutoff_max)
  { 
    /**
     * Check the input values for sanity */
    if (cutoff_min > cutoff_max) 
      {
	streamlog_out(WARNING) << "Error in returnClusterSize: cutoff_min < cutoff_max" << endl;
      }
    if (eneCluster < 0.0) 
      {
	streamlog_out(WARNING) << "Error in returnClusterSize: eneCluster is negative!" << endl;
      }
    if (cutoff_min < 0.0) 
      {
	streamlog_out(WARNING) << "Error in returnClusterSize : eneCluster is negative!" << endl;
      }

    int size = 0; //default size: zero
    
    // sizes
    int size_min = 10;
    int size_max = 120;
    
    // Input values in log-scale
    float log_ene = std::log(eneCluster+1);
    float log_min = std::log(cutoff_min+1);
    float log_max = std::log(cutoff_max+1);
    
    int size_steps = size_max - size_min; // default: 90 step sizes
    float log_delta = log_max - log_min;
    float log_step = log_delta/size_steps;
    
    int size_delta = (int) ((log_ene-log_min)/log_step); // which size bin does the value go to?
    
    if (size_delta >= size_steps)
      {
	size = size_max;
      }
    else if (size_delta < size_min)
      {
	size = size_min;
      }
    else 
      {
	size = size_min + size_delta;
      }	
    
  /**
   * Check the output */
    if (size <=0)
      {
	streamlog_out(WARNING) << "Error in returnClusterSize: return size is negative!" << std::endl;
    }

  //		std::cout << "DEBUG: DSTViewer::returnClusterSize()" << std::endl;
  //		std::cout << "log_ene = " << log_ene << std::endl;
  //		std::cout << "log_min = " << log_min << std::endl;
  //		std::cout << "log_max = " << log_max << std::endl;
  //		std::cout << "log_step = " << log_step << std::endl;
  //		std::cout << "size_delta = " << size_delta << std::endl;
  //		std::cout << size << std::endl;

  return size;
}
	

  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawStartPosition(LCEvent *evt) 
  {
    std::vector<float> vec_start_coord;

    evt->parameters().getFloatVals(_parNameShowerStartPos, vec_start_coord);

    // check: skip drawing shower start position for this event if too few position parameters given
    if (vec_start_coord.size() < 3) {
      streamlog_out(MESSAGE) << "shower start position NOT FOUND" << std::endl;
      return; }

    float startPos_x = vec_start_coord.at(0);
    float startPos_y = vec_start_coord.at(1);
    float startPos_z = vec_start_coord.at(2);

    streamlog_out(MESSAGE) << "shower start position " << startPos_x << " " << startPos_y << " " << startPos_z << std::endl;

    unsigned type, size, color;
    type = 2 | (13 << CED_LAYER_SHIFT); // 2=CED_STAR (type), XX << CED_LAYER_SHIFT=CED layer
    size = 200;
    color = 0xff0000; // RED

    if (_draw) ced_hit(startPos_x, startPos_y, startPos_z, type, size, color);

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::drawMCParticle(LCEvent *evt) 
  {
    if (_draw) {
      try {

        LCCollection* col = evt->getCollection( _colNameMCParticle );

        LCTypedVector<MCParticle> MCParticleCol(col);

        for (LCTypedVector<MCParticle>::iterator iter = MCParticleCol.begin();iter != MCParticleCol.end();++iter) {

          double x1 = (*iter)->getVertex()[0];
          double y1 = (*iter)->getVertex()[1];
          double z1 = (*iter)->getVertex()[2];

          double x2 = (*iter)->getEndpoint()[0];
          double y2 = (*iter)->getEndpoint()[1];
          double z2 = (*iter)->getEndpoint()[2];

          ced_line( x1, y1, z1, x2, y2, z2 , 11 << CED_LAYER_SHIFT , 2 , 0x888888);

        }

      } catch ( EVENT::DataNotAvailableException &err) {
        streamlog_out(WARNING) << "MCParticle collection " << _colNameMCParticle << " not available" << std::endl;
      }
    }

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  /* @todo implement rotated/staggered detector...*/
  void  EventDisplayProcessor::drawDetectorAHCAL() 
  {
    CALICE::CellDescription *cell_front_ll = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(25,25,1)));
    CALICE::CellDescription *cell_front_ur = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(61,61,1)));

    int lastLayer = _mapper->getMaxK() - 1;
    streamlog_out(DEBUG)<<" HCAL last layer: "<<lastLayer<<std::endl;
    
    CALICE::CellDescription *cell_back_ll = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(25,25,lastLayer)));
    CALICE::CellDescription *cell_back_ur = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(61,61,lastLayer)));

    double front_center_pos[3];
    front_center_pos[0] = 0.5 * ( cell_front_ll->getX() + cell_front_ur->getX() );
    front_center_pos[1] = 0.5 * ( cell_front_ll->getY() + cell_front_ur->getY() );
    front_center_pos[2] = 0.5 * ( cell_front_ll->getZ() + cell_front_ur->getZ() );

    double back_center_pos[3];
    back_center_pos[0] = 0.5 * ( cell_back_ll->getX() + cell_back_ur->getX() );
    back_center_pos[1] = 0.5 * ( cell_back_ll->getY() + cell_back_ur->getY() );
    back_center_pos[2] = 0.5 * ( cell_back_ll->getZ() + cell_back_ur->getZ() );

    double center_center_pos[3];
    center_center_pos[0] = 0.5 * ( front_center_pos[0] + back_center_pos[0] );
    center_center_pos[1] = 0.5 * ( front_center_pos[1] + back_center_pos[1] );
    center_center_pos[2] = 0.5 * ( front_center_pos[2] + back_center_pos[2] );

    double hcalThickness = 40./lastLayer * ( back_center_pos[2] - front_center_pos[2] );

    double hcalSize[3] = {900.,900.,hcalThickness};

    //double layerRotation[3] = {0.,frontFaceCell_ll->getAngle(),0.};

    streamlog_out(DEBUG)<<"\n===================================="<<std::endl;
    streamlog_out(DEBUG)<<" HCAL box dimensions:"<<std::endl;
    streamlog_out(DEBUG)<<" hcalThickness: "<<hcalThickness<<std::endl;
    streamlog_out(DEBUG)<<"front_center_pos: x="<<front_center_pos[0]<<" y="<<front_center_pos[1]<<" z="<<front_center_pos[2]<<std::endl;
    streamlog_out(DEBUG)<<"back_center_pos:  x="<<back_center_pos[0]<<" y="<<back_center_pos[1]<<" z="<<back_center_pos[2]<<std::endl;
    streamlog_out(DEBUG)<<std::endl;
    streamlog_out(DEBUG)<<" cell_front_ll->getX(): "<<cell_front_ll->getX()<<std::endl;
    streamlog_out(DEBUG)<<" cell_front_ur->getX(): "<<cell_front_ur->getX()<<std::endl;
    streamlog_out(DEBUG)<<std::endl;
    streamlog_out(DEBUG)<<" cell_front_ll->getY(): "<<cell_front_ll->getY()<<std::endl;
    streamlog_out(DEBUG)<<" cell_front_ur->getY(): "<<cell_front_ur->getY()<<std::endl;
    streamlog_out(DEBUG)<<std::endl;
    streamlog_out(DEBUG)<<" cell_front_ll->getZ(): "<<cell_front_ll->getZ()<<std::endl;
    streamlog_out(DEBUG)<<" cell_front_ur->getZ(): "<<cell_front_ur->getZ()<<std::endl;
    streamlog_out(DEBUG)<<"====================================\n"<<std::endl;


    if (_draw) ced_geobox(hcalSize,center_center_pos,0x000000);

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void  EventDisplayProcessor::drawDetectorSiWECAL() 
  {
    // draw SiW-ECAL relative to HCAL (z-position)

    CALICE::CellDescription *ahcal_cell_front_ll = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(25,25,1)));
    CALICE::CellDescription *ahcal_cell_front_ur = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(61,61,1)));

    CALICE::CellDescription *ahcal_cell_back_ll = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(25,25,38)));
    CALICE::CellDescription *ahcal_cell_back_ur = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(61,61,38)));

    double ahcal_front_center_pos_z =  0.5 * ( ahcal_cell_front_ll->getZ() + ahcal_cell_front_ur->getZ() );
    double ahcal_back_center_pos_z =  0.5 * ( ahcal_cell_back_ll->getZ() + ahcal_cell_back_ur->getZ() );

    double ahcal_center_center_pos_z = 0.5 * ( ahcal_front_center_pos_z + ahcal_back_center_pos_z );
    double ahcalThickness = 40./38. * ( ahcal_back_center_pos_z - ahcal_front_center_pos_z );

    double ahcalBegin = ahcal_center_center_pos_z - ( ahcalThickness / 2. );

    // hard-coded numbers from eLog

    double dist_ahc_emc = 32.;
    double emcThickness = 200.;

    double center_center_pos[3];
    center_center_pos[0] = 0.0;
    center_center_pos[1] = 0.0;
    center_center_pos[2] = ahcalBegin - dist_ahc_emc - ( emcThickness / 2. );

    double emcSize[3] = {200.,200.,emcThickness};

    if (_draw) ced_geobox(emcSize,center_center_pos,0x000000);

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void  EventDisplayProcessor::drawDetectorTCMT() 
  {
    // draw TCMT relative to HCAL (z-position)

    CALICE::CellDescription *ahcal_cell_front_ll = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(25,25,1)));
    CALICE::CellDescription *ahcal_cell_front_ur = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(61,61,1)));

    CALICE::CellDescription *ahcal_cell_back_ll = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(25,25,38)));
    CALICE::CellDescription *ahcal_cell_back_ur = _cellDescriptions->getByCellID(_mapper->getTrueCellID(_mapper->getTrueCellID(61,61,38)));

    double ahcal_front_center_pos_z =  0.5 * ( ahcal_cell_front_ll->getZ() + ahcal_cell_front_ur->getZ() );
    double ahcal_back_center_pos_z =  0.5 * ( ahcal_cell_back_ll->getZ() + ahcal_cell_back_ur->getZ() );

    double ahcal_center_center_pos_z = 0.5 * ( ahcal_front_center_pos_z + ahcal_back_center_pos_z );
    double ahcalThickness = 40./38. * ( ahcal_back_center_pos_z - ahcal_front_center_pos_z );

    double ahcalEnd = ahcal_center_center_pos_z + ( ahcalThickness / 2. );

    double dist_ahc_tcmt = 0;
    double tcmtThickness = 0;
    double center_center_pos[3] = {0, 0, 0};

    if (_fMokkaModelName == "TBhcalcatch08_02")
      {
	/* hard-coded numbers from Mokka model TBhcalcatch08_02*/
	dist_ahc_tcmt = 333.;
	tcmtThickness = 1473.2;
      }

    if (_fMokkaModelName == "TBCernJuly2011")
      {
	dist_ahc_tcmt =  83;//110.26;
	tcmtThickness = 1468;
	center_center_pos[0] = -13.2;
	center_center_pos[1] = 17.2;	
      }

    center_center_pos[2] = ahcalEnd + dist_ahc_tcmt + ( tcmtThickness / 2. );

    streamlog_out(DEBUG)<<"\n\n ahcal_center_center_pos_z="<<ahcal_center_center_pos_z
		<<" ahcalThickness="<<ahcalThickness
		<<" ahcalEnd="<<ahcalEnd
		<<" dist_ahc_tcmt ="<<dist_ahc_tcmt 
		<<" tcmtThickness="<<tcmtThickness
		<<" tcmt centered at z="<<center_center_pos[2]
		<<std::endl;
    streamlog_out(DEBUG)<<"TCMT starts at "<<ahcalEnd + dist_ahc_tcmt<<std::endl;

    double tcmtSize[3] = {1000.,1000.,tcmtThickness};

    if (_draw) ced_geobox(tcmtSize,center_center_pos,0x000000);

  }

  /*******************************************************************************************/
  /*                                                                                         */
  /*                                                                                         */
  /*                                                                                         */
  /*******************************************************************************************/
  int EventDisplayProcessor::getMarinaShowerStartHcalLayer(LCEvent *evt)
  {
    LCCollection *col = NULL;
    LCGenericObject *showerStartLayerObj = NULL;
    
    int lastHcalLayer = _mapper->getMaxK() - 1;

    int showerStartHcalLayer = -99;
    
    try{
      col = evt->getCollection(_showerStartColName);
      int nEntries = col->getNumberOfElements();
      
      for (int i = 0; i < nEntries; ++i)
        {
          showerStartLayerObj = dynamic_cast<LCGenericObject*>(col->getElementAt(i));
          
          /*calorimeter type: 0=ECAL, 1=first 30 layers in HCAL, 2=last coarse part of HCAL*/
          int type = showerStartLayerObj->getIntVal(0);
          
          /*shower starting layer number (in each calorimeter)*/
          int showerStart = showerStartLayerObj->getIntVal(1);
          
	  if (type == 1) /*first 30 layers in HCAL*/
	    {
	      showerStartHcalLayer = showerStart + 1;
	      //showerStartHcalLayer = showerStart;
	    }
	  else if (type == 2 && lastHcalLayer > 30)/*last 8 layers in HCAL*/
	    {
	      //showerStartHcalLayer = 30 + showerStart + 1;
	      showerStartHcalLayer = 30 + showerStart;
	      if (showerStartHcalLayer == 39) 
		{
		  //std::cout<<"\n\n Warning, showerStartHcalLayer = 39"<<std::endl;
		  showerStartHcalLayer = 38;
		}
	    }         
          
        }
    }
    catch(EVENT::DataNotAvailableException err){}
        
    return showerStartHcalLayer;    
  }
       

  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::processEvent( LCEvent * evt ) 
  {
    streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber()
                 << "   in run:  " << evt->getRunNumber()
                 << std::endl ;

    _mapper->print(std::cout);
    _mapper->printStats(std::cout);

    /* CED */
    if (_draw && !_appendToExistingCED) 
      {
	ced_new_event();
      }
    
    if (_draw) 
      {
	/* draw Si-W ECAL related part */
      if( parameters()->isParameterSet("Collection_SiWEcalCalorimeterHits") ) 
	{
	  drawDetectorSiWECAL();
	  drawSiWECAL(evt);
	}

      /* draw AHCAL related part */
      if( parameters()->isParameterSet("Collection_AhcalCalorimeterHits") ) 
	{
	  drawDetectorAHCAL();
	  
	  std::vector<CALICE::CellDescription*> allCellDesc = _cellDescriptions->getAllElements();
	  for (unsigned int iCell = 0; iCell < allCellDesc.size(); ++iCell) 
	    {
	      if (allCellDesc[iCell]->getSizeX() == 30.)
		drawCell(allCellDesc[iCell],0x000099,21,false);
	      else if (allCellDesc[iCell]->getSizeX() == 60.)
		drawCell(allCellDesc[iCell],0x000099,22,false);
	      else drawCell(allCellDesc[iCell],0x000099,23,false);
	    }
	  
	  drawAHCAL(evt);
	  
	  if (_drawHcalHistos) _theHcalHistosDrawer->fillHcalHistos(evt, _colNameAhc, _showerStartColName);
	  
	  drawAHCALTracks(evt);
	  
	}

      /* draw Ture MC Ahcal related part */
      if( parameters()->isParameterSet("Collection_AhcalSimCalorimeterHits") ) 
	{
	  drawDetectorAHCAL();
	  drawAHCALSim(evt);
	}

      /* draw TCMT related part */
      if( parameters()->isParameterSet("Collection_TcmtCalorimeterHits") ) 
	{
	  drawDetectorTCMT();
	  drawTCMT(evt);
	}

      /* draw remaining components */
      if( parameters()->isParameterSet("Collection_MCParticle") )
        drawMCParticle(evt);
      
      streamlog_out(DEBUG)<<"\n\n Before drawStartLayer"<<endl;
      drawStartLayer(evt);
      
      if( parameters()->isParameterSet("Parameter_ShowerStartPosition") )
        drawStartPosition(evt);

      if( parameters()->isParameterSet("ShowerShapeColNames") )
        drawShowerShapes(evt);

      
      
      /* last step: send event to CED */
      ced_send_event();
      
    }

    if (_waitForKeyPressed) {
      std::cout << " event " << evt->getEventNumber() << "       [ Press return for next event ] " << std::endl ;
      getchar();
    }

    _nEvt++ ;

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::check( LCEvent * evt ) 
  {

  }


  /**************************************************************************************/
  /*                                                                                    */
  /*                                                                                    */
  /*                                                                                    */
  /**************************************************************************************/
  void EventDisplayProcessor::end()
  {

  }

}
