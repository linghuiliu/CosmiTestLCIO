
#include "LabviewConverter.hh"

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

#include "LConverter.hh"
#include "LabviewBlock.hh"
#include "TempSensorBlock.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;


namespace marlin{


  LabviewConverter aLabviewConverter ;

  
  LabviewConverter::LabviewConverter() : DataSourceProcessor("LabviewConverter")
  {
    
    _description = "Reads the ascii ahcal labview raw data files. Make sure to not specify any LCIOInputFiles in the steering in order to read the Calice native raw data files ." ;
    
    registerProcessorParameter( "Data" , 
				"Initial String to the ahcal data file. i.e. 'Run'"  ,
				_data ,
				std::string("run270001.txt") ) ;
  
    registerProcessorParameter ( "Runnumber", "Runnumber", _runNumber, 270000);    
 
    registerProcessorParameter ( "SlowControlLineNumber", 
				 "First part of Slow Control Block, HBU 120 lines, EPT 1920 lines,  AHC2M New Labview output 0 lines",
				 _SlowControlLineNumber, 0);    

    registerProcessorParameter( "DetectorType" ,
                                "Type name of the detector. Currently valid identifiers are: AHC2, AHC2M AEC" ,
                                _detectorTypeName,
                                std::string("AHC2M") ) ;


 
  }
  
  LabviewConverter*  LabviewConverter::newProcessor() { 
    return new LabviewConverter ;
  }
  
