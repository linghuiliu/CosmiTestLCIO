#ifndef cloneUtils_hh
#define cloneUtils_hh

namespace EVENT {
  class LCGenericObject;
  class LCObject;
  class LCCollection;
}

namespace CALICE {

/** Utility functions to clone LCGenericObjects 
  *  @author G. Gaycken (LLR) (put into own file by R. Pöschl (LAL)
  *  @date Feb 22 2006
  */

/** Different methods for object cloning */
EVENT::LCGenericObject *cloneLCGenericObject(EVENT::LCObject *obj);
EVENT::LCGenericObject *cloneLCGenericObject(EVENT::LCGenericObject *generic_object);
EVENT::LCCollection *cloneCollection(EVENT::LCCollection *col);
}

#endif
