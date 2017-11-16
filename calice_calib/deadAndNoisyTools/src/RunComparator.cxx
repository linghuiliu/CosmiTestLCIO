#include "RunComparator.h"
#include <iterator>
#include <TVectorD.h>
#include <TString.h>

RunComparator::RunComparator( SpectrumPropertiesRunInfo const & firstRunInfo, 
			      SpectrumPropertiesRunInfo const & secondRunInfo )
  : _firstRunInfo(firstRunInfo), _secondRunInfo(secondRunInfo),
    _theGraph(0), _xAxisHisto(0) , _yAxisHisto(0)
{}

RunComparator::~RunComparator()
{
  delete _theGraph;
  delete _xAxisHisto;
  delete _yAxisHisto;
}

void RunComparator::prepareGraph( std::pair<double, double> (*calculatorFunction)
				  (TChannelSpectrumProperties const &, TChannelSpectrumProperties const & ) )
{
  // get all channels from the first run info (calling getChannelsWithRMSCut without
  // arguments does not perform a cut)
  std::vector<SpectrumPropertiesRunInfo::ChannelID> channelList = _firstRunInfo.getChannelsWithRMSCut();

  prepareGraph( calculatorFunction, channelList );
}

void RunComparator::prepareGraph( std::pair<double, double> (*calculatorFunction)
				              (TChannelSpectrumProperties const &, TChannelSpectrumProperties const & ),
				  std::vector<SpectrumPropertiesRunInfo::ChannelID> const & channelList )
{
  // instantiate two vectors for the values from the x and y axis
  TVectorD xAxisValues( channelList.size() );
  TVectorD yAxisValues( channelList.size() );

  // put the run numbers into the title 
  TString titleString("Runs ");
  titleString+=_firstRunInfo.getRunNumber();
  titleString+=" and ";
  titleString+=_secondRunInfo.getRunNumber();
  
  _channelValuesMap.clear();

  //iterate the channel list and fill the graph and the histograms
  for ( std::vector<SpectrumPropertiesRunInfo::ChannelID>::const_iterator channelIter = channelList.begin();
	channelIter < channelList.end(); channelIter++)
  {
    // get the channel spectrumPropertiess from the run infos 
    TChannelSpectrumProperties channelSpectrumProperties1 = _firstRunInfo.getChannelSpectrumProperties( *channelIter );
    TChannelSpectrumProperties channelSpectrumProperties2 = _secondRunInfo.getChannelSpectrumProperties( *channelIter );

    // calculate the x and y value from the channel spectrumPropertiess, using the calculator function
    std::pair<double, double> xyValues = (*calculatorFunction)( channelSpectrumProperties1,
								channelSpectrumProperties2 );

    // set the x and y values in the vectors
    // the index in the vector is the distance to the beginning
    Int_t vectorIndex = std::distance( channelList.begin(), channelIter);
    xAxisValues[ vectorIndex ] = xyValues.first;
    yAxisValues[ vectorIndex ] = xyValues.second;

    _channelValuesMap[*channelIter] = xyValues;
  }

  delete _theGraph;
  _theGraph = new TGraph( xAxisValues, yAxisValues );
  _theGraph->SetTitle( titleString );
 
  delete _xAxisHisto;
  _xAxisHisto = new TH1D("xAxisHisto", titleString, 100, 0, 0 );

  delete _yAxisHisto;
  _yAxisHisto = new TH1D("yAxisHisto", titleString, 100, 0, 0 );

  // fill the histograms
  for (Int_t i = 0; i < xAxisValues.GetNoElements() ; i++ )
  {
    _xAxisHisto->Fill( xAxisValues[i] );
    _yAxisHisto->Fill( yAxisValues[i] );
  }
  
}

std::pair<double, double> RunComparator::calculateChannelAndPedestalCorrectedMean
                                        (TChannelSpectrumProperties const & channelSpectrumPropertiesPMNoise, 
			                 TChannelSpectrumProperties const & channelSpectrumPropertiesOther)
{

  SpectrumPropertiesRunInfo::ChannelID channelID(  channelSpectrumPropertiesPMNoise._module,
					 channelSpectrumPropertiesPMNoise._chip,
					 channelSpectrumPropertiesPMNoise._channel );
  double globalChannelNumber =  SpectrumPropertiesRunInfo::getGlobalChannelNumber( channelID);

  double pedestalCorrectedMean = channelSpectrumPropertiesOther._mean - channelSpectrumPropertiesPMNoise._mean;

  return std::pair<double,double>( globalChannelNumber, pedestalCorrectedMean);
}

//std::auto_ptr<TGraph> RunComparator::getGraph()
TGraph * RunComparator::getGraph()
{
  // give a warning if the graph does not exist
  if ( !_theGraph) 
  {
    std::cout << "RunComparator::getGraph(): Error, graph not initialised. " 
	      << "Call prepareGraph() first." << std::endl;
  }

  return  _theGraph;
}

TH1D * RunComparator::getXAxisHisto()
{
  // give a warning if the histo does not exist
  if ( !_xAxisHisto) 
  {
    std::cout << "RunComparator::getXAxisHisto(): Error, histogram not initialised. " 
	      << "Call prepareGraph() first." << std::endl;
  }

  return _xAxisHisto;
}

TH1D * RunComparator::getYAxisHisto()
{
  // check that the histo exists before calling Clone()
  if ( !_yAxisHisto) 
  {
    std::cout << "RunComparator::getYAxisHisto(): Error, histogram not initialised. " 
	      << "Call prepareGraph() first." << std::endl;
  }

  return _yAxisHisto;
}

std::map< SpectrumPropertiesRunInfo::ChannelID, std::pair<double, double> > const & 
   RunComparator::getChannelValuesMap() const
{
  return _channelValuesMap;
}

std::set< SpectrumPropertiesRunInfo::ChannelID >
  RunComparator::getChannelsWithCut( Double_t yMin, Double_t yMax,
				    Double_t xMin, Double_t xMax ) const
{
  std::set< SpectrumPropertiesRunInfo::ChannelID > setWithCut;

  for ( std::map< SpectrumPropertiesRunInfo::ChannelID, std::pair<double, double> >::const_iterator iter 
	  = _channelValuesMap.begin() ;	iter != _channelValuesMap.end(); iter ++ )
  {
    std::pair<double, double> xyPair = iter->second;
    if  ( ( xyPair.first  >= xMin ) &&
	  ( xyPair.first  <= xMax ) &&
	  ( xyPair.second >= yMin ) &&
	  ( xyPair.second <= yMax ) )
    {
      setWithCut.insert( iter->first );
    }
  }
  
  return setWithCut;
}
