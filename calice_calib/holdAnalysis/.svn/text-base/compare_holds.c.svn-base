#include <sstream>
//this script is written by Janet Dietrich to compare old and new hold scan results per module
// 23rd June 2011  

void compare_holds(int max_modules=38) {
   
   gROOT->Reset();
   bool debug = false;
   gROOT->SetStyle("Plain");
   const float FIT_THRESHOLD = 200;
   const int MAXCHANNELS = 216;
   char FileName[255];
  
   char buffer[2048];
  
   TCanvas *holdscan_compare= new TCanvas("holdscan_compare","holdscan_compare");
   TCanvas *holdscan_comparea= new TCanvas("holdscan_comparea","holdscan_comparea");
   
   holdscan_compare->cd();
   TGraph *hold_values_diff = new TGraph(max_modules);
   TGraph *hold_values_Median_diff= new TGraph(max_modules);
   
   TGraphErrors *hold_values_old = new TGraphErrors(max_modules);
   TGraphErrors *hold_values_new= new TGraphErrors(max_modules);
   
      
   hold_values_diff->SetMarkerStyle(21);
   
   hold_values_diff->SetTitle("hold diff values per module");
   hold_values_diff->GetYaxis()->SetTitle("hold difference [ticks]");
   hold_values_diff->GetXaxis()->SetTitle("module");
   hold_values_diff->SetLineColor(2);
   hold_values_diff->SetMarkerColor(2);
     
   hold_values_Median_diff->SetMarkerStyle(21);
   hold_values_Median_diff->SetTitle("hold diff values per module");
   hold_values_Median_diff->GetYaxis()->SetTitle("hold difference [ticks]");
   hold_values_Median_diff->GetXaxis()->SetTitle("module");
  
   hold_values_old->SetMarkerStyle(21);
  
   hold_values_new->SetMarkerStyle(24);
   hold_values_new->SetMarkerColor(2);
   hold_values_new->SetLineColor(2);
   hold_values_new->SetTitle("hold values per module");
   hold_values_new->GetYaxis()->SetTitle("hold [ticks]");
   hold_values_new->GetXaxis()->SetTitle("module");
   hold_values_old->SetTitle("hold values per module");
   hold_values_old->GetYaxis()->SetTitle("hold [ticks]");
   hold_values_old->GetXaxis()->SetTitle("module");
  
  
  for(int mod=0;mod<max_modules;mod++){
  	// read the median values for every module from holdfit_module%d.dat files
  	
	sprintf(FileName,"holdfit_module%d.dat",mod+1); 
  	ifstream in_config;
  	in_config.open(FileName);
   	int chan=0;
   	Double_t fittedHolds[300];
   	Double_t fithold[300];
        int chip[700];
	int channel[700];
    	int max_line=0;
	while (in_config.good())  
    	{
   
     		in_config.getline(buffer, 500, '\n');
     		istrstream *instr = new istrstream(buffer);
     		*instr >>chip[chan]>>channel[chan]>>    fithold[chan];
     		max_line=max_line+1;
		chan++;
		
     	}
     
        if (debug) cout<< "mod="<<mod+1<<","<<max_line<< endl;
        double holdaverage =0;
        int writeOut = 0; 
      	// average hold for all channels for one module
	for(int chan=0;chan <max_line;chan++) {
         	holdaverage =holdaverage + (fithold[chan]/6.25);
         	fittedHolds[writeOut]=fithold[chan]/6.25;
         	
         	if (chan==MAXCHANNELS-1) {
			holdaverage=holdaverage/writeOut;
			
      		}
      		writeOut++;
    	}
  	// median

  	double median = TMath::Median(writeOut,fittedHolds,0,0);

  
  
     
   	////////////////////////////////////compare hold with ahc.cfg values//////////////////////////////////
   	
	char inputFileName_config[255];
   	// here is the old AHC.cfg file--> take care that u read the one that is used for the run u
	//analyse!
	sprintf(inputFileName_config,"/home/caliceon/AHC.cfg");
 
  	ifstream config;
  	config.open(inputFileName_config);

 
  	char buffer2[2048];
  
 
  	while (config.good())  
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
     
      
    		config.getline(buffer2, 256, '\n');
  
      		istrstream *str = new istrstream(buffer2);
      		*str >>slot>> fe >>item>> moduleNo>>stackPos >>cmbId >>cmbCanAdr>> cmbPinId >>Hold_ext>> Hold_CM_LED>> Hold_PM_LED>> Vcalib_CM>> Vcalib_PM;

        	if (moduleNo==mod+1){
	
	
	
   			double delta_hold=0;
			double delta_hold_media=0;
   			delta_hold=(holdaverage-Hold_ext);
			delta_hold_media=(median-Hold_ext);
			hold_values_new->SetPoint(mod,mod+1, median);
			hold_values_old->SetPoint(mod,mod+1, Hold_ext);
			hold_values_new->SetPointError(mod,1,TMath::Sqrt(median));
			hold_values_old->SetPointError(mod,1,TMath::Sqrt(Hold_ext));
			hold_values_diff->SetPoint(mod,mod+1, delta_hold);
			hold_values_Median_diff->SetPoint(mod,mod+1,delta_hold_media);
			
			
  		}
		
		
	}
     			
    }   			
    hold_values_Median_diff->GetYaxis()->SetTitle("new hold - old hold [ticks]");
    hold_values_Median_diff->GetXaxis()->SetTitle("module");
   
    
    hold_values_Median_diff->Draw("APL");
    
    if (debug) hold_values_diff->Draw("APsame");		
			
    TLegend *lega = new TLegend(0.81946309,0.795122,0.9557047,0.99);
    lega->SetBorderSize(0);
    lega->SetFillColor(kWhite);
    lega->AddEntry(hold_values_Median_diff,"diff default-new" , "l");
    if (debug) lega->AddEntry(hold_values_diff,"diff default- average new" , "l");
    lega->Draw("same");
    holdscan_compare->Print("diff_holds_modules.ps");
    holdscan_compare->SaveAs("diff_holds_modules.root");
   
   
    holdscan_comparea->cd();
    hold_values_new->Draw("AP");
    hold_values_new->GetYaxis()->SetTitle("hold [ticks]");
   hold_values_new->GetXaxis()->SetTitle("module");
   hold_values_old->SetTitle("hold values per module");
   hold_values_old->GetYaxis()->SetTitle("hold [ticks]");
   hold_values_old->GetXaxis()->SetTitle("module");
  
    hold_values_old->Draw("sameP");
    holdscan_comparea->Modified();
    TLegend *legb = new TLegend(0.71946309,0.85122,0.9557047,0.9525);
    //legb->SetBorderSize(0);
    legb->SetFillColor(kWhite);
    legb->AddEntry(hold_values_new,"new hold" , "l");
    legb->AddEntry(hold_values_old,"old hold" , "l");
    legb->Draw("same");
    holdscan_comparea->Print("holds_per_module.ps");
    holdscan_comparea->SaveAs("holds_per_module.root");
   		
}	
	
	
	
	
	
	
