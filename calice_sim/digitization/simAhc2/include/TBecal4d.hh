//ECAL Driver TB 2014-2015
//Eldwan Brianne
//DESY, Jan 2015

#ifndef TBecal4d_h
#define TBecal4d_h 1

#include <vector>
#include <utility>
#include "Control.hh"
#include "VSubDetectorDriver.hh"
#include "TBSD_VCellecal4d.hh"
#include "CGAGeometryEnvironment.hh"

#define MAX_TBECAL_LAYERS 100

class TBSD_VCellecal4d;
class Database;
class G4LogicalVolume;
class G4Material;
class G4VPhysicalVolume;

class TBecal4d : public VSubDetectorDriver
{
public:
  //Constructor
  TBecal4d();

  //Destructor
  ~TBecal4d();

  /** Main function called in Mokka
  */
  G4bool ContextualConstruct(
    const CGAGeometryEnvironment &aGeometryEnvironment,
    G4LogicalVolume *theWorld);

    /** Get ECAL grid size
    */
    G4double* GetGridSize() { return grid_size; }

    /** Get depth to layer
    */
    G4int GetDepthToLayer() const { return depthToLayer; }

  private:

    /** Build ECAL elements. An ECAL layers contains:
    - absorber plate
    - air gap
    - steel cassette
    - cable-fibre mix
    - PCB
    - 3M foil
    - scintillator
    - 3M foil
    - steel cassette
    - air gap
    ECAL contains 3 such layers, first layer outside stack and 2 other into the stack.
    */

    void BuildEcalElements();

    /** Set variable absorber thicknesses for each layer
    */
    void SetEcalAbsorberThickness();
    /** Place ECAL elements into layer
    */
    void PlaceEcalElementsIntoLayer();

    /** Set ECAL sensitive detector
    */
    void SetSD();

    /** Place ECAL layers into the world volume
    @ worldLog - world logical volume
    */
    G4bool PlaceEcalLayers(G4LogicalVolume *worldLog);

    /** Fetch ECAL related MySQL variables from the Mokka data base
    */
    void FetchMySQLVariables();

    /** Print out ECAL dimensions
    */
    void Print();

    /** Define new GEANT4 materials needed for ECAL
    */
    void DefineEcalMaterials();

    /** Set the level on which the layer is implemented in G4
    */
    void SetDepthToLayer(G4int i) {
      depthToLayer = i;
      #ifdef TBSD_DEBUG
      G4cout <<"DepthToLayer in Hcal: " << depthToLayer << G4endl;
      #endif
    }

    G4bool isValidSensitiveLayerPattern(G4String sensitiveLayerPattern);

    /** Fill std::vector<G4int> according to the string 'sensitiveLayerPattern'
    The variable 'n_layers' needs to be set correctly before using this method.
    */
    std::vector<G4int> getSensitiveLayerPatternVector(G4String sensitiveLayerPattern);

  private:

    Database* db;        /**< pointer to Mokka data base*/
    TBSD_VCellecal4d *ecalSD;//ECAL sensitive detector
    CGAGeometryEnvironment _aGeometryEnvironment;/**<The Environment object*/

    /**********Detector parameters********************************************/

    /**********Full layer****************/

    G4int ECAL_n_layers;     /**< number of ECAL layers*/
    G4int ncell_x;               //** Number of cells in x */
    G4int ncell_y;              //** Number of cells in y */
    G4double grid_size[2];

    G4double EBU_X_dimension;/**< X-dimension of ECAL box*/
    G4double EBU_Y_dimension;/**< Y-dimension of ECAL box*/
    G4double EBU_hx;        /**< half x-dimension of EBU detector box*/
    G4double EBU_hy;        /**< half y-dimension of EBU detector box*/

    G4double cal_hx;        /**< half x-dimension of HCAL detector box*/
    G4double cal_hy;        /**< half y-dimension of HCAL detector box*/
    G4double cal_hz;        /**< half z-dimension of HCAL detector box*/
    G4double x_begin;       /**< x-position where the ECAL detector starts*/
    G4double y_begin;       /**< y-position where the ECAL detector starts*/
    G4double z_begin;       /**< z-position where the ECAL detector starts*/
    G4double z_place;       /**< z-position where the ECAL detector is placed*/
    G4double rotationAngle; /**<rotation angle*/
    G4double config_angle;  /**< configuration angle of detector*/

    G4double lateral_x; //**dimension of ECAL board*/
    G4double lateral_y;  /**dimension of ECAL board*/

    /* The aluminium frame around the absorber is 1000x1000mm^2 */
    G4double ECAL_Layer_X_dimension;
    G4double ECAL_Layer_Y_dimension;

    //Absorber
    G4double Octagonal_absorber_inner_radious[2]; /**< Inner radious of octagonal absorber*/
    G4double Octagonal_absorber_outer_radious[2]; /**< Outer radious of octagonal absorber*/
    G4double Octagonal_absorber_z[2];             /**< Z of octagonal absorber*/
    G4int Octagonal_absorber_number_of_sides;     /**< Nr of sides of ctagonal absorber*/
    G4int Octagonal_absorber_number_of_layers;    /**< Nr of layers octagonal absorber*/

    G4int depthToLayer; /**<Variable describing the depth of where the layer is implemented within the G4 volumes hierarchy*/

    //*************** thicknesses *************/

