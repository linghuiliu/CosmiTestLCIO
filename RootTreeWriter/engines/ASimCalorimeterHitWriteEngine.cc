#include <cfloat>
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/MCParticle.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "ASimCalorimeterHitWriteEngine.hh"
#include <IMPL/LCCollectionVec.h>

#include "TROOT.h"

using namespace lcio;
using namespace std;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  
  /**********************************************************************/
  /*                                                                    */
  /*                                                                    */
  /*                                                                    */
  /**********************************************************************/
  void ASimCalorimeterHitWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterInputCollection(LCIO::SIMCALORIMETERHIT,
						_engineName + "_simHitInputCollection",
						"Name of SimCalorimeterHit input collections",
						_calorimCollection, 
						std::string("hcalSD") );

    _hostProcessor.relayRegisterProcessorParameter("ASimCalorimeterHitWriteEngine_caloType",
						   "ASimCalorimeterHitWriteEngine prefix to tree variables,e.g. HCAL or TCMT",
						   _prefix,
						   std::string("caloType"));

  }
  /**********************************************************************/
  /*                                                                    */
  /*                                                                    */
  /*                                                                    */
  /**********************************************************************/
  void ASimCalorimeterHitWriteEngine::registerBranches( TTree* hostTree )
  {
    if ( _prefix.size() > 0 )
      if ( _prefix[ _prefix.length()-1 ] != '_' )
	_prefix += "_";
    
    hostTree->Branch(string(_prefix+"nHitsPerEvent").c_str(), &_nHitsPerEvent, string(_prefix+"nHitsPerEvent/I").c_str());

    /* load the pre-generated root dictionary for the vector by executing:*/
    gROOT->ProcessLine("#include <vector>");

   //--------------- quantities per cell -------------------------------------------------------------------------------------------
    _IPtr = new vector<int>;
    hostTree->Branch(string(_prefix+"I").c_str(), &_IPtr);
    _JPtr = new vector<int>;
    hostTree->Branch(string(_prefix+"J").c_str(), &_JPtr);
    _KPtr = new vector<int>;
    hostTree->Branch(string(_prefix+"K").c_str(), &_KPtr);

    
    _xPtr = new vector<float>;
    hostTree->Branch(string(_prefix+"x").c_str(), &_xPtr);
    _yPtr = new vector<float>;
    hostTree->Branch(string(_prefix+"y").c_str(), &_yPtr);
    _zPtr = new vector<float>;
    hostTree->Branch(string(_prefix+"z").c_str(), &_zPtr);

    _hitEnergyPtr = new vector<float>;
    hostTree->Branch(string(_prefix+"hitEnergy").c_str(), &_hitEnergyPtr);
    _cellID0Ptr = new vector<int>;
    hostTree->Branch(string(_prefix+"cellID0").c_str(), &_cellID0Ptr);

    

   //--------------- quantities per event -------------------------------------------------------------------------------------------
    hostTree->Branch(string(_prefix+"xCog").c_str(), &_xCog, string(_prefix+"xCog/D").c_str());
    hostTree->Branch(string(_prefix+"yCog").c_str(), &_yCog, string(_prefix+"yCog/D").c_str());
    hostTree->Branch(string(_prefix+"zCog").c_str(), &_zCog, string(_prefix+"zCog/D").c_str());

	
    //--------------- quantities per layer -------------------------------------------------------------------------------------------
    hostTree->Branch(string(_prefix+"energySumPerEvent").c_str(), &_energySumPerEvent, string(_prefix+"energySumPerEvent/D").c_str());
    hostTree->Branch(string(_prefix+"nLayers").c_str(), &_nLayers, string(_prefix+"nLayers/I").c_str());	
    hostTree->Branch(string(_prefix+"energySumPerLayer").c_str(), &_energySumPerLayer, string(_prefix+"energySumPerLayer["+_prefix+"nLayers]/D").c_str());
    hostTree->Branch(string(_prefix+"nHitsPerLayer").c_str(),&_nHitsPerLayer, string(_prefix+"nHitsPerLayer["+_prefix+"nLayers]/I").c_str());
    hostTree->Branch(string(_prefix+"xCogPerLayer").c_str(), &_xCogPerLayer, string(_prefix+"xCogPerLayer["+_prefix+"nLayers]/D").c_str());
    hostTree->Branch(string(_prefix+"yCogPerLayer").c_str(), &_yCogPerLayer, string(_prefix+"yCogPerLayer["+_prefix+"nLayers]/D").c_str());
    hostTree->Branch(string(_prefix+"zCogPerLayer").c_str(), &_zCogPerLayer, string(_prefix+"zCogPerLayer["+_prefix+"nLayers]/D").c_str());

 

    _histHcalLongProfile = new TH1F("hcalLongProfile", "hcalLongProfile", 38, 0.5, 38.5);
  }


  /**********************************************************************/
  /*                                                                    */
  /*                                                                    */
  /*                                                                    */
  /**********************************************************************/
  void ASimCalorimeterHitWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    
    int evtNumber = evt->getEventNumber();
    //if (evtNumber >1) return;
    if ((evtNumber % 500) == 0) 
      cout<<" \n ---------> Event: "<<evtNumber<<" <-------------\n"<<endl;

    bool calorimCollectionFound = false;

    try {
      std::vector< std::string >::const_iterator iter;
      const std::vector< std::string >* colNames = evt->getCollectionNames();
      
      for( iter = colNames->begin() ; iter != colNames->end() ; iter++) {
	if ( *iter == _calorimCollection ) calorimCollectionFound = true;
      }
    }
    catch(DataNotAvailableException &e){
      std::cout <<  "WARNING: List of collection names not available in event "<< evt->getEventNumber() << std::endl;
      return;
    };


    //=================================================//
    //                                                 //
    // Initialize variables                            //
    //                                                 //
    //=================================================//
    _nHitsPerEvent = 0;

    (*_IPtr).clear();
    (*_JPtr).clear();
    (*_KPtr).clear();

    (*_xPtr).clear();
    (*_yPtr).clear();
    (*_zPtr).clear();

    (*_hitEnergyPtr).clear();
    (*_cellID0Ptr).clear();

    _xCog = 0, _yCog = 0, _zCog = 0;
	
    float xCogTemp = 0, yCogTemp = 0, zCogTemp = 0;
	
	
    _energySumPerEvent = 0;
    _nLayers = _fMaxLayers;
	
    double xCogPerLayerTemp[_fMaxLayers] = {0.};
    double yCogPerLayerTemp[_fMaxLayers] = {0.};
    double zCogPerLayerTemp[_fMaxLayers] = {0.};
    int nHitsPerLayerTemp[_fMaxLayers]   = {0};

    for (int iLayer = 0; iLayer < _fMaxLayers; iLayer++)
      {
	_energySumPerLayer[iLayer] = 0.;
	_nHitsPerLayer[iLayer]     = 0;
	_xCogPerLayer[iLayer]      = 0.;
	_yCogPerLayer[iLayer]      = 0.;
	_zCogPerLayer[iLayer]      = 0.;
      }
    
	
 

    //=================================================//
    //                                                 //
    // Reading collection of calorimeter hits          //
    //                                                 //
    //=================================================//
    if (calorimCollectionFound){
      LCCollection *inputCalorimCollection = evt->getCollection(_calorimCollection);
      int noElements = inputCalorimCollection->getNumberOfElements();
      
      _nHitsPerEvent = noElements;
      
      CellIDDecoder<SimCalorimeterHit> decoder(inputCalorimCollection);

     
      for (int i = 0; i < noElements; i++)
	{
	  SimCalorimeterHit *aCalorimHit = dynamic_cast<SimCalorimeterHit*>(inputCalorimCollection->getElementAt(i));
		  
	  _IPtr->push_back(decoder(aCalorimHit)["I"]);
	  _JPtr->push_back(decoder(aCalorimHit)["J"]);
	  int layer = decoder(aCalorimHit)["K"];
	  _KPtr->push_back(layer);

 	  const float *hitPos = aCalorimHit->getPosition();
	  _xPtr->push_back(hitPos[0]);
	  _yPtr->push_back(hitPos[1]);
	  _zPtr->push_back(hitPos[2]);
	  
	  double energy = aCalorimHit->getEnergy();
	  _hitEnergyPtr->push_back(energy);
	  _energySumPerEvent += energy;
	  
	  const int cellID0 = aCalorimHit->getCellID0();
	  _cellID0Ptr->push_back(cellID0);

	  xCogTemp += hitPos[0] * energy;
	  yCogTemp += hitPos[1] * energy;
	  zCogTemp += hitPos[2] * energy;
	  
	  if (layer > 38)
	    cout<<"layer: "<<layer<<"  energy: "<<energy<<endl;
	  _energySumPerLayer[layer-1] += energy;
	  
	  xCogPerLayerTemp[layer-1] += hitPos[0] * energy;
	  yCogPerLayerTemp[layer-1] += hitPos[1] * energy;
	  zCogPerLayerTemp[layer-1] += hitPos[2] * energy;
	  
	  nHitsPerLayerTemp[layer-1]++;
		  
	  
	}//end loop over SimCalorimeterHits

      //=======================================================================
	  
      if (_energySumPerEvent > 0)
	{
	  _xCog = xCogTemp / _energySumPerEvent;
	  _yCog = yCogTemp / _energySumPerEvent;
	  _zCog = zCogTemp / _energySumPerEvent;
	}
	  
      for (int iLayer = 0; iLayer < _fMaxLayers; iLayer++)
       	{
       	  _xCogPerLayer[iLayer] = xCogPerLayerTemp[iLayer] / _energySumPerLayer[iLayer];
       	  _yCogPerLayer[iLayer] = yCogPerLayerTemp[iLayer] / _energySumPerLayer[iLayer];
       	  _zCogPerLayer[iLayer] = zCogPerLayerTemp[iLayer] / _energySumPerLayer[iLayer];
		  
       	  _nHitsPerLayer[iLayer] = nHitsPerLayerTemp[iLayer];

	  _histHcalLongProfile->Fill(iLayer+1, _energySumPerLayer[iLayer]);
 
       	}
      


    }//end of calorimCollectionFound

 
    //======================================================================
    //======================================================================
    //======================================================================

    
  }//end fillVariables()

}//namespace marlin


