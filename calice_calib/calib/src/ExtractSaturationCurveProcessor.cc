#include "ExtractSaturationCurveProcessor.hh"

#include <assert.h>
#include <cmath>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TROOT.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "EVENT/LCCollection.h"
#include "marlin/ConditionsProcessor.h"
#include "marlin/Exceptions.h"
#include "ConditionsChangeDelegator.hh"

#include "collection_names.hh"
#include "HcalTempModel.hh"
#include "CalibrationSet.hh"
#include "GainConstants.hh"
#include "InterConstants.hh"

/*   CALICE includes*/
#include "AhcConditions.hh"
#include "AdcBlock.hh"
#include "MappingProcessor.hh"

using namespace marlin;
using namespace lcio;
using namespace CALICE;

ExtractSaturationCurveProcessor aExtractSaturationCurveProcessor;

/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/
ExtractSaturationCurveProcessor::ExtractSaturationCurveProcessor() :
  marlin::Processor("ExtractSaturationCurveProcessor") {

  _description = "2010/08/24 - Jara: Processor extracting saturation curves from ahc-Pm/Cm-Led - runs";

  registerProcessorParameter("InputCollectionName",
			     "Name of the input collection",
			     _inColName,
			     string("CALDAQ_ADCCol"));

  registerProcessorParameter("ConditionsCollectionName",
			     "Collection of AhcConditions objects",
			     _condColName,
			     string("AhcConditions"));

  registerProcessorParameter("RootOutputFileName",
			     "ROOT file to collect TGraphErrors",
			     _outFileName,
			     string("defaultGraphs.root"));

  registerProcessorParameter("DatOutputFileName",
			     "flatfile containing fit results",
			     _datFileName,
			     string("saturation_fit_output.dat"));

  registerProcessorParameter("TypeOfLedScan",
			     "Type of LED mode: 1375 is PmMode-run / 472 is PmMode-run",
			     _ledType,
			     1375);

  registerProcessorParameter("SubtractPedestal",
			     "(false)-without pedestal subtraction, (true)-subtract pedestal",
			     _subtractPedestal,
			     true);

  registerProcessorParameter("DoFit",
			     "(false)-without fit, (true)-do fit",
			     _doFit,
			     true);

  registerProcessorParameter("NumberPointsUsed",
			     "The number of the points used for the fit (counting from the back)",
			     _nPointsUsed,
			     10);

  registerProcessorParameter("MappingProcessorName",
			     "Name of the MappingProcessor",
                             _mappingProcessorName,
			     string("AhcMappingProcessor"));
  
}

/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/
void ExtractSaturationCurveProcessor::init() {
  printParameters();
   
  std::stringstream message;
  message << "undefined conditionsdata: ";
  bool error = false;

  /*-------------------------------------------------------------*/
  _ahcMapper = dynamic_cast<const AhcMapper*>(MappingProcessor::getMapper(_mappingProcessorName));

  if ( ! _ahcMapper ) {
    message << "MappingProcessor::getMapper("<< _mappingProcessorName
	    << ") did not return a valid mapper." << endl;
    error = true;
  }
  
  if (error) {
    streamlog_out(ERROR) << message.str();
    throw marlin::StopProcessingException(this);
  }
  /*-------------------------------------------------------------*/
  
  _ahcMaxNumberOfModules  = _ahcMapper->getMaxModule();
  _ahcMaxNumberOfChips    = _ahcMapper->getMaxChip();
  _ahcMaxNumberOfChannels = _ahcMapper->getMaxChannel();
  _ahcMaxNumberOfCells    = _ahcMaxNumberOfChannels * _ahcMaxNumberOfChips;
  _mapperVersion = _ahcMapper->getVersion();

  std::cout <<"Init: Max number of Modules="<<_ahcMaxNumberOfModules
       <<" Chips="<<_ahcMaxNumberOfChips
       <<" Channels="<<_ahcMaxNumberOfChannels
       <<" Cells="<<_ahcMaxNumberOfCells
       <<std::endl;
  std::cout <<"Mapper version is "<<_mapperVersion
       <<std::endl;
}


