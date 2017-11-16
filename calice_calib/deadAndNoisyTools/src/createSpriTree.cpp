#include "SpectrumPropertiesRunInfo.h"
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

/** \todo FIXME Current hack: The thresholds for dead and noisy are hard coded.
 */
static const float DEAD_THRESHOLD(20.5); ///< The threshold for dead channels is 20.5 RMS
static const float NOISY_THRESHOLD(140); ///< The threshold for noisy channels is 140 RMS

/** \todo FIXME Current hack: The name for the ahc.cfg file is hardcoded. It is subject to be removed
    when the mapping is done already in ahcBinHst.
 */
static const std::string AHC_CONFIG_NAME("AHCforCERN2010.cfg");

/** \todo FIXME Current hack: An offset to improve the scale of the run number on the x axis
 */
static const int RUN_NUMBER_OFFSET(360000); ///< In the CERN 2010 campain the runs were 360xxx

void printUsage(void)
{
    std::cout << "Usage: createSpriTree  [--with5RMSCut] [--ahcConfig ahc.cfg] "
	      << " [--outfile outfile.root] run1 [run2] ..."  << std::endl;
    std::cout << "  Create a root file with the TSprectrumPropertiesRunInfo (spri) trees." <<std::endl;
    std::cout << "  For each run there is a tree named spriTree_runnumber." << std::endl;
    std::cout << std::endl;
    std::cout << "  Optional arguments:" <<std::endl;
    std::cout << "  The optional arcuments must be given before the run numbers." << std::endl;
    std::cout << "  --ahcConfig\t Specify the ahcConfig file. If not given it is required that" 
	      << " the mapping was done in ahcBinHst" <<std::endl;
    std::cout << "  --outfile\t Name of the output root file."
	      << " If none is given the name is run1_tree.root." <<std::endl;
    std::cout << "  --with5RMSCut\t Apply iterative 5*RMS cut on histograms before filling the tree"
	      << std::endl;
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

  static const unsigned int maxOptions = 3;
  
  // a variable of the first argument containig a run number
  size_t firstRunArgument = 1; 

  std::string ahcConfigFileName;
  std::string outfileName;
  bool use5RMSCut = false;

  for (unsigned int option(0); option < maxOptions; option++)
  {  
    if (firstRunArgument == unsigned(argc))
    {
      break;
    }

    // check for the options 
    if ( std::string(argv[firstRunArgument]).compare("--ahcConfig") == 0 )
    {
      ahcConfigFileName =  std::string(argv[firstRunArgument+1]);
      firstRunArgument+=2;
    }
    else if( std::string(argv[firstRunArgument]).compare("--outfile") == 0 )
    {
      outfileName =  std::string(argv[firstRunArgument+1]);
      firstRunArgument+=2;      
    }
    else if( std::string(argv[firstRunArgument]).compare("--with5RMSCut") == 0 )
    {
      use5RMSCut = true;
      firstRunArgument+=1;      
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
  
  if (outfileName.empty())
  {
    // create the root file name from the first run number
    std::stringstream fileNameStream;
    fileNameStream << argv[firstRunArgument] << "_tree.root";
    outfileName = fileNameStream.str();
  }

  // some debug stuff
  std::cout << "ahc config is " <<( ahcConfigFileName.empty()? "not used." : ahcConfigFileName )
	    << std::endl;
  std::cout << "output file is " << outfileName  << std::endl;
  std::cout << "use5RMSCut is " << (use5RMSCut?"true":"false") << std::endl;
  std::cout << "runs ";
  for ( size_t run = firstRunArgument; run < unsigned(argc); run ++)
  {
    std::cout << argv[run] << "\t";
  }
  std::cout << std::endl;

  // phew, alot of overhead for a short programme.
  // Now we can start the real thing

  // try to open the root file. Append to it
  TFile outfile( outfileName.c_str(), "UPDATE" );

  for ( size_t run = firstRunArgument; run < unsigned(argc); run ++)
  {
    SpectrumPropertiesRunInfo spri( atoi(argv[run]), ahcConfigFileName, use5RMSCut );
    
    TString treeName("spriTree_");
    treeName+=argv[run];
    
    outfile.cd();
    spri.getSpectrumPropertiesTree()->Write(treeName);
  }
  
  outfile.Close();

  return 0;
}
