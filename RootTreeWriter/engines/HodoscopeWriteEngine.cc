#include "HodoscopeWriteEngine.hh"

#include <cfloat>
#include <iostream>
#include <string>

#include "EVENT/CalorimeterHit.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/CellIDDecoder.h"

#include "ModuleLocation.hh"
#include "CellIndex.hh"
#include "HcalCellIndex.hh"
#include "AhcAmplitude.hh"

using namespace lcio;
using namespace std;
using namespace CALICE;
using namespace TMath;

#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

/* This engine can be run within the RootTreeWriter
 * it was tested for root 5.16.00
 * depending on the required information level it will fill
 * 0: integrated values only
 * 1: layer by layer information in addition
 * 2: radial information in addition
 *
 * Due to our weired mapping it is only running for the HCAL up to now
 */
namespace marlin
{
  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void HodoscopeWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter("HodoscopeWriteEngine_Prefix1",
                                                   "HodoscopeWriteEngine prefix to tree variables",
                                                   _prefix[0],
                                                   std::string("hod1"));

    _hostProcessor.relayRegisterProcessorParameter("HodoscopeWriteEngine_Prefix2",
                                                   "HodoscopeWriteEngine prefix to tree variables",
                                                   _prefix[1],
                                                   std::string("hod2"));

    _hostProcessor.relayRegisterProcessorParameter("HodoscopeWriteEngine_isModInverted",
                                                   "run -48 have wrong hod numbers",
                                                   isModInverted,
                                                   (bool) false);


    _hostProcessor.relayRegisterInputCollection(LCIO::CALORIMETERHIT,_engineName+"_InCol1",
						"Name of input collection",
						_hitColName[0], std::string("HodoscopeHits")  );

    _hostProcessor.relayRegisterInputCollection(LCIO::CALORIMETERHIT,_engineName+"_InCol2",
						"Name of input collection",
						_hitColName[1], std::string("HodoscopeHits")  );

    _hostProcessor.relayRegisterInputCollection(LCIO::CALORIMETERHIT,_engineName+"_AhcalCol",
						"Name of ahcal collection",
						_ahcColName, std::string("CalorimeterHits")  );

