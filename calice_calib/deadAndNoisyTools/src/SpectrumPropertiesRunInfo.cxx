#include "SpectrumPropertiesRunInfo.h"
#include "SimpleMapper.h"

#include <string>
#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TIterator.h>
#include <TCollection.h>
#include <TTree.h>
#include <TH1D.h>
#include <TROOT.h>
#include <TKey.h>

#define MAX_ITERATIONS 10 // It is completely uncritical to hardcode this vaule.
                         // As it is used twice in the code I introduced a define.

// the static variables for the number of channels per chip and chips per modue
Int_t SpectrumPropertiesRunInfo::_nChannelsPerChip = 18;
Int_t SpectrumPropertiesRunInfo::_nChipsPerModule = 12;

SpectrumPropertiesRunInfo::SpectrumPropertiesRunInfo(Int_t runNumber, std::string AHCConfigFileName,
						     bool apply5RMSCut,  std::string runFileName,
						     unsigned int moduleTypes)
  : _runNumber( runNumber) , createdTree(true), _moduleTypeMask(moduleTypes)
{
  if (!AHCConfigFileName.empty())
  {
    _simpleMapper = new SimpleMapper( AHCConfigFileName );
  }
  else 
  {
    _simpleMapper = 0;
  }

  if ( !openRunFile( runFileName ) )
  {
    //opening the run file failed. Set the class to a safe state
    _channelSpectrumProperties = 0;
    _spectrumPropertiesTree = 0;
    _runNumber = -1;
    return;
  }

  // Create the tree from the input file.
  // First allocate the data class and the tree.

  // Change to the gROOT directory. This is the main memory root allocates.
  // Like this the next object (the tree) will be creates in memory and not
  // associated to any file. 
  // To write it to a file do:
  // file->cd();
  // tree->Write();
  gROOT->cd();

  // Create the tree in memory:
  _spectrumPropertiesTree = new TTree;
  _channelSpectrumProperties = new TChannelSpectrumProperties;
  _spectrumPropertiesTree->Branch("spectrumPropertiesTree","TChannelSpectrumProperties", &_channelSpectrumProperties);

  // Initialisation takes some time. print the status every 5 %
  // Some variables neede for this
  Int_t nEntries = _runFile->GetListOfKeys()->GetEntries();
  Int_t entry = 0;
  Int_t printEvery = nEntries / 20;

  std::cout << "SpectrumPropertiesRunInfo: reading input histograms, please wait... " << std::endl;

  // Now iterate all the keys in the file 
  TIterator * keyIter = _runFile->GetListOfKeys()->MakeIterator();
  while ( TKey *key = dynamic_cast<TKey *>(keyIter->Next())  )
  {
    if ( entry%printEvery == 0)
    {
      std::cout << "processed  " << entry*100 / nEntries << "%"<< std::endl;
    }

    // Count up the number of entries for the status coutput
    entry++;
    
    TH1D *histo = dynamic_cast<TH1D*>( key->ReadObj() );
    // check that the cast was successful, otherwise it was not a TH1D
    if (histo ==0) continue;
    
    // determine the configuration variables from the histogram title
    // the variables we want to scan for
    int crcSer       = -1;
    int slot         = -1;
    int frontEnd     = -1;
    int chip         = -1;
    int channel      = -1;
    int module       = -1;
    int layer        = -1;

    // try the old version without mapping
    int nNumbersRead = sscanf(histo->GetTitle(), "SER%d, Slot %d, FE%d, Chip %d, Chan %d", 
			                       &crcSer, &slot, &frontEnd, &chip, &channel);
    if (nNumbersRead == 5)
    {
      // ok, it's the old syntax. try to ger the mapping from the simpleMapper

      if (_simpleMapper)
      {
	if ( ! (_simpleMapper->getModuleType( SimpleMapper::SlotFrontendID(slot,frontEnd))
		& _moduleTypeMask) )
	{
	  // module type does not fit the moduleMask, just continue
	  delete histo;
	  continue;
	}

	// get module and layer from the config
	SimpleMapper::ModuleLayerID moduleLayerID = 
	  _simpleMapper->getModuleLayerID( SimpleMapper::SlotFrontendID(slot,frontEnd) );

	if (_simpleMapper->isAHC8( SimpleMapper::SlotFrontendID(slot,frontEnd) ) )
	{
	  // the hard coded geometry for AHC8:
	  // Chips 4 to 7 are not present, as well as channels 0 to 2 on chip 8.
	  // Just continue the loop
	  if (chip >= 4 && chip <=7)
	  {
	    delete histo;
	    continue;
	  }
	  if (chip == 8 && channel <=2 )
	  {
	    delete histo;
	    continue;
	  }
	}
	module = moduleLayerID.first;
	layer  = moduleLayerID.second;
      }
      else
      {
	std::cout << "No mapping from AHC.cfg availabel and it has not bee done in ahcBinHst."
		  << std::endl;
	std::cout << "Please provide the name of the AHC.cfg file."
		  << std::endl;
	delete histo;
	continue;
      }// else _simpleMapper
    }
    else// if (nNumbersRead == 5)
    {
      // try the new title format which is alredy mapped 
      int nNumbersRead = sscanf(histo->GetTitle(), "Module %d, Layer %d, Chip %d, Channel %d", 
				&module, &layer, &chip, &channel);
 
      if (nNumbersRead != 4)
      {
	std::cout << "Histogram titles do not have the right format."
		  << std::endl;
	delete histo;
	continue;	
      }
    }
    
    // fill the map with the ChannelID -> histoTitle correlation
    _histogramTitleMap[ChannelID( module, chip, channel )] 
      = std::string(histo->GetTitle());

    // apply the 5*rms cut if turned on
    if(apply5RMSCut)
    {
      double mean = histo->GetMean();
      double rms  = histo->GetRMS();

      // set nIterations outside the loop so we can check it after the loop has finished
      unsigned int nIterations = 0;
      for ( /* not set here */; nIterations < MAX_ITERATIONS; nIterations++ )
      {
	// limit the histogram to mean +- 5*rms
	histo->GetXaxis()->SetRangeUser( mean - 5*rms, mean + 5*rms );
	
	if (rms == histo->GetRMS() )
	{ // the rms is identical to the previous iteration: algoythm has converged
	  break;
	}
	else
	{ // remember the current values for the next iteration
	  mean = histo->GetMean();
	  rms  = histo->GetRMS();
	}
      }// for nIterations

      if (nIterations == MAX_ITERATIONS)
      {
	std::cout << "SpectrumPropertiesRunInfo: Warning, 5*RMS cut did not converge in 10 iterations on module "
		  << module << " , chip " << chip << " , channel " << channel << std::endl;
      }
    }


    _channelSpectrumProperties->Set( module, chip,  channel, 
				     histo->GetMean(), histo->GetRMS() );

    // fill the tree
    _spectrumPropertiesTree->Fill();

    // in addition to filling the tree also fill the map for random accress
    _channelSpectrumPropertiesMap[ ChannelID( module, chip, channel ) ] = *_channelSpectrumProperties;

    // ROOT created a clone of the object in the file in memory. We don't need it anymore.
    // Delete it in order not to fill up the memory.
    delete histo;
  }
  std::cout << "processed  100%"<< std::endl;

}




