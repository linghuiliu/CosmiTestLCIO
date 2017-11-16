#ifndef _HistMgr_hh_
#define _HistMgr_hh_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "HistPar.hh"
#include "FloatHistogram1D.hh"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include <lcio.h>
#include <iostream>

#ifdef HAVE_CALICEGUI
#  include <glibmm/thread.h>
#  include <GuiThread.hh>
#  include <unistd.h>
#endif

#ifdef BOUNDARY_CHECK
#include <cassert>
#endif

#include "KeyMap_t.hh"
#include "HistogramCollection_t.hh"
#include "Histogram2DCollection_t.hh"
#include "ProfileCollection_t.hh"
#include "GraphCollection_t.hh"

#include "accounting_ptr.h"

namespace histmgr {

  class HistWriterKit;

  /** Manages lists of histograms which can be written to prior assigned files.
   */
  class HistMgr
  {
    friend class HistMgrPtr;
  protected:
    /** id, destination file and folder of a histogram group
     * @todo remove _id
     */
    class HistogramGroupData_t {
      friend class HistMgr;
    public:
      HistogramGroupData_t() : _refCounter(0),_id(static_cast<unsigned int>(-1)), _version(0) {};

      HistogramGroupData_t(std::string file_name, std::string folder_name)
        : _refCounter(0),_id(static_cast<unsigned int>(-1)),_fileName(file_name),_folderName(folder_name), _version(0) {};

      UInt_t id() const {return _id;};
      UInt_t version() const {return _version;};
      void incrementVersion() {_version++;};
      const std::string &fileName() const {return _fileName;};
      const std::string &folderName() const {return _folderName;};

      HistogramGroupData_t &setFileName(const std::string &file_name)    { _fileName=file_name;return *this;};
      HistogramGroupData_t &setFolderName(const std::string &folder_name)  {_folderName=folder_name; return *this;};

      void ref() {_refCounter++;};
      void unref() {--_refCounter;};
      UInt_t references() const {return _refCounter;};

      bool isReferenced() const {return _refCounter>0;};

    protected:
      void setId(unsigned int id) {_id=id;};

      UInt_t      _refCounter;  /**< number of users of this group*/
      UInt_t      _id;          /**< unique histogram group id*/
      std::string _fileName;    /**< file to which all the histograms of this group will be written */
      std::string _folderName;  /**< folder inside the file to which all the histograms of this group will be written */
      UInt_t      _version;     /**> version of the histogram set, will be incremented upon writing */
    };

  public:

  protected:
    // typedef std::map<std::string, HistogramCollection_t > HistogramList_t;
    //  typedef std::map<std::string, HistogramGroupData_t > HistogramGroupList_t;
    typedef KeyMap_t<accounting_ptr<Histogram2DCollection_t> > Histogram2DList_t;
    typedef KeyMap_t<accounting_ptr<HistogramCollection_t> > HistogramList_t;
    typedef KeyMap_t<accounting_ptr<GraphCollection_t> >     GraphList_t;

  public:
    enum ECollectionType {kH1,kH2,kProfile,kGraph,kNTypes};

    static ECollectionType findType(const std::string &type_name);

  protected:

    class Group_t {
      friend class HistMgr;
    public:

      /** Called by the destructor of KeyMap_t .
       */
      void deleteSharedStorage() {};

      /** Get the histogram/graph group header.
       * The header contains the assignment to files and folders.
       */
      HistogramGroupData_t &header() {return _header;};

      /**  Get the histogram/graph group header.(read only).
       * The header contains the assignment to files and folders.
       */
      const HistogramGroupData_t &header() const {return _header;};

      /** Return true if the Histogram group contains collections of histograms.
       */
      bool hasHistogramCollections() const { return !_histogramList.empty();};

      /** Install the histogram collection and assign the key.
       */
      void setHistogramCollection(const Key_t &a_key, HistogramCollection_t *ref) {
        _histogramList.assign(a_key,accounting_ptr<HistogramCollection_t>(ref));
      };

      /** Verify whether a histogram collection exists
       */
      bool histogramCollectionExists(const Key_t &a_key) const {return _histogramList.hasKey(a_key);};

      /** Get the histogram collection addressed by the key.
       */
      HistogramCollection_t &histogramCollection(const Key_t &a_key) {return *(_histogramList[a_key]);}

