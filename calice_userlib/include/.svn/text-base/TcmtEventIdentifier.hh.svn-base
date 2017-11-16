#ifndef TcmtEventIdentifier_hh
#define TcmtEventIdentifier_hh 1

// LCIO includes
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"

// c++ includes
#include <string>

// CALICE includes
#include "BinnedVector.hh"
#include "TcmtEventBits.hh"

namespace CALICE {

  /**
   * class that can identify type of event in TCMT
   *
   * The signal in the TCMT is analysed and compared to different thresholds.
   * Following observables are used:
   * \li \c energy \c sum compared to min and max threshold
   * \li \c number \c hits compared to min and max threshold
   * \li \c number \c muon \c like \c layers compared to min threshold (the hits and energy in the layer have to be in range of a MIP signal)
   * \li \c number \c muon \c like \c towers compared to min and max threshold (the the sum of neighboring cells over consecutive layers have to exceed a certain limit)
   *
   * @see TcmtEventBits
   *
   * @todo implement thresholds for TCMT back section
   * @todo remove hardcoded thresholds
   *
   * @author Benjamin.Lutz@desy.de
   * @date November 2009
   * @version 1.0
   */
  class TcmtEventIdentifier {
  public:
    /**
     * constructor
     *
     * thresholds are set inside here, currently hardcoded
     *
     * @param[in] maxLayer maximum layer index (internally layer counting starts with 1)
     * @param[in] startBackLayer first layer to be considered back section of TCMT
     * @param[in] forcedEncoding can be used to overwrite the encoding string from the collection (to work around reconstruction bug)
     */
    TcmtEventIdentifier(const unsigned int maxLayer=16, const unsigned int startBackLayer = 9, const std::string &forcedEncoding = "");

    /**
     * function to do the calculation on a collection
     *
     * @param[in] col TCMT input collection
     */
    void process( lcio::LCCollection *col );

    /**
     * function to add the results as collection parameters
     *
     * @param[out] col TCMT collection
     */
    void addResults( lcio::LCCollection *col );


    /**
     * function to add the int with the event type bits to the event
     *
     * @param[in] parNameTcmtEventBits name of the parameter that should contain the bits
     * @param[out] evt event for which the parameter should be added
     */
    void addResults( const std::string parNameTcmtEventBits, lcio::LCEvent *evt );

  private:

    /**
     * internal function to set the thresholds
     *
     * @param[in] layerMuonHitMinCut     min number of hits to consider layer muon like
     * @param[in] layerMuonHitMaxCut     max number of hits to consider layer muon like
     * @param[in] layerMuonEnergyMinCut  min energy sum to consider layer muon like
     * @param[in] layerMuonEnergyMaxCut  max energy sum to consider layer muon like
     * @param[in] sumMuonHitMinCut       min number of hits to consider signal originating from muon
     * @param[in] sumMuonHitMaxCut       max number of hits to consider signal originating from muon
     * @param[in] sumMuonEnergyMinCut    min energy sum to consider signal originating from muon
     * @param[in] sumMuonEnergyMaxCut    max energy sum to consider signal originating from muon
     * @param[in] towerMuonHitCut        min number of hits in tower to consider tower muon like
     * @param[in] numberTowersMuonMinCut min number of muon like tower to consider event originating from muon
     * @param[in] numberTowersMuonMaxCut max number of muon like tower to consider event originating from muon
     * @param[in] numberLayersMuonCut    min number of muon like layers to consider event originating from muon
     */
    void setMuonThresholds( const int   layerMuonHitMinCut,
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
                            const int   numberLayersMuonCut);

    void reset();

    float _energy;
    std::vector<float> _energyS;
    float _rmsE;
    float _energyBack;
    int _nHits;
    int _nHitsBack;
    std::vector<float> _layerEnergy;
    std::vector<int>   _layerHits;

    BinnedVector<float, int> _xPos;
    BinnedVector<float, int> _yPos;
    BinnedVector<float, int> _xPosBack;
    BinnedVector<float, int> _yPosBack;

    int _nMuonLikeTowers;
    int _nMuonLikeLayers;

    std::string  _forcedEncoding;
    unsigned int _maxLayer;
    unsigned int _startBackLayer;

    int   _layerMuonHitMinCut;
    int   _layerMuonHitMaxCut;
    float _layerMuonEnergyMinCut;
    float _layerMuonEnergyMaxCut;

    int   _sumMuonHitMinCut;
    int   _sumMuonHitMaxCut;
    float _sumMuonEnergyMinCut;
    float _sumMuonEnergyMaxCut;

    float _backEnergyRatio;

    int   _towerMuonHitCut;
    int   _numberTowersMuonMinCut;
    int   _numberTowersMuonMaxCut;

    int _numberLayersMuonCut;


    TcmtEventBits _bits;
  };

} // end namespace CALICE

#endif
