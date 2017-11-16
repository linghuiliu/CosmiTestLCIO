#ifndef AngleTrackFinder_h
#define AngleTrackFinder_h 1

/*LCIO*/
#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/ClusterImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCIntVec.h"
#include "EVENT/Cluster.h"
#include "marlin/Processor.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"

/*CALICE*/
#include "Mapper.hh"
#include "CellDescription.hh"
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"

using std::cout;
using std::endl;
using namespace lcio;
using namespace marlin;

namespace CALICE {
  
  /** @brief Processor to extract MIP calibrations from muon beam runs
   * @author A.Vargas DESY
   * @date November 2010
   */
  
  class AngleTrackFinder: public marlin::Processor 
  {
    
  public:
    /**Return new instance of this processor
     */
    virtual Processor* newProcessor() 
    {
      return new AngleTrackFinder;
    }

    /**Default constructor
     */

    AngleTrackFinder();
    
    /**Destructor
     */
    ~AngleTrackFinder();
    
    /**Initialise useful variables
     */
    virtual void init();
    /**Process event (the working horse)
       @param evt event to be processed
    */
    virtual void processEvent(LCEvent * evt);
    /**Function to be called at the end of the job, after all events have been processed, for clean up
     */
    virtual void end() {};
    
  private:
    /**Open the input collections
       @param evt event to be processed
    */
    void openInputCollections(LCEvent *evt);
    
    /**Find the track
       @param evt event to be processed
    */
    void findTrack(LCEvent *evt);
    
    /**Check if cuts are fullfilled, and if analysis is to be continued
       @return analyse flag to show if the cuts are fullfilled or not
    */
    bool doFurtherAnalysis();
    
    /**Extract AHCAL information
     */
    std::vector<CalorimeterHit*> extractAHCALInformation();
    
    /**Extract number of hits and the energy sum from the input collection
       @param inputCol the input collection
    */
    
    std::pair<int, float> extractNumberOfHitsAndEnergySum(LCCollection *inputCol);
    
    /**Find clusters of MIP tracks
       @param hcalVec input vector of AHCAL hits
       @param outputVec vector of clusters (to be filled)
       @return vector of clusters
    */
    void findTrackClusters(std::vector<CalorimeterHit*> &hcalVec, EVENT::ClusterVec &outputVec);
    void findTrackClustersCoarse(std::vector<CalorimeterHit*> &hcalVec, EVENT::ClusterVec &outputVec);
    EVENT::ClusterVec extendTracksToTcmt(LCTypedVector<CalorimeterHit> tcmtVec,
					 EVENT::ClusterVec trackClusters);
    /**Select good tracks
       @param clusterVec vector of track clusters, on which the selection is performed
       @param trackSelectionMode track selection mode, set by the steering parameter TrackSelectionMode
       @param goodTrackClusters vectors of good track clusters (to be filled)
    */
    void selectTrackClusters(EVENT::ClusterVec clusterVec,
			     const std::string trackSelectionMode,
			     EVENT::ClusterVec &goodTrackClusters);
 
   void mergeCoarseClusters(EVENT::ClusterVec goodclusterVec,
			     EVENT::ClusterVec clusterCoarseVec,
			     EVENT::ClusterVec &outputVec,
			     const std::string trackSelectionMode);
    
    /**Get the whole track (i.e. track with no missing hits)
       @param trackCluster input track cluster, from which the mean X and Y of the track are calculated
       @return wholeTrack the track cluster which contains all the hits
    */
    ClusterImpl* getWholeTrack(Cluster *trackCluster);
    ClusterImpl* getWholeTrackSegment(Cluster *trackCluster);
    ClusterImpl* copyACluster(Cluster *trackCluster);
    /**Get the vector of track clusters containing all the remaining hits
       @param selectedTrackClusters vector of selected track clusters
       @return vector of track clusters containing all the remaining hits
    */
    EVENT::ClusterVec getRemainingHits(EVENT::ClusterVec selectedTrackClusters);
    EVENT::ClusterVec getRemainingHitsOfTrackSegment(
						     EVENT::ClusterVec selectedTrackClusters);
    EVENT::ClusterVec getFinalCluster(EVENT::ClusterVec selectedTrackClusters);
    /**Create output collection of track clusters
       @param evt the processed event
       @param finalVec final vector of track clusters, after all selections
    */
    void createOutputCollection(LCEvent *evt, EVENT::ClusterVec finalVec);
    

    EVENT::ClusterVec obtainSubClusters(EVENT::Cluster *clusterA);
    EVENT::ClusterVec removeIsolatedHits(EVENT::ClusterVec subCluster);
    bool checkXYAdjacentClusters(Cluster *clusterA,Cluster *clusterB, int numOfAdjacentCells);




  private:
    static const unsigned int MAXTCMTLAYERS = 16;
    static const unsigned int MAXTCMTSTRIPS = 20;

    const Mapper* _mapper; /**<the mapper*/
    std::string _mappingProcessorName; /**<name of the processor which provides the mapping*/
    std::string _cellDescriptionProcessorName;/**<name of the processor which provides the cells description*/
    
    MappedContainer<CALICE::CellDescription> *_cellDescriptions;


    LCCollection *_ecalInputCol;/**<ECAL input collection*/
    LCCollection *_ahcalInputCol;/**<AHCAL input collection*/
    LCCollection *_ahcalAmpInputCol;/**<AHCAL Amplitude input collection*/
    LCCollection *_tcmtInputCol;/**<TCMT input collection*/
    
    std::string _ecalInputColName;
    std::string _ahcalInputColName;
    std::string _ahcalAmpInputColName;
    std::string _ahcalOutputTrackColName;
    std::string _tcmtOutputTrackColName;
    std::string _tcmtInputColName;
    std::string _encodingString;
    std::string _trackSelectionMode; /**<track selection mode: NHITs = based on the number of hits;
					PERPENDICULAR = select only perpendicular tracks*/
    
    LCRelationNavigator *_navigator;
    EVENT::ClusterVec _goodTrackClusters;
    
    bool _useECAL;
    bool _useTCMT;
    bool _tcmtStartVertical;

    float _tcmtStripWidth;
    
    bool _isNonMuonRun; /**<flag for non-muon run*/
    int _ahcalMaxNoHits; /**<maximum number of hits in the AHCAL*/
    int _ecalMinNoHits; /**<minimum number of hits in the ECAL*/
    int _ecalMaxNoHits; /**<maximum number of hits in the ECAL*/
    float _tcmtMinEnergySum; /**<minimum energy sum in the TCMT*/
    float _mipCutValue; /**<MIP cut value (0.5 MIPs)*/
    int _mipCutVariable; /** choose amplitude to be used to perform the mip cut */
    int _trackNoHitsThreshold; /**<minimum number of hits for which a track is considered to be
				  a muon track (used only with TrackSelectionMode=NHITS)*/
    int _useCoarseSection;
    int _minActiveTCMTParallelStrips;

    IntVec _badAhcalModulesVec; /**<vector of bad AHCAL modules numbers*/
    IntVec _firstAhcalModulesVec;/**<vector of numbers of the beginning AHCAL modules which are
				    used for perpendicular track finding*/
    IntVec _lastAhcalModulesVec; /**<vector of numbers of the ending AHCAL modules which are
				    used for perpendicular track finding*/
    EVENT::CalorimeterHitVec _allAhcalHitsInEventVec; /**<vector containing all AHCAL hits from the event*/

  };/*end of class*/
}/*end of namespace CALICE*/

#endif
