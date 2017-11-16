#include "CorrectPositionProcessor.hh"

/* Marlin includes*/
#include "marlin/Exceptions.h"

/* LCIO includes*/
#include "Exceptions.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "EVENT/SimCalorimeterHit.h"

/* CALICE includes*/
#include "CellDescriptionProcessor.hh"
#include "MappingProcessor.hh"

using std::endl;

namespace CALICE {

  /* make processor known to Marlin*/
  CorrectPositionProcessor aCorrectPositionProcessor;

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  CorrectPositionProcessor::CorrectPositionProcessor() : Processor("CorrectPositionProcessor") 
  {
    /* tell what this processor does*/
    _description = "Creates new collection with CalorimeterHits that have the position from the CellDescription.";

    registerInputCollection( LCIO::CALORIMETERHIT, "InputCollection",
                             "Name of the input collection that should be corrected",
                             _inputCollectionName,
                             std::string("AhcCalorimeter_Hits") );

    registerOutputCollection( LCIO::CALORIMETERHIT, "OutputCollection",
                              "Name of the output collection for the corrected hits",
                              _outputCollectionName,
                              std::string("AhcCalorimeter_Hits_positionCorrected") );

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "Name of the CellDescriptionProcessor instance that provides"
				" the corrected position of the cells." ,
                                _cellDescriptionProcessorName,
                                std::string("MyCellDescriptionProcessor") ) ;

    registerOptionalParameter( "ScaleEnergy" ,
                               "scale factor for the energy",
                               _energyScaleFactor,
                               (float)1. ) ;

    registerProcessorParameter( "CreateSimRecRelation" ,
                                "Create LCRelation between CalorimeterHit and SimCalorimeterHit" ,
                                _createSimRecRelation,
                                false) ;
    
    registerProcessorParameter( "SimRecRelationName" ,
                                "Name of the relation between the Mokka SimCalorimeterHits"
				" and reconstructed CalorimeterHits",
                                _simRecRelationColName,
                                std::string("AhcSimRecRelation") ) ;
 
