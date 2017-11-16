#include <iostream>
#include <DetectorTransformation.hh>

namespace CALICE {
  void DetectorTransformation::print(std::ostream &os) 
  {
    os <<"*** Detector Transformation::print() *** "
       << "\n face position (mm): ( " << getDetectorX0() <<" ; "<< getDetectorY0() <<" ; "<< getDetectorZ0() <<" )"
       << "\n detector angle (deg)=" << getDetectorAngleZX()
       << "\n rotation origin: x=" << getDetectorRotationX0() << " z=" << getDetectorRotationZ0()
       <<"\n"<< std::endl;
  }
}
