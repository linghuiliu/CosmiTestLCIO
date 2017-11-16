#ifndef ModuleDescription_hh
#define ModuleDescription_hh

#include "lcio.h"
#include "IMPL/LCGenericObjectImpl.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <climits>
#include <stdexcept>
#include "stringFromToInts.hh"

#include <RtypesSubSet.h>
#include <cloneUtils.hh>

using namespace lcio;
namespace CALICE {


  enum EModuleDescriptionIntType {kModuleDescriptionIntModuleType, 
				  kModuleDescriptionIntNameIndexOrMinusOne,
				  kModuleDescriptionIntNameLength,
				  kNModuleDescriptionIntsOrNCells,
				  kModuleDescriptionIntNameIndex,
				  kModuleDescriptionIntCellDimensionStart,
				  kModuleDescriptionIntCellZPositionStart,
				  kNModuleDescriptionInts};

  enum EModuleDescriptionFloatType {kModuleDescriptionWidth, kModuleDescriptionHeight,  
				    kModuleDescriptionCellWidth, kModuleDescriptionCellHeight, 
				    kNModuleDescriptionFloats};

  enum EModuleDescriptionOptions {kModuleDescriptionNormal=0, kModuleDescriptionWithCellDimension=1, kModuleDescriptionWithCellDimensionAndZPos=3};

  /** Description of Calice (ECAL) detector modules.
   * Based on LCFixedObject.
   * @author Goetz Gayckem, LLR - ecole polytechnique)
   * @version $Id: ModuleDescription.hh,v 1.7 2009-07-02 13:07:56 meyern Exp $
   */
  class ModuleDescription : public lcio::LCGenericObject {
    
  public: 
    /** Default c'tor.
     */
    ModuleDescription():
      _createdObject( true ),
      _nCells(0),
      _cellIndexStart(0),
      _cellDimensionStart(0),
      _cellZPosStart(0)
    {
      // create a generic object of variable size
      _obj = new LCGenericObjectImpl;
      _obj->setIntVal(kModuleDescriptionIntModuleType,UCHAR_MAX);
    }

    ModuleDescription(const ModuleDescription &a) {
      //      std::cout <<  "  ModuleDescription::c tor> " << std::hex << static_cast<void *>(this)
      //		<< " <- " << static_cast<void *>(a._obj) << std::dec << (a._createdObject ? " owner" : "" ) <<std::endl;
      _obj=0;
      if (a._obj && a._createdObject) {
	_obj=dynamic_cast<lcio::LCGenericObjectImpl *>(cloneLCGenericObject(a._obj));
	_createdObject=true;
	assert ( _obj );
      }
      else {
	_createdObject=false;
	_obj=a._obj;
      }

      _nCells=a._nCells;
      _cellIndexStart=a._cellIndexStart;
      _cellDimensionStart=a._cellDimensionStart;
      _cellZPosStart=a._cellZPosStart;

      //init();
      //      std::cout << "      " << std::hex << static_cast<void *>(this)  << " : " << static_cast<void *>(_obj) << std::dec << (_createdObject ? " owner" : "") <<std::endl;
    }


