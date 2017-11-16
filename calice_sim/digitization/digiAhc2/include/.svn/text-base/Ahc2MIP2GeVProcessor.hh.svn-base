#ifndef AHC2MIP2GEVPROCESSOR_HH
#define AHC2MIP2GEVPROCESSOR_HH 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>
#include <map>

//ROOT
#include <TH1.h>
#include <TString.h>
#include <TFile.h>

using namespace lcio ;
using namespace marlin ;
using std::vector;

namespace CALICE
{
  /**
  * @brief Class doing the conversion between GeV and MIP + Correction of K layer (not smart!!)
  *
  *
  * @author eldwan.brianne@desy.de
  * @version 1.0
  * @date November 2015
  */

  class Ahc2MIP2GeVProcessor : public Processor {

  private:
    //processor parameters:
    float _MIPvalue; /**<MIP conversion factor in GeV*/
    std::string _rootfilename; /**<rootfile name*/

  public:

    virtual Processor*  newProcessor() { return new Ahc2MIP2GeVProcessor ; }

    /**Constructor
    */
    Ahc2MIP2GeVProcessor() ;

    /**Destructor
    */
    ~Ahc2MIP2GeVProcessor() {};

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

    /**Print Parameters
    */
    virtual void printParameters();

  protected:
    std::string _calorimInpCollection; /**<input collection name*/
    std::string _calorimOutCollection; /**<output collection name*/

    std::string _encoding;

    bool _debug; /**<flag to write rootfile*/

    TFile *rootfile; /**<rootfile*/
    std::map<int, TH1F*> m_hSpectra; /**<histogram per layer of energy*/

    StringVec _KVector; /** Vector of string containing the relation between K in Simulation and K in data*/
    std::map<int, int> _mapKs; /**<map containing SimK and K to convert*/

    int _nRun ; /**<run number*/
    int _nEvt ; /**<evt number*/
  } ;
}
#endif
