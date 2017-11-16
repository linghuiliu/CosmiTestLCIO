#ifndef __BIFWRITEENGINE_HH__
#define __BIFWRITEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"

#include <string>

#include "TTree.h"

namespace marlin
{
  class BifWriteEngine: public RootWriteEngine
  {
  public:
    
    BifWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
					   _engineName("BifWriteEngine")
    {}
    
    
    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );
    
    virtual void fillVariables( EVENT::LCEvent* );

    const static unsigned int MAXTRIGGERS  = 100;/*should be enough for the BIF!*/

    struct
    {
      unsigned long long int StartAcq; /**< Timestamp of the start acquisition */
      unsigned long long int StopAcq; /**< Timestamp of the stop acquisition */
      int nTrigger; /**< Number of BIF Triggers */
      int source[MAXTRIGGERS]; /**< Source of the Trigger (3 for Scint) */
      int BXID[MAXTRIGGERS]; /**< BXID of the Trigger */
      float Time[MAXTRIGGERS]; /**< Absolute Time of the Trigger */

    } _bifFill;

  private:
    void resetBifFill();

    std::string _engineName;
    std::string _enginePrefix;

    /* processor parameters*/
    std::string _prefix;
    std::string _bifColName;

  };

}/*namespace marlin*/

#endif /* __BIFWRITEENGINE_HH__*/
