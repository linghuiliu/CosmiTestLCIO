#ifndef CellParameterArray_t_hh
#define CellParameterArray_t_hh

#include <CellParameter.hh>
#include "SimpleArray_t.hh"

namespace CALICE {

//#include <SimpleTypeArray.hh>
//#include <ManagedArray.hh>
//typedef SimpleTypeArray<CellParameter> CellParameterPerModuleArray_t;
//typedef ManagedArray<CellParameterPerModuleArray_t> CellParameterArray_t;

/** Array to hold the cell parameters of all pads of a single module (obsolete).
 * This class is obsolete. Now a LCCollection is used instead.
 */
typedef SimpleArray_t<CellParameter> CellParameterPerModuleArray_t;

/** Array to hold the module cell parameter arrays of all modules (obsolete).
 * This class is obsolete. Now a LCCollection is used instead.
 */
typedef SimpleArray_t<CellParameterPerModuleArray_t> CellParameterArray_t;

}
#endif

