// ********************************************************
// *         One class to hold the ahcal Labview data     *
// *    -- Split and convert into lcio event              *
// ********************************************************
//
// $Id: LConverter.hh,v 0.0 2012/08/16 S.Lu Exp $

#ifndef LConverter_hh
#define LConverter_hh 1
#include <vector>
#include <iomanip>
#include <algorithm>    // std::reverse

class LConverter
{
public:
  LConverter() {}
  ~LConverter(void) {}

  inline std::vector<int> GetBunchXID() const
  { return _dataBunchXID; }

  inline std:: vector<int> GetCycleNr() const
  { return _dataCycleNr; }

  inline std:: vector<int> GetChipID() const
  { return _dataChipID; }

  inline std:: vector<int> GetASICNr() const
  { return _dataASICNr; }

  inline std:: vector<int> GetEvtNr() const
  { return _dataEvtNr; }

  inline std:: vector<int> GetChannel() const
  { return _dataChannel; }

  inline std:: vector<int> GetTDC() const
  { return _dataTDC; }

  inline std:: vector<int> GetADC() const
  { return _dataADC; }

  inline std:: vector<int> GetXPos() const
  { return _dataXPos; }

  inline std:: vector<int> GetYPos() const
  { return _dataYPos; }

  inline std:: vector<int> GetHitBit() const
  { return _dataHitBit; }

  inline std:: vector<int> GetGainBit() const
  { return _dataGainBit; }
  


  //GetNextEvent();
  //SpliteEvent();
  //WriteoutEvent();


  void SetBunchXID(int BunchXID )
  { _dataBunchXID.push_back(BunchXID); }

  void SetCycleNr(int CycleNr )
  { _dataCycleNr.push_back(CycleNr); }

  void SetChipID(int ChipID )
  { _dataChipID.push_back(ChipID); }

  void SetASICNr(int ASICNr )
  { _dataASICNr.push_back(ASICNr); }

  void SetEvtNr(int EvtNr )
  { _dataEvtNr.push_back(EvtNr); }

  void SetChannel(int Channel )
  { _dataChannel.push_back(Channel); }

  void SetTDC(int TDC )
  { _dataTDC.push_back(TDC); }

  void SetADC(int ADC )
  { _dataADC.push_back(ADC); }

  void SetXPos(int XPos )
  { _dataXPos.push_back(XPos); }

  void SetYPos(int YPos )
  { _dataYPos.push_back(YPos); }

  void SetHitBit(int HitBit )
  { _dataHitBit.push_back(HitBit); }

  void SetGainBit(int GainBit )
  { _dataGainBit.push_back(GainBit); }


  void SetLConverter(int Data[] )
  {
    SetBunchXID( Data[0] );
    SetCycleNr( Data[1] );
    SetChipID( Data[2] );
    SetASICNr( Data[3] );
    SetEvtNr( Data[4] );
    SetChannel( Data[5] );
    SetTDC( Data[6] );
    SetADC( Data[7] );
    SetXPos( Data[8] );
    SetYPos( Data[9] );
    SetHitBit( Data[10] );
    SetGainBit( Data[11] );
  }


  void Clear( void )
  {
    _dataBunchXID.clear();
    _dataCycleNr.clear();
    _dataChipID.clear(); 
    _dataASICNr.clear(); 
    _dataEvtNr.clear();  
    _dataChannel.clear();
    _dataTDC.clear();    
    _dataADC.clear();    
    _dataXPos.clear();   
    _dataYPos.clear();   
    _dataHitBit.clear(); 
    _dataGainBit.clear();
  }

  void Erase( int i )
  {
    _dataBunchXID.erase(_dataBunchXID.begin()+i);
    _dataCycleNr.erase(_dataCycleNr.begin()+i);
    _dataChipID.erase(_dataChipID.begin()+i); 
    _dataASICNr.erase(_dataASICNr.begin()+i); 
    _dataEvtNr.erase(_dataEvtNr.begin()+i);  
    _dataChannel.erase(_dataChannel.begin()+i);
    _dataTDC.erase(_dataTDC.begin()+i);    
    _dataADC.erase(_dataADC.begin()+i);    
    _dataXPos.erase(_dataXPos.begin()+i);   
    _dataYPos.erase(_dataYPos.begin()+i);   
    _dataHitBit.erase(_dataHitBit.begin()+i); 
    _dataGainBit.erase(_dataGainBit.begin()+i);
  }

  void Reverse ( void )
  {
    std::reverse(_dataBunchXID.begin(), _dataBunchXID.end());
    std::reverse(_dataCycleNr.begin(), _dataCycleNr.end());
    std::reverse(_dataChipID.begin(), _dataChipID.end());
    std::reverse(_dataASICNr.begin(), _dataASICNr.end());
    std::reverse(_dataEvtNr.begin(), _dataEvtNr.end());
    std::reverse(_dataChannel.begin(), _dataChannel.end());
    std::reverse(_dataTDC.begin(), _dataTDC.end());
    std::reverse(_dataADC.begin(), _dataADC.end());
    std::reverse(_dataXPos.begin(), _dataXPos.end());
    std::reverse(_dataYPos.begin(), _dataYPos.end());
    std::reverse(_dataHitBit.begin(), _dataHitBit.end());
    std::reverse(_dataGainBit.begin(), _dataGainBit.end());
  }
 
