#include "MappingProcessor.hh"

/* Marlin includes*/
#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"

/* CALICE includes*/
#include "AhcMapper.hh"
#include "Ahc2Mapper.hh"

namespace CALICE {

  /* generate instances of static objects
  * has to be done before processor instance is generated, otherwise
  * strange effects can happen.
  */
  std::map<std::string, Mapper*> MappingProcessor::_mapperMap;

  /*************************************************************************/
  /*                                                                       */
  /*                                                                       */
  /*                                                                       */
  /*************************************************************************/
  const Mapper* MappingProcessor::getMapper(const std::string& processorName)
  {
    return _mapperMap[processorName];
  }


  /*************************************************************************/
  /*                                                                       */
  /*                                                                       */
  /*                                                                       */
  /*************************************************************************/
  MappingProcessor::MappingProcessor() : Processor("MappingProcessor")
  {
    _description = "Processor that provides a CALICE Mapping object from conditions data";

    registerInputCollection( LCIO::LCGENERICOBJECT,
      "ModuleConnection" ,
      "Name of the ModuleConnection collection"  ,
      _colNameModuleConnection ,
      std::string("ModuleConnection") ) ;

      registerInputCollection( LCIO::LCGENERICOBJECT,
        "ModuleDescription" ,
        "Name of the ModuleDescription collection"  ,
        _colNameModuleDescription ,
        std::string("ModuleDescription") ) ;


        registerProcessorParameter( "MapperType" ,
        "Type name of the mapper instance. Currently valid identifiers are: AHC" ,
        _mapperTypeName,
        std::string("AHC") ) ;

      }


      /*************************************************************************/
      /*                                                                       */
      /*                                                                       */
      /*                                                                       */
      /*************************************************************************/
      void MappingProcessor::init()
      {
        /* usually a good idea to*/
        printParameters();

        _colModuleConnection  = NULL;
        _colModuleDescription = NULL;

        _moduleConnectionChanged  = false;
        _moduleDescriptionChanged = false;

        std::stringstream message;
        bool error=false;

        if ( _mapperTypeName == "AHC" )
        {
          _mapperMap[name()] = new AhcMapper();
        }
        else if ( _mapperTypeName == "AHC2" )
        {
          _mapperMap[name()] = new Ahc2Mapper();
        }
        else
        {
          message << "Unsupported mapper type name: " << _mapperTypeName << std::endl;
          error = true;
        }

        if (!ConditionsProcessor::registerChangeListener(this, _colNameModuleConnection))
        {
          message << " undefined conditions: " << _colNameModuleConnection << std::endl;
          error=true;
        }

        if (!ConditionsProcessor::registerChangeListener(this, _colNameModuleDescription))
        {
          message << " undefined conditions: " << _colNameModuleDescription << std::endl;
          error=true;
        }

        if (error)
        {
          streamlog_out(ERROR) << message.str();
          throw marlin::StopProcessingException(this);
        }

      }

      /*************************************************************************/
      /*                                                                       */
      /*                                                                       */
      /*                                                                       */
      /*************************************************************************/
      void MappingProcessor::conditionsChanged(  LCCollection *col )
      {
        std::string colName = col->getParameters().getStringVal("CollectionName") ;

        if (colName == _colNameModuleConnection)
        {
          _colModuleConnection     = col;
          _moduleConnectionChanged = true;
        }
        else if (colName == _colNameModuleDescription)
        {
          _colModuleDescription = col;
          _moduleDescriptionChanged = true;
        }

      }


      /*************************************************************************/
      /*                                                                       */
      /*                                                                       */
      /*                                                                       */
      /*************************************************************************/
      void MappingProcessor::processEvent( LCEvent *evt )
      {

        if ( _moduleDescriptionChanged ) {  /* if description has changed connection has to be filled, too*/

          if ( _mapperTypeName == "AHC" )
          static_cast<AhcMapper*>(_mapperMap[name()])->fill(_colModuleDescription, _colModuleConnection);
          else if ( _mapperTypeName == "AHC2" )
          {
            static_cast<Ahc2Mapper*>(_mapperMap[name()])->fill(_colModuleDescription, _colModuleConnection);
            std::cout << "AHC2 Mapper filled!" << std::endl;
          }

          _moduleDescriptionChanged = false;
          _moduleConnectionChanged = false;
        }
        else if ( _moduleConnectionChanged )
        {
          if ( _mapperTypeName == "AHC" )
          static_cast<AhcMapper*>(_mapperMap[name()])->updateConnections(_colModuleConnection);
          else if ( _mapperTypeName == "AHC2" )
          static_cast<Ahc2Mapper*>(_mapperMap[name()])->updateConnections(_colModuleConnection);

          _moduleConnectionChanged = false;
        }

      }

      /*************************************************************************/
      /*                                                                       */
      /*                                                                       */
      /*                                                                       */
      /*************************************************************************/
      void MappingProcessor::end()
      {
        delete _mapperMap[name()];
        _mapperMap.erase(name());
      }

      /* create instance to make processor known to Marlin
      * should be very last thing to do, to prevent order problems during
      * deletion of static objects.
      */
      MappingProcessor aMappingProcessor;

    } /* end namespace CALICE*/
