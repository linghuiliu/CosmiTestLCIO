#include "VirtualCells.hh"

namespace CALICE {
  VirtualCells::VirtualCells(int cellID, float angle) : _cellID(cellID), _cellAngle(angle) {
  }

  void VirtualCells::addCell(const float x, const float y, const float z) {
    float* pos = new float[3];
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    _cellPositions.push_back(pos);
  }

  void VirtualCells::addCell(const float *position) {
    float* pos = new float[3];
    pos[0] = position[0];
    pos[1] = position[1];
    pos[2] = position[2];
    _cellPositions.push_back(pos);
  }

} // end namespace CALICE
