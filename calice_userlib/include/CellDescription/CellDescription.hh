#ifndef CellDescription_hh
#define CellDescription_hh 1

namespace CALICE {

  /**
   * class to hold data describing calorimeter cells
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class CellDescription {

  public:
    /**
     * set position (mm)
     *
     * @param[in] x x-coordinate
     * @param[in] y y-coordinate
     * @param[in] z z-coordinate
     */
    void setPosition (const float x, const float y, const float z) {
      _x = x;
      _y = y;
      _z = z;
    }
    /**
     * set size (mm)
     *
     * @param[in] sizeX cell size in x direction
     * @param[in] sizeY cell size in y direction
     */
    void setSize(const float sizeX, const float sizeY) {
      _sizeX = sizeX;
      _sizeY = sizeY;
    }

    /**
     * set angle (degree)
     *
     * @param angle angle
     */
    void setAngle(const float angle) {
      _angle = angle;
    }

    /**
     * @returns x-coordinate in mm
     */
    float getX() const { return _x;}
    /**
     * @returns y-coordinate in mm
     */
    float getY() const { return _y;}
    /**
     * @returns z-coordinate in mm
     */
    float getZ() const { return _z;}

    /**
     * @returns x-size in mm
     */
    float getSizeX() const { return _sizeX;}
    /**
     * @returns y-size in mm
     */
    float getSizeY() const { return _sizeY;}

    /**
     * @returns angle in degree
     */
    float getAngle() const { return _angle;}

  private:
    float _x;
    float _y;
    float _z;
    float _angle;
    float _sizeX;
    float _sizeY;
  };

} // end namespace CALICE

#endif
