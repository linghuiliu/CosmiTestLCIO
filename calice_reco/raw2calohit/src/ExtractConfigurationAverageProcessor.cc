#include "ExtractConfigurationAverageProcessor.hh"

#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <IMPL/LCCollectionVec.h>

#include <IMPL/LCEventImpl.h>
#include <IO/ILCFactory.h>
#include <IO/LCWriter.h>

#include <lccd/DBInterface.hh>

#include <FastCaliceHit.hh>
#include <HcalTileIndex.hh>
#include <collection_names.hh>
#include <SimpleValue.hh>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "RunInformation.hh"
#include "streamlog/streamlog.h"

  CALICE::ExtractConfigurationAverageProcessor<int, float, CALICE::FastCaliceHit>
      aExtractConfigurationAverageProcessor("ExtractConfigurationAverageProcessor", 
                                            &CALICE::FastCaliceHit::getCellID,
                                            &CALICE::FastCaliceHit::getEnergyValue);

namespace CALICE{

  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  ExtractConfigurationAverageProcessor<KEY, 
                                       VALUE, 
                                       OBJECT_TO_AVERAGE>
  ::ExtractConfigurationAverageProcessor(const std::string name,
                                         PMF_KEY pmf_key,
                                         PMF_VALUE pmf_value
                                        ) :
    marlin::Processor(name),
    _defaultString( std::string( "None" ) ),
    _pmf_key(pmf_key),
    _pmf_value(pmf_value),
    _name(name)
{
    _description = "description to come";

    registerProcessorParameter("InputCollection",
			       "Name of input collection of FastCaliceHits",
			       _inColName,
			       std::string("FastCaliceHits"));

    registerProcessorParameter( "OutputCollection",
				"Name of output collection in DB or LCIO file",
				_outColName,
				std::string( "average" ) );

    registerProcessorParameter( "OutputFile",
				"Define file name to store results to LCIO file",
				_outFileName,
				_defaultString );

    registerProcessorParameter( "DBInitString",
				"Define DB init in order to save to DB",
				_dbInit,
				_defaultString );

    registerProcessorParameter( "DBFolder",
				"Specify DB folder in order to save to DB",
				_dbFolder,
				_defaultString );

    registerProcessorParameter( "DBDescription",
				"String to describe database entry",
				_dbDescription,
				std::string( "Results from ExtractConfigurationAverageProcessor" ) );
  }
  
  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::~ExtractConfigurationAverageProcessor(){
  }
  
  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::init(){
    printParameters();

    _from = lccd::LCCDPlusInf;
    _till = lccd::LCCDMinusInf;
  }

  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::processRunHeader( LCRunHeader *run )
  {

    RunInformation runInformation(run);

    _from = (runInformation.runStart()).timeStamp();
    _till = (runInformation.runEnd()).timeStamp();

    //  check whether to write to DB
    bool writeDB = false;
    if ( _dbInit != _defaultString && _dbFolder != _defaultString ) {

      writeDB = true;
      
    }

    streamlog_out_T(MESSAGE) << "Start of run: " << LCTime(_from).getDateString() << '\n'
                           << "end of run:   " << LCTime(_till).getDateString() << std::endl;


    if( (_from == 0 || _till == 0) && writeDB == true ) {

      throw std::runtime_error("Tries to write to DB with impossible from and till.");
      
    }


  }

 
  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::processEvent( LCEvent *evt ){

    //  read input collection from event
    LCCollection* inCol = 0;
    try{
      inCol = evt->getCollection( _inColName );

      //  loop over hits
      for( int i(0); i!=inCol->getNumberOfElements(); ++i ){
	OBJECT_TO_AVERAGE* obj = dynamic_cast<OBJECT_TO_AVERAGE*>( inCol->getElementAt(i) );
	if (obj){
	  _averageObjects[(obj->*_pmf_key)()].addValue((obj->*_pmf_value)());
	}
      } //  hit loop
    } catch (DataNotAvailableException &err){
      //   use Marlin verbosity levels to print 'WARNING - doing nothing'
    }  // close: try to read and process input collection
  }
  
  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::end(){
    //  check whether to write Simple lcio file
    bool writeFile = false;
    if ( _outFileName != _defaultString ) 
      writeFile = true;

    //  check whether to write to DB
    bool writeDB = false;
    if ( _dbInit != _defaultString && _dbFolder != _defaultString ) 
      writeDB = true;

    if ( !writeFile && !writeDB ){
      //  print to screen/log in case neither storage to file/DB is chosen
      print( std::cout );
    } else {
      LCCollection* outCol = getCollection();
      if ( writeFile )
	writeSimpleFile( outCol );
      if ( writeDB )
	writeDBFolder( outCol );
    }
  }
  
  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::print( std::ostream& out ){
    for( AVM_iterator iter = _averageObjects.begin();
	 iter != _averageObjects.end(); ++iter){
      HcalTileIndex cid((*iter).first);
      out << std::setw( 8 ) << cid.getModule() 
	  << std::setw( 8 ) << cid.getChip() 
	  << std::setw( 8 ) << cid.getChannel()
	  << std::setw( 16 ) << (*iter).second.getMean() 
	  << std::setw( 16 ) << (*iter).second.getRMS()
	  << std::setw( 8 ) << (*iter).second.getNumberOfValues()
	  << std::endl;
    }  // close: iterator over internal map
  }

  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::writeSimpleFile( LCCollection *col ){

    LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;
    
    lcWrt->open( _outFileName, LCIO::WRITE_NEW );
    LCEventImpl* evt = new LCEventImpl();
    evt->addCollection( col, _outColName );
    lcWrt->writeEvent( evt );
    lcWrt->flush();
    lcWrt->close();
    
  }

  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  void ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::writeDBFolder( LCCollection* col ){
    lccd::DBInterface db( _dbInit, _dbFolder, true );
    lccd::LCCDTimeStamp till = ( _till == lccd::LCCDPlusInf ) ? _till : _till+1;
    db.storeCollection( _from, till, col, _dbDescription );
  }

  template<class KEY, class VALUE, class OBJECT_TO_AVERAGE>
  LCCollection* ExtractConfigurationAverageProcessor<KEY, VALUE, OBJECT_TO_AVERAGE>::getCollection(){
    LCCollectionVec *col = new LCCollectionVec( LCIO::LCGENERICOBJECT );
    for( AVM_const_iterator iter = _averageObjects.begin();
	 iter != _averageObjects.end(); ++iter ){
      SimpleValue* val = new SimpleValue( (*iter).first, 
					  (*iter).second.getMean(), 
					  (*iter).second.getRMS(), 
					  (*iter).second.getNumberOfValues() );
      col->addElement( val );
    }
    return col;
  }

}; // namespace CALICE

