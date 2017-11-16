#ifndef _GraphCollection_t_hh_
#define _GraphCollection_t_hh_

#include <EVENT/LCCollection.h>
#include <EVENT/LCParameters.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <LCIOSTLTypes.h>
#include <lcio.h>
#include <vector>
#include <string>
#include <stdexcept>
#include "reference_counter.h"

#ifdef BOUNDARY_CHECK
#  include<iostream>
#  include <sstream>
#  include <cassert>
#endif

namespace histmgr {
/** Light wrapper around a LCCollection to facilitate the handling of collections of graphs.
 * All the graphs of the collection share the same x values. 
 * The wrapper supports 1 or 2 dimensional collections of graphs. Where each graph collection
 * is composed of several graph groups and each graph group contains a fixed set of graphs of
 * different types. More correctly the collection has a 2 dimensional or 3d diemensional structure.
 */
  class GraphCollection_t : public reference_counter
{
  friend class HistMgr;
public:

  GraphCollection_t(const std::string &collection_name, unsigned int n_graphs, 
		    const EVENT::StringVec &type_names, unsigned int n_expected_values, 
		    const EVENT::StringVec &opt_major_names)
  throw (std::bad_alloc, std::runtime_error);

  GraphCollection_t(const std::string &collection_name, const std::vector<unsigned int> &n_graphs,
		    const std::vector<std::string> &type_names, unsigned int n_expected_values,
		    const std::vector<std::string> &opt_major_names)
    throw (std::bad_alloc, std::runtime_error);


  /**
   * @param graphs the linearised one or two dimensional collection of graphs
   * @param indices optional list of indicies used to give two dimensional access to the histogram list.
   *         for each possible index of the first dimension is needed which contains the offset in the
   *         list. The second index is added to this offset.
   */
  GraphCollection_t(lcio::LCCollection *graphs, lcio::IntVec *indices);

  /** Create a histogram collection from lcio collection. 
   * @param graphs the linearised one or two dimensional collection of graphs (two dimensional collections have the collection parameter "major").
   * The method does not verify whether the lcio collection really is a histogram collection. However, if the collection has the parameter "major"
   * it creates an index vector (costly operation) assuming that it is a 2d array instead of a 1d array(i.e. collection).
   * The unique group id remains undedfined since it is not stored in the lcio collection.
   */
  GraphCollection_t(lcio::LCCollection *graphs);

  /** copy constructor.
   */
  GraphCollection_t(const GraphCollection_t &a) 
    : _graphCol(a._graphCol),
      _nGraphs(a._nGraphs),
      _majorIndex(0),
      _nTypes(a._nTypes)
  {
    if (a._majorIndex) {
      _majorIndex=new lcio::IntVec;
      _majorIndex->assign(a._majorIndex->begin(),a._majorIndex->end());
    }
    
    // the names are only copied if they are needed.
    // _typeNameList.assign(a._typeNameList.begin(),a._typeNameList.end());
    // _nameList.assign(a._nameList.begin(),a._nameList.end());
  };

  // default constructor
  GraphCollection_t() 
    : _graphCol(0),
      _nGraphs(0),
      _majorIndex(0),
      _nTypes(0)
  {};
    
  ~GraphCollection_t() {
    // the histogram may not be owned by this wrapper, so it must not be deleted automatically.
    // The major index is always owned by this wrapper, so it should be deleted
    delete _majorIndex;
  }

  /** delete the histogram collection and the index array
   * This method exists instead of a destrctor to prevent copying the arrays (FIXME)
   */
  void deleteCollection() {
    delete _graphCol;
    _graphCol=0;
    delete _majorIndex;
    _majorIndex=0;
  };

  void deleteSharedStorage() {
    deleteCollection();
  }


  //  /** get the unique group id
  //   */
  //  UInt_t id() const {return _id;};

  /** get the collection of graphs.
   * The index array needed for the two dimensional acces is added as a parameter named "major".
   */
  lcio::LCCollection *collection() {return _graphCol;};

  /** get the collection of graphs (read only).
   * The index array needed for the two dimensional acces is added as a parameter named "major".
   */
  const lcio::LCCollection *collection() const {return _graphCol;};

  /** Get the number of graphs.
   * This value does not take into account that for each graph several types are created.
   */
  unsigned int n() const {
    return _nGraphs;
  }

