#ifndef _RANDOMCELLSELECTOR_HPP_
#define _RANDOMCELLSELECTOR_HPP_

#include "gear/GearMgr.h"
#include "CellIDDecoder.hpp"
#include <iostream>
#include "TRandom.h"



namespace digisim
{//namespace digisim


  class RandomCellSelector
  {//class RandomCellSelector

  public:

    //Basic Constructor

    RandomCellSelector();

    //if using initSeed = 0 (linux time) be aware that it's updated only every second.
    void setInitSeed(int initSeed);

 /** SymmetryOrder is the order of the rotational symmetry <br>
     *   8 for an octagonal barrel calorimeter<br>
     *   2 for an endcap calorimeter<br>
     *   1 for a standalone prototype<br>
     *   0 for an idealized cylindrical calorimeter.
     */
    void setSymmetryOrder(int symmetryOrder);

    //fileName is the GEAR xml input file.
    void setupGeometry(bool useXMLfile, std::string fileName="");

    //will return the two cellIDs of a randomly chosen cell.
    long long getRandomCell();
    long long getRandomCell(int Layer);
    long long getRandomCell(int Layer, int Stave, int Module);

    //will return the first seed use to calculate the random cellIDs.
    int getRandomSeed();
    int getTotalNumberOfCells();
    bool useEncoder32();
    int maxK();
    int maxM();
    int maxS();
    int maxI();
    int maxJ();
    // destructor
    ~RandomCellSelector(){};

    //  -----Data Members-----

  private:

    gear::GearMgr* _gearMgr;
    CellIDDecoder _cellID;
    int _symmetryOrder;
    int _maxM;
    int _maxS;
    int _maxI;
    int _maxJ;
    int _nlayers;
    bool _useEncoder32;
    int _Layer;
    int _Module;
    int _Stave;
    TRandom _random;
    int _seed;
    int _numTotCells;
  };//class RandomCellSelector
}//namespace digisim


#endif
