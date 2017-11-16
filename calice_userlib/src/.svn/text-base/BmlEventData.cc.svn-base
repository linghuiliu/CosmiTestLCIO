#include "BmlEventData.hh"


BmlEventDataSup* BmlEventData::_bml_event_sup=0; 
bool BmlEventData::_isWarning1Printed=false;
bool BmlEventData::_isWarning2Printed=false;
bool BmlEventData::_isWarning3Printed=false;

namespace CALICE{

  void BmlEventData::addTDCChannels(const TDCChannelContainer_t& tdcChannelContainer) {
 
    //the initial position in the generic object
    int pos = kBmlEventIntValues;
    
    //Set the number of signal channels
    setNumberOfSignalChannels(static_cast<int>(tdcChannelContainer.size()));

    //loop over all channels
    for(TDCChannelContainer_t::const_iterator tdcchan_iter =
	  tdcChannelContainer.begin(); tdcchan_iter !=
	  tdcChannelContainer.end();tdcchan_iter++){
      //Fill the channel numberBmlCaen767EventDataDriver.cc
      obj()->setIntVal(pos, static_cast<int>( (*tdcchan_iter).first ));
      //The number of signals for this channel
      obj()->setIntVal(++pos, static_cast<int>( (*tdcchan_iter).second.size() ));
      pos++;
      int pos_startword = pos;
      //The word which will contain the starttime indicators
      int startword(0);
      //A counter for the signsls
      unsigned int imeas(0);
      //Now retrieve and fill the signals 
      for(std::vector< std::pair<bool,int> >::const_iterator tdcvec_iter = (*tdcchan_iter).second.begin(); tdcvec_iter != (*tdcchan_iter).second.end(); tdcvec_iter++) {
        pos++;
        //Get the pair of starttime indicator and time measurement
	std::pair<bool, int> thepair = (*tdcvec_iter);
        //Fill up the startword sequentially
        startword = (startword | ((static_cast<int>(thepair.first) &
				   0x1) << imeas));   
        obj()->setIntVal(pos, thepair.second );
        imeas++;
      }
      //Set the startword
      obj()->setIntVal(pos_startword, startword);
      pos++;
    }
    
  }
  
  /** A method which re-builds and returns the TCChannelContainer*/
  const TDCChannelContainer_t& BmlEventData::getTDCChannelContainer(){

    //Reset the  TDCChannelContainer
    _tdcChannelContainer.clear();
    

    //The number of channels which carry a signal in this board
    unsigned int numchannel( static_cast<unsigned int>(getNumberOfSignalChannels()) );

    //The position where the TDC Channel info start 
    int pos = kBmlEventIntValues;

    for( unsigned int numhandled(0); numhandled < numchannel; numhandled++) {
      unsigned int channum(getIntVal(pos));
      unsigned int numsig(getIntVal(++pos));
      unsigned int startword(getIntVal(++pos) );
      //the vector which will contain the starttime indicators and the
      //measured times
      std::vector< std::pair <bool, int> > tdcchan_vec;
      tdcchan_vec.resize(0);
      //Retrieve the starttime indicators and the measured time
      for (unsigned int isig(0); isig < numsig; isig++){
	int startbool = (( startword >> isig) & 0x1);
	tdcchan_vec.push_back(make_pair<bool, int>(static_cast<bool>(startbool), getIntVal(++pos)));
      }
      //Fill the values for the given channel
       _tdcChannelContainer.insert( std::make_pair(channum, tdcchan_vec));
       //Set the position to the next channel
      pos++;

    }

    return _tdcChannelContainer;
  } 


   std::ostream& BmlEventData::print(std::ostream &ostrm)  
   {
      
     ostrm << " BmlEventData: "  << std::endl;
     ostrm << " BoardID: " << std::hex << BoardID(getBoardID()) << std::endl;
     ostrm << " Base Address: " << getBaseAddress() << std::dec << std::endl;
     ostrm << " Record Label: " << getRecordLabel() << std::endl;
     ostrm << " Status Register: " << std::hex << getStatusRegister() << std::dec << std::endl;
     ostrm << " Number of words: " << getNumberOfWords() << std::endl; 
     ostrm << " Geo Address: " << std::hex << getGeoAddress() << std::dec << std::endl; 
     ostrm << " Event Number: " << getEventNumber() << std::endl; 
     ostrm << " Status: " << getStatus() << std::endl;
     ostrm << " EventDataCounter: " << getEventDataCounter() << std::endl;
     ostrm << " Number of channels with signal: " << getNumberOfSignalChannels() << std::endl;
     if( getNumberOfSignalChannels() > 0) { 
       ostrm << " The channels follow: " << std::endl;
       ostrm << " Falling edges are indicated by a minus sign " << std::endl;
       getTDCChannelContainer();
       for(TDCChannelContainer_t::iterator tdcchan_iter = _tdcChannelContainer.begin(); tdcchan_iter != _tdcChannelContainer.end(); tdcchan_iter++){
	 ostrm << "TDC Channel Number: " << static_cast<unsigned int>((*tdcchan_iter).first) << std::endl;
	 ostrm << "Number of measured signals: " << (*tdcchan_iter).second.size() <<std::endl;
	 for (std::vector< std::pair<bool,int> >::iterator tdcvec_iter = (*tdcchan_iter).second.begin();  tdcvec_iter != (*tdcchan_iter).second.end(); tdcvec_iter++ ) {
	   std::pair<bool, int> thepair = (*tdcvec_iter);
	   ostrm << "is StartTime?: " << thepair.first << std::endl;
	   ostrm << "Measured time: " << thepair.second << std::endl;
	 }   
       }//

     }
     return ostrm;
   }



 
}