    /** UsefulConstructor
     */
    ModuleDescription(UChar_t module_type, UInt_t n_cells, const std::string &name, EModuleDescriptionOptions options=kModuleDescriptionNormal)
      : _createdObject( true ),
	_nCells(n_cells),
	_cellIndexStart(0),
	_cellDimensionStart(0),
	_cellZPosStart(0)
    {
      // one for the length of the string
      UInt_t n_string_buffer_size=getNeededInts(name);
      // created generic object 
      _obj = new LCGenericObjectImpl;
	// enforce an allocation of the int and float array;

      UInt_t name_start = 0;
      switch (options) {
      case kModuleDescriptionNormal: {
	// hack to reserve space: set the last needed cell to a dummy value. This will automatically resize the underlying storage to 
	// the needed size.
	_cellIndexStart=kNModuleDescriptionIntsOrNCells;
	_obj->setIntVal(_cellIndexStart +  n_cells + n_string_buffer_size -1,0);
	_obj->setIntVal(kModuleDescriptionIntModuleType,static_cast<int>(module_type));
	// In normal ModuleDescription objects only elements until  kNModuleDescriptionIntsOrNCells are needed plus one int 
	// per n_cells + buffer for the module name
	name_start = static_cast<int>(kNModuleDescriptionIntsOrNCells+n_cells);
	_obj->setIntVal(kModuleDescriptionIntNameIndexOrMinusOne,name_start);
	
	_obj->setFloatVal(kNModuleDescriptionFloats+n_cells*2 -1 , 0. );
	break;
      }
      case kModuleDescriptionWithCellDimensionAndZPos: 
	_cellZPosStart=n_cells*4+kNModuleDescriptionFloats;
	// reserve more float space (see above):
	_obj->setFloatVal(_cellZPosStart+n_cells -1 , 0. );

      case kModuleDescriptionWithCellDimension: {
	_cellIndexStart=kNModuleDescriptionInts;
	_cellDimensionStart=n_cells*2+kNModuleDescriptionFloats;
	// reserve space (see above):
	_obj->setIntVal( _cellIndexStart + n_cells + n_string_buffer_size -1 ,0);
	// reserve float space (see above):
	_obj->setFloatVal(_cellDimensionStart+n_cells*2 -1 , 0. );

	_obj->setIntVal(kModuleDescriptionIntModuleType,static_cast<int>(module_type));
	_obj->setIntVal(kNModuleDescriptionIntsOrNCells,n_cells);

	// set marker which indicates extended module description objects:
	_obj->setIntVal(kModuleDescriptionIntNameIndexOrMinusOne,-1);
	name_start = static_cast<int>(kNModuleDescriptionInts+n_cells);
	_obj->setIntVal(kModuleDescriptionIntNameIndex,name_start);

	_obj->setIntVal(kModuleDescriptionIntCellDimensionStart,_cellDimensionStart);
	_obj->setIntVal(kModuleDescriptionIntCellZPositionStart,_cellZPosStart);

	break;
      }
      }

      convertStringToInts(_obj, name, name_start, kModuleDescriptionIntNameLength);
    }

    ModuleDescription &setModuleType(UChar_t type) {
      _obj->setIntVal(kModuleDescriptionIntModuleType,(int) type);
      return *this;
    }

    UChar_t getModuleType() const
    { 
      return (UChar_t) _obj->getIntVal(kModuleDescriptionIntModuleType);
    }

    /** return if the ModuleDescription object is defined.
     */
    bool isValid() const {
      return getModuleType() < UCHAR_MAX;
    }


    /** Get the name assigned to the module type.
     * The module type number is generally considered to be unique. It should not be
     * necessary to query the name and map type numbers to module names.
     * Due to the character encoding in an integer array, the function may perform slowly.
     */
    std::string getModuleTypeName() const 
    {
      int name_start = _obj->getIntVal(kModuleDescriptionIntNameIndexOrMinusOne);
      if (name_start < 0 ) {
	name_start = _obj->getIntVal(kModuleDescriptionIntNameIndex);
      }
      return getStringFromInts(_obj, static_cast<unsigned int>(name_start) , kModuleDescriptionIntNameLength);
    }

    ModuleDescription &setCellXPos(UInt_t cell_index, float x_pos) {
#ifdef BOUNDARY_CHECK
      assert(cell_index < getNCells());
#endif
      _obj->setFloatVal(kNModuleDescriptionFloats+(cell_index<<1),x_pos);
      return *this;
    }

    ModuleDescription &setCellYPos(UInt_t cell_index, float y_pos) {
#ifdef BOUNDARY_CHECK
      assert(cell_index < getNCells());
#endif
      _obj->setFloatVal(kNModuleDescriptionFloats+(cell_index<<1)+1,y_pos);
      return *this;
    }

    ModuleDescription &setCellPos(UInt_t cell_index, float x_pos, float y_pos) {
#ifdef BOUNDARY_CHECK
      assert(cell_index < getNCells());
#endif
      _obj->setFloatVal(kNModuleDescriptionFloats+(cell_index<<1),x_pos);
      _obj->setFloatVal(kNModuleDescriptionFloats+(cell_index<<1)+1,y_pos);
      return *this;
    }

