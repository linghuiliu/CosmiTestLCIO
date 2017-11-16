#ifndef DeepAnalysisProcessor_h 
#define DeepAnalysisProcessor_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif
 
#include "lcio.h"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCFloatVec.h>
#include <IMPL/CalorimeterHitImpl.h>//contains lcio:CalorimeterHit
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"

#include "DeepAnalysis.hh"
#include "MappingAndAlignment.hh"
#include "ConditionsChangeDelegator.hh"
#include "MultipleConditionsChangeDelegator.hh"
//#include <VRawADCValueProcessor.hh>

namespace CALICE { 

  /** Enumeration to set the different types of clusters. 
      * To be used after clustering a la Vasiliy Morgunov.
      */

  typedef enum {CLUSTER_EM=10, CLUSTER_TRK, CLUSTER_HAD, CLUSTER_NEUTR} ClusterTypes;

/** @class DeepAnalysisProcessor DeepAnalysisProcessor.hh 

    Processor for the deep analysis according to 
    the algorithm developed by Vasiliy Morgunov. Input: calibrated CalorimeterHits, 
    output: AhcClusters - collection of classified clusters, according to their energy.
    For details, see http://www.desy.de/~garutti/DOCU/deepanalysis.pdf
    
    @author S. Schmidt, DESY
    @date Apr 4 2007 
*/
  class DeepAnalysisProcessor : public marlin::Processor {
    
    public: 
    
      /** Return a new instance of the processor. */
      virtual Processor* newProcessor() { return new DeepAnalysisProcessor; }
    
      /** Default constructor. */
      DeepAnalysisProcessor();
    
      /** Called at the begin of the job before anything is read. 
	  Used to initialize the processor.
	  */
      virtual void init();
    
      /** Called for every run. */
      virtual void processRunHeader(LCRunHeader* run);
      
      /** Classification of hits into clusters, according to the deep analysis algorithm, on an event by event basis.
	  */
      virtual void processEvent(LCEvent* evt);
    
      /** @return @ref _gevPerMip 
	  @param detectorIndex index of the input collections
	  @param layerIndex index of the layer
	  */
      virtual double GeVperMip(unsigned detectorIndex, unsigned layerIndex);

      /** @return @ref _populatedLayerIndex
	  @param detectorIndex index of the input collections
	  @param layerIndex index of the layer
	  */
      virtual unsigned PopulatedLayerIndex(unsigned detectorIndex, unsigned layerIndex);

      /** Called for every event - right after processEvent() has been called for all processors. 
	  Use to check processing and/or produce check plots.
	  */
      virtual void check(LCEvent* evt);

      /** The coordinate system used in DeepAnalysis is different from
	  the one used in Calice. The relations are:
	  x_deepAnalysis = -x_calice
	  y_deepAnalysis = y_calice
	  z_deepAnalysis = -z_calice   	  

	  double x_calice = a_hit->getPosition()[0];
	  double y_calice = a_hit->getPosition()[1];
	  double z_calice = a_hit->getPosition()[2];
	  
	  double x_deepAnalysis = -x_calice;
	  double y_deepAnalysis = y_calice;
	  double z_deepAnalysis = -z_calice; 
	  */
      std::vector<float> GetRotatedPositionIfRquested(bool switchRotation, CalorimeterHit *a_hit);
    
      /** Called after data processing for clean up in the inverse order of the init() 
	  method so that resources allocated in the first processor also will be available 
	  for all following processors. */
      virtual void end();

    protected:
      lcio::FloatVec _thresholds;/**<Thresholds to be put into deep analysis algorithm.*/
      lcio::StringVec _inputCollectionNames; /**<List of input hit collection names 
						 (default: CalorimeterHits).*/
      std::string _outputCollectionName;  /**<Name of output collection (default: AhcClusters)*/  
      std::string _outputNeutrCollectionName;  /**<Name of neutral output collection (default: AhcNeutrClusters)*/  
      lcio::StringVec _colNamesModuleDescription;/**<Name of the conditions data collection 
						     containing module descriptions*/
      lcio::StringVec _colNamesModuleLocation; /**<Name of the conditions data collection 
						   containing module location.*/
      lcio::StringVec _colNamesModuleConnection;/**<Name of the conditions data collection 
						    containing module connection.*/

      std::vector<MultipleConditionsChangeDelegator<DeepAnalysisProcessor> > _moduleTypeChange;/**<Helper to listen
												   for module type 
												   change (conditions data).*/
      std::vector<MultipleConditionsChangeDelegator<DeepAnalysisProcessor> > _moduleLocationChange; /**<Helper to listen
													for module location
													change (conditions data).*/ 
      std::vector<MultipleConditionsChangeDelegator<DeepAnalysisProcessor> > _moduleConnectionChange; /**<Helper to listen
													  for module connection 
													  change (conditions data).*/

      std::vector<MappingAndAlignment> _mapping;/**<Vector of MappingAndAlignment (which handles 
						    the translation into unique layer/cell ids
						    and 3D space coordinates).*/
      int _viewConnectionTree;/**<View the mapping between channels and modules whenever 
				  the module location or module connection conditions 
				  data change (set to 0 or !=0).*/
      std::vector<std::vector<float> > _gevPerMip;/**<Store the conversion factors from E[MIPs] 
						      to E[GeV] (depend on the sampling fractions and on the index
						      of the occupied layer). */
      std::vector<std::vector<int> > _populatedLayerIndex;/**<Store the index of layers containing hits.*/
    
      lcio::FloatVec _samplingFractionA;/**<First part of parametrisation of the sampling fraction.*/
      lcio::FloatVec _samplingFractionB;/**<Second part of parametrisation of the sampling fraction.*/
      double _samplingFactor;/**<Sampling factor (in GeV) for transforming E[GeV] in E[MIPs], 
				 default MC value 875 keV.*/

      int _switchRotation; /**<Switch rotation ON/OFF*/
      double _mipThreshold;/**<Minimal energy deposition in units of MIPs to keep 
			       the hit (default 0.5)*/

    bool _fIsMC;
    
      /** Check out which layers contain hits, and fill the @ref _gevPerMip vectors. 
	  */
      virtual void updateCorrelations();

      /** Listen to changes of the module type (needed to correctly map the DAQ signals 
	  to cells and to calculate the 3D spacial coordinates for hits).

	  @param col input collections names
	  @param detectorIndex index of the input collections
	  */
      virtual void moduleTypeChanged(lcio::LCCollection* col, unsigned int detectorIndex) {
	_mapping[detectorIndex].moduleTypeChanged(col);
	updateCorrelations();
      };

      /** Listen to changes of the module location (needed to correctly map the DAQ signals 
	  to cells and to calculate the 3D spacial coordinates for hits).

	  @param col input collections names
	  @param detectorIndex index of the input collections
	  */
      virtual void moduleLocationChanged(lcio::LCCollection* col, unsigned int detectorIndex) {
	_mapping[detectorIndex].moduleLocationChanged(col);
	updateCorrelations();
      };

 
      /** Listen to changes of the module connection (needed to correctly map the DAQ signals 
	  to cells and to calculate the 3D spacial coordinates for hits).
	  
	  @param col input collections names
	  @param detectorIndex index of the input collections
	  */
      virtual void moduleConnectionChanged(lcio::LCCollection* col, unsigned int detectorIndex) {
	_mapping[detectorIndex].moduleConnectionChanged(col);
	updateCorrelations();
      };

  };

}

#endif
