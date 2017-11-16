#ifndef __HcalImplicits_hh__
#define __HcalImplicits_hh__ 1

/**  @brief Class with magic Hcal numbers
 *
 *   This class provides static methods for implicit numbers
 *   connected to the CALICE AHcal prototype
 *
 *   @author Niels/Meyer@desy.de
 *   @date   May 2008
 */

#include "HcalCellIndex.hh"
#include "ModuleConnection.hh"

class HcalImplicits{
public:

  /**  Nominal x-position of lower left corner for modules
   */
  static const float nominalPosX(){ 
    //return -450; 
    return 0; 
  }

  /**  Nominal y-position of lower left corner for upper/lower half-modules
   */
  static const float nominalPosY( const bool upper ){ 
    //return (upper ? 0 : -450);
    return 0;
  }

  /**  Nominal z-position of lower left corner for modules
   */
  static const float nominalPosZ( const unsigned int layer, const std::string absorberFeW ){
    /*  assume: 16mm absorber, 2mm air, 2mm cover, 2.5mm cable/air/board, 
	.1mm foil, 5mm scint, .1mm foil, 2mm cover, 2mm air
	energy deposition is located half length through scintillator, layer count starts with 1
    */
    
    /*if absorber thickness = 10 mm (tungsten: CERN 2010)*/
    if ( absorberFeW == "W"){
      return 19.1 + 25.7 * (layer-1);
    }
    /*if absorber thickness = 20 mm (steel: DESY 2006, CERN 2007, FNAL 2008/2009)*/
    else if( absorberFeW == "Fe") {
      return 25.1 + 31.7 * ( layer - 1 );
    }
    else{ return 19.1 + 25.7 * (layer-1); } //default (tungsten: CERN 2010)
  }

  /**  Number of modules
   */
  static const unsigned int numModules(){
    return ( numFineModules() + numCoarseModules() );
  }

  /**  Number of fine modules
   */
  static const unsigned int numFineModules(){
    return 30;
  }

  /**  Number of coarse modules
   */
  static const unsigned int numCoarseModules(){
    return 8;
  }

  /**  Number of cells
   */
  static const unsigned int numCells( const unsigned int modID ){
    return ( modID>numFineModules() ? 141 : 216 );
  }

  /**  Number of cells in half-modules
   */
  static const unsigned int numCells( const unsigned int modID,
				      const bool upper ){
    // @todo  check 72-69 vs. 69-72
    return ( modID>numFineModules() ? ( upper ? 72 : 69 ) : 108 );
  }

  /**  Module type
   */
  static const unsigned int moduleType( const unsigned int modID,
					const bool upper ){
    /*count AHCAL module types 4, 5, 6, 7 (reserve 0-3 for Si-W ECAL, the 'initial' definition)*/
    //return ( 4 + ( modID>30 ? 2 : 0 ) + ( upper ? 0 : 1 ) ); // 

    /*count AHCAL module types 0, 1, 2, 3 (convention used for AHCAL SW for 
      compatibility with (wrong) DB entries form CERN*/
    return ( ( modID>30 ? 2 : 0 ) + ( upper ? 0 : 1 ) ); //types had different numbering scheme in connection (HcalMapping 0, 1, 2, 3);  types had the same numbering scheme in description (4, 5, 6, 7) and locationReference (4, 5, 6, 7).

  }

  /**  The Hcal crate
   */
  static const unsigned int crate(){
     return 172;
  }

  /**  Half-module identifier based on HcalCellIndex
   */
  static const unsigned int geometricalModuleIndex( const unsigned int layer,
						    const bool upper ){
    CALICE::HcalCellIndex hci( ( upper ? 0 : 1 ), ( layer>30 ? 1 : 0 ), layer );
    return hci.getCellIndex();
  }

  /**  Connector type
   */
  static const CALICE::EModuleConnectorType connectorType( const bool upper ){
    return ( upper ? CALICE::kLeftConnector : CALICE::kRightConnector );
  }

  static const char moduleTypeChar( const unsigned int modId, 
				    const bool upper ){
    return ( modId>30  
	     ? ( upper ? 'C' : 'D' )
	     : ( upper ? 'A' : 'B' ) );
  }

}; // class HcalImplicits

#endif
