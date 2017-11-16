#ifndef _RootWriterKit_hh_
#define _RootWriterKit_hh_

#include <Rtypes.h>
#include <HistWriterKit.hh>
#include <RootWriter.hh>

namespace histmgr {
/** Create a histogram writer which writes the histograms to ROOT files.
 * @sa RootWriter.
 */
class RootWriterKit : public HistWriterKit
{
protected:
  RootWriterKit();
public:
  HistWriter *createWriter(const std::string &name) {return new RootWriter(name);};
  static RootWriterKit *__instance;
};
}
#endif
