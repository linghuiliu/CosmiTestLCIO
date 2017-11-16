#include <IntegratedHcalCalibrationProcessor.hh>

#include <lcio.h>
#include <lccd.h>

#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include <ConditionsChangeDelegator.hh>
#include <FastCaliceHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <HcalTempModel.hh>
#include <CalibrationSet.hh>
#include <GainConstants.hh>
#include <InterConstants.hh>
#include <MIPConstants.hh>
#include <TriggerBits.hh>
#include <set>

//#define HCALRECO_DEBUG

namespace CALICE {

  IntegratedHcalCalibrationProcessor aIntegratedHcalCalibrationProcessor;

  IntegratedHcalCalibrationProcessor::IntegratedHcalCalibrationProcessor() : 
    IntegratedHcalProcessor("IntegratedHcalCalibrationProcessor")
  {  
    _description = "Integrated data base based calibration for CALICE Hcal";
  
    registerProcessorParameter("InputCollectionName", 
			       "Name of the input collection",
			       _inputColName, 
			       std::string("CaliceHitsLevel1"));
			    
    registerProcessorParameter("OutputCollectionName", 
			       "Name of the output collection",
			       _outputColName, 
			       std::string("CaliceHitsLevel2"));

    registerProcessorParameter("PedestalSubtraction", 
			       "apply (1) or ignore (0) the pedestal subtraction",
			       _pedestalSubtraction, 
			       (int) 1);		
   
    registerProcessorParameter("ZeroSuppression", 
			       "apply (1) or ignore (0) mip threshold cut",
			       _zeroSuppression, 
			       (int) 1);

    registerProcessorParameter("SignificanceCut", 
			       "Defines how significant the deviation of a hit "
			       "from the pedestal has to be to survive. Always applied.",
			       _significanceCut, 
			       (float) 3.0);

    registerProcessorParameter("SkipPedestals", 
			       "Skip all events which have been used for "
			       "pedestal calculations in the following "
			       "processors",
			       _skipPedestals, 
			       (int) 0);		
	     
    registerProcessorParameter("minPedNumber", "Minimum number of pedestal "
			       "before the pedestal value is considered "
			       "valid and pedestal substraction is applied",
			       _minPedNumber, 
			       (int) 250);

    registerProcessorParameter("MipCut", "Minimal energy deposition in "
			       "units of MIP to keep hit, applied only if ZeroSuppression==1.",
			       _mipCut, 
			       (float) 0.5);		

    registerProcessorParameter("SaturationCorrection", "Toggle application of "
			       "saturation correction",
			       _doSaturationCorrection,
			       true);
	     
  }
    

  void IntegratedHcalCalibrationProcessor::init(){

    //  initialize inherited IntegratedHcalProcessor
    IntegratedHcalProcessor::init();

    //  reset private containers for pedestal
    if ( _pedestalSubtraction == 1 ) {
      for ( unsigned short mod=0; mod<HCAL_N_MOD+1; mod++ ){
	for ( unsigned short cell=0; cell<HCAL_N_CELL; cell++ ){
	  _pedSum[mod][cell] = 0;
	  _pedSumSquare[mod][cell] = 0;
	  _pedNum[mod][cell] = 0;
	  _ped[mod][cell] = 0;
	  _pedWidth[mod][cell] = 0;
	}
      }
      _pedCounter = 0;
    }

    //  reset private counters
    _hitCounter = 0;
    _eventCounter = 0;
    _saturationCounter = 0; 
  }


