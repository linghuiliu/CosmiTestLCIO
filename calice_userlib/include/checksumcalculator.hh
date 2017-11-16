#ifndef CHECKSUMCALCULATOR
#define CHECKSUMCALCULATOR 1
#include <stdexcept>
#include "lcio.h"
#include <EVENT/LCObject.h>
#include <EVENT/LCGenericObject.h>
#include "md5.hh" 

/** Calculates the checksum for an arbitrary LCGenericObject
  *  @author R. Pöschl DESY
  *  @date Nov 2005
  */
using namespace std;
using namespace lcio;

char* checksumcalculator(LCObject*); 

#endif
