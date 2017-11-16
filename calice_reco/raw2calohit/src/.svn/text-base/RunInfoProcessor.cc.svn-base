#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include "RunInfoProcessor.hh"

// -- LCIO headers
#include <EVENT/LCCollection.h>
#include <EVENT/LCIO.h>
#include <EVENT/LCRunHeader.h>
#include <EVENT/LCParameters.h>
#include <EVENT/CalorimeterHit.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <UTIL/LCTOOLS.h>

// -- marlin headers
//#include "marlin/ConditionsProcessor.h"
#include "marlin/Global.h"


// -- lccd headers
#include "lccd/LCConditionsMgr.hh"

// -- userlib headers
#include "collection_names.hh"
#include "BmlSlowRunDataBlock.hh"
#include "RunTimeWhizard.hh"
#include "BeamParameterException.hh"
#include "DBInitString.hh"


using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace marlin {
  RunInfoProcessor aRunInfoProcessor ;
  
  
  RunInfoProcessor::RunInfoProcessor() : Processor("RunInfoProcessor"), _vers_string(""), _vers_length(5)
					 //,_experimentalSetupChange(this,&RunInfoProcessor::experimentalSetupChanged),
                                         //_bmlSroRunDataChanged(this, &RunInfoProcessor::setBmlSroRunDataCol),_bmlSroRunDataCol(0)
  {
    // modify processor description
    _description = "Add Runinfo to Runheader needed for MC and DATA" ;
    
    registerProcessorParameter( "DBInit" ,
				"initialisation of the database " ,
				_dbInit ,
				DBInitString() );
    
    
    //Folder to be searched for the runtime information
    //Fixme: we need to make sure that the folders we select here correspond to the 
    //Run we have at hand
    registerProcessorParameter( "RunTimeFolder",
				"Collection from DB which contains the start amd stop time of a given run" ,
				_folderRunTime,
				std::string("/cd_calice/CALDAQ_RunTimeInfo"));
    
    
    //Folder to be searched for the runlocationinformation
    //Fixme: we need to make sure that the folders we select here correspond to the 
    //Run we have at hand
    registerProcessorParameter( "RunLocationFolder",
				"Collection from DB which contains information about the location of a run" ,
				_folderLocation,
				std::string("/cd_calice/RunLocation"));
    
    
    
    //The beam parameter folder
    //The string here will be taken if not being set to default 
    //If this is not default the user is required to set also the two parameter by hand
    //In that case the user is completely responsible for the correct choice of the folder 
    //name.
    //Setting the value to "non_existing" will force the query of the experimental setup folder
    registerProcessorParameter( "BeamParameterFolder",
				"Folder in which the beam parameters are stored" ,
				_folderBeamParameter,
				std::string("default"));
    
    registerProcessorParameter("CurrentTolerance",
                               "Tolerance in % allowed until current reading at run begin and run end is considered to be inconsinstent", 
                               _currentTolerance,
                               static_cast<float>(0.05));
    
    registerProcessorParameter("TimeTolerance",
                               "Tolerance in s allowed until reading of beam parameters (at run begin) is considered to be outdated", 
                               _timeTolerance,
                               static_cast<float>(600));
    
    
    //The folder to fall back if no beam parameters are available or if they are odd
    //mandatory for desy runs
    registerProcessorParameter( "BeamParameterExceptionFolder",
				"Folder in which beam parameters are stored if not in datastream or if odd" ,
				_folderBeamParameterException,
				std::string("/cd_calice/BeamParameterException"));
    
    
    registerProcessorParameter( "BendingCurrentToCheck",
				"The bending current to be checked to calculate the energy from the current" ,
				_bendingCurrentToCheck,
				8);
    
    //FIXME not sure whether we need to keep these parameters all the tim
    //In principle the class containing the beam parameters should know how a beam energy is
    //calculated
    registerProcessorParameter( "CurrentToEnergy",
				"Conversion factor from bending magnet current to energy" ,
				_currentToEnergy,
				static_cast<float>(4.72));
    
    registerProcessorParameter( "KeVToMeV",
				"Convert calculated Beam Energy into MeV" ,
				_toMeV,
				static_cast<float>(1000.));
    
    registerProcessorParameter( "RunNumber" ,
                                "Run number of the run for MC files, in case not written to RunHEader"
                                "A run number in the run header ALWAYS superseeds the steering",
                                _runNumber,
                                int(0)) ;

    registerProcessorParameter( "SafetyMargin",
                                "Apply a safety margin of n seconds between the "
                                "nominal run start and the first MC event "
                                "time stamp to ensure validity of conditions "
                                "data",
                                _safetyMargin,
                                int(0));

   registerProcessorParameter( "OverwriteRunNumber" ,
                                "Overwrite existing run number in run header",
                                _overwriteRunNumber,
                                bool(false)) ;
  }


  RunInfoProcessor::~RunInfoProcessor() {
    if ( _runlocationwhizard ) delete _runlocationwhizard;
    if ( _runInformation ) delete _runInformation;
  }
  
  void RunInfoProcessor::init() {//init method
    
    printParameters() ;
    
    _nRun    = 0;
    _nEvt    = 0;
    
    //CAMM: do we need the ConditionsChangeDelegators? We know the parameters are likely to change each run: why not updating each run anyway directly as done here?
    //marlin::ConditionsProcessor::registerChangeListener(&_bmlSroRunDataChanged,_colBmlSroRunData);
  }//end init method
  
  void RunInfoProcessor::processRunHeader( LCRunHeader* run) {

    _runInformation = new RunInformation();
    
    cout << endl << " ---> RunInfoProcessor::Processing Run Number : " << run->getRunNumber() << endl;
    
    //#ifdef RECO_DEBUG
     LCTOOLS::dumpRunHeader( run ) ;
     //#endif

    setVersString(run->getDescription());

    //set the MC flag
    bool type = (run->getDetectorName() != "Calice Prototypes") ||  (run->getDescription().find("Mokka") != run->getDescription().npos);
    
    _runInformation->isMC(type);
    
    // set run number for MC or if requested for data, too
    //plus an (updated) description
    std::stringstream description;
    //hard coded (oouah) -> to be changed
    description << run->getDescription() << "====for reconstruction====" << std::endl 
		<< " calice_userlib " << "v06-04" << std::endl
		<< " calice_reco v06-06-01" << std::endl
		<< " lcio  v02-03-03" << std::endl
		<< " marlin  v01-04-01" << std::endl
		<< " lccd  v01-02-01" << std::endl
		<< " CondDBMySQL_ILC-0-9-5" << std::endl;
    
    //An existing runnumber ALWAYS superseeds the steering
    if( run->getRunNumber() != 0)
      {
	if (_overwriteRunNumber == false)	  
	  _runNumber = run->getRunNumber();
      }

    LCRunModifier::setRun(run,_runNumber, description.str());
    if( _runNumber != run->getRunNumber() ) {
      std::stringstream message;
      message << "RunInfoProcessor::processRunHeader> RunNumber after runheader modification not correct" << std::endl 
      << "Run number from run header is: " << run->getRunNumber() << " Value of data member is: " << _runNumber; 
      throw std::runtime_error(message.str());
    }
    printRunParameters(run);
    
    //Initialize the run location whizard
    try{ 
    _runlocationwhizard = new RunLocationWhizard(_folderLocation, _dbInit);
    _runlocationwhizard->print(_runNumber, std::cout);
    
    //Fill Location and generic month of running
    _runInformation->location(_runlocationwhizard->getRunLocation(_runNumber));     
    _runInformation->runMonth (_runlocationwhizard->getGenericRunMonth(_runNumber));
    } catch (  lccd::LCCDException & aExc ) {
      std::cout << "RunInfoProcessor::ProcessRunHeader> Warning no well defined RunLocation Will return" << std::endl; 
      return;
    }
    //Initilaize the runtime whizard
    RunTimeWhizard runtimewhizard(_folderRunTime, _dbInit);    
    LCTime starttime(0);
    LCTime stoptime(0);
    try{
      starttime=runtimewhizard.getRunStartTime(_runNumber);
    } catch ( lccd::LCCDException & aExc ) {
      std::cout << "Warning no starttime found, will set tp 0" <<std::endl;
    }
    try{
      stoptime=runtimewhizard.getRunStopTime(_runNumber);
    } catch (  lccd::LCCDException & aExc ) {
      std::cout << "Warning no stoptime found, will set tp 0" <<std::endl;
    }
    _runInformation->runStart(starttime);    
    _runInformation->runEnd(stoptime);
    std::cout << "Start Time of Run " << _runNumber << " is: " << starttime.getDateString() << std::endl;;      
    std::cout << "Stop Time of Run " << _runNumber << " is: " << stoptime.getDateString() << std::endl;;      
    // In case of Check if the given run is shorter than the safety margin
    if(_runInformation->isMC()) {
      if (stoptime.timeStamp() - starttime.timeStamp() > _safetyMargin*NPS) {_eventTime=LCTime(starttime.timeStamp()+_safetyMargin*NPS);
      std::cout << "RunInfoProcessor> Event Time of Event after Safety is: " << _eventTime.getDateString() << std::endl; 
      }
      else {
	stringstream message;
	message << " Run shorter than given safety margin of "
		<< _safetyMargin << " seconds." << endl;
	throw runtime_error(message.str());
      }
      
    }


    //The next part of this method is dedicated to write the beam energy into the 
    //run header for mc we may consult the run header in the future
    //for data we consult the proper folder in the database
    //todo: in case of mc, comparison of mc particle momentume with the actual beam 
    //beam energy of the run which is to be simulated?
    if(_runInformation->isMC()) LCTOOLS::printParameters( run->getParameters() ) ;
    else {
      //We test the run info folder to check for a wrong version info on the lcioconverter
      //Compose the folder name
      std::string foldername_base("/cd_calice_");
      std::stringstream foldername; 
      foldername << foldername_base << _vers_string << "_" << _runlocationwhizard->getGenericRunType(_runNumber) << "/CALDAQ_RunInfo";
      //Get the conditions data handler
      IConditionsHandler* conData; 
      conData = new DBCondHandler(_dbInit,foldername.str(), "dummy");
      //update it 
      try {
	conData->update(_runInformation->runStart().timeStamp());
	//Now we can go ahead and extract the beam energy for this run
	////_runInformation->beamEnergyMeV(getBeamEnergy(_runInformation->runStart().timeStamp()));
      } catch ( lcio::Exception & aExc ) {
	//We have encountered obviously the situation where the version information needs to be patched.
	//Here we put in that we know that the problem occured in the transition between converter version
	//v04-01 and v04-02 so we query for the version v04-02 (Note no further patch will be applied).
	if(conData) delete conData;
	std::cout << "Patching description " << std::endl;
	std::stringstream thepatcheddescription;
	unsigned int loc = run->getDescription().find_first_of("\n");
	for(unsigned int ichar=0; ichar< loc+1; ichar++) thepatcheddescription << run->getDescription()[ichar];
	std::string patchstring(" calice_lcioconv v04-02 (converter version patched),\n ");
	std::string thesubstr( run->getDescription().substr(run->getDescription().find("calice_userlib")));
	thepatcheddescription << patchstring << thesubstr;
	setVersString(thepatcheddescription.str());
	LCRunModifier::setRun(run,_runNumber, thepatcheddescription.str());
	std::cout << "Re-printing Run Parameters: " << std::endl; 
	printRunParameters(run);
	//now try again with the corrected converter version
	std::stringstream foldername; 
	foldername << foldername_base << _vers_string << "_" << _runlocationwhizard->getGenericRunType(_runNumber) << "/CALDAQ_RunInfo";
	conData = new DBCondHandler(_dbInit,foldername.str(), "dummy");
	try{
	  conData->update(_runInformation->runStart().timeStamp());
	  //Now we can go ahead and extract the beam energy for this run
	  ////_runInformation->beamEnergyMeV(getBeamEnergy(_runInformation->runStart().timeStamp()));
	} catch ( lcio::Exception & aExc ) {
	  //If we're here, we simply give up 
	  std::stringstream message;
	  message << "RunInfoProcessor::processRunHeader> Failed desperately to find RunInfo Collection for this time stamp." << std::endl 
		  << "Please check the integrity of your data";
	  throw std::runtime_error(message.str());
	}
      }
      if(conData) delete conData;
      
    }
    //todo It looks as if we also need to access the number of DAQ Events here
    //... will appear somewhere here
    
    
    //_runInformation->beamEnergyMeV(getBeamEnergy((_runInformation->runStart().timeStamp())+1));
    
    //Now we have assembled the run information and can set the parameters
    _runInformation->set(run);
    _runInformation->print();

    
    //Count the number of runs 
    _nRun++ ;
  }
  
   void RunInfoProcessor::modifyEvent( LCEvent* evt ) {

     if(_runInformation->isMC()) {
       // Increase the next event time by one nano second
       _eventTime = LCTime(_eventTime.timeStamp() + 1LL);
#ifdef RECO_DEBUG
       std::cout << "RunInfoProcessor> Event Time of Event: " << evt->getEventNumber() << " is: " << _eventTime.getDateString() << std::endl; 
#endif
    // Modify the run number and the time stamp of each event
       static_cast<LCEventImpl*>(evt)->setRunNumber(_runNumber);;
       static_cast<LCEventImpl*>(evt)->setTimeStamp(_eventTime.timeStamp());
     }
  }

  void RunInfoProcessor::processEvent( LCEvent * evt )
  {//processEvent method
    //Do we need the event counter?
    //CAMM: yes, for the cout at the end....
    _nEvt ++ ;
  }//end process Event method
  
  
  void RunInfoProcessor::check( LCEvent * evt )
  {
    // nothing to check here - could be used to fill checkplots in reconstruction processor
  }
  
  
  
  void RunInfoProcessor::end()
  {
    cout << endl << "RunInfoProcessor::end()  " << name()
	 << " processed " << _nEvt << " events in " << _nRun << " runs "
	 << endl ;
  }
  
  void RunInfoProcessor::printRunParameters(const LCRunHeader *run){
    
    std::cout << "################################" << std::endl <<
      " Run parameters : " << std::endl <<
      "################################" << std::endl <<
      "-------- run number = " << run->getRunNumber() << "," << std::endl <<
      "-------- detector name : " << run->getDetectorName() << "," << std::endl <<
      "-------- description : " << run->getDescription() << "," << std::endl << 
      "-------- liste of active subdetectors : " ;
    
    const std::vector<std::string>  *vStr=run->getActiveSubdetectors();
    for (unsigned int i = 0; i < vStr->size(); i++){
      std::cout << "    " << (*vStr)[i];
    }
    std::cout << "################################" << std::endl;
    
    const LCParameters &params = run->getParameters();
    StringVec intkeys, intkeys2;
    intkeys2=params.getIntKeys(intkeys);
    std::cout << "Size of intKeys : " << intkeys.size() << ", " << intkeys2.size() << std::endl;
    for (unsigned int i = 0; i < intkeys.size(); i++){
      std::cout << " Key " << i << " = " << intkeys[i] << ", param=" << params.getIntVal(intkeys[i]) << std::endl;
    }
    
    StringVec floatkeys, floatkeys2;
    floatkeys2=params.getFloatKeys(floatkeys);
    std::cout << "Size of floatKeys : " << floatkeys.size() << ", " << floatkeys2.size() << std::endl;
    for (unsigned int i = 0; i < floatkeys.size(); i++){
      std::cout << " Key " << i << " = " << floatkeys[i] << ", param=" << params.getFloatVal(floatkeys[i]) << std::endl;
    }
    
    StringVec stringkeys, stringkeys2;
    stringkeys2=params.getStringKeys(stringkeys);
    std::cout << "Size of stringKeys : " << stringkeys.size() << ", " << stringkeys2.size() << std::endl;
    for (unsigned int i = 0; i < stringkeys.size(); i++){
      std::cout << " Key " << i << " = " << stringkeys[i] << ", param=" << params.getStringVal(stringkeys[i]) << std::endl;
    }
    
  }
  
  
  
  //Obtain the beam energy
  //set get method???
  unsigned int RunInfoProcessor::getBeamEnergy(const LCCDTimeStamp startTime) {
    std::string foldertoaccess("");
    std::stringstream foldername; 
    
    if(_folderBeamParameter == "default") {
      std::string foldername_base("/cd_calice_");
      foldername << foldername_base << _vers_string << "_" << _runlocationwhizard->getGenericRunType(_runNumber) << "/CALDAQ_BmlSroRunData" << _runlocationwhizard->getRunLocation(_runNumber);
      std::cout << "Composed Foldername for beam parameters: "<< foldername.str() << std::endl;
      _folderBeamParameter=foldername.str();     
      foldername.str("");
      foldername << "/cd_calice/BeamParameterException";
      std::cout << "Composed foldername for beam parameter exception: " << foldername.str() << std::endl;
      _folderBeamParameterException=foldername.str();     
    }  else { 
      //Now treat the user defined case
      std::cout << "User defined case: " << std::endl;
      std::cout << "Using folder: " << _folderBeamParameter << " to extract beam parameters" << std::endl;
      std::cout << "Using folder: " << _folderBeamParameterException << " in case exceptions are to be handled" << std::endl;
    }
    
    //consult function which handles beam parameters 
    //todo to be replaced by a dedicated class?
    return handleBeamParameters(startTime);
  }
  
  unsigned int RunInfoProcessor::handleBeamParameters(const LCCDTimeStamp starttime){
    
    //Initialization of variables
    unsigned int energy(0);
    IConditionsHandler* conData= new DBCondHandler(_dbInit,_folderBeamParameter, "dummy","") ;
    IConditionsHandler* conDataBeamException(0);      
    
    LCTime startTime(starttime);
    LCTime till(0);
    LCTime since(0);
    LCTime cernTimeStamp(0);
    std::string till_string("");
    std::string since_string("");
    //std::vector<double> bendCurrents;
    double bendCurBofr = 0;
    double bendCurEofr = 0;
    bool havebofr(false);
    bool haveeofr(false);
    LCCollection* colE(0);
    
    //Extract the beam parameters at run begin
    try {
      conData->update(startTime.timeStamp()) ;
      colE = conData->currentCollection();
      CALICE::BmlSlowRunDataBlock bmlSlowRunDataBlock(colE->getElementAt(0));
      //bmlSlowRunDataBlock.print(std::cout);
      bendCurBofr = bmlSlowRunDataBlock.getBendCurrents().at(_bendingCurrentToCheck);
      cernTimeStamp = bmlSlowRunDataBlock.getTimeStamp();
      till_string =  colE->getParameters().getStringVal((lccd::DBTILL).c_str());
      till = static_cast<long64>(strtoll(till_string.c_str(),0,0));
      since_string =  colE->getParameters().getStringVal((lccd::DBSINCE).c_str());
      since = static_cast<long64>(strtoll(since_string.c_str(),0,0));
      //std::cout << "till: " << till.getDateString() << std::endl;
      havebofr=true;
    } catch ( lcio::Exception & aExc ) {
      std::cout << "No beam parameter information at begin of run for run: " << _runNumber << std::endl;
    }
    
    //extract the beam parameters at run end
    try {
      //  NM: The next line is buggy
      //      'till' is the end of the validity of the beamline collection, which is valid at run start
      //      which is NOT the end of the run
      //      correct should be: till->runStopTime
      conData->update(till.timeStamp()) ;  
      //      otherwise, the next call gives back exactly the same collection as before 
      colE = conData->currentCollection();
      CALICE::BmlSlowRunDataBlock bmlSlowRunDataBlock(colE->getElementAt(0));
      //bmlSlowRunDataBlock.print(std::cout);
      bendCurEofr = bmlSlowRunDataBlock.getBendCurrents().at(_bendingCurrentToCheck);
      //      Since the collection is identical, the validity does not change and the next
      //      line does not alter 'till'
      till_string =  colE->getParameters().getStringVal((lccd::DBTILL).c_str());
      till = static_cast<long64>(strtoll(till_string.c_str(),0,0));
      haveeofr=true;
      energy=static_cast<unsigned int>(fabs(bendCurEofr)/_currentToEnergy*_toMeV);
      
    } catch ( lcio::Exception & aExc ) {
      std::cout << "No beam parameter information and end of run for run: " << _runNumber << std::endl;
    }
    
    //Handle exceptions
    //1) Reading at BoR or EoR missing or Wrong timestamp 
    if((!havebofr || !haveeofr) || abs(abs(since.unixTime()-cernTimeStamp.unixTime())-timeCor()) > _timeTolerance) {
      std::cout << "BeamParameterException detected: " << std::endl;
      std::cout << "Exists reading Begin of Run? " << havebofr  << std::endl;
      std::cout << "Exists reading End of Run? " << haveeofr  << std::endl;
      std::cout << "Difference between cern and DAQ timestamp: " << abs(abs(since.unixTime()-cernTimeStamp.unixTime())-timeCor()) << " s" << " Tolerance: " << _timeTolerance << " s" << std::endl;
      std::string colName = "BeamParameterExceptionCol";
      conDataBeamException = new lccd::DBCondHandler( _dbInit, _folderBeamParameterException, colName, "" ) ;
      try{
        conDataBeamException->update( static_cast<lccd::LCCDTimeStamp>(_runNumber));
        LCCollection* col = conDataBeamException->currentCollection();
	//Obtain the energy if possible
	BeamParameterException beamParameter(col->getElementAt(0));
#ifdef RECO_DEBUG 
	beamParameter.print(std::cout);
#endif       
	return beamParameter.getBeamEnergy();
      } catch  ( lcio::Exception & aExc ) {
        std::cout << "No beam parameter exception information for run: " << _runNumber << std::endl;
        std::cout << "Beam Energy will be set to 0 " << std::endl;
        return 0;
      }
      
      
    }
    
    //b) Inconistent readings between begin and end of run
    if( (havebofr && haveeofr) && ( fabs((bendCurBofr-bendCurEofr)/bendCurBofr) > _currentTolerance)) {
      std::cout << "BeamParameterException detected: " << std::endl;
      std::cout << "Bending current at Begin of Run: " << bendCurBofr << " A" << std::endl;
      std::cout << "Bending current at End   of Run: " << bendCurEofr << " A" << std::endl;
      std::cout << "Tolerance                      : " << _currentTolerance << " %" << std::endl;
       std::string colName = "BeamParameterExceptionCol";
       conDataBeamException = new lccd::DBCondHandler( _dbInit, _folderBeamParameterException, colName, "" ) ;
      try {
        conDataBeamException->update(static_cast<lccd::LCCDTimeStamp>(_runNumber));
	LCCollection* col = conDataBeamException->currentCollection();
	//Obtain the energy
	BeamParameterException beamParameter(col->getElementAt(0));
#ifdef RECO_DEBUG 
	beamParameter.print(std::cout);
#endif       
	return beamParameter.getBeamEnergy();
      } catch ( lcio::Exception & aExc ) {  
        std::cout << "No beam parameter exception information for run: " << _runNumber << std::endl;
        std::cout << "Beam Energy will be set to 0 " << std::endl;
        return 0;
      }
    }
    
    delete conData;
    if(conDataBeamException) delete conDataBeamException;
    std::cout << "Bending current from database is : " << bendCurEofr << " A." << std::endl;
    std::cout << "Energy from database is : " << bendCurEofr/_currentToEnergy*_toMeV << " MeV." << std::endl;
    return energy;
  }
  
  
  void RunInfoProcessor::setVersString(std::string description){
    
    //Parsing the run description string to get the converter version major and minor (not the debugging cycles)
    int ifound(0);
    std::string vers_string("");
    //Check for the place in the string in which the converter version is given
    for (unsigned int ichar=description.find_first_of(" ",description.find("calice_lcioconv"))+1; 
	 ichar < description.size();ichar++) {
      //if(run->getDescription()[ichar] != " " && atoi(run->getDescription()[ichar]) != atoi("-")){
      if( description[ichar] != ' ' && description[ichar] != '-' ){
	//std::cout << "Character at: " << ichar << " is: " << run->getDescription()[ichar] << std::endl;
	vers_string+= description[ichar];
	ifound++;
      }
      if(ifound==_vers_length) break;
    }
    _vers_string=vers_string;
    std::cout << "vers string is: " << _vers_string << std::endl;
    
  }
  
  float RunInfoProcessor::timeCor() {
    
    std::cout << std::endl;
    std::cout << "******************************************************************************************* " << std::endl;
    std::cout << "RunInfoProcessor::timeCor: Reading HARDCODED values to correct time stamps of cern database " << std::endl;
    std::cout << "TO BE CHANGED to external input!!!!!" << std::endl;    
    std::cout << "******************************************************************************************* " << std::endl;
    //The time at which cern has adjusted its clock (i.e. cern is synchronous to calice 
    if(_runInformation->runStart().timeStamp() > LCTime( 2006,8, 1, 0 , 0 , 0 ).timeStamp() && 
       _runInformation->runStart().timeStamp() < LCTime( 2006,8, 23, 3 , 0 , 0 ).timeStamp()) return 480;
    
    //cern has switched its clock from CEST -> UTC (was not recongnized by converter, not omnipotent ;-) ) 
    if(_runInformation->runStart().timeStamp() > LCTime( 2006,10, 1, 0 , 0 , 0 ).timeStamp() && 
       _runInformation->runStart().timeStamp() < LCTime( 2006,10, 29, 2 , 0 , 0 ).timeStamp()) return 7200;
    
    //General switch from CEST -> CET
    if(_runInformation->runStart().timeStamp() > LCTime( 2006,10, 29, 1 , 59 , 59 ).timeStamp() && 
       _runInformation->runStart().timeStamp() < LCTime( 2007, 3, 25, 2 , 0 , 0 ).timeStamp()) return 3600;

    //  NM: crude guess - switched back to CEST for summer 2007
    if(_runInformation->runStart().timeStamp() > LCTime( 2007, 3, 25, 2 , 0 , 0 ).timeStamp() && 
       _runInformation->runStart().timeStamp() < LCTime( 2007,10, 29, 2 , 0 , 0 ).timeStamp()) return 7200;
    return 0;
  }
  
}
