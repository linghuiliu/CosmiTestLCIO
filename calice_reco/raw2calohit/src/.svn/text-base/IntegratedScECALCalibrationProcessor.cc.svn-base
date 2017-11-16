// Version 0.1 Nov-10 2008 S.Uozumi
// -Oct-10 2009 (9a10)
//   bug on calculaton of tree means were fixed by coterra.
//   Temporally "/home/ilcsoft/calib_data/Intercalib_v0_0.dat" was put 
//    to give noiay channels.
//   undil in "Sep08_3" directory intercalib_mean, gaincalib_mean and 
//   mipcalib_mean were calculated including null or noisy channel.
// -Sep 11 2009 (9911)
//   new argument for steering file "EffectiveNpix" was added by coterra. 
// -Sep 11 2009 (9911)
//   MPPC saturation correction was implemented by coterra.
// -May 12 2011, by coterra
//    data files 
//    were implied into Calice data base
//
//TODO: now 2008 analysis should be temporarily done without temperture dep and 
// temp_this_evt for 2008 is set at 99999.


#define checkchange 0
#define enecheck 0
#define TempOK 1
#define debug 0
#define debugDeep 0
#define b110827a 0
#define b110908 0
#define b110909 0
#define NoisyChannelDAT 0

#include <IntegratedScECALCalibrationProcessor.hh>

#include <lcio.h>
#include <lccd.h>

#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include <ConditionsChangeDelegator.hh>   //(+)
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

//110512.coterra  added from ScECALCalibratesProcessor.cc
#include "lccd/IConditionsHandler.hh"
#include "ScECALMIPfit.hh"
#include "ScECALGainfit.hh"
#include "ScECALIntercalib.hh"
#include "ScECALTemperature.hh"
#include "ScECALNoisyChannel.hh"

#include <IMPL/CalorimeterHitImpl.h>
//9911.1059
#include <cmath>
//

//#define HCALRECO_DEBUG


namespace CALICE {

IntegratedScECALCalibrationProcessor aIntegratedScECALCalibrationProcessor;

  //IntegratedScECALCalibrationProcessor::IntegratedScECALCalibrationProcessor() : IntegratedScECALProcessor("IntegratedScECALCalibrationProcessor")
IntegratedScECALCalibrationProcessor::IntegratedScECALCalibrationProcessor() : Processor("IntegratedScECALCalibrationProcessor")
{  
  _description = "Integrated data base based calibration for CALICE ScECAL";

  registerProcessorParameter("DataYear",
                             "data taking was done in 2008 or 2009",
                             _dataYear,
                             (int) 2009 ); 

  registerProcessorParameter("ForMIPCalibration",
                             "If yes, this processor return energy in adc without temperature correction and return temperature for event.",
                             _forMIPCalibration,
                             (bool) false ); 

//TODO: temporally slope of temerature dependence of gain 
  registerProcessorParameter("TempCorrGainOn" ,
			     "temperature correnction parameter for gain is achieved or not",
			     _tempcorrgainOn,
                             (bool) false );

  registerProcessorParameter("TempCorrMipOn" ,
			     "temperature correnction parameter for mip is achieved or not",
			     _tempcorrmipOn,
                             (bool) false );

  std::vector<float> tempcorr_gain ;
  tempcorr_gain.push_back(0.0);
  tempcorr_gain.push_back(0.0);
  tempcorr_gain.push_back(25.0);
  registerProcessorParameter("TempCorrGain" ,
                             "temperature correction parapeter for Gain,tempcorr_gain[0]: offset, tempcorr_gain[1]: slope, tempcorr_gain[2]: T0 " , //Temporally T0 is put from xml file (T0 means temperature of Module when calib data was taken.
                             _tempcorr_gain,
                             tempcorr_gain);

  std::vector<float> tempcorr_mip ;
  tempcorr_mip.push_back(0.0);
  tempcorr_mip.push_back(0.0);
  tempcorr_mip.push_back(25.0);
  registerProcessorParameter("TempCorrMip" ,
                             "temperature correction parapeter for Gain, tempcorr_mip[0]: offset, tempcorr_mip[1]: slope, tempcorr_mip[2]: T0" , //Temporally T0 is put from xml file (T0 means temperature of Module when calib data was taken.
                             _tempcorr_mip,
                             tempcorr_mip);

//110511.coterra
//  registerProcessorParameter( "offsetTimeStamp", "Offset value of timestamp", 
//                              _offsetTimeStamp, ( long int) 1221662241 ); 

// instead to set long 122166241(for 2008), 1241067600(for 2009) so 
// to use this _offsetTimeStamp[0]*100000 + _offsetTimeStamp[1]
// UNIXTIME 1241067600 = 2009/04/30/ 05:00:00 UCT(GMT) 14:00 JST 
// and 0:00 CDT(Central Daylight Time of USA)
  std::vector<int> offsetTimeStamp;
  offsetTimeStamp.push_back(12410); // Needs x10^5
  offsetTimeStamp.push_back(67600); // Under 5 fingers
  offsetTimeStamp.push_back(60*60*5); //To set UCT 2009/04/30/ 00:00
//110810 to CDT  offsetTimeStamp.push_back(60*60*9); //To set UCT --> to JST
  offsetTimeStamp.push_back(60*60*-5); //To set UCT --> to CDT.
  registerProcessorParameter( "offsetTimeStamp", "Offset value of timestamp",
		              _offsetTimeStamp,
                              offsetTimeStamp );

// 39932.0000000000(+1.000..) correspond to 1241067600
//  std::vector<double> ondotori2stamp;
  std::vector<int> ondotori2stamp;
//  ondotori2stamp.push_back( 1241067600 / 39933 );
  ondotori2stamp.push_back( 39933 );
  //ondotori2stamp.push_back((double)((23*60*60 + 59*60 + 55) / (9999421296/10000))/10000);
  //ondotori2stamp.push_back( 0.00000864 );
//  ondotori2stamp.push_back((double)((23*60*60 + 59*60 + 55) / 999942.1296)/10000.);
  ondotori2stamp.push_back( 99994 );
  ondotori2stamp.push_back( 21296 );
//TODO:///////////////////  
  registerProcessorParameter( "ondotori2stamp", "To convert Ondotori's timestamp to UCT timestamp",
		              _ondotori2stamp,
                              ondotori2stamp );
// registerProcessorParameter cannot accept "double" Then These are fixed values so far.
//   _ondotori2stamp = ondotori2stamp;


//9911.1012 the effective number of pixels. coterra
  registerProcessorParameter("EffectiveNpix", "The effective number of pixels",
                             _effNpix, ( float ) 999999999. );
  registerProcessorParameter("InputCollectionName", "Name of the input collection",
                           _inputColName, std::string("ScECALHitsLevel0"));
			    
  registerProcessorParameter("OutputCollectionName", "Name of the output collection",
                           _outputColName, std::string("SceCalorimeter_Hits"));
  registerProcessorParameter("PedestalSubtraction", "apply (1) or ignore (0) the pedestal subtraction",
                           _pedestalSubtraction, (bool) true );		   
  registerProcessorParameter("ZeroSuppression", "apply (1) or ignore (0) calibration induced cut",
			    _zeroSuppression, (bool) false );
  registerProcessorParameter("SignificanceCut", "Defines how significant the deviation of a hit from the pedestal has to be to survive the zero suppression.",
			     _significanceCut, (float) 3.0);
  registerProcessorParameter("SkipPedestals", "Skip all events which have been used for pedestal calculations in the following processors",
			     _skipPedestals, (bool) false);			     
  registerProcessorParameter("minPedNumber", "Minimum number of pedestal before the pedestal value is considered valid and pedestal substraction is applied",
			     _minPedNumber, (int) 250);
  registerProcessorParameter("MipCut", "Minimal energy deposition in units of MIP to keep hit",
                             _mipCut, (float) 0.5);			 

  std::vector<float> gainLim ;
  gainLim.push_back(0.1);
  gainLim.push_back(1000.);
  registerProcessorParameter("dvalLimit" ,
			     "lower;0 and upper;1 limit of gainconstant" ,
		             _gainLim,
	                     gainLim );

  std::vector<float> gainErrLim ;
  gainErrLim.push_back(0.001);
  gainErrLim.push_back(1000.);
  registerProcessorParameter("dvalErrorLimit" ,
			     "lower;0 and upper;1 limit of error of gainconstant" ,
		             _gainErrLim,
	                     gainErrLim );

  std::vector<float> uniformGainSlope;
  uniformGainSlope.push_back(0.0);
  uniformGainSlope.push_back(0.0);
  registerProcessorParameter("UniformGainSlope",
		  	     "if UniformGainSlope > 0.001, all channel use simillar gainslope[0] and its error[1]",
		             _uniformGainSlope,
			     uniformGainSlope );


////////////////////////
// added from ScECALCAlibratesProcessor.cc


  //Read calibration constants from CALICE database
  registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ScECALMIPCollection" ,
                             "Name of the ScECAL MIP collection"  ,
                             _ScECALMIPColName ,
                             std::string("ScECALMIP") ) ;
  _ScECALMIPCol              = NULL;

