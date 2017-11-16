#include "HcalTMVAProcessor.hh"

#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCGenericObject.h"
#include "UTIL/CellIDDecoder.h"
#include "marlin/Exceptions.h"

#include "MappingProcessor.hh"
#include "TBTrackUtil/TrackProjection.hh"
#include "TriggerBits.hh"
#include "collection_names.hh" /*PAR_TRIGGER_EVENT*/

#include "TROOT.h"
#include "TMath.h"
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include <vector>
#include <map>
#include <algorithm>



/* for cellsize()*/
#define FIRST_COARSE 31
#define I3x3_LOW 31
#define I3x3_HIGH 60
#define I6x6_LOW 13
#define I6x6_HIGH 78
#define J3x3_LOW 31
#define J3x3_HIGH 60
#define J6x6_LOW 13
#define J6x6_HIGH 78

using namespace marlin;
using namespace lcio;
using namespace std;
 
HcalTMVAProcessor aHcalTMVAProcessor;

/**********************************************************************/
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
 
HcalTMVAProcessor::HcalTMVAProcessor() : Processor("HcalTMVAProcessor")
{
  _description = "TMVA analysis Processor";

  registerProcessorParameter("ShowerStartCollectionName",
			     "Name of input shower start collection",
			     _showerStartColName,
			     std::string("ShowerStartingLayer"));
  
  registerProcessorParameter("PtfTrackCollectionName",
			     "Name of input Primary Track Finder track collection name",
			     _ptfTrackColName,
                             std::string("PrimaryTrackHits"));
  
  registerProcessorParameter("HcalCollectionName",
			     "Name of input HCAL collection name",
			     _hcalColName,
                             std::string("AhcCalorimeter_Hits"));
  
  registerProcessorParameter("RootFileName",
			     "Name of ROOT output file name",
			     _rootFileName,
                             std::string("TMVAnalysis.root"));
  
  registerOptionalParameter( "rootFileMode","Mode for opening ROOT file",
                             _rootFileMode, std::string("RECREATE")      );
  
  
  registerProcessorParameter( "ProcessorName_Mapping" ,
			      "name of Ahcal MappingProcessor which takes care of the mapping",
			      _mappingProcessorName,
			      std::string("HcalMappingProcessor") ) ;
  
  
  registerProcessorParameter( "inputTBTrackFEXCol",
                              "Collection of TBTrackFEX input collection",
                              _fexTrackColName,
                              std::string( "TBTrackFEX" ) );

  registerProcessorParameter( "inputTBTrackFEYCol",
                              "Collection of TBTrackFEY input collection",
                              _feyTrackColName,
                              std::string( "TBTrackFEY" ) );
//--------TMVA variables-----------------------------------------------------

  registerProcessorParameter("TMVA_Method",
			     "Name of TMVA method",
			     _TMVA_Method,
                             std::string("BDT"));

  registerProcessorParameter("TMVA_Training_SignalFile",
			     "The MC ROOT file with signal",
			     _TMVA_Training_SignalFile,
                             std::string("/home/jzalieck/scripts/data/Run_Electron_5GeV.root"));

  registerProcessorParameter("TMVA_Training_BackgroundFile",
			     "The MC ROOT file with Background",
			     _TMVA_Training_BackgroundFile,
                             std::string("/home/jzalieck/scripts/data/Run_Pion_5GeV.root"));

  registerProcessorParameter("TMVA_Analysis_SignalFile",
			     "The data ROOT file with signal",
			     _TMVA_Analysis_SignalFile,
                             std::string("/home/jzalieck/scripts/data/Data_Run_Electron_5GeV.root"));

  registerProcessorParameter("argumentForPrepareTrainingAndTestTree",
			     "Argument for PrepareTrainingAndTestTree in TMVA training",
			     _argumentForPrepareTrainingAndTestTree,
                             std::string("nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V"));

  registerProcessorParameter("argumentForBookMethod",
			     "Argument for BookMethod in TMVA training",
			     _argumentForBookMethod,
                             std::string("!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning"));

  registerProcessorParameter("FillRootTree",
			     "Fills ROOT Trees",
			     _FillRootTree,
                             bool(true));

  registerProcessorParameter("TMVA_Training",
			     "Enables TMVA Training",
			     _TMVA_Training,
                             bool(false));

  registerProcessorParameter("TMVA_Analysis",
			     "Enables TMVA Analysis",
			     _TMVA_Analysis,
                             bool(false));

  registerProcessorParameter("signalWeight",
			     "Global weight for signal",
			     _signalWeight,
                             float(1.0));

  registerProcessorParameter("backgroundWeight",
			     "Global weight for background",
			     _backgroundWeight,
                             float(1.0));

//-----------------------------------------------------------------------------

}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/
HcalTMVAProcessor::~HcalTMVAProcessor()
{}
/**********************************************************************/
/*                                                                    */
/**********************************************************************/
void HcalTMVAProcessor::init()
{
  _mapper = dynamic_cast<const CALICE::AhcMapper*> ( CALICE::MappingProcessor::getMapper(_mappingProcessorName) );
  
  bool error = false;
  if ( ! _mapper) {
    streamlog_out(ERROR) << "Cannot obtain AhcMapper from MappingProcessor "<<_mappingProcessorName
		 <<". Mapper not present or wrong type." << endl;
    error = true;
  }
  if (error){throw StopProcessingException(this);}


  /*
    ROOT stuff
   */
  //TH1::SetDefaultSumw2(kTRUE);

  _hLong                 = new TH1F("longProfile",    "longProfile", 40, 0, 40);
  _hLong_fromShowerStart = new TH1F("longProfile_fromShowerStart", "longProfile_fromShowerStart", 42, 0, 42);
  _hEnergySum            = new TH1F("energySum",      "energySum", 300, 0.0, 3000.0);
  _hNumHits              = new TH1F("numHits",        "numHits", 300, 0, 300);
  _hShowerStart          = new TH1F("showerStart",    "showerStart", 40, 0, 40);
  _hLayerMaxEnergy       = new TH1F("layerMaxEnergy", "layerMaxEnergy", 40, 0, 40);
  _hNumHitsPerLayer      = new TH1F("numHitsPerLayer","numHitsPerLayer", 40, 0, 40);
  _hHitEnergy            = new TH1F("hitEnergy",      "hitEnergy", 200, 0, 500);

  _hCogX = new TH1F("cogX", "cogX", 400,-200, 200);
  _hCogY = new TH1F("cogY", "cogY", 400,-200, 200);

  _hTrkXSlope       = new TH1F("trkXSlope",       "trkXSlope",       200, -0.1, 0.1);
  _hTrkXOffset      = new TH1F("trkXOffset",      "trkXOffset",      160, -80, 80);
  _hTrkXAtHcalFront = new TH1F("trkXAtHcalFront", "trkXAtHcalFront", 400, -200, 200);
  _hTrkYSlope       = new TH1F("trkYSlope",       "trkYSlope",       200, -0.1, 0.1);
  _hTrkYOffset      = new TH1F("trkYOffset",      "trkYOffset",      160, -80, 80);
  _hTrkYAtHcalFront = new TH1F("trkYAtHcalFront", "trkYAtHcalFront", 400, -200, 200);

  _rootTree = new TTree("variables","analysis");
  _rootTree->Branch("ahc_energySum", &_energySum, "_energySum/F"); 
  _rootTree->Branch("ahc_energySum5RatioEnergySum", &_energyRatio, "_energyRatio/F"); 
  _rootTree->Branch("ahc_energy1Momentum", &_energy1Momentum, "&_energy1Momentum/F");
  _rootTree->Branch("ahc_energy2Momentum", &_energy2Momentum, "&_energy2Momentum/F");
  _rootTree->Branch("ahc_energy3Momentum", &_energy3Momentum, "&_energy3Momentum/F");
  _rootTree->Branch("ahc_energyDensity", &_energyDensity, "&_energyDensity/F");
  _rootTree->Branch("ahc_R90_hits", &_R90_hits, "&_R90_hits/F"); 
  _rootTree->Branch("ahc_N90ratioN", &_N90ratioN, "&_N90ratioN/F");
  _rootTree->Branch("ahc_cellsMeanEnergy", &_hitsMean, "&_hitsMean/F"); 
  _rootTree->Branch("ahc_layerMax", &_nLayerMaxEnergy, "&_nLayerMaxEnergy/I"); 
  _rootTree->Branch("ahc_layerStart", &_startLayer, "&_startLayer/I");
  _rootTree->Branch("ahc_layerMaxDiffStart", &_layerMaxDiffStart, "&_layerMaxDiffStart/I"); 

}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/
void HcalTMVAProcessor::processEvent(LCEvent* evt)
{

if(_FillRootTree == true) {fillRootTree(evt);}
}
/**********************************************************************/
/*                                                                    */
/**********************************************************************/
bool HcalTMVAProcessor::tbtrackLoop(LCEvent *evt)
{
  bool trackFound = false;

  float zPosHcal = 0;

  /*------------ X ---------------------------*/
  LCCollection* fexTrackCol = NULL;

  /* get best track projecton in x */
  TBTrack::TrackProjection *trkX = NULL;
  try
    {
      fexTrackCol = evt->getCollection( _fexTrackColName );
    }
  catch ( lcio::DataNotAvailableException &e ) 
    {
      streamlog_out( DEBUG ) << "HcalTMVAProcessor::tbtrackLoop(): " 
		     << "missing collection " << _fexTrackColName
		     << endl;
      throw SkipEventException( this ) ;
    }	

  float xTrkSlope      = -999;
  float xTrkOffset     = -999;
  float xTrkAtHcalFront = -999;

  int noElemX = fexTrackCol->getNumberOfElements();
  streamlog_out(DEBUG)<<" tbtrack loop: noElemX = "<<noElemX<<endl;

  if (noElemX > 0)
    {
      for (int iX = 0; iX < noElemX; ++iX)
	{
	  TBTrack::TrackProjection *currentTrkX = 
	    new TBTrack::TrackProjection(dynamic_cast< EVENT::LCGenericObject* >(fexTrackCol->getElementAt(iX)) );

	  if (trkX == NULL)
	    {
	      trkX = currentTrkX;
	    }
	  else
	    {
	      if (currentTrkX->probability() > trkX->probability())
		{
		  delete trkX;
		  trkX = currentTrkX;
		}
	      else
		{
		  delete currentTrkX;
		}
	    }
	}

      xTrkAtHcalFront = trkX->intercept( zPosHcal, 0 );
      xTrkSlope       = trkX->gradient(0);
      xTrkOffset      = trkX->intercept(0,0);
    }


  /*------------ Y ---------------------------*/
  LCCollection* feyTrackCol = 0;
  
  /* get best track projecton in y */
  TBTrack::TrackProjection *trkY = NULL;
  try
    {
      feyTrackCol = evt->getCollection( _feyTrackColName );
    }	
  catch ( lcio::DataNotAvailableException &e ) 
    {
      streamlog_out( DEBUG ) << "HcalTMVAProcessor::tbtracksLoop(): " 
		     << "missing collection " << _feyTrackColName
		     << std::endl;
       throw SkipEventException( this ) ;
   }
    
  float yTrkSlope       = -999;
  float yTrkOffset      = -999;
  float yTrkAtHcalFront = -999;

  int noElemY = feyTrackCol->getNumberOfElements();
  streamlog_out(DEBUG)<<" tbtrack loop: noElemY = "<<noElemY<<endl;

  if (noElemY > 0)
    {
      for (int iY = 0; iY < noElemY; ++iY)
	{
	  TBTrack::TrackProjection *currentTrkY = 
	    new TBTrack::TrackProjection(dynamic_cast< EVENT::LCGenericObject* >(feyTrackCol->getElementAt(iY)) );
	  
	  if (trkY == NULL)
	    {
	      trkY = currentTrkY;
	    }
	  else
	    {
	      if (currentTrkY->probability() > trkY->probability())
		{
		  delete trkY;
		  trkY = currentTrkY;
		}
	      else
		{
		  delete currentTrkY;
		}
	    }
	}
      
      yTrkAtHcalFront = trkY->intercept( zPosHcal, 0 );
      yTrkSlope       = trkY->gradient(0);
      yTrkOffset      = trkY->intercept(0,0);
    }
  
  /* if there is at least one TBTrackFEX and one TBTrackFEY per event*/
  if (xTrkAtHcalFront != -999 && yTrkAtHcalFront!= -999) 
    {
      _hTrkXSlope->Fill(xTrkSlope);
      _hTrkXOffset->Fill(xTrkOffset);
      _hTrkXAtHcalFront->Fill(xTrkAtHcalFront);

      _hTrkYSlope->Fill(yTrkSlope);
      _hTrkYOffset->Fill(yTrkOffset);
      _hTrkYAtHcalFront->Fill(yTrkAtHcalFront);
      
      _fxTrkAtHcalFront = xTrkAtHcalFront;
      _fyTrkAtHcalFront = yTrkAtHcalFront;

      trackFound = true;
    }


return trackFound;
 
}

