#include "SimpleHcalCalibrationProcessor.hh"

#include <fstream>
#include <iostream>
#include <cmath>

#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <IMPL/LCCollectionVec.h>

#include <marlin/Exceptions.h>

#include <collection_names.hh>
#include <TriggerBits.hh>
#include <FastCaliceHit.hh>

//#define RECO_DEBUG 

namespace CALICE {

SimpleHcalCalibrationProcessor aSimpleHcalCalibrationProcessor;

SimpleHcalCalibrationProcessor::SimpleHcalCalibrationProcessor() : marlin::Processor("SimpleHcalCalibrationProcessor"){
  _description = "Simple all-in-one calibration for CALICE Hcal";

  registerProcessorParameter("InputFile",
			     "Name of flat file with calibration constants",
			     _inputFileName,
			     std::string("simpleHcalCalibration.dat"));

  registerProcessorParameter("InputCollection",
			     "Name of input collection of CaliceHits",
			     _inputColName,
			     std::string("RawCaliceHits"));

  registerProcessorParameter("OutputCollection",
			     "Name of output collection of CaliceHits",
			     _outputColName,
			     std::string("CalibratedCaliceHits"));

  registerProcessorParameter( "correction" , 
			      "chose between vasiliys and satorus correction formula"  ,
			      _correction ,
			      std::string("vasiliy") ) ;

  registerProcessorParameter( "Npix" , 
			      "Number of effectiv pixel" ,
			      _Npix ,
			      (float) 1200);              

   registerProcessorParameter( "X-talk" , 
			      "x-talk probability" ,
			      _xTalk ,
			      (float) 0.3);               

   registerProcessorParameter( "alpha" , 
			      "vasiliys correction paramerter" ,
			      _alpha ,
			      (float) 0.0010);               
 
  registerProcessorParameter( "maxPixel_Cut" , 
			      "maximum number of fired pixels alllowed" ,
			      _pixCut ,
			      (int) 3000);               


  _calcPed = true;			     
}


SimpleHcalCalibrationProcessor::~SimpleHcalCalibrationProcessor(){
}


void SimpleHcalCalibrationProcessor::init(){
  std::ifstream inFile;
  inFile.open(_inputFileName.c_str());
  if (inFile.is_open()){
    unsigned short mod, cell;
    float mip, mipEr, gain, gainEr, ic, icEr;
    unsigned short stat;
    while (inFile.good()){
      inFile >> mod >> cell >> mip >> mipEr >> gain >> gainEr >> ic >> icEr >> stat;
      if ((mod < 1) || (mod > HCAL_N_MOD))
        std::cout << "Array too small for module " << mod << "." << std::endl;
      if (cell >= HCAL_N_CELL)
        std::cout << "Array too small for cell " << cell << "." << std::endl;	
      _mip[mod][cell] = mip;
      _gain[mod][cell] = gain;
      _ic[mod][cell] = ic;
      _stat[mod][cell] = stat;
#ifdef RECO_DEBUG
   std::cout << mod << " " << cell << " " << mip << " " << gain << " " << ic << " " << stat << std::endl;
#endif      
    }
  }
  _pedEvents=0;
  _pedExists=false;
  printParameters();
}


void SimpleHcalCalibrationProcessor::processEvent(lcio::LCEvent *evt){
  LCCollection *inCol = evt->getCollection(_inputColName);
  if (inCol){
    TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));
    if (trigBits.isPurePedestalTrigger()) {
      if (!_calcPed) resetPedestal();
      sumupPedestal(inCol);
      _pedEvents++;
    }
    if (trigBits.isBeamTrigger() && _calcPed) calcPedestal();

    if (_pedExists) {
#ifdef RECO_DEBUG
      std::cout << "processing real beam event" << std::endl;
#endif

      LCCollectionVec *outCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);

