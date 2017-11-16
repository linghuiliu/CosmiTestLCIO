#ifndef AHCAL_GANGING_PROCESSOR_HH
#define AHCAL_GANGING_PROCESSOR_HH 1

#include "marlin/Processor.h"
#include "lcio.h"

#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <iomanip>

#include <vector>

#include "EVENT/SimCalorimeterHit.h"

namespace CALICE {

  namespace AHCAL {

    namespace Digitization {

      namespace Ganging {

/** Helper class to store I, J, K.
 */ 
struct GeometricalIndices {

  int I;
  int J;
  int K;

};

/** Needed to fill the GeometricalIndices struct in a map.
 */ 
bool operator<(const GeometricalIndices& lhs,
               const GeometricalIndices& rhs) {

  // let the stl decide what is smaller
  return std::make_pair(lhs.I, std::make_pair(lhs.J,lhs.K) ) <
    std::make_pair(rhs.I, std::make_pair(rhs.J,rhs.K) );
}

//bool operator==(const GeometricalIndices& lhs,
//               const GeometricalIndices& rhs) {
//
//  return (lhs.I == rhs.I) && (lhs.J == rhs.J) && (lhs.K == rhs.K);
//
//}

/** Convenient output operator.
 */
std::ostream& operator<<(std::ostream& out, const GeometricalIndices& ijk) {

  out << "I: " << ijk.I << " J: " << ijk.J << " K: " << ijk.K ;

  return out;
    
}

//typedef std::map<GeometricalIndices,float> GangMap;

/** A ContributionMap maps a ganged index to all its contributing 
 * SimCalorimeterHits.
 */
//typedef std::map<GeometricalIndices,std::vector<SimCalorimeterHit*> > ContributionMap;

typedef std::map<GeometricalIndices, float > ContributionMap;

typedef enum moduleType {unknown, fine, coarse};

class Ganger;

/** The processor performing the ganging.
 *
 */
class ahcalGangingProcessor : public marlin::Processor {

public:

  // default c'tor
  ahcalGangingProcessor();
  
  marlin::Processor* newProcessor() { return new ahcalGangingProcessor; }

  /** Initiliases the ganging chain of responsibilty for fine and
   *  coarse modules. Defines location of module types.
   */
  void init();

  /** Called every event.
   */
  void processEvent(lcio::LCEvent* evt);

  /** Helper function to add up all SimCalorimeterHit energies.
   */
  float energySum(std::vector<SimCalorimeterHit*> vecSimHits);

  /** Creates the ganged collection.
   */
  void createOutputCollections(lcio::LCEvent* evt);

  /** Resets everything which is event specific.
   */
  void tidyUp();

  /**
   */
  void end();

private:
  
  /** The name of the incoming collection.
   */
  std::string _inputCollectionName;

  /** The name of the outgoing collection.
   */
  std::string _outputCollectionName;
  
  /**
   */
  float _tileBorderAttenuationFactor;

  /** Instance which holds the gather SimCalorimeterHits. The outcome
   *  of the ganging is here.
   */
  ContributionMap _contributionMap;

  /** All gangers needed for a fine module. Filled in init().
   */
  std::vector<Ganger*> _gangersForFineModules;

  /** All gangers needed for a coarse module. Filled in init().
   */
  std::vector<Ganger*> _gangersForCoarseModules;
  
  /** Layer to module type map. Filled in init().
   */
  std::map<int,moduleType> _moduleTypeOrder;


};

/** The work horse. A ganger holds all information for a region
 * to perform the ganging.
 */
class Ganger {

public:

  /** The Ganger is constructed with all information needed for a
   *  region to perform the ganging.
   */
  Ganger(int cs, int off_I, int off_J, ContributionMap* cm, float factor) :
      
    _cellSize(cs), 
    _offset_I(off_I), 
    _offset_J(off_J),
    _CM(cm),
    _tileBorderAttenuationFactor(factor)

  {} // end of c'tor
    
  virtual ~Ganger() {};

  /** Implementations have to implement where in I/J they are responsible.
   */
  virtual bool responsible(GeometricalIndices indices) = 0;
    
  /** Function which return true if the ganger is responsible for this
   * region and adds the hit to the ContributionMap.
   */
  bool addHit(GeometricalIndices indices, SimCalorimeterHit* hit) {
      
    if(responsible(indices) == true) {
        
      //      _GP->_gangMap[coarsen(indices)] += energy;

      float energy = hit->getEnergy();

      const float factor = this->_tileBorderAttenuationFactor;

      GeometricalIndices gangedIJ = coarsen(indices);

      if(indices.I == gangedIJ.I || 
         indices.I == (gangedIJ.I + (_cellSize-1))
         ) {

        energy *= factor;

      } 

      if(indices.J == gangedIJ.J ||
         indices.J == gangedIJ.J + (_cellSize-1)) {
        
        energy *= factor;

      }

      (*_CM)[gangedIJ] += energy;

//      std::cout << "ME: " 
//                << std::setw(7) << hit->getPosition()[0] << " " 
//                << std::setw(7) << hit->getPosition()[1] << " "
//                << std::setw(3) << indices.I << " " 
//                << std::setw(3) << indices.J << " " 
//                << std::setw(3) << indices.K << " "
//                << std::setprecision(5) << std::setw(5) 
//                << energy/hit->getEnergy()   << " " 
//                << std::setw(5) << 1000*hit->getEnergy() << '\n';

      return true;
        
    } else {
        
      return false;
        
    }

  }

  /** Function which returns the ganged index according to the
   *  constants defined at construction.
   */
  GeometricalIndices coarsen(GeometricalIndices ijk) {

    GeometricalIndices coarsenedIndices = {

      coarsenSingleIndex(ijk.I, _offset_I),
      coarsenSingleIndex(ijk.J, _offset_J),
      // nothing to do for K
      ijk.K

    };

    return coarsenedIndices;

  };

  /** Performs mapping from Mokka indices to ganged indices. The main
      "algorithm".
   */
  int coarsenSingleIndex(int index, int offset) {

    int coarsened_index = static_cast<int>(
                                           floor(static_cast<float>(index - offset) 
                                                 / static_cast<float>(_cellSize))
                                           ) * _cellSize + offset;

    return coarsened_index;

  }

private:

  /** Quadratic side length of the cells in the region the Ganger is responsible.
   */
  int _cellSize;

  /** Offset with respect to starting counting at 1 for I.
   */
  int _offset_I;

  /** Offset with respect to starting counting at 1 for J.
   */
  int _offset_J;

  /** Pointer to map which stores all ganged indices and SimCalorimeterHits
   */
  ContributionMap* _CM;

  //GangMap& _gangMap;
  //  ContributionMap& _contributionMap;

  float _tileBorderAttenuationFactor;

};

      }
    }
  }
}


#endif
