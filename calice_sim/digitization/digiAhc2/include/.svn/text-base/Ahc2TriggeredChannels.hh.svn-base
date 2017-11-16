#ifndef Ahc2TriggeredChannels_hh
#define Ahc2TriggeredChannels_hh 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>

#include "Ahc2CalibrationsProcessor.hh"
#include "Mapper.hh"
#include "Ahc2Mapper.hh"
#include "Ahc2Calibrations.hh"

#include <TH1.h>
#include <TFile.h>

using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace CALICE
{

  /**
  * @brief Class for counting the number of triggered channels per Chip (rejecting dead channels) used for time smearing
  *
  * @author eldwan.brianne@desy.de
  * @version 2.0
  * @date November 2016
  */

  class Ahc2TriggeredChannels : public Processor
  {
  private:

  public:

    virtual Processor*  newProcessor() { return new Ahc2TriggeredChannels ; }

    /**Constructor
    */
    Ahc2TriggeredChannels() ;

    /**Destructor
    */
    ~Ahc2TriggeredChannels() {};

    /**Initialise
    */
    virtual void init() ;

    /**Process header
    */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /**Process event (the working horse)
    */
    virtual void processEvent( LCEvent * evt ) ;

    /**Check
    */
    virtual void check( LCEvent * evt ) ;

    /**End of processing
    */
    virtual void end() ;

  protected:
    std::string _calorimInpCollection;/**<input collection name*/
    std::string _calorimOutCollection;/**<output collection name*/
    std::string _mappingProcessorName;        /**<name of the processor which provides the mapping*/

    std::string _encoding;

    int _nRun ;/**<run number*/
    int _nEvt ;/**<event number*/

    const Ahc2Mapper* _mapper;                               /**<the mapper*/

    std::map<int, int> m_ChipTrigger; /**<map with number of triggered channels per chip for pedestal shift*/
    IntVec nHitsPerChipAboveThr;/**<vector to store the number of hits per Chips per event*/
  } ;
}
#endif
