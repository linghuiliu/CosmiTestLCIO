#ifndef _ProfileCollection_t_hh_
#define _ProfileCollection_t_hh_

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include "HistPar.hh"
#include "Profile1D.hh"
#include "reference_counter.h"
#ifdef BOUNDARY_CHECK
# include <sstream>
#endif

namespace histmgr {

  class HistMgr;

  /** One or two dimensional collection of profile histograms.
   */
  class ProfileCollection_t : public reference_counter {
    friend class HistMgr;
  public:


    /** Create a collection of profile histograms which will have the same binning of the x-axis.
     * @param collection_name the of the histogram collection
     * @param name_list a vector which contains a name for each profile histogram of the collection.
     * @param n_hist the number of histograms to be created
     * @param par the binning of the histograms
     *
     * If the name_list is empty the name of the collection is used. The name will be extended by the index
     * in the collection.

     * The profile histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     */
    ProfileCollection_t(const std::string &collection_name,
                        const EVENT::StringVec &name_list, UInt_t n_hist,
                        const HistPar &par)
      throw (std::bad_alloc, std::runtime_error)
      : _histogramCol(0),
        _majorIndex(0)

    {
      createProfiles(collection_name, name_list, n_hist, par);
    }

    /** Create a collection of profile histograms which will have the same binning of the x-axis.
     * @param collection_name the of the histogram collection
     * @param name_list a vector containing the names of the profile histograms whose length is either equals
     *        n_hist or is zero. In the latter case the collection name is used for the all
     *        profile histograms.
     * @param n_hist_list the number of profile histograms to be created
     * @param par the binning of the histogram
     *
     * @return pointer to the histogram collection
     *
     * The profile histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     */
    ProfileCollection_t(const std::string &collection_name,
                        const EVENT::StringVec &name_list, const lcio::IntVec &n_hist_list,
                        const HistPar &par)
      : _histogramCol(0),
        _majorIndex(0)
    {
      createProfiles(collection_name,name_list, n_hist_list, par);
    }


    /**
     * @param profile_histograms the linearised one or two dimensional collection of profile histograms
     * @param indices optional list of indicies used to give two dimensional access to the profile histogram list.
     *         for each possible index of the first dimension is needed which contains the offset in the
     *         list. The second index is added to this offset.
     */
    ProfileCollection_t(lcio::LCCollection *profile_histograms, lcio::IntVec *indices)
      :_histogramCol(profile_histograms),
       _majorIndex(0)
    {
      if (indices) {
        if (indices->back()<profile_histograms->getNumberOfElements()) {
          indices->push_back(profile_histograms->getNumberOfElements());
        }
        // unfortunately there is currently no way to get the array which is attached to the LCIO collection.
        // so we have to maintain our own copy
        _majorIndex=new lcio::IntVec;
        _majorIndex->assign(indices->begin(),indices->end());
        profile_histograms->parameters().setValues(__majorIndexParameterName,*indices);
      }
    };

    /** Create a profile histogram collection from lcio collection.
     * @param profile_histograms the linearised one or two dimensional collection of profile histograms (two dimensional collections have the collection parameter "major").
     * The method does not verify whether the lcio collection really is a histogram collection. However, if the collection has the parameter "major"
     * it creates an index vector (costly operation) assuming that it is a 2d array instead of a 1d array(i.e. collection).
     * The unique group id remains undedfined since it is not stored in the lcio collection.
     */
    ProfileCollection_t(lcio::LCCollection *profile_histograms)
      : _histogramCol(profile_histograms),
        _majorIndex(0)
    {
      if (profile_histograms->parameters().getNInt(__majorIndexParameterName)>0) {
        _majorIndex=new lcio::IntVec;
        profile_histograms->parameters().getIntVals(__majorIndexParameterName,*_majorIndex);
      }
    };

    /** copy constructor.
     */
    ProfileCollection_t(const ProfileCollection_t &a)
      :_histogramCol(a._histogramCol),
       _majorIndex(0)
    {
      //    stat_copy(_histogramCol);
      if (a._majorIndex) {
        _majorIndex=new lcio::IntVec;
        _majorIndex->assign(a._majorIndex->begin(),a._majorIndex->end());
      }
      //    __instances++;
    };


    ~ProfileCollection_t() {
    }


    /** Default constructor.
     */
    ProfileCollection_t() : _histogramCol(0),_majorIndex(0) {};

    /** delete the histogram collection and the index array
     * This method exists instead of a destrctor to prevent copying the arrays (FIXME)
     */
    void deleteCollection() {
      delete _histogramCol;
      _histogramCol=0;
      delete _majorIndex;
      _majorIndex=0;
    };

    void deleteSharedStorage() {
      deleteCollection();
    }


    /** create several histograms which will have the same binning and the same name (except of a numeric extension).
     * @param collection_name the of the histogram collection
     * @param name_list a vector containing the names of the profile histograms whose length is either equals
     *        n_hist or is zero. In the latter case the collection name is used for the all
     *        profile histograms.
     * @param n_hist the number of histograms to be created
     * @param par the binning of the histograms
     *
     * @return pointer to the histogram collection.
     *
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     */
    lcio::LCCollection *createProfiles(const std::string &collection_name,
                                       const EVENT::StringVec &name_list, UInt_t n_hist,
                                       const HistPar &par)
      throw (std::bad_alloc,std::runtime_error);


