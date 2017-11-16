#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <sstream>
#include <cassert>

#include "HistMgr.hh"
#include "FloatHistogram1D.hh"
#include <EVENT/LCGenericObject.h>
#include "HistWriter.hh"
#include "HistWriterKit.hh"


namespace histmgr {

HistMgr *HistMgr::__histMgr=0;

  const char *HistMgr::__typeName[HistMgr::kNTypes]={
    "1D",
    "2D",
    "Profile",
    "Graph"
  };

  HistMgr::ECollectionType HistMgr::findType(const std::string &type_name) {
    for (unsigned int type_i=0; type_i<kNTypes; type_i++) {
      if (type_name.compare(__typeName[type_i])==0) return static_cast<ECollectionType>(type_i);
    }
    return kNTypes;
  }


#ifdef HAVE_CALICEGUI
UInt_t HistMgr::__instanceCounter=0;
Glib::StaticMutex HistMgr::_histogramListLock = GLIBMM_STATIC_MUTEX_INIT;
#endif

HistMgr::~HistMgr() 
{
#ifdef HAVE_CALICEGUI
  Glib::Mutex::Lock lock (_histogramListLock);
#endif
  if (this == __histMgr) {
    __histMgr=0;
  }
  
  //  for_each ( _histogramGroupList.begin(), _histogramGroupList.end() )
  // delete all the histogram collections
  //  for (HistogramList_t::iterator col_iter=_histograms.begin();
  //       col_iter!=_histograms.end();
  //       col_iter++) {
  //    col_iter->second.deleteCollection();
  //  }

  // delete registered Histogram writer
  for (std::map<std::string,HistWriterKit *>::iterator iter=_writerKitList.begin();
       iter!=_writerKitList.end();
       iter++) {
    delete iter->second;
    iter->second=0;
  };
}

bool HistMgr::hasLockedGroups() const
{
  for (HistogramGroupList_t::const_iterator group_iter=_histogramGroupList.begin();
       group_iter!=_histogramGroupList.end();
       group_iter++) {
    if ( (*group_iter).header().isReferenced()) {
      return true;
    }
  }
  return false;
}


void HistMgr::deleteInstance() 
{
  if (!__histMgr) return;
  
#ifdef HAVE_CALICEGUI
  if (!__histMgr->isUnused()) {
    __histMgr->_deleteIfUnused=true;
    return;
  }
#endif
  
  if (__histMgr->hasLockedGroups()) {
#ifdef HAVE_CALICEGUI
    __histMgr->_deleteIfUnused=true;
#endif
    return;
  }
  
#ifdef HAVE_CALICEGUI
  __instanceCounter--;
#endif
  delete __histMgr; __histMgr=0;
}

void HistMgr::unlockGroup(const Key_t &group_key) {
  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    _histogramGroupList[group_key].header().unref();
  }


#ifdef HAVE_CALICEGUI
  if (_deleteIfUnused) {
    deleteInstance();
  }
#endif
}

lcio::LCCollection *HistMgr::createHistograms(const Key_t &group_key, const Key_t &collection_key, 
					      const EVENT::StringVec &name_list, UInt_t n_hist, 
					      const HistPar &par,
					      bool may_overwrite) 
  throw (std::bad_alloc, std::runtime_error)
{
  if (histogramGroupExists(group_key) && histogramGroup(group_key).histogramCollectionExists(collection_key) && may_overwrite==false) {
    std::stringstream message;
    message << "ERROR:: HistMgr::createHistograms> Histogram collection " 
	    << collection_key.name() 
	    << " does already exist." << std::endl;
    throw std::runtime_error(message.str());
  }
  HistogramCollection_t *histogram_col=new HistogramCollection_t(collection_key.name(),name_list,n_hist,par);

  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    getOrCreatehistogramGroup(group_key).setHistogramCollection(collection_key,histogram_col);
  }
  return histogramGroup(group_key).histogramCollection(collection_key).collection();
}


