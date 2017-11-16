/** @brief  CALICE event selection (muon and trash)

@author M.V. Chadeeva
@ date November 2010
*/
// version 2.00
//=============================================================================
// Trash identification
//=============================================================================
bool CALICEOverlayPreparation::isTrash(unsigned start, float esum) {
  bool trash = false;

  // too low or too high energy deposition
  // constants e_trash_max and e_mult_min are defined in the header file
  // according to beam energy
  if ( (esum < e_trash_max) || (esum > e_mult_min) )
    trash = true;
  /*
  //exclude electrons for hadron runs with full CALICE setup
  if ( (runnum < 350000) && 
       ((nhitH + nhitT) < 30) && (start < 4) )
    trash = true;
  */
  if ( calor_front < ELAY && !nhitE ) // no hits in ECAL with ECAL in front
    trash = true;

  return trash;
}
//==============================================================================
//    Identification of muon-like events
//==============================================================================
bool CALICEOverlayPreparation::isMuon(float e_eh, float e_t) {

//  Numbers are extracted from 2D histogram
//    this is the corner triangle at histogram: E(ecal+hcal) vs E(tcmt) 
//    all muons are at this corner 
// this should work well for beam energies > 10 GeV

  if ( ((nhitE > 16) || (nhitH > 18)) && 
      (e_t > (amu12*e_eh + bmu12)) && (e_t < (amu23*e_eh + bmu23)) && (e_t < (amu13*e_eh + bmu13)) )
    return true;
  else 
    return false;
}
//===================================================================================
//     Function distXY calculates distance between two points in XY-plane
//===================================================================================
float CALICEOverlayPreparation::distXY(CHit* h1, CHit* h2) {

    return (sqrt((h1->x - h2->x)*(h1->x - h2->x) + (h1->y - h2->y)*(h1->y - h2->y)));
}
//===================================================================================
