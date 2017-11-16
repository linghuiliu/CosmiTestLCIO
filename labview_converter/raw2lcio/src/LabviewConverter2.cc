#include "LabviewConverter2.hh"

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
#include <string>

#include "LConverter.hh"
#include "LabviewBlock2.hh"
#include "TempSensorBlock2.hh"
#include "TempSensorBlockOld.hh"

using namespace std;
using namespace lcio;
using namespace CALICE;

namespace marlin{


  LabviewConverter2 aLabviewConverter2 ;


  LabviewConverter2::LabviewConverter2() : DataSourceProcessor("LabviewConverter2")
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

  LabviewConverter2*  LabviewConverter2::newProcessor() {
    return new LabviewConverter2 ;
  }

  void LabviewConverter2::init() {
    printParameters() ;
    nCorruptedCycle = 0;

    //_lConverter.init();
  }


  void LabviewConverter2::readDataSource( int numEvents ) {
	
    /*
      if( _detectorTypeName == "AEC" ){
      std::setlocale(LC_ALL,"ja_JP");
      std::setlocale(LC_TIME, "ja_JP");
      }
    */
	
    ifstream file_to_read;
    file_to_read.open( _data.c_str() );
	
    if (file_to_read == NULL){
      streamlog_out(ERROR0) << "Text input file not exists -- Check PATH in steering file!!!" << std:: endl;
      return;
    }
	
    int rn = _runNumber;
	
    LCRunHeaderImpl* runHdr = new LCRunHeaderImpl ;
    runHdr->setRunNumber( rn ) ;
	
    string detName("ahcal_next_generation")  ;
    runHdr->setDetectorName( detName ) ;
	
    stringstream description ;
    description << " run: " << rn <<" is the raw data from Labview !" ;
    runHdr->setDescription( description.str()  ) ;
	
    ProcessorMgr::instance()->processRunHeader( runHdr  ) ;
	
    int read[9];
    //int read[12];
    int nCol = 9;
    //int nCol = 12;
	
    int currentCycleNr = -1;
    int lastCycleNr = -1;
    bool CycleNrChanged = false;
    bool DiscardCycle = false;

    int LcioEventNr = 0;
	
    map< int, vector<rawData> > rData;
    map<int, int> nHitsPerChipMap;//Map for number of hits per Chips
    IntVec nHitsPerChip;//vector to store the number of hits per Chips per event

    rawData blockData;
	
    string line;
	
    size_t foundDate;
    string strDate ("Date");
	
    size_t foundUnixtime;
    string strUnixtime ("Unixtime");
	
	
    size_t foundTemperature = 0;
    size_t foundTemperature2 = 0;
    size_t foundTemperature3 = 0;
    string strTemperature("LDA:");
    string strTemperature2("# A5");
    string strTemperature3("# AM01");
    int TemperatureTimeRange[2] = {0,0};
    bool writeoutTemperature = true;

    bool newTemp = true;
    bool oldTemp = false;

    //FIX ME ONLY FOR 15 LAYERS ?? VECTOR???
    string TSensor[15][10];
    string lda[15], port[15];
    /**////////////////////////

    int index = 0;
	
    int iEvtTime = 0;
	
    int SlowControlLineCounter = 0; //First 1920 lines in each CERN testbeam

    cout << "Start Read Data" << endl;
	
    // **********************************************************
    //        Processing data
    //  build events according to the different BunchXID
    //  within one CycleNr, and remove them,
    //  until all collected data have been asigned to an event.
    //  Then readin the next CycleNr raw data.
    // **********************************************************
    while (!file_to_read.eof()) {
	
      for( int i = 0; i<nCol; i++) { read[i] = 0;}
	
      getline(file_to_read, line);
      //if(line[0] == '#') continue; //for comments
	
      if(line[0] == '#'){
	//Read SlowControl block
	//Read first 1920 lines for EPT SlowControl, HBU is 120 lines.
	if (SlowControlLineCounter < _SlowControlLineNumber )
	  {
	    streamlog_out(MESSAGE) << line <<endl;
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
	  if( _detectorTypeName == "AHC2M"){
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
		
	  streamlog_out(MESSAGE) <<" These events have been collected on date (Unix): " << line <<endl;
		
		
	}
		
		
		
	//Read temperature
	foundTemperature = line.find(strTemperature);
		
	if (foundTemperature!=string::npos) {
	  streamlog_out(MESSAGE) <<" Found Temperature New format " <<endl;
	  //Lastone
	  TemperatureTimeRange[1] = TemperatureTimeRange[0];
	  TemperatureTimeRange[0] = iEvtTime;
	  if(index == 14) index = 0;
	  newTemp = true;
	  oldTemp = false;
	  writeoutTemperature = true;

	  if(writeoutTemperature)
	    {
			
	      stringstream strstr;
	      strstr.str(line);

	      if( _detectorTypeName == "AHC2M") {
			
		streamlog_out(MESSAGE) <<" And at this moment, the Temperatures are: " <<endl;
			
		getline(strstr, TSensor[index][0], '\t');
		lda[index] = TSensor[index][0].substr(7,1);
		port[index] = TSensor[index][0].substr(14,1);
			
		strstr >> TSensor[index][1] >> TSensor[index][2] >> TSensor[index][3] >> TSensor[index][4] >> TSensor[index][5] >> TSensor[index][6] >> TSensor[index][7] >> TSensor[index][8];
		streamlog_out(MESSAGE) << index << " " << lda[index] << " " << port[index] <<"   " <<TSensor[index][1]<<"   "<<TSensor[index][2]<<"   "<<TSensor[index][3]<<"   "<<TSensor[index][4]<<"   "<<TSensor[index][5]<<"   "<<TSensor[index][6]<<"   "<<TSensor[index][7]<<"   "<<TSensor[index][8]<<endl;
		index++;
	      }
	      else{
		cout << "This detecot type: "<< _detectorTypeName <<" is not valid identifiers currently!\n"
		     << "Currently valid identifiers are: AHC2M and AEC "<< endl;
		return;
	      }
			
	    }
	}

	//Read temperature Old Serial
	foundTemperature2 = line.find(strTemperature2);
	foundTemperature3 = line.find(strTemperature3);
	if (foundTemperature2!=string::npos || foundTemperature3!=string::npos) {

	  streamlog_out(MESSAGE) <<" Found Temperature Old format " <<endl;
	  newTemp = false;
	  oldTemp = true;
	  if(index > 14) index = 0;
	  //Lastone
	  TemperatureTimeRange[1] = TemperatureTimeRange[0];
	  TemperatureTimeRange[0] = iEvtTime;
	  writeoutTemperature = true;

	  if(writeoutTemperature)
	    {
			
	      stringstream strstr;
	      strstr.str(line);
			
	      if( _detectorTypeName == "AHC2M") {
			
		streamlog_out(MESSAGE) <<" And at this moment, the Temperatures are: " <<endl;
			
		strstr  >> TSensor[index][0] >> TSensor[index][1] >> TSensor[index][2] >> TSensor[index][3] >> TSensor[index][4] >> TSensor[index][5] >> TSensor[index][6] >> TSensor[index][7] >> TSensor[index][8] >> TSensor[index][9];
		streamlog_out(MESSAGE) << index << " " << TSensor[index][0] <<"   " <<TSensor[index][1] <<"   "<<TSensor[index][2]<<"   "<<TSensor[index][3]<<"   "<<TSensor[index][4]<<"   "<<TSensor[index][5]<<"   "<<TSensor[index][6]<<"   "<<TSensor[index][7]<<"   "<<TSensor[index][8]<<"   "<<TSensor[index][9]<<endl;
		index++;
	      }
	      else{
		cout << "This detecot type: "<< _detectorTypeName <<" is not valid identifiers currently!\n"
		     << "Currently valid identifiers are: AHC2M and AEC "<< endl;
		return;
	      }
			
	    }
	}
		
	continue; //for comments in the txt file start with "#"
      }
	
      int index_max = index;
	
      //  Sebastian Laurien : Email 5/23/2013 10:26 AM
      //it is now
      //Readoutcycle>>BXid>>ASICid>>memcell>>Ch>>TDC>>ADC>>HitBit>>GainBit;
      stringstream(line)>>read[0]>>read[1]>>read[2]>>read[3]>>read[4]>>read[5]>>read[6]>>read[7]>>read[8];
	
      //if(read[0] == 0) continue;
      //stringstream(line)>>read[0]>>read[1]>>read[2]>>read[3]>>read[4]>>read[5]>>read[6]>>read[7]>>read[8]>>read[9]>>read[10]>>read[11];
      //0:BunchXID 1:CycleNr 2:ChipID2 3:Smt 4:EvtNr 5:chan 6:TDC 7:ADC (or 6:ADC_HG 7:ADC_LG) 8:Smt 9:Smt 10:Hit_Bit 11:Gain_Bit
	
      //In convert_root: &BunchXID, &CycleNr, &ChipID2, &EvtNr, &chn, &TDC, &ADC, &Hit_Bit, &Gain_Bit
	
      blockData.iCyNr = read[0];
      blockData.iBxID = read[1];
      blockData.iCpID = read[2];
      blockData.iEvNr = read[3];
      blockData.iChan = read[4];
      blockData.iTDC  = read[5];
      blockData.iADC  = read[6];
      blockData.iHBit = read[7];
      blockData.iGBit = read[8];
	
      //Shaojun's order
      /*
	blockData.iCyNr = read[1];
	blockData.iBxID = read[0];
	blockData.iCpID = read[2];
	blockData.iEvNr = read[4];
	blockData.iChan = read[5];
	blockData.iTDC  = read[7];
	blockData.iADC  = read[6];
	blockData.iHBit = read[10];
	blockData.iGBit = read[11];
      */

      //Check for BXID out of range (over 4095)
      if(read[1] > 4095)
	{
	  DiscardCycle = true;
	}
	
      streamlog_out(DEBUG4) <<"  BXID: "<<read[1] <<" CycleNr: "<<read[0] <<" ChipID: "<<read[2]<<"  Channel: " <<read[4] << std::endl;
	
      currentCycleNr = read[0];
      streamlog_out(DEBUG4) <<"lastCycleNr "  << lastCycleNr << " currentCycleNr " << currentCycleNr << endl;
	
      if( lastCycleNr != currentCycleNr )
	{
	  streamlog_out(DEBUG4) <<"(lastCycleNr != currentCycleNr) : lastCycleNr: "<< lastCycleNr<<"  currentCycleNr:"<<currentCycleNr << endl;
	  CycleNrChanged = true;
		
	  if( CycleNrChanged && !DiscardCycle )//Discard the cycle if strange BXID
	    {
	      streamlog_out(DEBUG4) <<" rData.size(): "<<rData.size() << endl;
		
	      for(map< int, vector<rawData> >::iterator it=rData.begin(); it!=rData.end();it++)
		{
		  //streamlog_out(MESSAGE)<< " ===== LabviewConverter2 processing Event "<< LcioEventNr <<"  ======" << endl;
		  LCEventImpl*  evt_new = new LCEventImpl() ;
			
		  LcioEventNr++; //Event start from 1.						
		  evt_new->setRunNumber(  rn   ) ;
		  evt_new->setEventNumber( LcioEventNr ) ;

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
			
			
		      LabviewBlock2 * lBlock = new LabviewBlock2(iCyNr, iBxID, iCpID, iEvNr, iChan,
								 iTDC, iADC, iHBit, iGBit);
		
		      if(nHitsPerChipMap.count(iCpID) == 0)
			nHitsPerChipMap[iCpID] = 0;

		      if(iHBit == 1)
			{
			  //HitsPerChips (number of triggered channels)
			  nHitsPerChipMap[iCpID]++;
			}

		      if (iCpID>=129 && iCpID<=140)
			colECAL->addElement( lBlock );
		      else{
			colHCAL->addElement( lBlock );
				
		      }
		    }

		  nHitsPerChip.clear();
		  for(map<int, int>::iterator it2 = nHitsPerChipMap.begin(); it2 != nHitsPerChipMap.end(); ++it2)
		    {
		      if(it2->second != 0)
			{
			  nHitsPerChip.push_back(it2->first*100+it2->second);
			}
		    }
		  nHitsPerChipMap.clear();

		  streamlog_out(DEBUG0)<<"--------------- Writing Event Parameters -------------------" <<endl;
		  streamlog_out(DEBUG0)<<"BXID: "<< it->first <<endl;
		  streamlog_out(DEBUG0)<<"HitPerChips: "<< nHitsPerChip.size() << endl;

		  //add event parameter
		  evt_new->parameters().setValue("BXID", it->first);
		  evt_new->parameters().setValues("nHitsPerChip", nHitsPerChip);

		  LCTime evttime( iEvtTime ) ;
		  if(evttime.timeStamp() != 0) {
		    evt_new->setTimeStamp( evttime.timeStamp()  ) ;
		    evt_new->addCollection( colECAL, "LabviewDataECAL" );
		    evt_new->addCollection( colHCAL, "LabviewDataHCAL" );
		  }
		  else
		    {
		      //No timestamp!
		      continue;
		    }

		  if (writeoutTemperature && newTemp){
		    LCCollectionVec *TCol = new LCCollectionVec( LCIO::LCGENERICOBJECT );
			
		    for(int i = 0; i < index_max; i++)
		      {
			int LDANr = atoi(lda[i].c_str());
			int PortNr = atoi(port[i].c_str());
			int T1 = atoi(TSensor[i][1].c_str());
			int T2 = atoi(TSensor[i][2].c_str());
			int T3 = atoi(TSensor[i][3].c_str());
			int T4 = atoi(TSensor[i][4].c_str());
			int T5 = atoi(TSensor[i][5].c_str());
			int T6 = atoi(TSensor[i][6].c_str());
			int TDIF = atoi(TSensor[i][7].c_str());
			int TPWR = atoi(TSensor[i][8].c_str());
				
			TempSensorBlock2  *TSensor = new TempSensorBlock2(LDANr, PortNr, T1, T2, T3, T4, T5, T6, TDIF, TPWR);
			TCol->addElement(TSensor );
		      }
		    evt_new->addCollection( TCol, "TempSensor");
		    foundTemperature = 0;
		    writeoutTemperature = false;
		    newTemp = false;
		    index = 0;
		  }
		  else if(writeoutTemperature && oldTemp){
		    LCCollectionVec *TCol = new LCCollectionVec( LCIO::LCGENERICOBJECT );
		    streamlog_out(MESSAGE) <<" Adding Temperature Collection TempSensorOld" <<endl;
		    for(int i = 0; i < index_max; i++)
		      {
			float serial = -1;

			//streamlog_out(MESSAGE) << TSensor[i][1] << endl;

			if(TSensor[i][1] == "A500XEBO") serial = 4;
			if(TSensor[i][1] == "A500XEBP") serial = 11;
			if(TSensor[i][1] == "A500XEBR") serial = 5;
			if(TSensor[i][1] == "A500XEBS") serial = 9;
			if(TSensor[i][1] == "A500XEBX") serial = 8;
			if(TSensor[i][1] == "A500XEBY") serial = 12;
			if(TSensor[i][1] == "A500XEBZ") serial = 6;
			if(TSensor[i][1] == "A500XEC0") serial = 10;
			if(TSensor[i][1] == "A500XET5") serial = 13;
			if(TSensor[i][1] == "AM01PONF") serial = 15;
			if(TSensor[i][1] == "AM01PONG") serial = 14;
			if(TSensor[i][1] == "AM01PONI") serial = 7;
			if(TSensor[i][1] == "AM01PONJ") serial = 3;
			if(TSensor[i][1] == "AM01PONK") serial = 1;
			if(TSensor[i][1] == "AM01PONM") serial = 2;

			float T1 = atof((TSensor[i][2].substr(0,4)).c_str());
			float T2 = atof((TSensor[i][3].substr(0,4)).c_str());
			float T3 = atof((TSensor[i][4].substr(0,4)).c_str());
			float T4 = atof((TSensor[i][5].substr(0,4)).c_str());
			float T5 = atof((TSensor[i][6].substr(0,4)).c_str());
			float T6 = atof((TSensor[i][7].substr(0,4)).c_str());
			float TDIF = atof((TSensor[i][8].substr(0,4)).c_str());
			float TPWR = atof((TSensor[i][9].substr(0,4)).c_str());
				
			TempSensorBlockOld  *TSensor = new TempSensorBlockOld(serial, T1, T2, T3, T4, T5, T6, TDIF, TPWR);
			TCol->addElement(TSensor );
		      }
		    evt_new->addCollection( TCol, "TempSensorOld");
		    streamlog_out(MESSAGE) <<" Added Temperature Collection TempSensorOld" <<endl;
		    foundTemperature2 = 0;
		    foundTemperature3 = 0;
		    writeoutTemperature = false;
		    oldTemp = false;
		    index = 0;
		  }

		  ProcessorMgr::instance()->processEvent( evt_new ) ;
			
		  // ------------ IMPORTANT ------------- !
		  // we created the event so we need to delete it ...
		  delete evt_new;
			
		} // loop map key to save the event with each CycleNr.
		
	    } // finished the current CycleNr.
	  else
	    {
	      streamlog_out(WARNING) << " ===== Strange BXID!!! ===== " << std::endl;
	      streamlog_out(WARNING) << "Cycle " << lastCycleNr << " Discarded" << std::endl;
	      nCorruptedCycle++;
	    }

	  lastCycleNr = currentCycleNr;
	  // start next CycleNr.
	  CycleNrChanged = false;
	  DiscardCycle = false;
	  rData.clear();

	} // if the current Cycle is completed read in
	
      streamlog_out(DEBUG4) <<"lastCycleNr==currentCycleNr : lastCycleNr: "<< lastCycleNr<<"  currentCycleNr:"<<currentCycleNr << endl;
      rData[read[1]].push_back(blockData);
    }
	
	
    file_to_read.close();
	
    delete runHdr ;
	
  }



  void LabviewConverter2::end() {
    streamlog_out(MESSAGE) <<" ===== End of LabviewConverter2 processor ===== " <<  endl;
    streamlog_out(MESSAGE) <<"nCorruptedCycle: " << nCorruptedCycle << endl;
  }

} //namespace
