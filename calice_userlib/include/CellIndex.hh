#ifndef _CellIndex_hh_
#define _CellIndex_hh_

#include <RtypesSubSet.h>
#ifdef BOUNDARY_CHECK
#  include <cassert>
#endif

namespace CALICE {
  // To not depend on Mokka the definitions are copied from
  //   Kernel/include/Control.hh
  // If the definitions change in there 

#define SHIFT_M 0
#define SHIFT_S 3
#define SHIFT_I 6
#define SHIFT_J 15
#define SHIFT_K 24
#define SHIFT_2 30
#define SHIFT_1 31
#define SHIFT_index 0
#define SHIFT_type  6
// 2014/8/1: wrongly had only 1 bit for "type" field, when 2 required
// problem found by D.Jeans, fix provided by R.Poeschl
//#define SHIFT_ID    7
//#define SHIFT_cell  15
//#define SHIFT_bad   30
#define SHIFT_ID    8
#define SHIFT_cell  16
#define SHIFT_bad  31 

#define MASK_M (unsigned int) 0x00000007
#define MASK_S (unsigned int) 0x00000038
#define MASK_I (unsigned int) 0x00007FC0
#define MASK_J (unsigned int) 0x00FF8000
#define MASK_K (unsigned int) 0x3F000000
#define MASK_2 (unsigned int) 0x40000000
#define MASK_1 (unsigned int) 0x80000000

#define MASK_index (unsigned int) 0x0000003F
// 2014/8/1: wrongly had only 1 bit for "type" field, when 2 required
// problem found by D.Jeans, fix provided by R.Poeschl
// #define MASK_type  (unsigned int) 0x00000040
// #define MASK_ID    (unsigned int) 0x00007F80
// #define MASK_cell  (unsigned int) 0x3FFF8000
// #define MASK_bad   (unsigned int) 0x40000000
#define MASK_type  (unsigned int) 0x000000C0
#define MASK_ID    (unsigned int) 0x0000FF00
#define MASK_cell  (unsigned int) 0x7FFF0000
#define MASK_bad   (unsigned int) 0x80000000 


#define MAX_M 8
#define MAX_S 8
#define MAX_I 512
#define MAX_J 512
#define MAX_K 64
#define MAX_index 64
// 2014/8/1: wrongly had only 1 bit for "type" field, when 2 required
// problem found by D.Jeans, fix provided by R.Poeschl
//#define MAX_type 2
#define MAX_type 4
#define MAX_ID 256
#define MAX_cell 32768
#define MAX_bad 2

#define SHIFT_WAFER_ROW SHIFT_M
#define MASK_WAFER_ROW  MASK_M
#define MAX_WAFER_ROW   MAX_M

#define SHIFT_WAFER_COL SHIFT_S
#define MASK_WAFER_COL  MASK_S
#define MAX_WAFER_COL   MAX_S

#define SHIFT_PAD_COL SHIFT_I
#define MASK_PAD_COL  MASK_I
#define MAX_PAD_COL   MAX_I

#define SHIFT_PAD_ROW SHIFT_J
#define MASK_PAD_ROW  MASK_J
#define MAX_PAD_ROW   MAX_J

#define SHIFT_LAYER SHIFT_K
#define MASK_LAYER  MASK_K
#define MAX_LAYER_TB   MAX_K


  /** The Mokka conform cell index.
   * The cell index encodes the wafer column, and row, pad column and row, and the layer
   * into one index. The wafer 0,0 is in the lower right corner (following a right handed coordinate
   * frame ) looking in beam direction at
   * the detector fron plate. Similar the pad 0,0 of each wafer is located in the lower right
   * corner of the wafer. Encoding for the ECAL: 
   *<table>
   *<tr><td>M:3,<td> wafer row</tr>
   *<tr><td>S-1:3,<td> wafer column</tr>
   *<tr><td>I:9,<td>pad coloumn</tr>
   *<tr><td>J:9,<td>pad row</tr>
   *<tr><td>K:6<td>layer</tr>
   *</table>
   */
  class CellIndex
  {
  public:

    CellIndex (UInt_t geometrical_cell_index) : _index(geometrical_cell_index), _second(0) {};
    CellIndex (UInt_t geometrical_cell_index, UInt_t second_cell_index) : 
      _index(geometrical_cell_index),
      _second(second_cell_index) {};

