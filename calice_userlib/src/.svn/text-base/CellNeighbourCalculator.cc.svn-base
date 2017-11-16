#include "CellNeighbourCalculator.hh"

#include "CellIterator.hh"
#include "CaliceException.hh"

#include "IMPL/LCCollectionVec.h"

namespace CALICE {

  CellNeighbourCalculator::CellNeighbourCalculator(const Mapper* mapper) : _mapper(mapper) {
  }


  void CellNeighbourCalculator::checkForCell(const unsigned int i, const unsigned int j, const unsigned int k, std::set<int>& cellIDset) const {
    int cellID = -1;
    try {
      cellID = _mapper->getTrueCellID( _mapper->getDecoder()->getCellID(i,j,k) ) ;
    }
    catch ( BadDataException& ) {
    }
    if (cellID != -1) cellIDset.insert(cellID);
  }

  CellNeighbours* CellNeighbourCalculator::getNeighbours(const int cellID) const {

    unsigned int I = _mapper->getDecoder()->getIFromCellID(cellID);
    unsigned int J = _mapper->getDecoder()->getJFromCellID(cellID);
    unsigned int K = _mapper->getDecoder()->getKFromCellID(cellID);
    unsigned int Isize = _mapper->getISizeFromCellID(cellID);
    unsigned int Jsize = _mapper->getJSizeFromCellID(cellID);

    std::string encoding = _mapper->getDecoder()->getCellIDEncoding();

    // std::cout << "CellNeighbours getNeighbours of cellID " << cellID << std::endl;
    // std::cout << "Encoding string " << encoding << std::endl;
    // std::cout << "I/J/K " << I << " " << J << " " << K << std::endl;
    // std::cout << "I size " << Isize << " J size " << Jsize << std::endl;

    std::set<int> directNeighboursModule;
    std::set<int> directNeighboursForward;
    std::set<int> directNeighboursBackward;
    std::set<int> cornerNeighboursModule;
    std::set<int> cornerNeighboursForward;
    std::set<int> cornerNeighboursBackward;


    for (unsigned int i = I-1; i < I + Isize + 1; ++i )
    for (unsigned int j = J-1; j < J + Jsize + 1; ++j ) {
      if ( ( i >= I ) && ( i < I + Isize ) && ( j >= J ) && ( j < J + Jsize ) ) { // within cell area
        checkForCell(i, j, K-1, directNeighboursBackward);
        checkForCell(i, j, K+1, directNeighboursForward);
      }
      else if ( ( i < I ) && ( ( j < J ) || ( j >= J + Jsize ) ) || ( i >= I + Isize ) && ( ( j < J ) || ( j >= J + Jsize ) ) ) { // corner cells
        checkForCell(i, j, K-1, cornerNeighboursBackward);
        checkForCell(i, j, K,   cornerNeighboursModule);
        checkForCell(i, j, K+1, cornerNeighboursForward);
      }
      else { // rest should be direct neighbours
        checkForCell(i, j, K-1, directNeighboursBackward);
        checkForCell(i, j, K,   directNeighboursModule);
        checkForCell(i, j, K+1, directNeighboursForward);
      }
    }

    CellNeighbours *neighbours = new CellNeighbours(cellID);
    for (std::set<int>::iterator iter = directNeighboursModule.begin(); iter != directNeighboursModule.end(); ++iter )
    neighbours->addNeighbour( (*iter), CellNeighbours::direct, CellNeighbours::module );

    for (std::set<int>::iterator iter = directNeighboursForward.begin(); iter != directNeighboursForward.end(); ++iter )
    neighbours->addNeighbour( (*iter), CellNeighbours::direct, CellNeighbours::forward );

    for (std::set<int>::iterator iter = directNeighboursBackward.begin(); iter != directNeighboursBackward.end(); ++iter )
    neighbours->addNeighbour( (*iter), CellNeighbours::direct, CellNeighbours::backward );

    for (std::set<int>::iterator iter = cornerNeighboursModule.begin(); iter != cornerNeighboursModule.end(); ++iter )
    neighbours->addNeighbour( (*iter), CellNeighbours::corner, CellNeighbours::module );

    for (std::set<int>::iterator iter = cornerNeighboursForward.begin(); iter != cornerNeighboursForward.end(); ++iter )
    neighbours->addNeighbour( (*iter), CellNeighbours::corner, CellNeighbours::forward );

    for (std::set<int>::iterator iter = cornerNeighboursBackward.begin(); iter != cornerNeighboursBackward.end(); ++iter )
    neighbours->addNeighbour( (*iter), CellNeighbours::corner, CellNeighbours::backward );

    return neighbours;
  }


  lcio::LCCollection* CellNeighbourCalculator::getNeighbours() const {

    lcio::LCCollectionVec *collection = new lcio::LCCollectionVec(lcio::LCIO::LCGENERICOBJECT);

    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter ) {

      int cellID = (*iter);

      CellNeighbours *neighbours = getNeighbours(cellID);

      collection->addElement(neighbours);
    }
    return collection;
  }

  void CellNeighbourCalculator::getNeighbours(MappedContainer<CellNeighbours>* container) const {

    container->clear();

    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter ) {

      int cellID = (*iter);

      CellNeighbours *neighbours = getNeighbours(cellID);

      container->fillByCellID(cellID,neighbours);
    }

  }


} // end namespace CALICE
