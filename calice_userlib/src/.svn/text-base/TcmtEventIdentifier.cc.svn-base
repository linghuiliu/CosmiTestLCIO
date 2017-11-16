#include "TcmtEventIdentifier.hh"

#include "EVENT/CalorimeterHit.h"

#include "FastDecoder.hh"
#include "CaliceException.hh"

#include <cmath>

namespace CALICE {

  TcmtEventIdentifier::TcmtEventIdentifier(const unsigned int maxLayer, const unsigned int startBackLayer, const std::string &forcedEncoding) {

    reset();

    _maxLayer = maxLayer;
    _forcedEncoding = forcedEncoding;
    _startBackLayer = startBackLayer;
    _xPos.setBinning(20,-500.,500,0);
    _yPos.setBinning(20,-500.,500,0);
    _xPosBack.setBinning(20,-500.,500,0);
    _yPosBack.setBinning(20,-500.,500,0);

    setMuonThresholds(1,4, 0.75,5.5, 11,32, 15.,53., 6, 2,6, 9 );
  }

  void TcmtEventIdentifier::setMuonThresholds( const int   layerMuonHitMinCut,
                                               const int   layerMuonHitMaxCut,
                                               const float layerMuonEnergyMinCut,
                                               const float layerMuonEnergyMaxCut,
                                               const int   sumMuonHitMinCut,
                                               const int   sumMuonHitMaxCut,
                                               const float sumMuonEnergyMinCut,
                                               const float sumMuonEnergyMaxCut,
                                               const int   towerMuonHitCut,
                                               const int   numberTowersMuonMinCut,
                                               const int   numberTowersMuonMaxCut,
                                               const int   numberLayersMuonCut) {

    _layerMuonHitMinCut     = layerMuonHitMinCut     ;
    _layerMuonHitMaxCut     = layerMuonHitMaxCut     ;
    _layerMuonEnergyMinCut  = layerMuonEnergyMinCut  ;
    _layerMuonEnergyMaxCut  = layerMuonEnergyMaxCut  ;
    _sumMuonHitMinCut       = sumMuonHitMinCut       ;
    _sumMuonHitMaxCut       = sumMuonHitMaxCut       ;
    _sumMuonEnergyMinCut    = sumMuonEnergyMinCut    ;
    _sumMuonEnergyMaxCut    = sumMuonEnergyMaxCut    ;
    _towerMuonHitCut        = towerMuonHitCut        ;
    _numberTowersMuonMinCut = numberTowersMuonMinCut ;
    _numberTowersMuonMaxCut = numberTowersMuonMaxCut ;
    _numberLayersMuonCut    = numberLayersMuonCut    ;
  }

  void TcmtEventIdentifier::reset() {

    _nHits      = 0;
    _nHitsBack  = 0;
    _energy     = 0.;
    _energyBack = 0.;

    _backEnergyRatio = 0;

    _nMuonLikeLayers = 0;
    _nMuonLikeTowers = 0;

    _layerHits.clear();
    _layerEnergy.clear();
    _layerEnergy.resize(_maxLayer+1,0);
    _layerHits.resize(_maxLayer+1,0);

    _energyS.clear();
    _energyS.resize(4,0);
    _rmsE = 0;

    _xPos.clear();
    _yPos.clear();
    _xPosBack.clear();
    _yPosBack.clear();

  }