    /** Set the cell z-position.
     * The result of this call is only defined if @ref hasCellZPos returns true.
     */
    ModuleDescription &setCellZPos(UInt_t cell_index, float z_pos) {
#ifdef BOUNDARY_CHECK
      assert(_cellZPosStart>0);
      assert(cell_index < getNCells());
#endif
      _obj->setFloatVal(_cellZPosStart+cell_index,z_pos);
      return *this;
    }

    Float_t getCellXPos(UInt_t cell_index) const
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if ((cell_index<<1)>(UInt_t) getNFloat()) {
	throw std::range_error("ModuleDescription::getYPos> Cell index out of bounds!");
      }
#endif
      return _obj->getFloatVal(kNModuleDescriptionFloats+(cell_index<<1));
    }

    Float_t getCellYPos(UInt_t cell_index) const
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      if ((cell_index<<1)>(UInt_t) getNFloat()) {
	throw std::range_error("ModuleDescription::getYPos> Cell index out of bounds!");
      }
#endif
      return _obj->getFloatVal(kNModuleDescriptionFloats+(cell_index<<1)+1);
    }

    /** Get the z position offset of an individual cell.
     * The result of this call is only defined if @ref hasCellZPos returns true.
     */
    Float_t getCellZPos(UInt_t cell_index) const
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif
    {
#ifdef BOUNDARY_CHECK
      assert( _cellZPosStart>0 );
      if (cell_index > getNCells()) {
	throw std::range_error("ModuleDescription::getYPos> Cell index out of bounds!");
      }
#endif
      return _obj->getFloatVal(_cellZPosStart+cell_index);
    }
    
    UInt_t getNCells() const 
    //#ifdef BOUNDARY_CHECK
    //      throw (std::logic_error)
    //#endif
    {
      return _nCells;
      //#ifdef BOUNDARY_CHECK
      //      if ((getNFloat()-kNModuleDescriptionFloats)&1 !=0) {
      //	throw std::logic_error("Remaining number of floats of ModuleDescription object is not even (but should be for x/y position pairs of the cells).");
      //      }
      //#endif
      return (getNFloat()-kNModuleDescriptionFloats)/2;
    }

    /** Return true if there are cell dimensions (width and height) per cell.
     * if there are no cell dimensions per cell the result of the calls @ref getIndividualCellWidth
     * and @ref getIndividualCellHeight will be undefined (most likely this will cause a segmentation
     * violation). If there are cell dimensions per cell then the calls getCellWidth() and getCellHeight
     * will cause an exception.
     */
    bool hasCellDimensionsPerCell() const {
      return _cellDimensionStart>0;
    }

    /** Return true if the cells have also a z-position offset.
     * If the cells do not have a z-position offset then the result of the call getZPos will be undefined
     * (i.e. most likely will cause a segmentation vaiolation).
     */
    bool hasCellZPos() const {
      return _cellZPosStart>0;
    }

    ModuleDescription &setCellWidth(float cell_width) {
      if (_cellDimensionStart>0) { 
	throw std::logic_error("ModuleDescription::setCellWidth> The module contains dimensions per cell. You must not specify a global cell width.");
      }
      _obj->setFloatVal(kModuleDescriptionCellWidth,cell_width);
      return *this;
    }

    float getCellWidth() const {
      if (_cellDimensionStart>0) { 
	throw std::logic_error("ModuleDescription::getCellWidth> The module contains dimensions per cell. You cannot get a global cell width.");
      }
      return _obj->getFloatVal(kModuleDescriptionCellWidth);
    }

    ModuleDescription &setCellHeight(float cell_height) {
      if (_cellDimensionStart>0) { 
	throw std::logic_error("ModuleDescription::setCellHeight> The module contains dimensions per cell. You must not specify a global cell height.");
      }
      _obj->setFloatVal(kModuleDescriptionCellHeight,cell_height);
      return *this;
    }

    float getCellHeight() const {
      if (_cellDimensionStart>0) { 
	throw std::logic_error("ModuleDescription::getCellHeight> The module contains dimensions per cell. You cannot get a global cell height.");
      }
      return _obj->getFloatVal(kModuleDescriptionCellHeight);
    };

    /** Set the indivual cell width.
     * The result of the call will be undefined if @ref hasCellDimensionsPerCell is not true.
     */
    ModuleDescription &setIndividualCellWidth(UInt_t cell_index, float cell_width) {
      if (_cellDimensionStart==0) { 
	throw std::logic_error("ModuleDescription::setIndividualCellWidth> All are expected to have the same dimensions. You may not specify individual cell dimensions.");
      }
#ifdef BOUNDARY_CHECK
      assert(cell_index < getNCells());
#endif
      _obj->setFloatVal(_cellDimensionStart+cell_index*2,cell_width);
      return *this;
    }

    /** Return the width of a specific cell.
     * This call only is defined if @ref hasCellDimensionsPerCell returns true.
     */
    float getIndividualCellWidth(UInt_t cell_index) const {
#ifdef BOUNDARY_CHECK
      assert( _cellDimensionStart>0 );
      assert( cell_index < getNCells() );
#endif
      return _obj->getFloatVal(_cellDimensionStart+cell_index*2);
    }

    /** Set the indivual cell height.
     * The result of the call will be undefined if @ref hasCellDimensionsPerCell is not true.
     */
    ModuleDescription &setIndividualCellHeight(UInt_t cell_index, float cell_height) {
      if (_cellDimensionStart==0) { 
	throw std::logic_error("ModuleDescription::setIndividualCellHeighth> All are expected to have the same dimensions. You may not specify individual cell dimensions.");
      }
#ifdef BOUNDARY_CHECK
      assert(cell_index < getNCells());
#endif
      _obj->setFloatVal(_cellDimensionStart+cell_index*2+1,cell_height);
      return *this;
    }

    /** Return the height of a specific cell.
     * This call only is defined if @ref hasCellDimensionsPerCell returns true.
     */
    float getIndividualCellHeight(UInt_t cell_index) const {
#ifdef BOUNDARY_CHECK
      assert( _cellDimensionStart>0 );
      assert( cell_index < getNCells() );
#endif
      return _obj->getFloatVal(_cellDimensionStart+cell_index*2+1);
    }

    ModuleDescription &setWidth(float width) {
      _obj->setFloatVal(kModuleDescriptionWidth,width);
      return *this;
    }

    float getWidth() const {
      return _obj->getFloatVal(kModuleDescriptionWidth);
    }

    ModuleDescription &setHeight(float height) {
      _obj->setFloatVal(kModuleDescriptionHeight,height);
      return *this;
    }

    float getHeight() const {
      return _obj->getFloatVal(kModuleDescriptionHeight);
    }

    /** Set the geometrical cell index of the specified cell.
     * @param cell_index the cell index (zero to n) which reflects the ADC read order: line zero to line 12 
     *        (6 for half equipped lines) of the first sample (each line is connected to one readout chip), 
     *        then the next sample which is multiplexed on the same 12(6 for half equipped modules) lines. 
     *        In total 18 samples are read i.e. the index runs from 0 to 215 or from 0 to 107.
     * @param geometrical_cell_index the Mokka conform geometrical cell index.
     * The geometrical cell index (should) respects the Mokka notation. It contains the layer index, the wafer
     * row and column and the the pad row and column. The indices stored in the ModuleDescription object only 
     * contain the wafer row, column, pad row and column. The layer index, and on offset to the wafer column 
     * and row must be added to form an index in the notation which is conforming to Mokka.
     * @todo I did not find a more clever way than putting this index into the database.
     */
    ModuleDescription &setGeometricalCellIndex(UInt_t cell_index, UInt_t geometrical_cell_index)
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif      
    {
#ifdef BOUNDARY_CHECK
      if (cell_index>=(UInt_t) getNCells()) {
	throw std::range_error("ModuleDescription::setGeometricalCellIndex> Cell index out of bounds!");
      }
#endif
      _obj->setIntVal(_cellIndexStart+cell_index,static_cast<int>(geometrical_cell_index));
      return *this;
    }

    /** Get the geometrical cell index of the specified cell which respects the Mokka notation.
   * The geometrical cell index conforms to the Mokka notation. It contains the layer index, the wafer
   * row and column and the the pad row and column. The indices stored in the ModuleDescription object only 
   * contain the wafer row, column, pad row and column. The layer index, and on offset to the wafer column 
   * and row must be added to form an index in the notation which is Mokka conform.
   * @todo I did not find a more clever way than putting this index into the database.
   */
    UInt_t getGeometricalCellIndex(UInt_t cell_index) const