/**********************************************************************/
/*                                                                    */
/**********************************************************************/
void HcalTMVAProcessor::end()
{
  _rootFile = gROOT->GetFile(_rootFileName.c_str());
  if ( _rootFile == NULL )
    {
      _rootFile = new TFile(_rootFileName.c_str(), _rootFileMode.c_str());
    }
 
  /*--------------------------------------------------------*/
  _rootFile->mkdir("hcal");
  _rootFile->cd("hcal");

  _hLong->Write();
  _hLong_fromShowerStart->Write();
  _hEnergySum->Write();
  _hNumHits->Write();
  _hShowerStart->Write();
  _hLayerMaxEnergy->Write();
  _hNumHitsPerLayer->Write();
  _hHitEnergy->Write();

  _hCogX->Write();
  _hCogY->Write();


  _rootFile->cd();
  /*--------------------------------------------------------*/  
  _rootFile->mkdir("track");
  _rootFile->cd("track");
  
  _hTrkXSlope->Write();
  _hTrkXOffset->Write();
  _hTrkXAtHcalFront->Write();

  _hTrkYSlope->Write();
  _hTrkYOffset->Write();
  _hTrkYAtHcalFront->Write();

  _rootFile->cd();
  /*--------------------------------------------------------*/ 
  _rootTree->Write();

  _rootFile->Close();
  delete _rootFile;

//-----------------TMVA Training----------------------------------------------------
if(_TMVA_Training==true){TMVAClassification(_TMVA_Method, _TMVA_Training_SignalFile, _TMVA_Training_BackgroundFile, _signalWeight, _backgroundWeight, _argumentForPrepareTrainingAndTestTree, _argumentForBookMethod);}
//----------------------------------------------------------------------------------

//-----------------TMVA Application-------------------------------------------------
if(_TMVA_Analysis==true){TMVAClassificationApplication(_TMVA_Method, _TMVA_Analysis_SignalFile);}
//----------------------------------------------------------------------------------
}