      /** Get the histogram collection addressed by the key (read only).
       */
      const HistogramCollection_t &histogramCollection(const Key_t &a_key) const {return *(_histogramList[a_key]);}

      /** Return true if the Histogram group contains collections of 2D histograms.
       */
      bool hasHistogram2DCollections() const { return !_histogram2DList.empty();};

      /** Install the histogram collection and assign the key.
       */
      void setHistogram2DCollection(const Key_t &a_key, Histogram2DCollection_t *ref) {
        _histogram2DList.assign(a_key,accounting_ptr<Histogram2DCollection_t>(ref));
      }

      /** Verify whether a histogram collection exists
       */
      bool histogram2DCollectionExists(const Key_t &a_key) const {return _histogram2DList.hasKey(a_key);}

      /** Get the histogram collection addressed by the key.
       */
      Histogram2DCollection_t &histogram2DCollection(const Key_t &a_key) {return *(_histogram2DList[a_key]);}

      /** Get the histogram collection addressed by the key (read only).
       */
      const Histogram2DCollection_t &histogram2DCollection(const Key_t &a_key) const {return *(_histogram2DList[a_key]);}


      /** Return true if the Histogram group contains collections of profile histograms.
       */
      bool hasProfileCollections() const { return !_profileList.empty();}

      /** Install the histogram collection and assign the key.
       */
      void setProfileCollection(const Key_t &a_key, ProfileCollection_t *ref) {
        _profileList.assign(a_key,accounting_ptr<ProfileCollection_t>(ref));
      }

      /** Verify whether a histogram collection exists
       */
      bool profileCollectionExists(const Key_t &a_key) const {return _profileList.hasKey(a_key);}

      /** Get the histogram collection addressed by the key.
       */
      ProfileCollection_t &profileCollection(const Key_t &a_key) {return *(_profileList[a_key]);}

      /** Get the histogram collection addressed by the key (read only).
       */
      const ProfileCollection_t &profileCollection(const Key_t &a_key) const {return *(_profileList[a_key]);}


      /** Return true if the Histogram group contains collections of graphs.
       */
      bool hasGraphCollections() const { return !_graphList.empty();}

      /** Install the histogram collection and assign the key.
       */
      void setGraphCollection(const Key_t &a_key, GraphCollection_t *ref) {
        _graphList.assign(a_key,accounting_ptr<GraphCollection_t>(ref));
      }

      /** Verify whether a histogram collection exists
       */
      bool graphCollectionExists(const Key_t &a_key) const {return _graphList.hasKey(a_key);}

      /** Get the graph collection addressed by the key.
       */
      GraphCollection_t &graphCollection(const Key_t &a_key) {return *(_graphList[a_key]);}

      /** Get the graph collection addressed by the key (read only).
       */
      const GraphCollection_t &graphCollection(const Key_t &a_key) const {return *(_graphList[a_key]);}


      /** Return true if the collection contains still elements.
       */
      bool hasCollection(ECollectionType type) const {
        const KeyMapBase_t *a_list=getNameList(type);
        return ! (a_list->empty());
      }

      /** Get the begin and end iterators of the name list.
       * This can be used to generate a name list of all the elements.
       */
      std::pair<KeyMapBase_t::NameMap_t::const_iterator, KeyMapBase_t::NameMap_t::const_iterator> nameListIterators(ECollectionType type)  const {
        const KeyMapBase_t *a_list=getNameList(type);
        return std::make_pair(a_list->nameListBegin(), a_list->nameListEnd());
      }

    protected:

      const KeyMapBase_t *getNameListConst(ECollectionType &type) const {
        const KeyMapBase_t *a_list=0;
        switch (type) {
        case kH1:
          a_list=&_histogramList;
          break;
        case kH2:
          a_list=&_histogram2DList;
          break;
        case kProfile:
          a_list=&_profileList;
          break;
        case kGraph:
          a_list=&_graphList;
          break;
        default:
          a_list=0;
          throw std::runtime_error("HistMgr::getNameList> unsupported histogram/graph type.");
        }
#ifdef BOUNDARY_CHECK
        assert ( a_list );
#endif
        return a_list;
      }

      const KeyMapBase_t *getNameList(ECollectionType &type) const {
        return getNameListConst(type);
      }

      KeyMapBase_t *getNameList(ECollectionType &type) {
        return const_cast<KeyMapBase_t *>(getNameListConst(type));
      }

