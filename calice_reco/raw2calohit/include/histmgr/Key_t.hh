#ifndef _Key_t_hh_
#define _Key_t_hh_

#include <string>
#include <cassert>

namespace histmgr {
  template <typename T> class KeyMap_t;

  /** A string key which will get a numeric identifier.
   * The primary key is the name. To prevent numerours string lookups, a
   * numeric identifier is assigned to each string key. The numeric identifier
   * is declared mutable. It will be set nevertheless of the const-ness of
   * the class.
   */
  class Key_t
  {
    template <typename T> friend class KeyMap_t;
    friend class KeyMapBase_t;
  public:
    Key_t(const std::string &key_name) : _name(key_name),_id(invalidKey()) {
      assert ( !key_name.empty() );
    };

    Key_t() : _id(invalidKey()) { };

    std::string &nameStorage() {return _name;};

    const std::string &name() const {return _name;};
    unsigned int id() const {return _id;};

    bool isSet() const {return _id!=invalidKey();};
    
    static unsigned int invalidKey() {return static_cast<unsigned int>(-1);}
    
  protected:
    void cache(unsigned int id) const {_id=id;};

  private:
    std::string _name;
    mutable unsigned int _id;
  };
}
#endif
