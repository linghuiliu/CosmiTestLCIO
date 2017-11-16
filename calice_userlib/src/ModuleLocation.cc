
#include "ModuleLocation.hh"
#include "CellIndex.hh"

namespace CALICE {

  void ModuleLocation::print(  std::ostream& os )
  {
    CellIndex a_cell( getCellIndexOffset() );
    os 
      << " cell index offset: layer=" << a_cell.getLayerIndex() 
      << " wafer=" << a_cell.getWaferColumn() << ", " << a_cell.getWaferRow();
    if (a_cell.getPadColumn()!=0 || a_cell.getPadRow()!=0) {
      os  << " pad=" << a_cell.getPadColumn() << ", " << a_cell.getPadRow();
    }
    os << " type=" << (int) getModuleType() 
       << " pos=" << getX() << "," << getY() << "," << getZ()
       << std::endl;
  }

}