      for (unsigned i=0; i< static_cast<unsigned>(inCol->getNumberOfElements()); i++){
	FastCaliceHit *hit = dynamic_cast<FastCaliceHit*>(inCol->getElementAt(i));
	unsigned short mod = (hit->getModuleID() & 0xFF00) >> 8;
	unsigned short cell = hit->getChip()*18 + hit->getChannel();
	
#ifdef RECO_DEBUG
        std::cout << "old hit: " << mod << " " << cell << " " << hit->getEnergyValue() << std::endl;
#endif
	if ((mod > HCAL_N_MOD+1) || (cell > HCAL_N_CELL)) continue;
	if (_stat[mod][cell] != 111) continue; //ignore cells with missing calibration
	float ampl = hit->getEnergyValue() - _ped[mod][cell];  //ampl in adc counts
//	float ampl = hit->getEnergyValue();
	float amplErr = 0;
	float N_pix = _Npix;    //number of effective pixel
	float xTalk = _xTalk;     //crosstalk

	//   correction factor
	float corr_vas;      // relative linearity correction factor
	float corrErr;   // uncertainty on relative correction
	//      correction with global factor on pixel level (involves gain and ic)
	
	//from satoru
	if(_correction == "satoru")
	  {
	    ampl = N_pix*std::log(2 * xTalk / (xTalk - 1. + sqrt(xTalk*xTalk + 2 * xTalk * (1.-2.*ampl*_ic[mod][cell]/_gain[mod][cell] /N_pix) +1.))); //corrected ampl in pix
	    
	    ampl = ampl*_gain[mod][cell]/_ic[mod][cell]/_mip[mod][cell]; //ampl in mip
	  }

	//from vasiliy
	else if(_correction == "vasiliy")
	  {	
	    corr_vas = 1./(1.-_alpha*ampl*_ic[mod][cell]/_gain[mod][cell]);

	    ampl /= _mip[mod][cell];   // amplitude in uncorrected MIP
	    ampl *= corr_vas;

	    float cutAmpl = _pixCut*_gain[mod][cell]/_ic[mod][cell]/_mip[mod][cell];
	    if (ampl > cutAmpl) ampl = cutAmpl;
	  }

	else
	  ampl = ampl;

	corrErr = 0;

	if (ampl < 0.5) continue;  // ignore hits<0.5MIP

	float mipErr = .05*_mip[mod][cell];  // relative mip uncertainty (global)
	amplErr = ampl * sqrt(mipErr*mipErr + corrErr*corrErr);

	//   create new CaliceHit and append to output collection
	FastCaliceHit* newHit = new FastCaliceHit(hit->getModuleID(), hit->getChip(), hit->getChannel(),
					  ampl, amplErr, hit->getTimeStamp());
#ifdef RECO_DEBUG
        std::cout << "new hit: " << mod << " " << cell << " " << ampl << std::endl;
#endif
	outCol->addElement(newHit);
      }
      evt->addCollection(outCol,_outputColName);
    }
  }

}



void SimpleHcalCalibrationProcessor::end(){

  if(_correction=="vasiliy")
    std::cout<<"used correction from vasiliy with correctionfactor "<<_alpha<<std::endl;
  else if(_correction=="satoru")
    std::cout<<"used correction from satoru with "<<_Npix<<" pixels and "<<_xTalk*100<<"% crosstalk probability"<<std::endl;
  else 
    std::cout<<"! uncorected data !"<<std::endl;
}



void SimpleHcalCalibrationProcessor::resetPedestal(){
#ifdef RECO_DEBUG
  std::cout << "resetting pedestal" << std::endl;
#endif
  for (unsigned short mod=0; mod<HCAL_N_MOD+1; mod++){
    for (unsigned short cell=0; cell<HCAL_N_CELL; cell++){
      _pedSum[mod][cell] = 0;
      _pedSumSquare[mod][cell] = 0;
      _pedNum[mod][cell] = 0;
    }
  }
  _calcPed = true;
}



void SimpleHcalCalibrationProcessor::sumupPedestal(LCCollection *col){
#ifdef RECO_DEBUG
  std::cout << "summing up pedestal" << std::endl;
#endif
  for (unsigned i=0; i < static_cast<unsigned>(col->getNumberOfElements()); i++){
    FastCaliceHit *hit = dynamic_cast<FastCaliceHit*>(col->getElementAt(i));
    unsigned short mod = (hit->getModuleID() & 0xFF00) >> 8;
    unsigned short cell = hit->getChip()*18 + hit->getChannel();
    float ampl = hit->getEnergyValue();
    _pedSum[mod][cell] += ampl;
    _pedSumSquare[mod][cell] += ampl*ampl;
    _pedNum[mod][cell]++;
  }
}



void SimpleHcalCalibrationProcessor::calcPedestal(){
#ifdef RECO_DEBUG
  std::cout << "calculating pedestal" << std::endl;
#endif
  for (unsigned short mod=1; mod<HCAL_N_MOD+1; mod++){
    for (unsigned short cell=0; cell<HCAL_N_CELL; cell++){
      if (_pedNum[mod][cell]!=0) _ped[mod][cell] = _pedSum[mod][cell]/_pedNum[mod][cell];
      //      _ped[mod][cell] = sqrt( _pedSumSquare[mod][cell] - _pedSum[mod][cell]*_ped[mod][cell]);
      //      if (_pedNum[mod][cell]>0){
      //	_ped[mod][cell] /= (float)_pedNum[mod][cell];
      //      }else{
      //	_ped[mod][cell] = 33000;
      //      }
    }
  }
  _calcPed = false;
  _pedExists = true;
}

}
