#ifndef CorrectPositionProcessor_hh
#define CorrectPositionProcessor_hh 1

/* LCIO includes*/
#include "lcio.h"

/* Marlin includes*/
#include "marlin/Processor.h"
#include "IMPL/CalorimeterHitImpl.h"

/* c++ includes*/
#include <string>

/* CALICE includes*/
#include "MappedContainer.hh"
#include "CellDescription.hh"
#include "Mapper.hh"

using namespace marlin;
using namespace lcio;

namespace CALICE {

  /**
   * Processor that generates a collection of CalorimeterHits with corrected position.
   *
   * @par processor parameters
   * <table>
   * <tr><td> steering file parameter name </td><td> description </td></tr>
   * <tr><td><b><em> CellDescriptionProcessorName </em></b></td><td> name of the CellDescriptionProcessor that provides position information</td></tr>
   * <tr><td><b><em> InputCollection </em></b></td><td> name of the collection that contains the CalorimeterHits to be corrected</td></tr>
   * <tr><td><b><em> OutputCollection </em></b></td><td> name of the collection that will contain the corrected CalorimeterHits</td></tr>
   * <tr><td><b><em> ScaleEnergy </em></b></td><td> scale factor for the energy of the hits</td></tr>
   * </table>
   *
   * @author Benjamin.Lutz@desy.de
   * @version 0.1
   * @date June 2009
   */
  class CorrectPositionProcessor : public Processor {

  public:
    CorrectPositionProcessor();

    virtual Processor*  newProcessor() { return new CorrectPositionProcessor ; }

    /** Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /** Called for every event.
     */
    virtual void processEvent( LCEvent * evt ) ;

    
  protected:
    /**Create a collection of type LCRelation (relation between Mokka SimCalorimeterHits
       and reconstructed CalorimeterHits), if the processor parameter  CreateSimRecRelation is true
     */
    void createSimRecRelation(LCEvent *evt);
     /**Check the LCRelation between Mokka SimCalorimeterHits
       and reconstructed CalorimeterHits
     */
    virtual void checkSimRecRelation(LCEvent *evt);



  private:
    std::string _inputCollectionName;          /**<name of the input collection (calibrated hits, but with no position)*/
    std::string _outputCollectionName;         /**<name of the output collection (calibrated hits, with correct position)*/

    std::string _cellDescriptionProcessorName;           /**<name of the processor which provides the cells description*/

    bool _scaleEnergy;                                   /**<flag to enable/disable the scaling of the hits energy*/
    float _energyScaleFactor;                            /**<factor to scale the hits energy*/

    MappedContainer<CellDescription> *_cellDescriptions; /**<mapped container of cells description*/

    std::string _simHitColName;          /**<name of collection containing SimCalorimeterHits*/
    std::string _simRecRelationColName;  /**<name of the relation between Mokka SimCalorimeterHits 
					    and reconstructed CalorimeterHits*/
    bool _createSimRecRelation;          /**<flag to disable/enable the creation of an LCRelation between 
					    the Mokka SimCalorimeterHits and the reconstructed CalorimeterHits*/
    MappedContainer<CalorimeterHitImpl>  *_recoContainer;     /**<mapped container of ganged AHCAL cells*/
    const Mapper *_mapper;                                    /**<pointer to the used mapped*/
    std::string _mappingProcessorName;                        /**<name of the mapping processor name*/

  };

} /* end namespace CALICE*/

#endif
