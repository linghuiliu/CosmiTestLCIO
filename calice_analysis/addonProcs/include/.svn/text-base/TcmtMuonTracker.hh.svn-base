#ifndef TCMT_MUON_TRACKER_H
#define TCMT_MUON_TRACKER_H

#include "marlin/Processor.h"

#include <map>
#include <list>

#include "EVENT/CalorimeterHit.h"

#include "DecoderSet.hh"

namespace CALICE {

  /** Processor which identifies muons in the TCMT by creating 2D (i-j) towers from hits in horizontal and vertical strips.
   *
   *  The tower dimension is defined by the overlap betwen TCMT strips with different orientation (5cm x 5 cm).
   *
   *  If the number of hits and the energy sum in the largest tower are above the corresponding thresholds, the event is
   *  flagged as containing a muon by setting an event parameter.
   *
   *  The maximum tower is the tower with the largest number of hits (largest visible energy if tie).
   *
   *  The processor appends the following event parameters for the maximum tower found to each event:
   *
   *  ProcessorName_maxTower_eSum            -> total visible energy from all hits in this tower
   *  ProcessorName_maxTower_nHits           -> all hits in this tower
   *  ProcessorName_maxTower_x               -> x-position of tower center
   *  ProcessorName_maxTower_y               -> y-position of tower center
   *  ProcessorName_maxTower_trackID_eSum    -> total visible energy from all hits in this tower that are NOT ISOLATED in z
   *  ProcessorName_maxTower_trackID_nHits   -> all hits in this tower that are NOT ISOLATED in z
   *  ProcessorName_maxTower_nNeighbours     -> number of neighboring towers
   *  ProcessorName_maxTower_col_eSum        -> total visible energy from all hits in this tower + neighboring towers
   *  ProcessorName_maxTower_col_nHits       -> all hits in this tower + neighboring towers
   *
   *  @author nils.feege@desy.de
   *  @date Oct 2010
   *
   */

  class TcmtMuonTracker : public marlin::Processor  
  {
  public:
    Processor*  newProcessor() { return new TcmtMuonTracker ; }

    TcmtMuonTracker();

    ~TcmtMuonTracker() {};

    /** Called at the begin of the job before anything is read.
     *  Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run){};

    /** Called for every event - this is where the actual action is taking place.      */
    void processEvent( LCEvent * evt );

    void end(){};

  protected:

    /** The name of the TCMT hit input collection
     */
    std::string  _tcmHitColName;

    /** Hits with energies below this threshold are ignored
     */
    float _mipThreshold;

    /** last layer taken into account (counting from 1 to 16)
     */
    int _kThreshold;

    /** The minimum number of isolated hits required for a tower (combined horizontal and vertical strips) for identifying a muon in the event
     */
    IntVec _mu_nHits_isolated;

    /** The minimum energy sum (isolated hits) required for a tower (combined horizontal and vertical strips) for identifying a muon in the event
     */
    FloatVec _mu_eSum_isolated;

    /**The name of the output collection (which contains the hits belonging to the muon track)
     */
    std::string _outputColName;

    /** Flag to set the orientation of the first TCMT layer (1=vertical, 0=horizontal)
     */
    bool _tcmtStartVertical;
    
    /** structure collecting tower information
     */
    struct tcmtTower {
      unsigned i;
      unsigned j;
      float    eSum;
      float    eSum_front;
      float    eSum_back;
      int      nHits;
      int      nHits_front;
      int      nHits_back;
      float    xstart;
      float    xend;
      float    ystart;
      float    yend;
      float    zstart;
      float    zend;
      int      trackID_nHits;
      float    trackID_eSum;
      int      nNeighbours;
      int      col_nHits;
      float    col_eSum;
    } _newTower;

    static const unsigned int MAX_TCMT_LAYERS = 16;

    /** define ID for tower
     */
    unsigned getTowerIDfromIJ( unsigned i_pos, unsigned j_pos ) 
    {
      return i_pos * 100 + j_pos;
    };
    
    /** get J from tower ID
     */
    unsigned getJfromTowerID( unsigned towerID ) 
    {
      return ( towerID % 100 );
    };

    /** get I from tower ID
     */
    unsigned getIfromTowerID( unsigned towerID ) 
    {
      return ( towerID - ( getJfromTowerID( towerID ) ) / 100 );
    };

    /** Function filling the tower map by looking for crosses in consecutive layers; the orientation of the first hit layer is not relevant
     */
    void find2DTowers( std::map<unsigned, CalorimeterHit*>, DecoderSet*, std::list<unsigned>, 
		       std::list<unsigned>, std::map<unsigned,tcmtTower> & );


    /** Function filling the tower map by looking for crosses in consecutive layers, i.e. 'hits' in 'superlayers', 
	first hit of cross must be in odd layer, second hit in the next layer
    */
    //void find2DCrosses( std::map<unsigned, CalorimeterHit*>, DecoderSet*, std::list<unsigned>, std::list<unsigned>, std::map<unsigned,tcmtTower> & );


    /** Function merging towers
     */
    void merge2DTowers( std::map<unsigned,tcmtTower> & , std::map<unsigned,tcmtTower> & );

    /** function adding neighbour information to singel tower
     */
    void findNeighbours( tcmtTower & , std::map<unsigned,tcmtTower> & );

    /**Fill output collection with the hits which belong to the muon tower
     */
    void fillOutputCollection(LCEvent *evt, tcmtTower &muonTower, LCCollection *inputCol, 
			      std::map<unsigned, CalorimeterHit*> hitMap_all, DecoderSet* decoder);
 };

}

#endif
