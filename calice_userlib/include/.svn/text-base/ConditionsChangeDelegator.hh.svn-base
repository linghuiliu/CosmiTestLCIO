#ifndef _ConditionsChangeDelegator_hh_
#define _ConditionsChangeDelegator_hh_

#include <lccd/IConditionsChangeListener.hh>
#include <string>

namespace CALICE {
  /** Listens for conditions data changes and delegates the information to a class.
   * a class can redirect several conditions data changes to itself.
   * @sa testConditionsDataDelegator.cc
   */ 
  template <class T>
  class ConditionsChangeDelegator : public lccd::IConditionsChangeListener {
  public:
    typedef void (T::* ConditionsChangeHandleFunc_t)(lcio::LCCollection* col);
    
    ConditionsChangeDelegator(T *responsible, ConditionsChangeHandleFunc_t a_func)
      : _funcP(a_func),
	_responsible(responsible)
    {
    };

    /** Be notified if conditions data changes and redirect changes to responsible class.
     * @param col the collection containing the new conditions data
     */
    void conditionsChanged( lcio::LCCollection* col ) {
      (_responsible->*_funcP)(col);
    };

  private:
    ConditionsChangeHandleFunc_t _funcP; /**< method to be called if the conditions data changed*/
    T *_responsible;                     /**< the object which will take care of the change*/
  };
}
#endif
