#ifndef _TBTrackDigitizer_hh_
#define _TBTrackDigitizer_hh_

#include <string>
#include <iostream>

#include "TBTrackBaseProcessor.hh"


//namespace CALICE {
  
  /* 
   * Version 1.1: 6/2/2007
   * DESY output set as TDC output (DC1 and DC2 swapped), CERN setup implemented.
   *
   * Version 1.0: 6/7/2006.
   * First release of the processor, DESY setup implemented.
   *
   * Description of the processor:
   * This processor performs the digitization of the MC hit produced in the 
   * Drift Chambers. The input collections are the four collection of hits in
   * the DCs, the output collection is a collection such as  the one produced
   * by the lcio converter for the proper TDC. The processor smear the DC hits
   * according to a given sigma.
   *
   * The options avilable are (updated to v1.1):
   * DC1_MC_CollectionName 		  Name of input DC1 Collection from MC
   * DC2_MC_CollectionName 		  Name of input DC2 Collection from MC
   * DC3_MC_CollectionName 		  Name of input DC3 Collection from MC
   * DC4_MC_CollectionName 		  Name of input DC4 Collection from MC
   * TBTrackHitCollectionName	  Name of output TDC collection
   * TBTrackTRUEHitCollectionName 	  not used at the moment
   * Cut_De_Dx				  Value above which hits in DC are used to calculate impact point
   * Misallignment			  for misallignment studies only, to be set to 0
   * Sigma				  Chamber resolution
   * DriftVelocity			  Gas drift velocity (or propagation time for CERN DC)
   * PrototypeModel			  Name of Prototype used (TBDesy0506,TBCern0806,..)
   * Debug				  Set to 1 for debug, all cout enabled.
   *
   * DC collection from simulation in DESY:
   * TBdch02_dchSD1 z = -1880
   * TBdch02_dchSD2 z = -880
   * TBdch02_dchSD3 z = -2780
   * TBdch02_dchSD4 z = -3780
   *
   * Wire position in DESY (in TDC channel order: DC2,DC1,DC3,DC4)
   *
   * X1 and X3 is LEFT
   * X2 and X4 is RIGHT
   * Y1 and Y3 is DOWN
   * Y2 and Y4 is UP
   *
   * Wire position in CERN
   *
   * 0, 4 and 2* is DOWN 
   * 1, 5 and 3* is UP
   * 2, 0* and 5* is LEFT
   * 3, 1* and 6* is RIGHT
   * the * stand for the second TDC 
   * 
   * Autor: Michele Faucci Giannelli, Royal Holloway Univerisity of London
   * email: giannell@pp.rhul.ac.uk or faucci@cern.ch
   */
  
  class TBTrackDigitizer : public TBTrackBaseProcessor {

  public:
    Processor*  newProcessor() {
      return new TBTrackDigitizer;
    }
    
    TBTrackDigitizer();
    
    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void Init() {}
    
    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    void ProcessRunHeader(LCRunHeader *run) {} 
    
    /** Called for every event
     */
    void ProcessEvent(LCEvent *evtP);
    
    void End() {}

    
  protected:
    
    std::string _dc1ColName;             /**< name of the DC1 collection (parameter). */
    std::string _dc2ColName;             /**< name of the DC2 collection (parameter). */
    std::string _dc3ColName;             /**< name of the DC3 collection (parameter). */
    std::string _dc4ColName;             /**< name of the DC4 collection (parameter). */
    std::string _trackerHitColName;      /**< if not empty a tracker hit collection of this name is added to the event.*/ 
    std::string _TRUETrackerHitColName;      /**< if not empty a tracker hit collection of this name is added to the event.*/ 

    /*
    std::string _driftChamberParColName;
    int _debug;
    double _sigma;
    double _drift_velocity;
    double _cut_de_dx;
    int  _no_all_hit;
    double _misallign;
    std::string _model;

    int n;
    double totx;
    double toty;
    double dimention;
    int timex;
    int timey;
    int timex1;
    int timey1;
    int timex2;
    int timey2;
    int newtimex;
    int newtimey;
    int newtimex1;
    int newtimey1;
    int newtimex2;
    int newtimey2;
    float posx;
    float posy;
    int ngoodhit;
    double z[4];
    int _ch_shift;
    int DC_number;

    unsigned int channum;
    
    TRandom* ran;
    */

  private:
    int _separateHitsXY;

  };
  
//}

#endif
