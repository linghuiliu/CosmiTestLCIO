#include "example001_CutHelper.hh" // provides event selection criteria for different particle types and beam momenta

example001_makePlots_anaReco(){

  /*

  Example ROOT macro creating analysis plots presented in CAN-034 and PhD thesis "Low-energetic Hadron Interactions in a Highly Granular Calorimeter" by N. Feege (in preparation).
  The analysis is based on data collected with the AHCAL and the TCMT (without any ECAL) at Fermilab in July 2008 and May 2009.
  The example run 520308 is a 10 GeV negative pion run.

  Conversion 1 mm = 0.00345 lambda
  AHCAL front face at z = 1532 mm

  */


  /* style issues */
  gStyle->SetOptStat(1110);
  /* ----- ----- ----- ----- ----- */


  /* define cuts, get cuts from CutHelper.hh
   */
  float eBeam = 10;
  TCut cut_beam = getCut( "beam" , eBeam );
  TCut cut_data_1 = getCut( "pi" , eBeam );
  TCut cut_data_2 = getCut( "tb_pi" , eBeam );
  /* ----- ----- ----- ----- ----- */


  /* open file and get tree */
  TFile *fin = new TFile("AnaReco_Run520308_rec.root","OPEN");
  TTree *tin = (TTree*)fin->Get("bigtree");
  /* ----- ----- ----- ----- ----- */


  /* create histogram frames */
  TH1F* h_eSum = new TH1F("h_eSum", ";E_{vis} [MIP];# entries", 40, 0, 800);
  TH1F* h_meanZ = new TH1F("h_meanZ", ";Z #left[#lambda_{int}#right];# entries", 40, 0, 4);
  TH1F* h_sigmZ = new TH1F("h_sigmZ", ";#sigma_{Z} #left[#lambda_{int}#right];# entries", 40, 0, 2);
  TH1F* h_meanR = new TH1F("h_meanR", ";R [mm];# entries", 40, 0, 200);
  TH1F* h_sigmR = new TH1F("h_sigmR", ";#sigma_{R} [mm];# entries", 40, 0, 200);

  TProfile *p_lon = new TProfile("p_lon", ";z #left[#lambda_{int}#right]; #LT E_{vis} #GT [MIP]", 38, 10.175*0.00345, 1212.875*0.00345);
  TProfile *p_rad = new TProfile("p_rad", ";r [mm]; #LT E_{vis} #GT #left[MIP/mm^{2}#right]", 20, 0, 600);
  /* ----- ----- ----- ----- ----- */


  /* fill histograms */
  tin->Draw("ahc_energySum >> h_eSum", cut_beam && cut_data_1 && cut_data_2);

  tin->Draw("(event_AhcAppendIntegralObservables_length1-1532) * 0.00345 >> h_meanZ", cut_beam && cut_data_1 && cut_data_2);
  tin->Draw(" event_AhcAppendIntegralObservables_length2       * 0.00345 >> h_sigmZ", cut_beam && cut_data_1 && cut_data_2);

  tin->Draw("event_AhcAppendIntegralObservables_rad1 >> h_meanR", cut_beam && cut_data_1 && cut_data_2);
  tin->Draw("event_AhcAppendIntegralObservables_rad2 >> h_sigmR", cut_beam && cut_data_1 && cut_data_2);

  tin->Draw("event_AhcAppendLongitudinalObservables_eSumPerZBin : (event_AhcAppendLongitudinalObservables_binCenterZ-1532)*0.00345 >> p_lon", cut_beam && cut_data_1 && cut_data_2, "prof");
  tin->Draw("AhcProfileProcessor_radialEnergyCog/AhcProfileProcessor_radialNormCog : AhcProfileProcessor_radialPositionCog >> p_rad", cut_beam && cut_data_1 && cut_data_2 , "prof");
  /* ----- ----- ----- ----- ----- */


  /* draw everything */
  TCanvas *c_eSum = new TCanvas();
  h_eSum->Draw("hist");

  TCanvas *c_meanZ = new TCanvas();
  h_meanZ->Draw("hist");

  TCanvas *c_sigmZ = new TCanvas();
  h_sigmZ->Draw("hist");

  TCanvas *c_meanR = new TCanvas();
  h_meanR->Draw("hist");

  TCanvas *c_sigmR = new TCanvas();
  h_sigmR->Draw("hist");

  TCanvas *c_lon = new TCanvas();
  p_lon->Draw("prof");

  TCanvas *c_rad = new TCanvas();
  p_rad->Draw("prof");
  /* ----- ----- ----- ----- ----- */


}
