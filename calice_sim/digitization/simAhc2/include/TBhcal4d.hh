#ifndef TBhcal4d_h
#define TBhcal4d_h 1

#include "Control.hh"
#include "TBSD_VCell4d.hh"
#include "VSubDetectorDriver.hh"
#include "CGAGeometryEnvironment.hh"
#include "G4Transform3D.hh"

#define MAX_TBHCAL_LAYERS 100

class TBSD_VCell4d;
class Database;
class G4LogicalVolume;
class G4Material;
class G4VPhysicalVolume;

class TBhcal4d : public VSubDetectorDriver
{

public:
  /** Default constructor
  */
  TBhcal4d();
  /** Destructor
  */
  ~TBhcal4d();

  /** Main function called in Mokka
  */
  G4bool ContextualConstruct(const CGAGeometryEnvironment &aGeometryEnvironment, G4LogicalVolume *theWorld);

  /** Get HCAL grid size
  */
  G4double GetGridSize() { return Hcal_cells_size; }

  /** Get depth to layer
  */
  G4int GetDepthToLayer() const { return depthToLayer; }

private:


  /** Build HCAL elements. An HCAL layers contains:
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

  HCAL contains 38 such layers, plus an additional terminating absorber plate.
  */
  void BuildHcalElements();

  /** Set variable absorber thicknesses for each layer
  */
  void SetHcalAbsorberThickness();
  /** Place HCAL elements into layer
  */
  void PlaceHcalElementsIntoLayer();

  /** Set HCAL sensitive detector
  */
  void SetSD();

  /** Place HCAL layers into the world volume
  @ worldLog - world logical volume

  Note: only one terminating absorber plate is build, of thickness 20.5 mm.
  */
  G4bool PlaceHcalLayers(G4LogicalVolume *worldLog);

  /** Fetch HCAL related MySQL variables from the Mokka data base
  */
  void FetchMySQLVariables();

  /** Print out HCAL dimensions
  */
  void Print();

  /** Define new GEANT4 materials needed for HCAL
  */
  void DefineHcalMaterials();

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

  //************** Scintillators **************************

  /* Main for building */
  G4bool BuildScintElements(G4LogicalVolume *WorldLogical);

  /* Function for building a scint layer */
  G4bool ScintConstruct(G4LogicalVolume *WorldLogical, G4double x_place, G4double y_place, G4double z_place, G4int idscintillator);

  /* Function for building elements of scint layer */
  void BuildElements(G4int idscintillator);

  /* Function for placing scint layer */
  G4bool PlaceScintElements(G4LogicalVolume *WorldLogical, G4double x_place, G4double y_place, G4double z_place, G4int idscintillator);

  /* Function setting the sensitive driver of scint layers */
  void SetSD(G4int idscintillator);

  //****************** Add Mat ******************/

  /* Main for building a single layer of additional material */
  G4bool BuildAddElements(G4LogicalVolume *WorldLogical);

private:

  Database* db;        /**< pointer to Mokka data base*/
  TBSD_VCell4d *hcalSD; /**<HCAL sensitive detector*/
  CGAGeometryEnvironment _aGeometryEnvironment;/**<The Environment object*/

  /**********Detector parameters********************************************/

  /**********Full layer****************/

  G4int HCAL_n_layers;     /**< number of HCAL layers*/
  G4int HCAL_Layer_ncell_x;/**< number of cells in x and y direction */
  G4int HCAL_Layer_ncell_y;/**< number of cells in x and y direction */
  G4double Hcal_cells_size;/**< parameter determing the cellwise subdivision of a layer */
  G4double cal_hx;        /**< half x-dimension of HCAL detector box*/
  G4double cal_hy;        /**< half y-dimension of HCAL detector box*/
  G4double cal_hz;        /**< half z-dimension of HCAL detector box*/
  G4double x_begin;       /**< x-position where the HCAL detector starts*/
  G4double y_begin;       /**< y-position where the HCAL detector starts*/
  G4double z_begin;       /**< z-position where the HCAL detector starts*/
  G4double z_place;       /**< z-position where the HCAL detector is placed*/
  G4double rotationAngle; /**<rotation angle*/
  G4double config_angle;  /**< configuration angle of detector*/
  G4double HCAL_Layer_X_dimension; /**< X dimension of HCAL Layer*/
  G4double HCAL_Layer_Y_dimension; /**< X dimension of HCAL Layer*/