  /** Check whether the graph collection contains more than one graph group.
   * @return true if the graph collection contains more than one graph group which can be addressed by a major index.
   */
  bool is2D() const {
    return _majorIndex && _majorIndex->size()>1;
  };

  /* Get the number of graph types.
   * nTypes() times n() graphs are created.
   */
  unsigned int nTypes() const {
    return _nTypes;
  }

  /** Get the number of major indices.
   * The collection is sliced in nMajor slices each having nMinor(major_index) elements.
   */
  unsigned int nMajor() const {
#ifdef BOUNDARY_CHECK
    assert(_majorIndex);
#endif
    return _majorIndex->size()-1;
  };

  /** Get the number of elements for the collection slice addressed by the given major index.
   * The major index must be valid.
   */
  unsigned int nMinor(unsigned int major_index) const {
#ifdef BOUNDARY_CHECK
    assert (major_index+1 < _majorIndex->size());
#endif
    return (*_majorIndex)[major_index+1]-(*_majorIndex)[major_index];
  };

  //    typedef lcio::LCGenericObjectImpl LCDoubleArrayObject;

  /** Create a collection of graphs.
   * Where for each element the given number of graph types is created.
   */
  GraphCollection_t(const std::string &collection_name, unsigned int n_graphs, 
		    const EVENT::StringVec &type_names, unsigned int n_expected_values,
		    const EVENT::StringVec &opt_major_names, bool may_overwrite=false)
    throw (std::bad_alloc, std::runtime_error);

  /** Create a 2D collection of graphs.
   * Where for each element of the 2 dimensional collection, the not only a single but for each graph type one..
   */
  GraphCollection_t(const std::string &collection_name, const std::vector<int> &n_graphs,
		    const std::vector<std::string> &type_names, unsigned int n_expected_values,
		    const std::vector<std::string> &opt_major_names, bool may_overwrite=false)
    throw (std::bad_alloc, std::runtime_error);

  /** Get the name of the graph group addressed by the major index.
   * The result is undefined if the graph group is not 2D.
   */
  std::string &getMajorName(unsigned int major_index, unsigned int type_index) const;

  /** Get the full name of the graph which is part of a specfic graph group addressed by the major index.
   * @param major_index the index of the sub graph group.
   * @param minor_index the index of the graph in the graph group.
   * @param type_index the index of the graph type.
   * @return the full graph name.
   * The result is undefined if the graph group is not 2D.
   */
  std::string &getName(unsigned int major_index, unsigned int minor_index, unsigned int type_index) const;

  /** Get the full name of the graph addressed by the index.
   * @param index the index of the graph in the graph group.
   * @param type_index the index of the graph type.
   * @return the full graph name.
   * The result is undefined if the graph group is not 2D.
   */
  std::string &getName(unsigned int index, unsigned int type_index) const;
    
  /** Set the common x value of all the graphs of the graph collection.
   * @return the index of the new value which can be used to set the corresponding values in all the other graphs.
   */
  unsigned int appendXValue(double x_value) {
    //      lcio::LCGenericObjectImpl *x_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(0));
    //      unsigned int n_elements=x_graph->getNDouble();
    //      x_graph->setDoubleVal(n_elements,x_value);
    return appendValue(0,x_value);
  }

protected:
  /** append a new value to the addressed graph,
   */
  unsigned int appendFloatValue(unsigned int full_index, float a_value) {
#ifdef BOUNDARY_CHECK
    assert ( full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()) );
#endif
    lcio::LCGenericObjectImpl *x_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
    unsigned int n_elements=x_graph->getNFloat();
    x_graph->setFloatVal(n_elements,a_value);
    return n_elements;
  }

