#include "Histogram2DCollection_t.hh"
#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <lcio.h>


namespace histmgr {

  const std::string Histogram2DCollection_t::__majorIndexParameterName("major");
  const std::string Histogram2DCollection_t::__histogramNameParameterName("histName");
  const std::string Histogram2DCollection_t::__defaultHistogramName("Histogram2D");
  //  unsigned int Histogram2DCollection_t::__instances=0;


  lcio::LCCollection *Histogram2DCollection_t::createHistograms(const std::string &collection_name, 
								const EVENT::StringVec &name_list, UInt_t n_hist, 
								const HistPar &x_par,
								const HistPar &y_par) 
    throw (std::bad_alloc, std::runtime_error)
  {
    deleteCollection();

    if (collection_name.empty()) {
      throw std::runtime_error("HistMgr::createHistograms> The collection name must not be empty.");
    }
    
    assert ( name_list.empty() || name_list.size()==n_hist );
    for (EVENT::StringVec::const_iterator name_iter=name_list.begin();
	 name_iter!=name_list.end();
	 name_iter++) {
      assert ( !name_iter->empty() );
    }
    
    if (x_par.xMax()<=x_par.xMin() || x_par.nBins()==0) {
      throw std::runtime_error("HistMgr::createHistograms> The number of bins is zero or the range is empty.");
    }
    if (y_par.xMax()<=y_par.xMin() || y_par.nBins()==0) {
      throw std::runtime_error("HistMgr::createHistograms> The number of bins is zero or the range is empty.");
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
      _histogramCol->parameters().setValues(Histogram2DCollection_t::__histogramNameParameterName,temp);
    }
    else {
      _histogramCol->parameters().setValues(Histogram2DCollection_t::__histogramNameParameterName,*(const_cast<EVENT::StringVec*>(&name_list)));
    }
    
    for (UInt_t hist_i=0; hist_i<n_hist; hist_i++) {
      FloatHistogram2D *a_hist=new FloatHistogram2D(hist_i,x_par,y_par);
      _histogramCol->addElement(a_hist);
    }

    return _histogramCol;
  }

  lcio::LCCollection *Histogram2DCollection_t::createHistograms(const std::string &collection_name, 
								const EVENT::StringVec &name_list, const lcio::IntVec &n_hist_list, 
								const HistPar &x_par,
								const HistPar &y_par) 
    throw (std::bad_alloc,std::runtime_error)
  {
    
    if (collection_name.empty()) {
      throw std::runtime_error("HistMgr::createHistograms> The histogram name must not be empty.");
    }
    
    //    assert ( !n_hist_list.empty() );
    assert ( name_list.empty()  || name_list.size()==n_hist_list.size() );
    
    for (EVENT::StringVec::const_iterator name_iter=name_list.begin();
	 name_iter!=name_list.end();
	 name_iter++) {
      assert ( !name_iter->empty() );
    }
    
    if (x_par.xMax()<=x_par.xMin() || x_par.nBins()==0) {
      throw std::runtime_error("HistMgr::createHistograms> The number of bins is zero or the range is empty.");
    }
    if (y_par.xMax()<=y_par.xMin() || y_par.nBins()==0) {
      throw std::runtime_error("HistMgr::createHistograms> The number of bins is zero or the range is empty.");
    }
    
    //    std::string full_histogram_name=group;
    //    full_histogram_name+="::";
    //    full_histogram_name+=collection_name;

    delete _histogramCol;
    delete _majorIndex;

    _histogramCol=new lcio::LCCollectionVec( lcio::LCIO::LCGENERICOBJECT );
    //DEBUG    stat_add(_histogramCol);

    if (name_list.empty()) {
      EVENT::StringVec temp;
      temp.push_back(collection_name);
      _histogramCol->parameters().setValues(Histogram2DCollection_t::__histogramNameParameterName,temp);
    }
    else {
      _histogramCol->parameters().setValues(Histogram2DCollection_t::__histogramNameParameterName,*(const_cast<EVENT::StringVec*>(&name_list)));
    }
    
    UInt_t n_hist=0;
    lcio::IntVec *index=new lcio::IntVec;
    index->resize(n_hist_list.size()+1);
    for (UInt_t index_i=0; index_i<n_hist_list.size(); index_i++) {
      (*index)[index_i]=n_hist;
      n_hist+=n_hist_list[index_i];
    }
    (*index)[n_hist_list.size()]=n_hist;

    _histogramCol->parameters().setValues(Histogram2DCollection_t::__majorIndexParameterName,*index);
    _majorIndex=index;
    
    for (UInt_t hist_i=0; hist_i<n_hist; hist_i++) {
      FloatHistogram2D *a_hist=new FloatHistogram2D(hist_i,x_par,y_par);
      _histogramCol->addElement(a_hist);
    }
    return _histogramCol;
  }

//   void Histogram2DCollection_t::stat_add(lcio::LCCollection *a_col) {
//     if (!a_col) return;
//     std::map<lcio::LCCollection *,int>::iterator elm=__colCounter.find(a_col);
//     assert (elm == __colCounter.end() || elm->second==0);
//     __colCounter[a_col]=1;
//     stat_show("add");
//   }

//   void Histogram2DCollection_t::stat_copy(lcio::LCCollection *a_col) {
//     if (!a_col) return;
//     std::map<lcio::LCCollection *,int>::iterator elm=__colCounter.find(a_col);
//     assert (elm != __colCounter.end());
//     //    std::cout << "copy " << static_cast<void *>(a_col) << std::endl;
//   }

//   void Histogram2DCollection_t::stat_remove(lcio::LCCollection *a_col) {
//     if (!a_col) return;
//     std::map<lcio::LCCollection *,int>::iterator elm=__colCounter.find(a_col);
//     assert (elm !=__colCounter.end());
//     __colCounter[a_col]--;
//     stat_show("remove");
//   }

//   void Histogram2DCollection_t::stat_show(const std::string &label) {
//     std::cout << "Show Stat:" << label  << std::endl;
//     for (std::map<lcio::LCCollection *,int>::const_iterator iter=__colCounter.begin();
// 	 iter!=__colCounter.end();
// 	 iter++ ) {
//       std::cout << static_cast<void *>(iter->first) << " counter = " << iter->second << std::endl;
//     }
//   }

  //std::map<lcio::LCCollection *, int > Histogram2DCollection_t::__colCounter;
}