  void TcmtEventIdentifier::process( lcio::LCCollection *col ) {

    std::string encodingString = col->getParameters().getStringVal("CellIDEncoding");

    if (_forcedEncoding != "") encodingString = _forcedEncoding;
    else if (encodingString == "") encodingString = "M:3,S-1:3,I:9,J:9,K-1:6";
                                                    
    FastDecoder* Kcoder = FastDecoder::generateDecoder(encodingString,"K");

    reset();

    for (int i =0; i < col->getNumberOfElements(); ++i ) {

      lcio::CalorimeterHit *hit = dynamic_cast<lcio::CalorimeterHit*>( col->getElementAt(i) );

      if (hit) {

        int   cellID = hit->getCellID0();
        float energy = hit->getEnergy();

        unsigned int K = Kcoder->decodeU(cellID);

        if (K == 0 || K > _maxLayer)
	  {
	    std::cout<<"\n TcmtEventIdentifier: K="<<K<<" maxLayer: "<<_maxLayer<<std::endl;
	    throw BadDataException("TcmtEventIdentifier::process() K outside range");
	  }

        bool vertical  = K%2;

        const float *position = hit->getPosition();

        if (vertical) _xPos[position[0]]++;
        else _yPos[position[1]]++;

        _energy         += energy;
        ++_nHits;
        _layerEnergy[K] += energy;
        ++(_layerHits[K]);

        _energyS[(K-1)/4] += energy;

        if (K >= _startBackLayer) {

          if (vertical) _xPosBack[position[0]]++;
          else _yPosBack[position[1]]++;

          _energyBack += energy;
          ++_nHitsBack;
        }

      }
    }

    _backEnergyRatio = _energyBack/_energy;

    for (unsigned int i =1; i < _layerEnergy.size(); ++i) {
      _rmsE += pow(_layerEnergy[i]-(_energy/(float)(_layerEnergy.size() -1 )),2);
    }
    _rmsE = sqrt(_rmsE);

    for (unsigned int layer=1; layer < _maxLayer; ++layer)
      if (   _layerEnergy[layer] >= _layerMuonEnergyMinCut && _layerEnergy[layer] <= _layerMuonEnergyMaxCut
             && _layerHits[layer]   >= _layerMuonHitMinCut    && _layerHits[layer] <= _layerMuonHitMaxCut )
        ++_nMuonLikeLayers;

    std::vector<int> xTowers = _xPos.getVector();
    std::vector<int> yTowers = _yPos.getVector();

    for (unsigned int tower = 0; tower < xTowers.size()-1; ++tower)
      if ( (xTowers[tower] + xTowers[tower+1]) >= _towerMuonHitCut )
        ++_nMuonLikeTowers;

    for (unsigned int tower = 0; tower < yTowers.size()-1; ++tower)
      if ( (yTowers[tower] + yTowers[tower+1]) >= _towerMuonHitCut )
        ++_nMuonLikeTowers;


    _bits.clear();

    if (_energy >= _sumMuonEnergyMinCut) _bits.setMinSumEnergyMuon(true);
    if (_energy <= _sumMuonEnergyMaxCut) _bits.setMaxSumEnergyMuon(true);

    if (_nHits >= _sumMuonHitMinCut) _bits.setMinNumberHitsMuon(true);
    if (_nHits <= _sumMuonHitMaxCut) _bits.setMaxNumberHitsMuon(true);

    if (_nMuonLikeLayers >= _numberLayersMuonCut) _bits.setMinNumberMuonLikeLayers(true);

    if (_nMuonLikeTowers <= _numberTowersMuonMaxCut)  _bits.setMaxNumberMuonLikeTowers(true);
    if (_nMuonLikeTowers >= _numberTowersMuonMinCut)  _bits.setMinNumberMuonLikeTowers(true);

    delete Kcoder;
  }

  void TcmtEventIdentifier::addResults( const std::string parNameTcmtEventBits, lcio::LCEvent *evt ) {
    evt->parameters().setValue(parNameTcmtEventBits,_bits.getInt());
  }

  void TcmtEventIdentifier::addResults( lcio::LCCollection *col ) {

    col->parameters().setValue("eventBits",_bits.getInt());
    IntVec bitVec =  _bits.getIntVec();
    col->parameters().setValues("eventBitVec",bitVec);

    col->parameters().setValue("nMuonLayers",_nMuonLikeLayers);
    col->parameters().setValue("nMuonTowers",_nMuonLikeTowers);

    col->parameters().setValue("energy",_energy);
    col->parameters().setValue("energyS0",_energyS[0]);
    col->parameters().setValue("energyS1",_energyS[1]);
    col->parameters().setValue("energyS2",_energyS[2]);
    col->parameters().setValue("energyS3",_energyS[3]);
    col->parameters().setValue("energyRMS",_rmsE);
    col->parameters().setValue("energyBack",_energyBack);
    col->parameters().setValue("backEnergyRatio",_backEnergyRatio);
    col->parameters().setValue("nHits",(int)_nHits);
    col->parameters().setValue("nHitsBack",(int)_nHitsBack);
    col->parameters().setValues("layerEnergy",_layerEnergy);
    col->parameters().setValues("layerHits",_layerHits);


    lcio::IntVec workAroundMissingConst;
    workAroundMissingConst = _xPos.getVector();
    col->parameters().setValues("xPosDistribution", workAroundMissingConst );
    workAroundMissingConst = _yPos.getVector();
    col->parameters().setValues("yPosDistribution", workAroundMissingConst );
    workAroundMissingConst = _xPosBack.getVector();
    col->parameters().setValues("xPosBackDistribution",workAroundMissingConst);
    workAroundMissingConst = _yPosBack.getVector();
    col->parameters().setValues("yPosBackDistribution",workAroundMissingConst);
  }

} // end namespace CALICE
