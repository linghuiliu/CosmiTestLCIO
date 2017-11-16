#ifndef _TBTrackRemover_hpp
#define _TBTrackRemover_hpp 1

#include <string>
#include <fstream>

// -- LCIO headers
#include "lcio.h"
#include "UTIL/LCTime.h"
#include "EVENT/LCRunHeader.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"

#include <collection_names.hh>


#include "marlin/Processor.h"


using namespace std;
using namespace lcio ;
using namespace lccd ;
using namespace marlin ;
using namespace CALICE;


namespace marlin {

  /** Preprocessor which is supposed to run at the !very! beginning of the processor chain. 
    * Adding information to the Run and EventHeader.for data and MC.  
    * In partcular the beam energy is extracted from the database for a given run
    * and the MC events get assigned a timestamp.
    * @author: A.M. Magnan Imperial College, modifs R.Poeschl LAL
    *
    * @date Apr  2007
    */

class TBTrackRemover : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new TBTrackRemover ; }
  
  
  TBTrackRemover() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;


 private:

  /** Helper class to for the modification of the Event Header */
  class LCEventModifier : public IMPL::LCEventImpl {
  public:
    static void deleteCollection(LCEvent *r, const std::string &name) {
      //LCEventModifier *m((LCEventModifier*)r);
      LCEventModifier *m((LCEventModifier*)dynamic_cast<LCEventImpl*>(r));
      assert(m!=0);

      m->setReadOnly(false);

      m->removeCollection(name);

      m->setReadOnly(true);
    }
  };

} ;

}
#endif
