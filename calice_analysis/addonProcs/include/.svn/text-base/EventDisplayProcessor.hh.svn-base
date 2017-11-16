#ifndef EVENTDISPLAYPROCESSOR_hh
#define EVENTDISPLAYPROCESSOR_hh 1

#include <cmath> //needed for detector geometry

// LCIO includes
#include "lcio.h"
#include "EVENT/CalorimeterHit.h"

// Marlin includes
#include "marlin/Processor.h"

// CALICE includes
#include "AhcMapper.hh"
#include "MappedContainer.hh"
#include "CellNeighbours.hh"
#include "CellDescription.hh"

#include "HcalHistosDrawer.hh"

using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  /**  @brief This is a Marlin processor for displaying events with CED.
   *
   * If you want to display events with CED, you need to start the display (<CODE>glced</CODE>) before running Marlin.
   *
   * Mouse commands that can be used within CED:
   *
   *    - <CODE> left + move </CODE> -> rotate view
   *    - <CODE> right + move down/up </CODE> -> zoom view in/out
   *    - <CODE> center + move </CODE> -> shift view
   *    - <CODE> wheel </CODE> -> shift view along z-axis
   *
   *
   * Keyboard commands that can be used within CED:
   *
   *    - <CODE> b </CODE> -> toggle background color
   *    - <CODE> r </CODE> -> got to default angular view
   *    - <CODE> s </CODE> -> got to default side view
   *    - <CODE> f </CODE> -> got to default front view
   *    - <CODE> v </CODE> -> switch on/off fisheye view
   *    - <CODE> 0 ... 9 </CODE> -> switch on/off CED display layer 0 ... 9
   *    - <CODE> shift + 0 ... 9 </CODE> -> switch on/off display layer 10 ... 19
   *    - <CODE> t </CODE> -> switch on/off CED display layer 20
   *    - <CODE> y </CODE> -> switch on/off CED display layer 21
   *    - <CODE> u </CODE> -> switch on/off CED display layer 22
   *    - <CODE> i </CODE> -> switch on/off CED display layer 23
   *
   *
   * Assignment of CED display layers:
   *
   *    -  0:
   *    -  1: AHCAL / ECAL / TCMT hits ( hit energy < 0.5 mip )
   *    -  2: AHCAL / ECAL / TCMT hits ( 0.5 mip <= hit energy < 1.65 mip )
   *    -  3: AHCAL / ECAL / TCMT hits ( 1.65 mip <= hit energy < 2.9 mip )
   *    -  4: AHCAL / ECAL / TCMT hits ( 2.9 mip <= hit energy < 5.4 mip )
   *    -  5: AHCAL / ECAL / TCMT hits ( hit energy >= 5.4 mip )
   *    -  6:
   *    -  7:
   *    -  8:
   *    -  9:
   *    - 10:
   *    - 11: MC particle track
   *    - 12: shower start layer
   *    - 13: shower start position
   *    - 14:
   *    - 15:
   *    - 16:
   *    - 17:
   *    - 18:
   *    - 19:
   *    - 20: cell frames: AHCAL hits
   *    - 21: cell frames: AHCAL tiles 3x3 cm^2
   *    - 22: cell frames: AHCAL tiles 6x6 cm^2
   *    - 23: cell frames: AHCAL tiles 12x12 cm^2
   *
   * CED color coding for hit energies:
   *
   * @image html EventDisplayProcessor_colorCode.png
   * @image latex EventDisplayProcessor_colorCode.eps "CED color coding" width=10cm
   *
   *
   * @author B. Lutz, DESY
   * @author N. Feege, University of Hamburg
   * @version 1.0
   * @date April 2010
   *
   */
  class EventDisplayProcessor : public Processor {

  public:

    virtual Processor*  newProcessor() { return new EventDisplayProcessor ; }


    EventDisplayProcessor() ;

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() ;

    /** Called for every run.
     */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /** Called for every event - the working horse.
     */
    virtual void processEvent( LCEvent * evt ) ;

    virtual void check( LCEvent * evt ) ;

    /** Called after data processing for clean up.
     */
    virtual void end() ;


  protected:

    std::string _colNameEmc;

    std::string _colNameAhc;
    std::string _colNameAhcSim;

    std::string _colNameTcm;
    std::string _colNameTcmSim;

    std::string _colNameMCParticle;

    std::string _parNameShowerStartLayer;
    std::string _parNameShowerStartPos;

    std::string _mappingProcessorName;
    std::string _cellNeighboursProcessorName;
    std::string _cellDescriptionProcessorName;
    
    std::string _fMokkaModelName;

    std::string _showerShapeColNames;
    std::string _trackColName;

    int _waitForKeyPressed;
    int _draw;
    int _appendToExistingCED;

    int _nRun ;
    int _nEvt ;

    const CALICE::AhcMapper *_mapper;
    unsigned int _mapperVersion;

    CALICE::MappedContainer<CALICE::CellDescription>* _cellDescriptions;
    CALICE::MappedContainer<CALICE::CellNeighbours>* _cellNeighbours;

    void chooseColorAndLayer(const float energy, unsigned int& color, unsigned int& layer) const;

    void drawCell(const CALICE::CellDescription* description, const unsigned int color, const unsigned int layer, const bool solid = true) const ;
    void drawHit(const CalorimeterHit* hit, const unsigned int color, const unsigned int layer) const ;

    void drawSiWECAL(LCEvent *evt);

    void drawAHCAL(LCEvent *evt);
    void drawAHCALTracks(LCEvent *evt);
    void drawAHCALSim(LCEvent *evt);

    void drawTCMT(LCEvent *evt);

    void drawStartLayer(LCEvent *evt);
    void drawStartPosition(LCEvent *evt);

    void drawMCParticle(LCEvent *evt);

    void drawDetectorSiWECAL();
    void drawDetectorAHCAL();
    void drawDetectorTCMT();

    bool _drawHcalHistos;
    bool _tcmtStartVertical;
    HcalHistosDrawer *_theHcalHistosDrawer;
    int getMarinaShowerStartHcalLayer(LCEvent *evt);
    std::string _showerStartColName;
    void drawPrimaryTrackHitsPosition(LCEvent *evt);
    void drawShowerShapes(LCEvent *evt);
    int returnClusterSize(float eneCluster, float cutoff_min, float cutoff_max); 
  } ;

}

#endif