      HistogramGroupData_t               _header;
      KeyMap_t<accounting_ptr<HistogramCollection_t> > _histogramList;
      KeyMap_t<accounting_ptr<Histogram2DCollection_t> > _histogram2DList;
      KeyMap_t<accounting_ptr<ProfileCollection_t> > _profileList;
      KeyMap_t<accounting_ptr<GraphCollection_t> >     _graphList;
    };

    //  std::vector<std::pair<HistogramGroupData_t, > > _histograms;
    typedef KeyMap_t<Group_t> HistogramGroupList_t;
    HistogramGroupList_t _histogramGroupList;

  public:
    bool histogramGroupExists(const Key_t &group_key) const {
      return _histogramGroupList.hasKey(group_key);
    }

    Group_t &histogramGroup(const Key_t &group_key) {
      return _histogramGroupList[group_key];
    }

    const Group_t &histogramGroup(const Key_t &group_key) const {
      return _histogramGroupList[group_key];
    }

  public:
    /** Get the addressed histrogram group.
     */
    Group_t &getOrCreatehistogramGroup(const Key_t &group_key) {
      if (!histogramGroupExists(group_key)) {
        _histogramGroupList.assign(group_key,Group_t());
        _histogramGroupList[group_key].header().setId(group_key.id());
      }
      return _histogramGroupList[group_key];
    }

  protected:
    /** constructor
     */
    HistMgr() : _writerKit(0)
#ifdef HAVE_CALICEGUI
              ,_deleteIfUnused(false)
              ,_refCounter(0)
#endif
    {  };

    /** destructor
     */
    ~HistMgr();
  public:
    /** Create an empty histogram group to which histograms can be assigned.
     * @param group_key the key to address the group
     * @return ID assigned to the group of the given name
     * The group name is uniqe. If the group exists already the id of the existing group is returned.
     *  @sa createHistograms
     */
    void createHistogramGroup(const Key_t &group_key) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      getOrCreatehistogramGroup(group_key);
      // UInt_t group_id;
      //    HistogramGroupList_t::const_iterator the_group=_groups.find(group_name);
      //    if (the_group==_groups.end()) {
      //      group_id=_groups.size();
      //      _groups.insert(make_pair(group_name,HistogramGroupData_t(group_id)));
      //}
      //    else {
      //      group_id=the_group->second.id();
      //    }
      //    return group_id;
    };