    CellIndex (UInt_t wafer_row    /*M*/, 
	       UInt_t wafer_column /*S*/, 
	       UInt_t pad_row      /*J*/, 
	       UInt_t pad_column   /*I*/, 
	       UInt_t layer        /*K*/)
    { 
#ifdef BOUNDARY_CHECK
      assert(wafer_row<MAX_WAFER_ROW);
      assert(wafer_column<MAX_WAFER_COL);
      assert(pad_row<MAX_PAD_ROW);
      assert(pad_column<MAX_PAD_COL);
      assert(layer<MAX_LAYER_TB);
#endif
//      std::cout << "wafer row: " << (wafer_row) << std::endl;
//      std::cout << "wafer column: " << (wafer_column) << std::endl;
//      std::cout << "pad row: " << (pad_row) << std::endl;
//      std::cout << "pad column: " << (pad_column) << std::endl;
//      std::cout << "layer: " << layer << std::endl;

     _index=(
	     ( (wafer_row) << SHIFT_WAFER_ROW) |
	     ( (wafer_column-1) << SHIFT_WAFER_COL) |
	     ( (pad_row) << SHIFT_PAD_ROW) |
	     ( (pad_column) << SHIFT_PAD_COL) |
	     ( (layer-1) << SHIFT_LAYER));
//     std::cout << "Index: " << std::hex << _index << std::dec << std::endl;

     _second=0;
    };
    CellIndex (UInt_t wafer_row    /*M*/, 
	       UInt_t wafer_column /*S*/, 
	       UInt_t pad_row      /*I*/, 
	       UInt_t pad_column   /*J*/, 
	       UInt_t layer        /*K*/,
	       UInt_t module_index,
	       UInt_t module_type,
	       UInt_t module_ID,
	       UInt_t cell_id,
	       UInt_t isBad)
    { 
#ifdef BOUNDARY_CHECK
      assert(wafer_row<MAX_WAFER_ROW);
      assert(wafer_column<MAX_WAFER_COL);
      assert(pad_row<MAX_PAD_ROW);
      assert(pad_column<MAX_PAD_COL);
      assert(layer<MAX_LAYER_TB);
      assert(module_index<MAX_index);
      assert(module_type<MAX_type);
      assert(module_ID<MAX_ID);
      assert(cell_id<MAX_cell);
      assert(isBad<MAX_bad);
#endif
     _index=(
	     ( (wafer_row) << SHIFT_WAFER_ROW) |
	     ( (wafer_column-1) << SHIFT_WAFER_COL) |
	     ( (pad_row) << SHIFT_PAD_ROW) |
	     ( (pad_column) << SHIFT_PAD_COL) |
	     ( (layer-1) << SHIFT_LAYER));

     _second=( (module_index << SHIFT_index) | 
	       (module_type  << SHIFT_type ) |
	       (module_ID    << SHIFT_ID   ) |
	       (cell_id      << SHIFT_cell ) |
	       (isBad        << SHIFT_bad  ) );

    };

     CellIndex (UInt_t geometrical_cell_index,UInt_t module_index,
		UInt_t module_type,UInt_t module_ID,UInt_t cell_id,
		UInt_t isBad) : _index(geometrical_cell_index){

       _second= ( (module_index << SHIFT_index) | 
		  (module_type  << SHIFT_type ) |
		  (module_ID    << SHIFT_ID   ) |
		  (cell_id      << SHIFT_cell ) |
		  (isBad        << SHIFT_bad  ) );

     };
   
    /** Get the layer index (index labeled K in Mokka)
     */
    UInt_t getLayerIndex() const {
      return (((_index & MASK_LAYER) >> SHIFT_LAYER) + 1);
    };
    
    /** Set the layer index (index labeled K in Mokka)
     */
    CellIndex &setLayerIndex(UInt_t layer /*K*/) 
    {
#ifdef BOUNDARY_CHECK
      assert(layer<MAX_LAYER_TB);
#endif
      _index = (_index & (~MASK_LAYER)) | ((layer-1) << SHIFT_LAYER);
      return *this;
    };

    /** Get the wafer row (index labeled M in Mokka)
     */
    UInt_t getWaferRow() const {
      return ((_index & MASK_WAFER_ROW) >> SHIFT_WAFER_ROW);
    };

    /** Set the wafer row (index labeled M in Mokka)
     */
    CellIndex &setWaferRow(UInt_t wafer_row /*M*/) {
#ifdef BOUNDARY_CHECK
      assert(wafer_row<MAX_WAFER_ROW);
#endif    
      _index = (_index & (~MASK_WAFER_ROW)) | (wafer_row << SHIFT_WAFER_ROW);
     return *this;
  };

    /** Get the wafer column (index labeled S in Mokka)
     */
    UInt_t getWaferColumn() const {
      return (((_index & MASK_WAFER_COL) >> SHIFT_WAFER_COL) + 1);
    };

    /** Set the wafer column (index labeled S in Mokka)
     */
    CellIndex &setWaferColumn(UInt_t wafer_column /*S*/) {
#ifdef BOUNDARY_CHECK
      assert(wafer_column<MAX_WAFER_COL);
#endif    
      _index = (_index & (~MASK_WAFER_COL)) | ((wafer_column-1) << SHIFT_WAFER_COL);
     return *this;
    };

