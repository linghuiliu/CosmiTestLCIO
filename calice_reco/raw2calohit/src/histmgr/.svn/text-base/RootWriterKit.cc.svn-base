#include "RootWriterKit.hh"
#include <HistMgr.hh>
#include <iostream>

namespace histmgr {
RootWriterKit *RootWriterKit::__instance=new RootWriterKit;

RootWriterKit::RootWriterKit() 
{
  HistMgr *histMgr=HistMgr::getInstance();
  if (histMgr) {
    histMgr->registerWriter(std::string("ROOT"),this);
  }
}
}
