/// This file contains classes and macros to assist the "registration"
/// and the runtime loading of engines.
/// @author Joergen Samson <joergen.samson@desy.de>
/// @date   Jan 2009

#ifndef __ENGINELOADERPROXY_HH__
#define __ENGINELOADERPROXY_HH__

#include <string>
namespace __RTW
{
  class EngineVersionInfo;
  class CompatVersionInfo;
  class EngineRegistrar;
}

namespace marlin
{
  class RootWriteEngine;
  class RootTreeWriter;
}

/// @cond INTERNALS

/// base class for engine register voodo. For internal use only.
class __RTW_Proxy_BASE
{
protected:
  __RTW_Proxy_BASE()          {}
public:
  virtual ~__RTW_Proxy_BASE() {}

public:
  /// \todo checkme:constructor of WriteEngine takes pointer to host TreeWriter processor... Why?
  /// needed to register parameters. Dont' really like this. Did f****d up Marlin design create this
  /// mess?!?
  virtual marlin::RootWriteEngine*        constructNewEngine(marlin::RootTreeWriter* host) = 0;
  virtual void                            disposeEngine( marlin::RootWriteEngine* engine ) = 0;
  virtual const __RTW::EngineVersionInfo* getEngineInfo() { return NULL; }
  virtual const __RTW::CompatVersionInfo* getCompatInfo() { return NULL; }
  virtual std::string                     getRegisterEngineName() const
  { return std::string("Unknown"); }

};


/// \todo Thinkabout it: replace include by forward declaration (have to move :RegisterProxy funtion for this)
// needed header to make Macro working... So include here.
#include "EngineRegistrar.hh"
/// @endcond

/// Macro to register engine within implementation (.cc) file. This obsoletes editing of 
/// engines/EnginesAdd.icc and engines/EnginesInclude.icc. The "registration" of an engine
/// with this macro also enables the "dynamic loading" of engines at run-time. 
#define RTW_REGISTER_ENGINE( theEngineName ) \
class __RTW_##theEngineName##_Proxy__:public __RTW_Proxy_BASE \
{ \
public: \
  __RTW_##theEngineName##_Proxy__() \
  { \
    __RTW::EngineRegistrar::RegisterProxy( this ); \
  } \
  virtual marlin::RootWriteEngine* constructNewEngine(marlin::RootTreeWriter* host) \
  { \
    return new theEngineName(host); \
  } \
  virtual void disposeEngine( marlin::RootWriteEngine* engine ) \
  { \
    delete engine; \
  } \
 \
  virtual std::string getRegisterEngineName() const \
  {  \
    return std::string( #theEngineName );  \
  } \
 \
 \
}; \
static  __RTW_##theEngineName##_Proxy__  __a_RTW_##theEngineName##_Proxy__



/// @cond INTERNALS


/// @endcond

#endif //__ENGINELOADERPROXY_HH__
