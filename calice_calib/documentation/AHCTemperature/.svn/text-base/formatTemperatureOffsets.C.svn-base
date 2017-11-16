//#define PRINTME 0

void formatTemperatureOffsets()
{ 
  //   /*INPUT*/
  //   const char *fileHardware = "slowControlOffsets_FNAL2008_until_run_360828.txt";
  //   const char *fileOffline  = "offlineOffsets-15-Feb-2011-08-48.txt";
  //   /*OUTPUT*/
  //   const char *fileHardwareForDB = "db_offsetHardware-from15Feb-until_run_360828.txt";
  //   const char *fileOfflineForDB  = "db_offsetOffline-from15Feb-until_run_360828.txt";
  //   const char *fileDiffForDB     = "db_offsetDiff-from15Feb-until_run_360828.txt";
  /*==============================================================================*/
  
  
//   /*INPUT*/
//   const char *fileHardware = "slowControlOffsets_CERN2007_from_run_360829.txt";
//   const char *fileOffline  = "offlineOffsets-15-Feb-2011-08-48.txt";
  
//   /*OUTPUT*/
//   const char *fileHardwareForDB = "db_offsetHardware-from15Feb-from_run_360829.txt";
//   const char *fileOfflineForDB  = "db_offsetOffline-from15Feb-from_run_360829.txt";
//   const char *fileDiffForDB     = "db_offsetDiff-from15Feb-from_run_360829.txt";
  /*==============================================================================*/


  const char *fileHardware = "slowControlOffsets_FNAL2008_until_run_360828.txt";
  const char *fileOffline  = "offlineOffsets-15-Feb-2011-08-48-Run360309-360310.txt";
  
  /*OUTPUT*/
  const char *fileHardwareForDB = "db_offsetHardware-from15Feb-Run360309-360310";
  const char *fileOfflineForDB  = "db_offsetOffline-from15Feb-Run360309-360310.txt";
  const char *fileDiffForDB     = "db_offsetDiff-from15Feb-Run360309-360310.txt";

  cout<<"After input files"<<endl;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  //gROOT->ProcessLine(".L ~/RootStuff/CaliceStyle.C");
  //CaliceStyle();
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/  
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    read the hardware offsets
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  ifstream fileHardwareOffsets(fileHardware);
  if (!fileHardwareOffsets.is_open())
    {
      cout<<"\nSorry, could not open input file "<<fileHardware<<endl;
      exit(1);
    }

  std::vector<float> offsetCMBVec_hardware;
  std::vector<float> offsetTempSensVec_hardware;
  std::vector<int>   moduleVec_hardware;
  ReadFile(fileHardwareOffsets, offsetCMBVec_hardware, offsetTempSensVec_hardware, moduleVec_hardware);

  cout<<"\n read hardware input file"<<endl;
  /*
  for (unsigned int i = 0; i < offsetCMBVec_hardware.size(); ++i)
  cout<<"i: "<<i<<" CMB offset: "<<offsetCMBVec_hardware[i]<<" sensor offset: "<<offsetTempSensVec_hardware[i]<<endl;
  */

  std::vector<float> countsVec;
  for (unsigned int i = 0; i < offsetTempSensVec_hardware.size(); ++i)
    countsVec.push_back(i);


  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    read the correction offsets (calculated offline)
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  ifstream fileOfflineOffsets(fileOffline);
  if (!fileOfflineOffsets.is_open())
    {
      cout<<"\nSorry, could not open input file "<<fileOffline<<endl;
      exit(1);
    }

  std::vector<float> offsetCMBVec_offline;
  std::vector<float> offsetTempSensVec_offline;
  std::vector<int>   moduleVec_offline;
  ReadFile(fileOfflineOffsets, offsetCMBVec_offline, offsetTempSensVec_offline, moduleVec_offline);

  cout<<"\n read offline input file"<<endl;
  
  /*
    for (unsigned int i = 0; i < offsetCMBVec_offline.size(); ++i)
    cout<<"i: "<<i<<" CMB offset: "<<offsetCMBVec_offline[i]<<" sensor offset: "<<offsetTempSensVec_offline[i]<<endl;
  */

  std::vector<float> countsVec_offline;
  for (unsigned int i = 0; i < offsetTempSensVec_offline.size(); ++i)
    countsVec_offline.push_back(i);




  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    draw the offsets
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  cout<<"\n start to draw"<<endl;
  gROOT->SetStyle("Plain");

  TCanvas *canv = new TCanvas("canv", "canv");
  //canv->Divide(2, 2);
  
  canv->cd(1);
  DrawAllOffsets(countsVec, offsetTempSensVec_hardware, "Temperature sensors", "Hardware offsets", "HARDWARE offsets");
  gPad->SetGridx();
  gPad->SetGridy();
  
  canv->cd(2);
  DrawSeparateTempOffsets(offsetTempSensVec_hardware, "HCAL layers", "Temperature offsets", "HARDWARE offsets");

  /*---------------------------------------------------------------------------*/
  canv->cd(3);
  DrawAllOffsets(countsVec_offline, offsetTempSensVec_offline, "Temperature sensors", "Offline offsets", "OFFLINE offsets");
  gPad->SetGridx();
  gPad->SetGridy();

  canv->cd(4);
  DrawSeparateTempOffsets(offsetTempSensVec_offline, "HCAL layers", "Temperature offsets", "OFFLINE offsets");


  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    write the offsets
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  cout<<"\n 1) Write file "<<fileHardwareForDB<<endl;
  std::vector<int>   statusVec_hardware;
  WriteFileForDB(moduleVec_hardware, offsetTempSensVec_hardware, statusVec_hardware, fileHardwareForDB);

  cout<<"\n 2) Write file "<<fileOfflineForDB<<endl;
  std::vector<int>   statusVec_offline;
  WriteFileForDB(moduleVec_offline, offsetTempSensVec_offline, statusVec_offline, fileOfflineForDB);

  cout<<"\n 3) Write file "<<fileDiffForDB<<endl;
  WriteDiffFileForDB(moduleVec_offline, offsetTempSensVec_offline, 
		     statusVec_offline,
		     moduleVec_hardware, offsetTempSensVec_hardware,
		     fileDiffForDB);


  

}
/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Write output file for the data base                                   */
/*                                                                        */
/*------------------------------------------------------------------------*/
void WriteDiffFileForDB(std::vector<int> &moduleVec_offline, std::vector<float> &offsetsVec_offline, std::vector<int> &statusVec_offline,
			std::vector<int> &moduleVec_hardware, std::vector<float> &offsetsVec_hardware, 
			const char *outputFileName)
{
  if (moduleVec_offline.size() != moduleVec_hardware.size()
      || offsetsVec_offline.size() != offsetsVec_hardware.size())
    {
      cout<<"ERROR, wrong sizes of input vectors"<<endl;
      cout<<" moduleVec_offline.size="<<moduleVec_offline.size()<<", moduleVec_hardware.size="<<moduleVec_hardware.size()<<endl;
      cout<<" offsetsVec_offline.size="<<offsetsVec_offline.size()<<" offsetsVec_hardware.size="<<offsetsVec_hardware.size()<<endl;
      return;
    }

  for (unsigned i = 0; i < moduleVec_offline.size(); ++i)
    {
      if (moduleVec_offline[i] != moduleVec_hardware[i])
	{
	  cout<<"ERROR, not the same module number in HARDWARE and OFFLINE"<<endl;
	  return;
	}
    }

  /*___________________________________________________________________*/
  std::vector<int> moduleVec;
  std::vector<int> chipVec;
  std::vector<int> channelVec;
  
  FillModuleChipChannelForTemperature(moduleVec, chipVec, channelVec);
  /*___________________________________________________________________*/


  int countSuspicious = 0;
  float error = 0;

  ofstream fileOut (outputFileName);
  if (fileOut.is_open())
    {
      for (unsigned int i = 0; i < moduleVec_hardware.size(); ++i)
	{
	  float diff = offsetsVec_hardware[i] - offsetsVec_offline[i];

	  fileOut << moduleVec[i]<<" "<<chipVec[i]<<" "<<channelVec[i]<<" "
		  <<diff<<" "<<error<<" "<<statusVec_offline[i]
		  <<endl;
#ifdef PRINTME
	  cout<<"DIFF OUT i: "<<i<<" module="<<moduleVec[i]<<" chip="<<chipVec[i]<<" chan="<<channelVec[i]
	      <<" offset="<<diff<<" error="<<error<<" status="<<statusVec_offline[i]<<endl;
#endif
	}

    }
  
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Write output file for the data base                                   */
/*                                                                        */
/*------------------------------------------------------------------------*/
void WriteFileForDB(std::vector<int> &moduleVec, 
		    std::vector<float> &offsetsVec, 
		    std::vector<int> &statusVec,
		    const char *outputFileName)
{  
  if (moduleVec.size() != offsetsVec.size()) 
    {
      cout<<"\n Something is wrong, less offsets then expected"<<endl;
      return;
    }
  
  int countSuspicious = 0;

  //std::vector<int> vecModule;
  std::vector<int> chipVec;
  std::vector<int> channelVec;
  
  FillModuleChipChannelForTemperature(moduleVec, chipVec, channelVec);
 

  float error = 0;
  int status  = -999;

  std::cout<<" \n size="<<offsetsVec.size()<<endl;
  /*
  for (unsigned int i = 0; i < moduleVec.size(); ++i)
    cout<<"      i="<<i<<" module="<<moduleVec[i]<<endl;
  */


  ofstream fileOut (outputFileName);
  if (fileOut.is_open())
    {
      for (unsigned int i = 0; i < offsetsVec.size(); ++i)
	{
	  /*this will be changed to: if (offsetsVec[i] = -999)*/
	  // if (offsetsVec[i] < 6.5 && offsetsVec[i] > -6.5)
	  if (offsetsVec[i] != -9999)
	    status = 1;
	  else
	    {
	      status = 0;
	      /*
	      cout<<" Warning, suspicious offset of "<<offsetsVec[i]
	      <<" for module/chip/channel "<<moduleVec[i]<<"/"<<chipVec[i]<<"/"<<channelVec[i]
	      <<endl;
	      */
	      countSuspicious++;
	    }

	  statusVec.push_back(status);

	  fileOut << moduleVec[i]<<" "<<chipVec[i]<<" "<<channelVec[i]<<" "
		  <<offsetsVec[i]<<" "<<error<<" "<<status
		  <<endl;
#ifdef PRINTME
	  cout<<"OUT i: "<<i<<" module="<<moduleVec[i]<<" chip="<<chipVec[i]<<" chan="<<channelVec[i]
	      <<" offset="<<offsetsVec[i]<<" error="<<error<<" status="<<status<<endl;
#endif
	}

      /*don't forget to close the file*/
      fileOut.close();
    }
  else cout << "Unable to open file "<<fileOut;
  

  cout<<"Found "<<countSuspicious<<" suspicious offsets"<<endl;
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Draw graph with all offsets                                           */
/*                                                                        */
/*------------------------------------------------------------------------*/
void DrawAllOffsets(std::vector<float> &countsVec, std::vector<float> &offsetsVec, TString labelX, TString labelY, TString title="")
{
  if (countsVec.size() == 0) 
    {
      cout<<"\nSorry, vector in DrawAllOffsets is empty"<<endl;
      exit(1);
    }

  TGraph *grHardwareOffsets = new TGraph(countsVec.size(), &countsVec[0], &offsetsVec[0]);
  grHardwareOffsets->SetMarkerStyle(kOpenCircle);
  grHardwareOffsets->SetMarkerColor(kRed);
  grHardwareOffsets->Draw("AP");
  grHardwareOffsets->SetTitle(title);
  grHardwareOffsets->GetXaxis()->SetTitle(labelX);
  grHardwareOffsets->GetYaxis()->SetTitle(labelY);
 
}
/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Draw graph with separate temperature offsets                          */
/*                                                                        */
/*------------------------------------------------------------------------*/
void DrawSeparateTempOffsets(std::vector<float> &offsetsVec, TString labelX, TString labelY, TString title="")
{
  if (offsetsVec.size() == 0) 
    {
      cout<<"\nSorry, vector in DrawSeparateTempOffsets is empty"<<endl;
      exit(1);
    }

  const int numTemperatureSensors = 5;
  const int maxLayers = offsetsVec.size() / numTemperatureSensors;

  float t1_hardware[maxLayers] = {0};
  float t2_hardware[maxLayers] = {0};
  float t3_hardware[maxLayers] = {0};
  float t4_hardware[maxLayers] = {0};
  float t5_hardware[maxLayers] = {0};

  float countsTemp[maxLayers] = {0};
  //cout<<"x2: size="<<offsetsVec.size()<<endl;
  
  for (unsigned int i = 0; i < offsetsVec.size(); ++i)
    {
      int t1_index = (i/numTemperatureSensors) * numTemperatureSensors;
      //cout<<"i: "<<i<<" t1_index="<<t1_index<<" i/numTemperatureSensors="<<i/numTemperatureSensors<<endl;
      t1_hardware[i/numTemperatureSensors] = offsetsVec[t1_index];

      int t2_index = (i/numTemperatureSensors) * numTemperatureSensors + 1;
      t2_hardware[i/numTemperatureSensors] = offsetsVec[t2_index];

      int t3_index = (i/numTemperatureSensors) * numTemperatureSensors + 2;
      t3_hardware[i/numTemperatureSensors] = offsetsVec[t3_index];

      int t4_index = (i/numTemperatureSensors) * numTemperatureSensors + 3;
      t4_hardware[i/numTemperatureSensors] = offsetsVec[t4_index];

      int t5_index = (i/numTemperatureSensors) * numTemperatureSensors + 4;
      t5_hardware[i/numTemperatureSensors] = offsetsVec[t5_index];

      countsTemp[i/numTemperatureSensors] = i/numTemperatureSensors;
    }

  TGraph *gr_t1 = new TGraph(maxLayers, countsTemp, t1_hardware);
  //TGraph *gr_t1 = new TGraph(28, countsTemp, t1_hardware);
  gr_t1->SetLineColor(kBlue);
  gr_t1->Draw("AL");

  gr_t1->SetTitle(title);
  gr_t1->GetXaxis()->SetTitle(labelX);
  gr_t1->GetYaxis()->SetTitle(labelY);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraph *gr_t2 = new TGraph(maxLayers, countsTemp, t2_hardware);
  gr_t2->SetLineColor(kRed);
  gr_t2->SetLineWidth(2);
  gr_t2->Draw("L && same");

  TGraph *gr_t3 = new TGraph(maxLayers, countsTemp, t3_hardware);
  gr_t3->SetLineColor(kMagenta);
  gr_t3->SetLineStyle(2);
  gr_t3->SetLineWidth(3);
  gr_t3->Draw("L && same");

  TGraph *gr_t4 = new TGraph(maxLayers, countsTemp, t4_hardware);
  gr_t4->SetLineColor(kGreen+3);
  gr_t4->SetLineStyle(3);
  gr_t4->SetLineWidth(3);
  gr_t4->Draw("L && same");

  TGraph *gr_t5 = new TGraph(maxLayers, countsTemp, t5_hardware);
  gr_t5->SetLineColor(kOrange-7);
  gr_t5->SetLineStyle(4);
  gr_t5->SetLineWidth(3);
  gr_t5->Draw("L && same");

  Draw5Legend(gr_t1, gr_t2, gr_t3, gr_t4, gr_t5, 
	      "T^{o} sensor 1", "T^{o} sensor 2", "T^{o} sensor 3", "T^{o} sensor 4", "T^{o} sensor 5");

  gr_t1->GetYaxis()->SetRangeUser(-10, 15);

  //cout<<"x end"<<endl;

}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  In the file from PVSS:                                                */
/*  there are 7 values per module, where                                  */
/*  0, 1 is the offset for the temperature of the CMBs (monitoring boards)*/
/*  2, 3, 4, 5, 6 are the offsets for the 5 temperature sensors           */
/*                                                                        */
/*------------------------------------------------------------------------*/
void ReadFile(ifstream &inputFile, std::vector<float> &offsetCMBVec, 
	      std::vector<float> &offsetTempSensVec,
	      std::vector<int> &moduleVec)
{
  int module  = -99;
  int channel = -99;
  float offset = 0;

  if (inputFile.is_open())
    {
      while (inputFile.good())
	{
	  inputFile >> module;
	  inputFile >> channel;
	  inputFile >> offset;
	  
	  //if (!inputFile.good()) break;
	  
	  if (channel < 2) offsetCMBVec.push_back(offset);
	  else 
	    {
	      offsetTempSensVec.push_back(offset);
	      moduleVec.push_back(module);
	    }
	}
      inputFile.close();
    }
  else
    {
      cout<<"\n Sorry, could not open input file"<<endl;
      exit(1);
    }
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Read old file, from                                                   */
/*  ./dumpCalib /cd_calice/Hcal/tempSensors "" 360828 sv                  */
/*                                                                        */
/*------------------------------------------------------------------------*/
void ReadFileOLDFORMAT(ifstream &inputFile, std::vector<float> &offsetCMBVec, std::vector<float> &offsetTempSensVec)
{
  int module  = -99;
  int channel = -99;
  int chip    = -99;
  float offset = 0;
  float error  = 0;
  int status   = 0;

  while (1)
    {
      inputFile >> module;
      inputFile >> chip;
      inputFile >> channel;
      inputFile >> offset;
      inputFile >> error;
      inputFile >> status;
      if (!inputFile.good()) break;

      //if (channel < 2) offsetCMBVec.push_back(offset);
      //else             
      offsetTempSensVec.push_back(offset);
    }
    
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Format as in:                                                         */
/* ./dumpCalib /cd_calice/Hcal/tempSensors "" 360828 sv                   */
/*                                                                        */
/*------------------------------------------------------------------------*/
void FillModuleChipChannelForTemperature(std::vector<int> &vecModule, 
					 std::vector<int> &vecChip,
					 std::vector<int> &vecChannel)
{
  int maxLayers = 38;
  int numTemperatureSensors = 5;

  int sensor  = -99;
  int module  = -99;
  int chip    = -99;
  int channel = 0;

  int tempSensorID = 0;


  /*------------------------------------------------------*/
  /*don't forget to clear the vectors*/
  vecModule.clear();
  vecChip.clear();
  vecChannel.clear();


  /*------------------------------------------------------*/
  /* Nasty, ugly, choose whatever bad adjective you want...
     
  The offsets of the temperature sensors are written to the data base
  with the SimpleValue, which uses the HcalTileIndex to encode the
  module/chip/channel

  whereas the temperature offsets are read in the AhcTempProvider::setCalibrations,
  where AhcTempSensorIndex is used to get the module and the sensor number.

  To clarify:
  AhcTempSensorIndex uses (from left to right) 8 bits for module and 8 bits for sensor
  HcalTileIndex      uses (from left to right) 16 bits for SiPM (not used here), 
                                               5 bits for channel, 5 bits for chip 
					       and 6 bits for module
   --------------------------------------------------------*/ 
  /*------------------------------------------------------*/
  const int HTI_MOD_MASK   = 0x0000003F;
  const int HTI_MOD_SHIFT  = 0;
  const int HTI_CHIP_MASK  = 0x000007C0;
  const int HTI_CHIP_SHIFT = 6;
  const int HTI_CHAN_MASK  = 0x0000F800;
  const int HTI_CHAN_SHIFT = 11;

  /*------------------------------------------------------*/
  //cout<<"\n\n FillModuleChipChannelForTemperature:"<<endl;

  for (int i = 0; i < maxLayers * numTemperatureSensors; ++i)
    {
      module = (i / numTemperatureSensors) + 1;
      sensor = i % numTemperatureSensors;

      int hti_module = sensor;
      int hti_channel = (module >> 3) & 0x1f;
      int hti_chip_partial = module & 7;
      int hti_chip = hti_chip_partial << 2;

      vecModule.push_back(hti_module);
      vecChip.push_back(hti_chip);
      vecChannel.push_back(hti_channel);
     
      /*this is just for printout, to check if the output from writeSimpleValue is the same as the my output
       */
      int hti_index = 0;
      hti_index = (hti_index & ~HTI_MOD_MASK)  | ((hti_module << HTI_MOD_SHIFT) & HTI_MOD_MASK);
      hti_index = (hti_index & ~HTI_CHIP_MASK) | ((hti_chip << HTI_CHIP_SHIFT) & HTI_CHIP_MASK);
      hti_index = (hti_index & ~HTI_CHAN_MASK) | ((hti_channel << HTI_CHAN_SHIFT) & HTI_CHAN_MASK);

#ifdef PRINTME
      cout<<"  To DB: i="<<i<<" module="<<hti_module<<" chip="<<hti_chip<<" chan="<<hti_channel<<" index="<<hti_index<<endl;
#endif
    }/*end loop over i*/


}
/***********************************************************************/
/*                                                                     */
/*       draw legend for 5 TGraph on the same plot                     */
/*                                                                     */
/***********************************************************************/
void Draw5Legend(TGraph *histo1, 
		 TGraph *histo2, 
		 TGraph* histo3,
		 TGraph* histo4,
		 TGraph* histo5,
		 const Char_t *label1, 
		 const Char_t *label2, 
		 const Char_t *label3, 
		 const Char_t *label4,
		 const Char_t *label5,
		 const Char_t *header="")
{
  Float_t max1 = histo1->GetMaximum();
  Float_t max2 = histo2->GetMaximum();
  Float_t max3 = histo3->GetMaximum();
  Float_t max4 = histo4->GetMaximum();
  Float_t max5 = histo5->GetMaximum();

  Float_t arr[5] = {max1, max2, max3, max4, max5};
  Float_t max = TMath::MaxElement(5, arr);

  //histo1->SetMaximum(max);
  histo1->SetMaximum(30);

  TLegend *legend = new TLegend(0.587644,0.650424,0.949713,0.951271,NULL,"brNDC");
  legend->SetTextAlign(22);
  legend->SetTextFont(42);
  if (header != "")  legend->SetHeader(header); 
  legend->SetTextFont(42);

  TLegendEntry* entry1 = legend->AddEntry(histo1,label1,"PL");  
  entry1->SetTextColor(histo1->GetLineColor());
  
  TLegendEntry* entry2 = legend->AddEntry(histo2,label2,"PL");
  entry2->SetTextColor(histo2->GetLineColor());

  TLegendEntry* entry3 = legend->AddEntry(histo3,label3,"PL");
  entry3->SetTextColor(histo3->GetLineColor());

  TLegendEntry* entry4 = legend->AddEntry(histo4,label4,"PL");
  entry4->SetTextColor(histo4->GetLineColor());

  TLegendEntry* entry5 = legend->AddEntry(histo5,label5,"PL");
  entry5->SetTextColor(histo5->GetLineColor());

  legend->SetFillColor(kWhite);
  legend->Draw();
  legend->SetBorderSize(1);
}