  void swapBunchXID ( void )
  {
    //swap the wrong order BunchXID in the Labview output
    //=======================================================	      
    int NCHIPS = 0;
    std::vector<int> worngBunchXID[16];
    
    int ichip = -1;
    
    int iChipID =  0;
    int maxIndex =  _dataBunchXID.size();
    
    for( int i =0; i < maxIndex; i++)
      {
	if( iChipID != _dataChipID.at(i) )
	  {
	    ichip++;
	    NCHIPS++;
	    iChipID = _dataChipID.at(i);
	  }
	worngBunchXID[ichip].push_back( _dataBunchXID.at(i) );
      }
    
    int maxCount = 0;;
    for(int ichip=0;ichip<NCHIPS;ichip++){
      maxCount += worngBunchXID[ichip].size();
    }

    std::cout<<"maxCount: "<<maxCount 
	     <<" maxIndex: "<<maxIndex
	     <<std::endl;
    
    if (maxCount == maxIndex ){
      
      int index = -1;
      for(int ichip=0;ichip<NCHIPS;ichip++){
	for (unsigned int k = 0 ; k < worngBunchXID[ichip].size(); k++){
	  index++;
	  _dataBunchXID[index] =
	    worngBunchXID[ichip].at(worngBunchXID[ichip].size()-1-k);
	}
	
      }
      
      for(int ichip=0;ichip<NCHIPS;ichip++){
	worngBunchXID[ichip].clear();
      }
      
    }
    else {std::cout <<"ERROR: something is wrong in the swap Labview wrong BunchXID" <<std::endl;}
    
    //=======================================================	      
    //End of the swap the wrong order BunchXID in the Labview output
    
  }

  void SwapEvtNr ( void )
  {
    //swap the wrong order EvtNr in the Labview output
    //=======================================================	      
    int NCHIPS = 0;
    std::vector<int> worngEvtNr[16];
    
    int ichip = -1;
    
    int iChipID =  0;
    int maxIndex =  _dataEvtNr.size();
    
    for( int i =0; i < maxIndex; i++)
      {
	if( iChipID != _dataChipID.at(i) )
	  {
	    ichip++;
	    NCHIPS++;
	    iChipID = _dataChipID.at(i);
	  }
	worngEvtNr[ichip].push_back( _dataEvtNr.at(i) );
      }
    
    int maxCount = 0;;
    for(int ichip=0;ichip<NCHIPS;ichip++){
      maxCount += worngEvtNr[ichip].size();
    }

    std::cout<<"maxCount: "<<maxCount 
	     <<" maxIndex: "<<maxIndex
	     <<std::endl;
    
    if (maxCount == maxIndex ){
      
      int index = -1;
      for(int ichip=0;ichip<NCHIPS;ichip++){
	for (unsigned int k = 0 ; k < worngEvtNr[ichip].size(); k++){
	  index++;
	  _dataEvtNr[index] =
	    worngEvtNr[ichip].at(worngEvtNr[ichip].size()-1-k);
	}
	
      }
      
      for(int ichip=0;ichip<NCHIPS;ichip++){
	worngEvtNr[ichip].clear();
      }
      
    }
    else {std::cout <<"ERROR: something is wrong in the swap Labview wrong EvtNr" <<std::endl;}
    
    //=======================================================	      
    //End of the swap the wrong order EvtNr in the Labview output
    
  }
 
  void PrintParameters(int i)
  {

    std::cout <<std::setw ( 5 )<<_dataBunchXID.at(i)
	      <<std::setw ( 5 )<<_dataCycleNr.at(i)
	      <<std::setw ( 5 )<<_dataChipID.at(i)
	      <<std::setw ( 5 )<<_dataASICNr.at(i)
	      <<std::setw ( 5 )<<_dataEvtNr.at(i)
	      <<std::setw ( 5 )<<_dataChannel.at(i)
	      <<std::setw ( 5 )<<_dataTDC.at(i)
	      <<std::setw ( 5 )<<_dataADC.at(i)
	      <<std::setw ( 5 )<<_dataXPos.at(i)
	      <<std::setw ( 5 )<<_dataYPos.at(i)
	      <<std::setw ( 5 )<<_dataHitBit.at(i)
	      <<std::setw ( 5 )<<_dataGainBit.at(i)
	      <<std::endl;
  }
  

  void PrintParametersLabel()
  {

    std::cout <<std::setw ( 5 )<<"#BunchXID"
	      <<std::setw ( 5 )<<"#CycleNr"
	      <<std::setw ( 5 )<<"#ChipID"
	      <<std::setw ( 5 )<<"#ASICNr"
	      <<std::setw ( 5 )<<"#EvtNr"
	      <<std::setw ( 5 )<<"#Channel"
	      <<std::setw ( 5 )<<"#TDC"
	      <<std::setw ( 5 )<<"#ADC"
	      <<std::setw ( 5 )<<"#XPos"
	      <<std::setw ( 5 )<<"#YPos"
	      <<std::setw ( 5 )<<"#HitBit"
	      <<std::setw ( 5 )<<"#GainBit"
	      <<std::endl;

   
  }

private:
  std::vector<int>    _dataBunchXID;
  std::vector<int>    _dataCycleNr;
  std::vector<int>    _dataChipID; 
  std::vector<int>    _dataASICNr; 
  std::vector<int>    _dataEvtNr;  	
  std::vector<int>    _dataChannel; 
  std::vector<int>    _dataTDC;    	
  std::vector<int>    _dataADC;    	
  std::vector<int>    _dataXPos;   	
  std::vector<int>    _dataYPos;   	
  std::vector<int>    _dataHitBit; 
  std::vector<int>    _dataGainBit; 

};

#endif
