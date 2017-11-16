#ifndef _CUTHELPER_HH_
#define _CUTHELPER_HH_

#include<iostream>
#include<string>
#include<sstream>

#include "TCut.h"

/**
 *
 * Set of functions to provide consistent cut parameters to all tools and analysis scripts.
 * The cuts are presented in CAN-034 and PhD thesis "Low-energetic Hadron Interactions in a Highly Granular Calorimeter" by N. Feege (in preparation).
 * The corresponding analysis is based on data collected with the AHCAL and the TCMT (without any ECAL) at Fermilab in July 2008 and May 2009.
 *
 */


TCut getBeamCut(){

  TCut cut("beamBit==1");

  return cut;

}


TCut getNoiseCut(){

  TCut cut("pedestalBit == 1 && calibBit == 0 && spillBit == 0");

  return cut;

}


TCut getPionCut( float eBeam ){

  TCut cut("");

  std::stringstream ss_cutString;

  ss_cutString << " event_AhcVetoRegion_ESum[5] >= 4";                     // remove empty events
  ss_cutString << " && vetoBit == 0 && event_AhcVetoRegion_nHits[4] < 15"; // remove pre-shower events

  if ( eBeam == 1 ) {

    ss_cutString << " && (ahc_energyPerLayer[29] + ahc_energyPerLayer[30] + ahc_energyPerLayer[31] + ahc_energyPerLayer[32] + ahc_energyPerLayer[33] + ahc_energyPerLayer[34] + ahc_energyPerLayer[35] + ahc_energyPerLayer[36] + ahc_energyPerLayer[37] + ahc_energyPerLayer[38]) < 4"; // muon cut at 1 GeV

  }
  else if ( eBeam == 2 || eBeam == 3 ) {

    ss_cutString << " && ( ahc_energyPerLayer[33] + ahc_energyPerLayer[34] + ahc_energyPerLayer[35] + ahc_energyPerLayer[36] + ahc_energyPerLayer[37] + ahc_energyPerLayer[38] ) < 3"; // muon cut at 2 GeV

  }
  else if ( eBeam == 4 ) {

    ss_cutString << " && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] >= 1 && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] <= 38"; // muon cut at 4 GeV
    //ss_cutString << " && ( ahc_energyPerLayer[34] + ahc_energyPerLayer[35] + ahc_energyPerLayer[36] + ahc_energyPerLayer[37] + ahc_energyPerLayer[38] ) < 5"; // alternate muon cut at 4 GeV

  }
  else if ( eBeam > 4 && eBeam < 8 ) {

    ss_cutString << " && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] >= 1 && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] <= 38";

  }
  else {

    ss_cutString << " && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] >= 1 && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] <= 38 && ahc_nHits > 60";

  }

  cut = (ss_cutString.str()).c_str();

  return cut;

}


TCut getContainmentCut( float eBeam ){

  TCut cut("");

  std::stringstream ss_cutString;

  ss_cutString << " event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] >= 1 && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] <= 5";

  cut = (ss_cutString.str()).c_str();

  return cut;

}


TCut getStartFoundCut( float eBeam ){

  TCut cut("");

  std::stringstream ss_cutString;

  ss_cutString << " event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] >= 1 && event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] <= 38";

  cut = (ss_cutString.str()).c_str();

  return cut;

}


