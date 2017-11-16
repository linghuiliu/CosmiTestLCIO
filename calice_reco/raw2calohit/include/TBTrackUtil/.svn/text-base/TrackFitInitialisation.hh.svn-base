#ifndef TBTrack_TrackFitInitialisation_HH
#define TBTrack_TrackFitInitialisation_HH

#include <string>
#include <iostream>

#include "TMatrixDSym.h"


namespace TBTrack {

  class TrackFitInitialisation {
    
  public:
    TrackFitInitialisation();
    
    // z position of hits
    double zLayer(unsigned l) const;
    void   zLayer(unsigned l, double z);
    
    // z position of beam origin
    double zBeam() const;
    void   zBeam(double z);
    
    // Average beam coordinate and tan(angle)
    double beamCoordinate() const;
    double beamTanAngle() const;
    
    // Insert beam position and angle at once
    void beamAverage(double c, double t);
    
    // Beam spread error matrix
    const TMatrixDSym& errorMatrix() const;
    void               errorMatrix(const TMatrixDSym &e);
    
    std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;
    
    
  private:
    double _zLayer[4];
    double _zBeam;
    double _beamAverage[2];
    TMatrixDSym _errorMatrix;
  };

}

#endif
