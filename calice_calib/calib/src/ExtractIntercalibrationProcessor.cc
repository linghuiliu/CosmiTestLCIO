#include "ExtractIntercalibrationProcessor.hh"

#include <fstream>
#include <math.h>
#include <assert.h>

#include "EVENT/LCCollection.h"
#include "marlin/Exceptions.h"

/*   CALICE includes*/
#include "AhcConditions.hh"
#include "AdcBlock.hh"
#include "MappingProcessor.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;

ExtractIntercalibrationProcessor aExtractIntercalibrationProcessor;

/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/
ExtractIntercalibrationProcessor::ExtractIntercalibrationProcessor() :
  marlin::Processor("ExtractIntercalibrationProcessor") {
 
  _description = "Jara 2010/12/15: Processor extracting Intercalibration constants from ahcPmLed & ahcCmLed - runs";

  registerProcessorParameter("InputCollectionName",
			     "Name of the input collection",
			     _inColName,
			     string("CALDAQ_ADCCol"));

  registerProcessorParameter("ConditionsCollectionName",
			     "Collection of AhcConditions objects",
			     _condColName,
			     string("AhcConditions"));

  registerProcessorParameter("MappingProcessorName",
                             "Name of the MappingProcessor",
                             _mappingProcessorName,
                             string("AhcMappingProcessor"));

  registerProcessorParameter("OutputFileName",
                             "ASCII file to write results",
                             _outFileName,
                             string("intercalibration.dat"));

}


/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/

void ExtractIntercalibrationProcessor::init() {
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

  cout <<"Init: Max number of Modules="<<_ahcMaxNumberOfModules
       <<" Chips="<<_ahcMaxNumberOfChips
       <<" Channels="<<_ahcMaxNumberOfChannels
       <<" Cells="<<_ahcMaxNumberOfCells
       <<endl;
  cout <<"Mapper version is "<<_mapperVersion
       <<endl;
}


/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/

