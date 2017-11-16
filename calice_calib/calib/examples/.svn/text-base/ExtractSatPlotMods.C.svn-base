//////////////////////////////////////////////////////////////////////////////////
// Jara Zalesak 2010/08/24                                                      //
// Main routine ExtractSatPlotMods(Module) displays saturation curves           //
// for both PM and CM modes of the ahcVcalibLedScan calibration data            //
// as the graphs taken from the root file for run # (Int_t Run) and             //
// number of fitted points (const Int_t NumFitPoints=10) from directory ./plots //
// for example: ./plots/206221-graphs.fit10points.s1_0.root                     //
// This file has been crated by running Marlin processor                        //
// ExtractSaturationCurveProcessor.cc                                           //
// For all plots at once you can use routine ExtractSatPlotAll()                //
// Run as a Root library root> .L ExtractSatPlotMods.C++                        //
//////////////////////////////////////////////////////////////////////////////////



#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <TFile.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>

#include <TPostScript.h>

const Int_t NoModules = 38;
const Int_t NoChips = 12;
const Int_t NoChannels = 18;
const Int_t Vmax = 65536;
const Double_t mez1 = 0.;
const Double_t mez2 = Vmax/2;

Double_t h_bmin = 27000.;
Double_t h_bmax = Vmax;
Int_t h_nbins; 



//const Int_t Run = 206220; // CM Vcalib 154 points backwards -v 5
const Int_t Run = 206221; // PM Vcalib 154 points backwards -v 5
const Int_t NumFitPoints = 10;

Int_t bChip = 0;
Int_t fChip = NoChips;

char gr_name[150];
int NoGrVcalib;
char hx_name[250];
Double_t x[NoChannels], ex[NoChannels], y[NoChannels], ey[NoChannels];

bool boolgr = false;
TGraphErrors* grN0[NoChips];
TCanvas *c1;

/////////////////////////////////////////////////////////////////////////////////////////
void ExtractSatPlotAll();

void ExtractSatPlotMods(Int_t Module){

  Int_t Chip;
  Int_t Channel;

  gStyle->SetOptFit(0);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);

  c1 = new TCanvas("c1","Graph1",5,10,800,600);

  TH2F* hpx;
  sprintf(hx_name,"Saturation - module%d, Run=%6d",Module, Run);

  if ( (Module<16) || (Module == 18) || (Module == 25) || (Module == 28)) {
    h_bmin = 40000.; 
  } else {
    h_bmin = 25000.; 
  }
  h_nbins = Int_t(h_bmax - h_bmin);

  hpx = new TH2F("hpx",hx_name,h_nbins,h_bmin,h_bmax,100,mez1,mez2);

  hpx->GetXaxis()->SetTitle("LED Vcalib [DAC bin]");
  hpx->GetYaxis()->SetTitle("Response [ADC bin]");
  hpx->Draw();
  hpx->SetStats(kFALSE);

  char f_name[250];
  sprintf(f_name,
	  "./plots/%06d-graphs.fit%dpoints.s1_0.root",Run,NumFitPoints);
  TFile f(f_name);
  cout <<"FILE opened:\n"
       << f_name
       << endl;

  int N_cut = 0;

  // Loop over chips in module
  for (Int_t iChip = 0; iChip < NoChips; iChip++) {
    Chip = iChip;
    cout<<"============================================================="<< endl;

  // Loop over channels
    for (Int_t iChan = 0; iChan < NoChannels; iChan++) {

      Double_t ParN0 = 0; Double_t ErrParN0 = 0;
      Double_t ParB = 0; Double_t ParC = 0; Double_t Slope = 0;

      Channel = iChan;

      sprintf(gr_name,"module%d_chip%d_channel%d",Module,Chip,Channel);

      TGraphErrors *gr = (TGraphErrors*)f.Get(gr_name);

      if (!gr) {
	boolgr = false; 
	cout <<"!!!!!!!!!!!!!!!!!!  NO GRAPH  !!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	continue;
      }
      boolgr = true;

      NoGrVcalib = (int)(gr -> GetN());
      cout <<" * Vcalibpoints: "<< NoGrVcalib <<endl;

      if ( NoGrVcalib == 0 ){
	boolgr = false; 
	cout <<"!!!!!!!!!!!!!!!!!!  NO GRAPH POINT  !!!!!!!!!!!!!!!!!!"<<endl;
	cout <<" * Vcalibpoints: "<< NoGrVcalib <<endl;
	continue;
      }

      Double_t* XVcalib = gr -> GetX();
      Double_t* YVcalib = gr -> GetY();
      Double_t* XErrVcalib = gr -> GetEX();
      Double_t* YErrVcalib = gr -> GetEY();

      Double_t Pedestal = YVcalib[0];
      Double_t PedErr   = YErrVcalib[0];
      Double_t AdcVmax  = YVcalib[NoGrVcalib-1];
      Double_t AdcVmaxErr  = YErrVcalib[NoGrVcalib-1];


      gr -> SetMarkerSize(0.3);

      if (iChan<6) {
        gr -> SetMarkerColor(2);
        gr -> SetMarkerStyle(20 + iChan);
      } else if (iChan<12) {
        gr -> SetMarkerColor(3);
        gr -> SetMarkerStyle(20 + iChan-6);
      } else {   
        gr -> SetMarkerColor(4);
        gr -> SetMarkerStyle(20 + iChan-12);
      }
      gr -> Draw("P");

      TF1 *fitFunction;

      if (gr->GetFunction("fitFunction")) {
	TF1 *fitFunction = gr->GetFunction("fitFunction");
	fitFunction = gr->GetFunction("fitFunction");

	fitFunction->SetLineWidth(1);
	fitFunction->SetLineColor(1+iChip);
	//	fitFunction->SetRange(0,65536);
	fitFunction->SetRange(65536,65536);
  
	ParN0 = fitFunction -> GetParameter(0);
	ErrParN0  = fitFunction -> GetParError(0);
	ParB  = fitFunction -> GetParameter(1);
	ParC  = fitFunction -> GetParameter(2);
      }

 
      if ( AdcVmax < 100000.) {
	//      if ( AdcVmax < 10000.) {
	N_cut++;
	//	fitFunction->SetRange(0,65536);
	gr -> Draw("P");

	printf("%2d %2d %2d %8.2f %8.2f\n",
	       Module, Chip, Channel,
	       AdcVmax, Pedestal
	       );
      }

    }   // loop over iChan
  }   // loop over iChip

  f.Close();

}




//////////////////////////////////////////////////////////////////////////////////

void ExtractSatPlotAll() {

  char psf[100];

  for (Int_t im=0; im<NoModules; im++) {
    sprintf(psf,"./test_M%02d.pdf",im+1);
    ExtractSatPlotMods(im+1);
    c1 -> Print(psf);
  }

}

