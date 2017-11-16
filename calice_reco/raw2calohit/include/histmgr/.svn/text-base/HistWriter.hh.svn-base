#ifndef _HistWriter_hh_
#define _HistWriter_hh_

#include "FloatHistogram1D.hh"
#include "FloatHistogram2D.hh"

namespace EVENT {
  class LCGenericObject;
}

namespace histmgr {

/** Abstract interface of a class to write histograms to disk.
 * @sa RootWriter.
 */
class HistWriter
{
public:
  virtual ~HistWriter() {}

  /** Enter a subdirectory in the file.
   * @param name the name of the sub directory
   * @param create set to true if non existing directories shall be created
   * @return true incase the directory exists
   */
  virtual bool enterDir(const std::string &name, Bool_t create=false) =0;

  /** Go up one directory level.
   */
  virtual void upDir() =0;
  
  /** Write the given histogram to a ROOT file.
   * @param hist reference to the histogram
   * @param name the name given to the histogram
   */
  virtual void writeToCurrentDir(const FloatHistogram1D &hist, const std::string &name) = 0;


  /** Write the given histogram to a ROOT file.
   * @param hist reference to the histogram
   * @param name the name given to the histogram
   */
  virtual void writeToCurrentDir(const FloatHistogram2D &hist, const std::string &name) = 0;

  /* Wrtite the given error graph to a ROOT file.
   * @param array_x a double precision float value array for the x-axis.
   * @param array_y a single precision float value array for the y-axis.
   * @param array_ey an optional single precision float value array for the y-errors.
   * @param name the name given to the graph
   */
  virtual void writeToCurrentDir(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const EVENT::LCGenericObject *array_ey, const std::string &name) =0;

  
};

}
#endif