lcio::LCCollection *HistMgr::createHistograms(const Key_t &group_key, const Key_t &collection_key, 
					      const EVENT::StringVec &name_list, const lcio::IntVec &n_hist_list, 
					      const HistPar &par,
					      bool may_overwrite) 
    throw (std::bad_alloc,std::runtime_error)
{

  
  if (histogramGroupExists(group_key) && histogramGroup(group_key).histogramCollectionExists(collection_key) && may_overwrite==false) {
    std::stringstream message;
    message << "ERROR:: HistMgr::createHistograms> Histogram collection " 
	    << collection_key.name() 
	    << " does already exist." << std::endl;
    throw std::runtime_error(message.str());
  }
  HistogramCollection_t *histogram_col=new HistogramCollection_t(collection_key.name(),name_list,n_hist_list,par);

  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    getOrCreatehistogramGroup(group_key).setHistogramCollection(collection_key,histogram_col);
  }
  return histogramGroup(group_key).histogramCollection(collection_key).collection();
}


lcio::LCCollection *HistMgr::create2DHistograms(const Key_t &group_key, const Key_t &collection_key, 
					      const EVENT::StringVec &name_list, UInt_t n_hist, 
					      const HistPar &x_par,
					      const HistPar &y_par,
					      bool may_overwrite) 
  throw (std::bad_alloc, std::runtime_error)
{
  if (histogramGroupExists(group_key) && histogramGroup(group_key).histogram2DCollectionExists(collection_key) && may_overwrite==false) {
    std::stringstream message;
    message << "ERROR:: HistMgr::createHistograms> 2D Histogram collection " 
	    << collection_key.name() 
	    << " does already exist." << std::endl;
    throw std::runtime_error(message.str());
  }
  Histogram2DCollection_t *histogram_col=new Histogram2DCollection_t(collection_key.name(),name_list,n_hist,x_par,y_par);

  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    getOrCreatehistogramGroup(group_key).setHistogram2DCollection(collection_key,histogram_col);
  }
  return histogramGroup(group_key).histogram2DCollection(collection_key).collection();
}


lcio::LCCollection *HistMgr::create2DHistograms(const Key_t &group_key, const Key_t &collection_key, 
					      const EVENT::StringVec &name_list, const lcio::IntVec &n_hist_list, 
					      const HistPar &x_par,
					      const HistPar &y_par,
					      bool may_overwrite) 
    throw (std::bad_alloc,std::runtime_error)
{

  
  if (histogramGroupExists(group_key) && histogramGroup(group_key).histogram2DCollectionExists(collection_key) && may_overwrite==false) {
    std::stringstream message;
    message << "ERROR:: HistMgr::createHistograms> 2D Histogram collection " 
	    << collection_key.name() 
	    << " does already exist." << std::endl;
    throw std::runtime_error(message.str());
  }
  Histogram2DCollection_t *histogram_col=new Histogram2DCollection_t(collection_key.name(),name_list,n_hist_list,x_par, y_par);

  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    getOrCreatehistogramGroup(group_key).setHistogram2DCollection(collection_key,histogram_col);
  }
  return histogramGroup(group_key).histogram2DCollection(collection_key).collection();
}

lcio::LCCollection *HistMgr::createGraphCollection(const Key_t &group_key, const Key_t &collection_key, unsigned int n_graphs, 
						   const EVENT::StringVec &type_names, unsigned int n_expected_values, 
						   const EVENT::StringVec &opt_major_names,
						   bool may_overwrite)
    throw (std::bad_alloc, std::runtime_error)
{
  if (histogramGroupExists(group_key) && histogramGroup(group_key).graphCollectionExists(collection_key) && may_overwrite==false) {
    std::stringstream message;
    message << "ERROR:: HistMgr::createHistograms> Graph collection " 
	    << collection_key.name() 
	    << " does already exist." << std::endl;
    throw std::runtime_error(message.str());
  }
  GraphCollection_t *graph_col=new GraphCollection_t(collection_key.name(),n_graphs,type_names,n_expected_values,opt_major_names,may_overwrite);

  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    getOrCreatehistogramGroup(group_key).setGraphCollection(collection_key,graph_col);
  }
  return histogramGroup(group_key).graphCollection(collection_key).collection();
}

