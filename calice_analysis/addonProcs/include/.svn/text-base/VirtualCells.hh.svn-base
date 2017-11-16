#ifndef VirtualCells_hh
#define VirtualCells_hh 1

#include <vector>

namespace CALICE {

  /**
   * container class for virtual sub cells
   *
   * @author Benjamin.Lutz@desy.de
   * @date July 2009
   * @version 0.1
   */
  class VirtualCells {
  public:

    /**
     * constructor
     *
     * @param cellID (Mokka) cell ID of the mother cell
     * @param angle angluar orientation of mother cell
     */
    VirtualCells(const int cellID, const float angle);

    /**
     * get cellID of the mother cell
     *
     * @returns (Mokka) cell ID of mother cell
     */
    int getCellID() const {return _cellID;}

    /**
     * get number of sub cells
     *
     * returns number of sub cells
     */
    unsigned int getNumberOfVirtualCells() const {return _cellPositions.size();}

    /**
     * get cell position of the i-th sub cell
     *
     * @returns array of 3 floats (x,y,z)
     */
    const float* getVirtualCellPosition(unsigned int i) const {return _cellPositions.at(i);}

    /**
     * add sub cell
     *
     * @param[in] x x position of sub-cell
     * @param[in] y y position of sub-cell
     * @param[in] z z position of sub-cell
     */
    void addCell(const float x, const float y, const float z);

    /**
     * add sub cell
     *
     * @param[in] position array of 3 floats (x,y,z)
     */
    void addCell(const float *position);

    /**
     * get the angle of cells
     *
     * @returns angle of cells
     */
    float getAngle() const {return _cellAngle;}

  protected:
    const int _cellID;
    const float _cellAngle;
    std::vector<const float*> _cellPositions;
  };

} // end namespace CALICE

#endif
