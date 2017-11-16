#ifndef _KeyMap_t_H_
#define _KeyMap_t_H_

#include <string>
#include <sstream>
#include <stdexcept>
#include <map>
#include <vector>


#ifdef BOUNDARY_CHECK
#  include <cassert>
#endif

#include "Key_t.hh"


namespace histmgr {

  /** Base class of the element list which only manages the name mapping.
   */
  class KeyMapBase_t
  {
    friend class HistMgr;
  public:
    typedef std::map<std::string, unsigned int> NameMap_t;
    NameMap_t::const_iterator nameListBegin() const {return _idList.begin();}
    NameMap_t::const_iterator nameListEnd() const   {return _idList.end();}

    bool empty() const { return _idList.empty();}

    unsigned int size() const {return _idList.size();}

    // TODO: find a better solution then creating keys in this unsafe manner
    static Key_t createKey(NameMap_t::const_iterator &iter) {
      Key_t a;            // initialise with dummy name;
      a.cache(iter->second);  // set already assigned id.
                              // if the id is set the name is not needed so the key can be initialised
                              // with a dummy name.
      return a;
    }

  protected:
    virtual ~KeyMapBase_t() {
      _idList.clear();
    }

    /** Check whether an element exist already for the given key.
     */
    bool _hasKey(const Key_t &a_key) const {
      if (!a_key.isSet()) {
        std::map<std::string, unsigned int>::const_iterator id=_idList.find(a_key.name());
        if (id==_idList.end()) {
          return false;
        }
        a_key.cache(id->second);
      }
      return true;
    }

    bool _insertNew( const Key_t &a_key, unsigned int next_element) {
      if (a_key.name().empty()) {
        throw std::logic_error("KeyMap_t::assign> Key name is empty");
      }
      std::pair<std::map<std::string, unsigned int>::iterator, bool> ret=_idList.insert(std::make_pair(a_key.name(),next_element));
      if (!ret.second)  {
        std::stringstream message;
        message << "KeyMap_t::assign> Failed to assign element to key " << a_key.name() << ".";
        throw std::runtime_error(message.str());
      }
      return true;
    }

    bool _setIndexCache(const Key_t &a_key) {
      if (!a_key.isSet()) {
        std::map<std::string, unsigned int>::const_iterator id=_idList.find(a_key.name());
        if (id==_idList.end()) {
          std::stringstream message;
          message << "KeyMap_t::elm> No elemenet exists for the key " << a_key.name() << ".";
          throw std::logic_error(message.str());
        }
        a_key.cache(id->second);
      }
      return true;
    }


  protected:
    NameMap_t _idList;

  };

  class HistMgr;
  template <typename _MapElm_t_> class KeyMap_t;

  /** Iterator for the KeyMap
   */
  template <typename _Map_t_>
  class KeyMapIterator_t
  {
    friend class KeyMap_t<_Map_t_>;

  protected:
    KeyMapIterator_t(std::map<std::string, unsigned int>::const_iterator iter,
                     std::map<std::string, unsigned int>::const_iterator end,
                     std::vector< _Map_t_ > * elm_list)
      : _iter(iter),_end(end),_elmListP(elm_list) {};

  public:
    KeyMapIterator_t &operator++(int) {do { _iter++; } while (_iter->second == Key_t::invalidKey() && _iter!=_end); return *this;};

    _Map_t_ & operator*() {
#ifdef BOUNDARY_CHECK
      assert (_iter->second < _elmListP->size());
#endif
      return (*_elmListP)[_iter->second];
    }

    const _Map_t_ & operator*() const {
#ifdef BOUNDARY_CHECK
      assert (_iter->second < _elmListP->size());
#endif
      return (*_elmListP)[_iter->second];
    }

    bool operator!=(const KeyMapIterator_t &a) {
      return _iter!=a._iter;
    }

    bool operator==(const KeyMapIterator_t &a) {
      return _iter==a._iter;
    }

    const std::string &name() const {return _iter->first;};
    //    Key_t makeKey() {
    //#ifdef BOUNDARY_CHECK
    //      assert (_iter != _end);
    //#endif
    //      return Key_t(_iter->first,_iter->second);
    //    }

