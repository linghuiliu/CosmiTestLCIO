/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVAClassificationApplication                                      *
 *                                                                                *
 * This macro provides a simple example on how to use the trained classifiers     *
 * within an analysis module                                                      *
 **********************************************************************************/

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TStopwatch.h"

#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

using namespace TMVA;

void TMVAClassificationApplication( TString myMethodList = "" ) 
{   
   //---------------------------------------------------------------
   // default MVA methods to be trained + tested

   // this loads the library
   TMVA::Tools::Instance();

   std::map<std::string,int> Use;


   Use["BDT"] = 1;
   

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassificationApplication" << std::endl;

   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   //
   // create the Reader object
   //
   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    

   // create a set of variables and declare them to the reader
   // - the variable names must corresponds in name and type to 
   // those given in the weight file(s) that you use
   Float_t ahc_energySum;
   Float_t ahc_energy1Momentum;
   Float_t ahc_energy2Momentum;
   Float_t ahc_energy3Momentum;
   Float_t ahc_energySum5RatioEnergySum;
   Float_t ahc_energyDensity;
   Float_t ahc_R90_hits;
   Float_t ahc_N90ratioN;
   Float_t ahc_cellsMeanEnergy;
   Float_t ahc_layerMax;
   Float_t ahc_layerStart;
   Float_t ahc_layerMaxDiffStart;
   
   Int_t layerMax;
   Int_t layerStart;
   Int_t layerMaxDiffStart;

   Float_t responseData;
   Float_t energySumData;
   Float_t energy1MomentumData;
   Float_t energy2MomentumData;
   Float_t energy3MomentumData;
   Float_t energySum5RatioEnergySumData;
   Float_t energyDensityData;
   Float_t R90_hitsData;
   Float_t N90ratioNData;
   Float_t cellsMeanEnergyData;
   Int_t layerMaxData;
   Int_t layerStartData;
   Int_t layerMaxDiffStartData;
   
   reader->AddSpectator("ahc_energySum", &ahc_energySum);
   
   reader->AddVariable("ahc_energy1Momentum", &ahc_energy1Momentum);
   reader->AddVariable("ahc_energySum5RatioEnergySum", &ahc_energySum5RatioEnergySum);
   reader->AddVariable("ahc_energy2Momentum", &ahc_energy2Momentum);
   reader->AddVariable("log(ahc_energy3Momentum)", &ahc_energy3Momentum);
   reader->AddVariable("ahc_energyDensity", &ahc_energyDensity);  
   reader->AddVariable("ahc_R90_hits", &ahc_R90_hits);
   reader->AddVariable("ahc_N90ratioN", &ahc_N90ratioN);
   reader->AddVariable("ahc_cellsMeanEnergy", &ahc_cellsMeanEnergy);
   reader->AddVariable("ahc_layerMax", &ahc_layerMax);
   reader->AddVariable("ahc_layerStart", &ahc_layerStart);
   reader->AddVariable("ahc_layerMaxDiffStart", &ahc_layerMaxDiffStart);
   
   //
   // book the MVA methods
   //
   TString dir    = "weights/";
   TString prefix = "TMVAClassification";

   // book method(s)
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = it->first + " method";
         TString weightfile = dir + prefix + "_" + TString(it->first) + ".weights.xml";
         reader->BookMVA( methodName, weightfile ); 
      }
   }
   
 // load the signal event samples from ROOT trees
      TString fname1 = "/home/jzalieck/data/results/Data_Run_Electron_5GeV.root";
      

      if (gSystem->AccessPathName( fname1 ))  // file does not exist in local directory
         {cout << "Signal file do not exist" <<endl;}
     

      TFile *input1 = TFile::Open( fname1 );  
      std::cout << "--- TMVAClassificationApp       : Using input file 1: " << input1->GetName() << std::endl;
      TTree *signal     = (TTree*)input1->Get("variables");
      

   signal->SetBranchAddress("ahc_energySum", &ahc_energySum);
   
   signal->SetBranchAddress("ahc_energy1Momentum", &ahc_energy1Momentum);
   signal->SetBranchAddress("ahc_energySum5RatioEnergySum", &ahc_energySum5RatioEnergySum);
   signal->SetBranchAddress("ahc_energy2Momentum", &ahc_energy2Momentum);
   signal->SetBranchAddress("ahc_energy3Momentum", &ahc_energy3Momentum);
   signal->SetBranchAddress("ahc_energyDensity", &ahc_energyDensity);  
   signal->SetBranchAddress("ahc_R90_hits", &ahc_R90_hits);
   signal->SetBranchAddress("ahc_N90ratioN", &ahc_N90ratioN);
   signal->SetBranchAddress("ahc_cellsMeanEnergy", &ahc_cellsMeanEnergy);
   signal->SetBranchAddress("ahc_layerMax", &layerMax);
   signal->SetBranchAddress("ahc_layerStart", &layerStart);
   signal->SetBranchAddress("ahc_layerMaxDiffStart", &layerMaxDiffStart);

   Double_t response;
   Int_t arrIt;
   arrIt = signal->GetEntries();

   float *responseArr = new float [arrIt];
   float *energySumDataArr = new float [arrIt];
   float *energy1MomentumDataArr = new float [arrIt];
   float *energy2MomentumDataArr = new float [arrIt];
   float *energy3MomentumDataArr = new float [arrIt];
   float *energySum5RatioEnergySumDataArr = new float [arrIt];
   float *energyDensityDataArr = new float [arrIt];
   float *R90_hitsDataArr = new float [arrIt];
   float *N90ratioNDataArr = new float [arrIt];
   float *cellsMeanEnergyDataArr = new float [arrIt];
   float *layerMaxDataArr = new float [arrIt];
   float *layerStartDataArr = new float [arrIt];
   float *layerMaxDiffStartDataArr = new float [arrIt];
  
   for (Long64_t ievt=0; ievt<signal->GetEntries();ievt++) {

      if (ievt%1000 == 0){
         std::cout << "--- ... Processing event: " << ievt << std::endl;
      }

      signal->GetEntry(ievt);

      response = reader->EvaluateMVA("BDT method");
      responseArr[ievt] = response;
      energySumDataArr[ievt] = ahc_energySum;
      energy1MomentumDataArr[ievt] = ahc_energy1Momentum;
      energy2MomentumDataArr[ievt] = ahc_energy2Momentum;
      energy3MomentumDataArr[ievt] = ahc_energy3Momentum;
      energySum5RatioEnergySumDataArr[ievt] = ahc_energySum5RatioEnergySum;
      energyDensityDataArr[ievt] = ahc_energyDensity;
      R90_hitsDataArr[ievt] = ahc_R90_hits;
      N90ratioNDataArr[ievt] = ahc_N90ratioN;
      cellsMeanEnergyDataArr[ievt] = ahc_cellsMeanEnergy;
      layerMaxDataArr[ievt] = layerMax;
      layerStartDataArr[ievt] = layerStart;
      layerMaxDiffStartDataArr[ievt] = layerMaxDiffStart;

         
   }
   
   
      
        TFile *target = TFile::Open("TMVApp.root","RECREATE");
        TTree *_rootTree = new TTree("variables","analysis");
      
        _rootTree->Branch("ahc_response", &responseData, "responseData/F"); 
  	_rootTree->Branch("ahc_energySum", &energySumData, "energySumData/F"); 
  	_rootTree->Branch("ahc_energySum5RatioEnergySum", &energySum5RatioEnergySumData, "energySum5RatioEnergySumData/F"); 
  	_rootTree->Branch("ahc_energy1Momentum", &energy1MomentumData, "&energy1MomentumData/F");
  	_rootTree->Branch("ahc_energy2Momentum", &energy2MomentumData, "&energy2MomentumData/F");
  	_rootTree->Branch("ahc_energy3Momentum", &energy3MomentumData, "&energy3MomentumData/F");
  	_rootTree->Branch("ahc_energyDensity", &energyDensityData, "&energyDensityData/F");
  	_rootTree->Branch("ahc_R90_hits", &R90_hitsData, "&R90_hitsData/F"); 
  	_rootTree->Branch("ahc_N90ratioN", &N90ratioNData, "&N90ratioNData/F");
  	_rootTree->Branch("ahc_cellsMeanEnergy", &cellsMeanEnergyData, "&cellsMeanEnergyData/F"); 
  	_rootTree->Branch("ahc_layerMax", &layerMaxData, "&layerMaxData/I"); 
  	_rootTree->Branch("ahc_layerStart", &layerStartData, "&layerStartData/I");
  	_rootTree->Branch("ahc_layerMaxDiffStart", &layerMaxDiffStartData, "&layerMaxDiffStartData/I"); 

