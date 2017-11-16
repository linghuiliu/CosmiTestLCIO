#include "TcmtMappingIProcessor.hh"
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "TcmtHit.hh"
#include "AdcBlock.hh"

namespace CALICE {

  TcmtMappingIProcessor aTcmtMappingIProcessor;

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  TcmtMappingIProcessor::TcmtMappingIProcessor()
    : VRawADCValueProcessor("TcmtMappingIProcessor")
  {
    _description = "This processor reads rawCalorimeterHits and applies part I of the mapping.";
    
    registerProcessorParameter("OutputCollectionName", "Name of the output collection",
			       _outputColName, string("CaliceHitsLevel1"));
    
    registerProcessorParameter("ViewMapping", 
			       "View the channels/strips mapping whenever location/connection conditions change (set to 0 or !=0)",
			       _viewConnectionTree,
			       0);
    
    registerProcessorParameter("PickModule",
			       "Select only a single module to be processed",
			       _pickModule,
			       0);			     
  }
  

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void TcmtMappingIProcessor::init() 
  {
    streamlog_out(DEBUG)<<"*** TcmtMappingI: init() called"<< endl;
    printParameters();
    
    stringstream message;
    message << "TcmtMapI: undefined conditions data: ";
    bool error = false;
    try 
      {
	streamlog_out(DEBUG)<<"TcmtMappingI: init(): calling VRawADCValue::init()..."<< endl;
	VRawADCValueProcessor::init();
      }
    catch (ErrorMissingConditionsDataHandler &conddata_error) 
      {
	// --- catch conditions data handler registration errors
	//   ... and build a combined error message.
	string a(conddata_error.what());
	error = true;
	if (a.size() > 0) 
	  {
	    a.erase(a.size()-1);
	    message << a; 
	  }
      }
    // FIXME: shouldn't this be moved inside the try block above?
    if(error) {
      message <<  ".";
      throw ErrorMissingConditionsDataHandler(message.str());
    }
    
    _mapping.setViewConnectionTree(_viewConnectionTree!=0);
    streamlog_out(DEBUG)<<"*** TcmtMappingI: init() done!"<< endl;
  }
  

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void TcmtMappingIProcessor::processEvent(LCEvent* evt) 
  {
    LCCollection* inVector = NULL;
    try 
      {
	inVector = evt->getCollection(_adcColName);
      }
    catch(DataNotAvailableException &e) 
      {
	streamlog_out(DEBUG)<<"Collection " << _adcColName<<" not available" << endl;
      }
    if(inVector == NULL) return;
    
    streamlog_out(DEBUG)<<"\n\nprocess EVENT="<<evt->getEventNumber()<< endl;

    LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::RAWCALORIMETERHIT);
    assert( _outputCol );
    
    LCFlagImpl hitFlag(_outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_TIME);
    hitFlag.setBit(LCIO::CHBIT_ID1);
    _outputCol->setFlag(hitFlag.getFlag());
    
    streamlog_out(DEBUG)<<"Input collection "<<_adcColName<<" has "<<inVector->getNumberOfElements()<<" elements"<<endl;

    /*------------------------ loop over adc blocks ------------------------------------*/
    for (int i = 0; i < inVector->getNumberOfElements(); i++) 
      {
	AdcBlock* _adcBlock = static_cast<AdcBlock*>(inVector->getElementAt(i));
	short crate = _adcBlock->getCrateID();
	
	if( !_mapping.isValidCrate(crate) ) continue;
	
	short slot    = _adcBlock->getSlotID();
	short fe      = _adcBlock->getBoardFrontEnd();
	short channel = _adcBlock->getMultiplexPosition();
	
	try 
	  {
	    for(short kadc = 0; kadc < 12; ++kadc) 
	      {
		unsigned elecChannel = _adcBlock->getElecChannel(kadc);
		unsigned chip = (elecChannel & 0xF);
		
		const TcmtConnection* connection = _mapping.getTcmtConnection( crate, slot, fe, chip, channel );
		if( connection == NULL ) continue;
		
		short cass = connection->getModuleID();
		
		// if user picks a module, skip all other modules
		if( _pickModule!=0 && ((cass & 0xFF00) >> 8 != _pickModule)) continue;
		
		float _energy = _adcBlock->getAdcVal(kadc);
		int fastID = connection->getFastID();
		TcmtHit *aTcmtHit = new TcmtHit(fastID, _energy, 0., 0);

		streamlog_out(DEBUG)<<"slot/fe/chip/chan="<< slot <<"/"<< fe <<"/"<< chip <<"/"<< channel
			    <<", fastID="<< hex << fastID << dec <<" energy="<<_energy
			    <<" moduleID="<<cass<<" stripID="<<connection->getStripID()<< endl;
		
		if(aTcmtHit) _outputCol->addElement(aTcmtHit); 
	      }
	  }
	catch(Exception& e) 
	  { 
	    cout << " Exception: " << e.what() << endl;
	  }
      }
    
    evt->addCollection(_outputCol, _outputColName);
    
  }
  

  
}