lcio::LCCollection *HistMgr::createGraphCollection(const Key_t &group_key, const Key_t &collection_key, const std::vector<int> &n_graphs,
						   const std::vector<std::string> &type_names, unsigned int n_expected_values,
						   const std::vector<std::string> &opt_major_names,
						   bool may_overwrite)
  throw (std::bad_alloc, std::runtime_error)
{
  if (histogramGroupExists(group_key) && histogramGroup(group_key).graphCollectionExists(collection_key) && may_overwrite==false) {
    std::stringstream message;
    message << "ERROR:: HistMgr::createHistograms> Graph collection " 
	    << collection_key.name() 
	    << " does already exist." << std::endl;
    throw std::runtime_error(message.str());
  }
  GraphCollection_t *graph_col=new GraphCollection_t(collection_key.name(),n_graphs,type_names,n_expected_values,opt_major_names);

  {
#ifdef HAVE_CALICEGUI
    Glib::Mutex::Lock lock (_histogramListLock);
#endif
    getOrCreatehistogramGroup(group_key).setGraphCollection(collection_key,graph_col);
  }
  return histogramGroup(group_key).graphCollection(collection_key).collection();
}

void HistMgr::assignFileName(const Key_t &group_key, const std::string &file_name, const std::string &folder_name)
{
  if (!_writerKit && _writerKitList.empty()) {
    std::cerr << "HistMgr::writeHistograms> WARNINING: There are currently no histogram writers registered." << std::endl
	      << "No histograms will be written!" << std::endl;

  }

  if (!histogramGroupExists(group_key)) {
    std::cerr << "HistMgr::writeHistograms> ERROR: the group " << group_key.name() << " does not exist." << std::endl;
  }
  else {
    HistogramGroupData_t &group=histogramGroup(group_key).header();
    if (group.fileName().empty()) {
      group.ref();
    }
    group.setFileName(file_name)
      .setFolderName(folder_name);
  }
}

void HistMgr::writeHistograms(bool snapshot)
{
  for (HistogramGroupList_t::iterator group_iter=_histogramGroupList.begin();
       group_iter!=_histogramGroupList.end();
       group_iter++) {
    Group_t &group=(*group_iter);
    if (!group.header().fileName().empty()) {
      writeHistograms(group,group_iter.name(),snapshot);
      if (!snapshot) {
	group.header().unref();
      }
    }
  }
}

void HistMgr::warnOnUnassigned()
{
  for (HistogramGroupList_t::iterator group_iter=_histogramGroupList.begin();
       group_iter!=_histogramGroupList.end();
       group_iter++) {
    if ((*group_iter).header().fileName().empty()) {
      std::cerr << "HistMgr::warnOnUnassigned> No output file assigned to histogram group " << group_iter.name() << std::endl;
    }
  }
}

void HistMgr::fillGroupList(std::vector<std::string> &dest_group_list) const
{
  dest_group_list.clear();
#ifdef HAVE_CALICEGUI
  Glib::Mutex::Lock lock (_histogramListLock);
  // FIXME: This function is called e.g. by the GUI from an other thread. To make sure
  // that the instance still exists:
  if (!__histMgr) return;
  // 
#endif
  for (HistogramGroupList_t::const_iterator group_iter=_histogramGroupList.begin();
       group_iter!=_histogramGroupList.end();
       group_iter++) {
    for (UInt_t type_i=0; type_i<kNTypes; type_i++) {
      if ((*group_iter).hasCollection(static_cast<ECollectionType>( type_i) )) {
	std::stringstream full_name;
	full_name << group_iter.name()
		  << " -- "
		  << __typeName[type_i];
	
	dest_group_list.push_back( full_name.str() );
      }
    }
  }
}

std::pair<std::string, HistMgr::ECollectionType> HistMgr::getGroupNameAndCollectionType(const std::string &full_name)
{
    std::string::size_type type_pos=full_name.rfind(" -- ");
    histmgr::HistMgr::ECollectionType type=histmgr::HistMgr::kNTypes;
    if (type_pos!=std::string::npos) {
      std::string type_name(full_name,type_pos+4,full_name.size()-type_pos-4);
      type=histmgr::HistMgr::findType(type_name);
    }
    return make_pair(std::string(full_name,0,type_pos),type);
}

