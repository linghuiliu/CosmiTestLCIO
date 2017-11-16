#include "EUDAQEventBuilder2016_woBIF.hh"
//lcio
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "UTIL/LCTime.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "IMPL/LCCollectionVec.h"
//#include "EVENT/LCCollection.h"
//#include "lcio.h"

//marlin
#include <marlin/ProcessorMgr.h>
#include "marlin/Exceptions.h"


#include <fstream>
#include <time.h>
#include <locale>
#include <map>
#include <vector>
#include <string.h>

#include "EUDAQBlock2016.hh"
#include "EUDAQTempSensorBlock.hh"
#include "hodoscopeBlock.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;


/**
 * Processor to read SLCIO EUDAQ files and sort them according to the BXID 
 * (now they are sorted in readout cycles) 
 * (either a separation between ECAL and AHCAL collections is done by EUDAQ) 
 * @author A. Irles, based on the EUDAQEventBuilder writen by A. Irles DESY Hamburg
 * @date January 12  2016
 * Created for 2016 testbeams EUDAQ data format.
 */


namespace CALICE{
    
    
  EUDAQEventBuilder2016_woBIF aEUDAQEventBuilder2016_woBIF ;
  
   
  EUDAQEventBuilder2016_woBIF::EUDAQEventBuilder2016_woBIF() : Processor("EUDAQEventBuilder2016_woBIF")
  {
    _description = "Processor to built events from raw EUDAQ slcio files";

    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection of EUDAQ raw data",
			       _inputColName,
			       std::string("EUDAQDataScCAL"));
    
    registerProcessorParameter("InputCollectionNameHodo1",
			       "Name of the input collection of hodoscope raw data",
			       _inputColNameHodo1,
			       std::string("hodoscope1"));
    
    registerProcessorParameter("InputCollectionNameHodo2",
			       "Name of the input collection of hodoscope raw data",
			       _inputColNameHodo2,
			       std::string("hodoscope2"));
    
    registerProcessorParameter("InputCollectionNameTemp",
    			       "Name of the input temperature collection",
    			       _inputColNameTemp,
    			       std::string("TempSensor"));

    registerProcessorParameter("DetectorType",
    			       "Name of the Detector",
    			       _detectorTypeName,
    			       std::string(""));

    registerProcessorParameter("OutputCollectionNameECAL",
			       "Name of the output collection of the sorted events for the ECAL ",
			       _outputColNameECAL,
			       std::string("EUDAQDataECAL"));

    registerProcessorParameter("OutputCollectionNameHCAL",
			       "Name of the output collection of the sorted events for the HCAL ",
			       _outputColNameHCAL,
			       std::string("EUDAQDataHCAL"));
    
    registerProcessorParameter("OutputCollectionNameHodo1",
			       "Name of the output collection of hodoscope raw data",
			       _outputColNameHodo1,
			       std::string("hodoscope1"));
    