    /** create several histograms which will have the same binning and the same name (except of a numeric extension).
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key name the of the histogram collection
     * @param n_hist the number of histograms to be created
     * @param par the binning of the histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *createHistograms(const Key_t& group_key, const Key_t &collection_key, UInt_t n_hist,
                                         const HistPar &par, bool may_overwrite=false)
      throw (std::bad_alloc,std::runtime_error)
    {
      EVENT::StringVec temp;
      return createHistograms(group_key,collection_key, temp,n_hist,par,may_overwrite);
    }

    /** create several histograms which will have the same binning and the same name (except of a numeric extension).
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key the name of the histogram collection
     * @param name_list a vector containing the names of the histograms whose length is either equals
     *        n_hist or is zero. In the latter case the collection name is used for the all
     *        histograms.
     * @param n_hist the number of histograms to be created
     * @param par the binning of the histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *createHistograms(const Key_t& group_key, const Key_t &collection_key, const EVENT::StringVec &name_list,
                                         UInt_t n_hist, const HistPar &par, bool may_overwrite=false)
      throw (std::bad_alloc,std::runtime_error);

    /** create 2D array of histograms which will all have the same binning and the same name (except of a numeric extension).
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param n_hist_list an array of histograms per major index
     * @param par the binning of the histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *createHistograms(const Key_t &group_key, const Key_t &collection_key,
                                         const lcio::IntVec &n_hist_list, const HistPar &par,
                                         bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error)
    {
      EVENT::StringVec temp;
      return createHistograms(group_key,collection_key,temp,n_hist_list,par,may_overwrite);
    };

    /** create 2D array of histograms which will all have the same binning and the same name (except of a numeric extension).
     * To create an array of  2x3 histograms
     * <code>
     * lcio::IntVec index
     * index.push_back(3)
     * index.push_back(3)
     * histmgr::Key_t hist_arr_2d_key("hist_arr_2d")
     * histogramList->createHistograms(group_key,hist_arr_2d_key,index,HistPar(4,-.5,3.5));
     * </code>
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param name_list a vector containing the names of the histograms whose length is either one or equals
     *        the number of elements of n_hist_list
     * @param n_hist_list an array of histograms per major index
     * @param par the binning of the histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *createHistograms(const Key_t &group_key, const Key_t &collection_key, const EVENT::StringVec &name_list,
                                         const lcio::IntVec &n_hist_list, const HistPar &par,
                                         bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error);

    /** create several histograms which will have the same binning and the same name (except of a numeric extension).
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key the of the histogram collection
     * @param n_hist the number of histograms to be created
     * @param x_par the binning of the histogram along x
     * @param y_par the binning of the histogram along x
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *create2DHistograms(const Key_t& group_key, const Key_t &collection_key, UInt_t n_hist,
                                           const HistPar &x_par, const HistPar &y_par, bool may_overwrite=false)
      throw (std::bad_alloc,std::runtime_error)
    {
      EVENT::StringVec temp;
      return create2DHistograms(group_key,collection_key, temp,n_hist,x_par, y_par,may_overwrite);
    }

    /** create several histograms which will have the same binning and the same name (except of a numeric extension).
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key the name of the histogram collection
     * @param name_list a vector containing the names of the histograms whose length is either equals
     *        n_hist or is zero. In the latter case the collection name is used for the all
     *        histograms.
     * @param n_hist the number of histograms to be created
     * @param x_par the binning of the x-axis of the histogram
     * @param y_par the binning of the y-axis of the histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *create2DHistograms(const Key_t& group_key, const Key_t &collection_key, const EVENT::StringVec &name_list,
                                           UInt_t n_hist, const HistPar &x_par, const HistPar &y_par, bool may_overwrite=false)
      throw (std::bad_alloc,std::runtime_error);

    /** create 2D array of histograms which will all have the same binning and the same name (except of a numeric extension).
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param n_hist_list an array of histograms per major index
     * @param x_par the binning of the x-axis of the histogram
     * @param y_par the binning of the y-axis the histogram
     * @param may_overwrite flag to overwrite or not the histogram, if existing (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *create2DHistograms(const Key_t &group_key, const Key_t &collection_key,
                                           const lcio::IntVec &n_hist_list, const HistPar &x_par, const HistPar &y_par,
                                           bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error)
    {
      EVENT::StringVec temp;
      return create2DHistograms(group_key,collection_key,temp,n_hist_list,x_par,y_par,may_overwrite);
    };


    /** create 2D array of histograms which will all have the same binning and the same name (except of a numeric extension).
     * To create an array of  2x3 histograms
     * <code>
     * lcio::IntVec index
     * index.push_back(3)
     * index.push_back(3)
     * histmgr::Key_t hist_arr_2d_key("hist_arr_2d")
     * histogramList->createHistograms(group_key,hist_arr_2d_key,index,HistPar(4,-.5,.5));
     * </code>
     * The histograms are only written to a file if the histogram group, to which this histogram belongs,
     * is assigned to a file (@ref histmgr::HistMgr::assignFileName).
     * @param group_key the name of the group this histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param name_list a vector containing the names of the histograms whose length is either one or equals
     *        the number of elements of n_hist_list
     * @param n_hist_list an array of histograms per major index
     * @param x_par the binning of the x-axis of the histogram
     * @param y_par the binning of the y-axis the histogram
     * @param may_overwrite flag to overwrite or not the histogram (default: false)
     *
     * @return the index of the first histogram for
     */
    lcio::LCCollection *create2DHistograms(const Key_t &group_key, const Key_t &collection_key, const EVENT::StringVec &name_list,
                                           const lcio::IntVec &n_hist_list, const HistPar &x_par,const HistPar &y_par,
                                           bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error);


    /** create an array of profile histograms which will all have the same binning of the x-axis and the same name (except of a numeric extension).
     * @param group_key the name of the group this profile histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param n_hist the number of histograms to be created
     * @param par the binning of the x-axis of the profile histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return a pointer to the the profile histogram collection
     */
    lcio::LCCollection *createProfile(const Key_t& group_key, const Key_t &collection_key, UInt_t n_hist,
                                      const HistPar &par, bool may_overwrite=false)
      throw (std::bad_alloc,std::runtime_error)
    {
      EVENT::StringVec temp;
      return createProfile(group_key,collection_key, temp,n_hist,par,may_overwrite);
    }