/*******************************************************************************************/
/*                                                                                         */
/*******************************************************************************************/
int HcalTMVAProcessor::getMarinaShowerStartHcalLayer(LCEvent *evt, std::string showerStartColName)
{
  LCCollection *col = NULL;
  LCGenericObject *showerStartLayerObj = NULL;
  
  int showerStartHcalLayer = -99;

  const int maxLayers = _mapper->getMaxK();
  
  try{
    col = evt->getCollection(showerStartColName);
    int nEntries = col->getNumberOfElements();
    streamlog_out(DEBUG)<<"getShowerStartHcalLayer: nEntries="<<nEntries<<endl;
    
    for (int i = 0; i < nEntries; ++i)
      {
	showerStartLayerObj = dynamic_cast<LCGenericObject*>(col->getElementAt(i));
	
	/*calorimeter type: 0=ECAL, 1=first 30 layers in HCAL, 2=last coarse part of HCAL*/
	int type = showerStartLayerObj->getIntVal(0);
	
	/*shower starting layer number (in each calorimeter)*/
	int showerStart = showerStartLayerObj->getIntVal(1);
	streamlog_out(DEBUG)<<"  PTF: shower start type = "<<type<<", found shower start in layer "<<showerStart<<endl;
	  
	if (maxLayers == 31)
	  {
	    if (type == 1) /*first 30 layers in HCAL*/
	      {
		showerStartHcalLayer = showerStart+1;
	      }
	    else
	      {
		 showerStartHcalLayer = -99;
	      }
	  }
	else if (maxLayers == 39)
	  {
	    if (type == 1) /*first 30 layers in HCAL*/
	      {
		showerStartHcalLayer = showerStart+1;
	      }
	    else if (type == 2)/*last 8 layers in HCAL*/
	      {
		showerStartHcalLayer = 30 + showerStart+1;
		if (showerStartHcalLayer == 39) 
		  {
		    streamlog_out(DEBUG)<<"\n\n Warning, showerStartHcalLayer = 39"<<std::endl;
		    showerStartHcalLayer = -99;
		  }
	      }
	  }
	
	//float averageX = showerStartLayerObj->getFloatVal(0);
	//float averageY = showerStartLayerObj->getFloatVal(1);
	
	
      }
  }
  catch(DataNotAvailableException err){}
  
  streamlog_out(DEBUG)<<"getShowerStartHcalLayer: showerStartHcalLayer="<<showerStartHcalLayer<<endl;
  
  return showerStartHcalLayer;
  
}

