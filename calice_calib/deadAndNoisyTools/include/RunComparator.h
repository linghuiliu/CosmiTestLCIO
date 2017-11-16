#ifndef RUN_COMPARATOR_H
#define RUN_COMPARATOR_H

#include "TChannelSpectrumProperties.h"
#include "SpectrumPropertiesRunInfo.h"

#include <map>
#include <set>
#include <vector>
#include <cfloat> // for the DBL_MAX constant

#include <TH1D.h>
#include <TGraph.h>

/** A class to facilitate the comparison of two runs.
 *  It creates a TGraph with two numbers calculated 
 *  from the TChannelSpectrumPropertiess for the two runs for 
 *  each channel, and the `projections` on the axes,
 *  i. e. a 1D histogram of each of the values.
 *
 *  The numbers calculated are defined by a function
 *  which takes two TChannelSpectrumPropertiess as input and
 *  returns a pair of doubles.
 *  The most important functions are defined within
 *  this class, but the user can also define 
 *  his own functions, which provides full flexibility.
 *
 *  Although the definition of function pointers look complicated  and lengthy,
 *  the usage is simple:
 *  \code
 *  // a PMNoise run
 *  SpectrumPropertiesRunInfo priPMNoise(123, "myAHCConfig.cfg");
 *
 *  // a PMLedVcalib run
 *  SpectrumPropertiesRunInfo priPMLED(124, "myAHCConfig.cfg");
 *
 *  RunComparator myRunComparator( priPMNoise, priPMLED );
 *
 *  // create the graph with the channel number on the x axis and the 
 *  // pedestal corrected mean on the y axis. Just pass the address of the 
 *  // function (thus the &).
 *  myRunComparator.prepareGraph( &calculateChannelAndPedestalCorrectedMean );
 *
 *  myRunComparator.getGraph()->Draw("AP"); //draw the graph
 *  myRunComparator.getXAxisHisto()->Draw(); //draw the histo for the x axis values
 *  \endcode
 * 
 * \todo Make the return values of the clones graphs and histos auto pointers. 
 * Damn, CINT does not support auto pointers :-(
 */
class RunComparator
{
 public:
  /** Initialise the class by giving the two SpectrumPropertiesRunInfo objects of the
   * two runs.
   */
  RunComparator( SpectrumPropertiesRunInfo const & firstRunInfo, SpectrumPropertiesRunInfo const & secondRunInfo );

  /** Destructor to free the dynamically allocated histograms
   */
  ~RunComparator();

  /** Convenience function which fills the graph and the histograms for all channels.
   */
  void prepareGraph( std::pair<double, double> (*calculatorFunction)
		                          (TChannelSpectrumProperties const &, TChannelSpectrumProperties const & ) );

  /** This function fills the graph and the histograms with the values calculates 
   *  by the calculatorFunction (first value on the x axis, second on the y axis).
   *
   * \param calculatorFunction The first parameter is a pointer to a function which returns 
   *        a pair of doubles and takes two const references to TChannelSpectrumProperties as argument.
   *        This function calculates the values for the x and y axis from the channel spectrumPropertiess.
   * \param channelList A const reference to a vector of channel IDs. These are the channels 
   *        for which the graph and the histograms are filled.
   */
  void prepareGraph( std::pair<double, double> (*calculatorFunction)
		                          (TChannelSpectrumProperties const &, TChannelSpectrumProperties const & ),
		     std::vector<SpectrumPropertiesRunInfo::ChannelID> const & channelList );

  /** This function returns the global channel number for the x axis (first value) 
   *  and \c channelSpectrumPropertiesOther._mean - \c channelSpectrumPropertiesPMNoise._mean for the y axis 
   *  (second value). This usually only makes sense if the first argument (\c spectrumPropertiesPMNoise) 
   *  is a noise run, where the mean value is the pedestal.
   */
  static std::pair<double, double> calculateChannelAndPedestalCorrectedMean
                                        (TChannelSpectrumProperties const & spectrumPropertiesPMNoise, 
			                 TChannelSpectrumProperties const & spectrumPropertiesOther);

  /** Gets a pointer to the graph object. The first variable of the return value of the 
   *  calculatorFunction is in the x axis, the second one on the y axis.
   *  \attention This pointer is only valid until the next call of prepareGraph().
   *  If you want to preserve the graph make a clone of it:
   *  \code 
   *  myRunComparator.prepareGraph( &functionForMyFirstGraph );
   *  // clone the graph to preserve it
   *  TGraph *myFirstGraph = myRunComparator.getGraph()->Clone();
   *
   *  // Now create the second graph. The first graph inside the RunComparator is deleted.
   *  myRunComparator.prepareGraph( &functionForMySecondGraph );
   *  // If we need no further graphs we don't need to clone this one:
   *  TGraph *mySecondGraph = myRunComparator.getGraph();
   *  \endcode
   */
  TGraph * getGraph();

  /** Gets a pointer to the x axis histogram (histogram of the first variable
   *   of the calculator function).
   *  \attention This pointer is only valid until the next call of prepareGraph().
   *  If you want to preserve the graph make a clone of it.
   */
  //std::auto_ptr<TH1D> getXAxisHisto();
  TH1D * getXAxisHisto();


  /** Gets a pointer to the y axis histogram (histogram of the first variable
   *   of the calculator function).
   *  \attention This pointer is only valid until the next call of prepareGraph() 
   *  (or the RunComparator is deleted).
   *  If you want to preserve the graph make a clone of it.
   */
  TH1D * getYAxisHisto();

  //  typedef std::pair<double, double> DoublePair;
  //  typedef std::map< SpectrumPropertiesRunInfo::ChannelID, DoublePair >  ChannelValuesMap;

  /**
   * Returns a map with all channels and the correlated x and y values
   */
  std::map< SpectrumPropertiesRunInfo::ChannelID, std::pair<double, double> > const & 
    getChannelValuesMap() const;

  /**
   * Returns a set with the channels and the correlated x and y values and cuts on
   * x and y. Note the ordering of the cut parameter: The cuts on y come first for convenience
   * since there will be cuts on y more often than on x, so one can just leave the default
   * values which do not cut at all.
   * n.b.: The return value is a set because it has a find() which allows to check whether a channel
   * is in the set.
   */
  std::set< SpectrumPropertiesRunInfo::ChannelID > 
    getChannelsWithCut( Double_t yMin = -DBL_MAX, Double_t yMax = DBL_MAX,
			Double_t xMin = -DBL_MAX, Double_t xMax = DBL_MAX) const;

 protected:
  SpectrumPropertiesRunInfo const & _firstRunInfo; ///< pointer to the first SpectrumPropertiesRunInfo
  SpectrumPropertiesRunInfo const & _secondRunInfo;///< pointer to the first SpectrumPropertiesRunInfo

  TGraph * _theGraph; ///< The internal instance of the graph
  TH1D * _xAxisHisto; ///< The internal instance of the x axis histogram
  TH1D * _yAxisHisto; ///< The internal instance of the y axis histogram

  /// A map to correlate channelID and the calculated values
  std::map< SpectrumPropertiesRunInfo::ChannelID, std::pair<double, double> > _channelValuesMap;
};

#endif //RUN_COMPARATOR_H