    /** create an array of profile histograms which will all have the same binning of the x-axis and the same name (except of a numeric extension).
     * @param group_key the name of the group this profile histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param name_list a vector containing the names of the profile histograms whose length is either one or equals
     *        the number of elements of n_hist_list
     * @param n_hist the number of histograms to be created
     * @param par the binning of the x-axis of the profile histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return a pointer to the the profile histogram collection
     */
    lcio::LCCollection *createProfile(const Key_t& group_key, const Key_t &collection_key, const EVENT::StringVec &name_list,
                                      UInt_t n_hist, const HistPar &par, bool may_overwrite=false)
      throw (std::bad_alloc,std::runtime_error);

    /** create 2D array of profile histograms which will all have the same binning and the same name (except of a numeric extension).
     * @param group_key the name of the group this profile histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param n_hist_list an array of histograms per major index
     * @param par the binning of the x-axis of the profile histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return a pointer to the the profile histogram collection
     */

    lcio::LCCollection *createProfile(const Key_t &group_key, const Key_t &collection_key,
                                      const lcio::IntVec &n_hist_list, const HistPar &par,
                                      bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error)
    {
      EVENT::StringVec temp;
      return createProfile(group_key,collection_key,temp,n_hist_list,par,may_overwrite);
    };

    /** create 2D array of profile histograms which will all have the same binning of the x-axis.
     * @param group_key the name of the group this profile histogram belongs to
     * @param collection_key name of the collection to which the histogram belongs
     * @param name_list a vector containing the names of the profile histograms whose length is either one or equals
     *        the number of elements of n_hist_list
     * @param n_hist_list an array of histograms per major index
     * @param par the binning of the x-axis of the profile histogram
     * @param may_overwrite flag to overwrite or not the histogram, if already existing (default: false)
     *
     * @return a pointer to the the profile histogram collection
     */
    lcio::LCCollection *createProfile(const Key_t &group_key, const Key_t &collection_key, const EVENT::StringVec &name_list,
                                      const lcio::IntVec &n_hist_list, const HistPar &par,
                                      bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error);


    /** Create a collection of graphs.
     * The graphs are addressed by a major and a minor index. The graph collection can for example be used
     * to create mean, rms, min,max graphs per module.
     * The following parameters can be supplied to the collection:
     * <ul>
     *   <li>minor_width : a vector which contains the width for each minor index of the line to be drawn.</li>
     *   <li>minor_style : a vector which contains the style for each minor index used to draw the line.</li>
     *   <li>minor_color : a vector which contains the color for each minor index used to draw the line.</li>
     *   <li>major_color : a vector which contains the color for each major index used to draw the line. This color has precedence over the minor_color</li>
     * </ul>
     */
    //  lcio::LCCollection *createGraphCollection(const Key_t &group_key, const Key_t &collection_key, unsigned int n_graphs,
    //                                      const std::vector<std::string> &type_names, unsigned n_expected_values);

    //  lcio::LCCollection *createGraphCollection2D(const std::string &group_key, const Key_t &collection_key, unsigned int n_major_indices,
    //                                        const std::vector<std::string> &minor_names, unsigned n_expected_values);

    lcio::LCCollection *createGraphCollection(const Key_t &group_key, const Key_t &collection_key, unsigned int n_graphs,
                                              const EVENT::StringVec &type_names, unsigned int n_expected_values,
                                              const EVENT::StringVec &opt_major_names,
                                              bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error);

    lcio::LCCollection *createGraphCollection(const Key_t &group_key, const Key_t &collection_key, const std::vector<int> &n_graphs,
                                              const std::vector<std::string> &type_names, unsigned int n_expected_values,
                                              const std::vector<std::string> &opt_major_names,
                                              bool may_overwrite=false)
      throw (std::bad_alloc, std::runtime_error);


    /** return the total number of entries in the whole histogram group.*/
    Double_t getNEntriesTotal(const Key_t &group_key) const;

    /** get a histogram of the collection with the given name.
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the index.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the index is out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param index the index within the collection
     * @return pointer to the histogram collection or zero.
     */
    FloatHistogram1D *getHistogram(const Key_t &group_key, const Key_t &hist_key, UInt_t index) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogramCollection(hist_key).histogram(index);