void HistMgr::fillHistogramCollectionList(const Key_t &group_key, 
					  ECollectionType type, 
					  std::vector<std::string> &dest_histogram_collection_list) const
{
  dest_histogram_collection_list.clear();
#ifdef HAVE_CALICEGUI
  Glib::Mutex::Lock lock (_histogramListLock);
  // FIXME: This function is called e.g. by the GUI from an other thread. To make sure
  // that the instance still exists:
  if (!__histMgr) return;
  // 
#endif

  if (!histogramGroupExists(group_key)) {
    std::cerr << "HistMgr::fillHistogramCollectionList> ERROR: the group " << group_key.name() << "does not exist." << std::endl;
  }
  else {
    const Group_t &group=histogramGroup(group_key);

    // TODO: need to change design such that all supported histogram, graph collections derive from the same base
    //       class which provides the same interface to create 1d and 2d collections, and to find out whether the
    //       collection is empty

    std::pair<KeyMapBase_t::NameMap_t::const_iterator, KeyMapBase_t::NameMap_t::const_iterator> limits=group.nameListIterators(type);
    for (KeyMapBase_t::NameMap_t::const_iterator col_iter=limits.first;
	 col_iter!=limits.second;
	 col_iter++) {

      histmgr::Key_t element_key=KeyMapBase_t::createKey(col_iter);
      unsigned int n_elements=0;
      switch (type) {
      case kH1:
	n_elements=group.histogramCollection(element_key).n();
	break;
      case kH2:
	n_elements=group.histogram2DCollection(element_key).n();
	break;
      case kProfile:
	n_elements=group.profileCollection(element_key).n();
	break;
      case kGraph:
	n_elements=group.graphCollection(element_key).n();
	break;
      default:
	break;
      }

      if (n_elements>0) {
	dest_histogram_collection_list.push_back(col_iter->first);
      }
    }
  }
}


Double_t HistMgr::getNEntriesTotal(const Key_t &group_key) const
{
  Double_t entries=0.;

  if (histogramGroupExists(group_key)) {
    const Group_t &group=histogramGroup(group_key);
    
    for (HistogramList_t::const_iterator col_iter=group._histogramList.begin();
	 col_iter!=group._histogramList.end();
	 col_iter++) {
      const HistogramCollection_t &hist_col=*(*col_iter);
      // warn if the histogram collection does not contain histograms
      if (hist_col.n()>0) {
	if (hist_col.is2D()) {
	  for (UInt_t major_i=0; major_i<(UInt_t) hist_col.nMajor(); major_i++) {
	    for (UInt_t hist_i=0; hist_i<(UInt_t) hist_col.nMinor(major_i); hist_i++) {
	      const FloatHistogram1D *a_hist=hist_col.histogram(major_i,hist_i);
	      entries+=a_hist->entries();
	    }
	  }
	}
	else {
	  for (UInt_t hist_i=0; hist_i<(UInt_t) hist_col.n(); hist_i++) {
	    const FloatHistogram1D *a_hist=hist_col.histogram(hist_i);
	    entries+=a_hist->entries();
	  }
	}
      }
    }
  }
  return entries;
}
  
  void HistMgr::writeHistograms(const Key_t &group_key, bool snapshot)
  {
    if (histogramGroupExists(group_key)) {
      std::cerr << "HistMgr::writeHistograms> ERROR: the group " << group_key.name() << "does not exist." << std::endl;
    }
    else {
      Group_t &group=histogramGroup(group_key);
      writeHistograms(group,group_key.name(),snapshot);
    }
  }


