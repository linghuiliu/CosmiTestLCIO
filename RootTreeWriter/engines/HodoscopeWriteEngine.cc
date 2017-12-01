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
                                                   "run - 48 have wrong hod numbers",
                                                   isModInverted,
                                                   (bool) false);

    _hostProcessor.relayRegisterProcessorParameter("HodoscopeWriteEngine_Height1",
                                                   "layer number for hodoscope 1",
                                                   height1,
                                                   0);

    _hostProcessor.relayRegisterProcessorParameter("HodoscopeWriteEngine_Height2",
                                                   "layer number for hodoscope 2",
                                                   height2,
                                                   13);


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
         sprintf(fname, "/afs/desy.de/group/flc/pool/liulingh/calice/RootTreeWriter/engines/onePC%d.txt",i+1);
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
      corrF1 = new TF1("corrF1","8./TMath::Pi()*TMath::ATan(([0]*TMath::CosH([0]*(7.5-x))*TMath::Sin(TMath::Pi()/8.*(7.5-x))-TMath::Pi()/8.*TMath::SinH([0]*(7.5-x))*TMath::Cos(TMath::Pi()/8*(7.5-x)))/([0]*TMath::Exp(8.*[0])+TMath::Pi()/8.*TMath::SinH([0]*(7.5-x))*TMath::Sin(TMath::Pi()/8.*(7.5-x))+[0]*TMath::CosH([0]*(7.5-x))*TMath::Cos(TMath::Pi()/8.*(7.5-x))))+x",-0.5,7.5);
      corrF1->SetParameter(0,0.317);

      corrF2 = new TF1("corrF2","-8./TMath::Pi()*TMath::ATan(([0]*TMath::CosH([0]*(x-75.5))*TMath::Sin(TMath::Pi()/8.*(x-75.5))-TMath::Pi()/8.*TMath::SinH([0]*(x-75.5))*TMath::Cos(TMath::Pi()/8*(x-75.5)))/([0]*TMath::Exp(8.*[0])+TMath::Pi()/8.*TMath::SinH([0]*(x-75.5))*TMath::Sin(TMath::Pi()/8.*(x-75.5))+[0]*TMath::CosH([0]*(x-75.5))*TMath::Cos(TMath::Pi()/8.*(x-75.5))))+x",75.5,83.5);
      corrF2->SetParameter(0,0.317);
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

    hostTree->Branch(string(_prefix[ihod]+"trueRecoX").c_str(), &_hitsFill.trueRecoX[ihod], 
		     string(_prefix[ihod]+"trueRecoX/D").c_str());

    hostTree->Branch(string(_prefix[ihod]+"trueRecoY").c_str(), &_hitsFill.trueRecoY[ihod], 
		     string(_prefix[ihod]+"trueRecoY/D").c_str());

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
            int channel = itr-adc.begin();
            if (channel>=64) {
              break;
            }
            _hitsFill.adc[ihod][channel] = *itr;
            _hitsFill.nph[ihod][channel] = adc2nph(*itr,ihod,channel);
          }
          recoverDeadCh();

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
      inCol = evt->getCollection( _ahcColName );
      std::string encoding = inCol->getParameters().getStringVal( "CellIDEncoding");
      if (encoding == "")
      {
        LCParameters &param = inCol->parameters();
        //set default encoding if no encoding present
        param.setValue(LCIO::CellIDEncoding, "M:3,S-1:3,I:9,J:9,K-1:6");
        encoding = inCol->getParameters().getStringVal( "CellIDEncoding");
      }
      CellIDDecoder<CalorimeterHit> decoder(inCol);
      bool hasKminus1 = false;
      if (encoding.find("K-1") != std::string::npos)
      {
        hasKminus1 = true;
      }
      int nHits=0;
      for ( int cellCounter = 0; cellCounter < inCol->getNumberOfElements() && cellCounter < (int)MAXCELLS; cellCounter++ )
      {
        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inCol->getElementAt(cellCounter));
        _ahcHits.hitI[nHits] = decoder(hit)["I"];
        _ahcHits.hitJ[nHits] = decoder(hit)["J"];

        if (hasKminus1) _ahcHits.hitK[nHits] = decoder(hit)["K-1"]+1;
        else            _ahcHits.hitK[nHits] = decoder(hit)["K"];

        _ahcHits.hitEnergy[nHits] = hit->getEnergy();
        
        const float *hitPos = hit->getPosition();
        _ahcHits.hitPos[nHits][0] = hitPos[0];
        _ahcHits.hitPos[nHits][1] = hitPos[1];
        _ahcHits.hitPos[nHits][2] = hitPos[2];
        
        nHits++;
      }
      _ahcHits.nHits = nHits;

      int maxPass = 0;
      int maxNX1 =  0;
      int maxNX2 =  0;
      int maxNY1 =  0;
      int maxNY2 =  0;
      double minDis = 0.;

      for (int nx1=0;nx1<_hitsFill.nRecoX[0];nx1++) {
        for (int ny1=0;ny1<_hitsFill.nRecoY[0];ny1++) {
          for (int nx2=0;nx2<_hitsFill.nRecoX[1];nx2++) {
            for (int ny2=0;ny2<_hitsFill.nRecoY[1];ny2++) {
              int passPts = 0;
              double dissum = 0.;
              for (nHits=0;nHits<_ahcHits.nHits;nHits++) {
                if (_ahcHits.hitEnergy[nHits]<350) continue;
                double distance = trackDistance(_hitsFill.recoX[0][nx1],_hitsFill.recoY[0][ny1],_hitsFill.recoX[1][nx2],_hitsFill.recoY[1][ny2],_ahcHits.hitPos[nHits]);
                if (distance < 30.) {
                  passPts++;
                  dissum += distance*distance;
                }
              }
              if (passPts>maxPass || (passPts==maxPass && dissum<minDis)) {
                maxPass = passPts;
                maxNX1  = nx1;
                maxNY1  = ny1;
                maxNX2  = nx2;
                maxNY2  = ny2;
                minDis  = dissum;
              } 
            }
          }
        }
      }

      if (maxPass == 0) {
        _hitsFill.trueRecoX[0] = -10000;
        _hitsFill.trueRecoY[0] = -10000;
        _hitsFill.trueRecoX[1] = -10000;
        _hitsFill.trueRecoY[1] = -10000;
      } else {
        _hitsFill.trueRecoX[0] = _hitsFill.recoX[0][maxNX1];
        _hitsFill.trueRecoY[0] = _hitsFill.recoY[0][maxNY1];
        _hitsFill.trueRecoX[1] = _hitsFill.recoX[1][maxNX2];
        _hitsFill.trueRecoY[1] = _hitsFill.recoY[1][maxNY2];
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
      _hitsFill.nRecoX[ihod] = 0;
      _hitsFill.nRecoY[ihod] = 0;
      for (int i=0;i<8;i++){
        _hitsFill.recoX[ihod][i] = 0;
        _hitsFill.recoY[ihod][i] = 0;
      }
      _hitsFill.trueRecoX[ihod] = 0;
      _hitsFill.trueRecoY[ihod] = 0;

    }
  }
  
  double HodoscopeWriteEngine::adc2nph( int adc, int ihod, int ch ) 
  {
    
    double nph = (1.0*adc-pedestal[ihod][ch])/onePC[ihod][ch];

    if (nph<0) nph=0;

    return nph;
  }

  void HodoscopeWriteEngine::recoverDeadCh() {
    if(_hitsFill.nph[0][33] > 1 && _hitsFill.nph[0][35] > 1) {
      _hitsFill.nph[0][61] = _hitsFill.nph[0][34]*(_hitsFill.nph[0][60]+_hitsFill.nph[0][62])/(_hitsFill.nph[0][33]+_hitsFill.nph[0][35]);
    }
    if(_hitsFill.nph[1][7] > 1 && _hitsFill.nph[1][9] > 1) {
      _hitsFill.nph[1][20] = _hitsFill.nph[1][8]*(_hitsFill.nph[1][19]+_hitsFill.nph[1][21])/(_hitsFill.nph[1][7]+_hitsFill.nph[1][9]);
    }
    if(_hitsFill.nph[1][10] > 1 && _hitsFill.nph[1][12] > 1) {
      _hitsFill.nph[1][23] = _hitsFill.nph[1][11]*(_hitsFill.nph[1][22]+_hitsFill.nph[1][24])/(_hitsFill.nph[1][10]+_hitsFill.nph[1][12]);
    }
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
        recoPos[axis][n]=(edgeCorrection(reco+n*16) - 41.5)*pitch;
        //recoPos[axis][n]=(reco+n*16 - 41.5)*pitch;
      }
    }
  }
  double HodoscopeWriteEngine::trackDistance(double x1, double y1, double x2, double y2, float * pos) {
    double z1, z2;
    z1 = height1 * 43.3;
    z2 = height2 * 43.3;

    double intercept[2];
    intercept[0] = ((pos[2]-z1)*x2 + (z2-pos[2])*x1) / (z2-z1);
    intercept[1] = ((pos[2]-z1)*y2 + (z2-pos[2])*y1) / (z2-z1);

    double distance = Sqrt((pos[0]-intercept[0])*(pos[0]-intercept[0])+(pos[1]-intercept[1])*(pos[1]-intercept[1]));
    return distance;
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
  double HodoscopeWriteEngine::edgeCorrection(double x) {
    if (x<8.5) {
      x = corrF1->GetX(x,-0.5,7.5);
    } else if (x>76.5) {
      x = corrF2->GetX(x,75.5,83.5);
    }
    return x;
  }

}/*namespace marlin*/