    std::map<std::string, unsigned int>::const_iterator _iter;
    std::map<std::string, unsigned int>::const_iterator _end;
    std::vector< _Map_t_ > *_elmListP;
  };

  /** Const Iterator for the KeyMap
   */
  template <typename _Map_t_>
  class KeyMapConstIterator_t
  {
    friend class KeyMap_t<_Map_t_>;

  protected:
    KeyMapConstIterator_t(std::map<std::string, unsigned int>::const_iterator iter,
                          std::map<std::string, unsigned int>::const_iterator end,
                          const std::vector< _Map_t_ > * elm_list)
      : _iter(iter),_end(end),_elmListP(elm_list) {};
  public:
    KeyMapConstIterator_t &operator++(int) {do { _iter++; } while (_iter->second == Key_t::invalidKey() && _iter!=_end); return *this;};

    const _Map_t_ & operator*() const {
#ifdef BOUNDARY_CHECK
      assert (_iter->second < _elmListP->size());
#endif
      return (*_elmListP)[_iter->second];
    }

    bool operator!=(const KeyMapConstIterator_t &a) {
      return _iter!=a._iter;
    }

    bool operator==(const KeyMapConstIterator_t &a) {
      return _iter==a._iter;
    }

    const std::string &name() const {return _iter->first;};
    //    Key_t makeKey() {
    //#ifdef BOUNDARY_CHECK
    //      assert (_iter != _end);
    //#endif
    //      return Key_t(_iter->first,_iter->second);
    //    }

    std::map<std::string, unsigned int>::const_iterator _iter;
    std::map<std::string, unsigned int>::const_iterator _end;
    const std::vector< _Map_t_ > *_elmListP;
  };

  /** List of elements (KeyMap).
   * The elements are identified by their names. A contrinuous serial number is assigned
   * to each element. The serial number can be used to quickly access the histograms
   */

  template <typename _MapElm_t_>
  class KeyMap_t : public KeyMapBase_t
  {
    friend class HistMgr;

  public:
    ~KeyMap_t() {
      _elmList.clear(); // ?
    }

    /** Check whether an element exist already for the given key.
     */
    bool hasKey(const Key_t &a_key) const {
      bool ret;
      if (! (ret= _hasKey(a_key)) ) return ret;
#ifdef BOUNDARY_CHECK
      assert(a_key.id() < _elmList.size());
#endif
      return ret;
    }

    /** Assign an element to the given key.
     * @param a_key the key to which an element should be assigned.
     * @param elm the element.
     * If an element is already assigned to the key, then the old element is deleted.
     */
    void assign(const Key_t &a_key, const _MapElm_t_ &elm) {
      if (hasKey(a_key)) {
        //      _elmList[a_key.id()].deleteSharedStorage();
        _elmList[a_key.id()]=elm;
      }
      else {
        _insertNew(a_key,_elmList.size());
        _elmList.push_back(elm);
      }
    }


    /** Get the element assigned to the given key.
     * @param a_key the key of the element.
     */
    _MapElm_t_ &operator[](const Key_t &a_key) {
      _setIndexCache(a_key);
#ifdef BOUNDARY_CHECK
      assert(a_key.id() < _elmList.size());
#endif
      return _elmList[a_key.id()];
    }

    /** Get the element assigned to the given key (read only access).
     * @param a_key the key of the element.
     */
    const _MapElm_t_ &operator[](const Key_t &a_key) const {
      return const_cast<KeyMap_t *>(this)->operator[](a_key);
    }

    typedef KeyMapIterator_t<_MapElm_t_> iterator;
    typedef KeyMapConstIterator_t<_MapElm_t_> const_iterator;

    iterator begin() {
      return iterator(_idList.begin(),_idList.end(),&_elmList);
    }

    iterator end() {
      return iterator(_idList.end(),_idList.end(),0);
    }

    const_iterator begin() const {
      return const_iterator(_idList.begin(),_idList.end(),&_elmList);
    }

    const_iterator end() const {
      return const_iterator(_idList.end(),_idList.end(),0);
    }


  private:
    std::vector< _MapElm_t_ > _elmList;
  };

}
#endif