SpectrumPropertiesRunInfo::~SpectrumPropertiesRunInfo()
{
  // the tree is not associated with a file, we have to delete it manually
  if (createdTree) // only delete if the tree was created in the constructor
  {
    delete _spectrumPropertiesTree;
  }

  // get rid of the open input file
  if (_runFile)
  {
    _runFile->Close();
    delete _runFile;
  }
  delete _simpleMapper;
}
SpectrumPropertiesRunInfo::SpectrumPropertiesRunInfo(Int_t runNumber,
						     TTree * spectrumPropertiesTree)
  : _runNumber( runNumber) , _simpleMapper(0), _spectrumPropertiesTree( spectrumPropertiesTree ),
    _channelSpectrumProperties(0), createdTree(false), _moduleTypeMask(0)
{
  _spectrumPropertiesTree->SetBranchAddress("spectrumPropertiesTree",&_channelSpectrumProperties);

  // fill the properties map from the tree
  for (int i=0; i<_spectrumPropertiesTree->GetEntries(); i++)
  {
    _spectrumPropertiesTree->GetEntry(i);
    _channelSpectrumPropertiesMap[ ChannelID(_channelSpectrumProperties->_module,
					     _channelSpectrumProperties->_chip,
					     _channelSpectrumProperties->_channel) ]
      = *_channelSpectrumProperties;    
  }

  // currently there is no need to open the run file since the map to access the histo names is not
  // filled.
  // openRunFile(std::string runFileName);
  _runFile=0;
}
 
