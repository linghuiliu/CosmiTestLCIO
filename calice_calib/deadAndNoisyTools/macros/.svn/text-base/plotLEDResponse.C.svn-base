/**
 * This macro plots the pedestal corrected mean of the LED data of a physics run.
 * It need the two ahcBinHSt output files runnumber.root and runnumber_o1.root.
 * In addition it needs the config file (currently hard coded), as it creates the 
 * SpectrumPropertiesRunInfo objects on the fly. (Change this for speed?)
 * 
 */

//gSystem->Load("libDeadAndNoisyTools.so");


void plotLEDResponse(int runNumber)
{

  // append the _o1 to the run number
  std::stringstream ledFileName;
  ledFileName << runNumber << "_o1.root";
  
  SpectrumPropertiesRunInfo priPMNoise(runNumber,"AHC_CERN_SPS_June2011.cfg",true);
  SpectrumPropertiesRunInfo priPMLED(runNumber,"AHC_CERN_SPS_June2011.cfg",true,ledFileName.str().c_str());

  RunComparator myRunComparator( priPMNoise, priPMLED);

  myRunComparator.prepareGraph( &RunComparator::calculateChannelAndPedestalCorrectedMean );
  TGraph * allChannelsGraph = dynamic_cast<TGraph*>(myRunComparator.getGraph()->Clone());

  std::vector< SpectrumPropertiesRunInfo::ChannelID > deadChannels =
    priPMNoise.getChannelsWithRMSCut( 0, 20.5 );
  myRunComparator.prepareGraph( &RunComparator::calculateChannelAndPedestalCorrectedMean, 
				deadChannels);
  TGraph * deadChannelsGraph = dynamic_cast<TGraph*>(myRunComparator.getGraph()->Clone());
  
  std::vector< SpectrumPropertiesRunInfo::ChannelID > noisyChannels = 
    priPMNoise.getChannelsWithRMSCut( 130 );
  myRunComparator.prepareGraph( &RunComparator::calculateChannelAndPedestalCorrectedMean, 
				noisyChannels);
  TGraph * noisyChannelsGraph = dynamic_cast<TGraph*>(myRunComparator.getGraph()->Clone());

  deadChannelsGraph->SetMarkerColor(kRed);
  deadChannelsGraph->SetMarkerStyle(2);
  
  noisyChannelsGraph->SetMarkerColor(kBlue);
  noisyChannelsGraph->SetMarkerStyle(3);

  std::stringstream title;
  title << "Pedestal corrected LED response, run "<< runNumber;
  allChannelsGraph->SetTitle(title.str().c_str());
  allChannelsGraph->GetXaxis()->SetTitle("Global channel number");
  allChannelsGraph->GetYaxis()->SetTitle("Mean(LED) - mean(PMNoise) [ADC counts]");
  

  allChannelsGraph->Draw("AP");
  deadChannelsGraph->Draw("PSame");
  noisyChannelsGraph->Draw("PSame");
  
}