void HistMgr::writeHistograms(Group_t &group, const std::string &group_name, bool snapshot)
{
  if (!_writerKit || group.header().fileName().empty()) return;

  std::string folder_name;
  //  std::string file_name=group.header().fileName();
  if (snapshot || group.header().version()>0) {
    std::stringstream folder_name_with_version;
    if (group.header().folderName().empty()){
      if (snapshot) {
	folder_name_with_version << "snapshot_" << group.header().version();
      }
    }
    else {
      folder_name_with_version << group.header().folderName() << "_" << group.header().version();
    }
    folder_name=folder_name_with_version.str();
  }
  else {
    folder_name=group.header().folderName();
  }
  
  HistWriter *writer=0;
  
  // for fast access the histogram collections are stored in a map.
  // During histogram creating the map structure changes. Thus, it is no
  // so easy to maintain an index of all histogram which belong to a certain 
  // group.
  // So, the whole list of histograms is worked through and all histograms 
  // which have the requested group_id are written.

  UInt_t n_histograms=0;
  UInt_t n_graphs=0;
  bool first=true;
  for (HistogramList_t::iterator col_iter=group._histogramList.begin();
       col_iter!=group._histogramList.end();
       col_iter++) {

    HistogramCollection_t &hist_col=*(*col_iter);
    if (hist_col.n()==0) {
      std::cerr << "HistMgr::writeHistograms> Histogram collection " << col_iter.name()
		<< " is empty." << std::endl;
      continue;
    }

    
    // the file is only created if the group contains at least one histogram;
    if (!writer) {
      writer=_writerKit->createWriter(group.header().fileName());
      if (!folder_name.empty()) {
	writer->enterDir(folder_name,true);
      }
    }
      
    writer->enterDir(col_iter.name(),true);

    UInt_t n_hists_written=0;
    UInt_t n_empty_hists_written=0;
    if (hist_col.is2D()) {
      for (UInt_t major_i=0; major_i<(UInt_t) hist_col.nMajor(); major_i++) {
	for (UInt_t hist_i=0; hist_i<(UInt_t) hist_col.nMinor(major_i); hist_i++) {
	  const FloatHistogram1D *a_hist=hist_col.histogram(major_i,hist_i);
	  if (a_hist->entries()>0) {
	    n_hists_written++;
	  }
	  else {
	    n_empty_hists_written++;
	  }
	  std::stringstream message;
	  std::string full_name(hist_col.getName(major_i));
	  std::string::size_type base_name_end_i=full_name.find("::");
	  if (base_name_end_i==std::string::npos || base_name_end_i+2>=full_name.size()) {
	    base_name_end_i=0;
	  }
	  else {
	    base_name_end_i+=2;
	  }
	  message << full_name.substr(base_name_end_i,full_name.size()-base_name_end_i)
		  << "_"
		  << major_i
		  << "_"
		  << hist_i; 
	  writer->writeToCurrentDir(*a_hist,message.str());
	}
      }
    }
    else {
      for (UInt_t hist_i=0; hist_i<(UInt_t) hist_col.n(); hist_i++) {
	const FloatHistogram1D *a_hist=hist_col.histogram(hist_i);
	if (a_hist->entries()>0) {
	  n_hists_written++;
	}
	else {
	  n_empty_hists_written++;
	}
	std::stringstream message;
	std::string full_name(hist_col.getName(hist_i));
	std::string::size_type base_name_end_i=full_name.find("::");
	if (base_name_end_i==std::string::npos || base_name_end_i+2>=full_name.size()) {
	  base_name_end_i=0;
	}
	else {
	  base_name_end_i+=2;
	}
	message << full_name.substr(base_name_end_i,full_name.size()-base_name_end_i)
		<< "_"
		<< hist_i; 
	writer->writeToCurrentDir(*a_hist,message.str());
      }
    }
    
    if (n_hists_written + n_empty_hists_written>0) {
      if (first ) {
	std::cout << "Written Histograms:" << std::endl;
	first=false;
      }
      
      std::cout << "\t" << group_name << ":" << n_hists_written + n_empty_hists_written   
		<<  " (no entries =" << n_empty_hists_written << ")" << std::endl;
    }
      
    n_histograms+=hist_col.n();
    
    writer->upDir();
    //    if (!folder_name.empty()) {
    //      writer->upDir();
    //    }
  }


  UInt_t n_2d_histograms=0;
  for (Histogram2DList_t::iterator col_iter=group._histogram2DList.begin();
       col_iter!=group._histogram2DList.end();
       col_iter++) {

    Histogram2DCollection_t &hist_col=*(*col_iter);
    if (hist_col.n()==0) {
      std::cerr << "HistMgr::writeHistograms> 2D Histogram collection " << col_iter.name()
		<< " is empty." << std::endl;
      continue;
    }

    
    // the file is only created if the group contains at least one histogram;
    if (!writer) {
      writer=_writerKit->createWriter(group.header().fileName());
      if (!folder_name.empty()) {
	writer->enterDir(folder_name,true);
      }
    }
      
    writer->enterDir(col_iter.name(),true);

    UInt_t n_hists_written=0;
    UInt_t n_empty_hists_written=0;
    if (hist_col.is2D()) {
      for (UInt_t major_i=0; major_i<(UInt_t) hist_col.nMajor(); major_i++) {
	for (UInt_t hist_i=0; hist_i<(UInt_t) hist_col.nMinor(major_i); hist_i++) {
	  const FloatHistogram2D *a_hist=hist_col.histogram(major_i,hist_i);
	  if (a_hist->entries()>0) {
	    n_hists_written++;
	  }
	  else {
	    n_empty_hists_written++;
	  }
	  std::stringstream message;
	  std::string full_name(hist_col.getName(major_i));
	  std::string::size_type base_name_end_i=full_name.find("::");
	  if (base_name_end_i==std::string::npos || base_name_end_i+2>=full_name.size()) {
	    base_name_end_i=0;
	  }
	  else {
	    base_name_end_i+=2;
	  }
	  message << full_name.substr(base_name_end_i,full_name.size()-base_name_end_i)
		  << "_"
		  << major_i
		  << "_"
		  << hist_i; 
	  writer->writeToCurrentDir(*a_hist,message.str());
	}
      }
    }
    else {
      for (UInt_t hist_i=0; hist_i<(UInt_t) hist_col.n(); hist_i++) {
	const FloatHistogram2D *a_hist=hist_col.histogram(hist_i);
	if (a_hist->entries()>0) {
	  n_hists_written++;
	}
	else {
	  n_empty_hists_written++;
	}
	std::stringstream message;
	std::string full_name(hist_col.getName(hist_i));
	std::string::size_type base_name_end_i=full_name.find("::");
	if (base_name_end_i==std::string::npos || base_name_end_i+2>=full_name.size()) {
	  base_name_end_i=0;
	}
	else {
	  base_name_end_i+=2;
	}
	message << full_name.substr(base_name_end_i,full_name.size()-base_name_end_i)
		<< "_"
		<< hist_i; 
	writer->writeToCurrentDir(*a_hist,message.str());
      }
    }
    
    if (n_hists_written + n_empty_hists_written>0) {
      if (first ) {
	std::cout << "Written Histograms:" << std::endl;
	first=false;
      }
      
      std::cout << "\t" << group_name << ":" << n_hists_written + n_empty_hists_written   
		<<  " (no entries =" << n_empty_hists_written << ")" << std::endl;
    }
      
    n_2d_histograms+=hist_col.n();
    
    writer->upDir();
    //    if (!folder_name.empty()) {
    //      writer->upDir();
    //    }
  }

  for (GraphList_t::iterator col_iter=group._graphList.begin();
       col_iter!=group._graphList.end();
       col_iter++) {

    GraphCollection_t &graph_col=*(*col_iter);
    if (graph_col.n()==0) {
      std::cerr << "HistMgr::writeHistograms> Histogram collection " << col_iter.name()
		<< " is empty." << std::endl;
      continue;
    }

    // the file is only created if the group contains at least one histogram;
    if (!writer) {
      writer=_writerKit->createWriter(group.header().fileName());
      if (!folder_name.empty()) {
	writer->enterDir(folder_name,true);
      }
    }
      
    writer->enterDir(col_iter.name(),true);
    
    UInt_t n_graphs_written=0;
    UInt_t n_empty_graphs_written=0;

    EVENT::LCGenericObject *array_x=graph_col.getXArray();

    if (graph_col.is2D()) {
      for (UInt_t major_i=0; major_i<(UInt_t) graph_col.nMajor(); major_i++) {
	for (UInt_t minor_i=0; minor_i<(UInt_t) graph_col.nMinor(major_i); minor_i++) {
	  std::stringstream graph_name;
	  graph_name << graph_col.getName(major_i)
		     << "_"
		     << major_i
		     << "_"
		     << minor_i; 

	  for (unsigned int type_i=0; type_i<graph_col.nTypes(); type_i++) {
	    std::stringstream full_name;
	    full_name << graph_name.str()
		      << "_"
		      << graph_col.getTypeName(type_i);

	    if (type_i+1<graph_col.nTypes() && graph_col.getTypeName(type_i+1)=="rms") {
	      writer->writeToCurrentDir(array_x, 
					graph_col.getArray(graph_col.getIndex(major_i,minor_i,type_i)),
					graph_col.getArray(graph_col.getIndex(major_i,minor_i,type_i+1)),
					full_name.str());
	      type_i++;
	    }
	    else {
	      writer->writeToCurrentDir(array_x, 
					graph_col.getArray(graph_col.getIndex(major_i,minor_i,type_i)),
					0,
					full_name.str());
	    }
	  }

	  if (array_x->getNDouble()>0) {
	    n_graphs_written++;
	  }
	  else {
	    n_empty_graphs_written++;
	  }
	}
      }
    }
    else {
      for (UInt_t gr_i=0; gr_i<(UInt_t) graph_col.n(); gr_i++) {

	std::stringstream graph_name;
	graph_name << graph_col.getName(gr_i)
		   << "_"
		   << gr_i;

	EVENT::LCGenericObject *array_x=graph_col.getXArray();

	for (unsigned int type_i=0; type_i<graph_col.nTypes(); type_i++) {
	  std::stringstream full_name;
	  full_name << graph_name.str();
	  if (!graph_col.getTypeName(type_i).empty()) {
	    full_name  << "_"
		       << graph_col.getTypeName(type_i);
	  }
	  
	  if (type_i+1<graph_col.nTypes() && graph_col.getTypeName(type_i+1)=="rms") {
	    writer->writeToCurrentDir(array_x, 
				      graph_col.getArray(graph_col.getIndex(gr_i,type_i)),
				      graph_col.getArray(graph_col.getIndex(gr_i,type_i+1)),
				      full_name.str());
	    type_i++;
	  }
	  else {
	      writer->writeToCurrentDir(array_x, 
					graph_col.getArray(graph_col.getIndex(gr_i,type_i)),
					0,
					full_name.str());
	  }
	}
	
	if (array_x->getNDouble()>0) {
	  n_graphs_written++;
	}
	else {
	  n_empty_graphs_written++;
	}
      }
    }

    n_graphs+=graph_col.n();

    writer->upDir();

  }

  delete writer;

  if (n_histograms==0 && n_graphs==0 && n_2d_histograms==0) {
    std::cerr << "HistMgr::writeHistograms> WARNING: group " << group_name 
	      << " is empty." << std::endl;
  }

  group.header().incrementVersion();

}

