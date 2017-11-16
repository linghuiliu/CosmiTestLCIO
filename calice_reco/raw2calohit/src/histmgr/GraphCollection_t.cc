#include <GraphCollection_t.hh>
#include <EVENT/LCIO.h>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <RtypesSubSet.h>

namespace histmgr {
  const std::string GraphCollection_t::__majorIndexParameterName("major");
  const std::string GraphCollection_t::__typeNameParameterName("typeName");
  const std::string GraphCollection_t::__graphNameParameterName("graphName");
  const std::string GraphCollection_t::__defaultGraphName("Graph");
  const std::string GraphCollection_t::__colorParameterName("color");
  const std::string GraphCollection_t::__majorColorParameterName("majorColor");
  const std::string GraphCollection_t::__widthParameterName("width");
  const std::string GraphCollection_t::__styleParameterName("style");

  GraphCollection_t::GraphCollection_t(const std::string &collection_name, unsigned int n_graphs, 
				       const EVENT::StringVec &type_names, unsigned int n_expected_values, 
				       const EVENT::StringVec &opt_major_names, bool may_overwrite)
    throw (std::bad_alloc, std::runtime_error)
    : _graphCol(0),
      _majorIndex(0),
      _nTypes(0)
  {

    assert ( opt_major_names.empty() || opt_major_names.size()==n_graphs );
    for (EVENT::StringVec::const_iterator name_iter=opt_major_names.begin();
	 name_iter!=opt_major_names.end();
	 name_iter++) {
      assert ( !name_iter->empty() );
    }

    _graphCol=new lcio::LCCollectionVec( lcio::LCIO::LCGENERICOBJECT );

    if (opt_major_names.empty()) {
      EVENT::StringVec temp;
      temp.push_back(collection_name);
      _graphCol->parameters().setValues(__graphNameParameterName,temp);
    }
    else {
      _graphCol->parameters().setValues(__graphNameParameterName,*(const_cast<EVENT::StringVec*>(&opt_major_names)));
    }

    if (type_names.size()>0) {
      _graphCol->parameters().setValues(__typeNameParameterName,*(const_cast<EVENT::StringVec*>(&type_names)));
      _nTypes=type_names.size();
    }
    else {
      _nTypes=1;
    }
    _typeNameList.assign(type_names.begin(),type_names.end());
    _graphCol->getParameters().getStringVals(__graphNameParameterName,_nameList);
  
    for (UInt_t graph_i=0; graph_i<n_graphs*nTypes()+1; graph_i++) {
      IMPL::LCGenericObjectImpl *a_graph=new IMPL::LCGenericObjectImpl;
      _graphCol->addElement(a_graph);
    }
    _nGraphs=n_graphs;
    //  return _graphCol;
  }


  GraphCollection_t::GraphCollection_t(const std::string &collection_name, const std::vector<int> &n_graphs,
				       const std::vector<std::string> &type_names, unsigned int n_expected_values,
				       const std::vector<std::string> &opt_major_names, bool may_overwrite)
    throw (std::bad_alloc,std::runtime_error)
    : _graphCol(0),
      _majorIndex(0),
      _nTypes(0)
  {
  
    if (collection_name.empty()) {
      throw std::runtime_error("GraphCollection::createHistograms> The group name or histogram name must not be empty.");
    }
  
    assert ( !n_graphs.empty() );
    assert ( opt_major_names.empty()  || opt_major_names.size()==static_cast<unsigned int>(n_graphs.size()) );

    for (EVENT::StringVec::const_iterator name_iter=opt_major_names.begin();
	 name_iter!=opt_major_names.end();
	 name_iter++) {
      assert ( !name_iter->empty() );
    }
   
    _graphCol=new lcio::LCCollectionVec( lcio::LCIO::LCGENERICOBJECT );
    if (opt_major_names.empty()) {
      EVENT::StringVec temp;
      temp.push_back(collection_name);
      _graphCol->parameters().setValues(__graphNameParameterName,temp);
    }
    else {
      _graphCol->parameters().setValues(__graphNameParameterName,*(const_cast<EVENT::StringVec*>(&opt_major_names)));
    }

    if (type_names.size()>0) {
      _graphCol->parameters().setValues(__typeNameParameterName,*(const_cast<EVENT::StringVec*>(&type_names)));
      _nTypes=type_names.size();
    }
    else {
      _nTypes=1;
    }
    _typeNameList.assign(type_names.begin(),type_names.end());
  
    UInt_t n_gr=0;
    lcio::IntVec *index=new lcio::IntVec;
    index->resize(n_graphs.size()+1);
    for (UInt_t index_i=0; index_i<static_cast<UInt_t>(n_graphs.size()); index_i++) {
      (*index)[index_i]=n_gr;
      n_gr+=n_graphs[index_i];
    }
    (*index)[n_graphs.size()]=n_gr;
    _majorIndex=index;
    _graphCol->parameters().setValues(__majorIndexParameterName,*index);

    for (UInt_t gr_i=0; gr_i<n_gr*nTypes()+1; gr_i++) {
      IMPL::LCGenericObjectImpl *a_graph=new IMPL::LCGenericObjectImpl;
      _graphCol->addElement(a_graph);
    }
    _nGraphs=n_gr;

    //  return _graphCol;
  }

  GraphCollection_t::GraphCollection_t(lcio::LCCollection *graphs, lcio::IntVec *indices) 
    : _graphCol(graphs), 
      _majorIndex(0) 
  {
    if (indices) {
      if (indices->back()<graphs->getNumberOfElements()) {
	indices->push_back(graphs->getNumberOfElements());
      }

      // unfortunately there is currently no way to get the array which is attached to the LCIO collection.
      // so we have to maintain our own copy
      _majorIndex=new lcio::IntVec;
      _majorIndex->assign(indices->begin(),indices->end());
      _graphCol->parameters().setValues(__majorIndexParameterName,*indices);
    }
    
    _nTypes=graphs->getParameters().getNString(__typeNameParameterName);

    _nGraphs=_graphCol->getNumberOfElements();
    if (_nTypes<=0) {
      _nTypes=1;
    }
    else {
      _nGraphs/=_nTypes;
    }
  }

  GraphCollection_t::GraphCollection_t(lcio::LCCollection *graphs) 
    : _graphCol(graphs),
      _majorIndex(0)
  {
    if (graphs->parameters().getNInt(__majorIndexParameterName)>0) {
      _majorIndex=new lcio::IntVec;
      graphs->parameters().getIntVals(__majorIndexParameterName,*_majorIndex);
    }

    _nTypes=graphs->getParameters().getNString(__typeNameParameterName);

    // the names are only copied if they are needed.
    //    if (graphs->parameters().getNString(__typeNameParameterName)>0) {
    //      graphs->parameters().getStringValus(__typeNameParameterName,_typeNameList);
    //    }

    _nGraphs=_graphCol->getNumberOfElements();
    if (_nTypes<=0) {
      _nTypes=1;
    }
    else {
      _nGraphs/=_nTypes;
    }
  }

}