  void LabviewConverter::init() {    
    // printParameters() ;    

    //_lConverter.init();
  }

    
  void LabviewConverter::readDataSource( int numEvents ) {
   
    /*
    if( _detectorTypeName == "AEC" ){
      std::setlocale(LC_ALL,"ja_JP");
      std::setlocale(LC_TIME, "ja_JP");
    }
    */

    ifstream file_to_read;
    file_to_read.open( _data.c_str() );
    
    int rn = _runNumber;
    
    LCRunHeaderImpl* runHdr = new LCRunHeaderImpl ; 
    runHdr->setRunNumber( rn ) ;
    
    string detName("ahcal_next_generation")  ;
    runHdr->setDetectorName( detName ) ;
    
    stringstream description ; 
    description << " run: " << rn <<" is the raw data from Labview !" ;
    runHdr->setDescription( description.str()  ) ;
    
    ProcessorMgr::instance()->processRunHeader( runHdr  ) ;
    
    int read[12];
    
    int currentCycleNr = 1;
    int lastCycleNr = 1;
    bool CycleNrChanged = false;
    
    int LcioEventNr = 0;
    
    LConverter _ldata;
    
    std::string line;
    
    size_t foundDate;
    string strDate ("Date");

    size_t foundUnixtime;
    string strUnixtime ("Unixtime");


    size_t foundTepmetrature = 0;
    string strTemperature("Temperatures");
    int TemperatureTimeRange[2] = {0,0};
    bool writeoutTemperature = false;
    float TSensor[6] = {0,0,0,0,0,0};

    int iEvtTime = 0;
    
    int SlowControlLineCounter = 0; //First 1920 lines in each CERN testbeam

    // **********************************************************
    //        Processing data
    //  build events according the the different BunchXID 
    //  within one CycleNr, and remove them, 
    //  until all collected data have been asigned to an event.
    //  Then readin the next CycleNr raw data.
    // **********************************************************
    while (!file_to_read.eof()) {
      
      for( int i = 0; i<12; i++) { read[i] = -1;}
      
      std::getline(file_to_read, line);
      //if(line[0] == '#') continue; //for comments
      
      if(line[0] == '#'){
	
	//Read SlowControl block
	//Read first 1920 lines for EPT SlowControl, HBU is 120 lines.
	if (SlowControlLineCounter < _SlowControlLineNumber )
	  {
	    streamlog_out(MESSAGE) << line <<std::endl;
	    SlowControl << line;
	    SlowControlLineCounter++;
	  }



	//Read on Date/Time
	foundDate = line.find(strDate);
	if (foundDate!=string::npos) {
	  //line.erase (0,20);
	  //size_t foundDate2 = line.find(".");
	  //if (foundDate2!=string::npos) {
	  //  line.erase (foundDate2,1);
	  //}
	  streamlog_out(MESSAGE) <<" These events have been collected on date: " << line <<endl; 
	  
	  // convert this string into timestamp
	  struct tm tm;
	  //if ( strptime(line.c_str(), "%d %m %Y %H:%M:%S", &tm) != NULL ){
	  if( _detectorTypeName == "AHC2"){
	    line.erase (0,20);
	    if ( strptime(line.c_str(), "%d. %b %Y %H:%M:%S", &tm) != NULL ){
	      time_t evtTimeDAQ;
	      evtTimeDAQ = mktime(&tm);
	      iEvtTime = (int)evtTimeDAQ;
	      streamlog_out(MESSAGE) <<" And the Unix UTC Timestamp  is "  << evtTimeDAQ <<endl;
	    }
	    else {
	      cout <<"Something wrong for the Timestamp in detector AHC2!"<<endl;
	    }
	  }
	  else if( _detectorTypeName == "AEC"){
	    line.erase (0,20);
	    
	    if ( strptime(line.c_str(), "%d. %b %Y %H:%M:%S", &tm) != NULL ){
	      time_t evtTimeDAQ;
	      evtTimeDAQ = mktime(&tm);
	      iEvtTime = (int)evtTimeDAQ;
	      streamlog_out(MESSAGE) <<" And the Unix UTC Timestamp  is "  << evtTimeDAQ <<endl;
	    }
	    else {
	      cout <<"Something wrong for the Timestamp in detector AEC!"<<endl;
	      
	    }
	  }
	  else {
	    cout <<"Something wrong for the Timestamp!"<<endl;
	    return;
	  }
	}

	foundUnixtime = line.find(strUnixtime);
	if (foundUnixtime!=string::npos) {

	  line.erase (0,12);
	  stringstream(line)>>iEvtTime;

	  streamlog_out(MESSAGE) <<" These events have been collected on date: " << line <<endl; 


	}



	//Read temperature
	foundTepmetrature = line.find(strTemperature);

	if (foundTepmetrature!=string::npos) {

	  //Lastone
	  TemperatureTimeRange[1] = TemperatureTimeRange[0];
	  TemperatureTimeRange[0] = iEvtTime;

	  if(writeoutTemperature)
	    streamlog_out(MESSAGE) <<TSensor[0]<<"   "
			   <<TSensor[1]<<"   "
			   <<TSensor[2]<<"   "
			   <<TSensor[3]<<"   "
			   <<TSensor[4]<<"   "
			   <<TSensor[5]<<"   "
			   <<"From "<<TemperatureTimeRange[0]
			   <<"  until "<<TemperatureTimeRange[1]
			   <<std::endl;
	  
	  //writeoutTemperature = true;
	  writeoutTemperature = false;

	  //New one
	  line.erase (0,17);

	  streamlog_out(MESSAGE) <<" And at this moment, the Temperatures are: " <<std::endl;
	  streamlog_out(MESSAGE) << line <<std::endl;
	  stringstream stream;


	  if( _detectorTypeName == "AHC2") {
	    stream.imbue(std::locale("german"));
	    //stream.imbue(std::locale("De_DE"));
	    stream << line;
	    stream>>TSensor[0]>>TSensor[1]>>TSensor[2]>>TSensor[3]>>TSensor[4]>>TSensor[5];
	  }
	  else if ( _detectorTypeName == "AEC") {
	     stringstream(line)>>TSensor[0]>>TSensor[1]>>TSensor[2]>>TSensor[3]>>TSensor[4]>>TSensor[5];
	  }
	  else{
	    std::cout << "This detecot type: "<< _detectorTypeName <<" is not valid identifiers currently!\n"
		      << "Currently valid identifiers are: AHC2 and AEC "<< std::endl;
	    return;
	  }

	}
	
	continue; //for comments
      }
      
      //  Sebastian Laurien : Email 5/23/2013 10:26 AM
      // where it was before: (dummy being unimportant stuff)
      // BXid>>Readoutcycle>>ASICid>>dummy>>memcell>>Ch>>TDC>>ADC>>dummy>>dummy>>HitBit>>GainBit
      //stringstream(line)>>read[0]>>read[1]>>read[2]>>read[3]>>read[4]>>read[5]>>read[6]>>read[7]>>read[8]>>read[9]>>read[10]>>read[11];


      //  Sebastian Laurien : Email 5/23/2013 10:26 AM
      //it is now
      //Readoutcycle>>BXid>>ASICid>>memcell>>Ch>>TDC>>ADC>>HitBit>>GainBit;
      stringstream(line)>>read[1]>>read[0]>>read[2]>>read[4]>>read[5]>>read[6]>>read[7]>>read[10]>>read[11];
      read[3] = 0;
      read[8] = 0;
      read[9] = 0;
       
      streamlog_out(DEBUG0) <<"BunchXID: "<<read[0] <<"  CycleNr: "<<read[1]<<" ChipID: "<<read[2]<<"  Channel: " <<read[5] <<std::endl;
      
      
      currentCycleNr =read[1];
      
      
      if( lastCycleNr != currentCycleNr )
	{
	  streamlog_out(DEBUG0) <<"lastCycleNr: "<< lastCycleNr<<"  currentCycleNr:"<<currentCycleNr <<std::endl;
	  CycleNrChanged = true;
	  lastCycleNr = currentCycleNr;
	  
	  if( CycleNrChanged )
	    {
	      streamlog_out(DEBUG0) <<" _ldata.size(): "<<_ldata.GetBunchXID().size() <<std::endl;
	      
	      //swap BunchXID and EvtNr for CERN test beam data in November,
	      //The known issue for the Labview DAQ wrong output for the BunchXID and EvtNr readout
	      //_ldata.swapBunchXID();
	      //_ldata.swapEvtNr();

	      _ldata.Reverse();
	      //Splite event according bunchXID within one CycleNr, count event number.
	      
	      int TotalRcdNr = _ldata.GetEvtNr().size();
	      int MaxRcdEvtNr = _ldata.GetBunchXID().size()/36; //maximium events number
	      
	      streamlog_out(DEBUG0) << "TotalRcdNr: " <<TotalRcdNr
			    << "     MaxRcdEvtNr: " <<MaxRcdEvtNr
			    << std::endl;
	      
	      
	      for( int iEv = 0; iEv < MaxRcdEvtNr; iEv++ )
		{
		  
		  if ( _ldata.GetBunchXID().size() == 0) break;
		  
		  //streamlog_out(MESSAGE)<< " ===== LabviewConverter processing Event "<< LcioEventNr <<"  ======" <<std::endl;
		  LCEventImpl*  evt = new LCEventImpl() ;
		  
 
		  LCCollectionVec *col = new LCCollectionVec( LCIO::LCGENERICOBJECT );
		  
		  int size = _ldata.GetBunchXID().size() -1;
		  int currentBunchXID = _ldata.GetBunchXID().at( size );
		  
		  streamlog_out(DEBUG0) <<" size: "<<size <<std::endl;
		  
		  bool trigger1 =false;
		  bool trigger2 =false;
		  
		  //for( unsigned int i=0; i<_ldata.GetBunchXID().size(); i++) {
		  for( int i = size; i >= 0;  --i) {
		    
		    streamlog_out(DEBUG0) <<"_ldata.GetBunchXID().at("<<i<<"): "<<_ldata.GetBunchXID().at(i)
				  <<" currentBunchXID: " << currentBunchXID
				  <<std::endl;

		    if ( _ldata.GetBunchXID().at(i) == currentBunchXID )
		      {
			// _ldata. PrintParameters(i);
			
			int iBxID = _ldata.GetBunchXID().at(i);
			int iCyNr = _ldata.GetCycleNr().at(i);
			int iCpID = _ldata.GetChipID().at(i);
			int iAsNr = _ldata.GetASICNr().at(i);
			int iEvNr = _ldata.GetEvtNr().at(i);
			int iChan = _ldata.GetChannel().at(i);
			int iTDC  = _ldata.GetTDC().at(i);
			int iADC  = _ldata.GetADC().at(i);
			int iXPos = _ldata.GetXPos().at(i);
			int iYPox = _ldata.GetYPos().at(i);
			int iHBit = _ldata.GetHitBit().at(i);
			int iGBit = _ldata.GetGainBit().at(i);

			// First event TDC is wrong. It is "0".
			// Two triggers have the same BunchXID, both triggers exist
			// Then write it out as an event.
			if( _detectorTypeName == "AHC2"){
			  if(iCpID == 129 && iChan == 35 && iTDC > 0) trigger1 =true;
			  if(iCpID == 137 && iChan == 35 && iTDC > 0) trigger2 =true;
			}
			else if( _detectorTypeName == "AHC2M"){
			  if(iCpID == 133 && iChan == 35 && iTDC > 0) trigger1 =true;
			  if(iCpID == 141 && iChan == 35 && iTDC > 0) trigger2 =true;
			}
			else if( _detectorTypeName == "AEC"){
			  // Write out all data
			  trigger1 =true;
			  trigger2 =true;
			}
			else{
			  std::cout << "This detector type: "<< _detectorTypeName <<" is not valid identifiers currently!\n"
				    << "Currently valid identifiers are: AHC2 and AEC "<< std::endl;
			  return;
			}

			LabviewBlock * lBlock = new LabviewBlock(iBxID, iCyNr, iCpID, iAsNr, iEvNr, iChan, 
								 iTDC, iADC, iXPos, iYPox, iHBit, iGBit);
			
			streamlog_out(DEBUG0) <<"iBxID: "<<iBxID<<" iCyNr: "<<iCyNr<<" iCpID: "<<iCpID
				      <<" Channel: " <<iChan <<" i: "<<i <<std::endl;
			
			
			_ldata.Erase(i);
			col->addElement( lBlock );

			
			
		      } // Add event element
		    
		  } // complete one event
		     
		  // keep this event if both trigger1 and trigger2 are true
		  // else delete this collection "col".
		  if (trigger1 && trigger2)
		    {
		      LcioEventNr++; //Event start from 1.
		      
		      evt->setRunNumber(  rn   ) ;
		      evt->setEventNumber( LcioEventNr ) ;
		      LCTime evttime( iEvtTime ) ;
		      evt->setTimeStamp( evttime.timeStamp()  ) ;
		      evt->addCollection( col, "LabviewData" );

		      //if(foundTepmetrature!=string::npos){
		      if (writeoutTemperature){
			LCCollectionVec *TCol = new LCCollectionVec( LCIO::LCGENERICOBJECT );
			
			for( int i = 0; i<6; i++){
			  float temp = TSensor[i];	    
			  TempSensorBlock  *TSensor = new TempSensorBlock(i, temp);
			  TCol->addElement(TSensor );
			}
			
			evt->addCollection( TCol, "TSensor");
			foundTepmetrature = 0;
			writeoutTemperature = false;
		      }
		  
		      ProcessorMgr::instance()->processEvent( evt ) ;

		      trigger1 = false;
		      trigger2 = false;

		    }
		  else{  delete col;  }

		  // ------------ IMPORTANT ------------- !
		  // we created the event so we need to delete it ...
		  delete evt ;
		 
		     
		} //get next event in this cycle
	      
	      _ldata.Clear();
	      
	      streamlog_out(DEBUG0) <<" After clear, _ldata.size(): "
			    <<_ldata.GetBunchXID().size() <<std::endl;
	      
	      CycleNrChanged = false;
	      
	    } //Complete splite the event in this cycle

	  
	} // if the current Cycle is completed read in
      

      _ldata.SetLConverter(read);


    }


    file_to_read.close();
    
    delete runHdr ;
    
  }
  
 
 
  void LabviewConverter::end() {    
    streamlog_out(MESSAGE) <<" ===== End of LabviewConverter processor ===== " << std::endl;
  }

} //namespace
