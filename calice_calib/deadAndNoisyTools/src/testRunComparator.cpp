#include "SpectrumPropertiesRunInfo.h"
#include "RunComparator.h"
#include "TGraph.h"
#include "TH1D.h"
#include <unistd.h>
int main()
{
  SpectrumPropertiesRunInfo priPMNoise(360787,"AHCforCERN2010.cfg");
  SpectrumPropertiesRunInfo priPMLED(360789,"AHCforCERN2010.cfg");
  
  RunComparator myRunComparator( priPMNoise, priPMLED);

  myRunComparator.prepareGraph( &RunComparator::calculateChannelAndPedestalCorrectedMean );

  myRunComparator.getGraph()->Draw("AP"); //draw the graph
  sleep(2);

  myRunComparator.getXAxisHisto()->Draw(); //draw the histo
  sleep(2);

  myRunComparator.getYAxisHisto()->Draw(); //draw the histo
  sleep(2);

  return 0;
}