    registerProcessorParameter("OutputCollectionNameHodo2",
			       "Name of the output collection of hodoscope raw data",
			       _outputColNameHodo2,
			       std::string("hodoscope2"));
    
  }


  EUDAQEventBuilder2016_woBIF::~EUDAQEventBuilder2016_woBIF() {}


    
  void EUDAQEventBuilder2016_woBIF::init() {
    temperaturecollection_write = false;  
    LcioEventNr=0;
    discarded_events = 0;
    all_events = 0;

    printParameters();
  }


  void EUDAQEventBuilder2016_woBIF::processEvent(LCEvent* evt){
  

    int runNumber  = evt->getRunNumber();//airqui
    int Timestamp ;//
        
    int read_0[5];
    std::vector<int> read_ADC;
    std::vector<int> read_TDC;

    rawData2016 blockData;
                  
    int temp[10];
    rawTemp blockTemp;

    std::map< int, std::vector<rawData2016> > rData;        
    std::vector<rawTemp>  rTemp;       

 
    LCCollection* col ; //data collection
    LCCollection* incolHodo1;
    LCCollection* incolHodo2; 
    LCCollection* colTemp ; //temp collection
    LCCollectionVec *colHodo1 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
    LCCollectionVec *colHodo2 = new LCCollectionVec( LCIO::LCGENERICOBJECT ); 

    try
      {
	incolHodo1 = evt->getCollection( _inputColNameHodo1 ) ;
	incolHodo2 = evt->getCollection( _inputColNameHodo2 ) ;

        for (int ielm=0;ielm<incolHodo1->getNumberOfElements();ielm++){
	  LCObject *obj = incolHodo1->getElementAt(ielm);
	  HodoscopeBlock * lBlock = new HodoscopeBlock(obj);
          colHodo1->addElement(lBlock);
	}
        for (int ielm=0;ielm<incolHodo2->getNumberOfElements();ielm++){
	  LCObject *obj = incolHodo2->getElementAt(ielm);
	  HodoscopeBlock * lBlock = new HodoscopeBlock(obj);
          colHodo2->addElement(lBlock);
	}
	//fetch EUDAQ data unsorted collection
	col = evt->getCollection( _inputColName ) ;
	const std::vector<std::string> *cnames = evt->getCollectionNames();

        //Get Timestamp from data
        string Timestamp_str = col->getParameters().getStringVal("Timestamp");
        struct tm tm;
        time_t epoch;

        strptime(Timestamp_str.c_str(), "%a, %d %b %Y %H:%M:%S %z", &tm);
        epoch = mktime(&tm);

        Timestamp = epoch;

	//Timestamp= col->getParameters().getIntVal("Timestamp_i");//TimeStamp();//airqui

	//Data Block  -- 
	// we add a crosscheck the quality of the data
	// comapring the values of the hbit and gain bit stored in the adc and tdc packets
	// if the bit values are uncoherent, we do not save the event (all information of all chips in the same BXID)
	bool good_data=true;

	for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
	  {
	    good_data=true;

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
	    
	    read_0[0] = lBlock.GetCycleNr();
	    read_TDC = lBlock.GetTDC();
	    read_ADC = lBlock.GetADC();
	    	    
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
		streamlog_out(WARNING)<< "Uncoherent gain bit value:  GB_ADC=" <<gb_adc<<" GB_TDC="<<gb_tdc<<" cycleNr="<<read_0[0] <<std:: endl;
		good_data=false;
	      }
	      
	      if( hb_adc<0 || hb_adc>1 || gb_adc<0 || gb_adc>1 ) {
		streamlog_out(WARNING)<< "Wrong bit value (but equal in ADC/TDC):  HB=" <<hb_adc<<" GB="<<gb_adc<<" cycleNr="<<read_0[0] <<std:: endl;
		good_data=false;
	      }
	    }
	
	    if(good_data==true)  	        
	      rData[read_0[1]].push_back(blockData);
	    else discarded_events++;
	  }




	streamlog_out(DEBUG4) <<" rData.size(): "<<rData.size() << endl;	
    
	temperaturecollection_write = false;
    
	for(unsigned cnamesindex = 0; cnamesindex< cnames->size(); cnamesindex++) {
	  if(cnames->at(cnamesindex)==_inputColNameTemp) 
	    temperaturecollection_write = true;
	}
    
	if(temperaturecollection_write == true){
      
	  //fetch EUDAQ temperature data collection
	  colTemp= evt->getCollection( _inputColNameTemp) ;
	  temperaturecollection_write = true;

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

	      streamlog_out(DEBUG4) <<" temperature      LDA: "<<temp[0] <<" port: "<<temp[1] <<" T1: "<<temp[2]<<"  TPWR: " <<temp[9] << std::endl;
	    }
	}


      }

    catch (  DataNotAvailableException err )
      {
	//	cout <<  "RootTreeGenerator3 WARNING: Collection "<< _inputColName
	//	     << " not available in event "<< evt->getEventNumber() << endl;
	return;
      }

   

    // **********************************************************
    //  Processing data
    //  build events according to the different BunchXID
    //  within one CycleNr, and remove them,
    //  until all collected data have been asigned to an event.
    //  If TempSensor collection is found, add it to the last event of the readoutcycle.
    // **********************************************************
      

    for(  map< int, vector<rawData2016> >::iterator it=rData.begin(); it!=rData.end();it++ )
      {
	
	//streamlog_out(MESSAGE)<< " ===== EUDAQEventBuilder2016 processing Event "<< LcioEventNr <<"  ======" << endl;
	LCEventImpl*  evt_new = new LCEventImpl() ;
	
       	LCCollectionVec *colTemp2= new LCCollectionVec( LCIO::LCGENERICOBJECT );

	LCCollectionVec *colECAL = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	LCCollectionVec *colHCAL = new LCCollectionVec( LCIO::LCGENERICOBJECT ); 
	for (unsigned int i = 0; i < it->second.size(); i++ )
	  {
	    int iCycleNr = it->second.at(i).iCycleNr;
	    int iBxID = it->second.at(i).iBxID;
	    int iEvNr = it->second.at(i).iEvNr;
	    int iCpID = it->second.at(i).iCpID;
	    int iNChan = it->second.at(i).iNChan;
	    std::vector<int> iTDC  = it->second.at(i).iTDC;
	    std::vector<int>  iADC  = it->second.at(i).iADC;

	//	if (iCpID<180) iCpID+=44;
	//	else iCpID+=36;

	    EUDAQBlock2016 * lBlock = new EUDAQBlock2016(iCycleNr, iBxID, iEvNr, iCpID, iNChan,
							 iTDC[0], iTDC[1], iTDC[2], iTDC[3], iTDC[4], iTDC[5],  iTDC[6], iTDC[7], iTDC[8], iTDC[9], iTDC[10], iTDC[11], iTDC[12],iTDC[13], iTDC[14], iTDC[15], iTDC[16], iTDC[17],
							 iTDC[18], iTDC[19], iTDC[20], iTDC[21], iTDC[22], iTDC[23],  iTDC[24], iTDC[25], iTDC[26], iTDC[27], iTDC[28], iTDC[29], iTDC[30],iTDC[31], iTDC[32], iTDC[33], iTDC[34], iTDC[35],
							 iADC[0], iADC[1], iADC[2], iADC[3], iADC[4], iADC[5],  iADC[6], iADC[7], iADC[8], iADC[9], iADC[10], iADC[11], iADC[12],iADC[13], iADC[14], iADC[15], iADC[16], iADC[17],
							 iADC[18], iADC[19], iADC[20], iADC[21], iADC[22], iADC[23],  iADC[24], iADC[25], iADC[26], iADC[27], iADC[28], iADC[29], iADC[30],iADC[31], iADC[32], iADC[33], iADC[34], iADC[35]
							 );
	    
	    if (iCpID>=129 && iCpID<=140)
	      colECAL->addElement( lBlock );
	    else
	      colHCAL->addElement( lBlock );
	    
	  }
	
	
	LcioEventNr++; //Event start from 1.
	evt_new->setRunNumber(  runNumber  ) ;
	evt_new->setEventNumber( LcioEventNr ) ;
	evt_new->setDetectorName( _detectorTypeName);

	LCTime evttime( Timestamp ) ;//
	
	evt_new->setTimeStamp( evttime.timeStamp()/1  ) ;
	evt_new->addCollection( colECAL, _outputColNameECAL );
	evt_new->addCollection( colHCAL, _outputColNameHCAL );
	evt_new->addCollection( colHodo1, _outputColNameHodo1 );
	evt_new->addCollection( colHodo2, _outputColNameHodo2 );

	//if temperature collection is in the readoutcycle, save it in the first built event
	if( temperaturecollection_write==true) {

	  for (unsigned int i = 0; i < rTemp.size(); i++ ) 
	    {      
	      EUDAQTempSensorBlock * tBlock = new EUDAQTempSensorBlock(rTemp.at(i).iLDANumber ,rTemp.at(i).iPortNumber,
								       rTemp.at(i).iT1,rTemp.at(i).iT2,rTemp.at(i).iT3,rTemp.at(i).iT4 ,rTemp.at(i).iT5 ,rTemp.at(i).iT6 ,
								       rTemp.at(i).iTDIF ,rTemp.at(i).iTPWR );
	      colTemp2->addElement( tBlock );
	    }
	  evt_new->addCollection( colTemp2, "TemperatureSensor" );
	  temperaturecollection_write=false;
	  rTemp.clear();
	}

	ProcessorMgr::instance()->processEvent( evt_new ) ;

	// ------------ IMPORTANT ------------- !
	// we created the event so we need to delete it ...
	delete evt_new ;

      } // loop map key to save the event with each CycleNr.
    
    rData.clear();

    throw marlin::SkipEventException(this); 

  }// look into collection

 

  void EUDAQEventBuilder2016_woBIF::end() {
    streamlog_out(MESSAGE) <<" ===== End of EUDAQEventBuilder2016_woBIF processor ===== " <<  endl;
    cout <<" ===== End of EUDAQEventBuilder2016_woBIF processor ===== " <<  endl;
    cout <<"All events                                                    = " << all_events <<  endl;
    cout <<" Number of discarded events (wrong hb/gb) = " << discarded_events <<  endl;
  }

  void EUDAQEventBuilder2016_woBIF::printParameters() {

    cout <<" ===== Parameters EUDAQEventBuilder2016_woBIF processor ===== " <<  endl;
    cout << "Intput Collections : " << _inputColName  << "\t" << _inputColNameTemp << endl;
    cout << "Output Collections : " << _outputColNameECAL << "\t" << _outputColNameHCAL << endl;
  
  }
  
} //namespace
