#ifndef _Clusteriser_hh_
#define _Clusteriser_hh_

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include <RtypesSubSet.h>
#include <string>
#include <iostream>
#include <marlin/Processor.h>
#include <vector>

#ifdef WITH_CONTROL_HISTOGRAMS
#  include "histmgr/Key_t.hh"
#endif

// FIXME: Should go into another namespace (?)
namespace CALICE {

  /** Simple class to try to select hits resulting from one mip.
   * This processor tries to find hits in the chosen hit collection
   * which are close together. If the number adjacent hits is large
   * enough a cluster is created.
   * @todo: add shower shape criterium, maximum number of tolerated hits 
   */
  class Clusteriser : public marlin::Processor
  {
  public:
    Processor*  newProcessor() { return new Clusteriser ; }

    Clusteriser();
    
    ~Clusteriser() {};
    

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
    Int_t   _nHitsMin;                /**< minimum required number of hits for a cluster (marlin processor parameter).*/
    Float_t _maxDist;                 /**< the maximum distance between hits considered to be close (marlin processor parameter).*/
    Float_t _minEnergy;               /**< the minimum energy required for a cluster to be accepted.*/
    Float_t _w0;                      /**< Cut parameter which defines the energy threshold of hits considered in the 
					   centre-of-gravity determination.*/
    
    EVENT::FloatVec _posError;        /**< the error on the cluster position in x, y and z. */

    //  Int_t _nNeighboursMax;
    //  Float_t _neighbourDist;

    std::string _colName;              /**< name of the hit input collection (Marlin processor parameter)*/
    std::string _clusterColName;       /**< name of the cluster output collection (Marlin processor parameter)*/

  private:
    std::vector<UInt_t> _isolatedHits; /**< temporary buffer*/

#ifdef WITH_CONTROL_HISTOGRAMS
    std::string _histogramGroupName;   /**< name of the histogram group (Marlin processor parameter).*/
    histmgr::Key_t _histGroupKey;      /**< Key for the histogram group.*/
    std::string _colNameModuleLocation;/**< name of the conditions data collection which contains the module location
					    (Marlin processor parameter).*/
    FloatVec _signalHistPar;           /**< binning of the cluster signal histogram (Marlin processor parameter).*/
    FloatVec _posHistPar;              /**< binning of the cluster barycentre histogram (Marlin processor parameter).*/
    

    // 1d collections of 1D histograms
    enum EH1Type {kH1ClusterSignal,kH1NHits,kH1Mean, kH1RMS, kNH1};
    LCCollection *_histCol[kNH1];

#endif

  };


}
#endif
