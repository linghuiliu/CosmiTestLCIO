#include "AhcSROmappingProcessor.hh"

#include "marlin/ConditionsProcessor.h"
#include "lccd/LCConditionsMgr.hh"

using namespace lcio ;
using namespace marlin ;

AhcSROmappingProcessor aAhcSROmappingProcessor;

/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/
AhcSROmappingProcessor::AhcSROmappingProcessor() : Processor("AhcSROmappingProcessor")
{
  _description = "fixes the mapping of the AhcSlowReaoutModBlocks";

  registerInputCollection( LCIO::LCGENERICOBJECT,
			   "inCollectionName" , 
			   "Name of the MCParticle collection"  ,
			   _inColName ,
			   std::string("AhcSroModData") ) ;

  registerInputCollection( LCIO::LCGENERICOBJECT,
			   "mappingCollectionName" , 
			   "Name of the MCParticle collection"  ,
			   _mappingColName ,
			   std::string("AhcSroModMapping") ) ;
  
  registerOutputCollection( LCIO::LCGENERICOBJECT,
			   "outCollectionName" , 
			   "Name of the MCParticle collection"  ,
			   _outColName ,
			   std::string("AhcSroModData_fixed") ) ;
}

/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/
void AhcSROmappingProcessor::init() 
{ 
  /* usually a good idea to*/
  printParameters() ;

  _lastDataCol = 0;

  _mappingAvailable = false;

  if (!marlin::ConditionsProcessor::registerChangeListener(this, _mappingColName)) 
    streamlog_out(ERROR) << "The collection " << _mappingColName 
		 << " is not registered in the conditions processor! No data can be processed." 
		 << std::endl;

  if (!marlin::ConditionsProcessor::registerChangeListener(this, _inColName)) 
    streamlog_out(ERROR) << "The collection " << _inColName 
		 <<" is not registered in the conditions processor! No data can be processed." 
		 << std::endl;

  _conditionsHandler = new CALICE::RunTimeConditionsHandler(_outColName);
  lccd::LCConditionsMgr::instance()->registerHandler(_outColName,_conditionsHandler);
  
  _mappingCol = NULL;
  _inCol      = NULL;
}


/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/
void AhcSROmappingProcessor::conditionsChanged( LCCollection * col ) 
{
  std::string colName = col->getParameters().getStringVal("CollectionName") ;
  
  if (colName == _mappingColName)
    {
      _mappingCol = col;
      _mappingChanged = true;
    }
  else if (colName == _inColName)
    {
      _inCol = col;
      _inputChanged = true;
    }

}


/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/
void AhcSROmappingProcessor::processEvent( LCEvent * evt ) 
{ 
  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
	       << "   in run:  " << evt->getRunNumber() 
	       << std::endl ;


  if (_mappingChanged == true)
    {
      _mapper.updateMapping(_mappingCol);

      _mappingAvailable = true;

      _mappingChanged = false;
    }


  if (_inputChanged == true)
    {
      if (_mappingAvailable)
	_conditionsHandler->update(_mapper.mapCollection(_inCol));

      _inputChanged = false;
    }

}

