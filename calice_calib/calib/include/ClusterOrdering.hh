#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include "HcalCellIndex.hh"
#include "IMPL/ClusterImpl.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCIntVec.h"
#include "EVENT/Cluster.h"

using namespace std;
using namespace CALICE;
using namespace lcio;

class ClusterOrdering
{

  //
  // cada vez que no se genera un objeto y se quiere ocupar ::
  // en la funcion, entonces esta debe de ser estatica.
  // -> no se puede ocupar, pues no hay objeto! ;)
  //
  public:
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
    static bool compareHitByLayer(CalorimeterHit *aHit, CalorimeterHit *bHit)
  {      
    const int cellIDA = aHit->getCellID0();
    HcalCellIndex geomIndexA(cellIDA);
    const int K_HitA = geomIndexA.getLayerIndex();
    
    const int cellIDB = bHit->getCellID0();
    HcalCellIndex geomIndexB(cellIDB);
    const int K_HitB = geomIndexB.getLayerIndex();
    
    return(K_HitA < K_HitB);
  };
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  static bool compareVectBySize(Cluster *clusterA, Cluster *clusterB)
  { 
    const EVENT::CalorimeterHitVec hitVecA = clusterA->getCalorimeterHits();
    const EVENT::CalorimeterHitVec hitVecB = clusterB->getCalorimeterHits();
    return (hitVecA.size() > hitVecB.size());
  };
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  static bool checkXYAdyacentClusters(Cluster *clusterA,Cluster *clusterB, float numOfAdyacentCells)
  {
    if (clusterA == NULL || clusterB == NULL) 
      {
	cout<<"\n myClusterOrdering::checkXYAdyacentClusters: NULL pointers, exiting"<<endl;
	exit(1);
      }

    // get the calorimeterHits
    const EVENT::CalorimeterHitVec hitVecA = clusterA->getCalorimeterHits();
    const EVENT::CalorimeterHitVec hitVecB = clusterB->getCalorimeterHits();
    // each cluster have same x,y
    float Delta_x = fabs(hitVecA[0]->getPosition()[0] - hitVecB[0]->getPosition()[0]);
    float Delta_y = fabs(hitVecA[0]->getPosition()[1] - hitVecB[0]->getPosition()[1]);
    
    float distanceInXY=(numOfAdyacentCells*30)+1;
    
    bool aDecision = false;
    if (Delta_x < distanceInXY && Delta_y < distanceInXY)
      {
	aDecision=true;
      }      
    return aDecision;    
  };
  
//   /******************************************************************************/
//   /*                                                                            */
//   /*                                                                            */
//   /*                                                                            */
//   /******************************************************************************/
//   static EVENT::ClusterVec mergeTwoClusters(EVENT::Cluster *clusterA, EVENT::Cluster *clusterB)
//   {
//     // Get the calorimeter Hits of the initial cluster    
    
//     EVENT::ClusterVec mergeClusterVec;
    
//     mergeClusterVec.push_back(clusterA);
//     mergeClusterVec.push_back(clusterB);
    
//     return mergeClusterVec;
//   };
  
  
//   /******************************************************************************/
//   /*                                                                            */
//   /*                                                                            */
//   /*                                                                            */
//   /******************************************************************************/
//   static EVENT::ClusterVec removeIsolatedHits(EVENT::ClusterVec subCluster)
//   {
//     EVENT::ClusterVec goodClusters;
    
//     for (unsigned isubCl = 0; isubCl < subCluster.size(); isubCl++)
//       {
// 	EVENT::CalorimeterHitVec hitVecA = subCluster[isubCl]->getCalorimeterHits();
// 	// at least 2 hits in the subcluster
// 	if (hitVecA.size() > 1)
// 	  {
// 	    goodClusters.push_back(subCluster[isubCl]);
// 	  }
//       }
    
//     return goodClusters;
    
//   }
  
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
//  static EVENT::ClusterVec obtainSubClusters(EVENT::Cluster *clusterA)
//   {
//     // Get the calorimeter Hits of the initial cluster
//     EVENT::CalorimeterHitVec hitVecA = clusterA->getCalorimeterHits();
    
//     std::sort(hitVecA.begin(), hitVecA.end(), myClusterOrdering::compareHitByLayer);
//     const int numOfHitsInCluster = hitVecA.size();
 
//     // first elemement
//     const int cellIDA0 = hitVecA[0]->getCellID0();
//     HcalCellIndex geomIndexA(cellIDA0);
//     int iniVal = geomIndexA.getLayerIndex();
    
//     int iNext;
//     int iloop     = 1;
//     int numInSeq  = 0;
//     int numOfGaps = 0;
//     int ifill     = 0;
        
//     EVENT::ClusterVec allVecSubsample;
//     ClusterImpl *aVecSubsample = new ClusterImpl;
    
//     // Fill the first element!
    
//       aVecSubsample->addHit(hitVecA[0], 1.);
      
//       while(iloop < numOfHitsInCluster)
// 	{
// 	  const int cellIDA = hitVecA[iloop]->getCellID0();
// 	  HcalCellIndex geomIndex(cellIDA);
// 	  iNext = geomIndex.getLayerIndex();      
// 	  int deltaElement = iNext - iniVal;
	  
// 	  // allow gaps with maximum 2 elements missing
	  
// 	  if(deltaElement < 4)
// 	    {
// 	      aVecSubsample->addHit(hitVecA[iloop], 1.);	  
// 	    }
// 	  else
// 	    {
	      
// 	      numOfGaps++;
// 	      allVecSubsample.push_back(aVecSubsample);
// 	      aVecSubsample = new ClusterImpl;
// 	      aVecSubsample->addHit(hitVecA[iloop], 1.);
// 	    }
	  
// 	  iniVal=iNext;
// 	  iloop++;   
	  
// 	}
      
//       /*Fill the last track segment*/
//       allVecSubsample.push_back(aVecSubsample);
      
//       return allVecSubsample;
      
//   };
  
};