    G4double poly_hthickness;           /**< half thickness (along z) of the scintillator (made of polystyrene)*/
    G4double tungsten_hthickness;       /**< half thickness of the tungsten layer*/
    G4double steel_support_hthickness;  /**< half thickness of the steel support layer*/
    G4double steel_cassette_hthickness; /**< half thickness of the steel cassette*/
    G4double foil_hthickness;           /**< half thickness of the 3M foil*/
    G4double pcb_hthickness;            /**< half thickness of the PCB board*/
    G4double cablefibre_mix_hthickness; /**< half thickness of the cable-fibre mixture*/

    G4double scinCassette_hthickness; /**< half thickness of the scintillator cassette + contents + air*/

    /**********Logical volumes arrays****************/

    /*********empty layer without cassette**************/
    G4LogicalVolume *WholeLayerAirLogical[MAX_TBECAL_LAYERS]; /**< logical volume for the uninstrumented ECAL layer*/
    //Instrumented layer
    G4LogicalVolume *WholeLayerLogical[MAX_TBECAL_LAYERS];     /**< logical volume for the fully instrumented ECAL layer*/

    //Absorber
    G4LogicalVolume *AbsLayerLogical[MAX_TBECAL_LAYERS];       /**< logical volume for the absorber plate */

    //Instrumentation
    G4LogicalVolume *CassetteLogical_1[MAX_TBECAL_LAYERS];     /**< logical volume for the steel cassette front */
    G4LogicalVolume *CassetteLogical_2[MAX_TBECAL_LAYERS];     /**< logical volume for the steel cassette back */
    G4LogicalVolume *CFmix_LayerLogical[MAX_TBECAL_LAYERS];    /**< logical volume for the cable-fibre mix*/
    G4LogicalVolume *PCBLayerLogical[MAX_TBECAL_LAYERS];       /**< logical volume for the PCB board*/
    G4LogicalVolume *FoilLayerLogical_1[MAX_TBECAL_LAYERS];    /**< logical volume for the first 3M foil */
    G4LogicalVolume *FoilLayerLogical_2[MAX_TBECAL_LAYERS];    /**< logical volume for the second 3M foil */
    G4LogicalVolume *ScintLayerLogical[MAX_TBECAL_LAYERS];     /**< logical volume for the scintillator*/

    //Airgaps
    G4LogicalVolume *AirgapLayerLogical_1[MAX_TBECAL_LAYERS];     /**< logical volume for the airgap front*/
    G4LogicalVolume *AirgapLayerLogical_2[MAX_TBECAL_LAYERS];     /**< logical volume for the airgap back*/
    G4LogicalVolume *GapLayerLogical[MAX_TBECAL_LAYERS];          /**< logical volume for the gap back*/

    //Tungsten specifics
    G4LogicalVolume *SteelSupportLogical[MAX_TBECAL_LAYERS];   /**< logical volume for the steel support*/
    G4LogicalVolume *AluminiumframeLogical[MAX_TBECAL_LAYERS];     /**< logical volume for the steel frame*/

    //----------------

    G4double steel_hthickness;                                 /**< half thickness of the absorber plate (made of steel)*/
    G4double layer_hthickness[MAX_TBECAL_LAYERS];              /**< half thickness of the ECAL layer*/

    G4LogicalVolume *DetectorLogical;       /**< logical volume for the ECAL detector */
    G4LogicalVolume *WorldLogical;          /**< world logical volume*/

    /****** Material *********/

    G4Material *poly;          /**< polystyrene - used as scintillator material*/
    G4Material *Polystyrole;   /**< material of the 3M foil*/
    G4Material *air;           /**< guess :) air*/
    G4Material *PCB;           /**< material used for the PCB board*/
    G4Material *S235;          /**< steel of type S235 - used for absorber plate and steel cassette material*/
    G4Material *tungstenalloy; /**< tungsten composed of tungsten Nikel and Copper*/
    G4Material *aluminium;     /** < Aluminiuml*/
    G4Material *CF_MIX;        /**< material of the cable-fibre mix*/
    G4Material *nikel;         /**< material nikel for tungsten alloy*/
    G4Material *coretun;       /**< material tungsten core for tungsten alloy*/
    G4bool checkForOverlappingVolumes;/**< flag to check for overlapping volumes (for debug purposes)*/

    G4double PCB_density;
    G4double PCB_silicon_fractiomass;
    G4double PCB_elO_fractionmass;
    G4double PCB_graphite_fractionmass;
    G4double PCB_elH_fractionmass;
    G4double PCB_elBr_fractionmass;
    G4double S235_density;
    G4double S235_iron_fractionmass;
    G4double S235_graphite_fractionmass;
    G4double S235_manganese_fractionmass;
    G4double tungstenalloy_density;
    G4double coretun_density;
    G4double nikel_density;
    G4double tungsten_fractionmass;
    G4double nikel_fractionmass;
    G4double copper_fractionmass;
    G4double PVC_density;
    G4double Polystyrole_density;
    G4double CF_Mix_density;
    G4double CF_MIX_air_fractionmass;
    G4double CF_MIX_PVC_fractionmass;
    G4double CF_MIX_Polystyrole_fractionmass;

    /* USER PARAMETERS */

    G4int beginWithAbsorber;
    G4bool isTungstenAbs;
    G4bool isIronAbs;
    G4String Ecal_Radiator_Material;
    std::vector<G4int> sensitiveLayerPatternVector;  /**<vector of integers indicating the arrangement of sensitive layers in the ECAL prototype (e.g. "101010...10 indicates that each second layer is equiped with a sensitive casette)*/
    G4double gap_hthickness;            /*USER DEFINED PARAMETER**< half thickness of the gap for EBU*/
    G4double airgap_hthickness;         /*USER DEFINED PARAMETER**< half thickness of the air gap in the back of the cassette*/

  };

  #endif