  //110508.coterra
  registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ScECALIntCalibCollection" ,
                             "Name of the ScECAL Inter calibration collection"  ,
                             _ScECALIntCalibColName ,
                             std::string("ScECALIntCalib") ) ;
  _ScECALIntCalibCol         = NULL;

  //110510.coterra
  registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ScECALGain" ,
                             "Name of the ScECAL One p.e. ADC counts collection"  ,
                             _ScECALGainColName ,
                             std::string("ScECALGain") ) ;
  _ScECALGainCol         = NULL;

  registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ScECALTemperatureCollection" ,
                             "Name of the ScECAL temperature collection"  ,
                             _ScECALTemperatureColName ,
                             std::string("ScECALTemperature") ) ;
  _ScECALTemperatureCol      = NULL;

  registerInputCollection( LCIO::LCGENERICOBJECT,
		              "ScECALNoisyChannelCollection" ,
			      "Name of the ScECAL noisy channel collection",
			      _ScECALNoisyChannelColName , 
			      std::string("ScECALNoisyChannel") ) ;
  _ScECALNoisyChannelCol      = NULL;

};

/********************************************************************/
/*                                                                  */
/*  database condition listener                                     */
/*                                                                  */
/********************************************************************/
void IntegratedScECALCalibrationProcessor::conditionsChanged( LCCollection * col )
{
  std::string colName = col->getParameters().getStringVal("CollectionName") ;

  if (colName == _ScECALMIPColName) 
    {

#if b110908
   cout << "b110908 _ScECALMPIColName=" << _ScECALMIPColName.c_str() << endl;
#endif
      
      _ScECALMIPCol = col;
      _MIPConstantChanged = true;
    }
  else if ( colName == _ScECALIntCalibColName )
    {
       _ScECALIntCalibCol = col;
       _IntCalibConstChanged = true; 
    }
  else if ( colName == _ScECALGainColName )
    {
       _ScECALGainCol = col;
       _GainConstChanged = true; 
    }
#if TempOK
  else if ( colName == _ScECALTemperatureColName && _dataYear != 2008 )
    {
       _ScECALTemperatureCol = col;
       _TemperatureChanged = true; 
    }
#endif
  else if ( colName == _ScECALNoisyChannelColName ) 
    {
	_ScECALNoisyChannelCol = col;
	_NoisyChannelChanged = true;
    }

};

 

void IntegratedScECALCalibrationProcessor::init(){

  //IntegratedScECALProcessor::init();
  if (_pedestalSubtraction==true) {
    for (unsigned short layer=0; layer<ScECAL_NLAYERS; layer++){
      for (unsigned short strip=0; strip<ScECAL_NSTRIPS; strip++){
        _pedSum[layer][strip] = 0;
        _pedSumSquare[layer][strip] = 0;
        _pedNum[layer][strip] = 0;
        _ped[layer][strip] = 0;
        _pedError[layer][strip] = 0;
      }
    }
    _pedCounter = 0;
  }
  _hitCounter = 0;
  _invalidMIPCounter = 0;
  _invalidSaturationCorrectionCounter = 0;
  _eventCounter = 0;
  _saturationCounter = 0; 

  // Read calibration constants

////////////////////////////////////////////
///// 110512.0807.coterra

  printParameters();

  // conditions flags
  _MIPConstantChanged  = false;
//110508.coterra
  _IntCalibConstChanged = false;
//110511.coterra
  _GainConstChanged = false;
  _TemperatureChanged   = false;
//121004
  _NoisyChannelChanged = false;

  std::stringstream message;
  bool error = false;

  if (!marlin::ConditionsProcessor::registerChangeListener(this, _ScECALMIPColName))
    {
      message << " undefined conditions: " << _ScECALMIPColName << std::endl;
      error = true;
    }

//110508.coterra
  if (!marlin::ConditionsProcessor::registerChangeListener(this, _ScECALIntCalibColName))
    {
      message << " undefined conditions: " << _ScECALIntCalibColName << std::endl;
      error = true;
    }

//110511.coterra
  if (!marlin::ConditionsProcessor::registerChangeListener(this, _ScECALGainColName))
    {
      message << " undefined conditions: " << _ScECALGainColName << std::endl;
      error = true;
    }
#if TempOK
if ( _dataYear !=2008 ) { //TODO: This should be removed after 2008 temperature DB problem is fixed. 
  if (!marlin::ConditionsProcessor::registerChangeListener(this, _ScECALTemperatureColName))
    {
      message << " undefined conditions: " << _ScECALTemperatureColName << std::endl;
      error = true;
    }
}

  if (!marlin::ConditionsProcessor::registerChangeListener(this, _ScECALNoisyChannelColName))
    {
      message << " undefined conditions: " << _ScECALNoisyChannelColName << std::endl;
      error = true;
    }


#endif
  if (error) {
    streamlog_out(ERROR) << message.str();
    //    throw marlin::StopProcessingException(this);
  }


#if NoisyChannelDAT 
  int noislayer,noisstrip;

  //TODO: write noisy channel information into CALICE database.

  //9a09.Noisy channel
  //ifstream file_noisy("/scratch/coterra/frmAzusa/scecal_codes/calib_data/NoisyChanSep08.dat");
  //ifstream file_noisy("/home/coterra/tempo_data/NoisyChanSep08.dat");
  ifstream file_noisy("./NoisyChanSep08.dat");
  if (file_noisy==0){
    cout<<"!!!ERROR ScECAL Noisy Channel data File Does Not EXIST!!!"<<endl;
  }
  _nnoisy = 0;
  for ( int i = 0; i < ( ScECAL_NLAYERS * ScECAL_NSTRIPS ); i++ ) {
    _noisychanLayer[i] = -99999;
    _noisychanStrip[i] = -99999;
  }
  do {
    file_noisy>>noislayer>>noisstrip;
    _noisychanLayer[_nnoisy] = noislayer;
    _noisychanStrip[_nnoisy] = noisstrip;
    _nnoisy++;
  } while (!file_noisy.eof());

#endif

  _listReadCounter = 1; 

};