#ifdef BOUNDARY_CHECK
      throw (std::range_error)
#endif      
    {
#ifdef BOUNDARY_CHECK
      if (cell_index>(UInt_t) getNCells()) {
	throw std::range_error("ModuleDescription::getGeometricalCellIndex> Cell index out of bounds!");
      }
#endif
      return _obj->getIntVal(_cellIndexStart+cell_index);
    }

    /** Return the type of the class 
     */
    const std::string getTypeName() const { 
      return std::string("ModuleDescription");
    }
    
    /** Return a brief description of the data members 
     */
    const std::string getDataDescription() const {
      std::stringstream message;
      message << "i[1]:type, f[" << getNFloat()/2 << "]: (x,y)";
      return message.str();
    }
    
    /** C'tor to be used for elements of LCGenericObjects read from
     * an LCIO file or the database. Subclasses should 'override' this, e.g.:<br>
     * Myclass(LCObject* obj) : ModuleDescription(obj) {} <br>
     */
    ModuleDescription(LCObject* obj) : 
      _createdObject(false)  {
      
      _obj = dynamic_cast<LCGenericObjectImpl*>( obj )  ;
      
      if( _obj==0 ){

	// could be an instance of ModuleDescription !?
	ModuleDescription* f = 
	  dynamic_cast< ModuleDescription* >( obj ) ;

	if( f != 0 )
	  _obj = f->obj() ;

      }

      // do some sanity checks ...
      if( _obj==0 ){
	throw Exception("Cannot create ModuleDescription from sth."
			" that is not LCGenericObjectImpl" ) ;
      }
      
      UInt_t n_int=_obj->getNInt();
      UInt_t n_float=_obj->getNFloat();

      
      UInt_t name_start;
      if(  ( n_int    < kNModuleDescriptionIntsOrNCells )   || 
	   ( n_float  < kNModuleDescriptionFloats)  ||
	   ( _obj->getNDouble() != 0 )   ) {
	throw Exception("ModuleDescription(LCObject* obj): Wrong number of elements in object" ) ;
      }
      if (_obj->getIntVal(kModuleDescriptionIntNameIndexOrMinusOne)==-1) {
	if (n_int < kNModuleDescriptionInts) {
	  throw Exception("ModuleDescription(LCObject* obj): Wrong number of int elements in object" ) ;
	}
	_nCells=_obj->getIntVal(kNModuleDescriptionIntsOrNCells);
	_cellIndexStart=kNModuleDescriptionInts;
	_cellDimensionStart=_obj->getIntVal(kModuleDescriptionIntCellDimensionStart);
	_cellZPosStart=_obj->getIntVal(kModuleDescriptionIntCellZPositionStart);
	name_start=static_cast<UInt_t>( _obj->getIntVal(kModuleDescriptionIntNameIndex) );

	UInt_t n_exp_floats=kNModuleDescriptionFloats;
	n_exp_floats+=_nCells*2;
	if (_cellDimensionStart > 0 ) {
	  n_exp_floats+=_nCells*2;
	}
	if (_cellZPosStart>0) {
	  n_exp_floats+=_nCells;
	}
	if (n_exp_floats!= n_float) {
	  throw Exception("ModuleDescription(LCObject* obj): Wrong number of int elements in object" ) ;
	}
      }
      else {
	name_start=static_cast<UInt_t>( _obj->getIntVal(kModuleDescriptionIntNameIndexOrMinusOne) );

	if  ( n_float < kNModuleDescriptionFloats ) {
	  throw Exception("ModuleDescription::init> Number of floats smaller than expected for a ModuleDescription object. Maybe this is a different object type?");
	}
	n_float -= kNModuleDescriptionFloats;
	if (n_float & 1) {
	  throw Exception("ModuleDescription::init> Number of cells is not an integer. Maybe this is not a ModuleDescription object?");
	}
	_nCells=n_float/2;
	_cellIndexStart=kNModuleDescriptionIntsOrNCells;
	_cellDimensionStart=0;
	_cellZPosStart=0;
      }
      
      UInt_t name_ints=getNeededInts(_obj->getIntVal(kModuleDescriptionIntNameLength));
      if ( name_start + name_ints != static_cast<UInt_t>( _obj->getNInt()) ) {
	throw Exception("ModuleDescription(LCObject* obj): Start index and length of module type name does not match the arraysize. "
			"Presumably, database and code versions do not match or wrong collection name/folder name.") ;
      }
      if (static_cast<UInt_t>(name_start-_cellIndexStart!=_nCells)) {
	throw Exception("ModuleDescription(LCObject* obj): The number of geometrical indices does not coincide with the number of cells. "
			"Presumably, database and code versions do not match or wrong collection name/folder name.") ;
      }

    }

    /** The LCGenericObjectImpl . Sublcasses use this to access their data.
     */
    LCGenericObjectImpl* obj() { return _obj ; }


    /** Clean up if we created a new LCGenericObjectImpl 
     */
    virtual ~ModuleDescription() { 
      //      std::cout << "  ModuleDescription::dtor> " << std::hex << static_cast<void *>(this)  << " : " << static_cast<void *>(_obj) << std::dec 
      //		<< (_createdObject ? " owner" : "" ) <<std::endl;
      if( _createdObject )  delete _obj ; 
    }
    

    /** Return the id of the underlying LCGenericObjectImpl */
    virtual int id() { return _obj->id() ; }
     

    // ---- need to implement LCGenericObject interface:
    
    int getNInt() const    { return _obj->getNInt() ; };
    int getNFloat() const  { return _obj->getNFloat() ; };
    int getNDouble() const { return 0 ; }; 
    
    int getIntVal(int index) const {
      return _obj->getIntVal( index ) ;
    };
    float getFloatVal(int index) const { 
      return _obj->getFloatVal( index ) ;
    };
    double getDoubleVal(int index) const {
      return  _obj->getDoubleVal( index ) ;
    };
    bool isFixedSize() const { return false ; };
    
    // ---- end of LCGenericObject interface
    
  protected:
    void init() throw (Exception) {
      // extended format ? 
      if (_obj->getIntVal(kModuleDescriptionIntNameIndexOrMinusOne)==-1) {
	_nCells=_obj->getIntVal(kNModuleDescriptionIntsOrNCells);
	_cellDimensionStart=_obj->getIntVal(kModuleDescriptionIntCellDimensionStart);
	_cellZPosStart=_obj->getIntVal(kModuleDescriptionIntCellZPositionStart);
      }
      else {
	_cellDimensionStart=0;
	_cellZPosStart=0;
	unsigned int n_float=getNFloat();
	if  ( n_float < kNModuleDescriptionFloats ) {
	  throw Exception("ModuleDescription::init> Number of floats smaller than expected for a ModuleDescription object. Maybe this is a different object type?");
	}
	n_float -= kNModuleDescriptionFloats;
	if (n_float & 1) {
	  throw Exception("ModuleDescription::init> Number of cells is not an integer. Maybe this is not a ModuleDescription object?");
	}
	_nCells=n_float/2;
      }
    }
    
    LCGenericObjectImpl* _obj ;
    bool _createdObject ;
    unsigned int _nCells;
    unsigned int _cellIndexStart;
    unsigned int _cellDimensionStart;
    unsigned int _cellZPosStart;
  }; 
  
} // namespace
#endif /* ifndef LCIO_LCFIXEDOBJECT_H */