      //    HistogramList_t::iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(index);
    };

    /** get a histogram of the collection with the given name (read only).
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the index.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the index is out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param index the index within the collection
     * @return pointer to the histogram collection or zero.
     */
    const FloatHistogram1D *getHistogram(const Key_t &group_key, const Key_t &hist_key, UInt_t index) const{
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogramCollection(hist_key).histogram(index);
      //    HistogramList_t::const_iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(index);
    };

    /** get a histogram of the collection which is organised as a two dimensional array (read only).
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the indices.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the indices are out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param major_index the major index within the collection
     * @param minor_index the major index within the collection
     * @return pointer to the histogram collection or zero.
     */
    const FloatHistogram1D *getHistogram(const Key_t &group_key, const Key_t &hist_key, UInt_t major_index, UInt_t minor_index) const{
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogramCollection(hist_key).histogram(major_index,minor_index);

      //    HistogramList_t::const_iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(major_index,minor_index);
    };

    /** get a histogram of the collection which is organised as a two dimensional array.
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the indices.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the indices are out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param major_index the major index within the collection
     * @param minor_index the major index within the collection
     * @return pointer to the histogram collection or zero.
     */
    FloatHistogram1D *getHistogram(const Key_t &group_key, const Key_t &hist_key , UInt_t major_index, UInt_t minor_index) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogramCollection(hist_key).histogram(major_index,minor_index);
      //    HistogramList_t::iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(major_index,minor_index);
    };

    /** get a 2D histogram of the collection with the given name.
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the index.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the index is out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param index the index within the collection
     * @return pointer to the histogram collection or zero.
     */
    FloatHistogram2D *get2DHistogram(const Key_t &group_key, const Key_t &hist_key, UInt_t index) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogram2DCollection(hist_key).histogram(index);

      //    HistogramList_t::iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(index);
    };

    /** get a 2D histogram of the collection with the given name (read only).
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the index.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the index is out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param index the index within the collection
     * @return pointer to the histogram collection or zero.
     */
    const FloatHistogram2D *get2DHistogram(const Key_t &group_key, const Key_t &hist_key, UInt_t index) const{
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogram2DCollection(hist_key).histogram(index);
      //    HistogramList_t::const_iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(index);
    };

    /** get a 2D histogram of the collection which is organised as a two dimensional array (read only).
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the indices.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the indices are out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param major_index the major index within the collection
     * @param minor_index the major index within the collection
     * @return pointer to the histogram collection or zero.
     */
    const FloatHistogram2D *get2DHistogram(const Key_t &group_key, const Key_t &hist_key, UInt_t major_index, UInt_t minor_index) const{
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogram2DCollection(hist_key).histogram(major_index,minor_index);

      //    HistogramList_t::const_iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(major_index,minor_index);
    };

    /** get a 2D histogram of the collection which is organised as a two dimensional array.
     * The histogram must not be deleted.
     * Generally, the method does not check the validity of the indices.
     * the method throws exceptions if the collection does not exist or If BOUNDARY_CHECK
     * is defined and the indices are out of range.
     *
     * @param group_key name of the histogram's group
     * @param hist_key name of the histogram
     * @param major_index the major index within the collection
     * @param minor_index the major index within the collection
     * @return pointer to the histogram collection or zero.
     */
    FloatHistogram2D *get2DHistogram(const Key_t &group_key, const Key_t &hist_key , UInt_t major_index, UInt_t minor_index) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogram2DCollection(hist_key).histogram(major_index,minor_index);
      //    HistogramList_t::iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second.histogram(major_index,minor_index);
    };

    /** verify if a histogram of the given name exists already.
     */
    bool exists(const Key_t &group_key, const Key_t &hist_key) const {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return ( _histogramGroupList.hasKey(group_key) && _histogramGroupList[group_key].histogramCollectionExists(hist_key));
      //    HistogramList_t::const_iterator iter=_histograms.find(group+"::"+collection_name);
      //    return iter!=_histograms.end();
    };

    /** get the one or two dimensional histogram collection.
     */
    HistogramCollection_t &getHistogramCollection(const Key_t &group_key, const Key_t &hist_key) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogramCollection(hist_key);
      //    HistogramList_t::iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second;
    };

    /** get the one or two dimensional histogram collection (read only).
     */
    const HistogramCollection_t &getHistogramCollection(const Key_t &group_key, const Key_t &hist_key) const {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogramCollection(hist_key);
      //    HistogramList_t::const_iterator iter=_histograms.find(group+"::"+collection_name);
      //    if (iter==_histograms.end()) {
      //      std::stringstream message;
      //      message << "Histogram collection " << collection_name << " does not exist.";
      //      throw std::logic_error(message.str());
      //    }
      //    return iter->second;
    };

    /** Get a graph collection.
     */
    GraphCollection_t &getGraphCollection(const Key_t &group_key, const Key_t &graph_key) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].graphCollection(graph_key);
    };

    /** Get a graph collection.(read only).
     */
    const GraphCollection_t &getGraphCollection(const Key_t &group_key, const Key_t &graph_key) const {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].graphCollection(graph_key);
    };

    /** Get a histogram2D collection.
     */
    Histogram2DCollection_t &getHistogram2DCollection(const Key_t &group_key, const Key_t &histogram2D_key) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogram2DCollection(histogram2D_key);
    };

    /** Get a histogram2D collection.(read only).
     */
    const Histogram2DCollection_t &getHistogram2DCollection(const Key_t &group_key, const Key_t &histogram2D_key) const {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].histogram2DCollection(histogram2D_key);
    };

    /** Get a profile collection.
     */
    ProfileCollection_t &getProfileCollection(const Key_t &group_key, const Key_t &profile_key) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].profileCollection(profile_key);
    };

    /** Get a profile collection.(read only).
     */
    const ProfileCollection_t &getProfileCollection(const Key_t &group_key, const Key_t &profile_key) const {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif
      return _histogramGroupList[group_key].profileCollection(profile_key);
    };


    //   /** get the histogram collection of the given name (read only)
    //    * The collection must not be deleted.
    //    * @param name of the histogram collection
    //    * @return pointer to the histogram collection or zero.
    //    */
    //   const lcio::LCCollection *getCollection(const std::string &name) const {
    // #ifdef HAVE_CALICEGUI
    //     Glib::Mutex::Lock lock (_histogramListLock);
    // #endif
    //     HistogramList_t::const_iterator iter=_histograms.find(name);
    //     if (iter!=_histograms.end()) return iter->second.collection();
    //     // TODO: throw exception instead ?
    //     return 0;
    //   };

    //   /** get the histogram collection of the given name
    //    * The collection must not be deleted.
    //    * @param name of the histogram collection
    //    * @return pointer to the histogram collection or zero.
    //    */
    //   lcio::LCCollection *getCollection(const std::string &name)  {
    // #ifdef HAVE_CALICEGUI
    //     Glib::Mutex::Lock lock (_histogramListLock);
    // #endif
    //     HistogramList_t::iterator iter=_histograms.find(name);
    //     if (iter!=_histograms.end()) return iter->second.collection();
    //     // TODO: throw exception instead ?
    //     return 0;
    //   };

    /** assign a histogram group to the file of the given name
     * @param group_key the key of the histogram group
     * @param file_name the name of the file
     * @param folder_name the name of a folder insider the file
     */
    void assignFileName(const Key_t &group_key, const std::string &file_name, const std::string &folder_name="");

    /** Issue a warning for all histogram groups which are not assigned to a file.*/
    void warnOnUnassigned();

    /** write all histograms groups to the assigned files.
     * groups to which no file was assigned are not written
     * @see HistWriter, HistWriterKit
     */
    void writeHistograms(bool snapshot=false);

    /** write all histograms of the specified group to the assigned root file.
     * If no file name was
     * @see HistWriter, HistWriterKit
     */
    void writeHistograms(const Key_t &group_key, bool snapshot=false);

    /** register a histogram writer.
     * At least one writer is needed to write histograms to disk.
     */
    void registerWriter(const std::string &name, HistWriterKit *kit) {
      if (!name.empty() && kit) {
        _writerKitList.insert(std::make_pair(name,kit));
        if (!_writerKit) _writerKit=kit;
      }
    };

    void listRegisteredGroups() const;

    /** Get the number of groups.
     */
    UInt_t getNGroups() const {return _histogramGroupList.size();};

    /** Lock a histogram group.
     * If a group is locked it will not be deleted until the lock is released.
     * @sa unlockGroup
     */
    void lockGroup(const Key_t &group_key) {
#ifdef HAVE_CALICEGUI
      Glib::Mutex::Lock lock (_histogramListLock);
#endif

      _histogramGroupList[group_key].header().ref();

      //    HistogramGroupList_t::iterator the_group=_groups.find(group_name);
      //    // if the cgroup does not exist it is created.
      //    if (the_group==_groups.end()) {
      //      UInt_t group_id=_groups.size();
      //      std::pair<HistogramGroupList_t::iterator, bool> ret=_groups.insert(make_pair(group_name,HistogramGroupData_t(group_id)));
      //      if (!ret.second) {
      //        throw std::runtime_error("Could not add histogram group!");
      //      }
      //      the_group=ret.first;
      //    }
      //    the_group->second.ref();
    };

    /** Release the lock on the histogram group.
     * If a group is locked it will not be deleted until the lock is released.
     * @sa lockGroup
     */
    void unlockGroup(const Key_t &group_key);

    /** Return true if some of the groups are locked.
     */
    bool hasLockedGroups() const;


    /** Copy the groups names into the given vecotor.
     * @param dest_group_list the vector which will be filled with the group names.
     * First the given vector will be cleared. Then all group names will be copied to
     * it.
     */
    void fillGroupList(std::vector<std::string> &dest_group_list) const;

    /** Copy the histogram collection names of the specified group to the given vector.
     * @param group_key the name of the group whose histogram collection are considered.
     * @param type type of the collection to which the histogram belongs
     * @param dest_histogram_collection_list the vector which will be filled with the names
     *         of the histgram collections which are associated to the specified group.
     * First the given vector will be cleared. Then all the names of the histogram
     * collections which are associated to the specfied group will be copied to it.
     */
    void fillHistogramCollectionList(const Key_t &group_key, ECollectionType type, std::vector<std::string> &dest_histogram_collection_list) const;


    /** Split the name which combines the group name and the collection type into its components.
     */
    static std::pair<std::string, HistMgr::ECollectionType> getGroupNameAndCollectionType(const std::string &full_name);

  protected:
    void writeHistograms(Group_t &group, const std::string &group_name, bool snapshot);
  public:

    void deleteFolder(const std::string &folder);

    static HistMgr *getInstance() {
      if (!__histMgr) {
#ifdef HAVE_CALICEGUI
        __instanceCounter++;
        if (__instanceCounter==1)
#endif
          {
            HistMgr *a_hist_mgr=new HistMgr;
            __histMgr=a_hist_mgr;
            // ensure that glib thread package is initialised otherwise locking is not available.
            // can this be done more than once ?
            if (__histMgr!=a_hist_mgr) {
              delete a_hist_mgr;
#ifdef HAVE_CALICEGUI
              __instanceCounter--;
#endif
            }
          }
#ifdef HAVE_CALICEGUI
        else {
          while (!__histMgr) {
            usleep(10000);
          }
        }
#endif
      }
      return __histMgr;
    };
    static  void deleteInstance();

  protected:
    std::map<std::string, HistWriterKit *> _writerKitList;
    HistWriterKit *_writerKit;

    //HistogramList_t  _histograms;
    //  HistogramGroupList_t  _groups;

    static HistMgr *__histMgr;
