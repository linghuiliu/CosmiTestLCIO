#ifndef NoiseParameterArray_t_hh
#define NoiseParameterArray_t_hh

#include <NoiseParameter.hh>
#include "SimpleArray_t.hh"

namespace CALICE {


/** Array to hold the cell parameters of all pads of a single module.
 */
typedef SimpleArray_t<NoiseParameter> NoiseParameterPerWaferArray_t;

/** Array to hold the module cell parameter arrays of all modules.
 */
typedef SimpleArray_t<NoiseParameterPerWaferArray_t> NoiseParameterArray_t;
typedef SimpleArray_t<NoiseParameterArray_t> NoiseParameterArrayofArray_t;



}
#endif

