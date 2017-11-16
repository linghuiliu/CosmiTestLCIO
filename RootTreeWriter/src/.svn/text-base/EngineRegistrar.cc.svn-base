#include "EngineRegistrar.hh"
#include "EngineLoaderProxy.hh"

#include "streamlog/streamlog.h"
#include "RootWriteEngine.hh"
#include <vector>
#include <algorithm>

using namespace std;
namespace __RTW
{

  EngineRegistrar& EngineRegistrar::TheInstance()
  {
    //static EngineRegistrar instance;
    // the above should be fine in my case: 
    // http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14
    // ISO/IEC14882 3.6.2, 3.7
    // but why bother just to make Valgrind happy?
    static EngineRegistrar* instance_p = new EngineRegistrar;
    return *instance_p;
  }

  void EngineRegistrar::RegisterProxy( __RTW_Proxy_BASE* aProxy )
  {
    TheInstance().doRegisterProxy( aProxy );
  }


  void EngineRegistrar::doRegisterProxy( __RTW_Proxy_BASE* aProxy )
  {
    /// \todo fixme: Space for lots of improvements. Check for "double"
    /// registration. Version checking already here? \dots
    _proxyList.push_back( aProxy );
  }




  template< class List_t >
  List_t& EngineRegistrar::appendAllEngines( List_t& appendList, marlin::RootTreeWriter* hostProc )
  {
    for ( ProxyBaseList::iterator proxit = _proxyList.begin();
	  proxit != _proxyList.end(); ++proxit )
      {
	/// \todo fixme: pass engine name given in proxy to the engine. Aka. 
	/// rely on "engineName" == "proxyClassName"
	std::string newEngineName = (*proxit)->getRegisterEngineName();
	typename List_t::iterator foundHere = std::find_if(  appendList.begin(),
							     appendList.end(),
							     RTW::EngineNameIs(newEngineName) );
	if ( foundHere != appendList.end() )
	  {
	    streamlog_out_T( WARNING ) << "Engine ["<<newEngineName<<"] already in list of engines. "
	      "Will _not_ add engine twice! Please take care that the correct engine is used. If "
	      "you have a smart idea how to handle this case, please file a feature-request to the "
	      "RootTreeWriter bug tracker." << std::endl;
	    continue;
	  }

	marlin::RootWriteEngine* newEngine = 
	  (*proxit)->constructNewEngine( hostProc );
	appendList.push_back( newEngine );
	streamlog_out_T( DEBUG ) << "Add engine ["<<newEngine->getEngineName()<<"]" << std::endl;
      }
    return appendList;
  }

  typedef std::vector<marlin::RootWriteEngine*>  WriteEngineVec;
  template WriteEngineVec& EngineRegistrar::appendAllEngines( WriteEngineVec& appendList, marlin::RootTreeWriter* hostProc );





}// namespace __RTW
