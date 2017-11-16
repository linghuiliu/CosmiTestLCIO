#include<iostream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TStyle.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TPaveStats.h"
#include <TList.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include "TH1F.h"

using namespace std;

//Function for drawing histograms------------------------------------------------------
void functionLegend(TH1F *hParticle1, TH1F *hParticle2, string Xlabel, string Ylabel)
{
hParticle1->SetLineColor(kRed);
hParticle2->SetLineColor(kBlue);
THStack * HistStack = new THStack();
HistStack->Add(hParticle1, "sames");
HistStack->Add(hParticle2, "sames");
HistStack->Draw("nostack");
TLegend *InfoLegend=new TLegend(.835474,.591936,.963398,.808642);
InfoLegend->AddEntry(hParticle1, "Pions"); 
InfoLegend->AddEntry(hParticle2, "Electrons");
InfoLegend->Draw();
HistStack->GetXaxis()->SetTitle(Xlabel.c_str());
HistStack->GetYaxis()->SetTitle(Ylabel.c_str());
TPaveStats *st1 = (TPaveStats*)hParticle1->GetListOfFunctions()->FindObject("stats");
TPaveStats *st2 = (TPaveStats*)hParticle2->GetListOfFunctions()->FindObject("stats");
st1->SetX1NDC(.5); st1->SetX2NDC(.7);
st2->SetX1NDC(.2); st2->SetX2NDC(.4); 
st1->SetOptStat(111111);
st2->SetOptStat(111111);
gPad->SetGridx();
gPad->SetGridy();
gPad->Modified();
}