    /** create several histograms which will have the same binning and the same name (except of a numeric extension).
     * @param collection_name the of the histogram collection
     * @param name_list a vector containing the names of the histograms whose length is either equals
     *        n_hist or is zero. In the latter case the collection name is used for the all
     *        histograms.
     * @param n_hist_list the number of histograms to be created
     * @param par the binning of the histogram
     *
     * @return pointer to the histogram collection
     *
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     */
    lcio::LCCollection *createProfiles(const std::string &collection_name,
                                       const EVENT::StringVec &name_list, const lcio::IntVec &n_hist_list,
                                       const HistPar &par)
      throw (std::bad_alloc,std::runtime_error);

    //  /** get the unique group id
    //   */
    //  UInt_t id() const {return _id;};

    /** get the collection of histograms.
     * The index array needed for the two dimensional acces is added as a parameter named "major".
     */
    lcio::LCCollection *collection() {return _histogramCol;};

    /** get the collection of histograms (read only).
     * The index array needed for the two dimensional acces is added as a parameter named "major".
     */
    const lcio::LCCollection *collection() const {return _histogramCol;};

    /** get the number of histograms in the one dimensional collection.
     */
    UInt_t n() const {
      return _histogramCol->getNumberOfElements();
    }

    /** Return true if the histogram collection is two instead of one diemensional.
     */
    bool is2D() const {
      return _majorIndex && _majorIndex->size()>1;
    };

    /** get the number of histograms in the one dimensional collection.
     */
    UInt_t nMajor() const {
#ifdef BOUNDARY_CHECK
      assert(_majorIndex);
#endif
      return _majorIndex->size()-1;
    };
    /** get the number of histograms in the one dimensional collection.
     */
    UInt_t nMinor(UInt_t major_index) const {
      return (*_majorIndex)[major_index+1]-(*_majorIndex)[major_index];
    };

    /** Get one histogram of the  collection (read only).
     */
    const Profile1D *histogram(UInt_t index) const {
#ifdef BOUNDARY_CHECK
      range_check(index);
#endif
      return dynamic_cast<Profile1D*>(_histogramCol->getElementAt(index));
    };

    /** Get one histogram of the  collection.
     */
    Profile1D *histogram(UInt_t index)  {
#ifdef BOUNDARY_CHECK
      range_check(index);
#endif
      return dynamic_cast<Profile1D*>(_histogramCol->getElementAt(index));
    };

    /** Get one histogram of the collection which is organised as a two dimensional array.
     */
    Profile1D *histogram(UInt_t major_index, UInt_t minor_index)  {
      UInt_t index=get_index(major_index,minor_index);
      return dynamic_cast<Profile1D*>(_histogramCol->getElementAt(index));
    };

    /** Get one histogram of the collection which is organised as a two dimensional array (read only).
     */
    const Profile1D *histogram(UInt_t major_index, UInt_t minor_index) const {
      UInt_t index=get_index(major_index,minor_index);
      return dynamic_cast<Profile1D*>(_histogramCol->getElementAt(index));
    };

    /** Get the name of the specified element of the histogram collection.
     * @param major_index the index of the histogram element or in case of an 2D histogram array the major index.
     * @return a reference to the name.
     */
    const std::string &getName(UInt_t major_index) const {
      if (_nameList.empty()) copyNames();
      if (_nameList.size()==1) {
        return _nameList[0];
      }
#ifdef BOUNDARY_CHECK
      assert( major_index < _nameList.size());
#endif
      return _nameList[major_index];
    };

  private:

    /** Copy the names from the collection to an accessible vector.
     * The vector of histogram names is attached as a parameter to the LCCollection.
     */
    void copyNames() const {
      if (_nameList.empty()) {
        _histogramCol->getParameters().getStringVals(__histogramNameParameterName,_nameList);
        if (_nameList.empty()) {
          _nameList.push_back(__defaultProfileName);
        }
      }
    }

    lcio::LCCollection  *_histogramCol;   /**< the histogram collection*/
    lcio::IntVec        *_majorIndex;     /**< optional list of offset to form a two
                                             dimensional array out of the list.*/
    mutable lcio::StringVec  _nameList;   /**< A vector which contains one name for all histograms of the collection
                                             or a name for each element.*/

    static const std::string __majorIndexParameterName;


  protected:
#ifdef BOUNDARY_CHECK
    void range_check(UInt_t index) const throw (std::range_error) {
      if (index>=static_cast<UInt_t>(_histogramCol->getNumberOfElements())) {
        std::stringstream message;
        message << "Range error accessing histogram " << index << ". Ther are only "
                << _histogramCol->getNumberOfElements() << " profile histograms in the collection.";
        throw std::range_error(message.str());
      }
    }
#endif

    UInt_t get_index(UInt_t major_index, UInt_t minor_index) const throw (std::range_error)
    {
#ifdef BOUNDARY_CHECK
      if (!_majorIndex) {
        throw std::logic_error("A major index was specified but the index collection does not exist.");
      }

      if (major_index>=_majorIndex->size()) {
        std::stringstream message;
        message << "Major index " << major_index << " out of ranger. The index collection contains only "
                << _majorIndex->size() << " indices.";
        throw std::range_error(message.str());
      }
#endif

      UInt_t index=(*_majorIndex)[major_index]+minor_index;

#ifdef BOUNDARY_CHECK
      if (index>=static_cast<UInt_t>(_histogramCol->getNumberOfElements())) {
        std::stringstream message;
        message << "Range error accessing histogram " << index << ". Ther are only "
                << _histogramCol->getNumberOfElements() << " profile histograms in the collection.";
        throw std::range_error(message.str());
      }
#endif
      return index;
    }


    static const std::string __histogramNameParameterName;
  private:
    static const std::string __defaultProfileName;

  };
}
#endif
