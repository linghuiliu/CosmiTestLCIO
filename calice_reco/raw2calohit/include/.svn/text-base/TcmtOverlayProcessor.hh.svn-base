#ifndef TcmtOverlayProcessor_h
#define TcmtOverlayProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <stdexcept>

#include <lcio.h>
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include "BaseMappingIIProcessor.hh"

namespace CALICE {

class TcmtOverlayProcessor : public BaseMappingIIProcessor {

public:
  TcmtOverlayProcessor();
  virtual ~TcmtOverlayProcessor() {};

  TcmtOverlayProcessor* newProcessor(){return new TcmtOverlayProcessor;}

  virtual void init();
  virtual void processEvent(lcio::LCEvent* evt);
  virtual void end();

protected:
  std::string _noiseColName;
  int _noiseColType;

  float _mipGev;
  float _energyThreshold;

  float _maxDiff;
  int _maxDiffEvt;
};

}

#endif