  void IntegratedHcalCalibrationProcessor::processEvent(LCEvent *evt) {

    lcio::LCCollection* inVector = NULL;
    try {

      // input collection is ADC per cell
      inVector = evt->getCollection(_inputColName);
    
    } // end of try to read input collection
    catch ( DataNotAvailableException &e ) {

      streamlog_out( ERROR0 ) << "IntegratedHcalCalibrationProcessor::processEvent(): " 
			      << "missing collection " << _inputColName
			      << std::endl;
      return;

    }	

    //    create output collection and set appropriate falgs
    lcio::LCCollectionVec* _outputCol = new LCCollectionVec( LCIO::RAWCALORIMETERHIT );
    if ( !_outputCol ) { 
      throw runtime_error( "IntegratedHcalCalibrationProcessor::processEvent(): "
			   "couldn't create new collection" );
    }
    lcio::LCFlagImpl hitFlag( _outputCol->getFlag() );
    hitFlag.setBit( LCIO::RCHBIT_TIME );
    hitFlag.setBit( LCIO::CHBIT_ID1 );
    _outputCol->setFlag( hitFlag.getFlag() );
  

    //   save average calo temp as collection parameter
    float avgTemp = _tempProvider->getAvgTemp();
    _outputCol->parameters().setValue( "avgTemp", avgTemp );
  
    //   save average module temp as collection parameters
    vector< float > avgModTemp;
    vector< int > modNum;
    for( unsigned i=1; i<HCAL_N_MOD; i++ ) {
      try {
	float modTemp = _tempProvider->getAvgModuleTemp( i );
	avgModTemp.push_back( modTemp );
	modNum.push_back( i );
      } catch ( std::out_of_range &e ) {
	//  we don't know how many modules are present
	//  we loop over the maximum of 1...38
	//  and do nothing if module does not exist
      }
    }
    _outputCol->parameters().setValues( "avgModTemp_value", avgModTemp );
    _outputCol->parameters().setValues( "avgModTemp_moduleNumber", modNum );


    //  check whether the event is a pedestal event
    const TriggerBits trigBits( evt->getParameters().getIntVal( PAR_TRIGGER_EVENT ) );
    if ( trigBits.isPurePedestalTrigger() && ( _pedestalSubtraction == 1 ) ) {

      streamlog_out( DEBUG0 ) << "collecting pedestals" << std::endl;

      //  loop over all pedestal amplitudes
      for ( unsigned i=0; 
            i < static_cast< unsigned >( inVector->getNumberOfElements() ); 
            i++ ){

        RawCalorimeterHit* aRawCalorimeterHit = 
          dynamic_cast< RawCalorimeterHit* >( inVector->getElementAt( i ) );

	if ( !aRawCalorimeterHit ) {
	  streamlog_out( ERROR0 ) << "ERROR - Collection " << _inputColName 
				  << " doesn't contain RawCalorimeterHits/"
				  << "FastCaliceHits -- will have to stop" 
				  << std::endl;
	  return;
	}
  
	//  create FastCaliceHit from input, extract information, delete again
        FastCaliceHit* hit = new FastCaliceHit( aRawCalorimeterHit );
        const unsigned short module = hit->getModule();
        const unsigned short cell = hit->getChip()*18 + hit->getChannel();
        float ampl = hit->getEnergyValue();
	delete hit;

        if ((module > HCAL_N_MOD+1) || (cell > HCAL_N_CELL)) {
          streamlog_out( ERROR0 ) << "invalid cell:" << *hit << std::endl;
	  continue;
	}
        streamlog_out( DEBUG0 ) << "  pedestal hit: " << *hit << std::endl;

        //  store the sum / sum2 / N of all pedestal events
        _pedSum[module][cell] += ampl;
        _pedSumSquare[module][cell] += pow(ampl,2);
        _pedNum[module][cell]++;

        //  the pedestal is the average of all pedestal "hits"
        _ped[module][cell] = _pedSum[module][cell]/_pedNum[module][cell];
	//  the pedestal width is the RMS
	_pedWidth[module][cell] = 
	  sqrt( _pedSumSquare[module][cell] / _pedNum[module][cell] -
	        pow( _pedSum[module][cell] / _pedNum[module][cell], 2 ) );
	//  the pedestal error is RMS/sqrt(N)
	_pedError[module][cell] = 
	  _pedNum[module][cell]>0 
	  ? _pedWidth[module][cell]/sqrt(_pedNum[module][cell]) 
	  : 0;
      }
      _pedCounter++;
      if ( _skipPedestals != 0 ) { 
        throw marlin::SkipEventException( &aIntegratedHcalCalibrationProcessor );
      }
    }


    // start of calibration if all constrains are true
    if ( ( _pedCounter >= _minPedNumber ) || ( _pedestalSubtraction == 0 ) ) {
      _eventCounter++;

      // loop over all hits
      for ( unsigned i = 0; 
	    i < static_cast<unsigned>( inVector->getNumberOfElements() ); 
            i++) 
	{
	  
	  RawCalorimeterHit* aRawCalorimeterHit = 
	    dynamic_cast< RawCalorimeterHit* >( inVector->getElementAt( i ) );
	  
	  if ( !aRawCalorimeterHit ) {
	    
	    std::stringstream message;
	    message << "Collection " << _inputColName 
		    << " doesn't contain RawCalorimeterHits/FastCaliceHits" 
		    << std::endl;
	    
	    throw runtime_error(message.str());
	  }
	  FastCaliceHit* hit = new FastCaliceHit( aRawCalorimeterHit );
	  
	  // get the amplitude (in ADC) and further hit properties
	  float ampl = hit->getEnergyValue();
	  float amplErr = hit->getEnergyError();
	  int cellID = hit->getCellID();
	  const int time = hit->getTimeStamp();

	  const unsigned short module = hit->getModule();
	  const unsigned short chip = hit->getChip();
	  const unsigned short channel = hit->getChannel();
	  const unsigned short cell = chip*18 + channel;

	  delete hit;
	  
	  streamlog_out( DEBUG0 ) << "start calibrating hit: " << *hit << std::endl;
	  
	  if ((module > HCAL_N_MOD+1) || (cell > HCAL_N_CELL)) {
	    streamlog_out( ERROR0 ) << "  STOP - invalid cell at mod/chip/chan "
				    << module << "/" << chip << "/" << channel 
				    << std::endl;
	    continue;
	  }

	  //  count hits close or at the ADC limit (ADC has sign bit + 15 data bits)
	  if ( ampl > 30000 ) {
	    _saturationCounter++;
	    _saturations[module]++;
	  }
	  
	  // do (optional) pedestal subtraction
	  if ( _pedestalSubtraction == 1 ) {
	    
	    streamlog_out( DEBUG0 ) << "  subtracting pedestal: " << _ped[module][cell]
				  << std::endl;
	    
	    // subtract the pedestal from the amplitude and add errors in quadrature
	    ampl -= _ped[module][cell];
	    amplErr = std::sqrt( pow( amplErr, 2 ) +
				 pow( _pedError[module][cell], 2 ) );

	    streamlog_out( DEBUG0 ) << "     pedestal subtracted amplitude: " << ampl 
				    << std::endl;
	    
	  }
	  
	  //  apply significance cut
	  if ( ampl < _significanceCut * _pedWidth[module][cell] ) {
	    
	    streamlog_out( DEBUG0 ) << "  STOP - not significant: " << ampl 
				    << " in mod/chip/chan " 
				    << module << "/" << chip << "/" << channel 
				    << "  with cut " << _significanceCut 
				    << "  ped-width " << _pedWidth[module][cell] 
				    << std::endl;
	    continue; 
	  }


	  //  get the mip calibration and convert to MIP
	  float mip  = getMip( cellID );
	  streamlog_out( DEBUG0 ) << "  getting MIP constant: " << mip << std::endl;

	  //  convert ADC -> MIP or stop if no suitable MIP coefficient available
	  if ( mip > 0 )
	    ampl /= mip;
	  else {
	    streamlog_out( ERROR0 ) << "  ERROR - ignoring cell with negative/"
				    << "zero MIP coefficient at mod/chip/chan " 
				    << module << "/" << chip << "/" << channel
				    << std::endl;
	    continue;
	  }
	  streamlog_out( DEBUG0 ) << "  amplitude in MIP: " << ampl << std::endl;

	  if ( _zeroSuppression &&  ampl < _mipCut ) {
	    streamlog_out( DEBUG0 ) << "  STOP - below threshold of: " << _mipCut 
				    << std::endl;
	    continue;
	  }

	  //  we have a hit, so count it
	  _hitCounter++;


	  //  apply (optional) saturation correction ...
	  if( _doSaturationCorrection == true ) {
	    streamlog_out( DEBUG0 ) << "  correcting saturation" << std::endl;

	    //  get the rest of the calibrations
	    float gain = getGain( cellID );
	    float ic   = getIC( cellID );
	    float lightyield = mip/gain*ic;
	    streamlog_out( DEBUG0 ) << "     Gain constant: " << gain
				    << " IC constant: " << ic
				    << " lightyield: " << lightyield
				    << std::endl;
	    if ( lightyield <= 0 ) {
	      streamlog_out( ERROR0 ) << "  ERROR - ignoring cell with negative/"
				      << "zero lightyield at mod/chip/chan " 
				      << module << "/" << chip << "/" << channel
				      << std::endl;
	      continue;
	    }

	    //  1. convert MIP -> pixel
	    ampl = ampl * lightyield;
	    streamlog_out( DEBUG0 ) << "    amplitude [pix]: " << ampl;

	    //  2. correct amplitude
	    ampl = getCorrectedAmplitude(cellID, ampl);
	    streamlog_out( DEBUG0 ) << "  corrected [pix]: " << ampl;

	    //  3. convert pixel -> MIP
	    ampl = ampl / lightyield;
	    streamlog_out( DEBUG0 ) << "  corrected [MIP]: " << ampl 
				    << std::endl;

	    //  TODO: correct calculation of amplitude error
	    amplErr = 0;
	  }

	  streamlog_out( DEBUG0 ) << " DONE - calibrated amplitude: " << ampl 
				  << std::endl;


	  // create the new output hit
	  FastCaliceHit* newHit = new FastCaliceHit( cellID,
						     ampl, 
						     amplErr, 
						     time );
	  if ( !newHit ) { 
	    throw runtime_error("Couldn't create new hit."); 
	  }

	  // add the new hit to the output collection
	  _outputCol->addElement( newHit );
	  
	  // for the temperature correction statistics 
	  fillTempCountMaps( cellID ); 
	  
	}    // end of loop over input hits

      // add the output collection to the event
      evt->addCollection( _outputCol, _outputColName );
    }
  
  }


