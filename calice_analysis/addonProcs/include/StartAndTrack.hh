/** @brief  First interaction layer and primary track finder

@author M.V. Chadeeva
@ date November 2010
*/
// version 2.00
//========================================================================
//------- Function getStartingLayer finds shower starting point-------
//------------------------------------------------------------------------
unsigned CALICEOverlayPreparation::getStartingLayer(CLayer* lr) {

  unsigned start = EHLAY-1;
  bool shower = false;
  unsigned i = 0;
  while (!lr[i].nh && i < EHLAY-1) { i++; }
  while ( (i < EHLAY-1) && !shower ) {
    if ( (mov_av[i] + mov_av[i+1]) > miplim
	 && (lr[i].nh + lr[i+1].nh) > hitlim )
      shower = true;
    i++;
  }
  if ( shower ) {
    if ( ( (i < EHLAY - 3) &&
	 ( (lr[i+2].emip < lr[i].emip) && (lr[i+3].emip < lr[i].emip) 
	     && ( (lr[i+3].emip + lr[i+2].emip) < (lr[i+1].emip + lr[i].emip) ) ) )
	   || ( i == ELAY ) )
      start = i;
    else start = i - 1;
  }
    return start;
}
//===========================================================================
//  Function FindTrack finds track in ECAL & HCAL
//  using "nearest neighbour" criteria.
//  Maximum distance parameters are defined taking in account the cell size
//--------------------------------------------------------------------------  

bool CALICEOverlayPreparation::FindTrack(unsigned start, CLayer* lr) {

  int len2sh = int(start) - int(calor_front) - _absTrackLengthLimit + 1;
  //if ( len2sh < 1 ) return false;
  bool isTrack = false;
  int ntrcand = 0;
  float xtr[20];
  float ytr[20];
// ------- Finding Primary track ----------------------------
  CHit* pcur = NULL;
  CHit* pc1 = NULL;
  float rtrack;
  bool isGap;
  std::vector<CHit*> temp;

  int i = 0; // first layer

  while ( i < len2sh && !isTrack) {               // Loop layer by layer to search track beginning
    while (!lr[i].nh && i < len2sh-1) i++;        // skip empty layers
    if ( (i == len2sh-1) && !lr[i].nh ) break;
    float rtmin = SQ12;
    int jgap = 0;
    for(std::vector<CHit*>::iterator p0 = lr[i].vl.begin();
	p0 != lr[i].vl.end(); p0++) {             // loop over  hits in layer of track beginning
      temp.clear();
      pcur = *p0;
      isGap = false;
      unsigned j = unsigned(i);
      while ( (!isGap) && (j < start) ) {        // loop over layers and fill temp candidate vector with hits
	jgap = 0;
	pc1 = NULL;
	temp.push_back(pcur);                    // push current hit in candidate track
	float r01max = pcur->r;                  // set max allowable radial distance to the next hit
	while ((j < start) && !pc1 && (jgap <= _NGAP)) { // loop over next to find nearest
	  if ((pcur->l < ELAY) && (j > ELAY-1)) r01max = SQ3;
	  for(std::vector<CHit*>::iterator p1 = lr[j+1].vl.begin(); p1 != lr[j+1].vl.end();
	      p1++) {                             // loop over the points in the next layer
	    float r01 = 0.0;
	    if ( pcur->b && ( (fabs(pcur->x) < fabs((*p1)->x)) || (fabs(pcur->y) < fabs((*p1)->y)) ) )
	      r01max = pcur->r + SQADD; 
	    for(std::vector<CHit*>::iterator pt = temp.begin(); pt != temp.end();
		pt++)                             // loop over previously filled candidate hits
	      r01 += fabs(distXY(*pt, *p1) - ((*p1)->z - (*pt)->z)*tba);
	    r01 /= temp.size();
	    if (r01 < r01max) {
	      r01max = r01;
	      pc1 = *p1; // select the nearest to all previous 
	    }
	  }
	  j++;
	  jgap++;
	}  // end of search for next nearest

	if (pc1 && ( j < start )) pcur = pc1;
	if ( jgap > _NGAP ) isGap = true;
      }// end of loop over layers up to FIP

      int tsize = temp.size();                 // temp track size for slopes comparison
      if ( (tsize > _absTrackLengthLimit) && 
	   ( float(tsize) >= (float(start) - float(calor_front))*_relTrackLengthLimit) ) {
	rtrack = fabs(distXY(*p0, pcur) - (pcur->z - (*p0)->z)*tba);
	if ( rtrack < pcur->r ) {
	  xtr[ntrcand] = 0.;
	  ytr[ntrcand] = 0.;
	  for(std::vector<CHit*>::iterator pt = temp.begin();
	      pt != temp.end(); pt++) {       // loop over previously filled candidate hits
	    xtr[ntrcand] += (*pt)->x;
	    ytr[ntrcand] += (*pt)->y;
	  }
	  xtr[ntrcand] /= tsize;
	  ytr[ntrcand] /= tsize;

	  if (rtrack < rtmin) {
	    isTrack = true;
	    rtmin = rtrack;
	    intrack.clear();
	    for (unsigned jj = 0; jj < temp.size(); jj++) 
	      intrack.push_back(temp[jj]); // put currently best in the final vector
	  }                                // end if closer to ideal slope
	  ntrcand++;
	}                                  // end if candidate is inside slope limit
      }                                    // end if track is long enough
    }                                      // end of loop over hits in first track layer
    i++;
  }                                        // end of loop layer by layer

  if ( ntrcand > 20 ) isTrack = false;
  else {
    if ( start >= ELAY2  && ntrcand > 1 ) {
      float t2tmax = 0.;
      for ( int k1 = 0; k1 < ntrcand; k1++ ) {
	for ( int k2 = k1+1; k2 < ntrcand; k2++ ) {
	  float dtrack = sqrt((xtr[k1] - xtr[k2])*(xtr[k1] - xtr[k2]) +
			      (ytr[k1] - ytr[k2])*(ytr[k1] - ytr[k2]) );
	  if ( dtrack > t2tmax ) t2tmax = dtrack;
	}
      }
      if ( ( t2tmax > ParaDistmax ) 
	   || ( ( start >= ELAY ) && ( t2tmax > ParaDistmin ) ) )
	paraID = true; // skip in case of parallel tracks
    }
  }
  return isTrack;
}

