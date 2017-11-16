#ifndef HstBase_HH
#define HstBase_HH

#include <string>

#include "TROOT.h"
#include "TApplication.h"

#include "RcdRecord.hh"


class HstBase {

public:
  HstBase(bool i=true) : _interactive(i), _application(0), _printLevel(1) {

    if(_interactive) {
      _application=new TApplication("HstBase Application",0,0);
      gROOT->Reset();
    }

    gROOT->SetStyle("Plain");
  }

  virtual ~HstBase() {
    delete _application;
    _application=0;
  }

  virtual unsigned printLevel() const {
    return _printLevel;
  }

  virtual void printLevel(unsigned p) {
    _printLevel=p;
  }

  virtual bool record(const RcdRecord &r) = 0;

  virtual bool update() = 0;

  virtual bool postscript(std::string) = 0;

  bool interactive() const {
    return _interactive;
  }

private:
  const bool _interactive;
  TApplication *_application;
  unsigned _printLevel;
};

#endif
