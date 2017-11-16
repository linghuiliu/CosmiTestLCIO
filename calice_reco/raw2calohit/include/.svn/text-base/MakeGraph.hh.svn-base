#ifndef _MakeGraphs_hh_

#include <vector>
class TGraph;

/** Create double precision graph from double and int vectors for x and y values.
 * Requires ROOT.
 */
TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<int> &y_arr);

/** Create double precision graph from double and unsigned int vectors for x and y values.
 * Requires ROOT.
 */
TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<unsigned int> &y_arr);

/** Create double precision graph from double and float vectors for x and y values.
 * Requires ROOT.
 */
TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<Float_t> &y_arr);

/** Create double precision graph from double vectors for x and y values.
 * Requires ROOT.
 */
TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<Double_t> &y_arr);


/** Create double precision graph with y-errors from double and float vectors for x and y values.
   * Requires ROOT.
   */
TGraph *makeGraph(const char *name, const std::vector<Double_t> &x_arr, const std::vector<Float_t> &y_arr, const std::vector<Float_t> &y_error_arr);

#endif
