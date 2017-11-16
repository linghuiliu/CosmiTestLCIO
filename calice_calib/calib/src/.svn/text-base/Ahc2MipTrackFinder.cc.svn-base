#include <cfloat>
#include <iostream>
#include <string>

#include "EVENT/CalorimeterHit.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include <IMPL/LCCollectionVec.h>
#include <EVENT/LCCollection.h>

#include "HcalSingleHits.hh"
#include "EcalSingleHits.hh"
#include "Ahc2MipTrackFinder.hh"

#include "marlin/ConditionsProcessor.h"
#include <marlin/Exceptions.h>

using namespace std;
using namespace marlin;

Ahc2MipTrackFinder aAhc2MipTrackFinder;

Ahc2MipTrackFinder::Ahc2MipTrackFinder():Processor("Ahc2MipTrackFinder")
{
  _description = "Creates Tracks of AHCal/Ecal hits";

  registerProcessorParameter("AHCalCollectionName",
  "Name of the input AHCAL collection",
  _ahcinputColName,
  std::string(""));

  registerProcessorParameter("ECalCollectionName",
  "Name of the input ECAL collection",
  _emcinputColName,
  std::string(""));

  registerProcessorParameter("OutputAhcHitCollectionName",
  "Name of the output AHCal hit collection, of type CalorimeterHit",
  _ahcHitOutputColName,
  std::string(""));

  registerProcessorParameter("OutputEmcHitCollectionName",
  "Name of the output ECal hit collection, of type CalorimeterHit",
  _emcHitOutputColName,
  std::string(""));

  registerProcessorParameter("NHitsOnTower",
  "Cut on Number of Hits per Tower",
  _cutNTowerHits,
  (int) 8);

  registerProcessorParameter("NHitsPerLayer",
  "Cut on Number of Hits per Layer",
  _cutNLayerHits,
  (int) 2);

  registerProcessorParameter("MMaxHit",
  "Upper cut on the number of hits",
  _nMaxHits,
  (int) 40);

  registerProcessorParameter("MMinHit",
  "Lower cut on the number of hits",
  _nMinHits,
  (int) 0);

  registerProcessorParameter("KeepT0_CherenkowHits",
  "Flags to keep T0 and Cherenkow hits in the final collection",
  _keepT0Cherenkow,
  (bool) false);

  StringVec T0Example;
  T0Example.push_back("ICoordinate");
  T0Example.push_back("JCoordinate");
  T0Example.push_back("KCoordinate");

  registerOptionalParameter( "T0Handler" ,
  "List of T0s in the AHCAL"  ,
  _T0Vector ,
  T0Example ,
  T0Example.size()) ;

  StringVec CherenkowExample;
  CherenkowExample.push_back("ICoordinate");
  CherenkowExample.push_back("JCoordinate");
  CherenkowExample.push_back("KCoordinate");

  registerOptionalParameter( "CherenkowHandler" ,
  "List of Cherenkows in the AHCAL"  ,
  _CherenkowVector ,
  CherenkowExample ,
  CherenkowExample.size()) ;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

void Ahc2MipTrackFinder::init()
{
  printParameters();
  _mapT0s.clear();

  //Process T0 (I, J, K) and put then into a map
  if( parameterSet( "T0Handler" ) )
  {
    unsigned index = 0 ;
    while( index < _T0Vector.size() )
    {

      string strI( _T0Vector[ index++ ] );
      string strJ( _T0Vector[ index++ ] );
      string strK( _T0Vector[ index++ ] );

      int I = atoi(strI.c_str());
      int J = atoi(strJ.c_str());
      int K = atoi(strK.c_str());

      _mapT0s.push_back(I*10000+J*100+K);
    }

    streamlog_out(MESSAGE) << "Number of Layers with T0s: " << _mapT0s.size() << endl;
  }

  _mapCherenkows.clear();

  //Process Cherenkow (I, J, K) and put then into a map
  if( parameterSet( "CherenkowHandler" ) )
  {
    unsigned index = 0 ;
    while( index < _CherenkowVector.size() )
    {

      string strI( _CherenkowVector[ index++ ] );
      string strJ( _CherenkowVector[ index++ ] );
      string strK( _CherenkowVector[ index++ ] );

      int I = atoi(strI.c_str());
      int J = atoi(strJ.c_str());
      int K = atoi(strK.c_str());

      _mapCherenkows.push_back(I*10000+J*100+K);
    }

    streamlog_out(MESSAGE) << "Number of Layers with Cherenkows: " << _mapCherenkows.size() << endl;
  }

  _nEvt = 0;
  _nRun = 0;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

void Ahc2MipTrackFinder::processRunHeader(LCEvent *evt)
{
  _nRun++ ;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

void Ahc2MipTrackFinder::processEvent(LCEvent *evt)
{
  //Clear towers at each new event
  _towerHCALHits.clear();
  _towerECALHits.clear();

  //Add Event variable (number of hits per event and per layer before and after tracking)
  HitPerlayer.clear();
  HitPerlayer.resize(30, 0);

  int nHitsAHCAL = 0;
  nHCALtracks = 0;

  //Get collection names in the event
  typedef const std::vector<std::string> StringVec ;
  StringVec* strVec = evt->getCollectionNames() ;

  LCCollectionVec *ahcHitOutputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
  LCCollectionVec *emcHitOutputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);

  ahcHitOutputCol->setFlag(ahcHitOutputCol->getFlag() | 1 << LCIO::CHBIT_LONG );
  ahcHitOutputCol->setFlag(ahcHitOutputCol->getFlag() | 1 << LCIO::RCHBIT_TIME );
  emcHitOutputCol->setFlag(emcHitOutputCol->getFlag() | 1 << LCIO::CHBIT_LONG );
  emcHitOutputCol->setFlag(emcHitOutputCol->getFlag() | 1 << LCIO::RCHBIT_TIME );

  /* start loop over input collection */
  streamlog_out(DEBUG0)<<" \n\n=========================Start to process event "<< evt->getEventNumber() <<endl;

  for( StringVec::const_iterator it = strVec->begin(); it != strVec->end() ; it++)
  {
    streamlog_out(DEBUG0)<< "Collection : " << *it << endl;
    LCCollection *inCol = dynamic_cast<LCCollection*>(evt->getCollection(*it));
    string sss = it->c_str();

    //Take AHCAL Collection Name
    if(sss == _ahcinputColName)
    {
      CellIDDecoder<CalorimeterHit> decoder_ahc(inCol);
      _encodingHCAL = inCol->getParameters().getStringVal( "CellIDEncoding");

      bool hasKminus1 = false;
      if (_encodingHCAL.find("K-1") != std::string::npos)
      {
        hasKminus1 = true;
      }

      //Number of elements in the collection
      int noElem = inCol->getNumberOfElements();
      streamlog_out(DEBUG0)<<"inputcol "<< *it <<" has "<< noElem <<" hits"<<endl;

      //Loop over AHCAL Hits
      for ( int cellCounter = 0; cellCounter < inCol->getNumberOfElements(); cellCounter++ )
      {
        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inCol->getElementAt(cellCounter));

        int layer = 0;
        if (hasKminus1) layer = decoder_ahc(hit)["K-1"] + 1;
        else layer = decoder_ahc(hit)["K"];
        int I =  decoder_ahc(hit)["I"];
        int J = decoder_ahc(hit)["J"];

        const float* hitPos = hit->getPosition();
        float x = hitPos[0];
        float y = hitPos[1];
        float z = hitPos[2];

        //streamlog_out(DEBUG0)<< "Ampl " << ampl << " Time " << time << endl;
        if(Check_T0(I, J, layer) || Check_Cherenkow(I, J, layer))
        {
          streamlog_out(DEBUG0) << "T0/Cherenkow found " << endl;

          if(_keepT0Cherenkow)
          {
            //Store T0 hits from the event
            CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
            newHit->setEnergy(hit->getEnergy());
            newHit->setEnergyError(0.);
            newHit->setTime(hit->getTime());
            newHit->setType(hit->getType());

            newHit->setPosition(hit->getPosition());
            newHit->setCellID0(hit->getCellID0());

            //Create a new hit in the Track Collection
            ahcHitOutputCol->addElement(newHit);

            continue;
          }
          else
          continue;
        }

        //Fill Towers
        HitPerlayer[layer]++;
        Fill2DMap(_towerHCALHits, I, J, layer, x, y, z, hit->getEnergy(), hit->getTime(), hit->getType(), hit->getCellID0());
        nHitsAHCAL++;
      }
    }

    //Take ECAL Collection if present
    if(sss == _emcinputColName)
    {
      CellIDDecoder<CalorimeterHit> decoder_emc(inCol);
      _encodingECAL = inCol->getParameters().getStringVal( "CellIDEncoding");

      bool hasKminus1 = false;
      if (_encodingECAL.find("K-1") != std::string::npos)
      {
        hasKminus1 = true;
      }

      int noElem = inCol->getNumberOfElements();
      streamlog_out(DEBUG0)<<"inputcol "<< *it <<" has "<< noElem <<" hits"<<endl;

      //Loop over ECAL Hits
      for ( int cellCounter = 0; cellCounter < inCol->getNumberOfElements(); cellCounter++ )
      {
        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inCol->getElementAt(cellCounter));

        int layer = 0;
        if (hasKminus1) layer = decoder_emc(hit)["K-1"] + 1;
        else layer = decoder_emc(hit)["K"];
        int I =  decoder_emc(hit)["I"];
        int J = decoder_emc(hit)["J"];

        const float* hitPos = hit->getPosition();
        float x = hitPos[0];
        float y = hitPos[1];
        float z = hitPos[2];

        //Fill Towers
        //HitPerlayer[layer]++;
        Fill2DMap(_towerECALHits, I, J, layer, x, y, z, hit->getEnergy(), hit->getTime(), hit->getType(), hit->getCellID0());
      }
    }
  }

  //Check number of hits per layer in the event
  bool rejectEvt = false;
  for(std::vector<int>::iterator it = HitPerlayer.begin(); it != HitPerlayer.end(); ++it)
  {
    if((*it) > _cutNLayerHits) rejectEvt = true;
  }

  if(rejectEvt) return;

  //Check number of hits in the AHCAL in the event
  if(nHitsAHCAL < _nMinHits) return;
  if(nHitsAHCAL > _nMaxHits) return;

  if(_towerHCALHits.size() == 0) return;
  else
  {
    //Check number of hits in a Tower
    nHCALtracks = selectTowerNHits(_towerHCALHits, _cutNTowerHits);

    //No Tracks found
    if(nHCALtracks == 0)
    return;

    streamlog_out(DEBUG)<<"NTracks "<< nHCALtracks << " in evt " << evt->getEventNumber() << endl;

    //Loop over the HCAL towers
    for(TowerHitVectorHCAL::iterator itTowerI = _towerHCALHits.begin();  itTowerI != _towerHCALHits.end(); itTowerI++)
    {
      map<float, vector<  HcalSingleHit > > &thisTowerI = itTowerI->second;
      for(map<float, vector<  HcalSingleHit > >::iterator itTowerJ = thisTowerI.begin();  itTowerJ != thisTowerI.end();  itTowerJ++)
      {
        vector< HcalSingleHit > &HCALHits = itTowerJ->second;

        //No hits in that tower go to next tower
        if(HCALHits.empty()) continue;

        //Loop over hits in the Tower
        for(vector< HcalSingleHit >::iterator it = HCALHits.begin(); it != HCALHits.end(); it++)
        {
          CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
          newHit->setEnergy(it->energy);
          newHit->setEnergyError(0.);
          newHit->setTime(it->hittime);
          newHit->setType(it->hittype);

          float newPosition[3] = {it->x, it->y, it->z};
          newHit->setPosition(newPosition);
          newHit->setCellID0(it->CellID0);

          //Create a new hit in the Track Collection
          ahcHitOutputCol->addElement(newHit);

          streamlog_out(DEBUG0)<< "Hit : \n"
          << "Position : " << newPosition[0] << " " << newPosition[1] << " " << newPosition[2]
          << "\n Energy " << it->energy
          << "\n Time " << it->hittime << endl;
          streamlog_out(DEBUG0)<<"(HCAL) Adding new hit to collection  " << _ahcHitOutputColName << endl;
        }

        pair<float, float> Coordinate_TowerHCAL;
        Coordinate_TowerHCAL = make_pair(itTowerI->first, itTowerJ->first);

        //Check Overlap of ECAL Hits with HCAL Tower
        if(_towerECALHits.size() == 0) continue;
        else
        {
          //Loop over ECAL Towers
          for(TowerHitVectorECAL::iterator itTowerI = _towerECALHits.begin();  itTowerI != _towerECALHits.end(); itTowerI++)
          {
            map<float, vector<  EcalSingleHit > > &thisTowerI = itTowerI->second;
            for(map<float, vector<  EcalSingleHit > >::iterator itTowerJ = thisTowerI.begin();  itTowerJ != thisTowerI.end();  itTowerJ++)
            {
              vector< EcalSingleHit > &ECALHits = itTowerJ->second;

              //No Hits in ECAL tower
              if(ECALHits.empty()) continue;

              pair<float, float> Coordinate_TowerECAL;
              Coordinate_TowerECAL = make_pair(itTowerI->first, itTowerJ->first);

              //Check if a ECAL Tower overlap with HCAL Tower
              bool overlap = CheckOverlap(Coordinate_TowerECAL, Coordinate_TowerHCAL);

              if(overlap)
              {
                IncrementOverlap(ECALHits);

                //Loop over ECAL hits in the Tower
                for(vector< EcalSingleHit >::iterator it = ECALHits.begin(); it != ECALHits.end(); it++)
                {
                  CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
                  newHit->setEnergy(it->energy);
                  newHit->setEnergyError(0.);
                  newHit->setTime(it->hittime);
                  newHit->setType(it->hittype);

                  float newPosition[3] = {it->x, it->y, it->z};
                  newHit->setPosition(newPosition);
                  newHit->setCellID0(it->CellID0);

                  //Create a new hit in the Track Collection
                  emcHitOutputCol->addElement(newHit);
                  streamlog_out(DEBUG0)<< "Hit : \n"
                  << "Position : " << newPosition[0] << " " << newPosition[1] << " " << newPosition[2]
                  << "\n Energy " << it->energy
                  << "\n Time " << it->hittime << endl;
                  streamlog_out(DEBUG0)<<"(ECAL) Adding new hit to collection  " << _emcHitOutputColName << endl;
                }//end loop ecal hits
              }//end overlap
            }
          }//end loop ECAL Towers
        }
      }
    }//end loop HCAL Towers
  }

  if(ahcHitOutputCol->getNumberOfElements() == 0)
  return;
  else
  {
    evt->parameters().setValue("nTracks", (int)nHCALtracks);
    evt->parameters().setValue("cutNTowerHits", (int)_cutNTowerHits);

    _nEvt++;

    LCParameters &theParam = ahcHitOutputCol->parameters();
    theParam.setValue(LCIO::CellIDEncoding, _encodingHCAL);

    evt->addCollection(ahcHitOutputCol, _ahcHitOutputColName.c_str());
    streamlog_out(DEBUG0)<<"Add collection  "<< _ahcHitOutputColName << " with " << ahcHitOutputCol->getNumberOfElements() << " Hits" << endl;

    if(emcHitOutputCol->getNumberOfElements() > 0)
    {
      LCParameters &theParam = emcHitOutputCol->parameters();
      theParam.setValue(LCIO::CellIDEncoding, _encodingECAL);

      evt->addCollection(emcHitOutputCol, _emcHitOutputColName.c_str());
      streamlog_out(DEBUG0)<<"Add collection  "<< _emcHitOutputColName << " with " << emcHitOutputCol->getNumberOfElements() << " Hits" << endl;
    }
  }
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

void Ahc2MipTrackFinder::check( LCEvent * evt )
{

}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

void Ahc2MipTrackFinder::end()
{
  std::cout << "Ahc2MipTrackFinder::end()  " << name()
  << " processed " << _nEvt << " events in " << _nRun << " runs "
  << std::endl ;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

void Ahc2MipTrackFinder::printParameters()
{
  std::cerr << "============= Ahc2MipTrackFinder =================" <<std::endl;
  std::cerr << "Select MIP Tracks in the AHCAL" <<std::endl;
  std::cerr << "Input AHCAL Collection name : " << _ahcinputColName << std::endl;
  std::cerr << "Output AHCAL Collection Name : " << _ahcHitOutputColName << std::endl;
  std::cerr << "Input ECAL Collection name : " << _emcinputColName << std::endl;
  std::cerr << "Output ECAL Collection Name : " << _emcHitOutputColName << std::endl;
  std::cerr << "Number of hits per events : " << _nMinHits << " < nHits < " << _nMaxHits << std::endl;
  std::cerr << "Track length minimum : " << _cutNTowerHits << std::endl;
  std::cerr << "Maximum nhits per layer : " << _cutNLayerHits << std::endl;
  std::cerr << "Keep Time Reference : " << _keepT0Cherenkow << std::endl;
  std::cerr << "=======================================================" <<std::endl;
  return;
}


/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//HCAL Tower Filling Function

int Ahc2MipTrackFinder::Fill2DMap(TowerHitVectorHCAL &TowerMap, int I, int J, int K, float x, float y, float z, Float_t ahc_hitEnergy,Float_t ahc_hitTime, Int_t ahc_hitType, int CellID0)
{

  HcalSingleHit HcalHit;
  HcalHit.I = I;
  HcalHit.J = J;
  HcalHit.K = K;
  HcalHit.x = x;
  HcalHit.y = y;
  HcalHit.z = z;
  HcalHit.energy = ahc_hitEnergy;
  HcalHit.hittime = ahc_hitTime;
  HcalHit.CellID0 = CellID0;
  HcalHit.hittype = ahc_hitType;
  TowerMap[x][y].push_back(HcalHit);

  return 0;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//ECAL Tower Filling Function

int Ahc2MipTrackFinder::Fill2DMap(TowerHitVectorECAL &TowerMap, int I, int J, int K, float x, float y, float z, Float_t emc_hitEnergy, Float_t emc_hitTime, Int_t emc_hitType, int CellID0)
{

  EcalSingleHit EcalHit;
  EcalHit.I = I;
  EcalHit.J = J;
  EcalHit.K = K;
  EcalHit.x = x;
  EcalHit.y = y;
  EcalHit.z = z;
  EcalHit.energy = emc_hitEnergy;
  EcalHit.hittime = emc_hitTime;
  EcalHit.CellID0 = CellID0;
  EcalHit.hittype = emc_hitType;
  EcalHit.multiplicity = 0;
  EcalHit.Overlap_strip = false;
  TowerMap[x][y].push_back(EcalHit);

  return 0;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//Function of select Tower over MinNHits

int Ahc2MipTrackFinder::selectTowerNHits(TowerHitVectorHCAL &towers, unsigned int MinNHits)
{
  int nTowersLeft = 0;
  streamlog_out(DEBUG0)<<"Select HCAL Towers" <<endl;
  TowerHitVectorHCAL::iterator itTowerI;

  for(itTowerI = towers.begin();  itTowerI != towers.end();)
  {
    map<float, vector<  HcalSingleHit > > &thisTowerI = itTowerI->second;
    for(map<float, vector<  HcalSingleHit > >::iterator itTowerJ = thisTowerI.begin();  itTowerJ != thisTowerI.end();)
    {
      vector< HcalSingleHit > &HCALHits = itTowerJ->second;
      if (HCALHits.size() >= MinNHits){ //if the currently viewed tower has enough hits, qualify as potential track
        // and keep it
        ++itTowerJ;
        nTowersLeft++;
      } else {
        // delete current tower, but increment iterator first
        thisTowerI.erase(itTowerJ++);
      }
    }
    if(thisTowerI.size() == 0){ // if current tower is now empty
      towers.erase(itTowerI++); // delete current tower, but increment iterator first
    } else { // else keep it
      ++itTowerI;
    }
  }
  return nTowersLeft;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//Incrementing function for overlapping strips

int Ahc2MipTrackFinder::IncrementOverlap(vector<EcalSingleHit> &ECALHits)
{
  for(vector<EcalSingleHit>::iterator it = ECALHits.begin(); it!= ECALHits.end(); it++)
  {
    it->Overlap_strip = true;
  }
  return 1;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//Overlap Check function based on geometry of strips (45*5 mm) and EPT HCAL tiles (3*3 cm)

bool Ahc2MipTrackFinder::CheckOverlap(pair<float, float> Coordinates_ECAL, pair<float, float> Coordinates_HCAL)
{
  bool Overlap = false;
  if((TMath::Abs(Coordinates_ECAL.first - Coordinates_HCAL.first) <= 37.5 && TMath::Abs(Coordinates_ECAL.second - Coordinates_HCAL.second) <= 17.5) || (TMath::Abs(Coordinates_ECAL.first - Coordinates_HCAL.first) <= 17.5 && TMath::Abs(Coordinates_ECAL.second - Coordinates_HCAL.second) <= 37.5))
  {
    streamlog_out(DEBUG0)<< "Overlap ECAL Hit with HCAL Tower" << endl;
    Overlap = true;
  }
  return Overlap;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//Remove T0 from Spectrum (Check if Hit is a T0 or not)

bool Ahc2MipTrackFinder::Check_T0(int Ihit, int Jhit, int Khit)
{
  bool is_T0 = false;

  int index = Ihit*10000+Jhit*100+Khit;
  if( find(_mapT0s.begin(), _mapT0s.end(), index) != _mapT0s.end() )
  {
    //streamlog_out(MESSAGE)<< "T0 found " << it->first << " " << it->second << " " << Khit << endl;
    is_T0 = true; //T0 found -> reject hit
  }

  return is_T0;
}

/***************************************************************************************/
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/*                                                                                                                                                     */
/***************************************************************************************/

//Remove Cherenkow from Spectrum (Check if Hit is a Cherenkow or not)

bool Ahc2MipTrackFinder::Check_Cherenkow(int Ihit, int Jhit, int Khit)
{
  bool is_Cherenkow = false;

  int index = Ihit*10000+Jhit*100+Khit;

  if( find(_mapCherenkows.begin(), _mapCherenkows.end(), index) != _mapCherenkows.end() )
  {
    //streamlog_out(MESSAGE)<< "Cherenkow found " << it->first << " " << it->second << " " << Khit << endl;
    is_Cherenkow = true; //Cherenkow found -> reject hit
  }

  return is_Cherenkow;
}