#ifdef HAVE_CALICEGUI
    HistMgr *ref() {
      Glib::Mutex::Lock lock (_histogramListLock);
      _refCounter++;
      return __histMgr;
    };
    void unref() {
      {
        Glib::Mutex::Lock lock (_histogramListLock);
        --_refCounter;
      }
      if (_deleteIfUnused && _refCounter==0) {
        deleteInstance();
      }
    };
    bool isUnused() {
      Glib::Mutex::Lock lock (_histogramListLock);
      return _refCounter==0;
    };

    bool _deleteIfUnused;
    UInt_t _refCounter;
    static UInt_t __instanceCounter;
    static Glib::StaticMutex _histogramListLock;
#endif
    static const char *__typeName[kNTypes];
  };

  class HistMgrPtr
  {
  public:
    HistMgrPtr(HistMgr *an_instance)
#ifdef HAVE_CALICEGUI
      : _instance(an_instance->ref())
#endif
    {};

    ~HistMgrPtr() {
#ifdef HAVE_CALICEGUI
      _instance->unref();
#endif
    };

    operator bool() {return _instance!=0;};

    HistMgr *const operator->() {
      return _instance;
    };

    const HistMgr * const operator->() const {
      return _instance;
    };

    HistMgr * const operator*() {
      return _instance;
    };

    const HistMgr  *const operator*() const {
      return _instance;
    };

  private:
    HistMgr *_instance;
  };
}
#endif
