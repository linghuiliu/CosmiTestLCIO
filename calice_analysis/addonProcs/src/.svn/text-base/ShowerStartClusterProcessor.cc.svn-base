#include "ShowerStartClusterProcessor.hh"

// Marlin includes
#include "marlin/Exceptions.h"

// LCIO includes
#include "IMPL/LCCollectionVec.h"

// CALICE includes
#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "ClusterShapesTyped.hh"

// MarlinUtil includes
#include "ClusterShapes.h"

// c++ includes
#include <algorithm>
#include <sstream>
#include <limits>

namespace CALICE {

  // create instance to make processor known to Marlin
  ShowerStartClusterProcessor aShowerStartClusterProcessor;

  ShowerStartClusterProcessor::ShowerStartClusterProcessor() : Processor("ShowerStartClusterProcessor") {

    _description = "Processor that finds the start of hadronic showers in the AHCAL";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellNeighboursProcessorName" ,
                                "name of CellNeighboursProcessor which takes care of the cell neighbours calculation",
                                _cellNeighboursProcessorName,
                                std::string("AhcCellNeighboursProcessor") ) ;

    registerInputCollection( LCIO::CALORIMETERHIT,
                             "AhcCollection" ,
                             "Name of the AHCAL CalorimeterHit collection"  ,
                             _colNameAhc ,
                             std::string("AhcCalorimeter_Hits") ) ;

    registerProcessorParameter( "StopAfterFirstMatch" ,
                                "Tells if all candiates should be analysed or if processing should stop after shower start is found.",
                                _stopAfterFirstMatch,
                                (int)1 ) ;

    registerProcessorParameter( "SeedThreshold" ,
                                "Threshold above which a hit is considered a seed candidate.",
                                _seedThreshold,
                                (float)1.65 ) ;

    registerProcessorParameter( "MinHitsCluster" ,
                                "Minimum amount of active cells to consider a cluster a shower start candidate.",
                                _minHitsCluster,
                                (int)2 ) ;

    _hitThreshold.push_back(4);
    registerProcessorParameter( "HitThreshold" ,
                                "Minimum amount of active cells in a cluster to be considered as shower start (including cells under seed threshold).",
                                _hitThreshold,
                                _hitThreshold ) ;

    _energyThreshold.push_back(16.);
    registerProcessorParameter( "EnergyThreshold" ,
                                "Energy threshold above which a cluster is considered a shower start.",
                                _energyThreshold,
                                _energyThreshold ) ;

    _maximumAngleDeviation.push_back(90.);
    registerProcessorParameter( "MaximumAngleDeviation" ,
                                "Maximum angle of cluster main axis to connection of IP with center of gravity.",
                                _maximumAngleDeviation,
                                _maximumAngleDeviation ) ;