/*********************************************************************************/
/*                                                                               */
/*********************************************************************************/
int HcalTMVAProcessor::cellsize( int cellid, bool coarse)
{
  int i = (cellid & 0x7fc0 ) >> 6;
  int j = (cellid & 0xff8000) >> 15;

  int layer = ( (cellid & 0x3F000000) >> 24 ) + 1 ;
  if (layer >= FIRST_COARSE ) // layer index starts with 1?
    coarse = true;
  
  int tilesize;
  if( coarse == false )
    tilesize=3;
  else
    tilesize=6;
  
  if ( i<I3x3_LOW || j<J3x3_LOW || i>I3x3_HIGH || j>J3x3_HIGH )
    tilesize=6;

  if ( i<I6x6_LOW || j<J6x6_LOW || i>I6x6_HIGH || j>J6x6_HIGH )
    tilesize=12;

  return tilesize;

}

//---------------------------------------------------------------------------------
//-------------------------------Fill ROOT Trees-----------------------------------
/*********************************************************************************/
/*                                                                               */
/*********************************************************************************/

void HcalTMVAProcessor::fillRootTree(LCEvent* evt)
{

  int evtNumber = evt->getEventNumber();
  if (evtNumber % 1000 == 0) streamlog_out(DEBUG)<<"Event "<<evtNumber<<endl;

  bool foundTrack = tbtrackLoop(evt);
  if (foundTrack == false) {throw SkipEventException( this ); }
  
  // find the shower start
  _startLayer = getMarinaShowerStartHcalLayer(evt, _showerStartColName);
  if (_startLayer > 0) _hShowerStart->Fill(_startLayer);
  
  // HCAL loop
  LCCollection *col= NULL;
  try{
    col = evt->getCollection(_hcalColName);
  }/*end of try */
  catch(lcio::DataNotAvailableException err)
    {
      /*skip this event in case there is no HCAL collection in it*/
      throw SkipEventException( this ) ;
      streamlog_out(DEBUG)<<" Collection "<<_hcalColName<<" not available in event "<<evtNumber<<endl;
    }
  
  
  CellIDDecoder<CalorimeterHit> cellIDDecoder(col);
  int nEntries = col->getNumberOfElements();
  _hNumHits->Fill(nEntries);
  
  /*initialize variables*/
  float energySum2 = 0.0, energySum3 = 0.0;
  float energy5Layers = 0.0;
  float cogX = 13.61;
  float cogY = 13.73;
  float r = 0.0;
  const int maxLayers = _mapper->getMaxK();
  float *energyPerLayerArr = new float [maxLayers];
  float *layerMaxRadiusArr = new float [maxLayers];
  float *hitsEnergyArr = new float [nEntries];
  int *entriesPerLayer = new int [maxLayers];
  bool coarseCellId = false;

  std::multimap<float, float> energyRadiusMap;
  std::multimap<float, float>::iterator iter;
  energyRadiusMap.clear();

  _energySum = 0.0;
  _energy1Momentum = 0.0;
  _energy2Momentum = 0.0;
  _energy3Momentum = 0.0;
  _energyDensity = 0.0;
  _R90_hits = 0.0;
  _N90ratioN = 0.0;
  _hitsMean = 0.0;

  for (int k = 0; k < maxLayers; ++k)
    {
      energyPerLayerArr[k] = 0;
      entriesPerLayer[k] = 0;
      layerMaxRadiusArr[k] = 0.0;
    }
   
  for (int i = 0; i < nEntries; ++i)
    {
      CalorimeterHit *caloHit = dynamic_cast<CalorimeterHit*>(col->getElementAt(i));
      float energy = caloHit->getEnergy();
      
      _hHitEnergy->Fill(energy);
      
      const int layer = cellIDDecoder(caloHit)["K-1"] + 1;
      energyPerLayerArr[layer-1] += energy;
      entriesPerLayer[layer-1]++;

      const float *position = caloHit->getPosition();
      cogX += position[0] * energy;
      cogY += position[1] * energy;

      //-----------Momentums---------------------------------------------
      _energySum += energy;
      energySum2 += energy*energy;
      energySum3 += energy*energy*energy; 
      r = sqrt(((position[0]-_fxTrkAtHcalFront)*(position[0]-_fxTrkAtHcalFront))+((position[1]-_fyTrkAtHcalFront)*(position[1]-_fyTrkAtHcalFront)));
      _energy1Momentum = _energy1Momentum + energy*r;
      _energy2Momentum = _energy2Momentum + energy*energy*r;
      _energy3Momentum = _energy3Momentum + energy*energy*energy*r;

      //------------R90_hits----------------------------------------------
      energyRadiusMap.insert(pair<float, float>(r, energy));
      hitsEnergyArr[i] = energy;
      //------------Energy density----------------------------------------

      if(layer>=30){coarseCellId =true;}
      else {coarseCellId =false;}
      _energyDensity+=energy/(cellsize(caloHit->getCellID0(), coarseCellId)*cellsize(caloHit->getCellID0(), coarseCellId)*0.5);
    }/*---- end loop over i -----*/
 
    _hEnergySum->Fill(_energySum);

//--------Sorting by layers (calculates E5-energy contained in first 5 layers)----------------
  for (int t = 0; t < 5; ++t)
    {energy5Layers += energyPerLayerArr[t];}
   
//--------Sorting by radius---------------------------------------
float tempEnergy =0.0;

  for (iter = energyRadiusMap.begin(); iter != energyRadiusMap.end(); ++iter)
    {    
     tempEnergy+=(*iter).second;

     if((tempEnergy>=(_energySum*0.9)))
	 {_R90_hits=(*iter).first; break;} 
    }

//---------Sorting by cells energy (decreasing order)--------------------- 
tempEnergy = 0.0;

int *sortedIndexArray = new int [nEntries];
TMath::Sort(nEntries, hitsEnergyArr, sortedIndexArray, true);

for(int a=0; a<nEntries; a++)
  { 
   tempEnergy += hitsEnergyArr[sortedIndexArray[a]];
   if(tempEnergy>=(_energySum*0.9))
   {_N90ratioN=((float)a+1.0)/(float)nEntries; a = nEntries; break;} 
        
  }

//-------------------------------------------------------------------------
 /*fill histos*/
  _hEnergySum->Fill(_energySum);
  _hCogX->Fill(cogX/_energySum);
  _hCogY->Fill(cogY/_energySum);
  
  _nLayerMaxEnergy = 0;
  double maxEnergy = 0;
  
  for (int k = 0; k < maxLayers; ++k)
    {
      _hLong->Fill(k, energyPerLayerArr[k]);
      _hNumHitsPerLayer->Fill(k, entriesPerLayer[k]);
      
      if (energyPerLayerArr[k] > maxEnergy)
	{
	  maxEnergy = energyPerLayerArr[k];
	  _nLayerMaxEnergy = k+1;
	}
    }
  
  _hLayerMaxEnergy->Fill(_nLayerMaxEnergy);
  

//------------Filling Root tree---------------------------- 
 
if ((_startLayer > 0)&&(_startLayer!=(maxLayers-1)))
{
  _energy1Momentum = _energy1Momentum/_energySum;
  _energy2Momentum = _energy2Momentum/energySum2;
  _energy3Momentum = _energy3Momentum/energySum3;
  _energyRatio = energy5Layers/_energySum;
  _hitsMean = _energySum/nEntries;
  _energyDensity = _energyDensity/nEntries;
  _layerMaxDiffStart = _nLayerMaxEnergy - _startLayer;
  if (_layerMaxDiffStart < 0) 
  {_startLayer = _nLayerMaxEnergy; _layerMaxDiffStart = 0;}
  
  _rootTree->Fill();
 }
delete []hitsEnergyArr;
delete []sortedIndexArray;

//-----------------------------------------------------------
  if (_startLayer > 0)
    {
      _startLayer = _startLayer-3;
      for (int iLayerShowerStart = _startLayer; iLayerShowerStart < maxLayers; ++iLayerShowerStart)
	{
	  if (energyPerLayerArr[iLayerShowerStart] > 0)
	    {
	      _hLong_fromShowerStart->Fill(iLayerShowerStart - _startLayer, 
					   energyPerLayerArr[iLayerShowerStart]);
	    }
	}
    }

}