TTree * SpectrumPropertiesRunInfo::getSpectrumPropertiesTree()
{
  return _spectrumPropertiesTree;
}


void SpectrumPropertiesRunInfo::print(std::string fileName ) const 
{
  std::ostream *myOutputStream;
  if (fileName.empty())
  {
    myOutputStream = &std::cout;
  }
  else
  {
    myOutputStream = new std::ofstream( fileName.c_str() , std::ios::out );
  }

  *myOutputStream << "#SpectrumPropertiesRunInfo for run " << _runNumber << std::endl;
  *myOutputStream << "#module" << "\t" << "chip"     << "\t" << "channel" << "\t"
		  << "mean"   << "\t" << "rms" << std::endl;
  // n.b. The << for every \t is to improve code readability AND have the same
  // spacing in the final output ( << "module \t chip" would add another space)

  for (Int_t i = 0; i <  _spectrumPropertiesTree->GetEntries(); i++)
  {
    _spectrumPropertiesTree->GetEntry(i);
    *myOutputStream << _channelSpectrumProperties->_module  <<"\t"
		    << _channelSpectrumProperties->_chip    <<"\t"
		    << _channelSpectrumProperties->_channel <<"\t"
		    << _channelSpectrumProperties->_mean    <<"\t"
		    << _channelSpectrumProperties->_rms     << std::endl;
  }

  if (!fileName.empty())
  {
    // we just created an ofstream. Delete it 
    delete myOutputStream;
  }
}

bool SpectrumPropertiesRunInfo::ChannelID::operator<(SpectrumPropertiesRunInfo::ChannelID const & right) const 
{
  if (_module == right._module)
  { // module numbers are equal, use chip ID instead
    if (_chip == right._chip)
    { // chip IDs are equal, use channel number 
      return _channel < right._channel;
    }
    else // chip numbers differ, use them
    {
      return _chip < right._chip;
    }
  }
  else // module number differs, use it to distinguish
  {
    return _module < right._module;
  }
}

SpectrumPropertiesRunInfo::ChannelID::ChannelID( Int_t module, Int_t chip, Int_t channel)
  : _module( module), _chip(chip), _channel(channel)
{}

std::vector< SpectrumPropertiesRunInfo::ChannelID >  SpectrumPropertiesRunInfo::getNoisyChannels( Double_t noisyCut ) const
{
  return getChannelsWithRMSCut(noisyCut);
}

std::vector< SpectrumPropertiesRunInfo::ChannelID >  SpectrumPropertiesRunInfo::getChannelsWithRMSCut(Double_t lowerCut,
										  Double_t upperCut) const 
{
  // the return value
  std::vector< SpectrumPropertiesRunInfo::ChannelID > channelsWithRMSCut;

  // loop the tree and fill the return vector
  for (Int_t entry = 0; entry < _spectrumPropertiesTree->GetEntries(); entry++)
  {
    _spectrumPropertiesTree->GetEntry( entry );
    if ( (_channelSpectrumProperties->_rms >= lowerCut) && (_channelSpectrumProperties->_rms <= upperCut) )
    {
      // channel rms is within cut range, add it to the return vector
      channelsWithRMSCut.push_back( ChannelID( _channelSpectrumProperties->_module,
					       _channelSpectrumProperties->_chip,
					       _channelSpectrumProperties->_channel ) );
    }
  }

  return channelsWithRMSCut;
}