/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/
void ExtractSaturationCurveProcessor::updateMapper() {
  _ahcMaxNumberOfModules  = _ahcMapper->getMaxModule();
  _ahcMaxNumberOfChips    = _ahcMapper->getMaxChip();
  _ahcMaxNumberOfChannels = _ahcMapper->getMaxChannel();
  _ahcMaxNumberOfCells    = _ahcMaxNumberOfChannels * _ahcMaxNumberOfChips;
  _mapperVersion          = _ahcMapper->getVersion();

}

/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/
void ExtractSaturationCurveProcessor::processEvent(LCEvent* evt) {
  if (_mapperVersion != _ahcMapper->getVersion()) {
    streamlog_out(DEBUG0) <<" Mapper version changed..."<<std::endl;
    this->updateMapper();

    std::cout <<"updateMapper: Max number of Modules="<<_ahcMaxNumberOfModules
	 <<" Chips="<<_ahcMaxNumberOfChips
	 <<" Channels="<<_ahcMaxNumberOfChannels
	 <<" Cells="<<_ahcMaxNumberOfCells
	 <<std::endl;
    std::cout <<"Mapper version is "<<_mapperVersion
	 <<std::endl;
  }

  int eventNumber = evt->getEventNumber();


  ////////////////////// LC Collection /////////////////////////

  LCCollection *condCol = 0;
  LCCollection *inCol = 0;

  try {
    condCol = evt->getCollection(_condColName.c_str());
  }
  catch (DataNotAvailableException err) {
    streamlog_out(DEBUG0)<<"Collection "<< _condColName<<" not found in event "
		 <<eventNumber<<"-- skipping event\n"<<std::endl;
    return;
  }

  try{
    inCol = evt->getCollection(_inColName.c_str());
  }
  catch (DataNotAvailableException err) {
    streamlog_out(DEBUG0)<<"Collection "<< _inColName<<" not found in event "
		 <<eventNumber<<"-- skipping event\n"<<std::endl;
    return;
  }

  streamlog_out(DEBUG0)<<"\n\n Collection "<<_condColName<<" has "<<condCol->getNumberOfElements()
	       <<" elements"<<std::endl;


  ////////////////////// Conditions DATA /////////////////////////

  for (int i = 0; i < condCol->getNumberOfElements(); i++) {
    AhcConditions *cond = dynamic_cast<AhcConditions*>(condCol->getElementAt(i));
    if (cond == NULL) {
      streamlog_out(DEBUG0)<<"Collection "<<_condColName
		   <<" does not contain AhcConditions objects -- skiping event"<<std::endl;
      return;
    }
    
    bool isValid = false;
    unsigned int mod = _ahcMapper->getModuleFromModuleID(cond->getModuleID(), isValid);
    if (!isValid) continue;

    condpoint newpoint;
    newpoint.vcalib = cond->getVcalib();
    streamlog_out(DEBUG0)<<"Vcalib: "<<cond->getVcalib()
		 <<" module: "<<mod
		 <<std::endl;

    if ( (eventNumber%1000) == 0) {
    	std::cout <<"Event="<<eventNumber
	   <<" Vcalib: "<<cond->getVcalib()
	   <<" module: "<<mod
	   <<std::endl;
    }
      
    for (int j = 0; j < 12; j++) {
      newpoint.shiftreg[j] = cond->getSR(j);
    }
    _cond[mod] = newpoint;
      
  }

  streamlog_out(DEBUG0)<<"\n\n Collection "<<_inColName<<" has "
	       <<inCol->getNumberOfElements()<<" elements"<<std::endl;



  ////////////////////// ADC DATA /////////////////////////

  unsigned short cell = 0;

  for (int i = 0; i < inCol->getNumberOfElements(); i++) {
    LCObject *obj = inCol->getElementAt(i);
    AdcBlock adcBlock(obj);
    
    short crate   = adcBlock.getCrateID();
    short slot    = adcBlock.getSlotID();
    short fe      = adcBlock.getBoardFrontEnd();
    short channel = adcBlock.getMultiplexPosition();

    streamlog_out(DEBUG0)<<"    crate/slot/fe/channel="<<crate<<"/"<<slot<<"/"<<fe<<"/"<<channel<<std::endl;

    for (unsigned short chip = 0; chip < _ahcMaxNumberOfChips; ++chip) {
      bool isValid;
      const int daqChannelID = _ahcMapper->getDecoder()->getDAQID(crate, slot, fe, chip, channel);
      const unsigned int module = _ahcMapper->getModuleFromDAQID(daqChannelID, isValid);
      if ( ! isValid ) continue;
	  
      cell = chip * 18 + channel;
      assert( (module < _ahcMaxNumberOfModules) && (cell < _ahcMaxNumberOfCells));
  
      unsigned short moduleID = _ahcMapper->getDecoder()->getModuleID(module, chip, channel);

      float ampl = (float)adcBlock.getAdcVal(chip);

      unsigned vc = _cond[module].vcalib;
      unsigned sr = _cond[module].shiftreg[chip];
      unsigned lt = (unsigned)_ledType; 
      
      if (sr == lt) { /*SR 1375 is PmMode-run / SR 472 is PmMode-run*/
	_data[daqChannelID][vc].sumPm  += ampl;
	_data[daqChannelID][vc].sum2Pm += (ampl*ampl);
	_data[daqChannelID][vc].nPm    += 1;
	_data[daqChannelID][vc].modID  = moduleID;
	if (ampl > 32766) _data[daqChannelID][vc].noOverAdc += 1; 
	
      }/*end of if srr == _ledType*/
      
    }/*end loop over chip*/
    
  } /*------------------ end loop over AdcBlocks ----------------*/
  
}


