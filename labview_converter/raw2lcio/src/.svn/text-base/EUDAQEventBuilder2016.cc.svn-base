#include "EUDAQEventBuilder2016.hh"
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

#include "EUDAQBlock2016.hh"
#include "EUDAQTempSensorBlock.hh"
#include "BIFBlock.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;


/**
 * Processor to read SLCIO EUDAQ files and sort them according to the BXID 
 * (now they are sorted in readout cycles) 
 * (either a separation between ECAL and AHCAL collections is done by EUDAQ) 
 * @author A. Irles, based on the EUDAQEventBuilder writen by A. Irles DESY Hamburg
 * @date January 12  2016
 * @modif May 2016, E. Brianne (BIF data and fix bug)
 * Created for 2016 testbeams EUDAQ data format.
 */


namespace CALICE{
    
    
  EUDAQEventBuilder2016 aEUDAQEventBuilder2016 ;
  
   
  EUDAQEventBuilder2016::EUDAQEventBuilder2016() : Processor("EUDAQEventBuilder2016")
  {
    _description = "Processor to built events from raw EUDAQ slcio files";

    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection of EUDAQ raw data",
			       _inputColName,
			       std::string(""));
    
    registerProcessorParameter("InputCollectionNameTemp",
    			       "Name of the input temperature collection",
    			       _inputColNameTemp,
    			       std::string(""));

    registerProcessorParameter("InputCollectionNameBIF",
    			       "Name of the input BIF collection",
    			       _inputColNameBIF,
    			       std::string(""));
    
    registerProcessorParameter("DetectorType",
    			       "Name of the Detector",
    			       _detectorTypeName,
    			       std::string(""));

    registerProcessorParameter("OutputCollectionNameECAL",
			       "Name of the ouput collection of the sorted events for the ECAL ",
			       _outputColNameECAL,
			       std::string(""));

    registerProcessorParameter("OutputCollectionNameHCAL",
			       "Name of the ouput collection of the sorted events for the HCAL ",
			       _outputColNameHCAL,
			       std::string(""));

