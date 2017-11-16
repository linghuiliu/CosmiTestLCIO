#include "ProfileCollection_t.hh"
#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <lcio.h>


namespace histmgr {

  const std::string ProfileCollection_t::__majorIndexParameterName("major");
  const std::string ProfileCollection_t::__histogramNameParameterName("histName");
  const std::string ProfileCollection_t::__defaultProfileName("Profile");
  //  unsigned int ProfileCollection_t::__instances=0;


  lcio::LCCollection *ProfileCollection_t::createProfiles(const std::string &collection_name, 
							  const EVENT::StringVec &name_list, UInt_t n_hist, 
							  const HistPar &par) 
    throw (std::bad_alloc, std::runtime_error)
  {
    deleteCollection();

    if (collection_name.empty()) {
      throw std::runtime_error("HistMgr::createProfiles> The collection name must not be empty.");
    }
    
    assert ( name_list.empty() || name_list.size()==n_hist );
    for (EVENT::StringVec::const_iterator name_iter=name_list.begin();
	 name_iter!=name_list.end();
	 name_iter++) {
      assert ( !name_iter->empty() );
    }
    
    if (par.xMax()<=par.xMin() || par.nBins()==0) {
      throw std::runtime_error("HistMgr::createProfiles> The number of bins is zero or the range is empty.");
    }
    
    //  std::string full_histogram_name=group;
    //  full_histogram_name+="::";
    //  full_histogram_name+=collection_name;
    
    delete _histogramCol;
    delete _majorIndex;
    _majorIndex=0;

    _histogramCol=new lcio::LCCollectionVec( lcio::LCIO::LCGENERICOBJECT );
    //DEBUG    stat_add(_histogramCol);

    if (name_list.empty()) {
      EVENT::StringVec temp;
      temp.push_back(collection_name);
      _histogramCol->parameters().setValues(ProfileCollection_t::__histogramNameParameterName,temp);
    }
    else {
      _histogramCol->parameters().setValues(ProfileCollection_t::__histogramNameParameterName,*(const_cast<EVENT::StringVec*>(&name_list)));
    }
    
    for (UInt_t hist_i=0; hist_i<n_hist; hist_i++) {
      Profile1D *a_hist=new Profile1D(hist_i,par);
      _histogramCol->addElement(a_hist);
    }

    return _histogramCol;
  }

  lcio::LCCollection *ProfileCollection_t::createProfiles(const std::string &collection_name, 
							      const EVENT::StringVec &name_list, const lcio::IntVec &n_hist_list, 
							      const HistPar &par) 
    throw (std::bad_alloc,std::runtime_error)
  {
    
    if (collection_name.empty()) {
      throw std::runtime_error("HistMgr::createProfiles> The histogram name must not be empty.");
    }
    
    assert ( !n_hist_list.empty() );
    assert ( name_list.empty()  || name_list.size()==n_hist_list.size() );
    
    for (EVENT::StringVec::const_iterator name_iter=name_list.begin();
	 name_iter!=name_list.end();
	 name_iter++) {
      assert ( !name_iter->empty() );
    }
    
    if (par.xMax()<=par.xMin() || par.nBins()==0) {
      throw std::runtime_error("HistMgr::createProfiles> The number of bins is zero or the range is empty.");
    }
    
    delete _histogramCol;
    delete _majorIndex;

    _histogramCol=new lcio::LCCollectionVec( lcio::LCIO::LCGENERICOBJECT );
    //DEBUG    stat_add(_histogramCol);

    if (name_list.empty()) {
      EVENT::StringVec temp;
      temp.push_back(collection_name);
      _histogramCol->parameters().setValues(ProfileCollection_t::__histogramNameParameterName,temp);
    }
    else {
      _histogramCol->parameters().setValues(ProfileCollection_t::__histogramNameParameterName,*(const_cast<EVENT::StringVec*>(&name_list)));
    }
    
    UInt_t n_hist=0;
    lcio::IntVec *index=new lcio::IntVec;
    index->resize(n_hist_list.size()+1);
    for (UInt_t index_i=0; index_i<n_hist_list.size(); index_i++) {
      (*index)[index_i]=n_hist;
      n_hist+=n_hist_list[index_i];
    }
    (*index)[n_hist_list.size()]=n_hist;

    _histogramCol->parameters().setValues(ProfileCollection_t::__majorIndexParameterName,*index);
    _majorIndex=index;
    
    for (UInt_t hist_i=0; hist_i<n_hist; hist_i++) {
      Profile1D *a_hist=new Profile1D(hist_i,par);
      _histogramCol->addElement(a_hist);
    }
    return _histogramCol;
  }

}
