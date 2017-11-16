#include "SpectrumPropertiesRunInfo.h"
#include "ChannelExpertQuality.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <TGraph.h>
#include <TVector.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TTree.h>
#include <TDirectory.h> 
#include <TROOT.h>
#include <TFile.h>

void printUsage(void)
{
    std::cout << "Usage: createBadChannelsList {--ahcConfig ahc.cfg [--with5RMSCut] | --rootFile spriFile.root} "
	      << "[--runNumberOffset offset] [--deadThreshold deadThreshold] [--noisyThreshold noisyThreshold] run1 [run2] ..."  << std::endl;

    std::cout << "  This programme plots the number of dead, noisy and bad channels against the run number." << std::endl;

    std::cout << " It either takes a root file with all the SpectrumPropertiesRunInfo tree as input, or the output files "
	      << "of ahcBinHst (one file per run, called runnumber.root). For the second option you have to provide"
	      << " the ahcConfig file for the mapping. The SpectrumPropertiesRunInfo objects are created on the fly, which makes"
	      << " it slow compared to the version with the tree.";
    std::cout << std::endl;
    std::cout << "  Options:" << std::endl;
    std::cout << "  --ahcConfig / --rootFile\t One of the two options has to be given. They are mutually excusive."<< std::endl;
    std::cout << "  --with5RMSCut \t\t Use the iterative 5*RMS cut when filling the tree. Only useful when using --ahcCfg." << std::endl;
    std::cout << "  --runNumberOffset \t\t Offset for the x axis in the plot. Default 631000" << std::endl;
    std::cout << "  --deadThreshold \t\t Threshold on the RMS below which the channel is marked as dead. Default 20.5" << std::endl;
    std::cout << "  --noisyThreshold \t\t Threshold on the RMS above which the channel is marked as noisy. Default 140" << std::endl;
}

void printChannelStatusMap( std::map< int, int > const & channelStatusMap , std::ostream & s = std::cout )
{
  s << "# module chip channel status "<< std::endl;
  s << "# ATTENTION: Currenlty this is the expert status, which by chance is identical "<< std::endl;
  s << "# to the status written to the data base. "<< std::endl;
  for ( std::map< int, int >::const_iterator mapIter= channelStatusMap.begin();
	mapIter != channelStatusMap.end(); mapIter++)
  {
    // the first entry in channelStatusMap is the channel, the second one the status
    SpectrumPropertiesRunInfo::ChannelID channelID = SpectrumPropertiesRunInfo::getChannelID( mapIter->first );
    s << channelID._module << "\t"
      << channelID._chip << "\t"
      << channelID._channel << "\t"
      << "0x" << std::hex <<  mapIter->second << std::dec << std::endl;
    // the channel status is put out in hex, so the states can be seen manually
  }
  s << "#total number of bad channels: " <<  channelStatusMap.size() << std::endl;
}