void IntegratedScECALCalibrationProcessor::processRunHeader(LCRunHeader* run) {

  cout << " RUN HEADER: ScECALCalibratesProcessor::processRunHeader " << endl;

};


void IntegratedScECALCalibrationProcessor::processEvent(LCEvent *evt) {

  int nEvent = evt->getEventNumber();
//  if ( nEvent%100 == 0 ) {
    std::cout <<" Event number : "<< nEvent <<std::endl;
//  }

//Noisy channel read start
 
  if (_NoisyChannelChanged )
    {
 
    if (!_ScECALNoisyChannelCol)
      {
         streamlog_out(ERROR) << "Cannot update Noisy channel, ScECAL Noisy channel collection is not valid." << std::endl;
         //        throw StopProcessingException(this);
      }

     _nnoisy = _ScECALNoisyChannelCol->getNumberOfElements(); 
     for (int i = 0; i < _nnoisy; ++i)
      {
         ScECALNoisyChannel *ScEcalNoisyChannel = new ScECALNoisyChannel( _ScECALNoisyChannelCol->getElementAt(i) );
 
         _noisychanLayer[i] = ScEcalNoisyChannel->getID0();
         _noisychanStrip[i] = ScEcalNoisyChannel->getID1();
 
         if ( nEvent%1000 == 0 ){ 
           cout << "NoisyChannel[" << i << "] layer-strip " << _noisychanLayer[i] << "-" << _noisychanStrip[i] << endl;
         } 
 
 
         delete ScEcalNoisyChannel;
      }
 
 
    _NoisyChannelChanged = false;
 
    }
//Noisy channel read finish


// Read calibration constants

  // MIP calibration constants
  int layer, strip, nSuccess;
  double mpv_landau, mpv_error, slope, slope_err, setTemp;

  double mipcalib_mean = 0;
  double mipcalib_err_mean = 0;
  double slope_mean = 0;
  double slope_err_mean = 0;
  double setTemp_mean  = 0;
  int mipcalib_count = 0;


#if checkchange
  cout << "HELLO  _MIPConstantChanged 0=" << _MIPConstantChanged << endl;
#endif

 if (_MIPConstantChanged ) // updateScECALMIP();
   {
#if checkchange
  cout << "HELLO  _MIPConstantChanged 1=" << _MIPConstantChanged << endl;
#endif

    if (!_ScECALMIPCol)
      {
        streamlog_out(ERROR) << "Cannot update MIP, ScECAL MIP collection is not valid." << std::endl;
        //        throw StopProcessingException(this);
      }
    for (int i = 0; i < _ScECALMIPCol->getNumberOfElements(); ++i)
      {
        ScECALMIPfit *ScEcalMip = new ScECALMIPfit( _ScECALMIPCol->getElementAt(i) );
#if checkchange
  cout << "HELLO  _MIPConstantChanged 1.3=" << _MIPConstantChanged << endl;
#endif
        layer = ScEcalMip->getID0();
        strip = ScEcalMip->getID1();
        mpv_landau  = ScEcalMip->getmpv_landau();
        mpv_error   = ScEcalMip->getmpv_error();
        slope       = ScEcalMip->getslope();
        slope_err   = ScEcalMip->getslope_err();
        setTemp     = ScEcalMip->getsetTemp();
        nSuccess    = ScEcalMip->getnSuccess();
#if checkchange
  cout << "HELLO  _MIPConstantChanged 1.4=" << _MIPConstantChanged << endl;
#endif
        _mipcalibconst[layer-1][strip-1] = mpv_landau;
        _mipcalibconst_err[layer-1][strip-1] = mpv_error; //110907 coterra sigma_gauss /sqrt( area_langau );
        _mipSlope[layer-1][strip-1] = slope;
        _mipSlopeErr[layer-1][strip-1] = slope_err;
        _mipSetTemp[layer-1][strip-1] = setTemp;

//        if ( nEvent%1000 == 0 ){ 
          cout << "MIPcalib " << layer << "-" << strip << " " << _mipcalibconst[layer-1][strip-1] 
               << " +- " << _mipcalibconst_err[layer-1][strip-1] << " slope: " << _mipSlope[layer-1][strip-1] 
               << " +- " << _mipSlopeErr[layer-1][strip-1] << " setTemp: " << _mipSetTemp[layer-1][strip-1]
               << endl;
//        }
         bool noisy = kFALSE; 
         for ( int i = 0; i < _nnoisy; i++ ) {
            if ( layer== _noisychanLayer[i] &&
                 strip== _noisychanStrip[i]    ) {
		     noisy = kTRUE;
                     _mipcalibconst[layer-1][strip-1] = 999999.9;
            }
         }
         if ( !noisy && _mipcalibconst[layer-1][strip-1] > 0.1 ) {
              mipcalib_mean += _mipcalibconst[layer-1][strip-1];
              mipcalib_err_mean += _mipcalibconst_err[layer-1][strip-1];
              slope_mean        += _mipSlope[layer-1][strip-1]; 
              slope_err_mean    += _mipSlopeErr[layer-1][strip-1];
              setTemp_mean      += _mipSetTemp[layer-1][strip-1];
              mipcalib_count++;
         }
        delete ScEcalMip;
      }

     mipcalib_mean /= mipcalib_count;
     mipcalib_err_mean /= mipcalib_count;
     slope_mean        /= mipcalib_count;
     slope_err_mean    /= mipcalib_count;
     setTemp_mean      /= mipcalib_count;

//110514 these lines should be in this loop 
//110514.0224.coterra CAUTION that layer stats 0 and strip also...
     for (int layer=0;layer<ScECAL_NLAYERS;layer++) {
       for (int strip=0;strip<ScECAL_NSTRIPS;strip++) {
         if ( _mipcalibconst[layer][strip] < 0.1) {
            _mipcalibconst[layer][strip]=mipcalib_mean;
            _mipcalibconst_err[layer][strip]=mipcalib_err_mean;
            _mipSlope[layer][strip] = slope_mean;
            _mipSlopeErr[layer][strip] = slope_err_mean;
            _mipSetTemp[layer][strip] = setTemp_mean;
         }
       }
     }

    _MIPConstantChanged = false;

   }

#if checkchange
  cout << "HELLO  _MIPConstantChanged 2=" << _MIPConstantChanged << endl;
#endif

//*********** read the MIP from database*************
  // Revise calibration constants for no-good channels

#if checkchange
  cout << "HELLO  _MIPConstantChanged 3=" << _MIPConstantChanged << endl;
#endif

  // Gain also temperature correction. 2012.10.04
  double gain, gain_error, gainslope, gainslope_err, gainsetTemp;

  double gaincalib_mean = 0;
  double gaincalib_err_mean = 0;
  double gainslope_mean = 0;
  double gainslope_err_mean = 0;
  double gainsetTemp_mean  = 0;
  int gaincalib_count = 0;


#if checkchange
  cout << "HELLO  _GainConstChanged 0=" << _GainConstChanged << endl;
#endif

 if (_MIPConstantChanged ) // updateScECALMIP();
   {
#if checkchange
  cout << "HELLO  _GainConstChanged 1=" << _GainConstChanged << endl;
#endif

    if (!_ScECALGainCol)
      {
        streamlog_out(ERROR) << "Cannot update MIP, ScECAL MIP collection is not valid." << std::endl;
        //        throw StopProcessingException(this);
      }
    for (int i = 0; i < _ScECALGainCol->getNumberOfElements(); ++i)
      {
        ScECALGainfit *ScEcalGain = new ScECALGainfit( _ScECALGainCol->getElementAt(i) );
#if checkchange
  cout << "HELLO  _GainConstChanged 1.3=" << _GainConstChanged << endl;
#endif
        layer = ScEcalGain->getID0();
        strip = ScEcalGain->getID1();
        gain          = ScEcalGain->getdval();
        gain_error    = ScEcalGain->getdval_error();
        gainslope     = ScEcalGain->getslope();
        gainslope_err = ScEcalGain->getslope_err();
        gainsetTemp   = ScEcalGain->getsetTemp();
        nSuccess      = ScEcalGain->getnSuccess();
#if checkchange
  cout << "HELLO  _GainConstChanged 1.4=" << _GainConstChanged << endl;
#endif
        _gaincalibconst[layer-1][strip-1] = gain;
        _gaincalibconst_err[layer-1][strip-1] = gain_error; //110907 coterra sigma_gauss /sqrt( area_langau );
        _gainSlope[layer-1][strip-1] = gainslope;
        _gainSlopeErr[layer-1][strip-1] = gainslope_err;
        _gainSetTemp[layer-1][strip-1] = gainsetTemp;

//        if ( nEvent%1000 == 0 ){ 
          cout << "Gaincalib " << layer << "-" << strip << " " << _gaincalibconst[layer-1][strip-1] 
               << " +- " << _gaincalibconst_err[layer-1][strip-1] << " gainslope: " << _gainSlope[layer-1][strip-1] 
               << " +- " << _gainSlopeErr[layer-1][strip-1] << " gainsetTemp: " << _gainSetTemp[layer-1][strip-1]
               << endl;
//        }
         bool noisy = kFALSE; 
         for ( int i = 0; i < _nnoisy; i++ ) {
            if ( layer== _noisychanLayer[i] &&
                 strip== _noisychanStrip[i]    ) {
		     noisy = kTRUE;
                     _gaincalibconst[layer-1][strip-1] = 999999.9;
            }
         }
         if ( !noisy && _gainLim[0] < _gaincalibconst[layer-1][strip-1] && _gaincalibconst[layer-1][strip-1] < _gainLim[1]  
		     && _gainErrLim[0] < _gaincalibconst_err[layer-1][strip-1] && _gaincalibconst_err[layer-1][strip-1] < _gainErrLim[1]	 
			 ) {
              gaincalib_mean += _gaincalibconst[layer-1][strip-1];
              gaincalib_err_mean += _gaincalibconst_err[layer-1][strip-1];
              gainslope_mean        += _gainSlope[layer-1][strip-1]; 
              gainslope_err_mean    += _gainSlopeErr[layer-1][strip-1];
              gainsetTemp_mean      += _gainSetTemp[layer-1][strip-1];
              gaincalib_count++;
         }
        delete ScEcalGain;
      }

     gaincalib_mean        /= gaincalib_count;
     gaincalib_err_mean    /= gaincalib_count;
     gainslope_mean        /= gaincalib_count;
     gainslope_err_mean    /= gaincalib_count;
     gainsetTemp_mean      /= gaincalib_count;

//110514 these lines should be in this loop 
//110514.0224.coterra CAUTION that layer stats 0 and strip also...
     for (int layer=0;layer<ScECAL_NLAYERS;layer++) {
       for (int strip=0;strip<ScECAL_NSTRIPS;strip++) {
          if (  _gainLim[0] >= _gaincalibconst[layer-1][strip-1] || _gaincalibconst[layer-1][strip-1] >= _gainLim[1]  
		     || _gainErrLim[0] >= _gaincalibconst_err[layer-1][strip-1] || _gaincalibconst_err[layer-1][strip-1] >= _gainErrLim[1]	 
	        ) {
            _gaincalibconst[layer][strip]=gaincalib_mean;
            _gaincalibconst_err[layer][strip]= gaincalib_err_mean;
            _gainSlope[layer][strip] = gainslope_mean;
            _gainSlopeErr[layer][strip] = gainslope_err_mean;
            _gainSetTemp[layer][strip] = gainsetTemp_mean;
         }
       }
     }

    _GainConstChanged = false;

   }

#if checkchange
  cout << "HELLO  _MIPConstantChanged 2=" << _MIPConstantChanged << endl;
#endif

//*********** read the MIP from database*************
  // Revise calibration constants for no-good channels

#if checkchange
  cout << "HELLO  _MIPConstantChanged 3=" << _MIPConstantChanged << endl;
#endif


//110508.coterra
  //int layer,strip; 
  //double mpv_rangau, calibconst_err;
  double calibconst = 0;
  double calibconst_err = 0;
  double intcalib_mean = 0;
  double intcalib_err_mean = 0;
  int intcalib_count = 0;

 if (_IntCalibConstChanged )
   {

    if (!_ScECALIntCalibCol)
      {
        streamlog_out(ERROR) << "Cannot update Inter calib, ScECAL Inter calib collection is not valid." << std::endl;
        //        throw StopProcessingException(this);
      }

    for (int i = 0; i < _ScECALIntCalibCol->getNumberOfElements(); ++i)
      {
        ScECALIntercalib *ScEcalIntCalib = new ScECALIntercalib( _ScECALIntCalibCol->getElementAt(i) );

        layer = ScEcalIntCalib->getID0();
        strip = ScEcalIntCalib->getID1();
        calibconst  = ScEcalIntCalib->getcalibconst();
        calibconst_err  = ScEcalIntCalib->getcalibconsterr();

        _intercalibconst[layer-1][strip-1] = calibconst;
        _intercalibconst_err[layer-1][strip-1] = calibconst_err;

        if ( nEvent%1000 == 0 ){ 
          cout << "IntCalib " << layer << "-" << strip << " " << _intercalibconst[layer-1][strip-1] << " +- " << _intercalibconst_err[layer-1][strip-1] << endl;
        } 

//110514.0958 Temporarily we do not care if this is noisy channel or not, although we care it for mipcalibrationconst.
        if (  _intercalibconst[layer-1][strip-1] > 0.1 ) {
          intcalib_mean += _intercalibconst[layer-1][strip-1];
          intcalib_err_mean += _intercalibconst_err[layer-1][strip-1];
          intcalib_count++;
        }

        delete ScEcalIntCalib;
      }

     intcalib_mean /= intcalib_count;
     intcalib_err_mean /= intcalib_count;

//110514.0224.coterra CAUTION that layer stats 0 and strip also...
     for (int layer=0;layer<ScECAL_NLAYERS;layer++) {
       for (int strip=0;strip<ScECAL_NSTRIPS;strip++) {
         if (_intercalibconst[layer][strip] < 0.1) { 
             _intercalibconst[layer][strip]=intcalib_mean;
             _intercalibconst_err[layer][strip]=intcalib_err_mean;
        }
       }
     }

    _IntCalibConstChanged = false;

   }

// std::map<long64, float> _templist;
 int datacounter = 0;
 long offset = _offsetTimeStamp[0]*100000 + _offsetTimeStamp[1] - _offsetTimeStamp[2];
#if TempOK
 if (_TemperatureChanged && _dataYear != 2008 ) 
  {
    for (int i = 0; i < _ScECALTemperatureCol->getNumberOfElements(); ++i) //for ScECALTemperatureCol Element
      {

      if (!_ScECALTemperatureCol)
        {
          streamlog_out(ERROR) << "Cannot update temperature data, ScECAL Temperature collection is not valid." << std::endl;
          //        throw StopProcessingException(this);
        }


   //   time_stamp -= _offsetTimeStamp; 

          ScECALTemperature *ScEcalTemper = new ScECALTemperature( _ScECALTemperatureCol->getElementAt(i) );

//	  year = ScEcalTemper->getyear();
//	  month = ScEcalTemper->getmonth();
//	  day = ScEcalTemper->getday();
//	  hour_minute = ScEcalTemper->gethour_minute();
//	  second = ScEcalTemper->getsecond();
	  double timeStampOndotori = ScEcalTemper->gettimestamp();
          float temperature_1 = ScEcalTemper->gettemp1();
          float temperature_2 = ScEcalTemper->gettemp2();
          float temperature_0 = ( temperature_1 + temperature_2 ) /2. ;// avarage of two temp

#if debugDeep
	cout << "HERE_TEMP_FROM_DB; stamp= " << std::setprecision(16) <<timeStampOndotori 
             << " temp1= " << temperature_1 << " temp2= " << temperature_2 
             << " temp_avr= " << temperature_0 << endl;
#endif

// NOTE THAT "Ondotori" is name of stand alone measurement system used in 2009 
//   ScECAL test beam experiment at FNAL.
//110714.coterra timeStampOndotori is like "xxxxx.yyyyyyyyyy".
// We want to comvert this number to in the same format as in the CALICE DB 
// timestamp.
// We set time1 is a five-finger integer of xxxxx and 
// time2 is a 10-finger integer from numbers after decimal point, like yyyyyyyyyy.
// Note that, but I don't know the reason, time1 part is incremantd 
//  when the time arrive at "one" o'clock, not 0(zero) of time2.
// NOTE THAT 39933.0000000000 corresponds to 
// JST of ( 2009/04/30 00:00:00.0000...) and 
// Unixtime of ( 2009/04/30 00:00:00.0000... + 09:00:00 ) 
// According to (23x60x60 + 50*59 +55)/9999421296( the numerator is a time in second 
// just before the day changes and denominator is time2 part in table corresponding 
// to the numerator and this number should be 0.00000864 ), 
// one second corresponds to 1/0.00000864 of time2 or one in time2 
// To make this number, 99994 and 21296 are assigned as 
// _ondotori2stamp[1] and _ondotori2stamp[2] respectively.
// In this way, 23x60x60 + 50*59 +55 should also be parameters.
// (TODO;) Therefore I should change way of parameters in this context.

// Since xxxxx becomes xxxxx + 1 when yyyyyyyyyy > 60x60/0.00000864
// xxxxx is incremented +1 during 0 < yyyyyyyyyy < 60x60/0.00000864 in this code.
//
// How did I get these relations.
//  from 
// year month day hour_minute second timestamp(ondotori) temp1 temp2
// =================================================================
// 2009 4 29 2359 55 39932.9999421296   24.5000   24.4000
// 2009 4 30 0000 55 39932.0006365741   24.6000   24.4000
// from upper line 9999421296 corresponding to 2009/April/29th 23:59:55
// Therefore, (23x60x60+59x60+55)seconds corresponts to 9999421296
// and 2009/April/30th/00:00:00 """ CHICAGO summer time(CDT) corresponds
// 1241067600 in unix time. Therefore, this number corresponds to 39932+1.
// Plus 1 is due to that 39932 is incremented to 39933 after one hour.
// and we use 39933 as time1 for also 39932.0000000000 to 
// 39932.(59minutes+59seconds)

	  int  time1 = (int) timeStampOndotori; //Integer part of timestamp of Ondotori
#if debugDeep
	cout << " time1= " << time1 << endl;
#endif
          //long64  ten2ten = 100000*100000;
          long ten5 = 100000;
          double timeminer = timeStampOndotori - (double)time1;
#if debugDeep
	cout << " timeminer= " << timeminer << endl;
#endif
          double middle = timeminer * ten5;
		middle *= ten5;
	  long64 time2 = (long64)middle; //* ten5 * ten5 ;
          //long64 time2 = timeminer * ten2ten;

	  long64 maxtime_blwDecimal = (long64)_ondotori2stamp[1]*100000 + _ondotori2stamp[2];
#if debugDeep
          cout << " time2 = " << std::setprecision(16) << time2 << " maxtime_blwDecimal = " <<std::setprecision(16) << maxtime_blwDecimal << endl;
#endif

	  double   time_denomi = ((double)maxtime_blwDecimal/ten5);
          double  time2toUnixtimefactor = (23.*60.*60. + 59.*60. + 55.)/ten5;
	          time2toUnixtimefactor /= time_denomi;
#if debugDeep
          cout << " time2 = " << time2 << " time2toUnixtimefactor = " <<std::setprecision(16) << time2toUnixtimefactor << endl;
#endif


          bool zerohour = false;
//TODO; 3600 is metal wired.
          if ( time2 * time2toUnixtimefactor < 3600. ) {
              zerohour = true;
          }
	  if ( zerohour ) { 
              time1 = time1+1;
          }
	  long64 timestampT = long64( offset + 24.*60.*60.*(time1 - _ondotori2stamp[0]) + time2*time2toUnixtimefactor);
          timestampT -= _offsetTimeStamp[3]; // Ondotori Time is in JST 
#if debug
	  cout << " timestampT = " << timestampT << " time1 - _ondotori2stamp[0] = " << time1 - _ondotori2stamp[0]  << endl; 
#endif

	  //_templist.insert(map<long64, float>::value_type( timestampT, temperature_0 ) );
	  _templist.insert(pair<long64, float>( timestampT, temperature_0 ) );
          datacounter ++;
#if debugDeep
	  cout << " _templist.size() = " << _templist.size() << endl;
#endif


        if ( nEvent%1000 == 0 ){ 
          cout << "Temperature avrage:1:2 "  
               << temperature_0 << ", " << temperature_1 << ", " << temperature_2 << endl;
        }

        delete ScEcalTemper;
      }

      _TemperatureChanged = false;

  }  //The END of  ScECALTemperatureCol Element
#endif

#if debug
  if ( nEvent == 0 ) {
	cout << "datacounter = " << datacounter << endl;
  }
#endif

#if checkchange
  cout << "HELLO  _MIPConstantChanged 6=" << _dataYear  << " " << _TemperatureChanged << endl;
#endif

  if ( _dataYear == 2008 ) _TemperatureChanged = false;






  Double_t Esum=0;
  float temp_this_evt = 0;
  long64 time_this_evt = 0;  
//110712.1715.coterra

  long64 timestamp = evt->getTimeStamp();

  long timestamp1=timestamp/1000000000;
if ( _dataYear != 2008 ) { //TODO offset should be less than 2008/August
#if debug
  long timestampcorr = timestamp1 - offset;
  int day = timestampcorr/(60*60*24);
  int hour = (timestampcorr/(60*60))%24;
  int minute = (timestampcorr/60)%(60);
  int second = timestampcorr%60;
	cout << "timestamp= " << timestamp
	     << " timestamp1= " << timestamp1
	     << " tmiestamp2= " << timestamp%1000000000 
             << " dd/hh/mm/ss= " 
             << day << "/" << hour << "/" << minute << "/" << second 
             << " nEvent= " << nEvent << endl;
#endif
//  _wholeit--;
  std::map< long64, float >::iterator it = _templist.begin();
  _listReadCounter--;
  advance( it, _listReadCounter );

#if debug
 // cout << " _templist.size()= " << _templist.size() << endl;
#endif


 
  while( it != _templist.end() ) {

        ++_listReadCounter;

#if debug
//  cout << "timestamp1 = " << timestamp1 << " (*it).first = " << (*it).first 
//       << " (*it).second = " << (*it).second << endl;
#endif

	if ( timestamp1 < (*it).first ) {
// Temperature is measured every one minute, and 
// temperrature is recorded in Reduced....dat, only when the temperature changes.
// Therefore, record in just upper line of the time just over the stamp time
// is always also in less than one minute before at least.
// Therefore, we can take temperature with (--it) directly.
//	   long64 timeEnd = (*it).first;
//           float tempEnd = (*it).second;

           --it;

//	   long64 timeBegi = (*it).first;
//           float tempBegi = (*it).second;
//           double slope = ( tempEnd - tempBegi ) / ( timeEnd - timeBegi );
//           double temporaoffset = tempBegi - timeBegi * slope;

#if debug
//	cout << " timeEnd=" << timeEnd << " tempEnd=" << tempEnd 
//             << " timeBegi=" << timeBegi << " tempBegi=" << tempBegi 
//             << " slope=" << slope << " temporaoffset=" << temporaoffset 
//             << endl;
#endif

//	   temp_this_evt = temporaoffset + timestamp1 * slope;
	   temp_this_evt = (*it).second;
	   time_this_evt = (*it).first;
	   ++it;
#if debugDeep
  cout << "timestamp1 = " << timestamp1 << " (*it).first = " << (*it).first 
       << " (*it).second = " << (*it).second << endl;
#endif
	   break;
        }
        
        ++it;
  }
  if ( it==  _templist.end()  ) {
	   cout << "WARNING TEMPERATURE DATA PERIOD is MISMATCHING!!!" << endl;
	   return;
  }

} else {  // the end of if 2009
  temp_this_evt = 99999.; 
}
  if ( _forMIPCalibration ) 
  {
    evt->parameters().setValue( "temperatureThisEvent", temp_this_evt );
  }

#if debug
  cout << "time_this_evt = " << time_this_evt << " temp_this_evt = " << temp_this_evt << endl;
#endif

  try {
    lcio::LCCollection* inVector = evt->getCollection(_inputColName);
    lcio::LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    if (!_outputCol) throw runtime_error("IntegratedScECALCalibrationProcessor::processEvent(): couldn't create new collection");
    lcio::LCFlagImpl hitFlag(_outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_TIME);
    hitFlag.setBit(LCIO::CHBIT_ID1);
    _outputCol->setFlag(hitFlag.getFlag());
#if b110827a
  cout << "HERE_1" << endl;
#endif
    const TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));
    if (trigBits.isPurePedestalTrigger() && (_pedestalSubtraction==true)) {
#ifdef HCALRECO_DEBUG
    std::cout << "IHC: collecting pedestals" << std::endl;
#endif    
      for (unsigned i=0; i< static_cast<unsigned>(inVector->getNumberOfElements()); i++){
        CalorimeterHit* aCalorimeterHit = dynamic_cast<CalorimeterHit*>(inVector->getElementAt(i));
	if (!aCalorimeterHit) {
	  std::stringstream message;
	  message << "Collection " << _inputColName << " doesn't contain CalorimeterHits" << std::endl;
	  throw runtime_error(message.str());
	}  
#if b110827a
  cout << "HERE_2" << endl;
#endif

        //FastCaliceHit* hit = new FastCaliceHit(aRawCalorimeterHit);
        const unsigned short layer = aCalorimeterHit->getCellID0();
        const unsigned short strip = aCalorimeterHit->getCellID1();

        if ((layer > ScECAL_NLAYERS) || (strip > ScECAL_NSTRIPS)) {
#ifdef HCALRECO_DEBUG
          std::cout << "  invalid channel" << std::endl;
#endif	
          //delete hit;
	  continue;
	}  
        float ampl = aCalorimeterHit->getEnergy();
	//float Npe = 0;
	//#ifdef HCALRECO_DEBUG
        //std::cout << "IHC: old hit: " << mod << " " << cell << " " << hit->getEnergyValue() << std::endl;
	//#endif
        _pedSum[layer-1][strip-1] += ampl;
        _pedSumSquare[layer-1][strip-1] += pow(ampl,2);
        _pedNum[layer-1][strip-1]++;
        _ped[layer-1][strip-1] = _pedSum[layer-1][strip-1]/_pedNum[layer-1][strip-1];
	_pedError[layer-1][strip-1] = sqrt(_pedSumSquare[layer-1][strip-1]/_pedNum[layer-1][strip-1]-pow(_pedSum[layer-1][strip-1]/_pedNum[layer-1][strip-1],2));
	//delete hit;
      }
#if b110827a
  cout << "HERE_3" << endl;
#endif

      _pedCounter++;
      if (_skipPedestals!=false) throw marlin::SkipEventException(&aIntegratedScECALCalibrationProcessor);
    }
