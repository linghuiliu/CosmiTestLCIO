///////////////////////////////////////////////////////////////// 
// author: Jara Zalesak 2010.07.16, update 2010.07.16          //
/////////////////////////////////////////////////////////////////

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
#include <TLine.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>

#include "Riostream.h"


const Int_t NoModules = 38;
const Int_t NoChips = 12;
const Int_t NoChannels = 18;
const Int_t Vmax = 65536;
const Int_t AdcSatBin = 65536/2-1;
const Int_t SiPM_pixels = 34*34;
const Double_t SiPM_fact = 0.8;
const Int_t NoChCh = NoChips * NoChannels;

struct _result{
  UShort_t u;
  Double_t IC;
  Double_t eIC;
  UInt_t   nru;
  UShort_t ok;
  Double_t min;
  Double_t max;
};


char run_name[6]; 
char ver_name[10]; 

const Int_t NoRuns = 3;
Int_t iRun = 0;

TCanvas *d1;

/////////////////////////////////////////////////////////////////////////
_result res[NoRuns][NoModules][NoChips][NoChannels];

void ReadSatFile(Int_t f_opt, Int_t run);
void PlotGraphMod(int mod);
void PlotGraphAll();

/////////////////////////////////////////////////////////////////////////


//////// Reading result's files
void ReadSatFile(Int_t f_opt, Int_t run) {
  char file_name[200]; 
  int m,a,c,nrun;
  float IC,eIC;
  float ftemp;
  float minIC,maxIC,maxWic,maxW;
  float pedPm,pedCm;

  FILE *file1;
  const Int_t line_char = 200;
  char line_str[line_char];

  Int_t No_rows_IC;

  cout << " Option is: "<<f_opt<<" and RUN number:"<<run<<endl;

  if          (run == 1) {
    No_rows_IC = 7546; // FNAL official IC constants
    sprintf(run_name,"500722");
  } else   if (run == 2) {  
    No_rows_IC = 7445; // CERN official IC constants
    sprintf(run_name,"330710");

  } else   if (run == 10) {  
    No_rows_IC = 7608;
    sprintf(run_name,"206221");

  } else {
    cout << " !!!!!!!!! WRONG CALIB RUN !!!!!!!!!!! "<<endl;
    return;
  }

  cout <<" run_name: "<<run_name<<endl;
  cout <<" No_rows_IC= "<<No_rows_IC<<endl;


  if (f_opt == 1) {
    sprintf(file_name,"./constants/dumpCalib.run500722.ic_constants.out");
  } else if (f_opt == 2) {
    sprintf(file_name,"./constants/dumpCalib.run330710.ic_constants.out");
  } else if (f_opt >= 3) {
    sprintf(file_name,"./inter/%s-intercalibration.s1_0.dat",run_name);
  } else {
    cout <<" !!!! NO correct OPTION !!!!  "<<f_opt<<endl;
    return;
  }

  file1=fopen(file_name,"r");
  if (!file1){
    cout<< "file "<<file_name<<" NOT open" <<endl;
  } else {
    cout<<"file opened: "<<file_name<<endl;

    for (int cc=0; cc < No_rows_IC; cc++) {
    

      if ( run < 3 ) {
	fgets (line_str, line_char, file1);
	sscanf(line_str,"%d %d %d %f %f",&m, &a, &c, &IC, &ftemp);
	res[iRun][m-1][a][c].IC  = Double_t(IC);
	res[iRun][m-1][a][c].eIC = 0.0;
	res[iRun][m-1][a][c].nru = 1;
	res[iRun][m-1][a][c].ok  = 1;

      } else if ( run < 999 ) {
	fgets (line_str, line_char, file1);
	sscanf(line_str,"%d %d %d %f %f %d %f %f %f %f %f %f",
	       &m, &a, &c, &IC, &eIC, &nrun, 
	       &minIC, &maxIC, &maxWic, &maxW,
	       &pedPm, &pedCm);
	res[iRun][m-1][a][c].IC  = Double_t(IC);
	res[iRun][m-1][a][c].eIC = Double_t(eIC);
	res[iRun][m-1][a][c].nru = UInt_t(nrun);
	res[iRun][m-1][a][c].ok  = 1;
	res[iRun][m-1][a][c].min  = Double_t(minIC);
	res[iRun][m-1][a][c].max  = Double_t(maxIC);

      }
    }
  }
  fclose(file1);

  cout <<" Reading the file DONE! "<<endl;
}


