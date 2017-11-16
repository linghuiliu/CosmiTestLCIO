#include "HitWriteEngine.hh"

#include <cfloat>
#include <iostream>
#include <string>

#include "EVENT/CalorimeterHit.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/CellIDDecoder.h"

#include "ModuleLocation.hh"
#include "CellIndex.hh"
#include "HcalCellIndex.hh"
#include "AhcAmplitude.hh"

using namespace lcio;
using namespace std;
using namespace CALICE;

#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

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

int cellsize( int cellid, bool coarse);

/* This engine can be run within the RootTreeWriter
 * it was tested for root 5.16.00
 * depending on the required information level it will fill
 * 0: integrated values only
 * 1: layer by layer information in addition
 * 2: radial information in addition
 *
 * Due to our weired mapping it is only running for the HCAL up to now
 */
namespace marlin
{
  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void HitWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_caloType",
						   "HitWriteEngine prefix to tree variables,e.g. Ahc,Emc or Tcmt",
						   _prefix,
						   std::string("caloType"));

    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_informationLevel",
						   "HitWriteEngine information level 0: integrated values,"
						   " 1: layer by layer information,"
						   " 2: radial information,"
						   " 3: cell wise information in addition",
						   _level,
						   int(2));

    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_mip2GeV",
						   "HitWriteEngine conversion factor from calibrated mip to gev",
						   _samplingFraction,
						   float(1));

    _hostProcessor.relayRegisterInputCollection(LCIO::CALORIMETERHIT,_engineName+"_InCol",
						"Name of input collection",
						_hitColName, std::string("CalorimeterHits")  );

    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_eventFiltering",
						   "Disable (0 - default) or Enable (1) collect"
						   " energySum5Layer and nHits5Layer",
						   _eventFiltering,
						   int(0));

    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_cogIJenable",
						   "Disable (0 - default) or Enable (1) calculating of cogI "
						   "(Cell Column) and cogJ (Cell Row) in first 5 layers",
						   _cogIJenable,
						   int(0));
    
    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_mipCut",
						   "apply a mip cut",
						   _mipCut,float(0.5));

    _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_useAhcAmplCol",
						   "Fill the tree with variables from the AhcAmplitude collection",
						   _useAhcAmplCol, bool(false));

   _hostProcessor.relayRegisterProcessorParameter("HitWriteEngine_AhcAmplRelCollectionName",
						  "Name of the AHCal amplitude collection, of type LCGenericObject",
						  _ahcAmplRelColName,
						  std::string("AhcHitAmplitudeRelation"));

   }


  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
   void HitWriteEngine::registerBranches( TTree* hostTree )
  {
    if ( _prefix.size() > 0 )
      if ( _prefix[ _prefix.length()-1 ] != '_' )
	_prefix += "_";

    /*---integrated values: information level 0---*/

    hostTree->Branch(string(_prefix+"iEvt").c_str(),     &_hitsFill.iEvt, 
		     string(_prefix+"iEvt/I").c_str());
    hostTree->Branch(string(_prefix+"nHits").c_str(),    &_hitsFill.nHits, 
		     string(_prefix+"nHits/I").c_str());
    hostTree->Branch(string(_prefix+"nLayers").c_str(),  &_hitsFill.nLayers, 
		     string(_prefix+"nLayers/I").c_str());
    hostTree->Branch(string(_prefix+"energySum").c_str(),&_hitsFill.energySum, 
		     string(_prefix+"energySum/F").c_str());
    hostTree->Branch(string(_prefix+"energyDensity").c_str(),&_hitsFill.energyDensity, 
		     string(_prefix+"energyDensity/F").c_str());
    hostTree->Branch(string(_prefix+"radius").c_str(),&_hitsFill.radius, 
		     string(_prefix+"radius/F").c_str());
    hostTree->Branch(string(_prefix+"radiusEw").c_str(),&_hitsFill.radiusEw, 
		     string(_prefix+"radiusEw/F").c_str());
    hostTree->Branch(string(_prefix+"cogX").c_str(),&_hitsFill.cogX, 
		     string(_prefix+"cogX/F").c_str());
    hostTree->Branch(string(_prefix+"cogY").c_str(),&_hitsFill.cogY, 
		     string(_prefix+"cogY/F").c_str());
    hostTree->Branch(string(_prefix+"cogZ").c_str(),&_hitsFill.cogZ, 
		     string(_prefix+"cogZ/F").c_str());

    /*------ adding energy weighted center of gravity with I, J cell indexes-------*/
    if ( _cogIJenable > 0 ) 
      {
	hostTree->Branch(string(_prefix+"cogI").c_str(),&_hitsFill.cogI, 
			 string(_prefix+"cogI/F").c_str());
	hostTree->Branch(string(_prefix+"cogJ").c_str(),&_hitsFill.cogJ, 
			 string(_prefix+"cogJ/F").c_str());
	hostTree->Branch(string(_prefix+"cogIGeom").c_str(),&_hitsFill.cogIGeom, 
			 string(_prefix+"cogIGeom/F").c_str());
	hostTree->Branch(string(_prefix+"cogJGeom").c_str(),&_hitsFill.cogJGeom, 
			 string(_prefix+"cogJGeom/F").c_str());
      }
    
    if ( _eventFiltering > 0 ) 
      {
	hostTree->Branch(string(_prefix+"energySum5Layer").c_str(),&_hitsFill.energySum5Layer, 
			 string(_prefix+"energySum5Layer/F").c_str());
	hostTree->Branch(string(_prefix+"nHits5Layer").c_str(),    &_hitsFill.nHits5Layer, 
			 string(_prefix+"nHits5Layer/I").c_str());
	hostTree->Branch(string(_prefix+"cogX5Layer").c_str(),&_hitsFill.cogX5Layer, 
			 string(_prefix+"cogX5Layer/F").c_str());
	hostTree->Branch(string(_prefix+"cogY5Layer").c_str(),&_hitsFill.cogY5Layer, 
			 string(_prefix+"cogY5Layer/F").c_str());
	hostTree->Branch(string(_prefix+"cogZ5Layer").c_str(),&_hitsFill.cogZ5Layer, 
			 string(_prefix+"cogZ5Layer/F").c_str());
      }

    /*---layer by layer: information level 1----*/

    if (_level > 0) 
      {
	hostTree->Branch(string(_prefix+"energyPerLayer").c_str(),    &_hitsFill.lEnergy, 
			 string(_prefix+"energyPerLayer["+_prefix+"nLayers]/F").c_str());
	hostTree->Branch(string(_prefix+"nHitsPerLayer").c_str(),     &_hitsFill.lNHits, 
			 string(_prefix+"nHitsPerLayer["+_prefix+"nLayers]/I").c_str());
	hostTree->Branch(string(_prefix+"energyPerLayer_err").c_str(),&_hitsFill.lEnergy_err, 
			 string(_prefix+"energyPerLayer_err["+_prefix+"nLayers]/F").c_str());
	
	hostTree->Branch(string(_prefix+"cellSize").c_str(),&_hitsFill.cellSize, 
			 string(_prefix+"cellSize["+_prefix+"nHits]/I").c_str());
	/*-------radial dependencies: information level 2---------*/

      if (_level > 1) 
	{
	  hostTree->Branch(string(_prefix+"cogXPerLayer").c_str(),  &_hitsFill.lCogX, 
			   string(_prefix+"cogXPerLayer["+_prefix+"nLayers]/F").c_str());
	  hostTree->Branch(string(_prefix+"cogYPerLayer").c_str(),  &_hitsFill.lCogY, 
			   string(_prefix+"cogYPerLayer["+_prefix+"nLayers]/F").c_str());
	  hostTree->Branch(string(_prefix+"radiusPerLayer").c_str(),&_hitsFill.lRadius, 
			   string(_prefix+"distanceFromCogPerLayer["+_prefix+"nLayers]/F").c_str()); 
	  hostTree->Branch(string(_prefix+"radiusEwPerLayer").c_str(),&_hitsFill.lRadiusEw, 
			   string(_prefix+"distanceFromCogEwPerLayer["+_prefix+"nLayers]/F").c_str());
	  
	  /*---- adding energy weighted center of gravity with I, J cell indexes---*/
        if ( _cogIJenable > 0 ) 
	  {
	    hostTree->Branch(string(_prefix+"cogIPerLayer").c_str(),&_hitsFill.lCogI, 
			     string(_prefix+"cogIPerLayer["+_prefix+"nLayers]/F").c_str());
	    hostTree->Branch(string(_prefix+"cogJPerLayer").c_str(),&_hitsFill.lCogJ, 
			     string(_prefix+"cogJPerLayer["+_prefix+"nLayers]/F").c_str());
	    hostTree->Branch(string(_prefix+"cogIGeomPerLayer").c_str(),&_hitsFill.lCogIGeom, 
			     string(_prefix+"cogIGeomPerLayer["+_prefix+"nLayers]/F").c_str());
	    hostTree->Branch(string(_prefix+"cogJGeomPerLayer").c_str(),&_hitsFill.lCogJGeom, 
			     string(_prefix+"cogJGeomPerLayer["+_prefix+"nLayers]/F").c_str());
	  }
	}

      /*-------- cell dependencies: information level 3------------*/

      if (_level > 2) 
	{
	  hostTree->Branch(string(_prefix+"hitCellID").c_str(),&_hitsFill.cellID, 
			   string(_prefix+"hitCellID["+_prefix+"nHits]/I").c_str());
	  hostTree->Branch(string(_prefix+"hitI").c_str(),&_hitsFill.hitI, 
			   string(_prefix+"hitI["+_prefix+"nHits]/I").c_str());
	  hostTree->Branch(string(_prefix+"hitJ").c_str(),&_hitsFill.hitJ, 
			   string(_prefix+"hitJ["+_prefix+"nHits]/I").c_str());
	  hostTree->Branch(string(_prefix+"hitK").c_str(),&_hitsFill.hitK, 
			   string(_prefix+"hitK["+_prefix+"nHits]/I").c_str());
	  hostTree->Branch(string(_prefix+"hitEnergy").c_str(),&_hitsFill.hitEnergy, 
			   string(_prefix+"hitEnergy["+_prefix+"nHits]/F").c_str());
	  hostTree->Branch(string(_prefix+"hitTime").c_str(),&_hitsFill.hitTime, 
			   string(_prefix+"hitTime["+_prefix+"nHits]/F").c_str());
	  hostTree->Branch(string(_prefix+"hitType").c_str(),&_hitsFill.hitType, 
			   string(_prefix+"hitType["+_prefix+"nHits]/I").c_str());
	  hostTree->Branch(string(_prefix+"hitRadius").c_str(),&_hitsFill.hitRadius, 
			   string(_prefix+"hitRadius["+_prefix+"nHits]/F").c_str());
	  hostTree->Branch(string(_prefix+"hitEnergyDensity").c_str(),&_hitsFill.hitEnergyDensity, 
			   string(_prefix+"hitEnergyDensity["+_prefix+"nHits]/F").c_str());
	  hostTree->Branch(string(_prefix+"hitPos").c_str(),   &_hitsFill.hitPos, 
			   string(_prefix+"hitPos["+_prefix+"nHits][3]/F").c_str());

	  if (_useAhcAmplCol == true)
	    {
	      hostTree->Branch(string(_prefix+"cellTemperature").c_str(),&_hitsFill.cellTemperature, 
			       string(_prefix+"cellTemperature["+_prefix+"nHits]/F").c_str());
	      hostTree->Branch(string(_prefix+"hitAmplRawADC").c_str(),&_hitsFill.hitAmplRawADC, 
			       string(_prefix+"hitAmplRawADC["+_prefix+"nHits]/F").c_str());
	      hostTree->Branch(string(_prefix+"hitAmplRawMinusPedestalADC").c_str(),&_hitsFill.hitAmplRawMinusPedestalADC, 
			       string(_prefix+"hitAmplRawMinusPedestalADC["+_prefix+"nHits]/F").c_str());
	    }
	}
      }

    /* init not yet initialised variables*/

    ievt = 0;

  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void HitWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    LCCollection* inCol;
    
    try {
      inCol = evt->getCollection( _hitColName );

 //      std::string encoding = "";
//       try{
// 	encoding = inCol->getParameters().getStringVal( "CellIDEncoding");
//       }
//       catch ( lcio::Exception & aExc ) 
// 	{
// 	  cout<<"\n\nn what "<<endl;
// 	  //abort();
// 	  LCParameters &param = inCol->parameters();
// 	  param.setValue(LCIO::CellIDEncoding, "M:3,S-1:3,I:9,J:9,K-1:6");
// 	  encoding = inCol->getParameters().getStringVal( "CellIDEncoding");

// 	}/*catch*/

      std::string encoding = inCol->getParameters().getStringVal( "CellIDEncoding");
      if (encoding == "")
	{
	  LCParameters &param = inCol->parameters();
	  /*set default encoding if no encoding present*/
	  param.setValue(LCIO::CellIDEncoding, "M:3,S-1:3,I:9,J:9,K-1:6");
	  encoding = inCol->getParameters().getStringVal( "CellIDEncoding");
	  
	}/*catch*/
   
      CellIDDecoder<CalorimeterHit> decoder(inCol);
  

      bool hasKminus1 = false;
      if (encoding.find("K-1") != std::string::npos)
	{
	  hasKminus1 = true;
	}

      streamlog_out(DEBUG) <<"\n Event "<<evt->getEventNumber()<<", loop over collection "<<_hitColName
		   <<" with "<<inCol->getNumberOfElements()<<" elements "<<endl;
      streamlog_out(DEBUG) <<" colName: "<<_hitColName<<" hasKminus1: "<<hasKminus1<<" encoding: "<<encoding<<endl;
      
      typedef LCTypedVector<CalorimeterHit> CHVect;
      //typedef CHVect::iterator CHIt;
      CHVect calHits( inCol );
              

      _hitsFill.energySum5Layer = -10000;
      _hitsFill.nHits5Layer     = 0;
      _hitsFill.cogX            = -10000;
      _hitsFill.cogY            = -10000;
      _hitsFill.cogZ            = -10000;
      _hitsFill.cogX5Layer      = -10000;
      _hitsFill.cogY5Layer      = -10000;
      _hitsFill.cogZ5Layer      = -10000;

      for ( unsigned int layerCounter = 0; layerCounter < MAXLAYERS ; layerCounter++ ) 
	{
	  nhits[layerCounter]          = 0;
	  energy[layerCounter]         = 0; 
	  energy_err[layerCounter]     = -10000; //TODO:
	  lcogx[layerCounter]          = 0;
	  lcogy[layerCounter]          = 0;
	  lcogI[layerCounter]          = 0;
	  lcogJ[layerCounter]          = 0;
	  lcogIGeom[layerCounter]      = 0;
	  lcogJGeom[layerCounter]      = 0;
	  lradius[layerCounter]        = 0;
	  lradiusEw[layerCounter]      = 0;
	  lEnergyDensity[layerCounter] = 0;
	}
      
      /*FIXME: should be really available number of layers (from cellIndex?)*/
      _hitsFill.nLayers = MAXLAYERS;
      
      eSum     = 0;
      nIJHits  = 0;
      cogx     = 0;
      cogy     = 0;
      cogz     = 0;
      cogI     = 0;
      cogJ     = 0;
      cogIGeom = 0;
      cogJGeom = 0;
      radius   = 0;
      radiusEw = 0;     
      tilesize = 0;
      ampl     = 0;
      time     = 0;
      type     = 0;
      ampl_GeV = 0;
      
      if ( _cogIJenable > 0 ) 
	{
	  cogIJeSum = 0;
	} 

      if ( _eventFiltering > 0 ) 
	{
	  esum_5Layer    = 0;
	  numHits5Layer  = 0;
	  esum_all5Layer = 0;
	  cogx5          = 0;
	  cogy5          = 0;
	  cogz5          = 0;
	}      
      
      LCRelationNavigator *navigator = NULL;
      if (_useAhcAmplCol == true)
	{
	  LCCollection *relAmpCol = evt->getCollection(_ahcAmplRelColName);
	  navigator = new LCRelationNavigator(relAmpCol);
	}
      

      nHits = 0; /* counter for real hits (level 3 branch filling and nHits filling)*/

      for ( int cellCounter = 0; cellCounter < inCol->getNumberOfElements() && cellCounter < (int)MAXCELLS ; cellCounter++ ) 
	{
	  CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inCol->getElementAt(cellCounter));
	  if( hit->getEnergy() < _mipCut ) continue;
	  
	  const float* hitPos =  hit->getPosition();

	  /*
	    FIXME: how does this work for the rest (ECAL, TCMT, ...) ?
	    HcalCellIndex cellIdx(calHits[cellCounter]->getCellID0());
	  */
// 	  CellIndex cellIdx(hit->getCellID0());
// 	  UInt_t layer = cellIdx.getLayerIndex();
    
	  int layer = 0;
	  if (hasKminus1) layer = decoder(hit)["K-1"] + 1;
	  else            layer = decoder(hit)["K"];
  	  
	  //HcalCellIndex cellIdx0(hit->getCellID0());
	  //Int_t I_tileColumn = cellIdx0.getTileColumn();
	  //Int_t J_tileRow = cellIdx0.getTileRow(); 
	  
	  Int_t I_tileColumn = decoder(hit)["I"];
	  Int_t J_tileRow = decoder(hit)["J"];

	  if ( (unsigned) layer >= MAXLAYERS )
	    {
	      cout << "WARNING: layer ["<<layer<<"] to large. "
		"This should not happen! Probably your reconstruction "
		"is broken. Please report this bug. Set layer to " 
		   << MAXLAYERS -1 << "." << endl; 
	      layer = MAXLAYERS-1;
	    }
	  
	  ampl = hit->getEnergy() * _samplingFraction;
	  time = hit->getTime();
	  type = hit->getType();

	  /*------------------------------------------------------------
	   Always fill integrated values (level 0)
	   Fill the energy sum and the number of hits for a ring outside 28 cm
	   (variable needed to reject background events, suggested by Vassily Morgunov)
	  ----------------------------------------------------------------*/
	  
	  if ( _eventFiltering > 0 ) 
	    {
	    if (layer <= 5)
	      {
		cogx5 += hitPos[0] * ampl;
		cogy5 += hitPos[1] * ampl;
		cogz5 += hitPos[2] * ampl;
		esum_all5Layer += ampl;
		
		radius = TMath::Sqrt(hitPos[0] * hitPos[0] + hitPos[1] * hitPos[1]);

		if (radius >= 280.0)
		  {
		    esum_5Layer += ampl;
		    numHits5Layer++;	      
		  }
	      }
	    }/*end of if _eventFiltering*/
	
		
	  /*------- Fill cell information just in highest level (level 2)-----------*/
	  if ( _level > 2 ) 
	    {
	      _hitsFill.cellID[nHits]    = hit->getCellID0();
	      _hitsFill.hitI[nHits]      = I_tileColumn;
	      _hitsFill.hitJ[nHits]      = J_tileRow;
	      _hitsFill.hitK[nHits]      = layer;
	      _hitsFill.hitEnergy[nHits] = ampl;
	      _hitsFill.hitTime[nHits] = time;
	      _hitsFill.hitType[nHits] = type;
	      _hitsFill.hitPos[nHits][0] = hitPos[0];
	      _hitsFill.hitPos[nHits][1] = hitPos[1];
	      _hitsFill.hitPos[nHits][2] = hitPos[2];

	      if (_useAhcAmplCol)
		{
		  /*fill the temperature*/
		  const EVENT::LCObjectVec &amplVec = navigator->getRelatedToObjects(hit);
		  
		  if (amplVec.size() > 0) 
		    {
		      LCGenericObject *obj = dynamic_cast<LCGenericObject*> (amplVec[0]);
		      AhcAmplitude *ahcAmpl = new AhcAmplitude(obj);
		      float temperature = ahcAmpl->getTemperature();
		      _hitsFill.cellTemperature[nHits]            = temperature;
		      _hitsFill.hitAmplRawADC[nHits]              = ahcAmpl->getAmplRawADC();
		      _hitsFill.hitAmplRawMinusPedestalADC[nHits] = ahcAmpl->getAmplRawMinusPedestalADC();
		      
		      delete ahcAmpl;
		    }
		  else
		    {
		      cout<<"AhcAmpl collection empty"<<endl;
		    }
		}
	    }

	  eSum += ampl;
	  nHits ++;
	  ievt ++;
	  
	  if ( _level > 1 ) 
	    {
	      if ( (_cogIJenable > 0) && (layer <= 5) ) 
		{
		  nIJHits ++;
		  cogIJeSum += ampl;
		  cogI      += I_tileColumn * ampl;
		  cogJ      += J_tileRow * ampl;
		  cogIGeom  += I_tileColumn;
		  cogJGeom  += J_tileRow;
		}
	      cogx += hitPos[0] * ampl;
	      cogy += hitPos[1] * ampl;
	      cogz += hitPos[2] * ampl;
	    }
	
	  /*-------------------------------------------------------------------------
	    For more detailed analysis also fill layer by layer information (level 1)
	    ----------------------------------------------------------------------*/
	  if ( _level > 0 ) 
	    {
	      energy[layer] += ampl;
	      nhits[layer]++;
	      
	      /*level 2 information even allows for radial quantities */
	      if ( _level > 1 ) 
		{
		  lcogx[layer] += hitPos[0] * ampl;
		  lcogy[layer] += hitPos[1] * ampl;
		  
		  if ( _cogIJenable > 0 ) 
		    {
		      lcogI[layer] += I_tileColumn * ampl;
		      lcogJ[layer] += J_tileRow * ampl;
		      lcogIGeom[layer] += I_tileColumn;
		      lcogJGeom[layer] += J_tileRow;
		    }
		}
	    }/*information level > 0 */
	  /*-------------------------------------------------------------------------*/
	}/* end loop over all hits*/

      _hitsFill.energySum = eSum;
      _hitsFill.iEvt = ievt;
      _hitsFill.nHits = nHits;
      
      if ( _eventFiltering > 0 ) 
	{
	  _hitsFill.energySum5Layer = esum_5Layer;
	  _hitsFill.nHits5Layer =  numHits5Layer;
	  _hitsFill.cogX5Layer  = cogx5/esum_all5Layer;
	  _hitsFill.cogY5Layer  = cogy5/esum_all5Layer;
	  _hitsFill.cogZ5Layer  = cogz5/esum_all5Layer;
      }
      
      if ( eSum > 0 ) 
	{     
	  _hitsFill.cogX   = cogx/eSum;
	  _hitsFill.cogY   = cogy/eSum;
	  _hitsFill.cogZ   = cogz/eSum;
	}
      
      if ( (_cogIJenable > 0) && (cogIJeSum > 0)) 
	{
	  _hitsFill.cogI = cogI/cogIJeSum;
	  _hitsFill.cogJ = cogJ/cogIJeSum;
	}

      /* calculate a geometrical cogIGeom, cogJGeom*/
      if ( (_cogIJenable > 0) && (nIJHits > 0)) 
	{
	  _hitsFill.cogIGeom = cogIGeom/nIJHits;
	  _hitsFill.cogJGeom = cogJGeom/nIJHits;
	}
      
      
      /*-------------------------------------------------------------------------*/
      /*for level 1 information loop over all layers*/
      if ( _level >0 ) 
	{ 
	  for ( unsigned int layerCounter = 0; layerCounter < MAXLAYERS ; layerCounter++ ) 
	    {
	      _hitsFill.lNHits[layerCounter]      = nhits[layerCounter];
	      _hitsFill.lEnergy[layerCounter]     = energy[layerCounter];
	      _hitsFill.lEnergy_err[layerCounter] = energy_err[layerCounter];
	      
	      if ( _level > 1 && energy[layerCounter] > 0)
		{
		  _hitsFill.lCogX[layerCounter] = lcogx[layerCounter]/energy[layerCounter];
		  _hitsFill.lCogY[layerCounter] = lcogy[layerCounter]/energy[layerCounter];
		  
		}/*information level >1*/
	      
	      if ( _level > 1 && _cogIJenable > 0 && energy[layerCounter] > 0 ) 
		{
		  _hitsFill.lCogI[layerCounter] = lcogI[layerCounter]/energy[layerCounter];
		  _hitsFill.lCogJ[layerCounter] = lcogJ[layerCounter]/energy[layerCounter];
		  _hitsFill.lCogIGeom[layerCounter] = lcogIGeom[layerCounter]/nhits[layerCounter];
		  _hitsFill.lCogJGeom[layerCounter] = lcogJGeom[layerCounter]/nhits[layerCounter];
		}/*information level >1*/
	      
	    }/*all layers*/
	  
	}/*information level > 0*/

      /*-------------------------------------------------------------------------*/

      double radGeom  = 0;
      double lradGeom = 0;

      /*for level 2 information loop over all hits twice*/
      if ( _level >1 ) 
	{
	  nHits = 0; // again reset real number of hits counter
	  for ( unsigned int cellCounter = 0; cellCounter < calHits.size() && cellCounter < MAXCELLS ; cellCounter++ ) 
	    {
	      if( calHits[cellCounter]->getEnergy() < _mipCut ) continue;
	    
	      /*
		FIXME: how does this work for the rest (ECAL, TCMT, ...) ?
		HcalCellIndex cellIdx(calHits[cellCounter]->getCellID0());
	      */
	      CellIndex cellIdx(calHits[cellCounter]->getCellID0());
	      int layer = cellIdx.getLayerIndex();
	      
	      tilesize  = cellsize(calHits[cellCounter]->getCellID0(),false);
	      _hitsFill.cellSize[nHits] = tilesize;
	 
	      const float* hitPos =  calHits[cellCounter]->getPosition();
	      lradGeom = sqrt(pow((lcogx[layer]/energy[layer] - hitPos[0]), 2) +
			      pow((lcogy[layer]/energy[layer] - hitPos[1]), 2));
	      lradius[layer] += lradGeom;
	      
	      radGeom = sqrt(pow((cogx/eSum - hitPos[0]), 2)+
			     pow((cogy/eSum - hitPos[1]), 2));		  	  
	      radius += radGeom;
	      ampl = calHits[cellCounter]->getEnergy() * _samplingFraction;
	      
	      lradiusEw[layer] += ampl/(tilesize*tilesize) * lradGeom;
	      lEnergyDensity[layer] += ampl/(tilesize * tilesize);
	      
	      /* Fill cell information just in the highest level (3) */
	      if ( _level > 2)
		{
		  _hitsFill.hitRadius[nHits] = radGeom;
		  _hitsFill.hitEnergyDensity[nHits] = ampl/(tilesize * tilesize);
		}
            nHits++;
	    }/*all hits*/

	for ( unsigned int layerCounter = 0; layerCounter < MAXLAYERS ; layerCounter++ ) 
	  {
	    radiusEw += lradiusEw[layerCounter];
	    _hitsFill.energyDensity += lEnergyDensity[layerCounter];
	    
	    if(nhits[layerCounter] > 0)
	      {
		_hitsFill.lRadius[layerCounter] = lradius[layerCounter]/nhits[layerCounter];
		_hitsFill.lRadiusEw[layerCounter] = lradiusEw[layerCounter]/lEnergyDensity[layerCounter];
	      } 
	    else 
	      {
		_hitsFill.lRadius[layerCounter] = 0;
		_hitsFill.lRadiusEw[layerCounter] = 0;
	      }
	  }/*all layers*/

	_hitsFill.radius = radius/nHits;
	_hitsFill.radiusEw = radiusEw/_hitsFill.energyDensity;

	}/*information level > 1*/

      if (_useAhcAmplCol) delete navigator;

    }/*try*/
    
    catch ( DataNotAvailableException err ) 
      {
	resetHitsFill();
	streamlog_out(DEBUG) <<  "HitWriteEngine WARNING: Collection "<< _hitColName
	     << " not available in event "<< evt->getEventNumber() << endl;
      }/*catch*/
    
  }/*fillVariables*/

  void HitWriteEngine::resetHitsFill() {

	  _hitsFill.iEvt = 0;
	  _hitsFill.nHits = 0;
	  _hitsFill.nLayers = 0;         /**< number of layers < MAXLAYERS !*/
	  _hitsFill.energySum = 0;
	  _hitsFill.energyDensity = 0;
	  _hitsFill.cogX = 0;            /**< center of gravity weighted by energy on whole calo*/
	  _hitsFill.cogY = 0;
	  _hitsFill.cogZ = 0;
	  _hitsFill.cogI = 0;            /**<center of gravity with cell index I (column)*/
	  _hitsFill.cogJ = 0;            /**<center of gravity with cell index J (row)*/
	  _hitsFill.cogIGeom = 0;        /**<geometrical (not weighted) center of gravity with cell index I (column)*/
	  _hitsFill.cogJGeom = 0;        /**<geometrical (not weighted) center of gravity with cell index J (row)*/
	  _hitsFill.radius = 0;          /**<shower radius (in x,y plane) calculated w.r.t. the cog */
	  _hitsFill.radiusEw = 0;        /**<shower radius (in x,y plane) calculated w.r.t. the cog energy weighted*/

	  for (int i=0; i<MAXCELLS; i++){
		  _hitsFill.cellID[i] = 0;
		  _hitsFill.hitI[i] = 0;
		  _hitsFill.hitJ[i] = 0;
		  _hitsFill.hitK[i] = 0;
		  _hitsFill.hitEnergy[i] = 0;
		  _hitsFill.hitTime[i] = 0;
		  _hitsFill.hitType[i] = 0;
		  _hitsFill.hitRadius[i] = 0;        /**< hit distance to the cog  (ideally one wants to replace
		  the cog with the DC track)*/
		  _hitsFill.hitEnergyDensity[i] = 0; /**< hit energy weighted by tile_size^2*hitRadius (dimention: E/mm^3)*/
		  _hitsFill.hitPos[i][0] = 0;
		  _hitsFill.hitPos[i][1] = 0;
		  _hitsFill.hitPos[i][2] = 0;
		  _hitsFill.cellSize[i] = 0;
		  _hitsFill.cellTemperature[i] = 0;
		  _hitsFill.hitAmplRawADC[i] = 0;
		  _hitsFill.hitAmplRawMinusPedestalADC[i] = 0;
	  }

	  for (int i=0; i<MAXLAYERS; i++){

		  _hitsFill.lNHits[i] = 0;        /**< layer by layer number of hits*/
		  _hitsFill.lEnergy[i] = 0;       /**< layer by layer Energy */
		  _hitsFill.lEnergy_err[i] = 0;   /**< layer by layer Energy error (not yet filled in CalorimeterHit class)*/

		  _hitsFill.lCogX[i] = 0;         /**< center of gravity weighted by energy in one layer*/
		  _hitsFill.lCogY[i] = 0;
		  _hitsFill.lCogI[i] = 0;         /**< center of gravity in I,J weighted by energy in one layer*/
		  _hitsFill.lCogJ[i] = 0;
		  _hitsFill.lCogIGeom[i] = 0;     /**< geometrical (not weighted) center of gravity in I,J */
		  _hitsFill.lCogJGeom[i] = 0;
		  _hitsFill.lRadius[i] = 0;       /**< shower radius (in x,y plane) in one layer calculated w.r.t. the cog*/
		  _hitsFill.lRadiusEw[i] = 0;     /**< energy weighted shower radius (in x,y plane) in
		  one layer calculated w.r.t. the cog*/
	  }
	  _hitsFill.energySum5Layer = 0;          /**< energy Sum for the first 5 layer outside 28 cm*/
	  _hitsFill.nHits5Layer = 0;                /**< number of Hits for the first 5 layer outside 28 cm*/

	  _hitsFill.cogX5Layer = 0;      /**< center of gravity weighted by energy over the first 5 layers*/
	  _hitsFill.cogY5Layer = 0;
	  _hitsFill.cogZ5Layer = 0;

  }
  
}/*namespace marlin*/


/*********************************************************************************/
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
int cellsize( int cellid, bool coarse = false )
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


