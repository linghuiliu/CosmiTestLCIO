#ifndef SPECTRUM_PROPERTIES_RUN_INFO_H
#define SPECTRUM_PROPERTIES_RUN_INFO_H


#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <THStack.h>
#include "TChannelSpectrumProperties.h"
#include "SimpleMapper.h"
#include <vector>
#include <string>
#include <TH1D.h>
#include <iostream>
#include <cfloat> // for the DBL_MAX constant
#include "ModuleType.h"

/** 
 *  The SpectrumPropertiesRunInfo is a utility class to access the spectrum properties for all channels of one specific run.
 *
 *  It uses a global channel number which is calculated as
 * \f$( moduleID \cdot nChipsPerModule + chipID ) \cdot nChannelsPerChip + channelNumber \f$,
 *  for instance for the channel number on the x-axis of the plots.
 *  You can used the getChannelID( int globalChannelNumber ) to retrieve the module, chip, and 
 *  channel number.
 *  The default values for \c nChipsPerModule and \c nChannelsPerChip are:
 *  \li \c nChipsPerModule = 12
 *  \li \c nChannelsPerChip = 18
 *
 *  You can change this during runtime. You should do it immediately after using the conctructor
 *  to avoid inconsistencies. In order not to overcomplicate the constructor it is not in there.
 *  These values can only change with new readout hardware.
 *
 *  \section FiveRMSCut The 5*RMS cut 
 *  In the spectrum there might be very few outliers which are extremely far away.
 *  To cut them away a "5RMSCut" can be turned on in the conctructor.
 *  Here the RMS of the histogram is taken and
 *  the histogam is limited to \f$mean \pm 5\cdot rms\f$. Afterwards the new rms is calculated.
 *  This procedure is repeated iteratively until it converges (maximal 10 times). Usually 
 *  the convergense needs less than 5 iterations.
 */
class SpectrumPropertiesRunInfo
{
 public:  // This class definition has to be at the beginnig of the public block. 
          // It is the return type of some public functions
  
  /// A channel identifier helper class. The key in the map.
  class  ChannelID
  {
    public:
    /// A constructor to initialise all variables at once
    ChannelID( Int_t module = -1, Int_t chip = -1, Int_t channel = -1);

    Int_t _module; ///< Module I
    Int_t _chip; ///< Chip ID within the module
    Int_t _channel; ///< Channel number on the chip
    bool operator<(ChannelID const & right) const; // < operator for map
    /// Conveniently print the internal variables
    void print(std::ostream & myOutStream = std::cout) const;
  };

  /** This constructor opens a root file with histograms containing 
   *  the spectrum per channel, named 'runNumber.root', and 
   *  instantiates an SimpleMapper from the AHCConfigFileName.
   *  From this it fills the spectrumPropertiesTree.
   *  If no file name for the AHCConfig is given, the mapping has to be one in ahcBinHst.
   *
   *  The 5RMSCut is off by default to avoid cutting information. But usually it's a good
   *  idea to turn it on.
   *
   *  The runFileName for the file with the input histograms is optional. If not given or
   *  empty it tries to open 'runNumber.root'.
   *
   *  You can select the module type be providing a word with the ModuleType bits.
   *  Default is the calorimeter ( HCAL and HCAL8 ).
   */
  SpectrumPropertiesRunInfo(Int_t runNumber, std::string AHCConfigFileName = "",
			    bool apply5RMSCut = false, std::string runFileName = "",
			    unsigned int moduleTypes = ModuleType::AHC | ModuleType::AHC8);  

  /** A constructor to initialise the class from a previously calculated spectrum properties tree.
   *  This is much faster and less memory consuming than creating it from the
   *  input file with the per-channel histograms.
   */
  SpectrumPropertiesRunInfo(Int_t runNumber, TTree * spectrumPropertiesTree);  
 
  /** The destructor to clean up the allocated objects
   */
  ~SpectrumPropertiesRunInfo();

  
//  TGraph * createRMSGraph(Int_t module);
//  TGraph * createRMSGraph(Int_t module, Int_t slot);
//  TGraph * createMeanGraph(Int_t module);
//  TGraph * createMeanGraph(Int_t module, Int_t slot);
//  TChannelSpectrumProperties getChannelSpectrumProperties( Int_t _module ,
//				       Int_t _chip,
//				       Int_t _channel );
  