#if b110827a
  cout << "HERE_4 pedCounter " << _pedCounter << endl;
#endif


    if ((_pedCounter >= _minPedNumber) || (_pedestalSubtraction==false)) {
#if b110827a
  cout << "HERE_5" << endl;
#endif
     _eventCounter++;
      for (unsigned i = 0; i < static_cast<unsigned>(inVector->getNumberOfElements()); i++) {
        CalorimeterHit* aCalorimeterHit = dynamic_cast<CalorimeterHit*>(inVector->getElementAt(i));
	if (!aCalorimeterHit) {
	  std::stringstream message;
	  message << "Collection " << _inputColName << " doesn't contain CalorimeterHits" << std::endl;
	  throw runtime_error(message.str());
	}
        //FastCaliceHit* hit = new FastCaliceHit(aRawCalorimeterHit);
        const unsigned short layer = aCalorimeterHit->getCellID0();
        const unsigned short strip = aCalorimeterHit->getCellID1();
	//#ifdef HCALRECO_DEBUG
        //std::cout << "mod: " << mod << ", cell: " << cell << std::endl;
	//#endif	
        if ((layer > ScECAL_NLAYERS) || (strip > ScECAL_NSTRIPS)) {
#ifdef HCALRECO_DEBUG
          std::cout << "  invalid channel " << layer << "-" << strip << std::endl;
#endif	
          //delete hit;
	  continue;
	} 

        float ampl = aCalorimeterHit->getEnergy();
	//float amplErr = 0;//aCalorimeterHit->getEnergyError();
	if (ampl>30000) {
	  _saturationCounter++;
	  _saturations[layer-1]++;
	}
	//#ifdef HCALRECO_DEBUG
        //std::cout << "IHC: old hit: " << mod << " " << cell << " " << hit->getEnergyValue() << std::endl;
	//#endif
	if (_pedestalSubtraction==true) {
	  ampl -= _ped[layer-1][strip-1];
  	  float amplErr = std::sqrt(aCalorimeterHit->getEnergyError()*aCalorimeterHit->getEnergyError()+pow(_pedError[layer-1][strip-1],2));
          if (ampl < _significanceCut * amplErr) {
	    //#ifdef HCALRECO_DEBUG
            //std::cout << "  not significant" << std::endl;
	    //std::cout << ampl << " " << _significanceCut << " " << aCalorimeterHit->getEnergyError() << " " << _pedError[layer-1][strip-1] << std::endl;
	    //#endif
            //delete hit;
	    continue; 
	  }
	  //#ifdef HCALRECO_DEBUG
	  //std::cout << "IHC: new hit: " << mod << " " << cell << " " << ampl << std::endl;
	  //#endif
	}
	_hitCounter++;
	//cout << "CHECKING " << _outputCol->getNumberOfElements() << " " << inVector->getNumberOfElements() << endl;

	// ScECAL - perform MIP calibration

	// gain calibration

	//if (mipCalib ) {

	//if (gainCalib && interCalib) {
	////ampl = ampl * _intercalibconst[layer-1][strip-1];
	////ampl = ampl / _gaincalibconst[layer-1][strip-1];
	//}

	// Saturation correction
	//Npe = ampl * _intercalibconst[layer-1][strip-1] 
	//  / _gaincalibconst[layer-1][strip-1];
	//Npe_corrected = this->SaturationCorrection(Npe);
	//ampl = Npe_corrected / _intercalibconst[layer-1][strip-1] 
	// * _gaincalibconst[layer-1][strip-1];

	//9910.2033 saturation correction. coterra
	//The number of photon.
//110712.1619.coterra get temperature of this event

//        long64 timestampTempmeasure = ScEcalTemperature->gettimestamp();
//        float temp_this_evt = gatTempFrmDB

//110712.1542 coterra. temperature correction was temporally[?] 
	// implemented. temp_this_evt is gotten temp for this event.
        //_tempcorr_gain[2] is temp. that gain data was taken.

	float gainconst_tempcorr = _gaincalibconst[layer-1][strip-1];
        if (_tempcorrgainOn) {
           if (_uniformGainSlope[0] > 0.00001 ) { _gainSlope[layer-1][strip-1] = _uniformGainSlope[0];}
	   gainconst_tempcorr = gainconst_tempcorr
	                        - _gainSlope[layer-1][strip-1] / 100.     //note negative sign.
                                * ( temp_this_evt - _gainSetTemp[layer-1][strip-1] ); 
        } 

        float ampl_adc = ampl;

	ampl = ampl / 
	      ( gainconst_tempcorr / 
		_intercalibconst[layer-1][strip-1] );

#if enecheck
        cout << " ampl 1 = " << ampl 
             << ", gain(temp.corrected if required) = " << gainconst_tempcorr
             << ", intercalib = " << _intercalibconst[layer-1][strip-1]
             << ", mipcalib = " << _mipcalibconst[layer-1][strip-1] 
             << ", effNpix = " << _effNpix << endl;
#endif
	
	// saturation correction for the number of photon.
	Double_t ampld = - _effNpix * std::log( 1. - ampl / _effNpix );

	//convert to the ADCcounts.
	ampl = ampld*
	        ( _gaincalibconst[layer-1][strip-1] / 
  		  _intercalibconst[layer-1][strip-1] );
		
	// MIP calibration
//110712.1558 coterra. temperature correction for MIPconst 
// was temporally[?] implemented. temp_this_evt is gotte temp for this event.
//_tempcorr_gain[2] is temp. that gain data was taken.
        float mipconst_tempcorr = _mipcalibconst[layer-1][strip-1];
#if b110909
  cout << "START_MIPTEMPCORRE" << endl;
#endif
	if (_tempcorrmipOn ){
	   mipconst_tempcorr = mipconst_tempcorr
	                       + _mipSlope[layer-1][strip-1]
                               * ( temp_this_evt - _mipSetTemp[layer-1][strip-1] );
#if b110909
  cout << "MIPcon at " << _mipSetTemp[layer-1][strip-1] << " = " << _mipcalibconst[layer-1][strip-1]
       << ", Temp this event = " << temp_this_evt << " slope of [" << layer << "]["
       << strip << "] = " << _mipSlope[layer-1][strip-1] << " Corrected constant = " << mipconst_tempcorr 
       << " _effNpix:" << _effNpix << endl;
#endif
	}
	ampl = ampl / mipconst_tempcorr;

#if enecheck
        cout << " ampl 2 = " << ampl << endl;
#endif
	//}

	/*
	//  container to collect all default calibration objects (to be deleted at end of routine)
	std::set< LCHcalCalibrationObject* > tempCalibrationObjects;

        GainConstants* gainCalib = _gainCalibSet->getCalib(hit->getModuleID(), hit->getCellKey());
	if ( gainCalib == 0 ) {
	  gainCalib = new GainConstants( hit->getChip(), hit->getChannel(), 400, 200 );
	  tempCalibrationObjects.insert( gainCalib );
	}

	InterConstants* interCalib = _interCalibSet->getCalib(hit->getModuleID(), hit->getCellKey());
	if ( interCalib == 0 ){
	  interCalib = new InterConstants( hit->getChip(), hit->getChannel(), 10, 5 );
	  tempCalibrationObjects.insert( interCalib );
	}

	MIPConstants* mipCalib = _mipCalibSet->getCalib(hit->getModuleID(), hit->getCellKey());
	if ( mipCalib == 0 ){
	  mipCalib = new MIPConstants( hit->getChip(), hit->getChannel(), 100000, 100000 );
	  tempCalibrationObjects.insert( mipCalib );
	}

	//   heavy patch to finally utilize default calibrations...
	if ( _lightyield[layer-1][strip-1] == 0 ) {
	  _lightyield[layer-1][strip-1] = interCalib->applyCalibration(gainCalib->applyCalibration(mipCalib->cancelCalibration(1.)));
	}
	
	if (mipCalib && mipCalib->calibrationValid()) {
#ifdef HCALRECO_DEBUG
	  std::cout << "gainCalib: " << gainCalib << std::endl;
	  if(gainCalib) std::cout << "gainCalib calibrationValid()" << gainCalib->calibrationValid() << std::endl;
	  std::cout << "interCalib: " << interCalib << std::endl;
	  if(interCalib) std::cout << "interCalib->calibrationValid(): " << interCalib->calibrationValid() <<std::endl;
	  std::cout << "Lightyield[" << mod << "]" << "[" << cell << "]:" << _lightyield[layer-1][strip-1] << std::endl;
#endif
 	  if (gainCalib && interCalib && gainCalib->calibrationValid() && interCalib->calibrationValid() && (_lightyield[layer-1][strip-1]!=0)) {
	    ampl = interCalib->applyCalibration( gainCalib->applyCalibration(ampl) );
  	    ampl = ampl*getSiPmSaturationCorrection(mod, cell, ampl)/_lightyield[layer-1][strip-1];
// fixme
	    amplErr = 0;
	  } else {
	    _invalidSaturationCorrectionCounter++;
	    _invalidSaturationCorrections[mod]++;
	    if (_fudgeNonExistingSaturationCorrections==1) {
	      ampl = mipCalib->applyCalibration(ampl);
// fixme
	      amplErr = 0;
	    } else {
#ifdef HCALRECO_DEBUG
              std::cout << "  calibration not valid " << std::endl;
#endif	    
              delete hit;
              continue;
	    }
	  }    
	}
	*/

	/*   never happens logically...
	     else {
	     _invalidMIPCounter++;
	     _invalidMIPCalibrations[mod]++;
	     #ifdef HCALRECO_DEBUG
	     std::cout << "  mip calibration not valid" << std::endl;	    
	     #endif
	     delete hit;
	     continue;
	     }
	*/

#ifdef HCALRECO_DEBUG
        std::cout << "   " << ampl << std::endl;
#endif	
	if ((ampl < _mipCut) && _zeroSuppression) {
#ifdef HCALRECO_DEBUG
          std::cout << "  zero suppressed" << std::endl;
#endif	
	  /*
	//  delete all default calibration objects 
	for ( std::set< LCHcalCalibrationObject* >::iterator iter = tempCalibrationObjects.begin(); 
	      iter != tempCalibrationObjects.end(); ++iter ) {
	  delete *iter;
	}
	  */
          //delete hit;
	  continue;
	}  
//	FastCaliceHit* newHit = new FastCaliceHit(hit->getModuleID(), hit->getChip(), hit->getChannel(),
//					  ampl, amplErr, hit->getTimeStamp());
        /*
          mod is the module production number, 1-38
         */
	//FastCaliceHit* newHit = new FastCaliceHit(hit->getCellID(),
	//				  ampl, amplErr, hit->getTimeStamp());
        //delete hit;
	//CalorimeterHit* newHit = &aCalorimeterHit;//new CalorimeterHit(&aCalorimeterHit);
 
	Esum += ampl;

	CalorimeterHitImpl* newHit = new CalorimeterHitImpl();

	newHit->setCellID0(aCalorimeterHit->getCellID0());
	newHit->setCellID1(aCalorimeterHit->getCellID1());
        //To use this processor for MIP calbration, you should 
        // _forMIPCalibration = true; ( default setting is talse )
        // _pedestalSubtraction = true ( false OK if you want, default setting is true )
        // _zeroSuppression = false ( default setting is false )
        if ( _forMIPCalibration )
        {

          newHit->setEnergy( ampl_adc );
        } else {
	  newHit->setEnergy(ampl);
        }
	//newHit->setEnergy(adc_block.getAdcVal(kadc));
	newHit->setTime(timestamp);
	newHit->setPosition(aCalorimeterHit->getPosition());
	//_outputCol->addElement(newHit);   
 
	//cout << "FILLING " << _outputCol->getNumberOfElements() << endl;
	if (!newHit) throw runtime_error("Couldn't create new hit."); 
	_outputCol->addElement(newHit);

	/*
	//  delete all default calibration objects 
	for ( std::set< LCHcalCalibrationObject* >::iterator iter = tempCalibrationObjects.begin(); 
	      iter != tempCalibrationObjects.end(); ++iter ) {
	  delete *iter;
	}
	tempCalibrationObjects.clear();
	*/
      }
      evt->addCollection(_outputCol, _outputColName);
    }  
