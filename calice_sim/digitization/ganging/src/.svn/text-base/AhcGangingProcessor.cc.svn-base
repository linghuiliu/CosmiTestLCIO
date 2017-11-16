#include "AhcGangingProcessor.hh"

#include "marlin/Exceptions.h"
#include "EVENT/SimCalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"

#include "MappingProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"

using std::endl;

namespace CALICE
{
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  AhcGangingProcessor::AhcGangingProcessor(const std::string processorName) 
    : marlin::Processor(processorName)
  {
    _description = "AHCal ganging";
    
    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection",
			       _inputColName,
			       std::string("hcalSD"));
    
    registerProcessorParameter("OutputCollectionName",
			       "Name of the output collection",
			       _outputColName,
			       std::string("AhcCalorimeter_Hits_ganged"));

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry "
				"of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellNeighboursProcessorName" ,
                                "Name of the CellNeighboursProcessor instance that provides the neighbours",
				_cellNeighboursProcessorName,
                                std::string("MyNeighboursProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
				"name of CellDescriptionProcessor which takes care of the cell description generation",
				_cellDescriptionProcessorName,
				std::string("MyCellDescriptionProcessor") ) ;
  
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  AhcGangingProcessor::~AhcGangingProcessor()
  {
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcGangingProcessor::init()
  {
    /*print the processor parameters and their values*/
    marlin::Processor::printParameters();

    std::stringstream message;
    bool error = false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);
    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName 
		<< ") did not return a valid mapper." << std::endl;
        error = true;
      }

