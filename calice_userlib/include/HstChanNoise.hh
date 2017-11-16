#ifndef HstChanNoise_HH
#define HstChanNoise_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "TROOT.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "TPostScript.h"

#include "HstBase.hh"
#include "HstTGraphErrors.hh"

#include "UtlAverage.hh"
#include "RcdRecord.hh"
#include "CrcLocationData.hh"
#include "CrcVlinkEventData.hh"
#include "DaqEvent.hh"
#include "SubAccessor.hh"


class HstChanNoise : public HstBase {

public:
  HstChanNoise(bool i=true) : HstBase(i) {
    for(unsigned i(0);i<22;i++) _slot[i]=false;
  }

  virtual ~HstChanNoise() {
  }

  void slotCanvas(unsigned slot) {
    std::cout << "slotCanvas called with slot " << slot << " Label = " << _label[slot] << std::endl;

    _slot[slot]=true;

    _canvas[slot]=new TCanvas(_label[slot].c_str(),_label[slot].c_str(),400+10*slot,10+10*slot,400,600);
    _canvas[slot]->Divide(1,2);

    //_graph[slot][0].Set(96);
    _graph[slot][0].SetTitle((_label[slot]+", Pedestal vs FE/Chip/Chan").c_str());
    //_graph[slot][0].SetMarkerColor(1);
    //_graph[slot][0].SetMarkerStyle(20);
      
    //_graph[slot][1].Set(96);
    _graph[slot][1].SetTitle((_label[slot]+", Noise vs FE/Chip/Chan").c_str());
    //_graph[slot][1].SetMarkerColor(1);
    //_graph[slot][1].SetMarkerStyle(20);
      
    for(unsigned fe(0);fe<8;fe++) {
      for(unsigned chip(0);chip<12;chip++) {
	for(unsigned chan(0);chan<18;chan++) {
	  std::ostringstream sout;
	  sout << _label[slot] << ", FE" << fe << ", Chip " << std::setw(2) << chip << ", Chan " << std::setw(2) << chan;
	  
	  _hist[slot][fe][chip][chan].SetNameTitle(sout.str().c_str(),sout.str().c_str());
	  _hist[slot][fe][chip][chan].SetBins(100,-30000,30000);
	}
      }
    }
  }

  bool postscript(std::string) {
    for(unsigned i(0);i<22;i++) {
      if(_slot[i]) {
	std::ostringstream sout;
	sout << "dps/chanNoiseSER0" << _label[i][4] << _label[i][5] << ".ps";
	_canvas[i]->Print(sout.str().c_str());
      }
    }
    return false;
  }

  bool update() { //bool ps=false) {
    std::cout << "Updating..." << std::endl;

    for(unsigned slot(5);slot<22;slot++) {
      if(_slot[slot]) {
	_graph[slot][0].Set(0);
	_graph[slot][1].Set(0);

	for(unsigned i(0);i<8;i++) {
	  for(unsigned chip(0);chip<12;chip++) {
	    for(unsigned chan(0);chan<18;chan++) {
	      unsigned bin(18*(12*i+chip)+chan);
	      _graph[slot][0].AddPoint(bin,_average[slot][i][chip][chan].average(),0.0,_average[slot][i][chip][chan].errorOnAverage());
	      _graph[slot][1].AddPoint(bin,_average[slot][i][chip][chan].sigma(),0.0,_average[slot][i][chip][chan].errorOnSigma());

	      _hist[slot][i][chip][chan].Reset();
	      _hist[slot][i][chip][chan].SetBins(100,unsigned(_average[slot][i][chip][chan].average())-50,unsigned(_average[slot][i][chip][chan].average())+50);
	      
	      _average[slot][i][chip][chan].reset();
	    }
	  }
	}
      }
    }

    for(unsigned i(0);i<22;i++) {
      if(_slot[i]) {
	_canvas[i]->Clear("D");
	_canvas[i]->cd(1);
	_graph[i][0].Draw("AP");
	_canvas[i]->cd(2);
	_graph[i][1].Draw("AP");
	_canvas[i]->Update();
      }
    }
    return true;
  }

  bool record(const RcdRecord &r) {

    if(r.recordType()==RcdHeader::runStart) {

      SubAccessor extracter(r);
      std::vector<const CrcLocationData<CrcVmeRunData>* > v(extracter.extract< CrcLocationData<CrcVmeRunData> >());

      for(unsigned i(0);i<v.size();i++) {
	if(!_slot[v[i]->slotNumber()]) {
	  std::ostringstream sout;
	  unsigned sn((v[i]->data()->epromHeader())&0xff);
	  if(sn<10) sout << "SER00" << sn << ", Slot " << std::setw(2) << (unsigned)v[i]->slotNumber();
	  else      sout << "SER0"  << sn << ", Slot " << std::setw(2) << (unsigned)v[i]->slotNumber();
	  _label[v[i]->slotNumber()]=sout.str();
	  slotCanvas(v[i]->slotNumber());
	}
      }
      return true;
    }

    if(r.recordType()!=RcdHeader::event) return true;
    
    SubAccessor extracter(r);
    std::vector<const CrcLocationData<CrcVlinkEventData>* > v(extracter.extract< CrcLocationData<CrcVlinkEventData> >());

    for(unsigned i(0);i<v.size();i++) {
      if(_slot[v[i]->slotNumber()]) {
        for(unsigned fe(0);fe<8;fe++) {
          const CrcVlinkFeData *fd(v[i]->data()->feData(fe));
          if(fd!=0) {
            for(unsigned chan(0);chan<v[i]->data()->feNumberOfAdcSamples(fe) && chan<18;chan++) {
              const CrcVlinkAdcSample *as(fd->adcSample(chan));
              if(as!=0) {
                for(unsigned chip(0);chip<12;chip++) {
		  _average[v[i]->slotNumber()][fe][chip][chan]+=as->adc(chip);
		  _hist[v[i]->slotNumber()][fe][chip][chan].Fill(as->adc(chip));
		}
	      }
	    }
	  }
	}
      }
    }
    return true;
  }

private:
  TCanvas *_canvas[22];

  //TGraphErrors _graph[22][2];
  HstTGraphErrors _graph[22][2];
  TH1D _hist[22][8][12][18];

  bool _slot[22];
  std::string _label[22];
  UtlAverage _average[22][8][12][18];
};

#endif
