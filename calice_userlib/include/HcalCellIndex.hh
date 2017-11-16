#ifndef _HcalCellIndex_hh_
#define _HcalCellIndex_hh_

#include <CellIndex.hh>
#include <sstream>

#include "EncodingStringHelper.hh"

#define SHIFT_I 6
#define SHIFT_J 15
#define SHIFT_KAI SHIFT_LAYER

#define MASK_I (unsigned int) 0x00007FC0
#define MASK_J (unsigned int) 0x00FF8000
#define MASK_KAI MASK_LAYER

#define SHIFT_PAD_ROW SHIFT_J
#define MASK_PAD_ROW  MASK_J

#define SHIFT_PAD_COL SHIFT_I
#define MASK_PAD_COL  MASK_I

namespace CALICE {
  /** like CALICE::CellIndex, but with HCAL related funciton names
   * @see CALICE::CellIndex
   */
  class HcalCellIndex: public CellIndex {
    public:
    HcalCellIndex (UInt_t pad_row      /*J*/, 
	           UInt_t pad_column   /*I*/, 
	           UInt_t layer        /*K*/)
     : CellIndex((pad_row << SHIFT_PAD_ROW) | ( pad_column << SHIFT_PAD_COL) | ( (layer-1) << SHIFT_LAYER))
    { };
    
    HcalCellIndex (UInt_t geometrical_cell_index) : CellIndex(geometrical_cell_index) {};
    

    /** Get the tile row (index labeled J in Mokka)
     */
    UInt_t getTileRow() const {
      return getPadRow();
    };


    /** Set the tile row (index labeled J in Mokka)
     */
    HcalCellIndex &setTileRow(UInt_t tile_row /*J*/) {
     setPadRow(tile_row);
     return *this;
    };


    /** Get the tile column (index labeled I in Mokka)
     */
    UInt_t getTileColumn() const {
      return getPadColumn();
    };


    /** Set the tile column (index labeled I in Mokka)
     */
    HcalCellIndex &setTileColumn(UInt_t tile_column /*I*/) 
    {
     setPadColumn(tile_column);
     return *this;
    };

    /** Return the cellid encoding string according to an lcio BitField64
     */
    static std::string getEncodingString(const unsigned int startbit) {
      
      std::ostringstream EncodingString;
    
      EncodingString << EncodingStringHelper::GetFieldDesc("I",MASK_I,SHIFT_I,startbit)
                     << "," 
                     << EncodingStringHelper::GetFieldDesc("J",MASK_J,SHIFT_J,startbit)
                     << ","
                     << EncodingStringHelper::GetFieldDesc("K-1",MASK_KAI,SHIFT_KAI,startbit);
    
      return EncodingString.str();
    
    }

  };

  inline std::ostream& operator<<(std::ostream& out, 
                         const CALICE::HcalCellIndex& hci) {

    out << "I (column): " << hci.getTileColumn() 
	<< " J (row): " << hci.getTileRow()
	<< " K (layer): " << hci.getLayerIndex()
      ;
    
    return out;

  }


}

#undef SHIFT_PAD_COL
#undef SHIFT_I
#undef MASK_PAD_COL
#undef MASK_I
#undef SHIFT_PAD_ROW
#undef SHIFT_J
#undef MASK_PAD_ROW
#undef MASK_J

#endif
