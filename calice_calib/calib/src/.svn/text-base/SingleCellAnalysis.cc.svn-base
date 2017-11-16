#include "SingleCellAnalysis.hh"

#include <iostream>
#include <fstream>

// LCIO includes
#include "UTIL/LCTypedVector.h"
#include "EVENT/CalorimeterHit.h"

// Marlin includes
#include "marlin/Exceptions.h"

// CALICE includes
#include "MappingProcessor.hh"
#include "CellIterator.hh"

namespace CALICE {

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  SingleCellAnalysis::SingleCellAnalysis() : Processor("SingleCellAnalysis")
  {

    _description = "Processor that collects cellwise informaion" ;

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "Hist_ESum_xbins" ,
                                "Energy sum histogram: number of bins in x-axis",
                                _Hist_ESum_xbins,
                                int( 100 ) ) ;

    registerProcessorParameter( "Hist_ESum_xmin" ,
                                "Energy sum histogram: lower limit on x-axis",
                                _Hist_ESum_xmin,
                                float( -505 ) ) ;

    registerProcessorParameter( "Hist_ESum_xmax" ,
                                "Energy sum histogram: upper limit on x-axis",
                                _Hist_ESum_xmax,
                                float( 505 ) ) ;

    registerInputCollection( LCIO::CALORIMETERHIT ,
                             "AHCALcollection" ,
                             "Name of the AHCAL Hit collection" ,
                             _ahcalColName ,
                             std::string("AhcCalorimeter_Hits"));

    registerProcessorParameter( "OutputFile_txt" ,
                                "Name of the .txt output file" ,
                                _txtOutputName,
                                std::string("SingleCellAnalysis_output.txt") ) ;

    registerProcessorParameter( "OutputFile_root" ,
                                "Name of the .root output file containing one histogram per cell" ,
                                _rootOutputName,
                                std::string("SingleCellAnalysis_output.root") ) ;

    _nEvents = 0;

    _cellStat = NULL;
    _cellHist = NULL;

  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  SingleCellAnalysis::~SingleCellAnalysis()
  {
    if (_cellStat) delete _cellStat;
    if (_cellHist) delete _cellHist;
  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SingleCellAnalysis::init()
  {
    printParameters();

    std::stringstream message;
    bool error = false;

    _mapper = (AhcMapper*)MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName << ") did not return a valid mapper." << std::endl;
        error = true;
      }

    if (error) {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    _mapperVersion = _mapper->getVersion();

    _cellStat = new MappedContainer<SingleCellStatistics>(_mapper);
    _cellHist = new MappedContainer<TH1F>(_mapper);

  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SingleCellAnalysis::updateMapping()
  {
    /*clear all objects in the container*/
    _cellStat->clear();

    _nEvents = 0;

    /*
     * initialise the container with 0 for all valid cellIDs
     */
    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter)
      {
        int cellID = *iter;

        SingleCellStatistics* cellStat = new SingleCellStatistics();
        _cellStat->fillByCellID( cellID, cellStat );

        int module = _mapper->getModuleFromCellID( *iter );
        int chip = _mapper->getChipFromCellID( *iter );
        int channel = _mapper->getChanFromCellID( *iter );

        std::stringstream ss_histName;
        ss_histName << "mod_" << module << "_chip_" << chip << "_chan_" << channel;
        std::string histName = ss_histName.str();
        TH1F* cellHist = NULL;

        cellHist = new TH1F( histName.c_str(), histName.c_str(), _Hist_ESum_xbins, _Hist_ESum_xmin, _Hist_ESum_xmax );
        cellHist->GetXaxis()->SetTitle("signal");
        cellHist->GetYaxis()->SetTitle("# entries");

        if ( cellHist == NULL )throw marlin::StopProcessingException(this);

        _cellHist->fillByCellID( cellID, cellHist );

      }

  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SingleCellAnalysis::processEvent( LCEvent *evt )
  {
    streamlog_out(DEBUG0)<<"\n\n EVENT: "<<evt->getEventNumber()<<std::endl;

    if (_mapperVersion != _mapper->getVersion()) this->updateMapping();
    _mapperVersion = _mapper->getVersion();

    _nEvents++;

    if (_nEvents % 10000 == 0)
      streamlog_out(MESSAGE) << "EventNr: " << _nEvents << std::endl;

    try {
      evt->getCollection( _ahcalColName );

      LCCollection* ahcCol = evt->getCollection( _ahcalColName );

      LCTypedVector<CalorimeterHit> ahcHits( ahcCol );
      LCTypedVector<CalorimeterHit>::iterator ahcHitIt;

      for ( ahcHitIt=ahcHits.begin(); ahcHitIt != ahcHits.end(); ahcHitIt++ ) {

        int cellID = (*ahcHitIt)->getCellID0();

        ( _cellStat->getByCellID( cellID ) )->add_hit( (*ahcHitIt)->getEnergy() );
        ( _cellHist->getByCellID( cellID ) )->Fill( (*ahcHitIt)->getEnergy() );

      }

    } catch (DataNotAvailableException& e) {
      streamlog_out(DEBUG) << "collection not in event : " << e.what()<< std::endl;
      return;
    }

  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SingleCellAnalysis::end()
  {

    //std::ofstream fout_txt( _txtOutputName.c_str(), ios::app );
    std::ofstream fout_txt( _txtOutputName.c_str() );
    fout_txt << "mod/I" << ":"
             << "chip/I" << ":"
             << "chan/I" << ":"
             << "nEvents/I" << ":"
             << "nHits/I" << ":"
             << "ESum_mean/F" << ":"
             << "ESum_rms/F" << ":"
             << "ESum_total/F" << ":"
             << "hit_energy_max/F" << std::endl;

    TFile *fout_root = new TFile( _rootOutputName.c_str(), "RECREATE" );

    /*
     * Check energy and hits in each cell
     */
    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter)
      {
        int cellID = *iter;

        int mod = _mapper->getModuleFromCellID( *iter );
        int chip = _mapper->getChipFromCellID( *iter );
        int chan = _mapper->getChanFromCellID( *iter );

        if ( _nEvents > 0 ) {

          SingleCellStatistics* cellStat = _cellStat->removeByCellID( cellID );

          int nHits = cellStat->get_n_hits();
          double ESum_total = cellStat->get_energy_sum();
          double ESum_mean = cellStat->get_energy_mean();
          double ESum_rms = cellStat->get_energy_rms();
          double hit_energy_max = cellStat->get_hit_energy_max();
          // double nHits_ratio = (double)nHits / (double)_nEvents;

          fout_txt << mod << " "
                   << chip << " "
                   << chan << " "
                   << _nEvents << " "
                   << nHits << " "
                   << ESum_mean << " "
                   << ESum_rms << " "
                   << ESum_total << " "
                   << hit_energy_max << std::endl;

        }
        else {

          fout_txt << mod << " "
                   << chip << " "
                   << chan  << " "
                   << _nEvents << " "
                   << 0 << " "
                   << 0 << " "
                   << 0 << " "
                   << 0 << " "
                   << 0 << std::endl;

        }

        ( _cellHist->removeByCellID( cellID ) )->Write();

      }

    delete _cellStat;
    delete _cellHist;

    fout_root->Close();

  }


  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  SingleCellAnalysis aSingleCellAnalysis;


} // end namespace CALICE
