/// This file contains classes and macros to assist the "registration"
/// and the runtime loading of engines.
/// @author Joergen Samson <joergen.samson@desy.de>
/// @date   Jan 2009

#ifndef __ENGINEREGISTRAR_HH__
#define __ENGINEREGISTRAR_HH__
#include "EngineLoaderProxy.hh"
#include <list>


class __RTW_Proxy_BASE;
namespace marlin
{
  class RootWriteEngine;
  class RootTreeWriter;
}

namespace __RTW
{

  class CompatVersionInfo
  {
  };
  
  class EngineVersionInfo
  {
  };
  
  class EngineRegistrar
  {
  public:
    static void RegisterProxy( __RTW_Proxy_BASE* );
    static EngineRegistrar& TheInstance();

    template< class List >
    List& appendAllEngines( List& appendList, marlin::RootTreeWriter* hostProc );

  protected:
    void doRegisterProxy( __RTW_Proxy_BASE* proxy );

  private:
    typedef std::list<__RTW_Proxy_BASE*> ProxyBaseList;
    ProxyBaseList _proxyList;
    
  };



} // namespace _RTW

#endif //__ENGINEREGISTRAR_HH__
