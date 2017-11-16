#ifndef MY_HCALHISTOSDRAWER_H
#define MY_HCALHISTOSDRAWER_H

#include "EVENT/LCEvent.h"
#include "lcio.h"

#include "TCanvas.h"
#include "TApplication.h"
#include "TH1F.h"

using namespace lcio;

/**Class to draw HCAL histograms event by event; 
   can be used with the EventDisplayProcessor
 */
class HcalHistosDrawer
{
public:
  /**Constructor
   */
  HcalHistosDrawer();
  
  /**Destructor
   */
  ~HcalHistosDrawer();


  void fillHcalHistos(LCEvent *evt, std::string hcalColName, std::string showerStartColName);
  int getMarinaShowerStartHcalLayer(LCEvent *evt, std::string showerStartColName);
  void clear();


private:
  TApplication* _theApp;
  TCanvas* _fCanvas;      /**<Canvas for displaying histograms*/

  static const int _nLayers = 38;

  TH1F *_hEnergyPerLayer;

};
#endif
