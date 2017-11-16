#ifndef _MipSelect_hh_
#define _MipSelect_hh_

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include <RtypesSubSet.h>
#include <string>
#include <iostream>
#include <marlin/Processor.h>
#include <vector>

#ifdef WITH_CONTROL_HISTOGRAMS
#include <ConditionsChangeDelegator.hh>
#include <histmgr/Key_t.hh>
#endif

// FIXME: Should go into another namespace (?)
namespace CALICE {

  /** Simple class to try to select hits resulting from one mip.
   * This processor tries to find hits in the chosen hit collection
   * which are close together. If the number adjacent hits is large
   * enough a cluster is created.
   * @todo: add shower shape criterium, maximum number of tolerated hits 
   */
  class MipSelect : public marlin::Processor
  {
  public:
    Processor*  newProcessor() { return new MipSelect ; }

    MipSelect();
    
    ~MipSelect() {};
    

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent 
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse. 
     */
    void processEvent( LCEvent * evtP );

    void end();

  protected:
    IntVec   _nHits;                  /**< minimum and maximum required number of hits for a cluster (Marlin processor parameter).*/
    Float_t _maxDist;                 /**< the maximum distance between hits considered to be close (Marlin processor parameter).*/
    FloatVec _clusterEnergy;          /**< minimum and maximum required cluster energy (Marlin processor parameter).*/
    Float_t _maxChi2;                 /**< minimum required chi2 of the x and y straight line fits (Marlin processor parameter).*/
    Float_t _w0;                      /**< Cut parameter which defines the energy threshold of hits considered in the 
					   centre-of-gravity determination (Marlin processor parameter).*/
    
    FloatVec _posError;               /**< Error assigned to the cluster barycentre (marlin processor parameter).*/
    //  Int_t _nNeighboursMax;
    //  Float_t _neighbourDist;

    std::string _colName;              /**< name of the hit input collection (Marlin processor parameter)*/
    std::string _clusterColName;       /**< name of the cluster output collection (Marlin processor parameter)*/

  private:
    std::vector<UInt_t> _isolatedHits; /**< temporary buffer*/
#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::Key_t       _histGroupKey;/**< Key for the histogram group.*/
    histmgr::Key_t       _histHitPosKey;/**< Key for the a 2D histogram of all the hits.*/

    std::string _colNameModuleLocation;/**< name of the conditions data collection which contains the module location
					    (Marlin processor parameter).*/
    FloatVec _signalHistPar;           /**< binning of the cluster signal histogram (Marlin processor parameter).*/
    FloatVec _posHistPar;              /**< binning of the cluster barycentre histogram (Marlin processor parameter).*/
    FloatVec _indexHistPar;

    // 1d collections of 1D histograms
    enum EH1Type {kH1NHits,kH1ClusterSignal,kH1SignalVsLayer, kH1HitX, kH1HitY, kH1Mean, kH1RMS, kH1Angle, kH1Chi2, kH1ResidualX, kH1ResidualY, kNH1};
    LCCollection *_histCol[kNH1];

    ConditionsChangeDelegator<MipSelect> _moduleLocationChange;   /**< helper class to listen for changes of the
										 location of modules  (conditions data).*/
    void moduleLocationChanged(lcio::LCCollection* col);                      /**< helper function to resizes histograms arrays accordingly.*/
    UInt_t _lastLayer;

#endif
  };


}
#endif
