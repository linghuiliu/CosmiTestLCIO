#ifndef __ROOTTREEWRITER_HH__
#define __ROOTTREEWRITER_HH__

#include "marlin/Processor.h"

#include "RootWriteEngine.hh"

#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"


#undef OLDMARLIN

namespace marlin
{

  /** This processor writes a \a ROOT tree.
   * 
   *  <h4>Extending the RootTreeWriter</h4> 
   *  To add additional branches (variables) to the output ROOT tree,
   *  you have to implement a new \e writer \e engine i.e. a
   *  class derived from the RootWriteEngine class. The script 
   *  createEngineTemplate.sh, which is included in the bin/ directory
   *  of the RootTreeWriter, helps you to create a skeleton for a new 
   *  engine. The sourcecode of the engines, including haders, must be
   *  placed into the \e engines subdirectory. <br/>
   *  \e UPDATE: (for users or RootTreeWriter prior to version 1.2.0)
   *  The macro RTW_REGISTER_ENGINE within the implementation (.cc-file)
   *  of an engine now activates the new engine. Activation by adding 
   *  corresponding lines to the files \link EnginesInclude.icc
   *  engines/EnginesInclude.icc\endlink and \link EnginesAdd.icc
   *  engines/EnginesAdd.icc\endlink is not needed anymore for engines
   *  which are created with a recent version of the \e createEngineTemplate.sh
   *  script. If you want to update your engine to the new activation scheme,
   *  please add the line '\it \e RTW_REGISTER_ENGINE( <EngineName> );' next
   *  to the implementation of you engine (in the .cc-file). Please note that
   *  this line has to be within the same \it namespace like the implementation
   *  of the engine. <br/>
   *  To assure compatibility with future versions of the RootTreeWriter, please
   *  also change the constructor of you engine in the .hh-file. The constructor 
   *  in the header file should look similar to this ( with "TheEngineName" 
   *  beeing the name of your engine.
   *  \code
   *    TheEngineName( RootTreeWriter* host ):RootWriteEngine(host),
   *                                          _engineName("TheEngineName")
   *    {}
   *  \endcode
   *  Please change the constructor to this (replace "TheEngineName" with your ac 
   *  actual engine name):
   *  \code
   *    TheEngineName( RootTreeWriter* host, std::string engineName = "TheEngineName" ):
   *      RootWriteEngine(host), _engineName(engineName) {}
   *  \endcode
   *  
   *  <h4>General Options</h4> 
   * 
   *  @param OutFileName           Name of the output ROOT file
   *  @param OutFileMode           (ROOT) open mode for the ROOT file e.g. "RECRATE"
   *  @param RootTreeName          Name of the TTree within the ROOT file
   *  @param <EngineName>_enable   Switch to enable(1) or disable(0) a RootWriteEngine
   *                               build in the RootTreeWriter
   * 
   * 
   *  @author J. Samson, DESY
   *  @version $Id: RootTreeWriter.hh,v 1.8 2008/08/14 21:58:18 meyern Exp $ 
   */
  class RootTreeWriter:public Processor
  {

  public:
    RootTreeWriter();

    virtual Processor* newProcessor()
    { return new RootTreeWriter; }

    
    virtual void init();
    virtual void processEvent( EVENT::LCEvent* evt );
    //    virtual void check( EVENT::LCEvent* evt );
    virtual void end();


    //----------------------------------------------------------------------------------------------------
    /// proxy member to allow engines to register processor parameters
    template<class T>
    void relayRegisterProcessorParameter(const std::string& name,
					 const std::string&description,
					 T& parameter,
					 const T& defaultVal,
					 int setSize=0 ) 
    {
      registerProcessorParameter( name, description, parameter, defaultVal, setSize );
    }

    /// proxy member to allow engines to register processor parameters
    template<class T>
    void relayRegisterOptionalParameter(const std::string& name,
					const std::string& description,
					T& parameter,
					const T& defaultVal,
					int setSize=0 ) {
      registerOptionalParameter( name, description, parameter, defaultVal, setSize );
    }



    /// proxy member to allow engines to register processor parameters
    void relayRegisterInputCollection(const std::string& type,
                                 const std::string& name,
                                 const std::string& description,
                                 std::string& parameter,
                                 const std::string& defaultVal,
                                 int setSize=0 ) 
    {
#ifdef OLDMARLIN
      registerProcessorParameter( name, description, parameter, defaultVal, setSize ) ;
#else
      registerInputCollection( type, name, description, parameter, defaultVal, setSize ) ;
#endif
    }

    /// proxy member to allow engines to register processor parameters
    void relayRegisterOutputCollection(const std::string& type,
                                 const std::string& name,
                                 const std::string& description,
                                 std::string& parameter,
                                 const std::string& defaultVal,
                                 int setSize=0 ) 
    {
#ifdef OLDMARLIN
      registerProcessorParameter( name, description, parameter, defaultVal, setSize ) ;
#else
      registerOutputCollection( type, name, description, parameter, defaultVal, setSize ) ;
#endif
    }

    //----------------------------------------------------------------------------------------------


        
  private:

    bool checkUsedEnginesExist();

    static std::map<TTree*,RootTreeWriter*> _treeFillerMap;
    static std::map<TTree*,RootTreeWriter*> _treeOwnerMap;
    static std::map<TFile*,RootTreeWriter*> _fileOwnerMap;

    static const double INVALID;

    std::string _rootFileName;
    std::string _rootFileMode;
    std::string _rootTreeName;

    TFile*      _outFile;
    TTree*      _bigTree;
    Long64_t    _maxTreeSize;
    
    typedef RootWriteEngine WriteEngine;
    typedef std::vector< RootWriteEngine* > WriteEngineVec;
    typedef WriteEngineVec::iterator WriteEngineVIt;
    WriteEngineVec _writeEngineList;

    // std::vector<string> mEngineNames;
    std::vector<int>   _engineEnable;

    struct RunEventStruct
    {
      Int_t    runNumber;
      Int_t    eventNumber;
      Long64_t eventTime;
    } _runEventStruct;


    
  };




}//namespace marlin



#endif //__ROOTTREEWRITER_HH__
