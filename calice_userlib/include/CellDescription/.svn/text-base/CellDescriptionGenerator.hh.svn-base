#ifndef CellPositionsGenerator_hh
#define CellPositionsGenerator_hh 1

#include "lcio.h"
#include "EVENT/LCCollection.h"

#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"

namespace CALICE {

  /**
   * class that generates cell descriptions objects
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class CellDescriptionGenerator {

  public:

    /**
     * constructor
     *
     * @param[in] mapper mapper
     */
    CellDescriptionGenerator(const Mapper* mapper) : _mapper(mapper) { }

    /**
     * function that updates a MappedContainer of CellDescription objects
     *
     * The cell positions are calculated the following way:
     * @f[ r_{detector} + r_{module} \cdot M_{zshift} + r_{cell} \cdot M_{rot} \\
     * \quad\mathrm{where}\quad
     * M_{rot}=
     * \begin{array}{ccc}
     * cos(\theta) & 0 & sin(\theta) \\
     * 0 & 1 & 0 \\
     * -sin(\theta) & 0 & cos(\theta)
     * \end{array}
     * \quad\mathrm{and}\quad
     * M_{zshift}=
     * \begin{array}{ccc}
     * 1 & 0 & 0 \\
     * 0 & 1 & 0 \\
     * 0 & 0 & 1/cos(\theta)
     * \end{array}
     * @f]
     *
     * @param[in] modDescriptionCol
     * @param[in] modConnectionCol
     * @param[in] modLocationCol
     * @param[in] detTransformationCol
     * @param[out] container
     */
    void generate(const lcio::LCCollection* modDescriptionCol, const lcio::LCCollection* modConnectionCol, const lcio::LCCollection* modLocationCol, const lcio::LCCollection* detTransformationCol, MappedContainer<CellDescription>* container ) const;

  private:
    const Mapper* _mapper;
  };

} //end namespace CALICE



#endif