  /** append a new value to the addressed graph,
   */
  unsigned int appendValue(unsigned int full_index, double a_value) {
#ifdef BOUNDARY_CHECK
    assert ( full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()) );
#endif
    lcio::LCGenericObjectImpl *x_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
    unsigned int n_elements=x_graph->getNDouble();
    x_graph->setDoubleVal(n_elements,a_value);
    return n_elements;
  }


  /** set a value of the addressed graph,
   */
  void setFloatValue(unsigned int full_index, unsigned int value_index, float a_value) {
#ifdef BOUNDARY_CHECK
    assert (full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
#endif
    lcio::LCGenericObjectImpl *a_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
#ifdef BOUNDARY_CHECK
    unsigned int n_elements=a_graph->getNFloat();
    assert (value_index < n_elements+1);
#endif
    a_graph->setFloatVal(value_index,a_value);
  }

  /** set a value of the addressed graph,
   */
  void setValue(unsigned int full_index, unsigned int value_index, float a_value) {
#ifdef BOUNDARY_CHECK
    assert (full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
#endif
    lcio::LCGenericObjectImpl *a_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
#ifdef BOUNDARY_CHECK
    unsigned int n_elements=a_graph->getNDouble();
    assert (value_index < n_elements+1);
#endif
    a_graph->setDoubleVal(value_index,a_value);
  }

  /** set a value of the addressed graph,
   */
  double getValue(unsigned int full_index, unsigned int value_index) const {
#ifdef BOUNDARY_CHECK
    assert (full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
#endif
    lcio::LCGenericObjectImpl *a_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
#ifdef BOUNDARY_CHECK
    unsigned int n_elements=a_graph->getNDouble();
    assert (value_index < n_elements+1);
#endif
    return a_graph->getDoubleVal(value_index);
  }

  /** set a value of the addressed graph,
   */
  double getFloatValue(unsigned int full_index, unsigned int value_index) const {
#ifdef BOUNDARY_CHECK
    assert (full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
#endif
    lcio::LCGenericObjectImpl *a_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
#ifdef BOUNDARY_CHECK
    unsigned int n_elements=a_graph->getNFloat();
    assert (value_index < n_elements+1);
#endif
    return a_graph->getFloatVal(value_index);
  }

  /** Add the given value to a certain value of the addressed graph.
   */
  void addToFloatValue(unsigned int full_index, unsigned int value_index, float a_value) {
#ifdef BOUNDARY_CHECK
    assert (full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
#endif
    lcio::LCGenericObjectImpl *a_graph=static_cast<lcio::LCGenericObjectImpl *>(_graphCol->getElementAt(full_index));
#ifdef BOUNDARY_CHECK
    unsigned int n_elements=a_graph->getNFloat();
    assert (value_index < n_elements+1);
#endif
    a_graph->setFloatVal(value_index,a_graph->getFloatVal(value_index)+a_value);
  }

  /** Calculate the full graph index in case of 2d graph groups.
   */
  unsigned int getIndex(unsigned int major_index, unsigned int minor_index, unsigned int type_index) const {
#ifdef BOUNDARY_CHECK
    if (!is2D()) {
      std::stringstream message;
      message << "GraphCollection::addYValue> Major index specified but graph group has only one dimension.";
      throw std::logic_error(message.str());
    }
    assert (major_index+1 < _majorIndex->size());
    assert (minor_index   < static_cast<unsigned int>((*_majorIndex)[major_index + 1]));
    assert (type_index < nTypes());
#endif
    return ((*_majorIndex)[major_index]+minor_index)*nTypes()+type_index+1;
  }

  /** Calculate the full graph index in case of a flat graph group.
   */
  unsigned int getIndex(unsigned int index, unsigned int type_index) const{
#ifdef BOUNDARY_CHECK
    if (is2D()) {
      std::stringstream message;
      message << "GraphCollection::addYValue> Major index specified but graph group, has only one dimension.";
      throw std::logic_error(message.str());
    }
    assert (index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
    assert (type_index < nTypes());
#endif
    return index*nTypes()+type_index+1;
  }

public:
  /** Set the y-value of the adddressed graph.  
   */
  void setYValue(unsigned int major_index, unsigned int minor_index, unsigned int type_index, unsigned int value_index, float a_value) {
    unsigned int full_index=getIndex(major_index, minor_index, type_index);
    setFloatValue(full_index,value_index, a_value);
      
  }

  /** Set the y-value of the adddressed graph.  
   */
  void setYValue(unsigned int index, unsigned int type_index, unsigned int value_index, float a_value) {
    unsigned int full_index=getIndex(index, type_index);
    setFloatValue(full_index, value_index, a_value);
  }

  /** Get the y-value of the adddressed graph.  
   */
  float getYValue(unsigned int index, unsigned int type_index, unsigned int value_index) const {
    unsigned int full_index=getIndex(index, type_index);
    return getFloatValue(full_index, value_index);
  }

  /** Get the y-value of the adddressed graph.  
   */
  float getYValue(unsigned int major_index, unsigned int minor_index, unsigned int type_index, unsigned int value_index) const {
    unsigned int full_index=getIndex(major_index, minor_index, type_index);
    return getFloatValue(full_index, value_index);
  }

  /** Get the x-value of the adddressed graph.  
   */
  double getXValue(unsigned int index, unsigned int value_index) const {
    unsigned int full_index=getIndex(index, 0);
    return getValue(full_index, value_index);
  }


  /** Get the name of the specified element of the graph collection.
   * @param major_index the index of the histogram element or in case of an 2D histogram array the major index.
   * @return a reference to the name.
   * The index must be valid.
   */
  const std::string &getName(unsigned int major_index) const {
    if (_nameList.empty()) copyNames();
    if (_nameList.size()==1) {
      return _nameList[0];
    }
#ifdef BOUNDARY_CHECK
    assert( major_index < _nameList.size());
#endif
    return _nameList[major_index];
  };

  /** Get the addressed type name.
   * @param type_index the index of the type.
   * @return a reference to the type name.
   * The index must be valid.
   */
  const std::string &getTypeName(unsigned int type_index) const {
    if (_typeNameList.empty()) {
      _graphCol->getParameters().getStringVals(__typeNameParameterName,_typeNameList);
      if (_typeNameList.empty()) {
	_typeNameList.push_back("");
      }
    }
#ifdef BOUNDARY_CHECK
    assert( type_index < _typeNameList.size());
#endif
    return _typeNameList[type_index];
  }

protected:
  EVENT::LCGenericObject *getXArray() {
    return getArray(0);
  }

  EVENT::LCGenericObject *getArray(unsigned int full_index) {
#ifdef BOUNDARY_CHECK
    assert(_graphCol);
    assert( full_index < static_cast<unsigned int>(_graphCol->getNumberOfElements()));
#endif
    return dynamic_cast<EVENT::LCGenericObject *>(_graphCol->getElementAt(full_index));
  }

//   /** Assignment operation which deletes the old collections.
//    */
//   GraphCollection_t &operator=(const GraphCollection_t &a) {
//     if (_graphCol!=a._graphCol) {
//       delete _graphCol;
//       _graphCol=a._graphCol;
//     }
//     if (_majorIndex!=a._majorIndex) {
//       delete _majorIndex;
//       _majorIndex=a._majorIndex;
//     }
//     _nTypes=a._nTypes;
//     _nGraphs=a._nGraphs;
//     return *this;
//   }

private:

  /** Copy the names from the collection to an accessible vector.
   * The vector of histogram names is attached as a parameter to the LCCollection.
   */
  void copyNames() const {
    if (_nameList.empty()) {
      _graphCol->getParameters().getStringVals(__graphNameParameterName,_nameList);
      if (_nameList.empty()) {
	_nameList.push_back(__defaultGraphName);
      }
#ifdef BOUNDARY_CHECK
     assert(_nameList.size()==1 || _nameList.size() == nMajor());
#endif
    }
  }

  //  UInt_t _id;                         /**< unique group id*/
  lcio::LCCollection  *_graphCol;     /**< the histogram collection*/
  unsigned int         _nGraphs;      /**< the number of graphs*/
  lcio::IntVec        *_majorIndex;   /**< optional list of offset to form a two 
					   dimensional array out of the list.*/
  unsigned int         _nTypes;       /**< number of types.*/

  mutable lcio::StringVec  _typeNameList; /**< A vector which contains one name for all graphs of the collection
				               or a name for each element (this list is only filled if a names are accessed).*/

  mutable lcio::StringVec  _nameList; /**< A vector which contains one name for all graphs of the collection
					   or a name for each element.*/

  static const std::string __majorIndexParameterName;
  static const std::string __typeNameParameterName;
protected:
  static const std::string __graphNameParameterName;
private:
  static const std::string __defaultGraphName;
public:
  static const std::string __colorParameterName;
  static const std::string __widthParameterName;
  static const std::string __styleParameterName;
  static const std::string __majorColorParameterName;

};
}
#endif
