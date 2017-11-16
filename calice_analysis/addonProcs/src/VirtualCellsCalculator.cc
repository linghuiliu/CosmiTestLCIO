#include "VirtualCellsCalculator.hh"

#include "CellIterator.hh"

#include <cmath>

namespace CALICE {

  VirtualCellsCalculator::VirtualCellsCalculator(const Mapper *mapper) : _mapper(mapper) {
  }

  void VirtualCellsCalculator::calculate( MappedContainer<CellDescription> *cellDescription, const float virtualCellSizeX, const float virtualCellSizeY, MappedContainer<VirtualCells> * container) {

    container->clear();

    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter ) {

      int cellID = (*iter);

      CellDescription* motherCell = cellDescription->getByCellID( cellID );

      float cellX = motherCell->getX();
      float cellY = motherCell->getY();
      float cellZ = motherCell->getZ();

      float cellAngle = motherCell->getAngle();

      float cellSizeX = motherCell->getSizeX();
      float cellSizeY = motherCell->getSizeY();

      float pi = M_PI;

      VirtualCells *virtualCells = new VirtualCells( cellID, cellAngle );

      for (float virtualX=-cellSizeX/2. + virtualCellSizeX/2.; virtualX < cellSizeX/2.; virtualX += virtualCellSizeX)
        for (float virtualY=-cellSizeY/2. + virtualCellSizeY/2.; virtualY < cellSizeY/2.; virtualY += virtualCellSizeY) {
          float virtualCellX = cellX + virtualX * cos(cellAngle/180.*pi) ;
          float virtualCellY = cellY + virtualY;
          float virtualCellZ = cellZ - virtualX * sin(cellAngle/180.*pi) ;

          virtualCells->addCell(virtualCellX, virtualCellY, virtualCellZ);
        }

      container->fillByCellID(cellID,virtualCells);
    }

  }
} // end namespace CALICE
