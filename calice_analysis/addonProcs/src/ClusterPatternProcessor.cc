#include "ClusterPatternProcessor.hh"

// Marlin includes
#include "marlin/Exceptions.h"

// LCIO includes
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"

// CALICE includes
#include "ClusterShapesTyped.hh"

// MarlinUtil includes
#include "ClusterShapes.h"

// c++ includes
#include <algorithm>
#include <sstream>
#include <limits>

namespace CALICE {

  // create instance to make processor known to Marlin
  ClusterPatternProcessor aClusterPatternProcessor;

  ClusterPatternProcessor::ClusterPatternProcessor() : Processor("ClusterPatternProcessor") {

    _description = "Processor that analyzes the pattern of different clusters";

    registerProcessorParameter( "ExplicitColNames" ,
                                "explicit list of collections containing cluster hits",
                                _explicitColNames,
                                _explicitColNames ) ;

    registerProcessorParameter( "StringColNames" ,
                                "event parameter (StringVec) containing a list of collections to be used",
                                _stringColNames,
                                _stringColNames ) ;

    registerProcessorParameter( "MinHitsCluster" ,
                                "Minimum number of hits in a cluster to keep it for analysis",
                                _minHitsCluster,
                                (int)2 ) ;

    registerProcessorParameter( "MinEnergyCluster" ,
                                "Minimum amount of energy in a cluster to keep it for analysis",
                                _minEnergyCluster,
                                (float)1 ) ;

  }

  void ClusterPatternProcessor::init() {


    bool error = false;

    if (error) throw StopProcessingException(this);

  }

  void ClusterPatternProcessor::processRunHeader(LCRunHeader *run) {

  }

  void ClusterPatternProcessor::processEvent( LCEvent *evt ) {

    int nCluster = 0;

    float maxDistance_r = 0;
    float maxDistance_z = 0;

    float maxOverlap_z = 0;

    float max_E_E = 0;
    int max_E_hits = 0;

    float max_hits_E = 0;
    int max_hits_hits = 0;

    float max_density = 0;


    /* collect clusters: loop over all input col candidates
     */
    std::vector<ClusterShapesTyped*> allCluster;

    StringVec v_colNames;
    evt->getParameters().getStringVals( _stringColNames , v_colNames );

    streamlog_out(DEBUG)<<"\n Start loop over "<<v_colNames.size()<<" collections named "<<_stringColNames<<std::endl;

    for ( unsigned i = 0; i < v_colNames.size(); i++ ){

      try {

        LCCollection* incol_i = evt->getCollection( v_colNames[i] );

        ClusterShapesTyped* shape_i = new ClusterShapesTyped;
        shape_i->fill<CalorimeterHit>(incol_i);

        int nHits_i = incol_i->getNumberOfElements();
        float energy_i = shape_i->getClusterShapesPointer()->getTotalAmplitude();

	streamlog_out(DEBUG)<<" shape "<<i+1<<", energy="<<energy_i
		    <<", smallest_length="<<shape_i->getClusterShapesPointer()->getElipsoid_r3()
		    <<std::endl;

	if ( nHits_i > max_hits_hits ) {
	  max_hits_E = energy_i;
	  max_hits_hits = nHits_i;
	}

	if ( energy_i > max_E_E ) {
	  max_E_E = energy_i;
	  max_E_hits = nHits_i;
	}

	if ( energy_i / (float)nHits_i > max_density )
	  max_density = energy_i / (float)nHits_i;

        if ( nHits_i >= _minHitsCluster && energy_i > _minEnergyCluster )
          allCluster.push_back(shape_i);

     }
      catch ( DataNotAvailableException &err) {
        streamlog_out(WARNING) << "collection " << v_colNames[i] << " not available" << std::endl;
      }

    }

    /* calculate pattern parameters */
    nCluster = allCluster.size();

    // loop: 1st cluster
    for ( unsigned a = 0; a <  allCluster.size(); a++ ) {

      float* pos_cog_a = (allCluster[a])->getClusterShapesPointer()->getCentreOfGravity();

      // loop: 2nd cluster
      for ( unsigned b = a+1; b <  allCluster.size(); b++ ) {

	float* pos_cog_b = (allCluster[b])->getClusterShapesPointer()->getCentreOfGravity();

	// check distance in r (x-y plane)
	float dr_a_b = sqrt( ( ( pos_cog_a[0] - pos_cog_b[0] ) * ( pos_cog_a[0] - pos_cog_b[0] ) ) + \
			     ( ( pos_cog_a[1] - pos_cog_b[1] ) * ( pos_cog_a[1] - pos_cog_b[1] ) ) );

	if ( dr_a_b > maxDistance_r )
	  maxDistance_r = dr_a_b;

	// check distance in z
	float dz_a_b = sqrt( ( pos_cog_a[2] - pos_cog_b[2] ) * ( pos_cog_a[2] - pos_cog_b[2] ) );

	if ( dz_a_b > maxDistance_z )
	  maxDistance_z = dz_a_b;

	// check overlap in z
	float overlap_z = 0;

	if ( overlap_z > maxOverlap_z )
	  maxOverlap_z = overlap_z;

      }

    }

    /* append output parameters to event */

    //evt->parameters().setValues(name()+"_candidates",collectionNames);

    evt->parameters().setValue(name()+"_nCluster",nCluster);
    evt->parameters().setValue(name()+"_maxDistance_r",maxDistance_r);
    evt->parameters().setValue(name()+"_maxDistance_z",maxDistance_z);
    evt->parameters().setValue(name()+"_maxOverlap_z",maxOverlap_z);
    evt->parameters().setValue(name()+"_maxE_E",max_E_E);
    evt->parameters().setValue(name()+"_maxE_Hits",max_E_hits);
    evt->parameters().setValue(name()+"_maxHits_E",max_hits_E);
    evt->parameters().setValue(name()+"_maxHits_Hits",max_hits_hits);
    evt->parameters().setValue(name()+"_maxDensity",max_density);

    /* debug output */
    streamlog_out(DEBUG) << "number of clusters in event: " << nCluster << std::endl;
    streamlog_out(DEBUG) << "maximum distance r: " << maxDistance_r << std::endl;
    streamlog_out(DEBUG) << "maximum distance z: " << maxDistance_z << std::endl;
    streamlog_out(DEBUG) << "maximum overlap  z: " << maxOverlap_z << std::endl;

    for ( unsigned i = 0; i <  allCluster.size(); i++ ) {
      streamlog_out(DEBUG) << "\nCluster " << i+1 << ":" << std::endl;
      streamlog_out(DEBUG) << "energy = " << (allCluster[i])->getClusterShapesPointer()->getTotalAmplitude() << std::endl;
      streamlog_out(DEBUG) << "hits = " << (allCluster[i])->getClusterShapesPointer()->getNumberOfHits() << std::endl;
      streamlog_out(DEBUG) << "cog_x = " << ((allCluster[i])->getClusterShapesPointer()->getCentreOfGravity())[0] << std::endl;
      streamlog_out(DEBUG) << "cog_y = " << ((allCluster[i])->getClusterShapesPointer()->getCentreOfGravity())[1] << std::endl;
      streamlog_out(DEBUG) << "cog_z = " << ((allCluster[i])->getClusterShapesPointer()->getCentreOfGravity())[2] << std::endl;
    }

    // cleanup to avoid memory leak
    for ( unsigned a = 0; a <  allCluster.size(); a++ )
      delete allCluster[a];

    allCluster.clear();

  }

  void ClusterPatternProcessor::end() {

  }

} // end namespace CALICE