void HistMgr::listRegisteredGroups() const
{  
  for (HistogramGroupList_t::const_iterator group_iter=_histogramGroupList.begin();
       group_iter!=_histogramGroupList.end();
       group_iter++) {
    const Group_t &group=(*group_iter);
    std::cout << group_iter.name()  << " : file=" << group.header().fileName() 
	      << " folder=" << group.header().folderName() 
	      << "  (version=" << group.header().version() << ")"
	      << " / ref=" << group.header().references()
	      << std::endl;
      
    UInt_t n_histograms=0;
    for (HistogramList_t::const_iterator col_iter=group._histogramList.begin();
	 col_iter!=group._histogramList.end();
	 col_iter++) {
      
      const HistogramCollection_t &hist_col=*(*col_iter);

      std::cout <<  n_histograms << " :"
		<< col_iter.name();
      if (hist_col.n()==0) {
	std::cout <<  " (empty)" << std::endl;
      }
      if (hist_col.is2D() && hist_col.nMajor()>=0) {
	UInt_t n_minor=hist_col.nMinor(0);
	bool number_of_minors_differ=false;
	for (UInt_t major_i=1; major_i<(UInt_t) hist_col.nMajor(); major_i++) {
	  if (n_minor!=hist_col.nMinor(major_i) ) {
	    number_of_minors_differ=true;
	  }
	}
	if (number_of_minors_differ) {
	  std::cout << " contains: ";
	  for (UInt_t major_i=1; major_i<(UInt_t) hist_col.nMajor(); major_i++) {
	    std::cout << hist_col.nMinor(major_i);
	    if (major_i+1<hist_col.nMajor()) {
	      std::cout << " + ";
	    }
	  }
	  std::cout << "histograms." << std::endl;
	}
	else {
	  std::cout << " contains: " << hist_col.nMajor() << " * " << n_minor << " histogramms." << std::endl;
	}
      }
      else {
	std::cout << " contains: " << hist_col.n() << " histogramms." << std::endl;
      }
      n_histograms++;
    }
  }
}
}
