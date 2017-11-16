#ifndef COL_PAR_HELPER
#define COL_PAR_HELPER

#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include <string>

void setCollectionParameters(lcio::LCCollectionVec* col, 
                             std::string parameterConfigFile);

#endif