    _posIP.push_back(0);
    _posIP.push_back(0);
    _posIP.push_back(0);
    registerProcessorParameter( "PositionOfIP" ,
                                "Position of the interaction point. (resp. particle gun), only used for defining direction to IP",
                                _posIP,
                                _posIP ) ;

  }

  void ShowerStartClusterProcessor::init() {


    bool error = false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);
    if ( ! _mapper) {
      streamlog_out(ERROR) << "Cannot obtain Mapper from MappingProcessor: "<<_mappingProcessorName<<". Maybe, processor is not present" << std::endl;
      error = true;
    }
    _mapperVersion = _mapper->getVersion();

    _cellNeighbours = CellNeighboursProcessor::getNeighbours(_cellNeighboursProcessorName);
    if ( ! _cellNeighbours ) {
      streamlog_out(ERROR) << "Cannot obtain cell neighbours from CellNeighboursProcessor: "<<_cellNeighboursProcessorName <<". Maybe, processor is not present" << std::endl;
      error = true;
    }


    if (_hitThreshold.size() != _energyThreshold.size() || _hitThreshold.size() != _maximumAngleDeviation.size() ) {
      streamlog_out(ERROR) << "Parameters HitThreshold, EnergyThreshold and MaximumAngleDeviation need to have the same number of elements." << std::endl
                   << "HitThreshold          has " << _hitThreshold.size() << " elements" << std::endl
                   << "EnergyThreshold       has " << _energyThreshold.size() << " elements" << std::endl
                   << "MaximumAngleDeviation has " << _maximumAngleDeviation.size() << " elements" << std::endl;
      error = true;
    }

    // prepare loosest requirements
    _minHitThreshold = *(min_element(_hitThreshold.begin(),_hitThreshold.end()));
    _minEnergyThreshold = *(min_element(_energyThreshold.begin(),_energyThreshold.end()));
    _maxMaximumAngleDeviation = *(max_element(_maximumAngleDeviation.begin(),_maximumAngleDeviation.end()));

    if (error) throw StopProcessingException(this);

  }

  void ShowerStartClusterProcessor::processRunHeader(LCRunHeader *run) {
    run->parameters().setValues(name()+"_hitThresholds",_hitThreshold);
    run->parameters().setValues(name()+"_energyThresholds",_energyThreshold);
    run->parameters().setValues(name()+"_angleLimits",_maximumAngleDeviation);
  }

  void ShowerStartClusterProcessor::collectHits( MappedContainer<CalorimeterHit>& allHits, LCCollectionVec* collection, const int cellID ) {

    CalorimeterHit *hit = allHits.removeByCellID(cellID);

    if (hit) {
      collection->addElement( hit );

      CellNeighbours *neighbours = _cellNeighbours->getByCellID( cellID );

      std::vector<int> moduleNeighbours = neighbours->getNeighbours(CellNeighbours::module);
      std::vector<int> forwardNeighbours = neighbours->getNeighbours(CellNeighbours::forward);

      /* The cellNeighbours cellID might have been generated with a different encoding.
       * Need to switch here!
       */
      _cellNeighbours->getDecoder()->setCellIDEncoding(CellNeighboursProcessor::getEncodingString(_cellNeighboursProcessorName) );

      for ( std::vector<int>::iterator iter=moduleNeighbours.begin(); iter != moduleNeighbours.end(); ++iter ) {
        CalorimeterHit *neighbourHit = allHits.getByCellID(*iter);
        if ( neighbourHit )
	  {
	    if ( neighbourHit->getEnergy() > _seedThreshold ) 
	      {
		collectHits(allHits, collection, *iter);
	      }
	    else 
	      {
		collection->addElement( allHits.removeByCellID(*iter) );
	      }
	  }
      }

      for ( std::vector<int>::iterator iter=forwardNeighbours.begin(); iter != forwardNeighbours.end(); ++iter ) {
        CalorimeterHit *neighbourHit = allHits.getByCellID(*iter);
        if ( neighbourHit )
	  {
	    if ( neighbourHit->getEnergy() > _seedThreshold ) 
	      {
		collectHits(allHits, collection, *iter);
	      }
	    else 
	      {
		collection->addElement( allHits.removeByCellID(*iter) );
	      }
	  }
      }

    }

  }

  void ShowerStartClusterProcessor::testForShowerStart( LCCollection *collection, std::vector<bool> &alreadyFound, FloatVec& showerStartPosition ) {

    int nHits = collection->getNumberOfElements();
    streamlog_out(DEBUG) << "  no of hits (min threshold): " << nHits << " (" << _minHitThreshold << ")" << std::endl;

    if ( nHits >= _minHitThreshold ) {

      ClusterShapesTyped shape;
      shape.fill<CalorimeterHit>(collection);

      float energy = shape.getClusterShapesPointer()->getTotalAmplitude();
      streamlog_out(DEBUG) << "  energy (min threshold): " << energy << " (" << _minEnergyThreshold << ")" << std::endl;

      if ( energy >= _minEnergyThreshold ) {

        float cogX = shape.getClusterShapesPointer()->getCentreOfGravity()[0];
        float cogY = shape.getClusterShapesPointer()->getCentreOfGravity()[1];
        float cogZ = shape.getClusterShapesPointer()->getCentreOfGravity()[2];
        float inertiaX = shape.getClusterShapesPointer()->getEigenVecInertia()[0]; // inertia vector has lenght 1 already
        float inertiaY = shape.getClusterShapesPointer()->getEigenVecInertia()[1];
        float inertiaZ = shape.getClusterShapesPointer()->getEigenVecInertia()[2];

        float vectorToIPX = cogX-_posIP[0];
        float vectorToIPY = cogY-_posIP[1];
        float vectorToIPZ = cogZ-_posIP[2];
        float vectorToIPLength = sqrt( pow(vectorToIPX,2) + pow(vectorToIPY,2) + pow(vectorToIPZ,2) );
        vectorToIPX /= vectorToIPLength; // normalize
        vectorToIPY /= vectorToIPLength;
        vectorToIPZ /= vectorToIPLength;

        float angleToIP = acos( inertiaX*vectorToIPX + inertiaY*vectorToIPY + inertiaZ*vectorToIPZ )/ M_PI *180.;
        streamlog_out(DEBUG) << "  angleToIP (max): " << angleToIP << " (" << _maxMaximumAngleDeviation << ")" << std::endl;

        float clusterLength = shape.getClusterShapesPointer()->getElipsoid_r_back();    // length to the position nearest to IP

        float showerStartA_x = cogX - clusterLength*inertiaX;
        float showerStartA_y = cogY - clusterLength*inertiaY;
        float showerStartA_z = cogZ - clusterLength*inertiaZ;

        float showerStartB_x = _posIP[0] + vectorToIPX * (vectorToIPLength - clusterLength);
        float showerStartB_y = _posIP[1] + vectorToIPY * (vectorToIPLength - clusterLength);
        float showerStartB_z = _posIP[2] + vectorToIPZ * (vectorToIPLength - clusterLength);

        FloatVec lines;
        // backward shower main axis
        lines.push_back( cogX );
        lines.push_back( cogY );
        lines.push_back( cogZ );
        lines.push_back( showerStartA_x );
        lines.push_back( showerStartA_y );
        lines.push_back( showerStartA_z );

        // line from IP to main axis start
        lines.push_back( _posIP[0] );
        lines.push_back( _posIP[1] );
        lines.push_back( _posIP[2] );
        lines.push_back( showerStartA_x );
        lines.push_back( showerStartA_y );
        lines.push_back( showerStartA_z );

        // line from IP to centre of gravity, but stop at main axis start
        lines.push_back( _posIP[0] );
        lines.push_back( _posIP[1] );
        lines.push_back( _posIP[2] );
        lines.push_back( showerStartB_x );
        lines.push_back( showerStartB_y );
        lines.push_back( showerStartB_z );

        collection->parameters().setValues("lines",lines);

        if ( angleToIP <= _maxMaximumAngleDeviation) 
	  {
	    unsigned int nFound = 0;
          
	    for ( unsigned int i = 0; i < alreadyFound.size(); ++i)
	      {
		if (alreadyFound[i])
		  {
		    ++nFound;
		  }
		else if ( nHits     >= _hitThreshold[i] &&
			  angleToIP <= _maximumAngleDeviation[i] &&
			  energy    >= _energyThreshold[i] ) 
		  {
		    showerStartPosition[i*6] = showerStartA_x;
		    showerStartPosition[i*6 + 1] = showerStartA_y;
		    showerStartPosition[i*6 + 2] = showerStartA_z;
		    showerStartPosition[i*6 + 3] = showerStartB_x;
		    showerStartPosition[i*6 + 4] = showerStartB_y;
		    showerStartPosition[i*6 + 5] = showerStartB_z;
		    
		    alreadyFound[i] = true;
		    ++nFound;
		    
		    streamlog_out(DEBUG) << "  matches requirement set " << i << std::endl
				 << "   hit threshold: "  << _hitThreshold[i] << std::endl
				 << "   energy threshold: " << _energyThreshold[i] << std::endl
				 << "   angleToIP limit: " << _maximumAngleDeviation[i] << std::endl;
		  }
	      }
	    
	    if (nFound == alreadyFound.size()) return;
	  }
      }
    }
  }
  
  void ShowerStartClusterProcessor::processEvent( LCEvent *evt ) {

    try {
      LCCollection* col = evt->getCollection( _colNameAhc );

      _cellIDEncoding = col->getParameters().getStringVal("CellIDEncoding");
      _mapper->getDecoder()->setCellIDEncoding( _cellIDEncoding );

      MappedContainer<CalorimeterHit> allHits(_mapper,false);
      std::vector<CalorimeterHit*> seedHits;

      for (int i=0; i < col->getNumberOfElements(); ++i) {

        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(col->getElementAt(i));
        if (! hit) {
          streamlog_out(ERROR) << "FATAL: element at " << i << " in collection cannot be casted to CalorimeterHit " << std::endl;
          throw StopProcessingException(this);
        }

        allHits.fillByCellID(hit->getCellID0(),hit);
        if ( hit->getEnergy() > _seedThreshold ) seedHits.push_back(hit);
      }

      // sort seed hits from front to back
      SortByZPosition comparator;
      std::sort(seedHits.begin(),seedHits.end(),comparator);

      bool foundShowerStart = false;
      int nCandidate = 0;
      StringVec collectionNames;
      FloatVec showerStart;
      showerStart.resize(_hitThreshold.size()*6, std::numeric_limits<float>::signaling_NaN()); // prepare result vector 2 positions with 3 coordinates
      std::vector<bool> alreadyFound;
      alreadyFound.resize(_hitThreshold.size(),false);

      for (std::vector<CalorimeterHit*>::iterator seedIter = seedHits.begin(); seedIter != seedHits.end() && !( foundShowerStart && _stopAfterFirstMatch); ++seedIter) {

        LCCollectionVec *collection = new LCCollectionVec(LCIO::CALORIMETERHIT);
        collection->setFlag(col->getFlag());
        collection->setSubset();
        collection->parameters().setValue("CellIDEncoding",_cellIDEncoding);

        collectHits(allHits, collection, (*seedIter)->getCellID0()); // this might change the cellID encoding
        allHits.getDecoder()->setCellIDEncoding( _cellIDEncoding ); // go back to old cellID encoding

        if (collection->getNumberOfElements() > _minHitsCluster) {

          ++nCandidate;

          streamlog_out(DEBUG) << " testing cluster " << nCandidate << " for shower start" << std::endl;

          testForShowerStart(collection, alreadyFound, showerStart);
          foundShowerStart = true;

          for (std::vector<bool>::iterator foundIter = alreadyFound.begin(); foundIter != alreadyFound.end(); ++foundIter)
            if (! *foundIter) foundShowerStart = false;

	  std::ostringstream collectionName;
          collectionName << name() << "_showerStartCandidate_" << nCandidate;

 	  evt->addCollection(collection,collectionName.str());
          collectionNames.push_back(collectionName.str());

	  streamlog_out(DEBUG)<<"\n Collection "<<collectionName.str()<<" added to event"<<std::endl;
        }
        else delete collection;

      }

      streamlog_out(DEBUG) << " tested " << nCandidate << " shower start candidates" << "\n"<<std::endl;


      evt->parameters().setValues(name()+"_candidates",collectionNames);
      evt->parameters().setValues(name()+"_showerStartPos",showerStart);
    }
    catch ( DataNotAvailableException &err) {
      streamlog_out(DEBUG) << "AHCAL collection " << _colNameAhc << " not available" << std::endl;
    }

  }

  void ShowerStartClusterProcessor::end() {

  }


  bool SortByZPosition::operator() (const CalorimeterHit* hit, const CalorimeterHit* ref) {
    return ( hit->getPosition()[2] < ref->getPosition()[2] );
  }


} // end namespace CALICE