  void IntegratedHcalCalibrationProcessor::end () {

    printTempCountMaps(std::cout);

    std::cout << "---  " << name () << " Report :" << std::endl;
    std::cout << "  processed " << _eventCounter << " events" << std::endl;

    std::cout << "  processed " << _hitCounter << " hits ("
	      << std::fixed << std::setprecision (1) 
	      << (float) _hitCounter / (float) _eventCounter 
	      << " per event)" 
	      << std::resetiosflags (std::ios::fixed) << std::setprecision (6) 
	      << std::endl;

    std::cout << "  found " << _invalidMIPCounter 
	      << " hits with invalid MIP calibration (" 
	      << std::fixed << std::setprecision (1) 
	      << (float) _invalidMIPCounter / (float) _hitCounter *100 << "%, " 
	      << std::fixed << std::setprecision (1) 
	      << (float) _invalidMIPCounter / (float) _eventCounter 
	      << " per event) " 
	      << std::resetiosflags (std::ios::fixed) 
	      << std::setprecision (6) << std::endl;

    std::cout << std::fixed << std::setprecision (1) << "   per module: ";

    for (std::map < unsigned, unsigned >::const_iterator it =
           _invalidMIPCalibrations.begin (); 
	 it != _invalidMIPCalibrations.end ();
	 it++) {

      std::cout << it->first << "/" 
		<< (float) it->second / (float) _eventCounter 
		<< "  ";
    }

    std::cout << std::resetiosflags (std::ios::fixed) 
	      << std::setprecision (6) 
	      << std::endl;

    std::cout << "  found " 
	      << _invalidSaturationCorrectionCounter 
	      << " hits with invalid saturation correction (" 
	      << std::fixed << std::setprecision (1) 
	      << (float) _invalidSaturationCorrectionCounter /
      (float) _hitCounter *100 << "%, " 
	      << std::fixed << std::setprecision (1) 
	      << (float) _invalidSaturationCorrectionCounter /
      (float) _eventCounter 
	      << " per event) " 
	      << std::resetiosflags (std::ios::fixed) 
	      << std::setprecision (6) << std::endl;

    std::cout << std::fixed << std::setprecision (1) << "   per module: ";

    for (std::map < unsigned, unsigned >::const_iterator it =
	   _invalidSaturationCorrections.begin ();
	 it != _invalidSaturationCorrections.end (); 
	 it++) {
      std::cout << it->first << "/" 
                << (float) it->second /
	(float) _eventCounter 
                << "  ";
    }

    std::cout << std::resetiosflags (std::ios::fixed) << std::setprecision (6) 
	      << std::endl;

    std::cout << "  found " << _saturationCounter 
	      << " saturated hits (" 
	      << std::fixed << std::setprecision (1) 
	      << (float) _saturationCounter /
      (float) _hitCounter *100 << "%, " 
	      << std::fixed << std::setprecision (1) 
	      << (float) _saturationCounter /
      (float) _eventCounter << " per event) " 
	      << std::resetiosflags (std::ios::fixed) 
	      << std::setprecision (6) << std::endl;

    std::cout << std::fixed << std::setprecision (1) << "   per module: ";

    for (std::map < unsigned, unsigned >::const_iterator it =
	   _saturations.begin (); 
	 it != _saturations.end (); 
	 it++) {
      std::cout << it->first << "/" 
                << (float) it->second /
	(float) _eventCounter 
                << "  ";
    }

    std::cout << std::resetiosflags (std::ios::fixed) << std::setprecision (6) 
	      << std::endl;

  }


}
