#include <ExtractBeamEnergyProcessor.hh>



using namespace marlin;
using namespace lcio;
using namespace std;


ExtractBeamEnergyProcessor aExtractBeamEnergyProcessor;

ExtractBeamEnergyProcessor::ExtractBeamEnergyProcessor() : Processor("ExtractBeamEnergyProcessor"){
  _description = "Extract the beam energy and particle type for a given run number from the MetaDataFolder of the Database (info originally from eLog)";


  registerInputCollection( LCIO::LCGENERICOBJECT, 
			   "BeamMetaDataCollectionName" ,
			   "Name of the BeamMetaDataFolder from database" ,
			   _BeamMetaDataColName ,
			   std::string("BeamMetaDataCollectionName") ) ; 
 
  
}

ExtractBeamEnergyProcessor::~ExtractBeamEnergyProcessor(){}


void ExtractBeamEnergyProcessor::init(){

  
  printParameters();
  int error=false;

  
  if (!ConditionsProcessor::registerChangeListener( this,  _BeamMetaDataColName ))
    {
      streamlog_out(ERROR) << " undefined conditions: " << _BeamMetaDataColName << std::endl;
      error=true;
    } 
  
  if (error)
   {
     throw marlin::StopProcessingException(this);
   } 

}



void ExtractBeamEnergyProcessor::conditionsChanged( LCCollection *col )
{
  
  string colName = col->getParameters().getStringVal("CollectionName") ;
  if (colName == _BeamMetaDataColName)
    {
      _colBeamMetaData = col;
      _beamMetaDataChanged = true;
    }
   
} 


void ExtractBeamEnergyProcessor::processEvent(LCEvent* evt){
  BeamEnergy= 0.;
  ParticleTyePDG= 0;
  
  if ( _beamMetaDataChanged ) {
    beamMetaData =  static_cast<CALICE::BeamMetaData*>(_colBeamMetaData->getElementAt(0));
    BeamEnergy = beamMetaData->getEnergy(); 
    ParticleTyePDG = beamMetaData->getPdgCode();
  }

   
  evt->parameters().setValue( "BeamEnergy", BeamEnergy);
  evt->parameters().setValue( "ParticleTyePDG", ParticleTyePDG);  
  
  // streamlog_out(DEBUG) << "BeamEnergy: " << evt->getParameters().getFloatVal("BeamEnergy")<<endl;;
  //streamlog_out(DEBUG) << "ParticleTypePDG: " << evt->getParameters().getIntVal("ParticleTyePDG")<<endl;;
  
  


  ExtractBeamEnergyProcessor aExtractBeamEnergyProcessor;
}

void ExtractBeamEnergyProcessor::end()
{
  streamlog_out(DEBUG) << "Run with beam energy of "<< BeamEnergy<<"GeV and PDG particle type "<< ParticleTyePDG <<endl;
  streamlog_out(DEBUG) << "ExtractBeamEnergyProcessor end." <<endl;
  
}