  /** Get a pointer to the spectrum properties tree.
   *  \attention You do not get the ownership of this tree. It belongs to the SpectrumPropertiesRunInfo class
   *  which will delete the tree in its destructor. After that the pointer is no longer valid.
   *
   *  \attention Do not use TTree->SetBranchAddress()! There is the _channelSpectrumProperties variable
   *  of SpectrumPropertiesRunInfo which is already set. You can access it with getSpectrumPropertiesTreeObject(). 
   *  If you change the branch address of the tree
   *  getNoisyChannels will fail, might even cause a segfault.
   */
  TTree * getSpectrumPropertiesTree(); 

  /** Print the spectrum properties tree. Give the name of the file as argument. 
   *  The file will be recreated, not appended.
   *  If the file name is empty print to stdout.
   */
  void print( std::string fileName = "" ) const ;

  /** Returns a vector with all ChannelIDs of channels which have an rms larger
   * or equal to \c noisyCut. Just calls getChannelsWithRMSCut( noisyCut);
   * Kept for convenience and backward conpatiblility.
   */
  std::vector< SpectrumPropertiesRunInfo::ChannelID > getNoisyChannels( Double_t noisyCut ) const ;

  
  /** Returns all channels with an RMS in the cut range (rms >= lowerCut) and (rms <=upperCut).
   *  The default maximal value of DBL_MAX (the highest number a double can hold in C++) allows
   *  to conveniently use only a lower cut. The lower cut of 0 should be fine since the rms by
   *  definition is larger or equals zero.
   */
  std::vector< SpectrumPropertiesRunInfo::ChannelID > getChannelsWithRMSCut( Double_t lowerCut = 0,
								   Double_t upperCut = DBL_MAX ) const;

  /** Returns all channels with a mean value in the cut range 
   *  (mean >= lowerCut) and (mean <=upperCut).
   *  The default maximal value of DBL_MAX (the highest number a double can hold in C++) allows
   *  to conveniently use only a lower cut.
   *  To apply only an upper cut use  -DBL_MAX for the lower cut. 
   *  This is the smallest negative number a double can hold. Note that DBL_MIN is the smallest
   *  positive number a double can hold, i.e. the smallest value that can be ditinguished from 0.
   *  Do not use this or 0 if you do not want to cut. Mean values can be negative!
   */
  std::vector< SpectrumPropertiesRunInfo::ChannelID > getChannelsWithMeanCut( Double_t lowerCut = -DBL_MAX,
								   Double_t upperCut = DBL_MAX )const;

  /** Convenience function to print a list of channels to the console 
   */
  void printChannels( std::vector< SpectrumPropertiesRunInfo::ChannelID > const  & listOfChannels );

  /** Get the ADC spectrum histogram of a specific channel. The histogram is loaded from the
   *  input file. Keep in mind that the histogram stays in memory if you don't delete it.
   *  It is safe to delete it after usage. The SpectrumPropertiesRunInfo class does have the ownership
   *  over the histogram, only root does the usual bookkeeping.
   *
   *  When using the constructor which takes the tree as input the file name
   *  'runNumber.root' is assumed as source for the histograms. If this file could
   *  not be opened the return value is 0.
   */
  TH1D * getChannelHisto( SpectrumPropertiesRunInfo::ChannelID channelID);

  /** Convenience function to call  getChannelHisto( ChannelID ) 
   *  from the three identifying integers.
   */
  TH1D * getChannelHisto( Int_t module, Int_t chip, Int_t channel );

  /** Convenience function to call  getChannelHisto( ChannelID ) 
   *  from the global channel number.
   */
  TH1D * getChannelHisto( Int_t globalChannelNumber );

  /** Get the global channel number
   *  \f$( moduleID \cdot nChipsPerModule + chipID ) \cdot nChannelsPerChip + channelNumber \f$
   *  from the channelID.
   */
  static Int_t getGlobalChannelNumber( ChannelID const & channelID );

  /** Get a ChannelID class from the global channel number */
  static ChannelID getChannelID( Int_t globalChannelNumber );

  /** Set the number of chips per module. This is used to calculate the global channel number.
   */
  static void setNChipsPerModule( Int_t nChipsPerModule );

  /** Set the number of channels per chip. This is used to calculate the global channel number.
   */
  static void setNChannelsPerChip( Int_t nChannelsPerChip );

  /** Get the number of channels per chip.
   */
  static Int_t getNChannelsPerChip();

  /** Get the number of chips per module.
    */ 
  static Int_t getNChipsPerModule();

  /** Get the channel's spectrum properties knowing the channel ID
   */
  TChannelSpectrumProperties getChannelSpectrumProperties( ChannelID const & channelID) const;

  /** Get the channel's spectrum properties knowing the global channel number
   */
  TChannelSpectrumProperties getChannelSpectrumProperties( Int_t globalChannelNumber ) const;
 
