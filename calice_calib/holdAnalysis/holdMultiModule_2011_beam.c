#include <sstream>
 /*=================================================================*/
  /*=================================================================*/
 
void holdMultiModule_2011_beam(int module,bool debug=false) 
{
  
  int eventsPerConfig=500;
  gROOT->Reset();

  const float FIT_THRESHOLD = 500;
  //const float FIT_THRESHOLD = 0;
  const unsigned scanwidth = 1110;
  const int MAXCHANNELS = 216;
  char inputFileName[255];
  sprintf(inputFileName,"hold_module%d.dat",module);
  
  const char outputSumFileName[255] = "summary.txt";
  const char outputFileName[255];
  sprintf(outputFileName, "holdfit_module%d.dat",module);
  
  const char title[255] = "multi channel holdscan";
  const char psFileName[255];
  const char rootfileName[255];
  
  sprintf(psFileName, "hold_module%d.ps",module);
  sprintf(rootfileName, "hold_module%d.root",module);
  
  ifstream in;
  in.open(inputFileName);

  ofstream out;
  out.open(outputFileName);

  ofstream outSum;
  outSum.open(outputSumFileName,ofstream::app);
  
  
  char buffer[5000];
  in.getline(buffer,sizeof(buffer));
  
  
  Double_t x[scanwidth],xErr[scanwidth],y[MAXCHANNELS][scanwidth],yErr[MAXCHANNELS][scanwidth],dummy;
  
  int i = 0;
  Double_t miny = 1e6;
  Double_t maxy = -1e6;
  Double_t hold[MAXCHANNELS],holdm[MAXCHANNELS],mean[MAXCHANNELS];
  
  for (int n = 0; n<MAXCHANNELS; n++) 
  {
      holdm[n] = -1e6;
      mean[n]=0;
  }
  
  while (in.good())  
  {
      in.getline(buffer,sizeof(buffer));
      istrstream *instr = new istrstream(buffer);
      *instr >> x[i];
      xErr[i]=0.0;
      int j=0;
      
      while (j < MAXCHANNELS)
      {
	  *instr >> y[j][i] >> yErr[j][i];
	  yErr[j][i]/=TMath::Sqrt(eventsPerConfig);
	  if (y[j][i] < miny) miny = y[j][i];
	  if (y[j][i] > maxy) maxy = y[j][i];
	  if (holdm[j] < y[j][i]) {holdm[j] = y[j][i];hold[j]=x[i];}
	  mean[j] += y[j][i];
	  if (debug) cout<<"hold[j]"<<hold[j]<<"holdm[j]"<<holdm[j]<<"mean[j]"<<mean[j]<<endl;
 	  // comment janet: hold[j] = x value= time; holdm[j]=y value = adc measurement; mean is defined for every channel; sums up y-axis values for these
	  //channels
	  j++;
	}
      
      i++;
      
  }
  in.close();
  
  for (int n = 0; n<j; n++) mean[n] /= i;
  
  for (int k(0);k<i;k++) x[k] *= 6.25;
  
  gStyle->SetTitleYOffset(1.4);
  
  
  
  
  
  
  
 //////////////////////////////////////////////// 
  
  TCanvas *c = new TCanvas("holdscanMulti","Holdscan");
  
  c->Divide(4,3);
  TGraph* gr[MAXCHANNELS];

  if (debug) cout<<"/n  A) Loop over "<<MAXCHANNELS<<" channels"<<endl;

  for (int chan=0;chan < MAXCHANNELS; chan++)
    {
      char graphTitle[512];
      sprintf(graphTitle,"holdscan chip %d chan %d", chan/18, chan%18);
      c->cd(chan/18+1);
      
      if (chan%18 == 0) TMultiGraph *mg = new TMultiGraph();

      gr[chan] = new TGraphErrors(i-1,x,y[chan],xErr,yErr[chan]);
      gr[chan]->Sort();
      gr[chan]->SetLineColor(chan%9+1);
      gr[chan]->SetMarkerStyle(21);
      gr[chan]->SetMarkerSize(0);
      gr[chan]->SetTitle(graphTitle);
      gr[chan]->SetMinimum(miny-0.1*(maxy-miny));
      gr[chan]->SetMaximum(maxy+0.1*(maxy-miny));
      gr[chan]->GetXaxis()->SetTitle("hold (ns)");
      gr[chan]->GetYaxis()->SetTitle("mean (channels)");
      

     // if (chan%18 == 0) gr[chan]->Draw("ALP");
      //else gr[chan]->Draw("sameLP");
      
      mg->Add(gr[chan],"l");
      if (debug) cout << chan/18 << " " << chan << "hold " << holdm[chan] << endl;
     
     if (((chan+1)%18) == 0) {
         mg->GetXaxis()->SetTitle("hold (ns)");
         mg->GetYaxis()->SetTitle("mean (channels)");
       	
	 mg->Draw("alsame");
      }
      else mg->Draw("al");
    }
  

  if (debug) cout<<"/n A) Loop over chips"<<endl;
  // print for every chip the 18 channels
  for (int chip=0;chip < 12;chip++)
    {
      TCanvas *c_chip = new TCanvas("holdscanMulti_chip","Holdscan_chip");
      const char chiprootfileName[255];
      const char chiprootfileNameps[255];
      
      if (debug) sprintf(chiprootfileName, "hold_chip%d_modul%d.root", chip,module);
      if (debug) sprintf(chiprootfileNameps, "hold_chip%d_modul%d.ps", chip,module);
      
      c_chip->cd();
      bool first_hist= true;
      TLegend *legall = new TLegend(0.71946309,0.495122,0.9557047,0.925);
      legall->SetBorderSize(0);
      legall->SetFillColor(kWhite);
      
      for (int chan= chip*18; chan < chip*18+18; chan++){
	
	gr[chan]->SetMarkerSize(0.8);
	if (first_hist ) gr[chan]->Draw("ALP");
	
	else gr[chan]->Draw("sameLP");
	first_hist=false;
			
	const char channelname[50];
	sprintf(channelname, "channel_%d",chan );
	legall->AddEntry(gr[chan],channelname , "l");
      }
      legall->Draw("same");
      if (debug) c_chip->SaveAs(chiprootfileName);
      if (debug) c_chip->Print(chiprootfileNameps);
      
    }
  
  
  if (debug) cout<<"/n  B) Loop over "<<MAXCHANNELS<<" channels"<<endl;
  // every module has 12 chips --> 216/18 = 12
  for (int chip=0;chip < MAXCHANNELS/18;chip++)
    {
      cout << "chip " << chip << ": mean ";
      Double_t average = 0;
      Double_t minhold = 1e6;
      Double_t maxhold = 0;
      int holds=0;
      for (int chan=0;chan < 18 && chan < (MAXCHANNELS-18*chip);chan++)
	{
	  if (mean[chip*18+chan] > FIT_THRESHOLD)
	  {
	      if (hold[chip*18+chan] < minhold) minhold = hold[chip*18+chan];
	      if (hold[chip*18+chan] > maxhold) maxhold = hold[chip*18+chan];
	      average += hold[chip*18+chan];
	      holds++;
	  }
	  else{
	    if (debug) std::cout<<"Something wrong with threshold"<<std::endl;
	    if (debug) std::cout<<"mean[chip*18+chan]"<<mean[chip*18+chan] <<std::endl;
	  }
	}
      if (holds >0) average /= holds;
      if (debug) cout << average << " min "<<minhold<<" max "<< maxhold<< " found "<<holds<<" holdtimes"<< endl;
    }
  
  Double_t pedestal[MAXCHANNELS];
  Double_t fithold[MAXCHANNELS];
  TGraph *hold_values = new TGraph(MAXCHANNELS);
  Double_t rms[MAXCHANNELS];
  Double_t amplitude[MAXCHANNELS];
  Double_t minAmplitude = 1e6;
  Double_t maxAmplitude = -1e6;
  Double_t minHold = 1e6;
  

  if (debug) cout<<"/n  C) Loop over "<<MAXCHANNELS<<" channels"<<endl;
  for (int chip=0;chip < MAXCHANNELS/18;chip++)
    {
      if (debug) cout << endl << "\n Fitting chip: " <<chip<< endl;
  
      TF1* fit[MAXCHANNELS];
      for (int chan=0;chan < MAXCHANNELS;chan++)
	{
	  //if (debug) cout <<"  chan="<<chan<<" mean[chan]="<<mean[chan]<<" FIT_THRESHOLD="<<FIT_THRESHOLD<<endl;
	  //	  if (holdm[chan]/mean[chan] > 1.5)

	  if (mean[chan] > FIT_THRESHOLD)
	    {
	      pedestal[chan] = (y[chan][0] + y[chan][1] + y[chan][2])/3.0;
	      
	      if (debug) std::cout<<"hold[chan]"<<hold[chan]<<endl;
	      if (debug) std::cout<<"y[chan][0]"<<y[chan][0]<<endl;
	      if (debug) std::cout<<"y[chan][1]"<<y[chan][1]<<endl;
	      if (debug) std::cout<<"y[chan][2]"<<y[chan][2]<<endl;
	      if (debug) std::cout<<"pedestal[chan]"<<pedestal[chan]<<endl;
	      
	      fit[chan] = new TF1("hold","gaus",(hold[chan]-3)*6.25,(hold[chan]+3)*6.25);
	      gr[chan]->Fit(fit[chan],"QRN");
	      
	      fit[chan] = new TF1("hold","gaus",fit[chan]->GetParameter(1)-50.0,fit[chan]->GetParameter(1)+100.);
	      gr[chan]->Fit(fit[chan],"QRN");
	      
	      fit[chan]->SetLineColor(15);
	      fit[chan]->SetLineWidth(2);
	      gr[chan]->SetMarkerSize(0);
	      int fitResult = gr[chan]->Fit(fit[chan],"QR+");
	     
	      fithold[chan] = fit[chan]->GetParameter(1);
	      if (TMath::IsNaN(fithold[chan])) fithold[chan]=0;
	      
	      if(debug) cout <<"fitResult="<<fitResult<<"fit[chan]->GetChisquare()/fit[chan]->GetNDF()"<<fit[chan]->GetChisquare()<<","<<fit[chan]->GetNDF()<<endl;
	      
	      if (debug) cout<<"chan: "<<chan<<"fithold[chan]="<<fithold[chan]<<endl;
	      
	      if (fithold[chan] <1){hold_values->SetPoint(chan, chan+1,0);}
	      
	      
	      else{
		hold_values->SetPoint(chan, chan+1,fithold[chan]/6.25); }
	      
	      amplitude[chan] = fit[chan]->GetParameter(0) - pedestal[chan];
	      if (minAmplitude > amplitude[chan]) minAmplitude = amplitude[chan];
	      if (maxAmplitude < amplitude[chan]) maxAmplitude = amplitude[chan];
	      rms[chan] = fit[chan]->GetParameter(2);
	      if (minHold > fithold[chan]) minHold = fithold[chan];
	      
	      if (debug) cout << "chip: " << chan/18 << " chan: " << chan%18
			      << " mean: " << fit[chan]->GetParameter(1)
			      << " RMS: " << fit[chan]->GetParameter(2) << endl;
	    }
	}
      
    }
  
  
  float holdaverage=0;
  Double_t fittedHolds[300];
  int writeOut = 0;

  if (debug) cout<<"/n  D) Loop over "<<MAXCHANNELS<<" channels"<<endl;
  for (int chan=0;chan < MAXCHANNELS;chan++)
    {
      if(debug) cout<<"chan: "<<chan<<endl;

      if (mean[chan] > FIT_THRESHOLD)
	{
	  if (debug)cout<<" mean[chan]: "<<mean[chan]<<" fithold[chan]: "<<fithold[chan]<<endl;
	  
	  
	  if (debug) cout << "chip: " << chan/18 << " chan: " << chan%18
			  << " found hold: "<< fithold[chan]/6.25<<" "  <<fithold[chan]
			  <<"  "<< "shift from min: " << fithold[chan]-minHold
			  << " relativ amplitude to min " << amplitude[chan]/minAmplitude << endl;
	  
	  
	  
	  
	  
	  if (debug)  cout<<"fithold[chan]+fit[chan]->GetParameter(2)/2="<<(fithold[chan]+fit[chan]->GetParameter(2)/2)/6.25<<endl;
	  out <<  chan/18 << " " << chan%18 << " " << fithold[chan]<< " "
	      <<fithold[chan]-minHold << " "<< maxAmplitude<< " "
	      << minAmplitude<<" " << amplitude[chan] << endl;
	  
	  // out <<  chan/18 << " " << chan%18 << " " << fithold[chan]-minHold << " "<< maxAmplitude<< " "<< minAmplitude<<" " << amplitude[chan] << endl;
	  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	  // attention-> inorder to avoid that some strange fits are not included we remove values were the hold is 1
	  if (fithold[chan]/6.25 <1) continue;
	  holdaverage = holdaverage + (fithold[chan]/6.25);
	  
	  if (debug)  std::cout << holdaverage << " " << fithold[chan] <<","<<(fithold[chan]/6.25) << std::endl;
	  fittedHolds[writeOut]=fithold[chan]/6.25;
	  
	  if (chan==MAXCHANNELS-1) {
	    holdaverage=holdaverage/writeOut;
	    
	  }
	  writeOut++;
	}
    }
  
  cout <<"writeOut="<<writeOut<<endl;
  
  
   double median = TMath::Median(writeOut,fittedHolds,0,0);
  
  if (holdaverage != 0 && writeOut != 0)
  {
     // outSum << module << " " << holdaverage/writeOut << " " << median << " " << writeOut << std::endl; 
     outSum << module << " " <<   median << " " << std::endl; 
     
  }
  else
  {
      cout<<"Sorry, I have a problem, holdaverage="<<holdaverage<<", writeOut="<<writeOut<<endl;
      cout<<"Don't know what to do, aborting..."<<endl;
      exit(1);
    }
 
  if (debug)   cout << "Median of hold: " << median <<endl;
 
  if (debug)  cout << writeOut << " alive channels found"<<endl;
  out.close();
  outSum.close();
  c->Print(psFileName);
  c->SaveAs(rootfileName);
  
   
  ////////////////////////////////////compare result of all channels///////////////////
  // this is added by janet
  
  TCanvas *holdscan_compare= new TCanvas("holdscan_compare","holdscan_compare");
  
  holdscan_compare->cd();
  // TGraph *hold_values = new TGraph(MAXCHANNELS);
  TGraph *line_average=new TGraph(MAXCHANNELS);
  TGraph *line_Median=new TGraph(MAXCHANNELS);
  TGraph *led=new TGraph(MAXCHANNELS);
  for(int chan=0;chan < MAXCHANNELS;chan++){
    
    
    //  hold_values->SetPoint(chan, chan+1,fithold[chan]/6.25);
    line_average->SetPoint(chan,chan+1,holdaverage);
    line_average->SetLineColor(3);
    line_Median->SetPoint(chan,chan+1,median);
    line_Median->SetLineWidth(1.8);
    line_Median->SetLineColor(2);
    line_Median->SetLineWidth(1.8);
  }
  
  hold_values->SetMarkerStyle(21);
  // hold_values->SetMarkerSize(0.9);
  const char tile[255];
  sprintf(title, "holds per channel for module%d",module);
  
  hold_values->SetTitle(title);
  hold_values->GetYaxis()->SetTitle("hold (ns)");
  hold_values->GetXaxis()->SetTitle("channel");
  hold_values->Draw("AP");
  
  
  line_average->Draw("same");
  line_Median->Draw("same");
  
  
  TLegend *lega = new TLegend(0.71946309,0.495122,0.9557047,0.925);
  lega->SetBorderSize(0);
  lega->SetFillColor(kWhite);
  lega->AddEntry(line_average,"average" , "l");
  lega->AddEntry(line_Median,"median" , "l");
  lega->Draw("same");
  if (debug) holdscan_compare->Print("comparison_of_holds.ps");
  if (debug) holdscan_compare->SaveAs("comparison_of_holds.root");
  
  char inputFileName_config[255];
  sprintf(inputFileName_config,"/home/caliceon/AHC.cfg");
  
  ifstream in_config;
  in_config.open(inputFileName_config);
  
  
  char buffer[2048];
  
  int a = 0;
  Double_t al[1100];
  
  while (in_config.good())
    {
      char item[200];
      float slot;
      float fe;
      int moduleNo;
      float stackPos;
      float  cmbId;
      float  cmbCanAdr;
      float  cmbPinId;
      float   Hold_ext;
      float  Hold_CM_LED;
      float  Hold_PM_LED; float  Vcalib_CM; float  Vcalib_PM;
      char line[256];
      
      
      in_config.getline(buffer, 256, '\n');
      
      istrstream *instr = new istrstream(buffer);
      //  *instr >> al[a];
      *instr >>slot>> fe >>item>> moduleNo>>stackPos >>cmbId >>cmbCanAdr
	     >> cmbPinId >>Hold_ext>> Hold_CM_LED>> Hold_PM_LED>> Vcalib_CM>> Vcalib_PM;
      
      
      if (debug)  cout<<"slot"<<slot<<","<<moduleNo<<","<<Hold_PM_LED<<endl;
      if (moduleNo==module){
	
	for(int chan=0;chan < MAXCHANNELS;chan++){
	  led->SetPoint(chan,chan+1,Hold_ext);
	  led->SetLineWidth(3);
	  led->SetLineColor(4);
	  led->SetLineWidth(1.8);
	  
	}
	
  	led->Draw("same");
      }
      
      
    }
  lega->AddEntry(led,"default value" , "l");
  lega->Draw("same");
  
  const char Name[255];
  const char Nameroot[255];
  const char Nameps[255];
  sprintf(Name, "comparison_of_holds_module%d.pdf",module);
  sprintf(Nameps, "comparison_of_holds_module%d.ps",module);
  sprintf(Nameroot, "comparison_of_holds_module%d.root",module);
  
  
  holdscan_compare->Print(Name);
  holdscan_compare->Print(Nameps);
  holdscan_compare->SaveAs(Nameroot);
 
}