for (Long64_t ievt=0; ievt<signal->GetEntries();ievt++) 
{
      responseData = responseArr[ievt];
      energySumData = energySumDataArr[ievt];
      energy1MomentumData = energy1MomentumDataArr[ievt];
      energy2MomentumData = energy2MomentumDataArr[ievt];
      energy3MomentumData = energy3MomentumDataArr[ievt];
      energySum5RatioEnergySumData = energySum5RatioEnergySumDataArr[ievt];
      energyDensityData = energyDensityDataArr[ievt];
      R90_hitsData = R90_hitsDataArr[ievt];
      N90ratioNData = N90ratioNDataArr[ievt];
      cellsMeanEnergyData = cellsMeanEnergyDataArr[ievt];
      layerMaxData = layerMaxDataArr[ievt];
      layerStartData = layerStartDataArr[ievt];
      layerMaxDiffStartData = layerMaxDiffStartDataArr[ievt];
    
      _rootTree->Fill();
}

      _rootTree->Write();
      target->Close();
      delete target;

      delete []responseArr;
      delete []energySumDataArr;
      delete []energy1MomentumDataArr;
      delete []energy2MomentumDataArr;
      delete []energy3MomentumDataArr;
      delete []energySum5RatioEnergySumDataArr;
      delete []energyDensityDataArr;
      delete []R90_hitsDataArr;
      delete []N90ratioNDataArr;
      delete []cellsMeanEnergyDataArr;
      delete []layerMaxDataArr;
      delete []layerStartDataArr;
      delete []layerMaxDiffStartDataArr;    
       

   std::cout << "--- Created root file: \"TMVApp.root\" containing the MVA output histograms" << std::endl;
  
   delete reader;
    
   std::cout << "==> TMVAClassificationApplication is done!" << endl << std::endl;
} 