int main(int argc, char* argv[])
{
  // check that there are arguments, otherwise give usage

  if (argc == 1) // no arguments given
  {
    printUsage();
    return 1;
  }

  // change this if you add new options
  static const unsigned int maxOptions = 5;

  // a variable of the first argument containig a run number
  size_t firstRunArgument = 1; 

  // the command line input variables
  bool use5RMSCut = false;
  std::string ahcConfigName;
  std::string inputRootFileName;
  int runNumberOffset = 361000; // offset for the Cern June 2011 run
  float deadThreshold = 20.5;
  float noisyThreshold = 140;
  
  for (unsigned int option(0); option < maxOptions; option++)
  {  
    // no more command line arguments, nothing to parse
    if (firstRunArgument == unsigned(argc))
    {
      break;
    }

    // check for the options 
    if ( std::string(argv[firstRunArgument]).compare("--ahcConfig") == 0 )
    {
      ahcConfigName =  std::string(argv[firstRunArgument+1]);
      firstRunArgument+=2;
    }
    else if( std::string(argv[firstRunArgument]).compare("--rootFile") == 0 )
    {
      inputRootFileName =  std::string(argv[firstRunArgument+1]);
      firstRunArgument+=2;      
    }
    else if( std::string(argv[firstRunArgument]).compare("--with5RMSCut") == 0 )
    {
      use5RMSCut = true;
      firstRunArgument+=1;      
    }
    else if( std::string(argv[firstRunArgument]).compare("--deadThreshold") == 0 )
    {
      deadThreshold = atof(argv[firstRunArgument+1]);
      firstRunArgument+=2;      
    }
    else if( std::string(argv[firstRunArgument]).compare("--noisyThreshold") == 0 )
    {
      noisyThreshold = atof(argv[firstRunArgument+1]);
      firstRunArgument+=2;      
    }
    else if( std::string(argv[firstRunArgument]).compare("--runNumberOffset") == 0 )
    {
      runNumberOffset = atoi(argv[firstRunArgument+1]);
      firstRunArgument+=2;      
    }
    else
    {
      //no options
      break;
    }
  }

  if (firstRunArgument == unsigned(argc))
  {
    std::cout << "Error, no runs specified" << std::endl;
    std::cout << std::endl;
    printUsage();
    return 2;
  }

  // check if first run argment starts with a number, in case of typos...
  if ( !isdigit(argv[firstRunArgument][0]) )
  {
    std::cout << "Error, first run number is not a digit. Do you have a typo?" << std::endl;
    std::cout << std::endl;
    printUsage();
    return 3;
  }
 
  // the first entry in channelStatusMap is the channel (global channel number), the second one the status
  std::map< int, int > channelStatusMap;

  // the key is the pair of global channel number and run number
  // the value is the status
  std::map< std::pair<int , int> , int > channelRunStatusMap;

  std::vector< SpectrumPropertiesRunInfo * > runInfos;

  // The pointer to the input root file is always there. It is also 
  // used as a switch between the rootFile and ahcConfig mode (if 0 use ahcConfig)
  TFile *inputRootFile = 0;

  if ( !inputRootFileName.empty())
  {
    inputRootFile= new TFile(inputRootFileName.c_str());
  }
  
  // scan the input number, no safety checking
  for (int i = firstRunArgument; i < argc; i++)
  {
    Int_t runNumber = std::atoi( argv[i] );
    std::cout << "run " << runNumber << std::endl;

    SpectrumPropertiesRunInfo *spri;
    if (inputRootFile)
    {
      // get the tree from the file;
      std::stringstream treeName;
      treeName << "spriTree_" << runNumber;

      inputRootFile->cd();
      TTree *spriTree=dynamic_cast<TTree *>(gDirectory->Get( treeName.str().c_str() ));
      if ( spriTree == 0 ) std::cerr << "ERROR: spriTree pointer is 0 !" << std::endl;

      spri = new SpectrumPropertiesRunInfo(runNumber, spriTree);     
    }
    else
    {
      // create the spri from the ahcBinHst outputm using ahcConfig
      spri = new SpectrumPropertiesRunInfo(runNumber, ahcConfigName, use5RMSCut);
    }
    runInfos.push_back(spri);
  }

  // vector with the numbers of dead/noisy/bad channels for all channels
  TVectorD nDeadChannelsVector( runInfos.size() );
  TVectorD nNoisyChannelsVector( runInfos.size() );
  TVectorD nBadChannelsVector( runInfos.size() );
  TVectorD runsVector( runInfos.size() );

  for ( std::vector< SpectrumPropertiesRunInfo * >::iterator runIter = runInfos.begin();
	runIter < runInfos.end(); runIter++ )
  {
    // dead channels 
    std::vector< SpectrumPropertiesRunInfo::ChannelID >  deadChannels = (*runIter)->getChannelsWithRMSCut(0, deadThreshold);
    for ( std::vector< SpectrumPropertiesRunInfo::ChannelID >::iterator channelIter = deadChannels.begin();
	  channelIter < deadChannels.end(); channelIter++ )
    {
      int globalChannelNumber = SpectrumPropertiesRunInfo::getGlobalChannelNumber(*channelIter);

      // check if the entry exists. Just |= is not ok since the map does not initialise the int 
      std::map< int, int >::iterator it = channelStatusMap.find( globalChannelNumber );
      if ( it != channelStatusMap.end() )
      {
	// entry exists, use bitwise or it add a flag
	it->second |= ChannelExpertQuality::dead;
      }
      else
      {
	// entry does not exist
	channelStatusMap[globalChannelNumber] = ChannelExpertQuality::dead;
      }
	
      // in the per run map we can be sure the channel does not exist, as dead is the first loop
      channelRunStatusMap[ std::pair< int, int >( globalChannelNumber, (*runIter)->getRunNumber() )] =  ChannelExpertQuality::dead;
    }

    // noisy channels
    std::vector< SpectrumPropertiesRunInfo::ChannelID >  noisyChannels = (*runIter)->getChannelsWithRMSCut(noisyThreshold);
    for ( std::vector< SpectrumPropertiesRunInfo::ChannelID >::iterator channelIter = noisyChannels.begin();
	  channelIter < noisyChannels.end(); channelIter++ )
    {
      int globalChannelNumber = SpectrumPropertiesRunInfo::getGlobalChannelNumber(*channelIter);

      // check if the entry exists. Just |= is not ok since the map does not initialise the int 
      std::map< int, int >::iterator it = channelStatusMap.find( globalChannelNumber );
      if ( it != channelStatusMap.end() )
      {
	// entry exists, use bitwise or it add a flag
	it->second |= ChannelExpertQuality::highRMS;
      }
      else
      {
	// entry does not exist
	channelStatusMap[globalChannelNumber] = ChannelExpertQuality::highRMS;
      }
	
      // in the per run map we can be sure the channel does not exist, as dead and noisy are mutually exclusive
      channelRunStatusMap[ std::pair< int, int >( globalChannelNumber, (*runIter)->getRunNumber() )] =  ChannelExpertQuality::highRMS;
    }

    // add loops for other bad criteria here...
    
    // finally the per run statistics
    int runVectorIndex = std::distance( runInfos.begin(), runIter ); // the position in the vectors, why the f. don't they have a push_back. Dynamic is different...
    nDeadChannelsVector[ runVectorIndex ] = deadChannels.size();
    nNoisyChannelsVector[ runVectorIndex ] = noisyChannels.size();
    nBadChannelsVector[ runVectorIndex ] = deadChannels.size() + noisyChannels.size();
    runsVector[ runVectorIndex ] = (*runIter)->getRunNumber() - runNumberOffset;
    
  } 

  // a few settings to improve the plot, especially get rid of the grey background
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetFrameFillColor(kWhite);

  TCanvas c;
  c.SetTopMargin(0.2); // increase the top margin to have space for the legend

  TGraph gDead(  runsVector, nDeadChannelsVector );
  TGraph gNoisy( runsVector, nNoisyChannelsVector );
  TGraph gBad(   runsVector, nBadChannelsVector );

  gDead.SetMarkerStyle(2);
  gDead.SetMarkerColor(kRed);
  gNoisy.SetMarkerStyle(3);
  gNoisy.SetMarkerColor(kBlue);
  gBad.SetMarkerStyle(4);

  gBad.Draw("AP");
  // now that the axes are there (after the first drawing) set the  yaxis range
  Double_t yMax = gBad.GetYaxis()->GetXmax();
  gBad.GetYaxis()->SetRangeUser(0, yMax);
  gBad.GetYaxis()->SetTitle("Number of Dead / Noisy / Bad Channels");

  std::stringstream xAxisTitle;
  xAxisTitle << "Run Number -" << runNumberOffset;
  gBad.GetXaxis()->SetTitle(xAxisTitle.str().c_str());
  gBad.SetTitle("Bad Channel History");

  gDead.Draw("PSame");
  gNoisy.Draw("PSame");

  TLegend legend(0.5,0.82,0.9,1.0);
  //  legend.SetHeader("The Legend Title");
  legend.AddEntry(&gDead,"Dead Channels","p");
  legend.AddEntry(&gNoisy,"Noisy Channels","p");
  legend.AddEntry(&gBad,"Bad = Dead + Noisy","p");
  legend.Draw();

  c.Print("nBadChannelsHistory.pdf");
  c.Print("nBadChannelsHistory.C");

  // print the bad channel list to a file
  std::ofstream badChannelsTextFile;
  badChannelsTextFile.open ("badChannels.txt");
  printChannelStatusMap( channelStatusMap , badChannelsTextFile );
  badChannelsTextFile.close();

  for ( std::vector< SpectrumPropertiesRunInfo * >::iterator runIter = runInfos.begin();
	runIter < runInfos.end(); runIter++ )
  {
    delete *runIter; // * dereferences the iterator, not the pointer
  }

  return 0;
}
