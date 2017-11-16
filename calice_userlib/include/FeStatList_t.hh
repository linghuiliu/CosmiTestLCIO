#ifndef _FeStatList_t_hh_
#define _FeStatList_t_hh_

#include <BoardID.hh>
#include <vector>
#include <map>
#include <stdexcept>

#if( !defined(_GLIBCXX_IOSTREAM) && !defined(_CPP_IOSTREAM))
namespace std {
  class ostream;
};
#endif

/** Abstract interface of an object to collect and show some statistical data.
 * The idea is that one of these objects is created per Crc board front-end.
 * @sa FeStatListBase_t
 * @author Goetz Gaycken, LLR - Ecole polytechnique
 * @date feb, 2006
 */
class StatObj_t {
public:

  /** objects with virtual methods should also have a virtual destructor.
   */
  virtual ~StatObj_t() {}

  /** Method which can be used to perform some calculations just before the results are shown.
   */
  virtual void finish() = 0;

  /** Print a header before printing the contents of all the objects.
   * @param out the stream to which the table header will be printed.
   * This method should rather be a static function.
   * It is called once just before the statistics of all the objects will be printed.
   * It can be used to print a header to the given stream.
   * @sa FeStatListBase_t::show
   */
  virtual void printTableHeader(std::ostream &out) const = 0 ;
  
  /** Print the statistics collected for the corresponding front-end to the output stream.
   * @param out the stream to which the statistics will be written to.
   * @sa FeStatListBase_t::show
   */
  virtual void print(std::ostream &out) = 0 ;

  /** Return true if the statistics collected for the corresponding front-end should be printed.
   * In  @ref FeStatListBase_t::show, the statistics will only be printed for the corresponding 
   * front-end if this method returns true.
   */
  virtual bool hasInfo() const =0;
};

/** Container class which can store statistics objects for all existing front-ends.
 * The statistics collected from all front-ends can be printed to an output stream.
 * <B>This class should not be used directly but the template @ref FeStatList_t</B> 
 * which gives You type safety.
 */
class FeStatListBase_t {
public:

protected:
  typedef StatObj_t *StatObjPtr;
  
private:
  std::map< unsigned int, std::vector< std::vector< StatObjPtr> > >  _feInfo;

public:
  /** Print statistics about all front-ends. 
   * @param label text to be printed as a header line.
   * The output depends on the statistics objects used.
   * <B>NOTE:</B> This method will call the method @ref StatObj_t::finish i.e. the
   * objects should not be changed afterwords otherwise the result will
   * rather undefined.
   */
  void show(const char *label);
protected:

  ~FeStatListBase_t();
  
  /** Get the statistics object for the given board id.
   * @param board_id id which tells the crate, slot and front-end.
   * This method should not be used directly but the template 
   * @ref FeStatList_t::getStat
   */
  StatObjPtr &getStat(unsigned int board_id) {
    CALICE::BoardID decoded_board_id(board_id);

    // search for the crate 
    std::map<unsigned int, std::vector< std::vector< StatObjPtr > > >::iterator crate_iter=_feInfo.find(decoded_board_id.getCrateID());
    if ( crate_iter == _feInfo.end()) {
      std::pair< std::map<unsigned int, std::vector< std::vector< StatObjPtr > > >::iterator , bool > ret;
      ret=_feInfo.insert(make_pair(decoded_board_id.getCrateID(), std::vector< std::vector< StatObjPtr > >()) );
      if (!ret.second) {
	throw std::runtime_error("CrcFeConfigurationDataDriver::getStat> Failed to gather statistics about a certain front-end (out of memory?).");
      }
      crate_iter=ret.first;
    }

    // search for the slot
    unsigned int slot=decoded_board_id.getSlotID();

    // It is assumed that the number of slots is small. Therefore,
    // for each crate a linear array is created for at least 22 slots
    // the array will be increased if a slot number is larger.
    // If there are many empty slots >32 than this implementation
    // is highly inefficient.
    
    // there are now dummy slots with a slot id  above 22
    //#ifdef BOUNDARY_CHECK
    // A crate should have a fairly limited number of slots.
    //    assert( slot <= 32 );
    //#endif      
    if (slot>=22) {
      slot=22;
    }
    if (crate_iter->second.size() <= slot) {
      crate_iter->second.resize( (slot < 23 ? 23 : slot+1) );
    }

    // search for the front-end
    unsigned int board_component_nr=decoded_board_id.getBoardComponentNumber();
#ifdef BOUNDARY_CHECK
    assert (slot < crate_iter->second.size() );
    assert (board_component_nr < 32 );
#endif
    
    if (crate_iter->second[slot].size() <= board_component_nr) {
      crate_iter->second[slot].resize( ( board_component_nr < 7 ? 8 : board_component_nr+1) );
    }
    
#ifdef BOUNDARY_CHECK
    assert (board_component_nr < crate_iter->second[slot].size() );
#endif
    return crate_iter->second[slot][board_component_nr];
  };

};

/** Template container which stores specific statistics objects per front-end.
 * The statistics can be printed
 */
template <class T_StatObj_t>
class FeStatList_t : public FeStatListBase_t
{
public:
  /** Get a statistics object for the given front-end.
   * @param board_id the id of the crate,slot and front-end.
   * If no statistics object exists for the given front-end then
   * a new object is created using the default constructor i.e.
   * the objects T_StatObj_t must implement a default constructor.
   */
  T_StatObj_t &getStat(unsigned int board_id) {
     StatObjPtr &a_ptr=FeStatListBase_t::getStat(board_id);
     if (!a_ptr) {
       a_ptr=new T_StatObj_t;
     }
     return *(static_cast<T_StatObj_t*>(a_ptr));
  };
};
#endif