#if b110827a
  cout << "HERE_6" << endl;
#endif
   //_sumHist->Fill(Esum);
  }    

  catch (DataNotAvailableException &e) {
#ifdef HCALRECO_DEBUG
    std::cout << "IntegratedScECALCalibrationProcessor::processEvent(): data not available exception" << std::endl;
#endif
  }
#if b110827a
  cout << "HERE_7" << endl;
#endif

	
};


void IntegratedScECALCalibrationProcessor::end() {

  //TFile* ofile = new TFile("test.root","RECREATE");
  //_sumHist->Write();
  //ofile->Close();

  std::cout << "---  " << name() << " Report :" << std::endl;
  std::cout << "  processed " << _eventCounter << " events" << std::endl;
  //std::cout << "  processed " << _hitCounter << " hits (" 
  //    << std::fixed << std::setprecision(1) << (float) _hitCounter/ (float) _eventCounter << " per event)" << std::resetiosflags(std::ios::fixed) << std::setprecision(6) << std::endl;
  //std::cout << "  found " << _invalidMIPCounter << " hits with invalid MIP calibration (" 
  //    << std::fixed << std::setprecision(1) << (float) _invalidMIPCounter/ (float) _hitCounter*100 << "%, " 
  //<< std::fixed << std::setprecision(1) << (float) _invalidMIPCounter/ (float) _eventCounter << " per event) " << std::resetiosflags(std::ios::fixed) <<  std::setprecision(6) << std::endl; 
  //std::cout << std::fixed << std::setprecision(1) << "   per module: ";; 

  //for (std::map<unsigned,unsigned>::const_iterator it=_invalidSaturationCorrections.begin(); it!=_invalidSaturationCorrections.end(); it++) {
  //  std::cout << it->first << "/" << (float) it->second/ (float) _eventCounter << "  ";
  //}
  //std::cout << std::resetiosflags(std::ios::fixed) << std::setprecision(6) << std::endl;
  //std::cout << "  found " << _saturationCounter << " saturated hits (" 
  //      << std::fixed << std::setprecision(1) << (float) _saturationCounter/ (float) _hitCounter*100 << "%, " 
  //<< std::fixed << std::setprecision(1) << (float) _saturationCounter/ (float) _eventCounter << " per event) " << std::resetiosflags(std::ios::fixed) <<  std::setprecision(6) << std::endl; 
  //std::cout << std::fixed << std::setprecision(1) << "   per module: ";
  //for (std::map<unsigned,unsigned>::const_iterator it=_saturations.begin(); it!=_saturations.end(); it++) {
  //std::cout << it->first << "/" << (float) it->second/ (float) _eventCounter << "  ";
  //}
  //std::cout << std::resetiosflags(std::ios::fixed) << std::setprecision(6) << std::endl;
};


};
