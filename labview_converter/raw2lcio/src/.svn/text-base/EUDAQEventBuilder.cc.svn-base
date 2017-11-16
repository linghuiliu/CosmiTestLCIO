#include "EUDAQEventBuilder.hh"

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

#include "EUDAQBlock.hh"
#include "EUDAQTempSensorBlock.hh"


using namespace std;
using namespace lcio;
using namespace CALICE;


/**
 * Processor to read SLCIO EUDAQ files and sort them according to the BXID 
 * (now they are sorted in readout cycles) 
 * (either a separation between ECAL and AHCAL collections is done by EUDAQ) 
 * @author A. Irles, based on the LabviewBlock2 writen by S. Lu DESY Hamburg
 * @date May 20 2015
 * Created for 2015 testbeams EUDAQ data format.
 */


namespace CALICE{
    
    
  EUDAQEventBuilder aEUDAQEventBuilder ;
    

    
  EUDAQEventBuilder::EUDAQEventBuilder() : Processor("EUDAQEventBuilder")
  {
    _description = "Processor to built events from raw EUDAQ slcio files";

    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection of EUDAQ raw data",
			       _inputColName,
			       std::string("EUDAQDataScCAL"));
    
    registerProcessorParameter("InputCollectionNameTemp",
    			       "Name of the input temperature collection",
    			       _inputColNameTemp,
    			       std::string("TempSensor"));

   registerProcessorParameter("DetectorType",
    			       "Name of the Detector",
    			       _detectorTypeName,
    			       std::string(""));

    registerProcessorParameter("OutputCollectionNameECAL",
			       "Name of the ouput collection of the sorted events for the ECAL ",
			       _outputColNameECAL,
			       std::string("EUDAQDataECAL"));

    registerProcessorParameter("OutputCollectionNameHCAL",
			       "Name of the ouput collection of the sorted events for the HCAL ",
			       _outputColNameHCAL,
			       std::string("EUDAQDataHCAL"));
    
  }


  EUDAQEventBuilder::~EUDAQEventBuilder() {}


    
  void EUDAQEventBuilder::init() {

  }


  void EUDAQEventBuilder::processEvent(LCEvent* evt){
  

    int runNumber  = evt->getRunNumber();//airqui
    int Timestamp ;//
        
    int read[9];
    rawData blockData;
                  
    int temp[10];
    rawTemp blockTemp;

    std::map< int, std::vector<rawData> > rData;        
    std::vector<rawTemp>  rTemp;       
    std::map<int, int> nHitsPerChipMap;//Map for number of hits per Chips
    IntVec nHitsPerChip;//vector to store the number of hits per Chips per event
 
    LCCollection* col ; //data collection
    LCCollection* colTemp ; //temp collection

    try
      {
	//fetch EUDAQ data unsorted collection
	col = evt->getCollection( _inputColName ) ;

	const std::vector<std::string> *cnames = evt->getCollectionNames();

	Timestamp= col->getParameters().getIntVal("Timestamp_i");//TimeStamp();//airqui

	for(int ielm = 0; ielm < col->getNumberOfElements(); ielm++)
	  {
	    
	    LCObject *obj = col->getElementAt(ielm);
	    EUDAQBlock lBlock(obj);
	    
	    read[0] = lBlock.GetCycleNr();
	    read[1] = lBlock.GetBunchXID();
	    read[2] = lBlock.GetChipID();
	    read[3] = lBlock.GetEvtNr();
	    read[4] = lBlock.GetChannel();
	    read[5] = lBlock.GetTDC();
	    read[6] = lBlock.GetADC();
	    read[7] = lBlock.GetHitBit();
	    read[8] = lBlock.GetGainBit();
	    
	    streamlog_out(DEBUG4) <<"  read SLCIO "<<lBlock.GetCycleNr()<<" "<<lBlock.GetEvtNr()<<" "<<lBlock.GetBunchXID()<<" "<<lBlock.GetChannel()<<" "<<lBlock.GetChipID()<<" "<<lBlock.GetTDC()<<" "<<lBlock.GetADC()<<" "<<lBlock.GetHitBit()<<" "<<lBlock.GetGainBit()<<endl;//airqui
	    
	    blockData.iCyNr = read[0];
	    blockData.iBxID = read[1];
	    blockData.iCpID = read[2];
	    blockData.iEvNr = read[3];
	    blockData.iChan = read[4];
	    blockData.iTDC  = read[5];
	    blockData.iADC  = read[6];
	    blockData.iHBit = read[7];
	    blockData.iGBit = read[8];
	    
	    
	    streamlog_out(DEBUG4) <<"  CycleNr: "<<read[0] <<" BXID: "<<read[1] <<" ChipID: "<<read[2]<<"  Channel: " <<read[4] << std::endl;


	    rData[read[1]].push_back(blockData);
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
      

    for(map< int, vector<rawData> >::iterator it=rData.begin(); it!=rData.end();it++ )
      {
	
	//streamlog_out(MESSAGE)<< " ===== EUDAQEventBuilder processing Event "<< LcioEventNr <<"  ======" << endl;
	LCEventImpl*  evt_new = new LCEventImpl() ;
	
       	LCCollectionVec *colTemp2= new LCCollectionVec( LCIO::LCGENERICOBJECT );

	LCCollectionVec *colECAL = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	LCCollectionVec *colHCAL = new LCCollectionVec( LCIO::LCGENERICOBJECT ); 
	for (unsigned int i = 0; i < it->second.size(); i++ )
	  {
	    int iBxID = it->second.at(i).iBxID;
	    int iCyNr = it->second.at(i).iCyNr;
	    int iCpID = it->second.at(i).iCpID;
	    int iEvNr = it->second.at(i).iEvNr;
	    int iChan = it->second.at(i).iChan;
	    int iTDC  = it->second.at(i).iTDC;
	    int iADC  = it->second.at(i).iADC;
	    int iHBit = it->second.at(i).iHBit;
	    int iGBit = it->second.at(i).iGBit;
	    
	    
	    EUDAQBlock * lBlock = new EUDAQBlock(iCyNr, iBxID, iCpID, iEvNr, iChan,
						 iTDC, iADC, iHBit, iGBit);

	    if(nHitsPerChipMap.count(iCpID) == 0)
	      nHitsPerChipMap[iCpID] = 0;

	    if(iHBit == 1)
	      nHitsPerChipMap[iCpID]++;
	    
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
	
	evt_new->setTimeStamp( evttime.timeStamp()  ) ;

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

 

  void EUDAQEventBuilder::end() {
    streamlog_out(MESSAGE) <<" ===== End of EUDAQEventBuilder processor ===== " <<  endl;
  }
  
} //namespace
