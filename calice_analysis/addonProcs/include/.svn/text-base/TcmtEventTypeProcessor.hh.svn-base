#ifndef TcmtEventTypeProcessor_hh
#define TcmtEventTypeProcessor_hh

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"

// c++ includes
#include <string>

// CALICE includes
#include "TcmtEventIdentifier.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor that analyses the TCMT event data and returns a bit reflecting the result
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> InputCollection </em></b></td><td> name of the input collection </td></tr>
   * <tr><td><b><em> TcmtEventBitName </em></b></td><td> name of event parameter that will contain the output bits</td></tr>
   * <tr><td><b><em> OverwriteEncoding </em></b></td><td> encoding string used instead of encoding string of collection (optional) </td></tr>
   * </table>
   *
   * @author Benjamin.Lutz@desy.de
   * @version 1.0
   * @date November 2009
   */
  class TcmtEventTypeProcessor : public Processor {
  public:

    TcmtEventTypeProcessor();

    virtual Processor*  newProcessor() { return new TcmtEventTypeProcessor ; }

    /* Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /* Called for every event.
     */
    virtual void processEvent( LCEvent * evt ) ;

    /* Called after data processing for clean up.
     */
    virtual void end() ;

  private:

    std::string _parNameTcmtEventBit;
    std::string _inputColName;
    std::string _alternativeEncoding;

    TcmtEventIdentifier *_tcmtEventIdentifier;

  };

} // end namespace CALCICE

#endif
