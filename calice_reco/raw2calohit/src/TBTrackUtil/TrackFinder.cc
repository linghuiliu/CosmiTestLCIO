#include <cassert>

#include "TVectorD.h"

#include "TrackFinder.hh"

namespace TBTrack {

TrackFinder::TrackFinder() {
}

void TrackFinder::alnConstants(const AlnConstants &ta) {
  _alignment=ta;
}

void TrackFinder::fitConstants(const FitConstants &te) {
  for(unsigned i(0);i<4;i++) {
    _probabilityCut[i]=te.probabilityCut(i+1);
  }

  for(unsigned xy(0);xy<2;xy++) {
    for(unsigned fb(0);fb<2;fb++) {
      for(unsigned eh(0);eh<2;eh++) {
	//te.fitConstants(xy,fb,eh).print();
	_fitter[xy][fb][eh].fitInitialisation(te.fitInitialisation(xy,fb,eh));
      }
    }
  }
}

std::vector<TrackProjection> TrackFinder::find(unsigned fb, unsigned eh, unsigned xy,
					       const std::vector<int> &v0,
					       const std::vector<int> &v1,
					       const std::vector<int> &v2,
					       const std::vector<int> &v3) {
  assert(xy<=1);
  assert(fb<=1);
  assert(eh<=1);
  
  const bool print(false);

  if(print) std::cout << "TrackFinder::find() "
		      << "Number of combinations = "
		      << "(" << v0.size() << " + 1) x "
		      << "(" << v1.size() << " + 1) x "
		      << "(" << v2.size() << " + 1) x "
		      << "(" << v3.size() << " + 1) = "
		      << (v0.size()+1)*(v1.size()+1)*(v2.size()+1)*(v3.size()+1) <<std::endl;

  TVectorD c(4);
  std::vector<std::vector<std::vector<std::vector<TrackProjection> > > > vTrack;

  TrackProjection dummy;
  dummy.chiSquared(1.0e6);

  unsigned hitPattern(0);
  int hits[4];

  for(unsigned l0(0);l0<=v0.size();l0++) {
    hitPattern=0;
    hits[0]=999999;
    if(l0<v0.size()) {
      hitPattern+=1;
      hits[0]=v0[l0];
    }
    c(0)=_alignment.coordinate(xy,0,hits[0]);
    vTrack.push_back(std::vector<std::vector<std::vector<TrackProjection> > >());

    for(unsigned l1(0);l1<=v1.size();l1++) {
      hitPattern&=1;
      hits[1]=999999;
      if(l1<v1.size()) {
	hitPattern+=2;
	hits[1]=v1[l1];
      }
      c(1)=_alignment.coordinate(xy,1,hits[1]);
      vTrack[l0].push_back(std::vector<std::vector<TrackProjection> >());
      
      for(unsigned l2(0);l2<=v2.size();l2++) {
	hitPattern&=3;
	hits[2]=999999;
	if(l2<v2.size()) {
	  hitPattern+=4;
	  hits[2]=v2[l2];
	}
	c(2)=_alignment.coordinate(xy,2,hits[2]);
	vTrack[l0][l1].push_back(std::vector<TrackProjection>());
	
	for(unsigned l3(0);l3<=v3.size();l3++) {
	  hitPattern&=7;
	  hits[3]=999999;
	  if(l3<v3.size()) {
	    hitPattern+=8;
	    hits[3]=v3[l3];
	  }
	  c(3)=_alignment.coordinate(xy,3,hits[3]);

	  /////// DISABLED !!!
	  for(unsigned bc(0);bc<1;bc++) {
	    hitPattern&=15;
	    if(bc==1) hitPattern+=48;

	    /*
	    unsigned nHits(0);
	    for(unsigned i(0);i<4;i++) {
	      if((hitPattern&(1<<i))!=0) {
		c(nHits)=x[i];
		nHits++;
	      }
	    }
	    c.ResizeTo(nHits);
	    */
	    
	  /*
	    if(hitPattern==15 ||
	    hitPattern== 7 ||
	    hitPattern==11 ||
	    hitPattern==13 ||
	    hitPattern==14) {
	    
	    if(hitPattern==15) {
	    c.ResizeTo(4);
	    c(0)=x[0];
	    c(1)=x[1];
	    c(2)=x[2];
	    c(3)=x[3];
	    }
	    
	    if(hitPattern== 7) {
	    c(0)=x[0];
	    c(1)=x[1];
	    c(2)=x[2];
	    }
	    
	    if(hitPattern==11) {
	    c(0)=x[0];
	    c(1)=x[1];
	    c(2)=x[3];
	    }
	    
	    if(hitPattern==13) {
	    c(0)=x[0];
	    c(1)=x[2];
	    c(2)=x[3];
	    }
	    
	    if(hitPattern==14) {
	    c(0)=x[1];
	    c(1)=x[2];
	    c(2)=x[3];
	    }
	  */
	  
	    //	  if(nHits>=3) {
	    TrackProjection trk(_fitter[xy][fb][eh].fitResult(hitPattern,c));
	    trk.fitType(xy,fb,eh);
	    for(unsigned i(0);i<4;i++) trk.hit(i,hits[i]);

	    //trk.xy(xy);
	    vTrack[l0][l1][l2].push_back(trk);
	    //  } else {
	    //dummy.hitPattern(hitPattern);
	    //vTrack[l0][l1][l2].push_back(dummy);
	    //}
	    if(print) {
	      std::cout << "TrackFinder::find() "
			<< "Combination "
			<< l0 << ", " << l1 << ", "
			<< l2 << ", " << l3 << std::endl;
	      vTrack[l0][l1][l2][l3].print();
	    }
	  }
	}
      }
    }
  }

  std::vector<TrackProjection> vGood;

  std::vector<bool> used[4];
  for(unsigned l0(0);l0<=v0.size();l0++) used[0].push_back(false);
  for(unsigned l1(0);l1<=v1.size();l1++) used[1].push_back(false);
  for(unsigned l2(0);l2<=v2.size();l2++) used[2].push_back(false);
  for(unsigned l3(0);l3<=v3.size();l3++) used[3].push_back(false);

  bool goodTrack(true);

  while(goodTrack) {

    goodTrack=false;
    double maxProb(_probabilityCut[1]);
    unsigned maxHits[4] = {0,0,0,0};

    for(unsigned l0(0);l0<v0.size();l0++) {
      if(!used[0][l0]) {
	for(unsigned l1(0);l1<v1.size();l1++) {
	  if(!used[1][l1]) {
	    for(unsigned l2(0);l2<v2.size();l2++) {
	      if(!used[2][l2]) {
		for(unsigned l3(0);l3<v3.size();l3++) {
		  if(!used[3][l3]) {
		    
		    assert(vTrack[l0][l1][l2][l3].hitPattern()==0 ||
			   vTrack[l0][l1][l2][l3].hitPattern()==15);

		    if(vTrack[l0][l1][l2][l3].probability()>maxProb) {
		      goodTrack=true;
		      maxProb=vTrack[l0][l1][l2][l3].probability();
		      maxHits[0]=l0;
		      maxHits[1]=l1;
		      maxHits[2]=l2;
		      maxHits[3]=l3;

		      if(print) {
			std::cout << "TrackFinder::find() "
				  << "Better combination "
				  << maxHits[0] << ", "
				  << maxHits[1] << ", "
				  << maxHits[2] << ", "
				  << maxHits[3]
				  << ", probability = "
				  << vTrack[l0][l1][l2][l3].probability()
				  << std::endl;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    if(goodTrack) {
      if(print) {
	std::cout << "TrackFinder::find() "
		  << "Best combination "
		  << maxHits[0] << ", "
		  << maxHits[1] << ", "
		  << maxHits[2] << ", "
		  << maxHits[3] << std::endl;
	vTrack[maxHits[0]][maxHits[1]][maxHits[2]][maxHits[3]].print();
      }

      vGood.push_back(vTrack[maxHits[0]][maxHits[1]][maxHits[2]][maxHits[3]]);
      used[0][maxHits[0]]=true;
      used[1][maxHits[1]]=true;
      used[2][maxHits[2]]=true;
      used[3][maxHits[3]]=true;
    }
  }



  goodTrack=true;
  while(goodTrack) {

    goodTrack=false;
    double maxProb(_probabilityCut[0]);
    unsigned maxHits[4]={0,0,0,0};

    for(unsigned l0(0);l0<=v0.size();l0++) {
      if(!used[0][l0]) {
	for(unsigned l1(0);l1<=v1.size();l1++) {
	  if(!used[1][l1]) {
	    for(unsigned l2(0);l2<=v2.size();l2++) {
	      if(!used[2][l2]) {
		for(unsigned l3(0);l3<=v3.size();l3++) {
		  if(!used[3][l3]) {

		    if(vTrack[l0][l1][l2][l3].numberOfHits()==3) {
		      if(vTrack[l0][l1][l2][l3].probability()>maxProb) {
			goodTrack=true;
			maxProb=vTrack[l0][l1][l2][l3].probability();
			maxHits[0]=l0;
			maxHits[1]=l1;
			maxHits[2]=l2;
			maxHits[3]=l3;
			
			if(print) {
			  std::cout << "TrackFinder::find() "
				    << "Better combination "
				    << maxHits[0] << ", "
				    << maxHits[1] << ", "
				    << maxHits[2] << ", "
				    << maxHits[3] << ", probability = "
				    << vTrack[l0][l1][l2][l3].probability()
				    << std::endl;
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    if(goodTrack) {
      if(print) {
	std::cout << "TrackFinder::find() "
		  << "Best combination "
		  << maxHits[0] << ", "
		  << maxHits[1] << ", "
		  << maxHits[2] << ", "
		  << maxHits[3] << std::endl;
	vTrack[maxHits[0]][maxHits[1]][maxHits[2]][maxHits[3]].print();
      }

      vGood.push_back(vTrack[maxHits[0]][maxHits[1]][maxHits[2]][maxHits[3]]);
      if(maxHits[0]<v0.size()) used[0][maxHits[0]]=true;
      if(maxHits[1]<v1.size()) used[1][maxHits[1]]=true;
      if(maxHits[2]<v2.size()) used[2][maxHits[2]]=true;
      if(maxHits[3]<v3.size()) used[3][maxHits[3]]=true;
    }
  }

  if(print) std::cout << "TrackFinder::find() "
		      << "Number of good tracks = "
		      << vGood.size() << std::endl << std::endl;

  return vGood;
}

std::ostream& TrackFinder::print(std::ostream &o=std::cout) const {
  o << "TrackFinder::print()" << std::endl;
  return o;
}

}