    /** Get the pad row (index labeled J in Mokka)
     */
    UInt_t getPadRow() const {
      return ((_index & MASK_PAD_ROW) >> SHIFT_PAD_ROW);
    };

    /** Set the pad row (index labeled J in Mokka)
     */
    CellIndex &setPadRow(UInt_t pad_row /*J*/) {
#ifdef BOUNDARY_CHECK
      assert(pad_row<MAX_PAD_ROW);
#endif    
     _index = (_index & (~MASK_PAD_ROW)) | (pad_row << SHIFT_PAD_ROW);
     return *this;
    };

    /** Get the pad column (index labeled I in Mokka)
     */
    UInt_t getPadColumn() const {
      return ((_index & MASK_PAD_COL) >> SHIFT_PAD_COL);
    };

    /** Set the pad column (index labeled I in Mokka)
     */
    CellIndex &setPadColumn(UInt_t pad_column /*I*/) 
    {
#ifdef BOUNDARY_CHECK
      assert(pad_column<MAX_PAD_COL);
#endif    
     _index = (_index & (~MASK_PAD_COL)) | (pad_column << SHIFT_PAD_COL);
     return *this;
    };


    UInt_t getModuleIndex() const {
      return ((_second & MASK_index) >> SHIFT_index);
    };

    CellIndex &setModuleIndex(UInt_t module_index){
      #ifdef BOUNDARY_CHECK
      assert(module_index<MAX_index);
#endif    
     _second = (_second & (~MASK_index)) | (module_index << SHIFT_index);
     return *this;
    };

    UInt_t getModuleType() const {
      return ((_second & MASK_type) >> SHIFT_type);
    };

    CellIndex &setModuleType(UInt_t module_type){
      #ifdef BOUNDARY_CHECK
      assert(module_type<MAX_type);
#endif    
     _second = (_second & (~MASK_type)) | (module_type << SHIFT_type);
     return *this;
    };

    UInt_t getModuleID() const {
      return ((_second & MASK_ID) >> SHIFT_ID);
    };

    CellIndex &setModuleID(UInt_t module_ID){
      #ifdef BOUNDARY_CHECK
      assert(module_ID<MAX_ID);
#endif    
     _second = (_second & (~MASK_ID)) | (module_ID << SHIFT_ID);
     return *this;
    };

    UInt_t getCellID() const {
      return ((_second & MASK_cell) >> SHIFT_cell);
    };

    CellIndex &setCellID(UInt_t cell_index){
      #ifdef BOUNDARY_CHECK
      assert(cell_index<MAX_cell);
#endif    
      _second = (_second & (~MASK_cell)) | (cell_index << SHIFT_cell);
     return *this;
    };

    UInt_t isBad() const {
      return ((_second & MASK_bad) >> SHIFT_bad);
    };

    CellIndex &setBad(UInt_t is_bad){
      #ifdef BOUNDARY_CHECK
      assert(is_bad<MAX_bad);
#endif    
      _second = (_second & (~MASK_bad)) | (is_bad << SHIFT_bad);
     return *this;
    };

    /** Get the Mokka conform cell index.
     */
    UInt_t getCellIndex() const {
      return _index;
    };

    /** Set the Mokka conform cell index.
     */
    CellIndex &setCellIndex(UInt_t mokka_cell_index) {
      _index=mokka_cell_index;
      return *this;
    }
    /** Get the 2nd cell index.
     */
    UInt_t getSecondIndex() const {
      return _second;
    };

    /** Set the 2nd cell index.
     */
    CellIndex &setSecondIndex(UInt_t cellID1) {
      _second=cellID1;
      return *this;
    }
      
  private:
    UInt_t _index;
    UInt_t _second;
  };

#undef SHIFT_WAFER_ROW
#undef SHIFT_M
#undef MASK_WAFER_ROW
#undef MASK_M
#undef MAX_WAFER_ROW
#undef MAX_M

#undef SHIFT_WAFER_COL
#undef SHIFT_S
#undef MASK_WAFER_COL
#undef MASK_S
#undef MAX_WAFER_COL
#undef MAX_S

#undef SHIFT_PAD_COL
#undef SHIFT_I
#undef MASK_PAD_COL
#undef MASK_I
#undef MAX_PAD_COL
#undef MAX_I

#undef SHIFT_PAD_ROW
#undef SHIFT_J
#undef MASK_PAD_ROW
#undef MASK_J
#undef MAX_PAD_ROW
#undef MAX_J

#undef SHIFT_index
#undef MASK_index
#undef MAX_index

#undef SHIFT_type
#undef MASK_type
#undef MAX_type

#undef SHIFT_ID
#undef MASK_ID
#undef MAX_ID

#undef SHIFT_cell
#undef MASK_cell
#undef MAX_cell

#undef SHIFT_bad
#undef MASK_bad
#undef MAX_bad

}
#endif