    registerProcessorParameter( "SimHitName" ,
                                "Name of the Mokka SimCalorimeterHits collection",
                                _simHitColName,
                                std::string("hcalSD") ) ;
    
    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry "
				"of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;
  }


  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void  CorrectPositionProcessor::init() 
  {
    /* usually a good idea*/
    printParameters();

    bool error = false;

    _cellDescriptions = CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions ) {
      streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor."
		   <<" Maybe, processor is not present" << endl;
      error = true;
    }
 
   _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper )
      {
        streamlog_out(ERROR) << "MappingProcessor::getMapper("<< _mappingProcessorName 
		     << ") did not return a valid mapper." << endl;
        error = true;
      }


    _scaleEnergy = parameterSet("ScaleEnergy");

    if (error) throw StopProcessingException(this);

  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void  CorrectPositionProcessor::processEvent( LCEvent *evt ) 
  {
    try {
      LCCollection *inputCol = evt->getCollection(_inputCollectionName);
      _cellDescriptions->getDecoder()->setCellIDEncoding(inputCol->getParameters().getStringVal("CellIDEncoding"));
      LCTypedVector<CalorimeterHit> input( inputCol );

      LCCollectionVec *output = new LCCollectionVec(LCIO::CALORIMETERHIT);
      output->setFlag(inputCol->getFlag() | 1 << LCIO::CHBIT_LONG );
      output->setSubset(false); /* we will generate our own objects*/
      output->parameters().setValue("CellIDEncoding",inputCol->getParameters().getStringVal("CellIDEncoding"));

      if (_createSimRecRelation == true)
	_recoContainer = new MappedContainer<CalorimeterHitImpl>(_mapper);


      LCRelationNavigator outputNavigator(LCIO::CALORIMETERHIT, LCIO::LCGENERICOBJECT);

      for (unsigned int i=0; i < input.size(); ++i) 
	{
	  CalorimeterHit *oldHit = input[i];

	  CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
	  
	  newHit->setCellID0(      oldHit->getCellID0()     );
	  newHit->setCellID1(      oldHit->getCellID1()     );

	  if (_scaleEnergy) 
	    {
	      newHit->setEnergy(     oldHit->getEnergy()*_energyScaleFactor );
	      newHit->setEnergyError(oldHit->getEnergyError()*_energyScaleFactor );
	    }
	  else 
	    {
	      newHit->setEnergy(       oldHit->getEnergy()      );
	      newHit->setEnergyError(  oldHit->getEnergyError() );
	    }

	  newHit->setTime(         oldHit->getTime()        );
	  newHit->setType(         oldHit->getType()        );
	  newHit->setRawHit(       oldHit->getRawHit()      );

	  
	  CellDescription* cellDescription = _cellDescriptions->getByCellID( newHit->getCellID0() );
	  float newPosition[3] = {cellDescription->getX(), cellDescription->getY(), cellDescription->getZ()};

 	  newHit->setPosition(newPosition);
 	  output->addElement(newHit);

	  /*----------------------------------------------------------------
	    for the relation between CalorimeterHits and SimCalorimeterHits
	  ------------------------------------------------------------------*/
	  if (_createSimRecRelation == true)
	    _recoContainer->fillByCellID(newHit->getCellID0(), newHit);

	}

      evt->addCollection(output,_outputCollectionName);

      /*----------------------------------------------------------------
	for the relation between CalorimeterHits and SimCalorimeterHits
	------------------------------------------------------------------*/
      if (_createSimRecRelation == true)
	{
	  this->createSimRecRelation(evt);
	  /*this->checkSimRecRelation(evt);*/
	}
      

    }
    catch ( const DataNotAvailableException &err) {
      streamlog_out(DEBUG0)<<"Event "<<evt->getEventNumber()<<" "<< err.what() << endl;
      return;
    }


  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void CorrectPositionProcessor::createSimRecRelation(LCEvent *evt)
  {
   const LCCollection *simHitCol = evt->getCollection( _simHitColName ) ;
    if (simHitCol == NULL)
      {
	streamlog_out(WARNING)<<" There is no collection named "<<_simHitColName
		      <<" which contains SimCalorimeterHits in this event ("
		      <<evt->getEventNumber()<<")"<<endl;
	return;
      }

    streamlog_out(DEBUG0)<<"\n\n============================="<<endl;
    streamlog_out(DEBUG0)<<"createSimRecRelation in event "<<evt->getEventNumber()<<endl;

    LCRelationNavigator relationNavigator(LCIO::CALORIMETERHIT, LCIO::SIMCALORIMETERHIT);

    int counter = 0;

    /*set the Mokka encoding string*/
    const std::string mokkaEncodingString = simHitCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    DecoderSet *decoder = _mapper->getDecoder();
    decoder->setCellIDEncoding(mokkaEncodingString);

    for (int iHit = 0; iHit < simHitCol->getNumberOfElements(); ++iHit)
       {
	 SimCalorimeterHit *simHit = dynamic_cast<SimCalorimeterHit*>(simHitCol->getElementAt(iHit));
	 int mokkaCellID = simHit->getCellID0();
	 
	 int trueCellID = 0;

	 try
	   {
	     trueCellID = _mapper->getTrueCellID(mokkaCellID);
	   }
	 catch (BadDataException &e)
	   {
	     streamlog_out(DEBUG0)<<" invalid cell id "<<mokkaCellID<<endl<< e.what() << endl;
	     continue;
	   }

	 CalorimeterHitImpl *recoHit = _recoContainer->getByCellID(trueCellID);
	 
	 streamlog_out(DEBUG0)<<"  i: "<<iHit<<" mokkaCellID="<<mokkaCellID<<" I/J/K="
		      <<_mapper->getDecoder()->getIFromCellID(mokkaCellID)
		      <<"/"<<_mapper->getDecoder()->getJFromCellID(mokkaCellID)
		      <<"/"<<_mapper->getDecoder()->getKFromCellID(mokkaCellID)
		      <<", trueCellID="<<trueCellID
		      <<", I/J/K="<<_mapper->getDecoder()->getIFromCellID(trueCellID)
		      <<"/"<<_mapper->getDecoder()->getJFromCellID(trueCellID)
		      <<"/"<<_mapper->getDecoder()->getKFromCellID(trueCellID)
		      <<" recoHit="<<recoHit
		      <<endl;
	 
	 if (recoHit != NULL)
	   {
	     counter++;
	     relationNavigator.addRelation(recoHit, simHit);
	   }

       }/*end loop over SimCalorimeterHits*/

    streamlog_out(DEBUG0)<<"\n event: "<<evt->getEventNumber()<<" no sim hits: "<<simHitCol->getNumberOfElements()
		 <<" no rec hits: "<<counter
		 <<endl;
    
    evt->addCollection(relationNavigator.createLCCollection(), _simRecRelationColName);

  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void CorrectPositionProcessor::checkSimRecRelation(LCEvent *evt)
  {
    LCCollection* relCol = evt->getCollection( _simRecRelationColName ) ;
    
    if( relCol == 0 ) {
      streamlog_out( WARNING ) << "No relation named "<<_simRecRelationColName<<" found" << endl;
      return ;
    }
    
    /*decoder, used below to set the different encoding strings*/
    DecoderSet *decoder = _mapper->getDecoder();

    /*Navigator used to navigate inside the LCRelation*/
    LCRelationNavigator relNav( relCol) ;
    
    std::vector<CalorimeterHitImpl*> recoVec = _recoContainer->getAllElements();
    for (std::vector<CalorimeterHitImpl*>::iterator iter = recoVec.begin();
	 iter != recoVec.end(); ++iter)
      {
	int cellID = (*iter)->getCellID0();

	/*set the encoding string to the encoding string used 
	 for the reconstructed hits*/
	decoder->setCellIDEncoding(evt->getCollection(_inputCollectionName)->getParameters().getStringVal("CellIDEncoding"));

	CalorimeterHitImpl *recHit = _recoContainer->getByCellID(cellID);
	const LCObjectVec &simHitVec = relNav.getRelatedToObjects(recHit);

	streamlog_out(DEBUG0)<<"\n----------------------"<<endl;
	streamlog_out(DEBUG0)<<" recHit: cellID="<<cellID<<" I/J/K="<<_mapper->getDecoder()->getIFromCellID(cellID)
		     <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
		     <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
		     <<" energy="<<recHit->getEnergy()<<", no sim hits="<<simHitVec.size()
		     <<endl;
	
	/*now set the encoding string to the one used in the SimCalorimeterHits
	 (useful here only for the debug output)*/
	decoder->setCellIDEncoding(evt->getCollection(_simHitColName)->getParameters().getStringVal("CellIDEncoding"));
	for (unsigned int i = 0; i < simHitVec.size(); ++i)
	  {
	    SimCalorimeterHit *simHit = dynamic_cast<SimCalorimeterHit*>(simHitVec[i]);

	    streamlog_out(DEBUG0)<<"   i: "<<i<<" simHit: cellID="<<simHit->getCellID0()<<" I/J/K="
			 <<_mapper->getDecoder()->getIFromCellID(simHit->getCellID0())
			 <<"/"<<_mapper->getDecoder()->getJFromCellID(simHit->getCellID0())
			 <<"/"<<_mapper->getDecoder()->getKFromCellID(simHit->getCellID0())
			 <<" energy="<<simHit->getEnergy()<<endl;
	  }

      }

  }


} /* end namespace CALICE*/
