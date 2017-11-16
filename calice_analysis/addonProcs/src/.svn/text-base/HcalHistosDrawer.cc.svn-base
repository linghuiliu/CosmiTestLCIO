#include "HcalHistosDrawer.hh"

#include "TROOT.h"
#include "TSystem.h"

#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCGenericObject.h"
#include "UTIL/CellIDDecoder.h"

using std::cout;
using std::endl;

/**************************************************************************************/
/*                                                                                    */
/*                                                                                    */
/*                                                                                    */
/**************************************************************************************/
HcalHistosDrawer::HcalHistosDrawer()
{
  int argc = 0;
  char *argv = (char *)"";

  _theApp = new TApplication("bob", &argc, &argv);
  _theApp->SetReturnFromRun(kTRUE);
  
  gROOT->SetStyle("Plain");
  _fCanvas = new TCanvas("HCAL","HCAL", 750, 750);
  _fCanvas->Draw();
  gSystem->ProcessEvents();

}


/**************************************************************************************/
/*                                                                                    */
/*                                                                                    */
/*                                                                                    */
/**************************************************************************************/
HcalHistosDrawer::~HcalHistosDrawer()
{

}

/**************************************************************************************/
/*                                                                                    */
/*                                                                                    */
/*                                                                                    */
/**************************************************************************************/
void HcalHistosDrawer::fillHcalHistos(LCEvent *evt, std::string hcalColName, std::string showerStartColName)
{
  int startLayer = this->getMarinaShowerStartHcalLayer(evt, showerStartColName);
  
  LCCollection *hcalCol = NULL;
  try
    {	
      hcalCol = evt->getCollection(hcalColName);

     _hEnergyPerLayer = new TH1F("hEnergyPerLayer", "HCAL energy sum vs layer number",     40, 0, 40);
      
      std::vector<int> nHcalHitsPerLayer(_nLayers, 0);
      std::vector<double> energyPerLayer(_nLayers, 0);
      
      CellIDDecoder<CalorimeterHit> myCellIDDecoder(hcalCol);

      float energySum = 0;
      
      /*-------------------------------------------------------------*/
      for (int i = 0; i < hcalCol->getNumberOfElements(); ++i)
	{
	  CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(hcalCol->getElementAt(i));
	  int layerNum = myCellIDDecoder(hit)["K-1"]+1;
	  nHcalHitsPerLayer[layerNum-1]++;
	  energyPerLayer[layerNum-1] += hit->getEnergy();
	  energySum += hit->getEnergy();
	  
	}/*------------- end loop over HCAL hits --------------------*/
      
      int nLayerMaxEnergy = 0;
      double maxEnergy = 0;
      
      for (int iLayer = 0; iLayer < _nLayers; ++iLayer)
	{
	  _hEnergyPerLayer->Fill(iLayer+1, energyPerLayer[iLayer]);

	  if (energyPerLayer[iLayer] > maxEnergy)
	    {
	      maxEnergy = energyPerLayer[iLayer];
	      nLayerMaxEnergy = iLayer;
	    }
	}
      
      cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
      cout<<" HcalHistosDrawer:"<<endl<<endl;
      cout<<" The layer with MAX energy: "<<(nLayerMaxEnergy+1)<<", energy: "<<maxEnergy<<" MIPs"<<endl;
      cout<<" The shower starts in layer: "<<startLayer<<endl;
      cout<<" Total energy sum per event: "<<energySum<<" MIPs"<<endl;
      //cout<<" layer_max - layer_start = "<<(nLayerMaxEnergy+1 - startLayer)<<endl;
      cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;

      _hEnergyPerLayer->Draw();
      
      gPad->SetGridx();
      gPad->SetGridy();
      
      /*update the canvas and the ROOT application*/
      _fCanvas->Update();
      gSystem->ProcessEvents();
      _theApp->SetReturnFromRun(kTRUE);
      

      /*clear the vectors*/
      energyPerLayer.clear();
      
      this->clear();
      
    }
  catch(EVENT::DataNotAvailableException err)
    {
      cout<<"Collection "<<hcalColName<<" not available in event "<<evt->getEventNumber()<<endl;
    }

}

/**************************************************************************************/
/*                                                                                    */
/*                                                                                    */
/*                                                                                    */
/**************************************************************************************/
void HcalHistosDrawer::clear()
{
  /*clear the histograms, to avoid the warning about replacing histograms with the same name
   */
  delete _hEnergyPerLayer;

}

/*******************************************************************************************/
/*                                                                                         */
/*                                                                                         */
/*                                                                                         */
/*******************************************************************************************/
int HcalHistosDrawer::getMarinaShowerStartHcalLayer(LCEvent *evt, std::string showerStartColName)
{
  LCCollection *col = NULL;
  LCGenericObject *showerStartLayerObj = NULL;
  
  int showerStartHcalLayer = -99;
  
  try{
    col = evt->getCollection(showerStartColName);
    int nEntries = col->getNumberOfElements();
    
    for (int i = 0; i < nEntries; ++i)
      {
	showerStartLayerObj = dynamic_cast<LCGenericObject*>(col->getElementAt(i));
	
	/*calorimeter type: 0=ECAL, 1=first 30 layers in HCAL, 2=last coarse part of HCAL*/
	int type = showerStartLayerObj->getIntVal(0);
	
	/*shower starting layer number (in each calorimeter)*/
	int showerStart = showerStartLayerObj->getIntVal(1);
	
	  
	if (type == 1) /*first 30 layers in HCAL*/
	  {
	    showerStartHcalLayer = showerStart + 1;
	    //showerStartHcalLayer = showerStart;
	  }
	else if (type == 2)/*last 8 layers in HCAL*/
	  {
	    showerStartHcalLayer = 30 + showerStart + 1;
	    //showerStartHcalLayer = 30 + showerStart;
	    if (showerStartHcalLayer == 39) 
	      {
		//std::cout<<"\n\n Warning, showerStartHcalLayer = 39"<<std::endl;
		showerStartHcalLayer = 38;
	      }
	  }
	
	//float averageX = showerStartLayerObj->getFloatVal(0);
	//float averageY = showerStartLayerObj->getFloatVal(1);
	
	
      }
  }
  catch(EVENT::DataNotAvailableException err){}
  
  return showerStartHcalLayer;
  
}