std::vector< SpectrumPropertiesRunInfo::ChannelID >  SpectrumPropertiesRunInfo::getChannelsWithMeanCut(Double_t lowerCut,
										   Double_t upperCut) const 
{
  // the return value
  std::vector< SpectrumPropertiesRunInfo::ChannelID > channelsWithMeanCut;

  // loop the tree and fill the return vector
  for (Int_t entry = 0; entry < _spectrumPropertiesTree->GetEntries(); entry++)
  {
    _spectrumPropertiesTree->GetEntry( entry );
    if ( (_channelSpectrumProperties->_mean >= lowerCut) && (_channelSpectrumProperties->_mean <= upperCut) )
    {
      // channel rms is within cut range, add it to the return vector
      channelsWithMeanCut.push_back( ChannelID( _channelSpectrumProperties->_module,
					       _channelSpectrumProperties->_chip,
					       _channelSpectrumProperties->_channel ) );
    }
  }

  return channelsWithMeanCut;
}

void SpectrumPropertiesRunInfo::printChannels( std::vector< SpectrumPropertiesRunInfo::ChannelID >  const & listOfChannels )
{

  std::cout << "# module chip channel" << std::endl;
  for ( std::vector<ChannelID>::const_iterator chanIDIter = listOfChannels.begin();
	chanIDIter < listOfChannels.end(); chanIDIter++)
  {

    std::cout << chanIDIter->_module << "\t"
	      << chanIDIter->_chip << "\t"
	      << chanIDIter->_channel << std::endl;
  }
}

TH1D * SpectrumPropertiesRunInfo::getChannelHisto( SpectrumPropertiesRunInfo::ChannelID channelID)
{
  if (_histogramTitleMap.size() == 0)
  {
    std::cerr << "Accessing the histograms when creating the SpectrumPropertiesRunInfo "
	      << " class from a tree is not supported yet." 
	      << " Write the populateHistogramTitleMap() function" << std::endl;
    return 0;
  }
  
  if (_runFile && _runFile->IsOpen() )
  {
    return dynamic_cast<TH1D *>(_runFile->Get( _histogramTitleMap[channelID].c_str() ));
  }
  else
  {
    std::cout << "Sorry, the runFile could not be opened and the hist could not be read"
	      << std::endl;
    return 0;
  }
}

TH1D * SpectrumPropertiesRunInfo::getChannelHisto( Int_t module, Int_t chip, Int_t channel )
{
  return getChannelHisto( ChannelID( module, chip, channel ) );
}

TH1D * SpectrumPropertiesRunInfo::getChannelHisto( Int_t globalChannelNumber)
{
  return getChannelHisto( getChannelID(globalChannelNumber) );
}

void SpectrumPropertiesRunInfo::ChannelID::print(std::ostream & myOutStream) const
{
  myOutStream << "module "<<_module <<" ; chip "<<_chip << " ; channel "<< _channel << std::endl;
}

Int_t SpectrumPropertiesRunInfo::getGlobalChannelNumber( ChannelID const & channelID )
{
  return ( channelID._module * _nChipsPerModule + channelID._chip ) * _nChannelsPerChip
         + channelID._channel;
}

SpectrumPropertiesRunInfo::ChannelID SpectrumPropertiesRunInfo::getChannelID( Int_t globalChannelNumber )
{
  ChannelID retval;

  // n.b. this is an integer division. The rest is just discarded.
  retval._module = globalChannelNumber / (_nChipsPerModule*_nChannelsPerChip);
  retval._chip =  (globalChannelNumber / _nChannelsPerChip) % _nChipsPerModule;
  retval._channel = globalChannelNumber % _nChannelsPerChip;

  return retval;
}

void SpectrumPropertiesRunInfo::setNChipsPerModule( Int_t nChipsPerModule )
{
  _nChipsPerModule = nChipsPerModule;
}

void SpectrumPropertiesRunInfo::setNChannelsPerChip( Int_t nChannelsPerChip )
{
  _nChannelsPerChip = nChannelsPerChip;
}