  /**********Shower Start Finder****************/
  G4int HCAL_SSFLayer_ncell_x;/*USER DEFINED PARAMETER**< number of cells in x and y direction  for Shower Start Finder*/
  G4int HCAL_SSFLayer_ncell_y;/*USER DEFINED PARAMETER**< number of cells in x and y direction  for Shower Start Finder*/
  G4double SSF_HBU_X_dimension;        /**< X-dimension of HCAL Shower Start Finder box*/
  G4double SSF_HBU_Y_dimension;        /**< Y-dimension of HCAL Shower Start Finder box*/
  G4double HBU_hx;        /**< half x-dimension of HBU detector box*/
  G4double HBU_hy;        /**< half y-dimension of HBU detector box*/

  //Absorber
  G4double Octagonal_absorber_inner_radious[2]; /**< Inner radious of octagonal absorber*/
  G4double Octagonal_absorber_outer_radious[2]; /**< Outer radious of octagonal absorber*/
  G4double Octagonal_absorber_z[2];             /**< Z of octagonal absorber*/
  G4int Octagonal_absorber_number_of_sides;     /**< Nr of sides of ctagonal absorber*/
  G4int Octagonal_absorber_number_of_layers;    /**< Nr of layers octagonal absorber*/

  G4int depthToLayer; /**<Variable describing the depth of where the layer is implemented within the G4 volumes hierarchy*/

  G4double poly_hthickness;           /**< half thickness (along z) of the scintillator (made of polystyrene)*/
  G4double tungsten_hthickness;       /**< half thickness of the tungsten layer*/
  G4double steel_support_hthickness;  /**< half thickness of the steel support layer*/
  G4double steel_cassette_hthickness; /**< half thickness of the steel cassette*/
  G4double foil_hthickness;           /**< half thickness of the 3M foil*/
  G4double pcb_hthickness;            /**< half thickness of the PCB board*/
  G4double cablefibre_mix_hthickness; /**< half thickness of the cable-fibre mixture*/

  G4double scinCassette_hthickness; /**< half thickness of the scintillator cassette + contents + air*/

  /**********Logical volumes arrays****************/

  /**** Full layer ********/
  G4LogicalVolume *WholeLayerLogical[MAX_TBHCAL_LAYERS];     /**< logical volume for the fully instrumented HCAL layer*/
  //SSF
  G4LogicalVolume *WholeLayer_SSF_Logical[MAX_TBHCAL_LAYERS]; /**< logical volume for the scintillator cassette of the SSF */
  /*********empty layer without cassette**************/
  G4LogicalVolume *WholeLayerAirLogical[MAX_TBHCAL_LAYERS]; /**< logical volume for the uninstrumented HCAL layer*/

  //Absorber
  G4LogicalVolume *AbsLayerLogical[MAX_TBHCAL_LAYERS];       /**< logical volume for the absorber plate */

  G4LogicalVolume *CassetteLogical_1[MAX_TBHCAL_LAYERS];       /**< logical volume for the steel cassette */
  G4LogicalVolume *CassetteLogical_2[MAX_TBHCAL_LAYERS];       /**< logical volume for the steel cassette */
  G4LogicalVolume *CFmix_LayerLogical[MAX_TBHCAL_LAYERS];    /**< logical volume for the cable-fibre mix*/
  G4LogicalVolume *PCBLayerLogical[MAX_TBHCAL_LAYERS];       /**< logical volume for the PCB board*/
  G4LogicalVolume *FoilLayerLogical_1[MAX_TBHCAL_LAYERS];    /**< logical volume for the first 3M foil */
  G4LogicalVolume *FoilLayerLogical_2[MAX_TBHCAL_LAYERS];    /**< logical volume for the second 3M foil */
  G4LogicalVolume *ScintLayerLogical[MAX_TBHCAL_LAYERS]; /**< logical volume for the scintillator*/
  G4LogicalVolume *AirgapLayerLogical_1[MAX_TBHCAL_LAYERS]; /**< logical volume for the airgap*/
  G4LogicalVolume *AirgapLayerLogical_2[MAX_TBHCAL_LAYERS]; /**< logical volume for the airgap*/

  //Tungsten
  G4LogicalVolume *SteelSupportLogical[MAX_TBHCAL_LAYERS];   /**< logical volume for the steel support*/
  G4LogicalVolume *AluminiumframeLogical[MAX_TBHCAL_LAYERS];     /**< logical volume for the steel frame*/

