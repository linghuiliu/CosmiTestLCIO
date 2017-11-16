#ifndef _RootWriter_hh_
#define _RootWriter_hh_

class TFile;
class TDirectory;
class TH1;
class TH2;
class TGraph;
class FloatHistogram3D;

#include <Rtypes.h>
#include <vector>
#include "HistWriter.hh"
#include <string>

namespace histmgr {
  /** Write histograms to ROOT files.
   */
  class RootWriter : public HistWriter
  {
  public:
    /** create or update the given file and open for writing
     * @param file_name the name including the path name of the file.
     */
    RootWriter(const std::string &file_name);

    /** write everything to disk, close file and destruct object.
     */
    ~RootWriter();

    /** Enter a subdirectory in the file.
     * @param name the name of the sub directory
     * @param create set to true if non existing directories shall be created
     * @return true incase the directory exists
     */
    bool enterDir(const std::string &name, bool create=false);

    /** Go up one directory level.
     */
    void upDir();

    /** Write the given histogram to a ROOT file.
     * @param hist reference to the histogram
     * @param name the name given to the histogram
     */
    void writeToCurrentDir(const FloatHistogram1D &hist, const std::string &name);

    /** Write the given histogram to a ROOT file.
     * @param hist reference to the histogram
     * @param name the name given to the histogram
     */
    void writeToCurrentDir(const FloatHistogram2D &hist, const std::string &name);

    /** Write the given graph to a ROOT file.
     * @param array_x graph parameters along x
     * @param array_y graph parameters along y
     * @param name name of the graph to be written
     */
    void writeToCurrentDir(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const std::string &name);

    /* Wrtite the given error graph to a ROOT file.
     * @param array_x a double precision float value array for the x-axis.
     * @param array_y a single precision float value array for the y-axis.
     * @param array_ey an optional single precision float value array for the y-errors.
     * @param name the name given to the graph
     */
    void writeToCurrentDir(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const EVENT::LCGenericObject *array_ey, const std::string &name);

    static TH1 *createTH1(const FloatHistogram1D &hist, const std::string &name);
    static TH2 *createTH2(const FloatHistogram2D &hist, const std::string &name);
    static TGraph *createGraph(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const std::string &name);
    static TGraph *createGraph(const EVENT::LCGenericObject *array_x, const EVENT::LCGenericObject *array_y, const EVENT::LCGenericObject *array_ey, const std::string &name);
  protected:
    static TDirectory *enterDir(TDirectory *dir, const std::string &name);

    TFile *_file;
    std::vector<TDirectory *> _dirStack;
  };
}
#endif
