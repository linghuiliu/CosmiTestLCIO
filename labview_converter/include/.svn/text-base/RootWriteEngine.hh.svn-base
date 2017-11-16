#ifndef __ROOTWRITEENGINE_HH__
#define __ROOTWRITEENGINE_HH__

#include <string>
#include "EngineLoaderProxy.hh"


//forward declarations
namespace EVENT
{
  class LCEvent;
}

class TTree;


//class declaration
namespace marlin
{

  class RootTreeWriter;


  /// Abstract base class of callback classes for the RootTreeWrite processor
  /// Abstract interface for a RootTreeWriter-engine. Implement an descendant of
  /// this class to add variables to the output tree of the RootTreeWriterProcessor.
  /// After that add the new \a engine to the list of writer engines in the 
  /// RootTreeWriter processor
  class RootWriteEngine
  {
  public:
    /// constructor
    /// \param[in] host pointer to the RootTreeWriter processor the constructor
    /// of a derived class must pass this pointer to this base class e.g. \verbatim
    /// SomeeWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
    ///                                          _engineName("SomeWriteEngine")
    /// {} \endverbatim
    RootWriteEngine( RootTreeWriter* host):_hostProcessor(*host)
    {}
    
    //    virtual RootWriteEngine* newEngine( Processor* )=0;

    /// Returns the name of the engine to the RootTreeWriter processor.
    /// \return Must return a string with the engine name. 
    /// \todo fixme: should be const, but must be declared const in 
    /// implementations too. Need to fix all existing engines?!?
    virtual const std::string& getEngineName() =0;
//    virtual const std::string& getEngineName() const
//    {
//      return const_cast<RootWriteEngine*>(this)->getEngineName();
//    }
//

    /// Used to register steering parameters.
    /// Implement to register \a marlin steering file parameters. Use \a marlin syntax
    /// to register parameters for the calling RootTreeWriter processor. 
    /// 
    /// use \verbatim 
    /// _hostProc.relayRegisterProcessorParameter(...)
    /// _hostProc.relayRegister...()
    /// \endverbatim instead of \a marlin processors \verbatim register...() \endverbatim
    /// methods
    virtual void registerParameters() =0;

    
    /// Implement to register local variables to the output tree
    /// \param pointer to the \a ROOT tree, which the RootTreeWriter processor fills at the
    /// end of each event.
    /// Usually the implementation looks like \verbatim
    /// hostTree->Branch("variable",&_variable" ,"variable/F"  );
    /// \endverbatim. But you can add any type of branch you like.
    virtual void registerBranches( TTree* )   =0;

    /// Implement this to extract the variables which you want to fill into the \a ROOT tree
    /// from the event. Write the value of the variables you want to add to the tree to
    /// the member variables, which you registered to the \a ROOT tree. The 
    /// RootTreeWriter will call TTree::Fill() for you.
    /// \param  evt  the current event 
    virtual void fillVariables( EVENT::LCEvent* evt ) =0;


    virtual ~RootWriteEngine()
    {}
    
  protected:
    RootTreeWriter& _hostProcessor;
  };


}//namespace marlin


/// @cond INTERNALS
namespace RTW
{
  /// helper class to compare engine-names with strings
  class EngineNameIs
  {
  public:
    EngineNameIs():_compStr(){};
    EngineNameIs(std::string compStr):_compStr(compStr){};

    bool operator()( marlin::RootWriteEngine& engine )
    {
      return ( engine.getEngineName() == _compStr );
    }
    bool operator()( marlin::RootWriteEngine* engine )
    { 
      return operator()( *engine );
    }

  private:
    std::string _compStr;
    
  };
  
}// namespace RTW
/// @endcond


#endif //__ROOTWRITEENGINE_HH__
