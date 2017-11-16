#include "example001_CutHelper.hh" // provides event selection criteria for different particle types and beam momenta

using namespace std;

int example001_makePlots_3DProfile(){

  /*

  Example ROOT macro creating 3D shower profiles in the AHCAL. The example run 520308 is a 10 GeV negative pion run
  collected with the AHCAL and the TCMT (without any ECAL) at Fermilab in July 2008.

  Conversion 1 mm = 0.00345 lambda
  AHCAL front face at z = 1532 mm

  */


  /* set stye options
   */
  gStyle->SetOptStat(0);


  /* define cuts, get cuts from CutHelper.hh
   */
  float eBeam = 10;
  TCut cut_beam = getCut( "beam" , eBeam );
  TCut cut_data_1 = getCut( "pi" , eBeam );
  TCut cut_data_2 = getCut( "tb_pi" , eBeam );


  /* print cuts
   */
  cout << cut_beam.GetTitle() << endl;
  cout << cut_data_1.GetTitle() << endl;
  cout << cut_data_2.GetTitle() << endl;


  /* get input file
   */
  TFile *fin = new TFile("AnaReco_Run520308_rec.root","OPEN");
  TTree *tin = (TTree*)fin->Get("bigtree");


  /* create temporary cavas
   */
  TCanvas *ctemp = new TCanvas();


  /* determine binning in longitudinal direction
   */
  TH1F* htemp_longpos = new TH1F("htemp_longpos","",1001,-0.5,1000.5);
  tin->Draw("_N_AhcProfileProcessor_longitudinalPositionCog >> htemp_longpos");
  int zbins = htemp_longpos->GetMean() - 2; // -2 because of over and underflow bins

  tin->Draw("AhcProfileProcessor_longitudinalPositionCog:AhcProfileProcessor_longitudinalPositionCog","");

  float zwidth = tin->GetV2()[2]-tin->GetV2()[1];
  float zmin   = 1532 + -50*zwidth - zwidth/2.;
  float zmax   = 1532 + 50*zwidth + zwidth/2.;

  cout << "Longitudinal binning: nBins    = " << zbins << endl;
  cout << "Longitudinal binning: binWidth = " << zwidth << endl;
  cout << "Longitudinal binning: minZ   = " << zmin << endl;
  cout << "Longitudinal binning: maxZ   = " << zmax << endl;

  /* determine binning in radial direction
   */
  TH1F* htemp_radpos = new TH1F("htemp_radpos","",1001,-0.5,1000.5);
  tin->Draw("_N_AhcProfileProcessor_radialPositionCog >> htemp_radpos");
  int rbins = htemp_radpos->GetMean() - 2; // -2 because of over and underflow bins

  tin->Draw("AhcProfileProcessor_radialPositionCog:AhcProfileProcessor_radialPositionCog","");

  float rwidth = tin->GetV2()[2]-tin->GetV2()[1];
  float rmin   = tin->GetV2()[1] - rwidth/2.;
  float rmax   = tin->GetV2()[rbins] + rwidth/2.;

  cout << "Radial binning: nBins    = " << rbins << endl;
  cout << "Radial binning: binWidth = " << rwidth << endl;
  cout << "Radial binning: minR   = " << rmin << endl;
  cout << "Radial binning: maxR   = " << rmax << endl;


  /* create 3D profile
   */
  TH2F *h_E_sum  = new TH2F("h_E_sum","",  zbins, zmin, zmax, rbins, rmin, rmax);
  h_E_sum->Sumw2();

  h_E_sum->GetXaxis()->SetTitle("z [mm]");
  h_E_sum->GetXaxis()->SetNdivisions(204);
  h_E_sum->GetXaxis()->SetTitleOffset(2.2);
  h_E_sum->GetXaxis()->SetRangeUser(1532,2732);

  h_E_sum->GetYaxis()->SetTitle("r [mm]");
  h_E_sum->GetYaxis()->SetNdivisions(304);
  h_E_sum->GetYaxis()->SetTitleOffset(1.5);
  h_E_sum->GetYaxis()->SetRangeUser(0,300);

  h_E_sum->GetZaxis()->SetTitle("#rho #left[MIP / mm^{2}#right]");
  h_E_sum->GetZaxis()->SetTitleOffset(1.5);
  h_E_sum->GetZaxis()->SetNdivisions(203);

  TH2F *h_N_sum  = (TH2F*)h_E_sum->Clone("h_N_sum");
  h_N_sum->Sumw2();

  tin->Draw("AhcProfileProcessor_2DradialPositionCog:AhcProfileProcessor_2DlongitudinalPositionCog >> h_E_sum",(cut_beam && cut_data_1 && cut_data_2) * "AhcProfileProcessor_2DEnergyCog");
  tin->Draw("AhcProfileProcessor_2DradialPositionCog:AhcProfileProcessor_2DlongitudinalPositionCog >> h_N_sum",(cut_beam && cut_data_1 && cut_data_2) * "AhcProfileProcessor_2DNormCog");

  TH2F *h_E_mean  = (TH2F*)h_E_sum->Clone("h_E_mean");
  h_E_mean->Sumw2();
  h_E_mean->Divide( h_N_sum );


  /* cleanup
   */
  ctemp->Close();


  /* draw profiles
   */
  TCanvas *c1 = new TCanvas();
  c1->SetLogz(1);
  c1->SetTheta(40.);
  c1->SetPhi(130.);
  h_E_mean->DrawCopy("lego2");

  TCanvas *c2 = new TCanvas();
  c2->SetRightMargin(0.25);
  c2->SetLogz(1);
  h_E_mean->GetXaxis()->SetTitleOffset(1.5);
  h_E_mean->GetZaxis()->SetTitleOffset(1.7);
  h_E_mean->DrawCopy("colz");


  /* print info
   */
  cout << "z   = Position along nominal beam axis in global CALICE coordinate sysytem. The AHCAL front face is at z = 1532 mm." << endl;
  cout << "r   = Radial position with respect to radial center of gravity (in x- and y-direction) in the AHCAL." << endl;
  cout << "rho = Mean energy density in given bin." << endl;


  return 1;

}
