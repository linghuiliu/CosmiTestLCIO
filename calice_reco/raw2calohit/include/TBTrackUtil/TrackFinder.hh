#ifndef TrackFinder_HH
#define TrackFinder_HH

#include <iostream>
#include <vector>

#include "TrackProjection.hh"
#include "TrackFitter.hh"
#include "AlnConstants.hh"
#include "FitConstants.hh"


namespace TBTrack {

  class TrackFinder {
    
  public:
    TrackFinder();  
    
    void alnConstants(const AlnConstants &a);
    void fitConstants(const FitConstants &p);
    
    std::vector<TrackProjection> find(unsigned fb, unsigned eh, unsigned xy,
				      const std::vector<int> &v0,
				      const std::vector<int> &v1,
				      const std::vector<int> &v2,
				      const std::vector<int> &v3);
    
    std::ostream& print(std::ostream &o) const;
    
    
  private:
    double _probabilityCut[4];
    AlnConstants _alignment;
    TrackFitter _fitter[2][2][2];
  };
  
}

#endif
