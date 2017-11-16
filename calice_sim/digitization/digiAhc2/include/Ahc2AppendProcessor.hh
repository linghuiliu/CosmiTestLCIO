#ifndef Ahc2AppendProcessor_h
#define Ahc2AppendProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"

namespace CALICE {

  /**  
   * @brief Processor which allows to append events from additional LCIO files.
   * 
   * @author eldwan.brianne@desy.de
   * @version 1.0
   * @date Decmeber 2015
   */

  class Ahc2AppendProcessor : public marlin::Processor {
  
  public:
  
    virtual Processor* newProcessor() { return new Ahc2AppendProcessor; }
  
    /**Constructor
     */
    Ahc2AppendProcessor();

    /**Destructor
     */
    ~Ahc2AppendProcessor(){};

    /**Initialise
     */
    virtual void init();
  
    /**Process event (the working horse)
     */
    virtual void processEvent(lcio::LCEvent *evt); 
  
  protected:

    lcio::StringVec _appendFileNames; /**<vector of names of input files*/
    lcio::StringVec _appendCollectionInputNames; /**<name of the input collection to append*/
    lcio::StringVec _appendCollectionOutputNames; /**<name of the output collection after appending*/

    lcio::LCReader* _lcReader; /**<Reader in LCIO to open a file*/
    bool _appendRepeatCollections; /**<continue appending after end of appending file (loop again over events)*/
  };

}

#endif
