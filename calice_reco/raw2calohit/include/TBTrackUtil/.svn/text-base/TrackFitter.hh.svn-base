#ifndef TBTrack_TrackFitter_HH
#define TBTrack_TrackFitter_HH

#include <string>
#include <iostream>

#include "TVectorD.h"


namespace TBTrack {

  class LinearFitter;
  class TrackFitInitialisation;
  class TrackFitResult;


  class TrackFitter {
    
  public:
    TrackFitter();
    ~TrackFitter();  
    
    void fitInitialisation(const TrackFitInitialisation &fc);
    
    TrackFitResult fitResult(unsigned h, const TVectorD &c) const;
    
    std::ostream& print(std::ostream &o=std::cout, const std::string &s="") const;
    
    
  private:
    double _beam[2];
    LinearFitter *_linearFitter[64];
  };

}

#endif