  /**** Shower Start Finder ********/
  G4LogicalVolume *CFmix_Layer_SSF_Logical[MAX_TBHCAL_LAYERS];
  G4LogicalVolume *Cassette_SSF_Logical_1[MAX_TBHCAL_LAYERS];      /**< logical volume for the steel cassette SSF  */
  G4LogicalVolume *Cassette_SSF_Logical_2[MAX_TBHCAL_LAYERS];      /**< logical volume for the PCB board SSF */
  G4LogicalVolume *PCBLayer_SSF_Logical[MAX_TBHCAL_LAYERS];       /**< logical volume for the PCB board SSF*/
  G4LogicalVolume *FoilLayer_SSF_Logical_1[MAX_TBHCAL_LAYERS];    /**< logical volume for the first 3M foil SSF  */
  G4LogicalVolume *FoilLayer_SSF_Logical_2[MAX_TBHCAL_LAYERS];   /**< logical volume for the second 3M foil SSF  */
  G4LogicalVolume *ScintLayer_SSF_Logical[MAX_TBHCAL_LAYERS]; /**< logical volume for the scintillator SSF */
  G4LogicalVolume *AirgapLayer_SSF_Logical_1[MAX_TBHCAL_LAYERS]; /**< logical volume for the airgap*/
  G4LogicalVolume *AirgapLayer_SSF_Logical_2[MAX_TBHCAL_LAYERS]; /**< logical volume for the airgap*/

  //Thicknesses

  G4double steel_hthickness;              /**< half thickness of the absorber plate (made of steel)*/
  G4double layer_hthickness[MAX_TBHCAL_LAYERS];              /**< half thickness of the HCAL layer*/

  G4LogicalVolume *DetectorLogical;       /**< logical volume for the HCAL detector */
  G4LogicalVolume *WorldLogical;          /**< world logical volume*/

  //************** Materials ******************/

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

  //****** USER PARAMETERS *****************//

  G4int beginWithAbsorber;
  G4bool isTungstenAbs;
  G4bool isIronAbs;
  G4String Hcal_Radiator_Material;
  std::vector<G4int> sensitiveLayerPatternVector;       /**<vector of integers indicating the arrangement of sensitive layers in the HCAL prototype (e.g. "101010...10 indicates that each second layer is equiped with a sensitive casette)*/
  G4double airgap_hthickness;         /*USER DEFINED PARAMETER**< half thickness of the air gap in the back of the cassette*/

  //********************** Scintillator beam ***************

  G4int Add_Scint;

  //Big Scintillator
  G4LogicalVolume *ScintillatorLogical500x500_front;
  G4LogicalVolume *ScintillatorLogical500x500_back;

  //Small Scintillator
  G4LogicalVolume *ScintillatorLogical100x100_front;
  G4LogicalVolume *ScintillatorLogical100x100_back;

  //Sensitive detector
  TBSD_VCell4d *scintSD_500x500_front; /**<Scint sensitive detector big scint front*/
  TBSD_VCell4d *scintSD_500x500_back; /**<Scint sensitive detector big scint back*/
  TBSD_VCell4d *scintSD_100x100_front; /**<Scint sensitive detector small scint front*/
  TBSD_VCell4d *scintSD_100x100_back; /**<Scint sensitive detector small scint back*/

  /* Variables containing info on detector dimensions*/
  G4double X_dim_Scint500x500;
  G4double Y_dim_Scint500x500;
  G4double hthickness_Scint500x500;
  G4double z_begin_Scint500x500_front;
  G4double z_begin_Scint500x500_back;

  //Small Scint
  G4double X_dim_Scint100x100;
  G4double Y_dim_Scint100x100;
  G4double hthickness_Scint100x100;
  G4double z_begin_Scint100x100_front;//1st small
  G4double z_begin_Scint100x100_back;//2nd small

  G4double sci_hx, sci_hy, sci_hz;//dimension scint box

  //********************** Additional Material ***************

  G4int Add_Mat;

  //Additionnal Mat
  G4LogicalVolume *AddMaterialLogical;
  G4Material *mat;         /**< material for Additional mat*/
  G4bool isLead, isIron, isAl;
  
  //Parameters
  G4double X_dim_AddMaterial;
  G4double Y_dim_AddMaterial;
  G4double hthickness_AddMaterial;
  G4double z_begin_AddMaterial;
  G4String Radiator_AddMaterial;

  G4double AddMaterial_hx, AddMaterial_hy, AddMaterial_hz;//dimension AddMaterial box
};

#endif
