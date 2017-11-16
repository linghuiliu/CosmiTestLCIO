#include "RemoveBadCells.hh"

// std includes
#include <iterator>
#include <iostream>
#include <algorithm>

// Marlin includes
#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "lccd/IConditionsHandler.hh"

// LCIO includes
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"

// CALICE includes
#include "MappingProcessor.hh"
#include "CellIterator.hh"

namespace CALICE {

  RemoveBadCells::RemoveBadCells() : marlin::Processor("RemoveBadCells")
  {


    _description = "processor for removing bad cells from HitCollections";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides"
                                " the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("AhcMappingProcessor") ) ;

    registerProcessorParameter( "InputColName" ,
                                "The name of the input collection" ,
                                _colName_noCut ,
                                std::string("AhcCalorimeter_Hits") );

    registerProcessorParameter( "colName_rmBad" ,
                                "The name of the AHC hit collection after removing bad cells (output)" ,
                                _colName_rmBad ,
                                std::string("AhcCalorimeter_Hits_rmBadCells") );

    registerProcessorParameter( "ColName" ,
                                "The name of the collection containing the cell quality information",
                                _cellQualityColName,
                                std::string("AhcCellQuality") ) ;
 
    registerProcessorParameter( "isTCMT", "is TCMT",
                                _isTCMT, bool(false));

    _cellQualityCol = NULL;
    _cellQualityContainer = NULL;

  }


  RemoveBadCells::~RemoveBadCells()
  {

    if ( _cellQualityContainer ) delete  _cellQualityContainer;

  }