Int_t SpectrumPropertiesRunInfo::getNChannelsPerChip(){return _nChannelsPerChip;}

Int_t SpectrumPropertiesRunInfo::getNChipsPerModule(){return _nChipsPerModule;}

TChannelSpectrumProperties SpectrumPropertiesRunInfo::getChannelSpectrumProperties(  ChannelID const & channelID) const
{
  // search if the channel ID exists. We do not want to access
  // map[] because this would create the object if not there and spoil the map.
  std::map< ChannelID, TChannelSpectrumProperties >::const_iterator channelSpectrumPropertiesIter = 
    _channelSpectrumPropertiesMap.find(channelID);

  if ( channelSpectrumPropertiesIter != _channelSpectrumPropertiesMap.end() )
  {
    return channelSpectrumPropertiesIter->second;
  }
  
  // in case the object was not found return an "empty" ChannelID. All ID values are -1
  return TChannelSpectrumProperties();
}

TChannelSpectrumProperties SpectrumPropertiesRunInfo::getChannelSpectrumProperties( Int_t globalChannelNumber ) const 
{
  return getChannelSpectrumProperties( getChannelID( globalChannelNumber ) );
}

TChannelSpectrumProperties const * const *  SpectrumPropertiesRunInfo::getSpectrumPropertiesTreeObject()
{
  return & _channelSpectrumProperties;
}

THStack *  SpectrumPropertiesRunInfo::drawGetDeadChannelsPlot( Double_t deadCut, Double_t xAxisMax)
{
  TH1D * rmsHisto = new TH1D("runHisto", "RMS Spectrum Run ", 100, 0, xAxisMax);

  TH1D * deadHisto = new TH1D("deadHisto", "RMS Spectrum of Dead Channel", 100, 0, xAxisMax);
  deadHisto->SetLineColor(kRed);

  // fill the histograms
  for (std::map< ChannelID, TChannelSpectrumProperties >::const_iterator channelSpectrumPropertiesIter 
	 = _channelSpectrumPropertiesMap.begin();
       channelSpectrumPropertiesIter != _channelSpectrumPropertiesMap.end(); channelSpectrumPropertiesIter++ )
  {
    TChannelSpectrumProperties const & channelSpectrumProperties = channelSpectrumPropertiesIter->second;

    rmsHisto->Fill( channelSpectrumProperties._rms );
    if ( channelSpectrumProperties._rms < deadCut )
    {
      deadHisto->Fill( channelSpectrumProperties._rms );
    }
  }

  TString stackTitle("RMS Spectrum and Dead Channels, Run ");
  stackTitle += _runNumber;

  THStack * hStack = new THStack("deadHStack",stackTitle);

  hStack->Add( rmsHisto );
  hStack->Add( deadHisto );

  // Draw one before setting the axis labels. The reason is that the axes don't exist before
  // begin drawn :-O
  hStack->Draw("nostack");

  hStack->GetXaxis()->SetTitle("RMS [ADC Counts]");
  hStack->GetYaxis()->SetTitle("Number of Entries");

  // redraw with axis labels
  hStack->Draw("nostack");

  return hStack;
}

Int_t SpectrumPropertiesRunInfo::getRunNumber() const
{
  return _runNumber;
}

//  if ( !openRunFileName )
bool SpectrumPropertiesRunInfo::openRunFile( std::string runFileName )
{
  TString fileName; //helper name, nice since TString has += operator

  // Generate the file name for the runFile. This is the input file.
  if (runFileName.empty())
  { // no file name given. Create it from the run number
    fileName += _runNumber; // the string was empty, start with the number
    fileName += ".root"; // append the .root suffix
  }
  else
  {
    fileName = runFileName;
  }

  _runFile = new TFile(fileName);

  if (!_runFile->IsOpen() )
  {
    std::cerr << "SpectrumPropertiesRunInfo: File "<< fileName << " could not be opened" << std::endl;
    // set the class to a safe state
    delete _runFile;
    _runFile = 0;
    return false;
  }
  else
  {
    return true;
  }
}
