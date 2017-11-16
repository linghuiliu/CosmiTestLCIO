#ifndef CellIterator_hh
#define CellIterator_hh

namespace CALICE {

  class Mapper;

  /**
   * iterator to run over all valid (Mokka) cell IDs
   *
   * To access the CellID use the *() operator.
   *
   * @ingroup CellIDgroup
   *
   * @author Benjamin.Lutz@desy.de
   * @date June 2009
   */
  class CellIterator {

  public:
    CellIterator(const Mapper* mapper, const unsigned int index = 0 );

    /**
     * jump to the next valid (Mokka) cell ID
     */
    void operator ++ ();

    /**
     * compare if two iterators are not equal
     */
    bool operator != (const CellIterator& other) const { return other._index != _index ; }

    /**
     * compare if two iterators are equal
     */
    bool operator == (const CellIterator& other) const { return other._index == _index ; }

    /**
     * access to the current (Mokka) cell ID
     *
     * @returns the current cell ID
     */
    int operator * () const { return _currentCellID; }

  protected:

    const Mapper* _mapper;
    unsigned int _index;
    int _currentCellID;
  };



} // end namespace CALICE

#endif
