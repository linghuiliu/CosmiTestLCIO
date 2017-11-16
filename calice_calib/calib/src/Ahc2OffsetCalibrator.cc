#include "Ahc2OffsetCalibrator.hh"

//ROOT
#include <TStyle.h>
#include <TGraph.h>
#include <TFile.h>
#include <TAxis.h>

//lcio
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "UTIL/LCTime.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "IMPL/LCCollectionVec.h"

//marlin
#include <marlin/ProcessorMgr.h>
#include "marlin/Exceptions.h"

#include <fstream>
#include <time.h>
#include <locale>
#include <map>
#include <vector>
#include <string.h>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

#include "EUDAQBlock2016.hh"
#include "BIFBlock.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;

/**
 * Processor to read SLCIO EUDAQ files and sort them according to the BXID 
 * Then do the AHCAL-BIF Offset calibration by counting the number of correlated events
 * (either a separation between ECAL and AHCAL collections is done by EUDAQ) 
 * @author E. Brianne, DESY Hamburg
 * @date May 2016
 * Created for 2016 testbeams EUDAQ data format and AHCAL-BIF Offset calibration.
 */

namespace CALICE{
   
  Ahc2OffsetCalibrator aAhc2OffsetCalibrator ;
  
   
  Ahc2OffsetCalibrator::Ahc2OffsetCalibrator() : Processor("Ahc2OffsetCalibrator")
  {
    _description = "Processor to built events from raw EUDAQ slcio files";

    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection of EUDAQ raw data",
			       _inputColName,
			       std::string(""));

    registerProcessorParameter("InputCollectionNameBIF",
    			       "Name of the input BIF collection",
    			       _inputColNameBIF,
    			       std::string(""));
    
    registerProcessorParameter("Output_file",
			       "Output rootfile name for number of events per bif offset",
			       _outputfile,
			       std::string(""));

    StringVec BifExample;
    BifExample.push_back("10000");
    BifExample.push_back("20000");
    BifExample.push_back("10");
  