void ExtractIntercalibrationProcessor::updateMapper() {
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

void ExtractIntercalibrationProcessor::processEvent(LCEvent* evt) {
  if (_mapperVersion != _ahcMapper->getVersion()) {
    streamlog_out(DEBUG0) <<" Mapper version changed..."<<std::endl;
    this->updateMapper();
    
    cout <<"updateMapper: Max number of Modules="<<_ahcMaxNumberOfModules
	 <<" Chips="<<_ahcMaxNumberOfChips
	 <<" Channels="<<_ahcMaxNumberOfChannels
	 <<" Cells="<<_ahcMaxNumberOfCells
	 <<endl;
    cout <<"Mapper version is "<<_mapperVersion
	 <<endl;
  }
 
  int eventNumber = evt->getEventNumber();


  ////////////////////// LC Collections /////////////////////////

  LCCollection *condCol = 0;
  LCCollection *inCol = 0;

  try {
    condCol = evt->getCollection(_condColName);
  }
  catch (DataNotAvailableException err) {
    streamlog_out(DEBUG0)<<"Collection "<< _condColName<<" not found in event "
		 <<eventNumber<<"-- skipping event\n"<<endl;
    return;
  }

  try{
    inCol = evt->getCollection(_inColName);
  }
  catch (DataNotAvailableException err) {
    streamlog_out(DEBUG0)<<"Collection "<< _inColName<<" not found in event "
		 <<eventNumber<<"-- skipping event\n"<<endl;
    return;
  }


  streamlog_out(DEBUG0)<<"\n\n Collection "<<_condColName<<" has "<<condCol->getNumberOfElements()
	       <<" elements"<<std::endl;
 

  ////////////////////// Conditions DATA /////////////////////////
 
  for (int i = 0; i < condCol->getNumberOfElements(); i++) {
    AhcConditions *cond = dynamic_cast<AhcConditions*>(condCol->getElementAt(i));
    if (cond == NULL) {
      streamlog_out(DEBUG0)<<"Collection "<<_condColName
		   <<" does not contain AhcConditions objects -- skiping event"<<endl;
      return;
    }
    
    bool isValid = false;
    unsigned int mod = _ahcMapper->getModuleFromModuleID(cond->getModuleID(), isValid);
    if (!isValid) continue;
    condpoint newpoint;
    newpoint.vcalib = cond->getVcalib();
    streamlog_out(DEBUG0)<<"Vcalib: "<<cond->getVcalib()
		 <<" module: "<<mod
		 <<endl;
      
    if ( (eventNumber%1000) == 0) {
      cout <<"Event="<<eventNumber
	   <<" Vcalib: "<<cond->getVcalib()
	   <<" module: "<<mod
	   <<endl;
    }
      
    for (int j = 0; j < 12; j++) {
      newpoint.shiftreg[j] = cond->getSR(j);
      //streamlog_out(DEBUG0)<<"   shift register: "<<cond->getSR(j)<<endl;
    }    
    _cond[mod] = newpoint;
  
  }

  streamlog_out(DEBUG0)<<"\n\n Collection "<<_inColName<<" has "
	       <<inCol->getNumberOfElements()<<" elements"<<endl;


  ////////////////////// ADC DATA /////////////////////////

  unsigned short cell = 0;
  
  for (int i = 0; i < inCol->getNumberOfElements(); i++) {
    LCObject *obj = inCol->getElementAt(i);
    AdcBlock adcBlock(obj);
      
    short crate   = adcBlock.getCrateID();
    short slot    = adcBlock.getSlotID();
    short fe      = adcBlock.getBoardFrontEnd();
    short channel = adcBlock.getMultiplexPosition();

    for (unsigned short chip = 0; chip < _ahcMaxNumberOfChips; ++chip) {
      bool isValid;
      const int daqChannelID    = _ahcMapper->getDecoder()->getDAQID(crate, slot, fe, chip, channel);
      const unsigned int module = _ahcMapper->getModuleFromDAQID(daqChannelID, isValid);
      if ( ! isValid ) continue;
	  
      cell = chip * 18 + channel;
      assert( (module < _ahcMaxNumberOfModules) && (cell < _ahcMaxNumberOfCells));
  
      float ampl = (float)adcBlock.getAdcVal(chip);
  
      unsigned vc = _cond[module].vcalib;
      unsigned sr = _cond[module].shiftreg[chip];
 	  
      if (sr==472) {
	//streamlog_out(DEBUG0)<<" sr="<<sr<<" vc="<<vc<<std::endl;
	_data[daqChannelID][vc].sumCm += ampl;
	_data[daqChannelID][vc].sum2Cm += (ampl*ampl);
	_data[daqChannelID][vc].nCm += 1;

      } else if (sr==1375) {
	//streamlog_out(DEBUG0)<<" sr="<<sr<<" vc="<<vc<<std::endl;
	_data[daqChannelID][vc].sumPm += ampl;
	_data[daqChannelID][vc].sum2Pm += (ampl*ampl);
	_data[daqChannelID][vc].nPm += 1;
      }
	  
    } /*end loop over chip*/

    
  } /*------------------ end loop over AdcBlocks ----------------*/

}


/******************************************************************/
/*                                                                */
/*                                                                */
/*                                                                */
/******************************************************************/

void ExtractIntercalibrationProcessor::end() {

  FILE * pFile;
  pFile = fopen (_outFileName.c_str(),"w");

  streamlog_out(DEBUG0)<<"\n\n\n Open file "<<_outFileName<<endl;

  for (map<unsigned, map<unsigned, datapoint> >::const_iterator iterOut = _data.begin(); 
       iterOut != _data.end(); iterOut++) {
    
    map<unsigned, datapoint> cmap = (*iterOut).second;
    const int daqChannelID = (*iterOut).first;
    bool first = true;
    double pedPm = 99999;
    double pedCm = 99999;
    double sumw = 0, sum = 0, sum2 = 0;
    int n = 0;

    double minIC = 0., maxIC = 0.;
    double maxW = 0.; 
    unsigned maxWvc = 0;
    unsigned maxn = 0;
    
    for (map<unsigned, datapoint>::const_iterator iterIn = cmap.begin(); iterIn != cmap.end(); iterIn++) {
      datapoint cdat = (*iterIn).second;

      unsigned vc = (*iterIn).first;

      if (cdat.nPm<100 || cdat.nCm<100) continue;

      streamlog_out(DEBUG1)<<"vc="<<vc<<"  --> nPm="<<cdat.nPm<<" sumPm="<<cdat.sumPm<<" nCm="<<cdat.nCm<<" sumCm="<<cdat.sumCm<<endl;
	
      if (first) {
	pedPm = cdat.sumPm / cdat.nPm;
	pedCm = cdat.sumCm / cdat.nCm;
	first = false;
      }
	
      double meanPm = -999, RMS2Pm = -999;
      meanPm = cdat.sumPm / cdat.nPm - pedPm;
      RMS2Pm = cdat.sum2Pm/cdat.nPm - (cdat.sumPm/cdat.nPm)*(cdat.sumPm/cdat.nPm);

      double meanCm = -999, RMS2Cm = -999;
      meanCm = cdat.sumCm / cdat.nCm - pedCm;
      RMS2Cm = cdat.sum2Cm/cdat.nCm - (cdat.sumCm/cdat.nCm)*(cdat.sumCm/cdat.nCm);

      streamlog_out(DEBUG1)
	<<"meanPm="<<meanPm<<" pedPm="<<pedPm<<" RMS2Pm="<<RMS2Pm
	<<" meanCm="<<meanCm<<" pedCm="<<pedCm<<" RMS2Cm="<<RMS2Cm 
	<<endl;

      double w = 0;
      double Ci = 0; double Pi = 0; 
      double eCi2 = 0;        
      double ePi2 = 0;
      double ICi = 0;
      
      if (meanPm > 200 && meanCm > 1000 && meanCm < 30000) {
	if (RMS2Pm>0 && RMS2Cm>0) {
	
	  Ci = meanCm; Pi = meanPm; // mean values for each point
	  eCi2 = RMS2Cm / cdat.nCm; // square of errors of averages        
	  ePi2 = RMS2Pm / cdat.nPm; // Err = rms/sqrt(n)
	  ICi = Ci / Pi; // IC for point = ratio of means

	  //	  w = sqrt(1./(RMS2Pm + RMS2Cm));
	  // new counting of weights from error propagation
	  w = 1 / (ICi * sqrt( eCi2/(Ci*Ci) + ePi2/(Pi*Pi) ));

	  n++;
	  sumw += w;
	  sum  += w * ICi;
	  sum2 += w * ICi*ICi;

	  if (n==1) {
	    minIC = meanCm/meanPm; maxIC = minIC;
	    maxW = w; maxWvc = vc;
	  } else {
	    double tempIC = meanCm/meanPm;
	    if (tempIC < minIC) minIC = tempIC;
	    if (tempIC > maxIC) maxIC = tempIC;
	    if (w > maxW) { maxW = w; maxWvc = vc; maxn = n;}
	  }
	}
      }
    
      streamlog_out(DEBUG1)
	<<" ICi="<<ICi<<" eCi2="<<eCi2<<" ePi2="<<ePi2<<" w="<<w
	<<"  n="<<n<<" sumw="<<sumw<<" sum="<<sum<<" sum2="<<sum2
	<<endl;
    } /*------------------- end loop over iterIn Vcalib (vc)  ------------------------*/
    
    double ic = -99.9, icErr = -9.99;
    
    if (n > 0) {
      ic = sum/sumw;
      icErr = sqrt(sum2/sumw - sum*sum/(sumw*sumw));
      icErr /= sqrt(n);    // to get error of mean over n runs
    }

    
    unsigned int module  = _ahcMapper->getModuleFromDAQID(daqChannelID);
    unsigned int chip    = _ahcMapper->getDecoder()->getChipFromDAQID(daqChannelID);
    unsigned int channel = _ahcMapper->getDecoder()->getChannelFromDAQID(daqChannelID); 

    streamlog_out(DEBUG1)<<"   module/chip/channel="<<module<<"/"<<chip<<"/"<<channel<<endl;

    fprintf (pFile, " %2d %2d %2d  %7.3f %6.3f %2d  %6.2f %6.2f %5d  %2d %6.2f  %6.1f %6.1f\n",
	     module,chip,channel,ic,icErr,n,
	     minIC, maxIC, maxWvc, maxn, maxW, pedPm, pedCm);


  } /*---------------- end loop over iterOut (DAQ channels M/A/C --------------------*/


   fclose (pFile);
}