TCut getElectronCut( float eBeam ){


  TCut cut("");

  std::stringstream ss_cutString;

  ss_cutString << " event_AhcVetoRegion_ESum[5] >= 4";                     // remove empty events
  ss_cutString << " && vetoBit == 0 && event_AhcVetoRegion_nHits[4] < 15"; // remove pre-shower events

  ss_cutString << " && ahc_cogZ < 1892 ";                                  // basic electron cut from EM paper

  if ( eBeam == 1 ) {

    ss_cutString << " && ( ahc_energyPerLayer[19] + ahc_energyPerLayer[20] + ahc_energyPerLayer[21] + ahc_energyPerLayer[22] + ahc_energyPerLayer[23] + ahc_energyPerLayer[24] + ahc_energyPerLayer[25] + ahc_energyPerLayer[26] + ahc_energyPerLayer[27] + ahc_energyPerLayer[28] + ahc_energyPerLayer[29] + ahc_energyPerLayer[30] + ahc_energyPerLayer[31] + ahc_energyPerLayer[32] + ahc_energyPerLayer[33] + ahc_energyPerLayer[34] + ahc_energyPerLayer[35] + ahc_energyPerLayer[36] + ahc_energyPerLayer[37] + ahc_energyPerLayer[38] ) < 8"; // muon cut at 1 GeV

    ss_cutString << " && event_AhcShowerStartClusterPattern_maxE_E >= 6 && event_AhcShowerStartClusterPattern_maxE_Hits >= 2"; // muon, pion, empty events cut at 1 GeV

  }
  else {

    ss_cutString << " && ( ahc_energyPerLayer[29] + ahc_energyPerLayer[30] + ahc_energyPerLayer[31] + ahc_energyPerLayer[32] + ahc_energyPerLayer[33] + ahc_energyPerLayer[34] + ahc_energyPerLayer[35] + ahc_energyPerLayer[36] + ahc_energyPerLayer[37] + ahc_energyPerLayer[38] ) < 5"; // muon cut above 1 GeV

    ss_cutString << " && event_AhcShowerStartClusterPattern_maxE_E >= 18 && event_AhcShowerStartClusterPattern_maxE_Hits >= 2"; // muon, pion, empty events cut above 1 GeV

  }

  cut = (ss_cutString.str()).c_str();

  return cut;

}


TCut getMuonCut( float eBeam ){

  TCut cut("");

  std::stringstream ss_cutString;

  ss_cutString << " event_AhcMipTrackBitGenerator_maxTrack_hits>=34 && event_AhcMipTrackBitGenerator_maxTrack_hits<=36"; // select long tracks (tight cut)

  ss_cutString << " && ahc_nHits < 60 "; // select only single-muon events

  ss_cutString << " && multiADC > 2000 "; // require signal of at least 1 MIP in multiplicity counter

  ss_cutString << " && ( event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] < 1 || event_AhcShowerStartClusterProcessor_showerStartPos_layer[0] > 38 )"; // remove showering events

  cut = (ss_cutString.str()).c_str();

  return cut;

}


TCut getTBCut( float eBeam , std::string particle ){


  TCut cut("");

  std::stringstream ss_cutString;

  ss_cutString << " 1 ";

  if ( particle == "pi" ) {

    ss_cutString << " && multiADC < 3800";    // remove multi-particle events

    ss_cutString << " && cherenkow2Bit == 0"; // remove possible electron contamination

    if ( eBeam < 8 ) {

      ss_cutString << " && cherenkowBit == 0";
    }
    else {

      ss_cutString << " && cherenkowBit == 1";

    }

  }

  if ( particle == "e" ) {

    ss_cutString << " && multiADC < 3800";                        // remove multi-particle events

    ss_cutString << "&& cherenkow2Bit == 1 && cherenkowBit == 0"; // remove possible pion contamination

  }

  if ( particle == "mu" ) {

  }

  cut = (ss_cutString.str()).c_str();

  return cut;

}


TCut getMCCut( float eBeam ){

  TCut cut("mcp_endPos[2]>1532 && mcp_endPos[2]<2755");

  return cut;

}


TCut getCut( std::string cutname, float eBeam ){

  TCut cut_i = "";

  if ( cutname == "beam" ) cut_i = getBeamCut();

  if ( cutname == "noise" ) cut_i = getNoiseCut();

  /* particle ID cuts
   */
  if ( cutname == "pi" ) cut_i = getPionCut( eBeam );

  if ( cutname == "e" ) cut_i = getElectronCut( eBeam );

  if ( cutname == "mu" ) cut_i = getMuonCut( eBeam );

  /* cuts for use only with TestBeam data
   */
  if ( cutname == "tb_pi" ) cut_i = getTBCut( eBeam , "pi" );

  if ( cutname == "tb_e" ) cut_i = getTBCut( eBeam , "e"  );

  if ( cutname == "tb_mu" ) cut_i = getTBCut( eBeam , "mu"  );

  /* additional cuts
   */
  if ( cutname == "cont" ) cut_i = getContainmentCut( eBeam );

  if ( cutname == "found" ) cut_i = getStartFoundCut( eBeam );

  /* cuts for use only with Monte Carlo data
   */
  if ( cutname == "mc" ) cut_i = getMCCut( eBeam );

  return cut_i;

}


#endif
