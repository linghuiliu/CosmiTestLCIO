#include "CalHitModifier.hpp"
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "marlin/Processor.h"
#include "CalHitMapMgr.hpp"
#include "ModifierParameters.hpp"
#include "TempCalHit.hpp"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/RawCalorimeterHitImpl.h"
#include "IMPL/LCRelationImpl.h"
#include "marlin/StringParameters.h"
#include <list>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace digisim {

class Digitizer {

  /*
    -----Public Methods-----
  */
public:
  //Basic Constructor
  Digitizer(std::string name);
  // destructor
  ~Digitizer();

  //Initialize Digitizer: vector stores the parameters for each valid modifier
  void init(marlin::StringParameters* pars);

  //Initializes the digitizer and its parameters
  void init(std::vector<std::string> collnames, std::vector<ModifierParameters> modifiers);

  //Yet another initializer method.  Initializes the digitizer and its parameters
  void init(std::string inColl, std::string outColl, std::string linksColl, std::vector<ModifierParameters> modifiers);

  //Method to instantiate modifiers
  void initialize(std::vector<ModifierParameters> modifiers);

  //Called for every event - the event loop
  void process( LCEvent * evt );

  //Called after data processing for clean up
  void end();

  //Provides CellSelector for use in the modifiers
  //CellSelector getCellSelector();

  //Returns name of this digitizer
  std::string getName() { return _name; };



  /*
    -----Private Methods-----
  */
private:
  //private default constructor
  Digitizer() { };

  //Method to create a map of UNCALIBRATED temp hits
  std::map<long long, TempCalHit> createTempHits(const CalHitMap& simHits);

  //Method to create the output collections produced by the digitizer
  void createOutputCollections(const std::map<long long, TempCalHit> tmpHits,
			       LCCollection*& rawVec, LCCollection*& relVec);

  /*
    -----Data Members-----
  */

  //Run number
  int _nRun;

  //Event number
  int _nEvt;

  //Debug mode
  int _debug;

  //Name of this Digitizer object
  std::string _name;

  //modifiers
  std::vector<CalHitModifier*> _modifs;

  //name of input collection
  std::string _inputColl;

  //name of output collection
  std::string _outputColl;

  //name of raw2sim relations collection
  std::string _linksColl;

  //cell selector
  //CellSelector _selector;

  //histograms container
  //std::map<std::string, ICloud1D> cloudMap

};  //end class Digitizer
} //end namespace digisim