int tmvaHistos()
{
gROOT->Reset();
gStyle->SetCanvasColor(10);
gStyle->SetPadBorderSize(0);
gStyle->SetPadColor(10);
gStyle->SetCanvasBorderMode(0);
gStyle->SetFrameBorderMode(0);
gStyle->SetOptFit(11111);

TCanvas *ct = new TCanvas("ct","Separation variables", 1300, 800);
TCanvas *ct2 = new TCanvas("ct2","Separation variables", 1300, 800);
ct->Divide(2,3);
ct2->Divide(2,3);


//Define histograms---------------------------------------------------
TH1F *hEnergySum5RatioEnergySum_Pion = new TH1F("hEnergySum5RatioEnergySum", "E_5layers over E_total", 200, 0.0, 1.0);
TH1F *hEnergySum5RatioEnergySum_Electron = new TH1F("hEnergySum5RatioEnergySum", "E_5layers over E_total", 200, 0.0, 1.0);

TH1F *hEnergy1Momentum_Pion = new TH1F("hEnergy1Momentum_Pion", "First energy momentum", 200, 0.0, 400.0);
TH1F *hEnergy1Momentum_Electron = new TH1F("hEnergy1Momentum_Electron", "First energy momentum", 200, 0.0, 400.0);

TH1F *hEnergy2Momentum_Pion = new TH1F("hEnergy2Momentum_Pion", "Second energy momentum", 200, 0.0, 400.0);
TH1F *hEnergy2Momentum_Electron = new TH1F("hEnergy2Momentum_Electron", "Second energy momentum", 200, 0.0, 400.0);

TH1F *hEnergy3Momentum_Pion = new TH1F("hEnergy3Momentum_Pion", "Third energy momentum", 200, 0.0, 400.0);
TH1F *hEnergy3Momentum_Electron = new TH1F("hEnergy3Momentum_Electron", "Third energy momentum", 200, 0.0, 400.0);

TH1F *hR90_Pion = new TH1F("hR90_Pion", "R at 90% of energy", 200, 0.0, 600.0);
TH1F *hR90_Electron = new TH1F("hR90_Electron", "R at 90% of energy", 200, 0.0, 600.0);

TH1F *hN90ratioN_Pion = new TH1F("hN90ratioN_Pion", "N90/N", 200, 0.95, 1.001);
TH1F *hN90ratioN_Electron = new TH1F("hN90ratioN_Electron", "N90/N", 200, 0.95, 1.001);

TH1F *hHitsMean_Pion = new TH1F("hHitsMean_Pion", "Hits energy average value", 200, 0.0, 5.0);
TH1F *hHitsMean_Electron = new TH1F("hHitsMean_Electron", "Hits energy average value", 200, 0.0, 5.0);

TH1F *hLayerMax_Pion = new TH1F("hLayerMax_Pion", "Maximum energy loss layers", 41, 0, 40);
TH1F *hLayerMax_Electron = new TH1F("hLayerMax_Electron", "Maximum energy loss layers", 41, 0, 40);

TH1F *hLayerStart_Pion = new TH1F("hLayerStart_Pion", "Shower start layers", 41, 0, 40);
TH1F *hLayerStart_Electron = new TH1F("hLayerStart_Electron", "Shower start layers", 41, 0, 40);

TH1F *hLayerMaxDiffStart_Pion = new TH1F("hLayerMaxDiffStart_Pion", "L_max-L_start", 41, 0, 40);
TH1F *hLayerMaxDiffStart_Electron = new TH1F("hLayerMaxDiffStart_Electron", "L_max-L_start", 41, 0, 40);

TH1F *hEnergyDensity_Pion = new TH1F("hEnergyDensity_Pion", "L_max-L_start", 200, 0.0, 1.0);
TH1F *hEnergyDensity_Electron = new TH1F("hEnergyDensity_Electron", "L_max-L_start", 200, 0.0, 1.0);

TH1F *hEnergySum_Pion = new TH1F("hEnergySum_Pion", "Energy sum", 200, 0.0, 500.0);
TH1F *hEnergySum_Electron = new TH1F("hEnergySum_Electron", "Energy sum", 200, 0.0, 500.0);



//Open ROOT files----------------------------------------------
TFile f_Pion("Run_Pion_5GeV.root");
TFile f_Electron("Run_Electron_5GeV.root");

//Get trees in ROOT files-------------------------------------
TTree* variables_Pion = (TTree*)f_Pion.Get("variables");
TTree* variables_Electron = (TTree*)f_Electron.Get("variables");
Int_t nEntries_Pion = (Int_t)variables_Pion->GetEntries();
Int_t nEntries_Electron = (Int_t)variables_Electron->GetEntries();

//Define variables for signal(electrons) and background(pions)--
Float_t energySum5RatioEnergySum_Pion(0);
Float_t energySum5RatioEnergySum_Electron(0);
Float_t energy1Momentum_Pion(0);
Float_t energy1Momentum_Electron(0);
Float_t energy2Momentum_Pion(0);
Float_t energy2Momentum_Electron(0);
Float_t energy3Momentum_Pion(0);
Float_t energy3Momentum_Electron(0);
Float_t energyDensity_Pion(0);
Float_t energyDensity_Electron(0);
Float_t R90_Pion(0);
Float_t R90_Electron(0);
Float_t N90ratioN_Pion(0);
Float_t N90ratioN_Electron(0);
Float_t hitsMean_Pion(0);
Float_t hitsMean_Electron(0);
Int_t layerMax_Pion(0);
Int_t layerMax_Electron(0);
Int_t layerStart_Pion(0);
Int_t layerStart_Electron(0);
Int_t layerMaxDiffStart_Pion(0);
Int_t layerMaxDiffStart_Electron(0);
Float_t energySum_Pion(0);
Float_t energySum_Electron(0);



variables_Pion->SetBranchAddress("ahc_energySum5RatioEnergySum", &energySum5RatioEnergySum_Pion);
variables_Electron->SetBranchAddress("ahc_energySum5RatioEnergySum", &energySum5RatioEnergySum_Electron);
variables_Pion->SetBranchAddress("ahc_energy1Momentum", &energy1Momentum_Pion);
variables_Electron->SetBranchAddress("ahc_energy1Momentum", &energy1Momentum_Electron);
variables_Pion->SetBranchAddress("ahc_energy2Momentum", &energy2Momentum_Pion);
variables_Electron->SetBranchAddress("ahc_energy2Momentum", &energy2Momentum_Electron);
variables_Pion->SetBranchAddress("ahc_energy3Momentum", &energy3Momentum_Pion);
variables_Electron->SetBranchAddress("ahc_energy3Momentum", &energy3Momentum_Electron);
variables_Pion->SetBranchAddress("ahc_energyDensity", &energyDensity_Pion);
variables_Electron->SetBranchAddress("ahc_energyDensity", &energyDensity_Electron);
variables_Pion->SetBranchAddress("ahc_R90_hits", &R90_Pion);
variables_Electron->SetBranchAddress("ahc_R90_hits", &R90_Electron);
variables_Pion->SetBranchAddress("ahc_N90ratioN", &N90ratioN_Pion);
variables_Electron->SetBranchAddress("ahc_N90ratioN", &N90ratioN_Electron);
variables_Pion->SetBranchAddress("ahc_cellsMeanEnergy", &hitsMean_Pion);
variables_Electron->SetBranchAddress("ahc_cellsMeanEnergy", &hitsMean_Electron);
variables_Pion->SetBranchAddress("ahc_layerMax", &layerMax_Pion);
variables_Electron->SetBranchAddress("ahc_layerMax", &layerMax_Electron);
variables_Pion->SetBranchAddress("ahc_layerStart", &layerStart_Pion);
variables_Electron->SetBranchAddress("ahc_layerStart", &layerStart_Electron);
variables_Pion->SetBranchAddress("ahc_layerMaxDiffStart", &layerMaxDiffStart_Pion);
variables_Electron->SetBranchAddress("ahc_layerMaxDiffStart", &layerMaxDiffStart_Electron);
variables_Pion->SetBranchAddress("ahc_energySum", &energySum_Pion);
variables_Electron->SetBranchAddress("ahc_energySum", &energySum_Electron);

//Fill histograms---------------------------------------
for (int i = 0; i < nEntries_Pion; ++i)
{
variables_Pion->GetEntry(i);
hEnergySum5RatioEnergySum_Pion->Fill(energySum5RatioEnergySum_Pion);
hEnergy1Momentum_Pion->Fill(energy1Momentum_Pion);
hEnergy2Momentum_Pion->Fill(energy2Momentum_Pion);
hEnergy3Momentum_Pion->Fill(energy3Momentum_Pion);
hEnergyDensity_Pion->Fill(energyDensity_Pion);
hR90_Pion->Fill(R90_Pion);
hN90ratioN_Pion->Fill(N90ratioN_Pion);
hHitsMean_Pion->Fill(hitsMean_Pion);
hLayerMax_Pion->Fill(layerMax_Pion);
hLayerStart_Pion->Fill(layerStart_Pion);
hLayerMaxDiffStart_Pion->Fill(layerMaxDiffStart_Pion);
hEnergySum_Pion->Fill(energySum_Pion);
}
for (int i = 0; i < nEntries_Electron; ++i)
{
variables_Electron->GetEntry(i);
hEnergySum5RatioEnergySum_Electron->Fill(energySum5RatioEnergySum_Electron);
hEnergy1Momentum_Electron->Fill(energy1Momentum_Electron);
hEnergy2Momentum_Electron->Fill(energy2Momentum_Electron);
hEnergy3Momentum_Electron->Fill(energy3Momentum_Electron);
hEnergyDensity_Electron->Fill(energyDensity_Electron);
hR90_Electron->Fill(R90_Electron);
hN90ratioN_Electron->Fill(N90ratioN_Electron);
hHitsMean_Electron->Fill(hitsMean_Electron);
hLayerMax_Electron->Fill(layerMax_Electron);
hLayerStart_Electron->Fill(layerStart_Electron);
hLayerMaxDiffStart_Electron->Fill(layerMaxDiffStart_Electron);
hEnergySum_Electron->Fill(energySum_Electron);
}

//Plot histograms--------------------------------------------------
ct->cd(1);
functionLegend(hEnergySum5RatioEnergySum_Pion, hEnergySum5RatioEnergySum_Electron, "E_5/E_total", "Events");
ct->cd(2);
functionLegend(hEnergy1Momentum_Pion, hEnergy1Momentum_Electron, "E1_momentum", "Events");
ct->cd(3);
functionLegend(hEnergy2Momentum_Pion, hEnergy2Momentum_Electron, "E2_momentum", "Events");
ct->cd(4);
functionLegend(hEnergy3Momentum_Pion, hEnergy3Momentum_Electron, "E3_momentum", "Events");
ct->cd(5);
functionLegend(hR90_Pion, hR90_Electron, "R90", "Events");
ct->cd(6);
functionLegend(hN90ratioN_Pion, hN90ratioN_Electron, "N90/N", "Events");
ct2->cd(1);
functionLegend(hHitsMean_Pion, hHitsMean_Electron, "Cells energy average", "Events");
ct2->cd(2);
functionLegend(hLayerMax_Pion, hLayerMax_Electron, "Layer nr. with max E loss", "Events");
ct2->cd(3);
functionLegend(hLayerStart_Pion, hLayerStart_Electron, "Shower start layer", "Events");
ct2->cd(4);
functionLegend(hLayerMaxDiffStart_Pion, hLayerMaxDiffStart_Electron, "L_max-L_start", "Events");
ct2->cd(5);
functionLegend(hEnergyDensity_Pion, hEnergyDensity_Electron, "Energy density", "Events");
ct2->cd(6);
functionLegend(hEnergySum_Pion, hEnergySum_Electron, "Energy sum", "Events");

return(0);
}