  void RemoveBadCells::init()
  {

    printParameters();

    _cellQualityChanged              = false;

    std::stringstream message;
    bool error = false;

    _mapper = (AhcMapper*)MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName << ") did not return a valid mapper." << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _cellQualityColName))
      {
        message << " undefined conditions: " << _cellQualityColName << std::endl;
        error = true;
      }

    if (error) {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    _mapperVersion = _mapper->getVersion();

    _cellQualityContainer = new MappedContainer<CellQuality>(_mapper);
  }


  void RemoveBadCells::processRunHeader( LCRunHeader* run)
  {

  }


  /***************************************************************************************/
  /* function to listen for condition changes in LCCD                                    */
  /*                                                                                     */
  /* should only remember pointer of collection and set flag for the changed conditions; */
  /* processing should wait till process event. this ensures that constants that depend  */
  /* on two different conditions collections have both collections available             */
  /*                                                                                     */
  /* the callbacks from LCCD have no guarantied order                                    */
  /*                                                                                     */
  /***************************************************************************************/
  void RemoveBadCells::conditionsChanged( LCCollection * col ) {

    std::string colName = col->getParameters().getStringVal("CollectionName") ;

    if (colName == _cellQualityColName)
      {
        _cellQualityCol = col;
        _cellQualityChanged = true;
      }
    else
      {
        const std::list<lccd::IConditionsHandler*> theList = this->handlerList();
        std::cout<<"AAA "<<theList.size()<<std::endl;

        for(std::list<lccd::IConditionsHandler*>::const_iterator it=theList.begin();it != theList.end();++it){
          std::cout << "AAA handler:" << (*it)->name() << std::endl;
          if( col == (*it)->defaultCollection()){
            std::cout << "using default collection" << std::endl;
          }
        }

        streamlog_out(ERROR) << "Called as conditions listener for collection " << colName << ", but not responsible." << std::endl;
        throw StopProcessingException(this);
      }

  }


  void RemoveBadCells::updateMapping()
  {
    /*clear all objects in the container*/
    _cellQualityContainer->clear();

    if ( _cellQualityCol ) updateCellQuality();

  }


  void RemoveBadCells::updateCellQuality()
  {
    if (!_cellQualityCol)
      {
        streamlog_out(ERROR) << "Cannot update bad cells, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    delete _cellQualityContainer;

    _cellQualityContainer = new MappedContainer<CellQuality>(_mapper);

    if (_isTCMT) _tcmtDeadVec.clear();
    for (int i = 0; i < _cellQualityCol->getNumberOfElements(); ++i)
      {
        CellQuality *cellQuality = new CellQuality(_cellQualityCol->getElementAt(i));

	if (_isTCMT)  _tcmtDeadVec.push_back(cellQuality->getCellID());
        try
          {
            _cellQualityContainer->fillByModuleID(cellQuality->getCellID(), cellQuality);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateCellQuality(): invalid module id " << cellQuality->getCellID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    _cellQualityChanged = false;

  }


  void RemoveBadCells::processEvent( LCEvent * evt )
  {
    if (!_isTCMT)
      {
	if (_mapperVersion != _mapper->getVersion()) this->updateMapping();
	_mapperVersion = _mapper->getVersion();
      }

    if (_cellQualityChanged) updateCellQuality();

    LCCollection* hitCol_noCut;
    LCCollection* hitCol_rmBad = new LCCollectionVec( LCIO::CALORIMETERHIT );
    CellIDDecoder<CalorimeterHit> *decoder = 0;

    try {

      hitCol_noCut = evt->getCollection( _colName_noCut );
      decoder = new CellIDDecoder<CalorimeterHit> (hitCol_noCut);
      LCTypedVector<CalorimeterHit> hits_noCut( hitCol_noCut );
      LCTypedVector<CalorimeterHit>::iterator hitIt;

      std::string encoding = hitCol_noCut->getParameters().getStringVal(LCIO::CellIDEncoding);
      bool hasKminus1 = false;
      if (encoding.find("K-1") != std::string::npos)
	{
	  hasKminus1 = true;
	}
      streamlog_out(DEBUG)<<"\n encoding: "<<encoding<<std::endl;

      for ( hitIt=hits_noCut.begin(); hitIt != hits_noCut.end(); hitIt++ ) 
	{
	  int cellID = (*hitIt)->getCellID0();
	  int layer = 0;
	  int strip = 0;
	  
	  bool isBadCell = false;
	  
	  if (!_isTCMT) isBadCell = _cellQualityContainer->getByCellID( cellID );

	  else if (_isTCMT) 
	    {
	      const int I = (*decoder)(*hitIt)["I"];
	      const int J = (*decoder)(*hitIt)["J"];
	      if (I != 0) strip = I - 1;
	      if (J != 0) strip = J - 1;
	      
	      if (hasKminus1) layer = (*decoder)(*hitIt)["K-1"] + 1;
	      else            layer = (*decoder)(*hitIt)["K"];

 	      /*see calice_db_tools/trunk/dbfill/writeDeadCellMapTCMT.cc*/
	      int layerStripID = layer * 10 + strip * 1000;

	      streamlog_out(DEBUG)<<" layerStripID="<<layerStripID<<" nelem="<<_cellQualityCol->getNumberOfElements()<<std::endl;
	      
	      if(std::find(_tcmtDeadVec.begin(), _tcmtDeadVec.end(), layerStripID) != _tcmtDeadVec.end())
		{
		  isBadCell = true;
		}

	    }/*end isTCMT*/
	  
	  
	  if ( !isBadCell  ) 
	    {
	      //create copy of calorimeter hit
	      CalorimeterHitImpl* hit_copy = new CalorimeterHitImpl();
	      hit_copy->setEnergy( (*hitIt)->getEnergy() );
	      hit_copy->setEnergyError( (*hitIt)->getEnergyError() );
	      hit_copy->setCellID0( (*hitIt)->getCellID0() );
	      hit_copy->setCellID1( (*hitIt)->getCellID1() );
	      hit_copy->setTime( (*hitIt)->getTime() );
	      hit_copy->setPosition( (*hitIt)->getPosition() );
	      
	      //add hit to new collection
	      hitCol_rmBad->addElement(hit_copy);

	      if (_isTCMT) streamlog_out(DEBUG) <<"keep cell layer/strip="<<layer<<"/"<<strip<<std::endl;
	    }
	  else 
	    {	 
	      if (!_isTCMT)
		{
		  int mod = _mapper->getModuleFromCellID( cellID );
		  int chip = _mapper->getChipFromCellID( cellID );
		  int chan = _mapper->getChanFromCellID( cellID );	      
		  streamlog_out(DEBUG) << "SKIP cell: mod " << mod <<", chip " << chip << ", chan " << chan << std::endl;
		}
	      else if (_isTCMT)
		{
		  streamlog_out(DEBUG) << "SKIP cell: layer/strip=" << layer <<"/" << strip<< std::endl;
		}
	      
	    }
	}
      
      //set flag and additional parameters (int, float, string)
      hitCol_rmBad->setFlag( hitCol_noCut->getFlag() );
      
      StringVec parameters_intKeys;
      hitCol_noCut->parameters().getIntKeys( parameters_intKeys );
      for ( unsigned int i = 0; i < parameters_intKeys.size(); i++) {
        std::vector<int> tempVec_int;
        hitCol_noCut->parameters().getIntVals( parameters_intKeys[i], tempVec_int );
        hitCol_rmBad->parameters().setValues( parameters_intKeys[i], tempVec_int );
      }

      StringVec parameters_floatKeys;
      hitCol_noCut->parameters().getFloatKeys( parameters_floatKeys );
      for ( unsigned int i = 0; i < parameters_intKeys.size(); i++) {
        std::vector<float> tempVec_float;
        hitCol_noCut->parameters().getFloatVals( parameters_floatKeys[i], tempVec_float );
        hitCol_rmBad->parameters().setValues( parameters_floatKeys[i], tempVec_float );
      }

      StringVec parameters_stringKeys;
      hitCol_noCut->parameters().getStringKeys( parameters_stringKeys );
      for ( unsigned int i = 0; i < parameters_stringKeys.size(); i++) {
        std::vector<std::string> tempVec_string;
        hitCol_noCut->parameters().getStringVals( parameters_stringKeys[i], tempVec_string );
        hitCol_rmBad->parameters().setValues( parameters_stringKeys[i], tempVec_string );
      }

      evt->addCollection( hitCol_rmBad, _colName_rmBad );

    }
    catch ( DataNotAvailableException err ) {

      std::cout <<  "RemoveBadCells WARNING: Collection "<< _colName_noCut
                << " not available in event "<< evt->getEventNumber() << std::endl;

    }

  }


  void RemoveBadCells::end()
  {
    delete _cellQualityContainer;

  }


  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  RemoveBadCells aRemoveBadCells;

}
