#ifndef Ahc2CollectionMerger_h
#define Ahc2CollectionMerger_h 1

#include "marlin/Processor.h"
#include "lcio.h"

namespace CALICE {

  /**
  * @brief Processor which allows to merge collections from the same event (use for DD4HEP).
  *
  * @author eldwan.brianne@desy.de
  * @version 1.0
  * @date September 2016
  */

  class Ahc2CollectionMerger : public marlin::Processor {

  public:

    virtual Processor* newProcessor() { return new Ahc2CollectionMerger; }

    /**Constructor
    */
    Ahc2CollectionMerger();

    /**Destructor
    */
    ~Ahc2CollectionMerger(){};

    /**Initialise
    */
    virtual void init();

    /**Process event (the working horse)
    */
    virtual void processEvent(lcio::LCEvent *evt);

  protected:

    lcio::StringVec _CollectionInputNames; /**<name of the input collection to append*/
    std::string _CollectionOutputName; /**<name of the output collection after appending*/

    std::string _encoding;
  };

}

#endif