//---------------------------------------------------------------------------------
//-------------------------------TMVA Classification-------------------------------
/*********************************************************************************/
/*                                                                               */
/*********************************************************************************/

void HcalTMVAProcessor::TMVAClassification(std::string TMVAMethodName, std::string signalFileName, std::string backgroundFileName, float signalWeight, float backgroundWeight, std::string argumentForPrepareTrainingAndTestTree, std::string argumentForBookMethod)
{
  
   TString myMethodList = TMVAMethodName;

   //MVA method to be trained + tested
   std::map<std::string,int> Use;
   
   //Uses BDT for classification
   Use["BDT"] = 1;
  
   cout << endl;
   cout << "==> Start TMVAClassification" << endl;

   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) cout << it->first << " ";
            cout << endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // Create a new root output file.
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory will
   // then run the performance analysis for you.
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   // Transformations can be =I;D;P;G,D
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );

   
   factory->AddSpectator("ahc_energySum", 'F');
   factory->AddVariable("ahc_energy1Momentum", 'F');
   factory->AddVariable("ahc_energySum5RatioEnergySum", 'F');
   factory->AddVariable("ahc_energy2Momentum", 'F');
   factory->AddVariable("log(ahc_energy3Momentum)", 'F');
   factory->AddVariable("ahc_energyDensity", 'F');  
   factory->AddVariable("ahc_R90_hits", 'F');
   factory->AddVariable("ahc_N90ratioN", 'F');
   factory->AddVariable("ahc_cellsMeanEnergy", 'F');
   factory->AddVariable("ahc_layerMax", 'I');
   factory->AddVariable("ahc_layerStart", 'I');
   factory->AddVariable("ahc_layerMaxDiffStart", 'I');

   // read training and test data
  
      // load the signal and background event samples from ROOT trees
      TString fname1 = signalFileName;
      TString fname2 = backgroundFileName;

      if (gSystem->AccessPathName( fname1 ))  // file does not exist in local directory
         {cout << "File 1 do not exist" <<endl;}
      if (gSystem->AccessPathName( fname2 ))  // file does not exist in local directory
         {cout << "File 2 do not exist" <<endl;}

      TFile *input1 = TFile::Open( fname1 );
      TFile *input2 = TFile::Open( fname2 );

      cout << "--- TMVAClassification       : Using input file 1: " << input1->GetName() << endl;
      cout << "--- TMVAClassification       : Using input file 2: " << input2->GetName() << endl;

      TTree *signal     = (TTree*)input1->Get("variables");
      TTree *background = (TTree*)input2->Get("variables");

      // ====== register trees ====================================================
      factory->AddSignalTree    ( signal,     signalWeight     );
      factory->AddBackgroundTree( background, backgroundWeight );
      


   // Apply additional cuts on the signal and background samples (can be different)
   TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";

   // tell the factory to use all remaining events in the trees after training for testing:
   factory->PrepareTrainingAndTestTree( mycuts, mycutb,
                                        argumentForPrepareTrainingAndTestTree );

   // If no numbers of events are given, half of the events in the tree are used for training, and
   // the other half for testing:

   // ---- Book MVA method
   // Cut optimisation


   if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod( TMVA::Types::kBDT, "BDT",
                           argumentForBookMethod );


   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   cout << "==> Wrote root file: " << outputFile->GetName() << endl;
   cout << "==> TMVAClassification is done!" << endl;

   delete factory;
}

