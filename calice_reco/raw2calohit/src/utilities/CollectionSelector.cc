#include "CollectionSelector.hh"
#include <vector>
#include <cctype>
#include <cassert>
#include <cstdlib>


#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>


namespace CALICE {

  // create instance to make this Processor known to Marlin
  CollectionSelector a_collection_selector;

  CollectionSelector::CollectionSelector() 
    : Processor("CollectionSelector")
  {
    _description = "Mark matching or not matching collections transient" ;

    StringVec defaultExcludePattern ;
    defaultExcludePattern.push_back("");
    
    registerOptionalParameter( "excludePattern" , 
			       "Mark matching collections transient"  ,
			       _excludePattern ,
			       defaultExcludePattern ,
			       defaultExcludePattern.size() ) ;

    StringVec defaultIncludePattern ;
    
    registerOptionalParameter( "includePattern" , 
			       "remove transient mark from matching collections"  ,
			       _includePattern ,
			       defaultIncludePattern ,
			       defaultIncludePattern.size() ) ;

    {
      _selectionList.push_back( "ReconstructionState" );
      _selectionList.push_back( "4" );

      registerOptionalParameter( "SelectionList" , 
				 "Include collections only if the given integer parameter contains the given value."  ,
				 _selectionList,
				 _selectionList);
      _selectionList.clear();
    }

    

  }

  void CollectionSelector::init() {
    //    if( parameterSet( "excludePattern" ) ) {
    //    }
    //    if( parameterSet( "includePattern" ) ) {
    //    }
    printParameters();

    assert( _selectionList.size()% 2 == 0 );

    for (StringVec::const_iterator selection_iter=_selectionList.begin();
	 selection_iter!=_selectionList.end();
	 selection_iter++) {
      
      _selectionVariableName.push_back(*selection_iter);
      selection_iter++;
      assert(selection_iter != _selectionList.end());
      
      assert( !selection_iter->empty() && isdigit( (*selection_iter)[0] ) );

      _selectionValue.push_back(atoi(selection_iter->c_str()));

    }

    
    StringVec *arr[2]={&_excludePattern,&_includePattern};
    //    std::string arr_name[2] = {std::string("exlcude"),std::string("include")};
    for (UInt_t arr_i=0; arr_i<2;arr_i++) {
      for (StringVec::iterator pattern_iter=arr[arr_i]->begin();
	   pattern_iter!=arr[arr_i]->end();
	   pattern_iter++) {
	//std::cout << arr_name[arr_i] << ":" << *pattern_iter << std::endl;
	if (pattern_iter->size()>=2) {
	  if ((*pattern_iter)[0]=='\"' && (*pattern_iter)[pattern_iter->size()-1]=='\"') {
	    pattern_iter->erase(0,1); //remove first character
	    pattern_iter->erase(pattern_iter->size()-1,1); //remove last character
	  }
	}
      }
    }
  }

  //  void CollectionSelector::processRunHeader( LCRunHeader* run) { } 

  void CollectionSelector::processEvent( LCEvent * evtP ) {
    if (evtP) {
      
      // loop over all collections.
      // FIXME: Currently, the only way to do this, is to get a list of all collection names
      // and than get one collection after the other.
      const std::vector<std::string>  *col_names=evtP->getCollectionNames();
      for (std::vector<std::string>::const_iterator col_name_iter=col_names->begin();
	   col_name_iter!=col_names->end();
	   col_name_iter++) {

	for (StringVec::const_iterator pattern_iter=_excludePattern.begin();
	     pattern_iter!=_excludePattern.end();
	     pattern_iter++) {
	  if (pattern_iter->empty() || col_name_iter->find(*pattern_iter)!=std::string::npos) {
	    setTransient(evtP,*col_name_iter,true);
	  }
	}

	bool include=true;
	if (!_selectionVariableName.empty()) {
	  include=false;
	  IntVec::const_iterator value_iter=_selectionValue.begin();

	  for (StringVec::const_iterator flag_iter=_selectionVariableName.begin();
	       flag_iter != _selectionVariableName.end();
	       flag_iter++, value_iter++) {
#ifdef BOUNDARY_CHECK
	    assert( value_iter != _selectionValue.end());
#endif
	    if (evtP->getParameters().getIntVal(*flag_iter)== *value_iter) {
	      include=true;
	      break;
	    }
	  }
	}

	if (include) {
	for (StringVec::const_iterator pattern_iter=_includePattern.begin();
	     pattern_iter!=_includePattern.end();
	     pattern_iter++) {
	  if (pattern_iter->empty() || col_name_iter->find(*pattern_iter)!=std::string::npos) {
	    setTransient(evtP,*col_name_iter,false);
	  }
	}
	}

      }
    }
  }

  void CollectionSelector::end() {
  }

  void CollectionSelector::setTransient(lcio::LCEvent *evtP, const std::string &name, bool transient)
  {
    LCCollection *col=evtP->getCollection(name);
    if(transient) {
      col->setFlag(col->getFlag() | (1<<lcio::LCCollection::BITTransient)) ;
    }
    else {
      col->setFlag(col->getFlag() & (~(1<<lcio::LCCollection::BITTransient))) ;
    }
  }
 
}