  /** Get the object in memory which is in the branch address of the channel properties tree.
   *  With this you can access data from the tree by saying \c tree->GetEntry(i).
   *
   *  This function returns a pointer to a const pointer to a const TChannelSpectrumProperties.
   *  The reason for this is that ROOT modifies the contents of the pointer 
   *   \c TChannelSpectrumProperties \c * \c _channelSpectrumProperties  with every call of \c tree->GetEntry(i).
   *  If it would just return the pointer you would have to call getChannelPropertiesTreeObject()
   *  after every call of \c tree->GetEntry(i).
   *  \code
   *  mySpectrumPropertiesRunInfo.getChannelPropertiesTree()->GetEntry(1); // get one object in the tree
   *
   *  TChannelSpectrumProperties const * const * channelSpectrumPropertiesObjectHandler 
   *                                                   = mySpectrumPropertiesRunInfo.getChannelPropertiesTreeObject();
   *
   *  // Attention: Do not dereference the handler and save the pointer
   *  TChannelSpectrumProperties const * const channelSpectrumPropertiesPointer = * channelSpectrumPropertiesObjectHandler; // It it BAD to store the pointer;
   *  
   *  mySpectrumPropertiesRunInfo.getChannelPropertiesTree()->GetEntry(2); // get another object in the tree
   *
   *  // Attention: channelSpectrumPropertiesPointer still points to the first object, which might be
   *  // invalid. A nive way to create segfaults if you access it.
   *
   *  // correct: dereference when accessising the data
   *  Double_t mean = (*channelSpectrumPropertiesObjectHandler)->_mean;
   *
   *  \endcode
   */
  TChannelSpectrumProperties const * const *  getSpectrumPropertiesTreeObject();

  /** Returns a THStack containing two histograms: One with the RMS spectrum and the second
   *  only with the part being cut as dead (in red). For convenience they are put into 
   *  a THStack to draw them using \c hStack->Draw("nostack"). Please note that drawing the
   *  histograms stacked (i. e. without the \c nostack option) does not make sense.
   * 
   *  The Draw command is executed before the THStack is returned. It just draws to the currently
   *  active pad. The stack with the histograms is returned to facilitate modifications.
   *
   *  The function hands over the ownership of the THStack and of the histograms. The histograms
   *  are not owned by the stack. Delete the histograms and the stack if you don't need them
   *  any more to avoid memory leaks.
   */
  THStack * drawGetDeadChannelsPlot( Double_t deadCut = 20.5, Double_t xAxisMax = 70. );

  /** Returns the run number.
   */
  Int_t getRunNumber() const;

 protected:
  Int_t _runNumber; //< The run  number of this run
  //  TString _runFileName; //< The string containing the file name for _runFile
  TFile * _runFile; //< The file with the histograms per channel for one run

  SimpleMapper * _simpleMapper; //< The instance of the SimpleMapper object.

  TTree * _spectrumPropertiesTree;  //< The internal pointer to the channel properties tree;
  TChannelSpectrumProperties * _channelSpectrumProperties; //< The data object for the tree.

  /** A map to store the ChannelID and the associated histogram titles.
   *  This is necessary as the title contains the serial number which
   *  is never mapped anywhere and discarded when reading in the histo.
   *  This map avoids scanning all keys of the file when accessing individual 
   *  histograms.
   */      
  std::map< ChannelID, std::string > _histogramTitleMap;

  /** A map for random access to the TChannelSpectrumPropertiess.
   *  This is a duplication of the data in the tree. We pay this price for 
   *  fast random access, not having to search the tree for every request.
   */
   /*  N.B. The map is not the fastest implementation you can think of.
   *  As the indices are contiuous a vector with direct access would be faster.
   *  However this means using the global channel number, which can be changed during
   *  run time. So this map would have to be updated whenever nChannelsPerChip and
   *  nChipsPerModule is changed. To keep the consistency simple we just use a map.
   */
  std::map< ChannelID, TChannelSpectrumProperties > _channelSpectrumPropertiesMap;

  static Int_t _nChannelsPerChip; //< The number of channels per chip.
  static Int_t _nChipsPerModule; //<< The number of chips per readout module.
 
  //< Helper function to avoid code duplication. Called from the constructors
  bool openRunFile(std::string runFileName);
  bool createdTree; // flag to remember if the constructor created the tree (needed in destructor)

  /// The mask for the module types to accept
  unsigned int _moduleTypeMask;
};

#endif //  SPECTRUM_PROPERTIES_RUN_INFO_H