    registerOptionalParameter( "BifHandler" , 
			       "Start of the scan, stop of scan and step"  ,
			       _BifVector ,
			       BifExample ,
			       BifExample.size()) ;
  }


  Ahc2OffsetCalibrator::~Ahc2OffsetCalibrator() {}

  void Ahc2OffsetCalibrator::init() {
    LcioEventNr = 0;
    nCorrelatedEvt.clear();

    //Scan parameters
    StartScan = 0;
    StopScan = 0;
    StepScan = 0;

    //Process Bif scan parameters
    if( parameterSet( "BifHandler" ) ) 
      {
	unsigned index = 0 ;
	while( index < _BifVector.size() )
	  {
	  
	    std::string strStart( _BifVector[ index++ ] ); 
	    std::string strStop( _BifVector[ index++ ] ); 
	    std::string strStep( _BifVector[ index++ ] ); 
	  
	    StartScan = atoi(strStart.c_str());
	    StopScan = atoi(strStop.c_str());
	    StepScan = atoi(strStep.c_str());
	  }
      }

    printParameters();
  }

  void Ahc2OffsetCalibrator::processEvent(LCEvent* evt){
  
    int EUDAQEvtNumber = evt->getEventNumber();
    int Timestamp = 0;//Event Timestamp
    unsigned long long int TimeStamp_start = 0;//BIF Start Timestamp
    unsigned long long int TimeStamp_stop = 0;//BIF Stop Timestamp

    int read_0[5];
    std::vector<int> read_ADC;
    std::vector<int> read_TDC;

    rawData2016 blockData;
                 
    std::map< int, std::vector<rawData2016> > rData;           
    std::vector<int>  rBIF;
    std::vector< std::vector<int> > new_rBIF;

    LCCollection* col = NULL; //data collection
    LCCollection* colBIF = NULL; //bif collection
 
    try
      {
	//fetch EUDAQ data unsorted collection
	col = evt->getCollection( _inputColName );
      }
    catch(EVENT::DataNotAvailableException &e)
      {
	streamlog_out(WARNING)<< "Event " << EUDAQEvtNumber << " missing collection "
			      <<_inputColName<<endl<<e.what()<<endl;

	if(evt->getTimeStamp() == -1)
	  throw SkipEventException(this);
	else
	  return;
      }

    //Get Timestamp from data
    string Timestamp_str = col->getParameters().getStringVal("Timestamp");
    struct tm tm;
    time_t epoch;

    strptime(Timestamp_str.c_str(), "%a, %d %b %Y %H:%M:%S %z", &tm);
    epoch = mktime(&tm);

    Timestamp = epoch;

    //Data Block
    for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
      {
	LCObject *obj = col->getElementAt(ielm);
	EUDAQBlock2016 lBlock(obj);
	    
	read_0[0] = lBlock.GetCycleNr();
	read_0[1] = lBlock.GetBunchXID();
	read_0[2] = lBlock.GetEvtNr();
	read_0[3] = lBlock.GetChipID();
	read_0[4] = lBlock.GetNChannels();

	read_TDC = lBlock.GetTDC();
	read_ADC = lBlock.GetADC();

	blockData.iCycleNr = read_0[0];
	blockData.iBxID = read_0[1];
	blockData.iEvNr = read_0[2];
	blockData.iCpID = read_0[3];
	blockData.iNChan = read_0[4];
	blockData.iTDC  = read_TDC;
	blockData.iADC  = read_ADC;
   	        
	rData[read_0[1]].push_back(blockData);
      }

    streamlog_out(DEBUG4) <<" rData.size(): "<< rData.size() << endl;	

    //Try fetch BIF Data
    try
      {
	//fetch EUDAQ temperature data collection
	colBIF= evt->getCollection(_inputColNameBIF) ;
      }
    catch(EVENT::DataNotAvailableException &e)
      {
	streamlog_out(WARNING)<< "Event " << EUDAQEvtNumber << " missing collection "
			      <<_inputColNameBIF<<endl<<e.what()<<endl;

	if(evt->getTimeStamp() == -1)
	  throw SkipEventException(this);
	else
	  return;
      }

    //BIF Block
    for(int ielm = 0; ielm < colBIF->getNumberOfElements(); ielm++)
      {
	LCGenericObject *obj = dynamic_cast<LCGenericObject*>(colBIF->getElementAt(ielm));

	//Check Start and Stop
	unsigned int start_acq = (unsigned int)obj->getIntVal(0);
	unsigned int stop_acq = (unsigned int)obj->getIntVal(obj->getNInt() - 4);
	
	bool skipEvent = false;

	if( ((start_acq)&0xFF000000) != BIF_START_ACQ )
	  {
	    streamlog_out(WARNING)<< "Problem with the start acquisition of the BIF" << endl;
	    skipEvent = true;
	  }
	
	if( ((stop_acq)&0xFF000000) != BIF_STOP_ACQ )
	  {
	    streamlog_out(WARNING)<< "Problem with the stop acquisition of the BIF" << endl;
	    skipEvent = true;
	  }

	if(skipEvent)
	  {
	    streamlog_out(WARNING)<< "Skipping event " << EUDAQEvtNumber << " because of missing start/stop acquisition" << endl;
	    throw SkipEventException(this);
	  }

	//Get Timestamp start
	unsigned int start_TS_low = (unsigned int)obj->getIntVal(2);
	unsigned int start_TS_high = (unsigned int)obj->getIntVal(3);
	TimeStamp_start = ( (unsigned long long int)start_TS_high<<32 | start_TS_low );
	
	//Get Timestamp stop
	unsigned int stop_TS_low = (unsigned int)obj->getIntVal(obj->getNInt() - 2);
	unsigned int stop_TS_high = (unsigned int)obj->getIntVal(obj->getNInt() - 1);
	TimeStamp_stop = ( (unsigned long long int)stop_TS_high<<32 | stop_TS_low );

	streamlog_out(DEBUG) << " BIF Block - Start Time : " << TimeStamp_start << endl 
			     << " - Stop Time : " << TimeStamp_stop << endl 
			     << " Acq time : " << TimeStamp_stop - TimeStamp_start << endl;

	//Push values in vector without start and stop
	for(int i = 4; i < obj->getNInt() - 4; i++)
	  {
	    rBIF.push_back(obj->getIntVal(i));
	  }

	//Divide original vector in N vector corresponding to N triggers of the BIF each of a size of 4
	unsigned int nTrigger = (int)rBIF.size()/4;
	streamlog_out(DEBUG) << "Number of BIF Trigger in the ROC " << EUDAQEvtNumber 
			     << " : " << nTrigger << endl;

	new_rBIF.resize(nTrigger);
	for(unsigned int i = 0; i < nTrigger; i++)
	  {
	    new_rBIF[i].resize(4);
	    for(unsigned int ii = 0; ii < 4; ii++)
	      {
		new_rBIF[i][ii] = rBIF.at(4*i + ii);
	      }
	  }

	rBIF.clear();
      }

    // **********************************************************
    //  Processing data
    //  build events according to the different BunchXID
    //  within one CycleNr, and remove them,
    //  until all collected data have been asigned to an event.
    // **********************************************************

    for(std::map< int, vector<rawData2016> >::iterator it=rData.begin(); it!=rData.end();++it)
      {	
	//streamlog_out(DEBUG)<< " ===== Ahc2OffsetCalibrator processing Event "<< LcioEventNr <<"  ======" << endl;
	LcioEventNr++; //Event start from 1.

	//Create new BIF Blocks
	for(unsigned int i = 0; i < new_rBIF.size(); i++)
	  {
	    int source = -1;
	    unsigned int TriggerSource = (unsigned int)new_rBIF[i].at(0);

	    if( ((TriggerSource)&0xFFF0000) == BIF_1ST_LEMO )
	      source = 0;
	    if( ((TriggerSource)&0xFFF0000) == BIF_2ND_LEMO )
	      source = 1;
	    if( ((TriggerSource)&0xFFF0000) == BIF_3RD_LEMO )
	      source = 2;
	    if( ((TriggerSource)&0xFFF0000) == BIF_4TH_LEMO )
	      source = 3;
	    
	    unsigned int TS_low = (unsigned int)new_rBIF[i].at(2);
	    unsigned int TS_high = (unsigned int)new_rBIF[i].at(3);
	    unsigned long long int TimeStamp = ( (unsigned long long int)TS_high<<32 | TS_low );

	    //Relative time to start acquisition
	    unsigned long long int RelativeTime = TimeStamp - TimeStamp_start;

	    //Start of scan of bifoffset
	    for(int _offset = StartScan; _offset <= StopScan;)
	      {
		//BXID of the Trigger
		int BXID_BIF = (int)(RelativeTime - _offset)/BIF_BINNING;
		//Time of the Trigger
		float Time_BIF = (float)((RelativeTime - _offset)%BIF_BINNING * BIF_RESOLUTION);

		if(it->first == BXID_BIF)
		  {
		    nCorrelatedEvt[_offset]++;
		  }

		//streamlog_out(DEBUG) <<"Event " << LcioEventNr << " BXID Correlated " << nCorrelatedEvt[_offset] << " with offset " << _offset <<  endl;

		_offset += StepScan;//increase scan step
	      }
	  }
      } // loop map key to save the event with each CycleNr.
    
    new_rBIF.clear();
    rData.clear();

    throw SkipEventException(this);
  }// look into collection

  void Ahc2OffsetCalibrator::end() {

    int ipoint = 0;

    TGraph *_bifgraph = new TGraph();

    //Write results to file
    for(int _offset = StartScan; _offset <= StopScan;)
      {
	streamlog_out(DEBUG) << "Total Number of BXID Correlated " << nCorrelatedEvt[_offset] << " with offset " << _offset <<  endl;

	_bifgraph->SetPoint(ipoint, _offset, nCorrelatedEvt[_offset]);
	ipoint++;

	_offset += StepScan;//increase scan step
      }

    _bifgraph->SetMarkerSize(20);
    _bifgraph->GetXaxis()->SetTitle("Offset");
    _bifgraph->GetYaxis()->SetTitle("Number of correlated events");

    _bifgraph->SetName("Offset vs Number of Correlated Events");
    _bifgraph->SetTitle("Offset vs Number of Correlated Events");

    TFile *fOut = new TFile(_outputfile.c_str(), "RECREATE");
    fOut->cd();
    _bifgraph->Write();
    fOut->Close();

    streamlog_out(MESSAGE) <<" ===== End of Ahc2OffsetCalibrator processor ===== " <<  endl;

  }

  void Ahc2OffsetCalibrator::printParameters() {

    cout <<" ===== Parameters Ahc2OffsetCalibrator processor ===== " <<  endl;
    cout << "Intput Collections : " << _inputColName 
	 << "\t" << _inputColNameBIF << endl;

    cout << "Scan parameters: " << endl
	 << "\t Start " << StartScan << endl
	 << "\t Stop " << StopScan << endl
	 << "\t Step " << StepScan << endl;
  
  }
  
} //namespace