    registerProcessorParameter("BIF_offset",
			       "BIF Offset between start acquisition and first BXID of the SPIROC",
			       _bifoffset,
			       (int) 0);
    
  }


  EUDAQEventBuilder2016::~EUDAQEventBuilder2016() {}


    
  void EUDAQEventBuilder2016::init() {
    temperaturecollection_write = false;
    bifcollection_write = false;
    LcioEventNr = 0;
    discarded_events = 0;
    all_events = 0;

    printParameters();
  }


  void EUDAQEventBuilder2016::processEvent(LCEvent* evt){
  

    int runNumber = evt->getRunNumber();
    int EUDAQEvtNumber = evt->getEventNumber();
    int Timestamp = 0;//Event Timestamp
    unsigned long long int TimeStamp_start = 0;//BIF Start Timestamp
    unsigned long long int TimeStamp_stop = 0;//BIF Stop Timestamp

    int read_0[5];
    std::vector<int> read_ADC;
    std::vector<int> read_TDC;

    rawData2016 blockData;
    std::map<int, int> nHitsPerChipMap;//Map for number of hits per Chips
    IntVec nHitsPerChip;//vector to store the number of hits per Chips per event
                  
    int temp[10];
    rawTemp blockTemp;

    std::map< int, std::vector<rawData2016> > rData;        
    std::vector<rawTemp>  rTemp;       
    std::vector<int>  rBIF;
    std::vector< std::vector<int> > new_rBIF;

    temperaturecollection_write = false;

    LCCollection* col = NULL; //data collection
    LCCollection* colTemp = NULL; //temp collection
    LCCollection* colBIF = NULL; //bif collection
 
    try
      {
	//fetch EUDAQ data unsorted collection
	col = evt->getCollection( _inputColName );
      }
    catch(EVENT::DataNotAvailableException &e)
      {
	streamlog_out(DEBUG)<< "Event " << EUDAQEvtNumber << " missing collection "
			    <<_inputColName<<endl<<e.what()<<endl;

	if(evt->getTimeStamp() == -1)
	  throw SkipEventException(this);
	else
	  return;
      }

    const std::vector<std::string> *cnames = evt->getCollectionNames();

    for(unsigned cnamesindex = 0; cnamesindex< cnames->size(); cnamesindex++) {
      if(cnames->at(cnamesindex)==_inputColNameTemp) 
	temperaturecollection_write = true;
    }

    //Get Timestamp from data
    string Timestamp_str = col->getParameters().getStringVal("Timestamp");
    struct tm tm;
    time_t epoch;

    strptime(Timestamp_str.c_str(), "%a, %d %b %Y %H:%M:%S %z", &tm);
    epoch = mktime(&tm);

    Timestamp = epoch;


    //Data Block  -- 
    // we add a crosscheck the quality of the data
    // comapring the values of the hbit and gain bit stored in the adc and tdc packets
    // if the bit values are uncoherent, we do not save the event (all information of all chips in the same BXID)
    
    for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
      {

	bool good_data=true;
	all_events++;

	LCObject *obj = col->getElementAt(ielm);
	EUDAQBlock2016 lBlock(obj);

	read_0[0] = lBlock.GetCycleNr();
	read_0[1] = lBlock.GetBunchXID();
	read_0[2] = lBlock.GetEvtNr();
	read_0[3] = lBlock.GetChipID();
	read_0[4] = lBlock.GetNChannels();
	  
	read_TDC = lBlock.GetTDC();
	read_ADC = lBlock.GetADC();
	  
	streamlog_out(DEBUG4)  <<"  read SLCIO "<<lBlock.GetEvtNr()<<" "<<lBlock.GetBunchXID()<<endl;//airqui
	  
	blockData.iCycleNr = read_0[0];
	blockData.iBxID = read_0[1];
	blockData.iEvNr = read_0[2];
	blockData.iCpID = read_0[3];
	blockData.iNChan = read_0[4];
	blockData.iTDC  = read_TDC;
	blockData.iADC  = read_ADC;

	// check the quality of the data
	if( read_ADC.size() != read_TDC.size() ) {
	  streamlog_out(WARNING)<< "different size for ADC and TDC "<< std:: endl;
	  good_data=false;
	}

	double adc_v=0, tdc_v=0, hb_adc=-1, hb_tdc=-1, gb_adc=-1, gb_tdc=-1;

	for(unsigned int iRead=0; iRead<read_ADC.size(); iRead++) {

	  adc_v=read_ADC[iRead]%4096;// adc, tdc
	  tdc_v=read_TDC[iRead]%4096;
	  
	  hb_adc=(read_ADC[iRead]& 0x1000)?1:0;
	  hb_tdc=(read_TDC[iRead]& 0x1000)?1:0;
	  
	  gb_adc=(read_ADC[iRead]& 0x2000)?1:0;
	  gb_tdc=(read_TDC[iRead]& 0x2000)?1:0;

	  if(adc_v >4096 || tdc_v>4096) {
	    streamlog_out(WARNING)<< "Wrong adc or tdc value: ADC=" <<adc_v<<" TDC="<<tdc_v<<" cycleNr="<<read_0[0] <<std:: endl;
	    good_data=false;
	  }
	  
	  if(hb_adc != hb_tdc)  {
	    streamlog_out(WARNING)<< "Uncoherent hit bit value: HB_ADC=" <<hb_adc<<" HB_TDC="<<hb_tdc<<" cycleNr="<<read_0[0] <<std:: endl;
	    good_data=false;
	  }
	  
	  if(gb_adc != gb_tdc)  {
	    streamlog_out(WARNING)<< "Uncoherent gain bit value:  GB_ADC=" <<gb_adc<<" HB_TDC="<<gb_tdc<<" cycleNr="<<read_0[0] <<std:: endl;
	    good_data=false;
	  }
	  
	  if( hb_adc<0 || hb_adc>1 || gb_adc<0 || gb_adc>1 ) {
	    streamlog_out(WARNING)<< "Wrong bit value (but equal in ADC/TDC):  HB=" <<hb_adc<<" GB="<<gb_adc<<" cycleNr="<<read_0[0] <<std:: endl;
	    good_data=false;
	  }
	}
 
	if(good_data==true) rData[read_0[1]].push_back(blockData);
	else discarded_events++;
      }

    streamlog_out(DEBUG4) <<" rData.size(): "<< rData.size() << endl;	

    //Temperature Block
    if(temperaturecollection_write == true)
      {
	try
	  {
	    //fetch EUDAQ temperature data collection
	    colTemp= evt->getCollection( _inputColNameTemp) ;
	  }
	catch(EVENT::DataNotAvailableException &e)
	  {
	    streamlog_out(WARNING)<< "Event " << EUDAQEvtNumber << " missing TEMP collection "
				  <<_inputColNameTemp<<endl<<e.what()<<endl;
	    return;
	  }

	for(int ielm = 0; ielm < colTemp->getNumberOfElements(); ielm++)
	  {
	    //i:LDA;i:port;i:T1;i:T2;i:T3;i:T4;i:T5;i:T6;i:TDIF;i:TPWR
	    LCObject *obj = colTemp->getElementAt(ielm);
	    EUDAQTempSensorBlock tBlock(obj);

	    temp[0] = tBlock.GetLDANumber();
	    temp[1] = tBlock.GetPortNumber();
	    temp[2] = tBlock.GetT1();
	    temp[3] = tBlock.GetT2();
	    temp[4] = tBlock.GetT3();
	    temp[5] = tBlock.GetT4();
	    temp[6] = tBlock.GetT5();
	    temp[7] = tBlock.GetT6();
	    temp[8] = tBlock.GetTDIF();
	    temp[9] = tBlock.GetTPWR();

        
	    blockTemp.iLDANumber = temp[0];
	    blockTemp.iPortNumber = temp[1];
	    blockTemp.iT1 = temp[2];
	    blockTemp.iT2 = temp[3];
	    blockTemp.iT3 = temp[4];
	    blockTemp.iT4 = temp[5];
	    blockTemp.iT5 = temp[6];
	    blockTemp.iT6 = temp[7];
	    blockTemp.iTDIF = temp[8];
	    blockTemp.iTPWR = temp[9];

	    rTemp.push_back(blockTemp);

	    streamlog_out(DEBUG4) <<" temperature - LDA: "<<temp[0] <<" port: "<<temp[1] <<" T1: "<<temp[2]<<"  TPWR: " <<temp[9] << std::endl;
	  }
      }

    //Try fetch BIF Data
    try
      {
	//fetch EUDAQ temperature data collection
	colBIF= evt->getCollection(_inputColNameBIF) ;
      }
    catch(EVENT::DataNotAvailableException &e)
      {
	streamlog_out(WARNING)<< "Event " << EUDAQEvtNumber << " missing BIF collection "
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
    //  If TempSensor collection is found, add it to the last event of the readoutcycle.
    // **********************************************************

    for(std::map< int, vector<rawData2016> >::iterator it=rData.begin(); it!=rData.end();++it)
      {	
	//streamlog_out(MESSAGE)<< " ===== EUDAQEventBuilder2016 processing Event "<< LcioEventNr <<"  ======" << endl;

	LCEventImpl* evt_new = new LCEventImpl() ;
	LCCollectionVec *colTemp2 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	LCCollectionVec *colBIF2 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	LCCollectionVec *colECAL = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	LCCollectionVec *colHCAL = new LCCollectionVec( LCIO::LCGENERICOBJECT ); 

	LcioEventNr++; //Event start from 1.
	evt_new->setRunNumber(  runNumber  ) ;
	evt_new->setEventNumber( LcioEventNr ) ;
	evt_new->setDetectorName( _detectorTypeName);

	LCTime evttime( Timestamp ) ;//
	evt_new->setTimeStamp( evttime.timeStamp() ) ;

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
	    //BXID of the Trigger
	    int BXID_BIF = (int)(RelativeTime - _bifoffset)/BIF_BINNING;
	    //Time of the Trigger
	    float Time_BIF = (float)((RelativeTime - _bifoffset)%BIF_BINNING * BIF_RESOLUTION);

	    BIFBlock *bifBlock = new BIFBlock(source, BXID_BIF, Time_BIF);

	    if(it->first == BXID_BIF)
	      colBIF2->addElement(bifBlock);
	  }

	std::stringstream ss;
	ss << TimeStamp_start; 
	std::string TimeStamp_start_str = ss.str();
	ss.str("");
	ss << TimeStamp_stop; 
	std::string TimeStamp_stop_str = ss.str();
	ss.str("");
	
	colBIF2->parameters().setValue("DataDescription", "i:InputSource; i:BXID:f:Time");
	colBIF2->parameters().setValue("TypeName", "BIFBlock");
	colBIF2->parameters().setValue("Start_Acquisition", TimeStamp_start_str);
	colBIF2->parameters().setValue("Stop_Acquisition", TimeStamp_stop_str);
	evt_new->addCollection(colBIF2, "BIFData");
	
	for (unsigned int i = 0; i < it->second.size(); i++ )
	  {
	    int iCycleNr = it->second.at(i).iCycleNr;
	    int iBxID = it->second.at(i).iBxID;
	    int iEvNr = it->second.at(i).iEvNr;
	    int iCpID = it->second.at(i).iCpID;
	    int iNChan = it->second.at(i).iNChan;
	    std::vector<int> iTDC  = it->second.at(i).iTDC;
	    std::vector<int>  iADC  = it->second.at(i).iADC;

	    EUDAQBlock2016 * lBlock = new EUDAQBlock2016(iCycleNr, 
							 iBxID, 
							 iEvNr, 
							 iCpID, 
							 iNChan,
							 iTDC[0], iTDC[1], iTDC[2], iTDC[3], iTDC[4], iTDC[5],  iTDC[6], iTDC[7], iTDC[8], iTDC[9], iTDC[10], iTDC[11], iTDC[12],iTDC[13], iTDC[14], iTDC[15], iTDC[16], iTDC[17],
							 iTDC[18], iTDC[19], iTDC[20], iTDC[21], iTDC[22], iTDC[23],  iTDC[24], iTDC[25], iTDC[26], iTDC[27], iTDC[28], iTDC[29], iTDC[30],iTDC[31], iTDC[32], iTDC[33], iTDC[34], iTDC[35],
							 iADC[0], iADC[1], iADC[2], iADC[3], iADC[4], iADC[5],  iADC[6], iADC[7], iADC[8], iADC[9], iADC[10], iADC[11], iADC[12],iADC[13], iADC[14], iADC[15], iADC[16], iADC[17],
							 iADC[18], iADC[19], iADC[20], iADC[21], iADC[22], iADC[23],  iADC[24], iADC[25], iADC[26], iADC[27], iADC[28], iADC[29], iADC[30],iADC[31], iADC[32], iADC[33], iADC[34], iADC[35]
							 );


	    if(nHitsPerChipMap.count(iCpID) == 0)
	      nHitsPerChipMap[iCpID] = 0;

	    for(int ichn = 0; ichn < iNChan; ichn++)
	      {
		//Get HitBit
		int HitBit = (iADC[ichn]& 0x1000)?1:0;

		//HitsPerChips (number of triggered channels)
		if(HitBit == 1)
		  nHitsPerChipMap[iCpID]++;
	      }
	    

	    if (iCpID>=129 && iCpID<=140)
	      colECAL->addElement( lBlock );
	    else
	      colHCAL->addElement( lBlock );	    
	  }

	nHitsPerChip.clear();
	for(map<int, int>::iterator it2 = nHitsPerChipMap.begin(); it2 != nHitsPerChipMap.end(); ++it2)
	  {
	    if(it2->second != 0)
	      nHitsPerChip.push_back(it2->first*100+it2->second);
	  }
	nHitsPerChipMap.clear();

	streamlog_out(DEBUG0)<<"--------------- Writing Event Parameters -------------------" <<endl;
	streamlog_out(DEBUG0)<<"BXID: "<< it->first <<endl;
	streamlog_out(DEBUG0)<<"HitPerChips: "<< nHitsPerChip.size() << endl;

	//add event parameter
	evt_new->parameters().setValue("BXID", it->first);
	evt_new->parameters().setValues("nHitsPerChip", nHitsPerChip);

	evt_new->addCollection( colECAL, _outputColNameECAL );
	evt_new->addCollection( colHCAL, _outputColNameHCAL );

	//if temperature collection is in the readoutcycle, save it in the first built event
	if(temperaturecollection_write == true) 
	  {
	    for (unsigned int i = 0; i < rTemp.size(); i++ ) 
	      {      
		EUDAQTempSensorBlock * tBlock = new EUDAQTempSensorBlock(rTemp.at(i).iLDANumber,
									 rTemp.at(i).iPortNumber,
									 rTemp.at(i).iT1,
									 rTemp.at(i).iT2,
									 rTemp.at(i).iT3,
									 rTemp.at(i).iT4,
									 rTemp.at(i).iT5,
									 rTemp.at(i).iT6,
									 rTemp.at(i).iTDIF,
									 rTemp.at(i).iTPWR);
		colTemp2->addElement( tBlock );
	      }

	    evt_new->addCollection( colTemp2, "TemperatureSensor" );
	    temperaturecollection_write = false;
	    rTemp.clear();
	  }

	ProcessorMgr::instance()->processEvent( evt_new ) ;

	// ------------ IMPORTANT ------------- !
	// we created the event so we need to delete it ...
	delete evt_new ;

      } // loop map key to save the event with each CycleNr.
    
    new_rBIF.clear();
    rData.clear();

    throw SkipEventException(this);

  }// look into collection

  void EUDAQEventBuilder2016::end() {
    cout <<" ===== End of EUDAQEventBuilder2016 processor ===== " <<  endl;
    cout <<" All events                                                    = " << all_events <<  endl;
    cout <<" Number of discarded events (wrong hb/gb) = " << discarded_events <<  endl;

  }

  void EUDAQEventBuilder2016::printParameters() {

    cout <<" ===== Parameters EUDAQEventBuilder2016 processor ===== " <<  endl;
    cout << "Intput Collections : " << _inputColName 
	 << "\t" << _inputColNameTemp
	 << "\t" << _inputColNameBIF << endl;

    cout << "Output Collections : " << _outputColNameECAL
	 << "\t" << _outputColNameHCAL << endl;

    cout << "BIF Offset : " << _bifoffset << endl;
  
  }
  
} //namespace

