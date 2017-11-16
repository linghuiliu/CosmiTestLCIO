#ifndef _HistWriterKit_hh_
#define _HistWriterKit_hh_

#include "HistWriter.hh"
#include <string>

namespace histmgr {

  /** Abstract interface of a histogram writer kit.
   * The writer kit is used to create the actual histogram writer.
   * @sa RootWriterKit
   */
  class HistWriterKit
  {
  public:
    /** Destructor.
     * Abstract classes need a virtual destructor.
     */
    virtual ~HistWriterKit() {};

    /** create a histogram write.
     * @param name the name of the file to which the histograms should be written
     * @return a pointer a histogram writer which should be ready to write histograms.
     */
    virtual HistWriter *createWriter(const std::string &name) = 0;
  };

}
#endif