//---------------------------------------------------------------------------------
//-------------------------------TMVA Application----------------------------------
/*********************************************************************************/
/*                                                                               */
/*********************************************************************************/
void HcalTMVAProcessor::TMVAClassificationApplication(std::string TMVAMethodName, std::string signalFileName)
{
   
   TString myMethod = TMVAMethodName;

   std::map<std::string,int> Use;

   //-----------Uses BDT for analysis-------------
   Use["BDT"] = 1;
   
   cout << endl;
   cout << "==> Start TMVAClassificationApplication" << endl;

   // create the Reader object

   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    

   // Create a set of variables and declare them to the reader
   // the variable names must corresponds in name and type to 
   // those given in the weight file that you use
   
   //Variables for reading from data sample
   Float_t ahc_energySum;
   Float_t ahc_energy1Momentum;
   Float_t ahc_energy2Momentum;
   Float_t ahc_energy3Momentum;
   Float_t ahc_energySum5RatioEnergySum;
   Float_t ahc_energyDensity;
   Float_t ahc_R90_hits;
   Float_t ahc_N90ratioN;
   Float_t ahc_cellsMeanEnergy;
   Float_t ahc_layerMax;
   Float_t ahc_layerStart;
   Float_t ahc_layerMaxDiffStart;
   
   Int_t layerMax;
   Int_t layerStart;
   Int_t layerMaxDiffStart;
   
   //Variables for writing in TMVApp.root with classifier response 
   Float_t responseData;
   Float_t energySumData;
   Float_t energy1MomentumData;
   Float_t energy2MomentumData;
   Float_t energy3MomentumData;
   Float_t energySum5RatioEnergySumData;
   Float_t energyDensityData;
   Float_t R90_hitsData;
   Float_t N90ratioNData;
   Float_t cellsMeanEnergyData;
   Int_t layerMaxData;
   Int_t layerStartData;
   Int_t layerMaxDiffStartData;
   
   reader->AddSpectator("ahc_energySum", &ahc_energySum);
   
   reader->AddVariable("ahc_energy1Momentum", &ahc_energy1Momentum);
   reader->AddVariable("ahc_energySum5RatioEnergySum", &ahc_energySum5RatioEnergySum);
   reader->AddVariable("ahc_energy2Momentum", &ahc_energy2Momentum);
   reader->AddVariable("log(ahc_energy3Momentum)", &ahc_energy3Momentum);
   reader->AddVariable("ahc_energyDensity", &ahc_energyDensity);  
   reader->AddVariable("ahc_R90_hits", &ahc_R90_hits);
   reader->AddVariable("ahc_N90ratioN", &ahc_N90ratioN);
   reader->AddVariable("ahc_cellsMeanEnergy", &ahc_cellsMeanEnergy);
   reader->AddVariable("ahc_layerMax", &ahc_layerMax);
   reader->AddVariable("ahc_layerStart", &ahc_layerStart);
   reader->AddVariable("ahc_layerMaxDiffStart", &ahc_layerMaxDiffStart);
   
   //book the MVA method
   TString dir    = "weights/";
   TString prefix = "TMVAClassification";
    
   TString methodName = myMethod + " method";
   TString weightfile = dir + prefix + "_" + myMethod + ".weights.xml";
   reader->BookMVA( methodName, weightfile ); 
   
   
   //load the signal event samples from ROOT trees
   TString fname1 = signalFileName;
      

      if (gSystem->AccessPathName( fname1 ))  // file does not exist in local directory
         {cout << "Signal file do not exist" <<endl;}
     

      TFile *input1 = TFile::Open( fname1 );  
      cout << "--- TMVAClassificationApp       : Using input file: " << input1->GetName() << endl;
      TTree *signal     = (TTree*)input1->Get("variables");
      

   signal->SetBranchAddress("ahc_energySum", &ahc_energySum); 
   signal->SetBranchAddress("ahc_energy1Momentum", &ahc_energy1Momentum);
   signal->SetBranchAddress("ahc_energySum5RatioEnergySum", &ahc_energySum5RatioEnergySum);
   signal->SetBranchAddress("ahc_energy2Momentum", &ahc_energy2Momentum);
   signal->SetBranchAddress("ahc_energy3Momentum", &ahc_energy3Momentum);
   signal->SetBranchAddress("ahc_energyDensity", &ahc_energyDensity);  
   signal->SetBranchAddress("ahc_R90_hits", &ahc_R90_hits);
   signal->SetBranchAddress("ahc_N90ratioN", &ahc_N90ratioN);
   signal->SetBranchAddress("ahc_cellsMeanEnergy", &ahc_cellsMeanEnergy);
   signal->SetBranchAddress("ahc_layerMax", &layerMax);
   signal->SetBranchAddress("ahc_layerStart", &layerStart);
   signal->SetBranchAddress("ahc_layerMaxDiffStart", &layerMaxDiffStart);

   Double_t response;
   Int_t arrIt;
   arrIt = signal->GetEntries();

   float *responseArr = new float [arrIt];
   float *energySumDataArr = new float [arrIt];
   float *energy1MomentumDataArr = new float [arrIt];
   float *energy2MomentumDataArr = new float [arrIt];
   float *energy3MomentumDataArr = new float [arrIt];
   float *energySum5RatioEnergySumDataArr = new float [arrIt];
   float *energyDensityDataArr = new float [arrIt];
   float *R90_hitsDataArr = new float [arrIt];
   float *N90ratioNDataArr = new float [arrIt];
   float *cellsMeanEnergyDataArr = new float [arrIt];
   int *layerMaxDataArr = new int [arrIt];
   int *layerStartDataArr = new int [arrIt];
   int *layerMaxDiffStartDataArr = new int [arrIt];
  
   for (Long64_t ievt=0; ievt<signal->GetEntries();ievt++) 
    {

      if (ievt%1000 == 0){cout << "--- ... Processing event: " << ievt << endl;}

      signal->GetEntry(ievt);

      //Evaluates classifier response for each event in data file  
      response = reader->EvaluateMVA(methodName);
      
      responseArr[ievt] = response;
      energySumDataArr[ievt] = ahc_energySum;
      energy1MomentumDataArr[ievt] = ahc_energy1Momentum;
      energy2MomentumDataArr[ievt] = ahc_energy2Momentum;
      energy3MomentumDataArr[ievt] = ahc_energy3Momentum;
      energySum5RatioEnergySumDataArr[ievt] = ahc_energySum5RatioEnergySum;
      energyDensityDataArr[ievt] = ahc_energyDensity;
      R90_hitsDataArr[ievt] = ahc_R90_hits;
      N90ratioNDataArr[ievt] = ahc_N90ratioN;
      cellsMeanEnergyDataArr[ievt] = ahc_cellsMeanEnergy;
      layerMaxDataArr[ievt] = layerMax;
      layerStartDataArr[ievt] = layerStart;
      layerMaxDiffStartDataArr[ievt] = layerMaxDiffStart;       
     }
   
   
       //Creates ROOT file to store classifier response and variables
        TFile *target = TFile::Open("TMVApp.root","RECREATE");
        TTree *_rootTree = new TTree("variables","analysis");
      
        _rootTree->Branch("ahc_response", &responseData, "responseData/F"); 
  	_rootTree->Branch("ahc_energySum", &energySumData, "energySumData/F"); 
  	_rootTree->Branch("ahc_energySum5RatioEnergySum", &energySum5RatioEnergySumData, "energySum5RatioEnergySumData/F"); 
  	_rootTree->Branch("ahc_energy1Momentum", &energy1MomentumData, "&energy1MomentumData/F");
  	_rootTree->Branch("ahc_energy2Momentum", &energy2MomentumData, "&energy2MomentumData/F");
  	_rootTree->Branch("ahc_energy3Momentum", &energy3MomentumData, "&energy3MomentumData/F");
  	_rootTree->Branch("ahc_energyDensity", &energyDensityData, "&energyDensityData/F");
  	_rootTree->Branch("ahc_R90_hits", &R90_hitsData, "&R90_hitsData/F"); 
  	_rootTree->Branch("ahc_N90ratioN", &N90ratioNData, "&N90ratioNData/F");
  	_rootTree->Branch("ahc_cellsMeanEnergy", &cellsMeanEnergyData, "&cellsMeanEnergyData/F"); 
  	_rootTree->Branch("ahc_layerMax", &layerMaxData, "&layerMaxData/I"); 
  	_rootTree->Branch("ahc_layerStart", &layerStartData, "&layerStartData/I");
  	_rootTree->Branch("ahc_layerMaxDiffStart", &layerMaxDiffStartData, "&layerMaxDiffStartData/I"); 

	//Fill ROOT file with classifier response and all variables
	for (Long64_t ievt=0; ievt<signal->GetEntries();ievt++) 
	{
      	responseData = responseArr[ievt];
      	energySumData = energySumDataArr[ievt];
      	energy1MomentumData = energy1MomentumDataArr[ievt];
      	energy2MomentumData = energy2MomentumDataArr[ievt];
      	energy3MomentumData = energy3MomentumDataArr[ievt];
      	energySum5RatioEnergySumData = energySum5RatioEnergySumDataArr[ievt];
      	energyDensityData = energyDensityDataArr[ievt];
      	R90_hitsData = R90_hitsDataArr[ievt];
      	N90ratioNData = N90ratioNDataArr[ievt];
      	cellsMeanEnergyData = cellsMeanEnergyDataArr[ievt];
      	layerMaxData = layerMaxDataArr[ievt];
      	layerStartData = layerStartDataArr[ievt];
      	layerMaxDiffStartData = layerMaxDiffStartDataArr[ievt];
    
      	_rootTree->Fill();
	}

      _rootTree->Write();
      target->Close();
      
      delete target;

      delete []responseArr;
      delete []energySumDataArr;
      delete []energy1MomentumDataArr;
      delete []energy2MomentumDataArr;
      delete []energy3MomentumDataArr;
      delete []energySum5RatioEnergySumDataArr;
      delete []energyDensityDataArr;
      delete []R90_hitsDataArr;
      delete []N90ratioNDataArr;
      delete []cellsMeanEnergyDataArr;
      delete []layerMaxDataArr;
      delete []layerStartDataArr;
      delete []layerMaxDiffStartDataArr;

      delete reader;    
       

   cout << "--- Created root file: \"TMVApp.root\" containing the MVA output histograms" << endl;  
   cout << "==> TMVAClassificationApplication is done!" << endl << endl;
}
