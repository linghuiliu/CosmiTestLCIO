//Class which knows about the detector geometry and help pick-up one hit randomly.
//Interfaced with GEAR
//Created by A.-M. Magnan, 2006-06-26
// To do list : 2006-29-06 : need to replace wafer info in tempGEARvariables.hpp by the proper GEAR variables as soon as they exist. 

#include "RandomCellSelector.hpp"
#include "gear/CalorimeterParameters.h"
#include "gear/LayerLayout.h"
#include "gearxml/GearXML.h"
#include "CellIDDecoder.hpp"
#include "gearimpl/Util.h"
#include "math.h"
#include "tempGEARvariables.hpp"

#include "TRandom.h"

#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace gear;

namespace digisim
{//namespace digisim

  //Basic Constructor
  RandomCellSelector::RandomCellSelector() {}

  void RandomCellSelector::setInitSeed(int initSeed) {
    _random.SetSeed(initSeed); // 0 means the linux time.
    _seed = _random.GetSeed();
    std::cout << "---> Initialisation of the random seed : " << _random.GetSeed() << ". Has to be done only once !!!! Input : initSeed = " << initSeed << std::endl;
  }

  void RandomCellSelector::setSymmetryOrder(int symmetryOrder) {
    _symmetryOrder = symmetryOrder;
    std::cout << "---> Initialisation of the symmetryOrder : " ;
    if (symmetryOrder == 0 || symmetryOrder == 8) std::cout << "     BARREL CALORIMETER " << std::endl;
    else if (symmetryOrder == 2) std::cout << "     ENDCAP CALORIMETER " << std::endl;
    else if (symmetryOrder == 1) std::cout << "     ECAL PROTOTYPE " << std::endl;
    else if (symmetryOrder == 16) std::cout << "     MAPS " << std::endl;
    else {
      std::cerr << "Unknown Model !! Will stop the program." << std::endl;
      exit(0);
    }
  }

  void RandomCellSelector::setupGeometry(bool useXMLfile, std::string fileName){
    //setupGeometry
    tempGEARvariables waferinfo(_symmetryOrder);
    std::cerr << " !! WARNING : using temp wafer infos file include/tempGEARvariables.hpp, figures have to be check!! " << std::endl;
    _nlayers = waferinfo.nlayers;
    _maxS = waferinfo.nWafer0;//Stave, in x (r for cyl. geometry)
    _maxM = waferinfo.nWafer1;//Module, in z
    _maxI = waferinfo.nCellperWafer0;//cells in x
    _maxJ = waferinfo.nCellperWafer1;//cells in z
    _numTotCells = _nlayers*_maxS*_maxM*_maxI*_maxJ;
    _useEncoder32 = waferinfo.useEncoder32;
    if (useXMLfile){
      GearXML gearXML( fileName ) ;
      _gearMgr = gearXML.createGearMgr() ;

      if (_symmetryOrder == 8 || _symmetryOrder == 0){//barrel
	const CalorimeterParameters& p = _gearMgr->getEcalBarrelParameters();
	if (p.getLayoutType() != CalorimeterParameters::BARREL) {
	  std::cerr << " !!ERROR!! Problem when localizing hit : should be in Barrel but is : " << p.getLayoutType() << std::endl;
	  exit(0);
	}
	const LayerLayout& l = p.getLayerLayout() ;
	_nlayers = l.getNLayers();
      }//barrel

      else if (_symmetryOrder == 2){//endcap
	const CalorimeterParameters& p = _gearMgr->getEcalEndcapParameters();
	if (p.getLayoutType() != CalorimeterParameters::ENDCAP) {
	  std::cerr << " !!ERROR!! Problem when localizing hit : should be in Endcap but is : " << p.getLayoutType() << std::endl;
	  exit(0);
	}
	const LayerLayout& l = p.getLayerLayout() ;
	_nlayers = l.getNLayers();
      }//endcap
      //std::cout << " testgear - instantiated  GearMgr from file " << fileName
      //	      << std::endl ;
      //std::cout  << *_gearMgr  << std::endl ;
    }//if useXMLfile

  }//end setupGeometry


  long long RandomCellSelector::getRandomCell(){
    int K = static_cast<int>(_nlayers*_random.Rndm()) + 1;
    int S = static_cast<int>(_maxS*_random.Rndm()) + 1;
    int M = static_cast<int>(_maxM*_random.Rndm());
    int I = static_cast<int>(_maxI*_random.Rndm());
    int J = static_cast<int>(_maxJ*_random.Rndm());
    assert(K > 0 && K <= _nlayers);
    assert(S > 0 && S <= _maxS);
    assert(M >= 0 && M < _maxM);
    assert(I >= 0 && I < _maxI);
    assert(J >= 0 && J < _maxJ);

    CellIDDecoder encode(_symmetryOrder,_useEncoder32);
    return (static_cast<long long>(encode.Encode(K,S,M,I,J).second) << 32 ) + static_cast<long long>(encode.Encode(K,S,M,I,J).first);

  }

  long long RandomCellSelector::getRandomCell(int Layer){
    int K = Layer;
    int S = static_cast<int>(_maxS*_random.Rndm()) + 1;
    int M = static_cast<int>(_maxM*_random.Rndm());
    int I = static_cast<int>(_maxI*_random.Rndm());
    int J = static_cast<int>(_maxJ*_random.Rndm());
    assert(K > 0 && K <= _nlayers);
    assert(S > 0 && S <= _maxS);
    assert(M >= 0 && M < _maxM);
    assert(I >= 0 && I < _maxI);
    assert(J >= 0 && J < _maxJ);


    CellIDDecoder encode(_symmetryOrder,_useEncoder32);
    return (static_cast<long long>(encode.Encode(K,S,M,I,J).second) << 32 ) + static_cast<long long>(encode.Encode(K,S,M,I,J).first);

  }

  long long RandomCellSelector::getRandomCell(int Layer, int Stave, int Module){
    int K = Layer;
    int S = Stave;
    int M = Module;
    int I = static_cast<int>(_maxI*_random.Rndm());
    int J = static_cast<int>(_maxJ*_random.Rndm());
    assert(K > 0 && K <= _nlayers);
    assert(S > 0 && S <= _maxS);
    assert(M >= 0 && M < _maxM);
    assert(I >= 0 && I < _maxI);
    assert(J >= 0 && J < _maxJ);

    CellIDDecoder encode(_symmetryOrder,_useEncoder32);
    return (static_cast<long long>(encode.Encode(K,S,M,I,J).second) << 32 ) + static_cast<long long>(encode.Encode(K,S,M,I,J).first);

  }

  int RandomCellSelector::getRandomSeed(){
    return _seed;
  }

  int RandomCellSelector::getTotalNumberOfCells(){
    return _numTotCells;
  }

  bool RandomCellSelector::useEncoder32(){
    return _useEncoder32;
  }

  int RandomCellSelector::maxK(){
    return _nlayers;
  }

  int RandomCellSelector::maxS(){
    return _maxS;
  }

  int RandomCellSelector::maxM(){
    return _maxM;
  }

  int RandomCellSelector::maxI(){
    return _maxI;
  }

  int RandomCellSelector::maxJ(){
    return _maxJ;
  }

}//namespace digisim
