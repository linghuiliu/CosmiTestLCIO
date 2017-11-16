#include <CollectionHistogramer.hh>
#include <vector>
#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <iomanip>

namespace CALICE {

  const char *CollectionHistogramer::__histogramTypeName[kNParMaps]={
    "Collections", 
    "Transient Collections",
    "Integer Parameters",
    "Float Parameters",
    "String Parameters"
  };


  // create instance to make this Processor known to Marlin
  CollectionHistogramer a_collection_histogramer;

  CollectionHistogramer::CollectionHistogramer() 
    : Processor("CollectionHistogramer")
  {
    _description = "Collect the names of all collections which exist in the lcio stream and histogram the number of elements." ;

    registerProcessorParameter( "HistogramEventParameters" , 
			       "If set to !=0 the event parameters are histogrammed." ,
			       _histogramParameters ,
			       0) ;

  }

  void CollectionHistogramer::init() {
    for (UInt_t hist_i=0; hist_i<kNParMaps; hist_i++) {
      _hist[hist_i].clear();
    }
  }

  //  void CollectionHistogramer::processRunHeader( LCRunHeader* run) { } 

  void CollectionHistogramer::processEvent( LCEvent * evtP ) {
    if (evtP) {
      
      // loop over all collections.
      // Currently, the only way to do this, is to get a list of all collection names
      // and than get one collection after the other.
      const std::vector<std::string>  *col_names=evtP->getCollectionNames();
      for (std::vector<std::string>::const_iterator col_name_iter=col_names->begin();
	   col_name_iter!=col_names->end();
	   col_name_iter++) {

	LCCollection* a_col = evtP->getCollection( *col_name_iter ) ;

	// add collection to map or if it already exists collect statistics about the number of elements in the collection
	if (a_col->isTransient()) {
	  _hist[kTransientCollectionMap][*col_name_iter].add(a_col->getNumberOfElements(),1.);
	}
	else {
	  _hist[kCollectionMap][*col_name_iter].add(a_col->getNumberOfElements(),1.);
	}
      }
      if (_histogramParameters!=0) {
	lcio::StringVec keys;
	evtP->getParameters().getIntKeys(keys);
	for (lcio::StringVec::const_iterator key_iter=keys.begin();
	     key_iter!=keys.end();
	     key_iter++) {
	  _hist[kIntParMap][*key_iter].add(evtP->getParameters().getNInt(*key_iter));
	}
	keys.clear();
	evtP->getParameters().getFloatKeys(keys);
	for (lcio::StringVec::const_iterator key_iter=keys.begin();
	     key_iter!=keys.end();
	     key_iter++) {
	  _hist[kFloatParMap][*key_iter].add(evtP->getParameters().getNFloat(*key_iter));
	}
	keys.clear();
	evtP->getParameters().getStringKeys(keys);
	for (lcio::StringVec::const_iterator key_iter=keys.begin();
	     key_iter!=keys.end();
	     key_iter++) {
	  _hist[kStringParMap][*key_iter].add(evtP->getParameters().getNString(*key_iter));
	}
      }
    }
  }

  void CollectionHistogramer::end() {

    std::cout << "---  " << name() << " Report :" << std::endl;

    // get the length of the longest collection name
    // used below for output formatting
    UInt_t size=0;
    for (UInt_t hist_i=0; hist_i<kNParMaps; hist_i++) {
      if (_hist[hist_i].size()==0) continue;

      for(std::map<std::string,Average_t>::iterator col_stat_iter=_hist[hist_i].begin();
	  col_stat_iter!=_hist[hist_i].end();
	  col_stat_iter++) {
	if (col_stat_iter->first.size()>size) size=col_stat_iter->first.size();
      }
      std::cout << " -  " << __histogramTypeName[hist_i] << " :" << std::endl;
      // print the names of all collections in the stream and print the number of occurances and the 
      // min/mean/max number of elements which have been stored in the collection.
      for(std::map<std::string,Average_t>::iterator col_stat_iter=_hist[hist_i].begin();
	  col_stat_iter!=_hist[hist_i].end();
	  col_stat_iter++) {
	col_stat_iter->second.calculate();
	std::cout << std::setw(size) << col_stat_iter->first << ":" << col_stat_iter->second << std::endl;
      }
      std::cout << std::endl;
    }
  }
}