/////////////////////////////////////////////////////////////////////////


//////// PLoting result's graphs
void PlotGraphMod(int mod) {

  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);

  char hz_name[100];
  TGraphErrors* grIC;
  TGraphAsymmErrors* grICbound;
  TGraph* grFNAL;
  TGraph* grCERN;

  d1 = new TCanvas("d1","d1 hpz",45,35,800,600);
  TH2F* hpz;

  iRun = 0;
  ReadSatFile(1,1); // FNAL IC calibration DB

  iRun = 1;
  ReadSatFile(2,2); // CERN IC calibration DB

  iRun = 2;
  ReadSatFile(3,10); // Inter calibration runs PM/CM 206221/206220


  for (int ir = 0; ir<NoRuns; ir++){
    printf("M7:A4:C3:| IC=%7.3f +/-%7.3f | %4u\n",
	   res[ir][7-1][4][3].IC,res[ir][7-1][4][3].eIC,res[ir][7-1][4][3].nru);
    printf("M8:A4:C3:| IC=%7.3f +/-%7.3f | %4u\n",
	   res[ir][8-1][4][3].IC,res[ir][8-1][4][3].eIC,res[ir][8-1][4][3].nru);
  }

  sprintf(hz_name,"IC - module%d, Run=%s vs FNAL&CERN",mod, run_name);
  hpz = new TH2F("hpz",hz_name, NoChCh,-0.5,NoChCh-0.5, 100,0.,20.);
  hpz->GetXaxis()->SetTitle("18*chip + cell");
  hpz->GetYaxis()->SetTitle("IC_const");
  hpz->Draw();
  hpz->SetStats(kFALSE);

  double xx[NoChCh];
  double ex[NoChCh];
  double yy[NoChCh];
  double ey[NoChCh];
  double eyh[NoChCh];
  double eyl[NoChCh];

  double yfnal[NoChCh];
  double ycern[NoChCh];

  for (int ix = 0; ix < NoChCh; ix++) {
  
    ex[ix] = 0.5;
    xx[ix] = double(ix);
    yy[ix] = res[2][mod-1][ix/NoChannels][ix%NoChannels].IC;
    ey[ix] = res[2][mod-1][ix/NoChannels][ix%NoChannels].eIC;

    eyh[ix] = (res[2][mod-1][ix/NoChannels][ix%NoChannels].max - res[2][mod-1][ix/NoChannels][ix%NoChannels].IC);
    eyl[ix] = (res[2][mod-1][ix/NoChannels][ix%NoChannels].IC - res[2][mod-1][ix/NoChannels][ix%NoChannels].min);

    yfnal[ix] = res[0][mod-1][ix/NoChannels][ix%NoChannels].IC;
    ycern[ix] = res[1][mod-1][ix/NoChannels][ix%NoChannels].IC;

    cout
      <<ix <<"-"<< ix/NoChannels <<"-"<< ix%NoChannels 
      <<" IC="<<yy[ix] <<" eIC="<<ey[ix]
      << endl;
  }

  grIC = new TGraphErrors(NoChCh, xx, yy, ex, ey);
  grFNAL = new TGraph(NoChCh, xx, yfnal);
  grCERN = new TGraph(NoChCh, xx, ycern);
  grICbound = new TGraphAsymmErrors(NoChCh, xx, yy, ex, ex, eyl, eyh);

  grICbound -> SetFillColor(5);
  grICbound -> SetFillStyle(1001);
  grICbound -> Draw("2");

  grFNAL -> SetMarkerColor(2);
  grFNAL -> SetMarkerStyle(21);
  grFNAL -> Draw("P");

  grCERN -> SetMarkerColor(3);
  grCERN -> SetMarkerStyle(22);
  grCERN -> Draw("P");

  grIC -> SetMarkerColor(1);
  grIC -> SetMarkerStyle(20);
  grIC -> SetMarkerSize(0.5);
  grIC -> Draw("P");

}

/////////////////////////////////////////////////////////////////////////


//////// All graphs in once
void PlotGraphAll() {

  char mfile[100];

  for (int im = 0; im < NoModules; im++) {
    sprintf(mfile,"InterCalib.Mod%02d.pdf",im+1);
    PlotGraphMod(im+1); 
    d1->Print(mfile);
  }

}

/////////////////////////////////////////////////////////////////////////