/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/

void ExtractSaturationCurveProcessor::end() {
  TFile fout(_outFileName.c_str(), "recreate");
  if (!fout.IsOpen()) {
    streamlog_out(ERROR0)<<" Could not open ROOT file "<<_outFileName<<std::endl;
    exit (1);
  }

  FILE * pFile;
  pFile = fopen (_datFileName.c_str(),"w");

  if (pFile == NULL) {
    streamlog_out(ERROR)<<"\n\n Cannot open file "<<_datFileName<<"\n"<<std::endl;
    exit(1);
  }


  /*loop over all channels*/
  for (map<unsigned, map<unsigned, datapoint> >::const_iterator iterOut = _data.begin(); 
       iterOut != _data.end(); iterOut++)  {
    map<unsigned, datapoint> cmap = (*iterOut).second;
    unsigned daqChannelID = (*iterOut).first;
    bool first        = true;
    double pedPm      = 0.;
    double pedPmCount = 0.;
    int module_ID     = -999;

    /*collect data for this channel in vectors*/
    vector<double> pm_signal_value;
    vector<double> pm_signal_error;
    vector<double> vcalib_value;
    vector<double> vcalib_error;
    double vcalib_err = 0.0;
    unsigned short VcalibOverAdc = 0;
    unsigned short VcalibRMSOverAdc = 0;
    unsigned short OverAdc = 0;

    for (map<unsigned,datapoint>::const_iterator iterIn = cmap.begin(); iterIn != cmap.end(); iterIn++) {
      datapoint cdat = (*iterIn).second;

      if (cdat.nPm < 100) continue;
      
      unsigned vc = (*iterIn).first;

      if (first && cdat.nPm > 0) {
	if (_subtractPedestal ) pedPm = cdat.sumPm / cdat.nPm;
	first = false;
	module_ID = cdat.modID;
	pedPmCount = cdat.sumPm / cdat.nPm;
      }
	
      double meanPm = -999, RMS2Pm = -999;
      
      if (cdat.nPm > 0) {
	meanPm = cdat.sumPm / cdat.nPm - pedPm;
	RMS2Pm = (cdat.sum2Pm/cdat.nPm - (cdat.sumPm/cdat.nPm)*(cdat.sumPm/cdat.nPm)) / cdat.nPm;
	if (meanPm > 32766) VcalibOverAdc++;
	if ((meanPm + sqrt(RMS2Pm))  > 32766) VcalibRMSOverAdc++;
	OverAdc += cdat.noOverAdc;
      }

      if (meanPm >= 0.0) {
	pm_signal_value.push_back(meanPm);
	pm_signal_error.push_back( sqrt(RMS2Pm) );
	vcalib_value.push_back(vc);
	vcalib_error.push_back(vcalib_err);
      }
    }
    
    double parameter_0 = -99999;
    double parameter_1 = 0.001;
    double parameter_2 = -99999;
    double error_0     = -99.999;
    double probability = -9.999;

    unsigned int module  = _ahcMapper->getModuleFromDAQID(daqChannelID);
    unsigned int chip    = _ahcMapper->getDecoder()->getChipFromDAQID(daqChannelID);
    unsigned int channel = _ahcMapper->getDecoder()->getChannelFromDAQID(daqChannelID); 

    /*create TGraphErrors from vectors*/
    char graph_title[40]; 
    sprintf(graph_title, "module%i_chip%i_channel%i", module, chip, channel);

    std::cout << "Graph Title: " << graph_title
	 << "  Vector size=" << vcalib_value.size()
	 << std::endl;
    
    TGraphErrors graph_i;

    if(_doFit && vcalib_value.size() > (unsigned int)_nPointsUsed) {
      graph_i = TGraphErrors(vcalib_value.size(), &(vcalib_value[0]), 
			     &(pm_signal_value[0]), &(vcalib_error[0]), 
			     &(pm_signal_error[0]));
      
      /*fit last points of curve*/
      double Rmin = vcalib_value[vcalib_value.size()-_nPointsUsed] - 1;
      double Rmax = vcalib_value[vcalib_value.size()-1] + 1;
       
      if ( Rmax > Rmin ) {
	TF1 *fitFunction = new TF1("fitFunction","[0] * ( 1-exp( -(x+[2]) * [1]))",Rmin,Rmax);
	
	/*guess starting values for fits:*/
	double guess_0 = pm_signal_value[(vcalib_value.size() - 1)];
	double guess_1 = 2.0 / guess_0;
	double guess_2 = -30000;
	
	/*do fit, repeat it until probability in accepted range or maximum number of iterations reached, now twice as default */
	for(int i = 0; i < 2; i++) {
	  fitFunction->SetParameter(0, guess_0);
	  fitFunction->SetParameter(1, guess_1);
	  fitFunction->SetParameter(2, guess_2);
	  
	  graph_i.Fit("fitFunction", "RMQ");
	  
	  parameter_0 = fitFunction->GetParameter(0);
	  parameter_1 = fitFunction->GetParameter(1);
	  parameter_2 = fitFunction->GetParameter(2);
	  error_0 = fitFunction->GetParError(0);
	  probability = fitFunction->GetProb();
	  
	  guess_0 = parameter_0;
	  guess_1 = parameter_1;
	  guess_2 = parameter_2;
	}
      } else {
    	  std::cout << " Problems with Rmin, Rmax or Vcalib values "<<Rmin<<" "<<Rmax<<std::endl;
    	  std::cout << "  Vector size=" << vcalib_value.size()<<std::endl;

	for (unsigned int iv = 0; iv < vcalib_value.size(); iv++) {
		std::cout<<"iv="<<iv<<"  Vcalib="<<vcalib_value[iv]<<std::endl;
	}
      }
    } else {
      graph_i = TGraphErrors(vcalib_value.size(), &(vcalib_value[0]), 
			     &(pm_signal_value[0]), &(vcalib_error[0]), 
			     &(pm_signal_error[0]));
    }


    /*check if any INFinities or NANs occur in results*/
    if( std::isnan(parameter_0) != 0 || std::isinf(parameter_0) != 0 || std::isnan(error_0) != 0 || std::isinf(error_0) != 0 ) {
      parameter_0 = -99999;
      parameter_1 = 0.001;
      parameter_2 = -99999;
      error_0     = -99.999;
      probability = -9.999;
    }

    float SignalVcalibMax;
    if (vcalib_value.size() > 0) {
      SignalVcalibMax = pm_signal_value[(vcalib_value.size() - 1)];
    } else {
      SignalVcalibMax = 0;
      std::cout <<" !!!! Vcalib size vector is 0 -> SignalVcalibMax = 0"<<std::endl;
    }

    fprintf (pFile, "%3d %3d %3d %9.1f %7.3f %5.3f %9.1f %7.3f %7.1f %3d %3d %5d %7.1f\n", 
	     module, chip, channel, 
	     parameter_0, error_0, probability, parameter_2, 
	     parameter_1*parameter_0, pedPmCount, VcalibOverAdc, 
	     VcalibRMSOverAdc, OverAdc, SignalVcalibMax);

    
    graph_i.SetName(graph_title);
    graph_i.Write();
    
  }

  fclose (pFile);
  fout.Close();
  
};