    _cellNeighbours = CALICE::CellNeighboursProcessor::getNeighbours(_cellNeighboursProcessorName);
    if ( ! _cellNeighbours ) 
      {
	streamlog_out(ERROR) << "Cannot obtain cell neighbours from CellNeighboursProcessor "
		     <<_cellNeighboursProcessorName<<". Maybe, processor is not present" 
		     << std::endl;
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
  void AhcGangingProcessor::processEvent(LCEvent* evt)
  {
    streamlog_out(DEBUG0)<<" \n\n ===================== START event "<<evt->getEventNumber()<<endl;

    LCCollection *inputCol = NULL;
    try{
      inputCol = evt->getCollection( _inputColName );
    }
    catch ( EVENT::DataNotAvailableException &e )
      {
	streamlog_out(DEBUG0)<<" Collection "<<_inputColName<<" not available, skip this event"<<endl;
	//throw marlin::SkipEventException(this);
	return; // allow lcio event go to next processors. 
      }
 
    int nHits = inputCol->getNumberOfElements();
    streamlog_out(DEBUG0)<<" nHcalHits="<<nHits<<endl;

    streamlog_out(DEBUG0)<<"\n\n Start to fill ganged container "<<std::endl;
 
    /*create a new map container. Note the 'false' flag, which tells the container
      that he is not responsible for the deletion of the contained elements.
      We use this because at the end of the event we have all the elements from
      the container into an LCCollection, and Marlin deletes them at the end of the event.*/
    MappedContainer<CalorimeterHitImpl> *gangedContainer = new MappedContainer<CalorimeterHitImpl>(_mapper, false);
 
    /*set the Mokka encoding string*/
    const std::string mokkaEncodingString = inputCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    gangedContainer->getDecoder()->setCellIDEncoding(mokkaEncodingString);
   

    for (int iHit = 0; iHit < inputCol->getNumberOfElements(); ++iHit)
       {
	 SimCalorimeterHit *simHit = dynamic_cast<SimCalorimeterHit*>(inputCol->getElementAt(iHit));
	 int mokkaCellID = simHit->getCellID0();

	 streamlog_out(DEBUG0)<<"\nsim  I/J/K="<< gangedContainer->getDecoder()->getIFromCellID(mokkaCellID)
		      <<"/"<<gangedContainer->getDecoder()->getJFromCellID(mokkaCellID)
		      <<"/"<<gangedContainer->getDecoder()->getKFromCellID(mokkaCellID)
		      <<" cellID: "<<mokkaCellID
		      <<" energy: "<<simHit->getEnergy()
		      <<endl;
	 
	 int trueCellID = 0;
	 try
	   {
	     trueCellID = _mapper->getTrueCellID(mokkaCellID);
	   }
	 catch (BadDataException &e)
	   {
	     streamlog_out(DEBUG0)<<" invalid cell id "<<mokkaCellID<<std::endl<< e.what() << std::endl;
	     continue;
	   }
 
	 streamlog_out(DEBUG0)<<"true I/J/K="<< gangedContainer->getDecoder()->getIFromCellID(trueCellID)
		      <<"/"<<gangedContainer->getDecoder()->getJFromCellID(trueCellID)
		      <<"/"<<gangedContainer->getDecoder()->getKFromCellID(trueCellID)
		      <<endl;
	 	 
	 /*if there is nothing in the container for this cellID, trueHit = NULL*/
	 CalorimeterHitImpl *trueHit = gangedContainer->getByCellID(trueCellID);
	 if (trueHit == NULL)
	   {
	     trueHit = new CalorimeterHitImpl();
	     trueHit->setCellID0(trueCellID);
	     trueHit->setEnergy(simHit->getEnergy());
	     
	     /*fill the container*/
	     gangedContainer->fillByCellID(trueCellID, trueHit);
	     
 	     streamlog_out(DEBUG0)<<"            energy: one="<<trueHit->getEnergy()<<endl;
	   }
	 else /*the container already has a hit with this true cell ID*/
	   {	     
	     int cellID = trueHit->getCellID0();

	     streamlog_out(DEBUG0)<<"            energy: old="<<simHit->getEnergy()
			  <<" I/J/K="<<gangedContainer->getDecoder()->getIFromCellID(cellID)
			  <<"/"<<gangedContainer->getDecoder()->getJFromCellID(cellID)
			  <<"/"<<gangedContainer->getDecoder()->getKFromCellID(cellID)
			  <<endl;
	     streamlog_out(DEBUG0)<<"            energy: new="<<trueHit->getEnergy()<<endl;
	     streamlog_out(DEBUG0)<<"            energy: sum="<<(simHit->getEnergy() + trueHit->getEnergy())<<endl;

	     trueHit->setEnergy(trueHit->getEnergy() + simHit->getEnergy());
	   }

	 streamlog_out(DEBUG0)<<" ->final ganged hit: I/J/K="
		      <<gangedContainer->getDecoder()->getIFromCellID(trueCellID)
		      <<"/"<<gangedContainer->getDecoder()->getJFromCellID(trueCellID)
		      <<"/"<<gangedContainer->getDecoder()->getKFromCellID(trueCellID)
		      <<", energy[GeV]="<<trueHit->getEnergy()<<std::endl;
       }
  

    
    LCCollectionVec *outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    std::vector<CalorimeterHitImpl*> gangedVec = gangedContainer->getAllElements();
    for (std::vector<CalorimeterHitImpl*>::iterator iter = gangedVec.begin();
	 iter != gangedVec.end(); ++iter)
      {
 	CalorimeterHitImpl *currentHit = (*iter);
	outputCol->addElement(currentHit);
      }/*------------ end loop over hits --------------------------------------*/

    LCParameters &param = outputCol->parameters();
    param.setValue(LCIO::CellIDEncoding, mokkaEncodingString);
   
    if (outputCol->getNumberOfElements() > 0)
      evt->addCollection(outputCol, _outputColName);
    
    streamlog_out(DEBUG0)<<"\n After ganging: gangedContainer has "<<gangedContainer->getAllElements().size()<<" elements"<<endl;
    delete gangedContainer;
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcGangingProcessor::end()
  {
  }
  
  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  AhcGangingProcessor aAhcGangingProcessor;

}/*end of namespace CALICE*/