      for (int i=0;i<2;i++) {
         char fname[256];
         sprintf(fname, "/directorypath/RootTreeWriter/engines/onePC%d.txt",i+1);
         FILE *fp = fopen(fname,"r");
         float ped, opc;
         int j=0;
         while(fscanf(fp,"%f %f", &ped, &opc) != EOF) {
            onePC[i][j] = opc;
            pedestal[i][j] = ped;
            j++;
         }
         fclose(fp);
      }
   }


  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
   void HodoscopeWriteEngine::registerBranches( TTree* hostTree )
  {
  for (int ihod=0;ihod<2;ihod++) {
    if ( _prefix[ihod].size() > 0 )
      if ( _prefix[ihod][ _prefix[ihod].length()-1 ] != '_' )
	_prefix[ihod] += "_";

    /*---integrated values: information level 0---*/

    hostTree->Branch(string(_prefix[ihod]+"cycle").c_str(),     &_hitsFill.cycle[ihod], 
		     string(_prefix[ihod]+"cycle/I").c_str());
    hostTree->Branch(string(_prefix[ihod]+"tdc").c_str(),    &_hitsFill.tdc[ihod], 
		     string(_prefix[ihod]+"tdc/I").c_str());
    hostTree->Branch(string(_prefix[ihod]+"accept").c_str(),  &_hitsFill.accept[ihod], 
		     string(_prefix[ihod]+"accept/I").c_str());

    hostTree->Branch(string(_prefix[ihod]+"adc").c_str(), _hitsFill.adc[ihod], 
		     string(_prefix[ihod]+"adc[64]/I").c_str());

    hostTree->Branch(string(_prefix[ihod]+"nph").c_str(), _hitsFill.nph[ihod], 
		     string(_prefix[ihod]+"nph[64]/D").c_str());
/*
    hostTree->Branch(string(_prefix[ihod]+"pedestal").c_str(), pedestal[ihod], 
		     string(_prefix[ihod]+"pedestal[64]/D").c_str());

    hostTree->Branch(string(_prefix[ihod]+"onePC").c_str(), onePC[ihod], 
		     string(_prefix[ihod]+"onePC[64]/D").c_str());
*/
    hostTree->Branch(string(_prefix[ihod]+"nRecoX").c_str(), &_hitsFill.nRecoX[ihod], 
		     string(_prefix[ihod]+"nRecoX/I").c_str());

    hostTree->Branch(string(_prefix[ihod]+"nRecoY").c_str(), &_hitsFill.nRecoY[ihod], 
		     string(_prefix[ihod]+"nRecoY/I").c_str());

    hostTree->Branch(string(_prefix[ihod]+"recoX").c_str(), _hitsFill.recoX[ihod], 
		     string(_prefix[ihod]+"recoX["+_prefix[ihod]+"nRecoX]/D").c_str());

    hostTree->Branch(string(_prefix[ihod]+"recoY").c_str(), _hitsFill.recoY[ihod], 
		     string(_prefix[ihod]+"recoY["+_prefix[ihod]+"nRecoY]/D").c_str());

  }
  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void HodoscopeWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    LCCollection* inCol;
    
    try {
    for (int ih=0;ih<2;ih++) {
      inCol = evt->getCollection( _hitColName[ih] );
      int ihod;
      if(isModInverted) ihod = 1-ih;
      else ihod=ih;

      for ( int ielm = 0; ielm < inCol->getNumberOfElements(); ielm++ ) 
	{
	  LCObject *obj = inCol->getElementAt(ielm);
	  HodoscopeBlock lBlock(obj);
          
          cycle = lBlock.GetCycleN();
          tdc = lBlock.GetTDC();
          accept = lBlock.GetAccept();
          adc = lBlock.GetADC();

          _hitsFill.cycle[ihod] = cycle;
          _hitsFill.tdc[ihod] = tdc;
          _hitsFill.accept[ihod] = accept;
          for(auto itr = adc.begin(); itr != adc.end(); ++itr) {
            if (itr-adc.begin()>=64) {
              break;
            }
            _hitsFill.adc[ihod][itr - adc.begin()] = *itr;
            _hitsFill.nph[ihod][itr - adc.begin()] = adc2nph(*itr,ihod,itr-adc.begin());
          }

          reconstruct(ihod);
          _hitsFill.nRecoX[ihod] = nReco[0];
          for (int n=0;n<_hitsFill.nRecoX[ihod];n++) {
            _hitsFill.recoX[ihod][n] = recoPos[0][n];
          }
          _hitsFill.nRecoY[ihod] = nReco[1];
          for (int n=0;n<_hitsFill.nRecoY[ihod];n++) {
            _hitsFill.recoY[ihod][n] = recoPos[1][n];
          }
	}/* end loop over all hits*/
    }
    }/*try*/
    
    catch ( DataNotAvailableException err ) 
      {
	resetHitsFill();
	streamlog_out(DEBUG) <<  "HodoscopeWriteEngine WARNING: Collection "<< _hitColName[0]
	     << " not available in event "<< evt->getEventNumber() << endl;
      }/*catch*/
  }/*fillVariables*/

  void HodoscopeWriteEngine::resetHitsFill() {
  for (int ihod=0;ihod<2;ihod++) {
    _hitsFill.cycle[ihod] = 0;
    _hitsFill.tdc[ihod] = 0;
    _hitsFill.accept[ihod] = 0;

    for (int i=0; i<64; i++){
      _hitsFill.adc[ihod][i] = 0;
      _hitsFill.nph[ihod][i] = 0;
    }
  }
  }
  
  double HodoscopeWriteEngine::adc2nph( int adc, int ihod, int ch ) 
  {
    
    double nph = (1.0*adc-pedestal[ihod][ch])/onePC[ihod][ch];

    if (nph<0) nph=0;

    return nph;
  }
  
  void HodoscopeWriteEngine::reconstruct(int ihod) {
    double xsum,ysum;
  
    for (int axis=0;axis<2;axis++) {
      xsum=0;
      ysum=0;
      for(int fiber=0;fiber<16;fiber++) {
        int ch[2];
        chAt(fiber,ihod,axis,ch);
        double nphsum = _hitsFill.nph[ihod][ch[0]]+_hitsFill.nph[ihod][ch[1]];
        xsum += nphsum*Cos(thetaWith(fiber));
        ysum += nphsum*Sin(thetaWith(fiber));
      }
      double theta = argWith(xsum,ysum);
      double reco=fiberWith(theta);
      if (theta<0) {
        nReco[axis]=0;
      } else {
        nReco[axis]=(83.5-reco)/16+1;
      }
      for(int n=0;n<nReco[axis];n++) {
        recoPos[axis][n]=(reco+n*16)*5.;
      }
    }
  }
  void HodoscopeWriteEngine::chAt(int fiber, int ihod, int axis, int *ch) {
    if (ihod==0) {
      if (axis==0) {
        ch[0]=32+(fiber+12)%16;
        ch[1]=63-(fiber+12)%16;
      } else {
        ch[0]=15-(fiber+12)%16;
        ch[1]=16+(fiber+12)%16;
      }
    } else {
      if (axis==0) {
        ch[0]=fiber;
        ch[1]=16+(fiber+12)%16;
      } else {
        ch[0]=47-fiber;
        ch[1]=48+fiber;
      }
    }
  }
  double HodoscopeWriteEngine::thetaWith(int fiber) {
    return (fiber+0.5)*2.*Pi()/16.;
  }
  double HodoscopeWriteEngine::fiberWith(double theta) {
    return theta*16./2./Pi() - 0.5;
  }
  double HodoscopeWriteEngine::argWith(double x, double y) {
    if (x==0) return -1;
    double arg = ATan(y/x);
    if(x<0) arg += Pi();
    if(arg<0) arg += 2*Pi();
    return arg;
  }

}/*namespace marlin*/


