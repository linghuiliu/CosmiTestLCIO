//ECAL Driver TB 2014-2015
//v1.0, 01.15
//to simulate TB 2014 Setup
//Eldwan Brianne

#include "Control.hh"
#include "TBecal4d.hh"
#include "CGADefs.h"
#include "MySQLWrapper.hh"
#include "CGAGeometryManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVParameterised.hh"
#include "G4UnitsTable.hh"
#include "UserInit.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#ifdef MOKKA_GEAR
#include "gear/CalorimeterParameters.h"
#include "gearimpl/CalorimeterParametersImpl.h"
#include "gearimpl/LayerLayoutImpl.h"
#include "MokkaGear.h"
#endif

#define TBECAL4D_DEBUG 0

#include <sstream>

/* define some levels of detail for graphical display*/
#define DM_FULL 3
#define DM_ABSORBERANDSENSITIVE 2
#define DM_WHOLELAYERONLY  1
/*
Geometry driver for test-beam configuration 2014-2015

//Layer pattern
0 : no layer (absorber only)
1 : EBU Horizontal -> readout parallele to strips
2 : EBU Transverse -> readout perpendicular to strips

*/

INSTANTIATE(TBecal4d)
/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

TBecal4d::TBecal4d() : VSubDetectorDriver("TBecal4d","TBecal"),
db(0),
_aGeometryEnvironment("","",NULL, NULL),
config_angle(0)
{
  checkForOverlappingVolumes = false;
  #ifdef TBECAL4D_DEBUG
  checkForOverlappingVolumes = true;
  #endif
}
/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/
TBecal4d::~TBecal4d()
{

}
/*================================================================================*/
/*                                                                                */
/*     Main function                                                              */
/*                                                                                */
/*================================================================================*/

G4bool TBecal4d::ContextualConstruct(const CGAGeometryEnvironment &aGeometryEnvironment,
  G4LogicalVolume *WorldLog)
  {
    G4cout<<"\n ***************************************************************"<<G4endl;
    G4cout<<" *                                                             *"<<G4endl;
    G4cout<<" *    Build ECAL based on the TBecal4d driver                  *"<<G4endl;
    G4cout<<" *                                                             *"<<G4endl;
    G4cout<<" ***************************************************************"<<G4endl;

    /*Obtain the pointer to our database via the Environment object*/
    _aGeometryEnvironment = aGeometryEnvironment;
    db = new Database(_aGeometryEnvironment.GetDBName());

    WorldLogical = WorldLog;

    G4cout<<"\n Before FetchMySQLVariables"<<G4endl;
    FetchMySQLVariables();

    G4cout<<"\n before DefineEcalMaterials "<<G4endl;
    DefineEcalMaterials();

    G4cout<<"\n before BuildEcalElements "<<G4endl;
    BuildEcalElements();

    G4cout<<"\n before Print "<<G4endl;
    Print();

    /*-------------------------------------------------------------------
    GEAR information
    */
    G4double innerRadius = 0;
    G4double outerRadius = cal_hx ;
    G4double leastZ      = z_begin;
    G4int symmetryOrder  = 4;      /*this is a standalone prototype*/
    G4double phi         = 0;
    gear::CalorimeterParametersImpl *gearParam =
    new gear::CalorimeterParametersImpl(innerRadius, outerRadius, leastZ, symmetryOrder, phi);

    for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
    {
      G4double distance = 0; /*distance of this layer from the origin*/
      G4double layerThickness = 0;
      G4double absorberThickness = 0;
      if (isTungstenAbs) {
        layerThickness    = (tungsten_hthickness + steel_support_hthickness + scinCassette_hthickness) * 2.;
        absorberThickness = tungsten_hthickness * 2.;
      } else if (isIronAbs) {
        layerThickness    = (steel_hthickness + scinCassette_hthickness) * 2.;
        absorberThickness = steel_hthickness * 2.;
      }
      G4double cellSize0 = 45. * mm; /*cell size along the x-axis*/
      G4double cellSize1 = 5. * mm; /*cell size along the y-axis*/

      gearParam->layerLayout().positionLayer(distance, layerThickness, cellSize0, cellSize1, absorberThickness);

    }/*end loop over iLayer*/

    /* write parameters to GearManager*/
    MokkaGear* gearMgr = MokkaGear::getMgr() ;
    gearMgr->setEcalEndcapParameters( gearParam ) ;
    /*-------------------------------------------------------------------*/

    G4bool cokay = PlaceEcalLayers(WorldLogical);

    delete db;
    G4cout << "\nDone building TBecal4d" << G4endl;

    return cokay;
  }

  /*================================================================================*/
  /*                                                                                */
  /*       Fetch HCAL related MySQL variables                                       */
  /*                                                                                */
  /*================================================================================*/
  void TBecal4d::FetchMySQLVariables()
  {

    /* config angle from environment object
    config_angle: angle reflecting the rotations for non normal incidence.*/
    config_angle                     =  _aGeometryEnvironment.GetParameterAsDouble("configuration_angle") + _aGeometryEnvironment.GetParameterAsDouble("EcalRotationAngle");
    grid_size[0]                     = _aGeometryEnvironment.GetParameterAsDouble("grid_size_x");
    grid_size[1]                     = _aGeometryEnvironment.GetParameterAsDouble("grid_size_y");
    ECAL_n_layers                    = _aGeometryEnvironment.GetParameterAsInt("Ecal_nlayers");
    lateral_x                        = _aGeometryEnvironment.GetParameterAsDouble("Lateral_X");
    lateral_y                        = _aGeometryEnvironment.GetParameterAsDouble("Lateral_Y");
    Ecal_Radiator_Material           = _aGeometryEnvironment.GetParameterAsString("Ecal_radiator_material");

    beginWithAbsorber = UserInit::getInstance()->getInt("Ecal_beginWithAbsorber");//temporarly defined as user Defined variables

    /*Absorber thickness*/
    steel_hthickness = 17.2/2.0; //EUDET AHCAL

    if(Ecal_Radiator_Material == "tungsten") {
      isTungstenAbs = true;
      isIronAbs     = false;
    }  else if(Ecal_Radiator_Material == "Iron") {
      isTungstenAbs = false;
      isIronAbs     = true;
    }  else
    Control::Abort("TBecal4d: invalid radiator material name. \nIt has to be either Iron or tungsten!!!",MOKKA_ERROR_BAD_GLOBAL_PARAMETERS);

    /*we are dealing with a grid that change depending on the strip position (vertical and horizontal to the readout path)
    We only to know the number of layers in each layout, the number of cells in x, y and z direction
    and create the grid
    */

    db->exec("select *  from ECAL_tungsten_virt;" );
    db->getTuple();

    /* The aluminium frame around the absorber is 1000x1000mm^2 */
    ECAL_Layer_X_dimension           = db->fetchInt("ECAL_Layer_X_dimension")/2;
    ECAL_Layer_Y_dimension           = db->fetchInt("ECAL_Layer_Y_dimension")/2;

    ncell_x = (G4int)(lateral_x/ (grid_size[0]));
    ncell_y = (G4int)(lateral_y/ (grid_size[1]));

    assert(ECAL_n_layers > 0);
    assert(ncell_x >= 0 || ncell_y >= 0);

    /*the beginning of the Ecal USER PARAMETER! */
    // x_begin                          = db->fetchDouble("X_position_of_first_ECAL_layer");
    // y_begin                          = db->fetchDouble("Y_position_of_first_ECAL_layer");
    // z_begin                          = db->fetchDouble("Z_position_of_first_ECAL_layer");
    x_begin                          = UserInit::getInstance()->getDouble("X_position_of_first_ECAL_layer");
    y_begin                          = UserInit::getInstance()->getDouble("Y_position_of_first_ECAL_layer");
    z_begin                          = UserInit::getInstance()->getDouble("Z_position_of_first_ECAL_layer");

    /* get 'global' x and y translation from steering file (at run time) or database*/
    G4double XTranslation = _aGeometryEnvironment.GetParameterAsDouble("EcalTranslateX");
    G4double YTranslation = _aGeometryEnvironment.GetParameterAsDouble("EcalTranslateY");

    x_begin += XTranslation;
    y_begin += YTranslation;

    /* get rotation angle from steering file (at run time) or database*/
    rotationAngle = _aGeometryEnvironment.GetParameterAsDouble("EcalRotationAngle");
    rotationAngle = rotationAngle*deg;

    /*Absorber stuff*/
    db->exec("select * from ECAL_tungsten_absorber_virt;");
    db->getTuple();

    Octagonal_absorber_inner_radious[0] = db->fetchDouble("Octagonal_absorber_inner_radious_front");
    Octagonal_absorber_inner_radious[1] = db->fetchDouble("Octagonal_absorber_inner_radious_back");
    Octagonal_absorber_outer_radious[0] = db->fetchDouble("Octagonal_absorber_outer_radious_front");
    Octagonal_absorber_outer_radious[1] = db->fetchDouble("Octagonal_absorber_outer_radious_back");

    Octagonal_absorber_number_of_sides  = db->fetchInt("Octagonal_absorber_number_of_sides");
    Octagonal_absorber_number_of_layers = db->fetchInt("Octagonal_absorber_number_of_layers");

    /* Materials*/
    db->exec("select * from ECAL_tungsten_materials_virt;");
    db->getTuple();

    PCB_density				                 = db->fetchDouble("PCB_density")*g/cm3;
    PCB_silicon_fractiomass		         = db->fetchDouble("PCB_silicon_2.33_fractiomass");
    PCB_elO_fractionmass			         = db->fetchDouble("PCB_elO_fractionmass");
    PCB_graphite_fractionmass		       = db->fetchDouble("PCB_graphite_fractionmass");
    PCB_elH_fractionmass			         = db->fetchDouble("PCB_elH_fractionmass");
    PCB_elBr_fractionmass			         = db->fetchDouble("PCB_elBr_fractionmass");
    S235_density			                 = db->fetchDouble("S235_density")*g/cm3;
    S235_iron_fractionmass		         = db->fetchDouble("S235_iron_fractionmass");
    S235_graphite_fractionmass		     = db->fetchDouble("S235_graphite_fractionmass");
    S235_manganese_fractionmass		     = db->fetchDouble("S235_manganese_fractionmass");
    tungstenalloy_density			         = db->fetchDouble("tungsten_density")*g/cm3;
    coretun_density			               = db->fetchDouble("tungsten_core_density")*g/cm3;
    tungsten_fractionmass	             = db->fetchDouble("tungsten_core_tungsten_fractionmass");
    nikel_fractionmass		             = db->fetchDouble("tungsten_nikel_fractionmass");
    nikel_density                      = db->fetchDouble("nikel_density")*g/cm3;
    copper_fractionmass		             = db->fetchDouble("tungsten_copper_fractionmass");
    PVC_density				                 = db->fetchDouble("PVC_density")*g/cm3;
    Polystyrole_density			           = db->fetchDouble("Polystyrole_density")*g/cm3;
    CF_Mix_density			               = db->fetchDouble("CF_Mix_density")*g/cm3;
    CF_MIX_air_fractionmass		         = db->fetchDouble("CF_MIX_air_fractionmass");
    CF_MIX_PVC_fractionmass		         = db->fetchDouble("CF_MIX_PVC_fractionmass");
    CF_MIX_Polystyrole_fractionmass	   = db->fetchDouble("CF_MIX_Polystyrole_fractionmass");

    /* thicknesses*/
    db->exec("select * from ECAL_tungsten_layerthickness_virt;");
    db->getTuple();

    poly_hthickness                       = db->fetchDouble("poly_hthickness")/2;
    tungsten_hthickness                   = db->fetchDouble("tungsten_hthickness")/2;
    steel_support_hthickness              = db->fetchDouble("steel_support_thickness")/2;
    airgap_hthickness                     = UserInit::getInstance()->getDouble("Ecal_airgap_hthickness")/2; //USER PARAMETER
    steel_cassette_hthickness             = db->fetchDouble("steel_cassette_hthickness")/2;
    foil_hthickness                       = db->fetchDouble("foil_hthickness")/2;
    pcb_hthickness                        = (db->fetchDouble("pcb_hthickness") - 1.0)/2;
    cablefibre_mix_hthickness             = db->fetchDouble("cablefibre_mix_hthickness")/2;
    gap_hthickness                        = UserInit::getInstance()->getDouble("gap_hthickness")/2;

    Octagonal_absorber_z[0]               = (-1) * tungsten_hthickness;
    Octagonal_absorber_z[1]               = tungsten_hthickness;

    /*--------------------------
    scintillator cassette + contents + air gaps:
    -----------------------------------*/
    scinCassette_hthickness =  airgap_hthickness /* air gap in the front*/
    + steel_cassette_hthickness                /*steel cassette*/
    + cablefibre_mix_hthickness                /*cable-fibre mix*/
    + pcb_hthickness                           /*PCB*/
    + foil_hthickness                          /*3M foil*/
    + poly_hthickness                          /*scintillator*/
    + foil_hthickness                          /*3M foil*/
    + steel_cassette_hthickness                /*steel cassette*/
    + airgap_hthickness                        /* air gap in the back*/
    + gap_hthickness;                          /* air gap of 1 mm for ECAL*/

    G4cout << "Layer half thickness: " << scinCassette_hthickness << G4endl;

    /*---------------------------------------
    Introduce a string of numbers to deal with 2015 layer configuration:
    0--> no EBUs
    1--> EBU Horizontal
    2--> EBU Transverse
    ---------------------------------------*/

    /* set up vector for the sensitive layer pattern*/
    G4String sensitiveLayerPatternFromSteeringFile = _aGeometryEnvironment.GetParameterAsString("Ecal_layer_pattern");
    G4cout <<" \n\n\n PATTERN length = " << sensitiveLayerPatternFromSteeringFile.length() << " , " << sensitiveLayerPatternFromSteeringFile << G4endl;

    /*---------------
    if steering command:
    /Mokka/init/globalModelParameter Ecal_layer_pattern 010101
    exists in steering file:
    set sensitiveLayerPatternVector to that value
    ----------*/

    if ( ((G4int)sensitiveLayerPatternFromSteeringFile.length()) != 0 )
    {
      if (isValidSensitiveLayerPattern(sensitiveLayerPatternFromSteeringFile))
      {
        sensitiveLayerPatternVector = getSensitiveLayerPatternVector(sensitiveLayerPatternFromSteeringFile);
      }
      else
      {
        G4cout << "'/Mokka/init/ECALLayerPattern' parameter is not valid in steering file. ABORTING MOKKA" << G4endl;
        exit(1);
      }
    }
    else
    {
      for (G4int i = 0; i < ECAL_n_layers; ++i)
      sensitiveLayerPatternVector.at(i) = 3;
    }
  }

  /*================================================================================*/
  /*                                                                                */
  /*  Define calculated variables, materials, solids, logical volumes               */
  /*                                                                                */
  /*================================================================================*/

  void TBecal4d::BuildEcalElements()
  {

    /*Information needed when hits are processed later on*/
    /*Must be set before calling the constructor of the sensitive detector,
    which is done in SetSD()!*/
    SetDepthToLayer(1);

    /* create and register the sensitive detector before
    defining the sensitive logical volumes!
    */
    SetSD();

    //setting the dimenstion for EBU
    EBU_X_dimension= (G4double) (ncell_x * 45.00);
    EBU_Y_dimension= (G4double) (ncell_y * 5.00);

    /*calorimeter dimensions*/
    cal_hz = 0;
    cal_hx = (G4double)( ncell_x * grid_size[0] )*2;
    cal_hy = (G4double)( ncell_y * grid_size[1] )*2;

    EBU_hx =  EBU_X_dimension/2;
    EBU_hy =  EBU_Y_dimension/2;

    std::stringstream stringForLayerNo; /*string to save layer number*/

    //Layer Box
    G4Box              *WholeLayerSolid[MAX_TBECAL_LAYERS]        = {NULL};
    G4Box              *WholeLayerAirSolid[MAX_TBECAL_LAYERS]     = {NULL};

    //For Tungsten
    G4Polyhedra        *AbsLayerSolidW[MAX_TBECAL_LAYERS]         = {NULL};
    G4Box              *AluminiumframeAll[MAX_TBECAL_LAYERS]      = {NULL};
    G4SubtractionSolid *AluminiumframeSolid[MAX_TBECAL_LAYERS]    = {NULL};
    G4Box              *SteelSupportSolid[MAX_TBECAL_LAYERS]      = {NULL};

    //For a simple Iron absorber
    //Absorber Box
    G4Box              *AbsLayerSolidFe[MAX_TBECAL_LAYERS]        = {NULL};

    /*the cassette (made of S235)*/
    G4Box *CassetteSolid = new G4Box("EcalCassetteSolid",             EBU_hx, EBU_hy, steel_cassette_hthickness);
    /*the cable-fibre mixture */
    G4Box *CFmix_LayerSolid = new G4Box("EcalCFmix_LayerSolid",       EBU_hx, EBU_hy, cablefibre_mix_hthickness);
    /*a PCB layer*/
    G4Box *PCBLayerSolid = new G4Box("EcalPCBLayerSolid",             EBU_hx, EBU_hy, pcb_hthickness);
    /*3M foil*/
    G4Box *FoilLayerSolid = new G4Box("Ecal3MFoilSolid",              EBU_hx, EBU_hy, foil_hthickness);
    /* Scintillator*/
    G4Box *ScintLayerSolid = new G4Box("EcalScintLayerSolid",         EBU_hx, EBU_hy, poly_hthickness);
    /* airgap*/
    G4Box *AirgapLayerSolid = new G4Box("EcalAirgapLayerSolid",       EBU_hx, EBU_hy, airgap_hthickness);
    /* gap*/
    G4Box *GapLayerSolid = new G4Box("EcalGapLayerSolid",             EBU_hx, EBU_hy, gap_hthickness);

    for (G4int i = 0; i < MAX_TBECAL_LAYERS; ++i)
    {
      //Whole layer
      WholeLayerLogical[i]        = NULL;
      /***empty layer***/
      WholeLayerAirLogical[i]     = NULL;//complete empty layer filled with air

      /*Absorber*/
      AbsLayerLogical[i]          = NULL;

      /**Tungsten**/
      AluminiumframeLogical[i]    = NULL;
      SteelSupportLogical[i]      = NULL;

      /**Iron**/
      CassetteLogical_1[i]        = NULL;//cassette front
      CassetteLogical_2[i]        = NULL;//cassette back
      CFmix_LayerLogical[i]       = NULL;//cable mix fibre
      PCBLayerLogical[i]          = NULL;//PCB
      FoilLayerLogical_1[i]       = NULL;//Foil front
      FoilLayerLogical_2[i]       = NULL;//Foil back
      ScintLayerLogical[i]        = NULL;//Scintillator
      AirgapLayerLogical_1[i]     = NULL;//airgap front
      AirgapLayerLogical_2[i]     = NULL;//airgap back
      GapLayerLogical[i]          = NULL;//gap back filled with air
    }

    for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
    {
      /* ECAL layer thickness */
      if (isTungstenAbs)
      {
        layer_hthickness[iLayer] = poly_hthickness     /*scintillator*/
        + tungsten_hthickness                          /*absorber tungsten*/
        + steel_support_hthickness                     /* steel support */
        + airgap_hthickness                            /*air gap in the front*/
        + airgap_hthickness                            /*air gap in the back*/
        + 2.0*steel_cassette_hthickness                /*2 times steel cassette*/
        + 2.0*foil_hthickness                          /*2 times 3M foil*/
        + pcb_hthickness                               /*PCB*/
        + cablefibre_mix_hthickness                    /*cable-fibre mix*/
        + gap_hthickness;                              /* air gap of 1 mm for ECAL*/
      }
      else if (isIronAbs)
      {
        layer_hthickness[iLayer] = poly_hthickness     /*scintillator*/
        + steel_hthickness                             /*absorber steel s235, 19.0mm*/
        + gap_hthickness                               /* air gap of 1 mm for ECAL*/
        + airgap_hthickness                            /*air gap in the front*/
        + airgap_hthickness                            /*air gap in the back*/
        + 2.0*steel_cassette_hthickness                /*2 times steel cassette*/
        + 2.0*foil_hthickness                          /*2 times 3M foil*/
        + pcb_hthickness                               /*PCB*/
        + cablefibre_mix_hthickness;                   /*cable-fibre mix*/
      }

      //Calorimeter half z
      cal_hz += layer_hthickness[iLayer];

      stringForLayerNo << (iLayer+1);
      G4cout<< "Layer " << iLayer+1 << " thickness: " << layer_hthickness[iLayer]*2 << G4endl;

      /***create a whole layer filled with just air*******/
      WholeLayerAirSolid[iLayer]   = new G4Box("EcalLayerSolid", cal_hx, cal_hy, scinCassette_hthickness);
      WholeLayerAirLogical[iLayer] = new G4LogicalVolume(WholeLayerAirSolid[iLayer], air, G4String("EcalLayerAirLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      WholeLayerAirLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
      
      /*create a whole for:
      air + steel cassette + Cable-Mix + PCB + 3M foil + scintillator + 3M foil + steel cassette + air + gap*/

      /**for normal layer****/
      WholeLayerSolid[iLayer]   = new G4Box("WholeLayerSolid", EBU_hx, EBU_hy, scinCassette_hthickness);
      WholeLayerLogical[iLayer] = new G4LogicalVolume(WholeLayerSolid[iLayer], air, G4String("WholeLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      WholeLayerLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);

      if (isTungstenAbs)
      {
        /*create an absorber plate with an octogonal shape*/
        /*the 90/4 degrees is here needed because by default the octagon is tilted wrt how we want it*/
        AbsLayerSolidW[iLayer] = new G4Polyhedra("AbsLayerSolidW", 90/4*deg, 0,
        Octagonal_absorber_number_of_sides,
        Octagonal_absorber_number_of_layers,
        Octagonal_absorber_z,
        Octagonal_absorber_inner_radious,
        Octagonal_absorber_outer_radious);

        AbsLayerLogical[iLayer] = new G4LogicalVolume(AbsLayerSolidW[iLayer], tungstenalloy, G4String("EcalAbsLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
        G4VisAttributes *AbsLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Blue());
        AbsLayerLogical[iLayer]->SetVisAttributes(AbsLayerLogicalvisAtt);

        /* And the aluminium frame around it*/
        AluminiumframeAll[iLayer]     = new G4Box("AluminiumframeAll", ECAL_Layer_X_dimension, ECAL_Layer_Y_dimension, tungsten_hthickness);
        AluminiumframeSolid[iLayer]   = new G4SubtractionSolid("AluminiumframeAll - AbsLayerSolidW", AluminiumframeAll[iLayer], AbsLayerSolidW[iLayer]);
        AluminiumframeLogical[iLayer] = new G4LogicalVolume(AluminiumframeSolid[iLayer], aluminium, G4String("HcalAluminiumframeLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);

        /*Steel Support*/
        SteelSupportSolid[iLayer] = new G4Box("SteelSupportSolid",cal_hx, cal_hy, steel_support_hthickness);
        SteelSupportLogical[iLayer] = new G4LogicalVolume(SteelSupportSolid[iLayer], S235, G4String("EcalSteelSupportLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);

      }
      else if (isIronAbs)
      {
        /*create an steel absorber plate */
        AbsLayerSolidFe[iLayer]   = new G4Box("AbsLayerSolidFe", cal_hx, cal_hy, steel_hthickness);
        AbsLayerLogical[iLayer]   = new G4LogicalVolume(AbsLayerSolidFe[iLayer], S235, G4String("EcalAbsLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
        G4VisAttributes *AbsLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Blue());
        AbsLayerLogical[iLayer]->SetVisAttributes(AbsLayerLogicalvisAtt);
      }

      /**** airgap ***/
      AirgapLayerLogical_1[iLayer] = new G4LogicalVolume(AirgapLayerSolid, air, G4String("EcalAirgapLogical_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      AirgapLayerLogical_2[iLayer] = new G4LogicalVolume(AirgapLayerSolid, air, G4String("EcalAirgapLogical_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *AirgapLayerLogicalvisAtt_1 = new G4VisAttributes(G4Colour::White());
      G4VisAttributes *AirgapLayerLogicalvisAtt_2 = new G4VisAttributes(G4Colour::White());
      AirgapLayerLogical_1[iLayer]->SetVisAttributes(AirgapLayerLogicalvisAtt_1);
      AirgapLayerLogical_2[iLayer]->SetVisAttributes(AirgapLayerLogicalvisAtt_2);

      /*** gap ****/
      GapLayerLogical[iLayer] = new G4LogicalVolume(GapLayerSolid, air, G4String("EcalGapLogical_1") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *GapLayerLogicalvisAtt = new G4VisAttributes(G4Colour::White());
      GapLayerLogical[iLayer]->SetVisAttributes(GapLayerLogicalvisAtt);

      /****casette layer front***/
      CassetteLogical_1[iLayer] = new G4LogicalVolume(CassetteSolid, S235, G4String("EcalCassetteLogical_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      CassetteLogical_2[iLayer] = new G4LogicalVolume(CassetteSolid, S235, G4String("EcalCassetteLogical_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *CassetteLogicalvisAtt_1 = new G4VisAttributes(G4Colour::Blue());
      G4VisAttributes *CassetteLogicalvisAtt_2 = new G4VisAttributes(G4Colour::Blue());
      CassetteLogical_1[iLayer]->SetVisAttributes(CassetteLogicalvisAtt_1);
      CassetteLogical_2[iLayer]->SetVisAttributes(CassetteLogicalvisAtt_2);

      /*the cable-fibre mixture */
      CFmix_LayerLogical[iLayer] = new G4LogicalVolume(CFmix_LayerSolid, CF_MIX, G4String("EcalCFmix_LayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *CFmix_LayerLogicalvisAtt = new G4VisAttributes(G4Colour::Green());
      CFmix_LayerLogical[iLayer]->SetVisAttributes(CFmix_LayerLogicalvisAtt);

      /*a pcb layer*/
      PCBLayerLogical[iLayer] = new G4LogicalVolume(PCBLayerSolid, PCB, G4String("EcalPCBLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *PCBLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Green());
      PCBLayerLogical[iLayer]->SetVisAttributes(PCBLayerLogicalvisAtt);

      /*3M foil*/
      FoilLayerLogical_1[iLayer] = new G4LogicalVolume(FoilLayerSolid, Polystyrole, G4String("EcalFoilLayerLogical_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      FoilLayerLogical_2[iLayer] = new G4LogicalVolume(FoilLayerSolid, Polystyrole, G4String("EcalFoilLayerLogical_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *FoilLayerLogicalvisAtt_1 = new G4VisAttributes(G4Colour::Red());
      G4VisAttributes *FoilLayerLogicalvisAtt_2 = new G4VisAttributes(G4Colour::Red());
      FoilLayerLogical_1[iLayer]->SetVisAttributes(FoilLayerLogicalvisAtt_1);
      FoilLayerLogical_2[iLayer]->SetVisAttributes(FoilLayerLogicalvisAtt_2);

      /* Scintillator*/
      ScintLayerLogical[iLayer] = new G4LogicalVolume(ScintLayerSolid, poly, G4String("EcalScintLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *ScintLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Cyan());
      ScintLayerLogical[iLayer]->SetVisAttributes(ScintLayerLogicalvisAtt);

      ScintLayerLogical[iLayer]->SetSensitiveDetector(ecalSD);
    }/*end loop over layers*/

    /*the z-position of the of the calorimeter*/
    z_place = z_begin + cal_hz;

    /*create first the whole detector, filled with air*/
    G4Box *DetectorSolid = new G4Box("EcalDetectorSolid", cal_hx, cal_hy, cal_hz);
    DetectorLogical = new G4LogicalVolume(DetectorSolid, air, "EcalDetectorLogical", 0, 0, 0);

    /*------------------------------------------------------------*/

    #ifdef TBSCECAL01_DEBUG
    G4cout<<" In BuildEcalElements(), before PlaceEcalElementsIntoLayer"<<G4endl;
    #endif
    PlaceEcalElementsIntoLayer();
  }

  /*================================================================================*/
  /*                                                                                */
  /*  We have to place the Ecal layers into the world in order to cope              */
  /*  with the various configuration (i.e. impact) angles                           */
  /*                                                                                */
  /*================================================================================*/

  G4bool TBecal4d::PlaceEcalLayers(G4LogicalVolume *WorldLog)
  {
    G4double inverseCosConfigAngle   = 1.0/cos(config_angle);

    /*coordinates of the middle of the layer*/
    G4double lay_x = x_begin;
    G4double lay_y = y_begin;
    G4double lay_z = z_begin + layer_hthickness[0] * inverseCosConfigAngle; // z position of the 1st layer (starts at the middle of a layer)

    G4double steel_support_z = 0;
    G4double absorber_z      = 0;
    G4double scinCassette_z  = 0;

    if (isTungstenAbs)
    {
      steel_support_z = z_begin + steel_support_hthickness * inverseCosConfigAngle;
      absorber_z      = z_begin + (2*steel_support_hthickness + tungsten_hthickness) * inverseCosConfigAngle;
      scinCassette_z  = z_begin + (2*tungsten_hthickness + 2*steel_support_hthickness + scinCassette_hthickness)*inverseCosConfigAngle;
    }
    else if (isIronAbs)
    {
      absorber_z      = z_begin + steel_hthickness * inverseCosConfigAngle;
      scinCassette_z  = z_begin + (2*steel_hthickness + scinCassette_hthickness)*inverseCosConfigAngle;
    }

    G4cout<< "inverseCosConfigAngle: " << inverseCosConfigAngle << G4endl;
    G4cout << setprecision(6);
    G4cout << "x Position of layer: " << lay_x << G4endl;
    G4cout << "y Position of Layer: " << lay_y << G4endl;
    G4cout << "z Position of absorber: " << absorber_z <<"\n"<< G4endl;
    if (isTungstenAbs)
    {
      G4cout << "Z position of steel support: " << steel_support_z <<"\n"<< G4endl;
    }
    G4cout << "z Position of scinCassette: " << scinCassette_z <<"\n"<< G4endl;
    G4cout << "scinCassette_hthickness: "<< scinCassette_hthickness << G4endl;

    /*calculate full ECAL thickness*/
    G4double fullECALThickness = 0;
    for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
    {
      fullECALThickness += 2.0 * layer_hthickness[iLayer] * inverseCosConfigAngle;
    }

    G4cout<< "Total ECAL thickness: " << fullECALThickness << "\n\n" << G4endl;

    /*helpers*/
    G4double deltaSteelSupport = 0;
    G4double deltaAbsorber     = 0;
    G4double deltaScinCassette = 0;

    if (isTungstenAbs) {
      deltaSteelSupport = fullECALThickness/2. - steel_support_hthickness * inverseCosConfigAngle;
      deltaAbsorber     = fullECALThickness/2. - (2.* steel_support_hthickness + tungsten_hthickness)  * inverseCosConfigAngle;

      deltaScinCassette = fullECALThickness/2
      - 2*(tungsten_hthickness + steel_support_hthickness) * inverseCosConfigAngle - scinCassette_hthickness*inverseCosConfigAngle;
    } else if (isIronAbs) {
      deltaAbsorber     = fullECALThickness/2. - steel_hthickness  * inverseCosConfigAngle;

      deltaScinCassette = fullECALThickness/2  - scinCassette_hthickness * inverseCosConfigAngle;
    }

    G4double delta = fullECALThickness/2. - layer_hthickness[0] * inverseCosConfigAngle;

    if (isTungstenAbs)
    {
      G4cout<< "FullECALThickness: " <<fullECALThickness << " deltaAbsorber: " << deltaAbsorber << " deltaSteelSupport: " << deltaSteelSupport<< " deltaScinCassette: " << deltaScinCassette << "\n" << G4endl;
    }
    else if (isIronAbs)
    {
      G4cout << "FullECALThickness: " << fullECALThickness << " deltaAbsorber: " << deltaAbsorber << " deltaScinCassette: " << deltaScinCassette << "\n" << G4endl;
    }

    /*----------------------------------------------------------------------------------*/
    G4double xOffsetAbsorber = 0;
    G4double zOffsetAbsorber = 0;

    G4double xOffsetSteelSupport = 0;
    G4double zOffsetSteelSupport = 0;

    G4double xOffsetScinCassette = 0;
    G4double zOffsetScinCassette = 0;

    G4double xOffset = 0;
    G4double zOffset = 0;

    for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
    {
      if (iLayer >= 1)
      {
        lay_z += layer_hthickness[iLayer - 1] * inverseCosConfigAngle + layer_hthickness[iLayer] * inverseCosConfigAngle;

        if (isTungstenAbs)
        {
          absorber_z      += (tungsten_hthickness + 2.*(scinCassette_hthickness + steel_support_hthickness) + tungsten_hthickness) * inverseCosConfigAngle;
          steel_support_z += (steel_support_hthickness + 2*(tungsten_hthickness +  scinCassette_hthickness) + steel_support_hthickness) * inverseCosConfigAngle;

          scinCassette_z  += (2*steel_support_hthickness + 2. * tungsten_hthickness + 2. * scinCassette_hthickness ) * inverseCosConfigAngle;

          deltaSteelSupport =  deltaSteelSupport - steel_support_hthickness  * inverseCosConfigAngle
          - (iLayer - 1) * scinCassette_hthickness * inverseCosConfigAngle;
          deltaAbsorber     = deltaAbsorber - (2. * steel_support_hthickness + tungsten_hthickness) * inverseCosConfigAngle
          - (iLayer - 1) * scinCassette_hthickness * inverseCosConfigAngle;


          deltaScinCassette = deltaScinCassette - 2. * (tungsten_hthickness +  steel_support_hthickness) * inverseCosConfigAngle
          - (iLayer + 1) * scinCassette_hthickness * inverseCosConfigAngle;

          delta = delta - layer_hthickness[iLayer - 1] * inverseCosConfigAngle - layer_hthickness[iLayer] * inverseCosConfigAngle;
        }
        else if (isIronAbs )
        {
          absorber_z      += 2.*(steel_hthickness + scinCassette_hthickness) * inverseCosConfigAngle;
          scinCassette_z  += 2.*(steel_hthickness + scinCassette_hthickness) * inverseCosConfigAngle;
          deltaAbsorber     = deltaAbsorber - tungsten_hthickness * inverseCosConfigAngle
          - (iLayer - 1) * scinCassette_hthickness * inverseCosConfigAngle;
          deltaScinCassette = deltaScinCassette - 2. * steel_hthickness  * inverseCosConfigAngle
          - (iLayer + 1) * scinCassette_hthickness * inverseCosConfigAngle;

          delta = delta - layer_hthickness[iLayer - 1] * inverseCosConfigAngle - layer_hthickness[iLayer] * inverseCosConfigAngle;
        }
      }

      /* put in rotation*/
      xOffsetAbsorber = deltaAbsorber * sin(rotationAngle);
      zOffsetAbsorber = deltaAbsorber - deltaAbsorber * cos(rotationAngle);

      if (isTungstenAbs)
      {
        xOffsetSteelSupport = deltaSteelSupport * sin(rotationAngle);
        zOffsetSteelSupport = deltaSteelSupport - deltaSteelSupport * cos(rotationAngle);
      }

      xOffsetScinCassette = deltaScinCassette * sin(rotationAngle);
      zOffsetScinCassette = deltaScinCassette - deltaScinCassette * cos(rotationAngle);

      xOffset = delta * sin(rotationAngle);
      zOffset = delta - delta * cos(rotationAngle);

      #ifdef TBSCECAL_DEBUG
      G4cout << "=========================================layer: " << (iLayer+1) << G4endl;
      G4cout<<" xOffsetAbsorber = "<<xOffsetAbsorber<<G4endl;
      G4cout<<" zOffsetAbsorber = "<<zOffsetAbsorber << G4endl;
      if (isTungstenAbs)
      {
        G4cout<<" xOffsetSteelSupport = "<<xOffsetSteelSupport<<G4endl;
        G4cout<<" zOffsetSteelSupport = "<<zOffsetSteelSupport << G4endl;
        G4cout<<" tungsten_hthickness= "<<tungsten_hthickness<<G4endl;
        G4cout<<" steel_support_z = " << steel_support_z<<G4endl;
        G4cout<<" deltaSteelSupport = "<< deltaSteelSupport<<G4endl;
      } else if (isIronAbs)
      {
        G4cout<<" steel_hthickness= "<<steel_hthickness<<G4endl;
      }
      G4cout<<" deltaAbsorber =  "<<deltaAbsorber<<G4endl;
      G4cout<<" deltaScinCassette = "<< deltaScinCassette<<G4endl;
      G4cout<<" 2. * scinCassette_hthickness = "<<2. * scinCassette_hthickness<<G4endl;
      G4cout<<" absorber_z = "<<absorber_z<<G4endl;
      G4cout<<" scinCassette_z = " << scinCassette_z<<G4endl;
      G4cout<<" layer "<<(iLayer+1)<<" lay_x="<<lay_x<<" lay_y="<<lay_y <<" lay_z="<<lay_z<<G4endl<<G4endl;
      G4cout<<endl;
      #endif

      G4ThreeVector translateEcalAbsorber(lay_x + xOffsetAbsorber, lay_y, absorber_z + zOffsetAbsorber);
      G4ThreeVector translateEcalScinCassette(lay_x + xOffsetScinCassette, lay_y, scinCassette_z + zOffsetScinCassette);

      //rotation
      G4RotationMatrix* rotation = new G4RotationMatrix();
      rotation->rotateY(config_angle + rotationAngle);

      std::stringstream stringForLayerNo;
      stringForLayerNo << (iLayer + 1);

      /*place layer into logical volume reserved for the ECAL*/
      if(iLayer >=1 || beginWithAbsorber > 0)
      {
        new G4PVPlacement(rotation, translateEcalAbsorber, AbsLayerLogical[iLayer], G4String("AbsorberLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
        if(isTungstenAbs)
        {
          new G4PVPlacement(rotation, translateEcalAbsorber, AluminiumframeLogical[iLayer], G4String("AluminiumframeLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);

          G4ThreeVector translateEcalSteelSupport(lay_x + xOffsetSteelSupport, lay_y, steel_support_z + zOffsetSteelSupport);
          new G4PVPlacement(rotation, translateEcalSteelSupport, SteelSupportLogical[iLayer], G4String("SteelSupportLayerPhys ") +   G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
        }
      }
      if( sensitiveLayerPatternVector.at(iLayer) == 1 )
      {
        //EBU Horizontal
        new G4PVPlacement(rotation, translateEcalScinCassette, WholeLayerLogical[iLayer], G4String("WholeLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
      }
      else if(sensitiveLayerPatternVector.at(iLayer) == 2 )
      {
        //EBU Transverse !!! Need for rotation here!!!!
        rotation->rotateZ(90*deg);
        new G4PVPlacement(rotation, translateEcalScinCassette, WholeLayerLogical[iLayer], G4String("WholeLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
      }
      else if(sensitiveLayerPatternVector.at(iLayer) == 0 )
      {
        //No Active material
        new G4PVPlacement(rotation, translateEcalScinCassette, WholeLayerAirLogical[iLayer], G4String("WholeLayerAirPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
      }

    }/*-------------- end loop over ECAL layers ----------------------------------------*/

    return true;
  }

  /*================================================================================*/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /*================================================================================*/

  void TBecal4d::SetSD()
  {
    /*Birks law and time cut:
    default values: Ecal_apply_Birks_law = 1, Ecal_time_cut = 150. nsec
    */

    G4int Ecal_apply_Birks_law  = _aGeometryEnvironment.GetParameterAsInt("Ecal_apply_Birks_law");
    G4double Ecal_time_cut      = _aGeometryEnvironment.GetParameterAsDouble("Ecal_time_cut");

    G4double zBeginTemp = 0;
    if (Ecal_time_cut > 0) zBeginTemp = z_begin;
    else zBeginTemp = 0;

    /* create SD */
    ecalSD = new TBSD_VCellecal4d("ecalSD", this, this->GetGridSize(), ncell_x, ncell_y, GetDepthToLayer(), TBECAL, Ecal_apply_Birks_law, Ecal_time_cut, zBeginTemp);

    //register
    RegisterSensitiveDetector(ecalSD);

  }

  /*================================================================================*/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /*================================================================================*/

  void TBecal4d::Print()
  {
    G4cout << "\n  ------> TBecal4d parameters: <---------------------" << G4endl;
    G4cout<<"  ECAL begins at position ("<<G4BestUnit(x_begin, "Length")
    <<", "<<G4BestUnit(y_begin, "Length")<<", "<<G4BestUnit(z_begin, "Length")<<")"<<G4endl;
    G4cout<<"  ECAL dimensions: x="<<G4BestUnit(cal_hx*2, "Length")
    <<", y="<<G4BestUnit(cal_hy*2, "Length")
    <<", z="<<G4BestUnit(cal_hz*2, "Length")
    <<G4endl;
    G4cout<<"  ECAL placed at z="<<G4BestUnit(z_place, "Length")<<G4endl;

    G4cout<<"  Number of ECAL layers:   "<<ECAL_n_layers<<G4endl;
    G4cout<<"  ECAL rotation angle:     "<<G4BestUnit(rotationAngle, "Angle")<<G4endl;
    G4cout<<"  ECAL configuration angle:"<<G4BestUnit(config_angle, "Angle")<<G4endl;
    G4cout<<"  Number of cells in x:    "<<ncell_x<<G4endl;
    G4cout<<"  Number of cells in y:    "<<ncell_y<<G4endl;
    G4cout<<"  Scintillator (poly) thickness:          "<<G4BestUnit(poly_hthickness*2,           "Length")<<G4endl;
    G4cout<<"  Air gap thickness :                      "<<G4BestUnit(airgap_hthickness*2,        "Length")<<G4endl;
    G4cout<<"  Gap thickness :                      "<<G4BestUnit(gap_hthickness*2,        "Length")<<G4endl;
    G4cout<<"  Steel cassette thickness:               "<<G4BestUnit(steel_cassette_hthickness*2, "Length")<<G4endl;
    G4cout<<"  3M foil thickness:                      "<<G4BestUnit(foil_hthickness*2,           "Length")<<G4endl;
    G4cout<<"  PCB plate thickness:                    "<<G4BestUnit(pcb_hthickness*2,            "Length")<<G4endl;
    G4cout<<"  Cable-fibre mix thickness:              "<<G4BestUnit(cablefibre_mix_hthickness*2, "Length")<<G4endl;

    if (isTungstenAbs)
    {
      G4cout << "  steel_support_hthickness:                " <<G4BestUnit(steel_support_hthickness*2,  "Length")<<G4endl;
      G4cout << "  tungsten_density:		          " << G4BestUnit(tungstenalloy_density,"Volumic Mass") << G4endl;
      G4cout << "  tungsten_thickness:		          " << G4BestUnit(tungsten_hthickness*2,"Length") << G4endl;
      G4cout << "  tungsten_core_density:	                  " << G4BestUnit(coretun_density,"Volumic Mass") << G4endl;
      G4cout << "  nikel_density:   	                  " << G4BestUnit(nikel_density,"Volumic Mass") << G4endl;
      G4cout << "  tungsten_core_tungsten_fractionmass:     " << tungsten_fractionmass << G4endl;
      G4cout << "  tungsten_nikel_fractionmass              " << nikel_fractionmass << G4endl;
    }
    else if (isIronAbs)
    {
      G4cout << "  steel_radiator_thickness:	          " << G4BestUnit(steel_hthickness*2,"Length") << G4endl;
    }

    G4cout<<"\n ECAL_n_layers: "<<ECAL_n_layers<<G4endl;
    G4cout << resetiosflags(ios::left);

    G4cout << "       TBecal4d materials          " << G4endl;
    G4cout << "      PCB_density                        " << G4BestUnit(PCB_density,"Volumic Mass") << G4endl;
    G4cout << "      PCB_silicon_fractiomass            " << PCB_silicon_fractiomass << G4endl;
    G4cout << "      PCB_elO_fractionmass               " << PCB_elO_fractionmass << G4endl;
    G4cout << "      PCB_graphite_fractionmass          " << PCB_graphite_fractionmass << G4endl;
    G4cout << "      PCB_elH_fractionmass               " << PCB_elH_fractionmass << G4endl;
    G4cout << "      PCB_elBr_fractionmass              " << PCB_elBr_fractionmass << G4endl;
    G4cout << "      S235_density                       " <<  G4BestUnit(S235_density,"Volumic Mass")<< G4endl;
    G4cout << "      S235_iron_fractionmass             " << S235_iron_fractionmass << G4endl;
    G4cout << "      S235_graphite_fractionmass         " << S235_graphite_fractionmass << G4endl;
    G4cout << "      S235_manganese_fractionmass        " << S235_manganese_fractionmass << G4endl;
    G4cout << "      tungsten_copper_fractionmass       " << copper_fractionmass << G4endl;
    G4cout << "      PVC_density                        " << G4BestUnit(PVC_density,"Volumic Mass") << G4endl;
    G4cout << "      Polystyrole_density                " << G4BestUnit(Polystyrole_density,"Volumic Mass") << G4endl;
    G4cout << "      CF_Mix_density                     " << G4BestUnit(CF_Mix_density,"Volumic Mass") << G4endl;
    G4cout << "      CF_MIX_air_fractionmass            " << CF_MIX_air_fractionmass << G4endl;
    G4cout << "      CF_MIX_PVC_fractionmass            " << CF_MIX_PVC_fractionmass << G4endl;
    G4cout << "      CF_MIX_Polystyrole_fractionmass    " << CF_MIX_Polystyrole_fractionmass << G4endl;

    if (isTungstenAbs)
    {
      G4cout<<"\n OCTAGON"<<G4endl;
      G4cout<<" inner radius 0: "<<Octagonal_absorber_inner_radious[0]<<" 1: "<<Octagonal_absorber_inner_radious[1]<<G4endl;
      G4cout<<" outer radius 0: "<<Octagonal_absorber_outer_radious[0]<<" 1: "<<Octagonal_absorber_outer_radious[1]<<G4endl;
      G4cout<<" z 0: "<<Octagonal_absorber_z[0]<<" 1: "<<Octagonal_absorber_z[1]<<G4endl;
      G4cout<<" number of sides: "<<Octagonal_absorber_number_of_sides <<G4endl;
      G4cout<<" number of layers: "<<Octagonal_absorber_number_of_layers<<G4endl;

      G4cout<<"\n  Layer" <<" Tungsten Absorber thickness"<<" Layer thickness"<<G4endl;
      G4cout<<"  Ecal_radiator_thickness has been updated! "<<G4endl;

      for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
      {
        G4cout<<setw(4)<<(iLayer+1)<<" "
        <<setw(7)<<G4BestUnit(tungsten_hthickness*2, "Length")<<" "
        <<setw(15)<<G4BestUnit(layer_hthickness[iLayer]*2, "Length")<<G4endl;
      }
    }
    else if (isIronAbs)
    {

      G4cout<<"\n  Layer" <<" Iron Absorber thickness"<<" Layer thickness"<<G4endl;
      G4cout<<"  Ecal_radiator_thickness has been updated! "<<G4endl;

      for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
      {
        G4cout<<setw(4)<<(iLayer+1)<<" "
        <<setw(7)<<G4BestUnit(steel_hthickness*2, "Length")<<" "
        <<setw(15)<<G4BestUnit(layer_hthickness[iLayer]*2, "Length")<<G4endl;
      }
    }

    G4String layers, pattern;


    G4cout<<"  ----------------------------------------------------------"<<G4endl;
  }

  /*================================================================================*/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /*================================================================================*/

  void TBecal4d::DefineEcalMaterials() {
    G4Element* elH  = CGAGeometryManager::GetElement("H", true);  /*Hydrogen */
    G4Element* elC  = CGAGeometryManager::GetElement("C", true);  /*Carbon */
    G4Element* elO  = CGAGeometryManager::GetElement("O", true);  /*Oxygen */
    G4Element* elCl = CGAGeometryManager::GetElement("Cl", true); /*Chlorine */
    G4Element* elBr = CGAGeometryManager::GetElement("Br", true); /*Bromine */

    G4String name;
    G4int nel,natoms;

    /* PCB (Printed Circuit Board) Material FR4
    Composition and density found under
    http://pcba10.ba.infn.it/temp/ddd/ma/materials/list.html
    */

    PCB = new G4Material(name="PCB", PCB_density, nel=5);
    PCB->AddMaterial(CGAGeometryManager::GetMaterial("silicon_2.33gccm"),PCB_silicon_fractiomass);
    PCB->AddElement(elO, PCB_elO_fractionmass);
    PCB->AddMaterial(CGAGeometryManager::GetMaterial("graphite"),PCB_graphite_fractionmass);
    PCB->AddElement(elH, PCB_elH_fractionmass);
    PCB->AddElement(elBr, PCB_elBr_fractionmass);

    /*The steel we are going to use in the Ecal: Material S235JR (old name St37)
    Numbers found under
    http://n.ethz.ch/student/zwickers/ download/fs_pe_grundlagen_cyrill.pdf
    */

    S235 = new G4Material(name="S235", S235_density, nel=3);
    S235->AddMaterial(CGAGeometryManager::GetMaterial("iron"), S235_iron_fractionmass);
    S235->AddMaterial(CGAGeometryManager::GetMaterial("graphite"), S235_graphite_fractionmass);
    S235->AddMaterial(CGAGeometryManager::GetMaterial("manganese"), S235_manganese_fractionmass);

    //Tungsten is built with density 17.84*g/cm3 and it has 5.25% of nikel, 1.76% of copper and 92.99% of 19.3gr/cm3 tungsten*/
    G4double a, z;
    double newtungstenalloy_density = 17.84*g/cm3;
    nikel          = new G4Material(name="nikel",   z=28.,  a=58.71*g/mole,  nikel_density);
    tungstenalloy  = new G4Material(name="tungstenalloy", newtungstenalloy_density, nel=3);
    tungstenalloy->AddMaterial(CGAGeometryManager::GetMaterial("copper"),copper_fractionmass);
    tungstenalloy->AddMaterial(CGAGeometryManager::GetMaterial("tungsten_19.3gccm"),tungsten_fractionmass);
    tungstenalloy->AddMaterial(nikel,  nikel_fractionmass);

    //PVC
    //Numbers from http://www.elpac.de/Kunststoffkleinteile/Kleines_Kunststoff-Know-_How/PVC-P/pvc-p.html
    G4Material* PVC = new G4Material(name="PCB", PVC_density, nel=3);
    PVC->AddElement(elH, natoms=3);
    PVC->AddElement(elC, natoms=2);
    PVC->AddElement(elCl, natoms=1);

    //Polystyrole
    /*    Numbers from http://de.wikipedia.org/wiki/Polystyrol
    the structural formula for the Styrene Polymer is C6H5CH=CH2
    The difference to Styropor definition in CGAGeometryManager
    comes since we do not have the material in a foamed form*/
    Polystyrole = new G4Material(name="Polystyrole", Polystyrole_density, nel=2);
    Polystyrole->AddElement(elH, natoms=8);
    Polystyrole->AddElement(elC, natoms=8);

    //CFMix
    /*Now we define the material cf_mix
    We assume the following:
    a) a layer has a volume of V_total = 90x90x0.15 cm^3 = 1215 cm^3(last number is
    longitudinal space reserved for cable fibre mix)
    b) coax cable has diameter of 0.12 cm
    fibre has diameter of 0.5 cm
    The cables are on average 45 cm long
    => V_coax = PI*(0.06 cm)^2*45 cm = 0.510 cm^3
    => V_fibre = PI*(0.025 cm)^2*45 cm = 0.088 cm^3
    ...The rest is occupied by air
    V_air = V_total - V_coax - V_fibre = 1214.402
    There is one coax. cable and one fibre per tile
    and we have on average 185 tiles per layer
    => Total mass of coax cable (fibre), m = density*V
    m_coax = (1.35*0.510)*185 = 127.37 g
    m_fibre = (1.065*0.088)*185 = 17.33 g
    ... and
    m_air = 1214.402*1.29e-03 = 1.45 g
    total density = (m_air + m_coax + m_fibre)/1215. = 0.120 g/cm^3
    */

    CF_MIX = new G4Material(name="Cable Fibre Mix",CF_Mix_density, nel=3);
    CF_MIX->AddMaterial(CGAGeometryManager::GetMaterial("air"), CF_MIX_air_fractionmass);
    CF_MIX->AddMaterial(PVC, CF_MIX_PVC_fractionmass);
    CF_MIX->AddMaterial(Polystyrole, CF_MIX_Polystyrole_fractionmass);

    /*materials*/
    poly      = CGAGeometryManager::GetMaterial("G4_POLYSTYRENE");
    air       = CGAGeometryManager::GetMaterial("air");
    aluminium = CGAGeometryManager::GetMaterial("Aluminium");

    G4cout<<"\n  -----------> TBecal4d material properties <----------------"<<G4endl;
    G4cout<<"  TungstenALLOY: X0 = "<<tungstenalloy->GetRadlen() /cm <<" cm"
    <<",  Lambda_I ="<<tungstenalloy->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;
    G4cout<<"  Tungsten19.3: X0 = "<<CGAGeometryManager::GetMaterial("tungsten_19.3gccm")->GetRadlen() /cm <<" cm"
    <<",  Lambda_I ="<<CGAGeometryManager::GetMaterial("tungsten_19.3gccm")->GetNuclearInterLength()/cm<<" cm"
    <<", density = "<<G4BestUnit(CGAGeometryManager::GetMaterial("tungsten_19.3gccm")->GetDensity(), "Volumic Mass")
    <<G4endl;

    G4cout<<"  Nickel: X0 = "<<nikel->GetRadlen() /cm <<" cm"
    <<",  Lambda_I ="<<nikel->GetNuclearInterLength()/cm<<" cm"
    <<", density = "<<G4BestUnit(nikel->GetDensity(), "Volumic Mass")
    <<G4endl;

    G4cout<<"  Copper: X0 = "<<CGAGeometryManager::GetMaterial("copper")->GetRadlen() /cm <<" cm"
    <<",  Lambda_I ="<<CGAGeometryManager::GetMaterial("copper")->GetNuclearInterLength()/cm<<" cm"
    <<", density = "<<G4BestUnit(CGAGeometryManager::GetMaterial("copper")->GetDensity(), "Volumic Mass")
    <<"\n"<<G4endl;


    G4cout<<"  Scintillator (polystyrene): X0 = "<<poly->GetRadlen() /cm <<" cm"
    <<",  Lambda_I ="<<poly->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;
    G4cout<<"  Steel (ST235):              X0 = "<<S235->GetRadlen()/cm <<" cm"
    <<",  Lambda_I ="<<S235->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;
    G4cout<<"  Air:                        X0 = "<<air->GetRadlen()/cm <<" cm"
    <<",  Lambda_I = "<<air->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;
    G4cout<<"  Cable-fibre mix:            X0 = "<<CF_MIX->GetRadlen()/cm <<" cm"
    <<",  Lambda_I = "<<CF_MIX->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;
    G4cout<<"  PCB material:               X0 = "<<PCB->GetRadlen() /cm <<" cm"
    <<",  Lambda_I = "<<PCB->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;
    G4cout<<"  3M foils (polystyrole):     X0 = "<<Polystyrole->GetRadlen()/cm <<" cm"
    <<",  Lambda_I = "<<Polystyrole->GetNuclearInterLength()/cm<<" cm"
    <<G4endl;

    G4cout<<"  ----------------------------------------------------------\n"<<G4endl;

  }

  /*================================================================================*/
  /*                                                                                */
  /*                                                                                */
  /*                                                                                */
  /*================================================================================*/

  void TBecal4d::PlaceEcalElementsIntoLayer()
  {

    G4double steelCassettePosition1[MAX_TBECAL_LAYERS] = {0.};
    G4double steelCassettePosition2[MAX_TBECAL_LAYERS] = {0.};
    G4double cableFibreMixPosition[MAX_TBECAL_LAYERS]  = {0.};
    G4double pcbPosition[MAX_TBECAL_LAYERS]            = {0.};
    G4double foil3MPosition1[MAX_TBECAL_LAYERS]        = {0.};
    G4double foil3MPosition2[MAX_TBECAL_LAYERS]        = {0.};
    G4double scintillatorPosition[MAX_TBECAL_LAYERS]   = {0.};
    G4double airgapPosition1[MAX_TBECAL_LAYERS]        = {0.};
    G4double airgapPosition2[MAX_TBECAL_LAYERS]        = {0.};
    G4double gapPosition[MAX_TBECAL_LAYERS]            = {0.};

    for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
    {
      /***********************************************************/
      /*---- Put airgap front into the layer ----*/
      airgapPosition1[iLayer] = - scinCassette_hthickness + airgap_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0, airgapPosition1[iLayer]), AirgapLayerLogical_1[iLayer], "EcalAirgapPhys Front", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put cassette front plate into the layer ----*/
      steelCassettePosition1[iLayer] = airgapPosition1[iLayer] + airgap_hthickness + steel_cassette_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0, steelCassettePosition1[iLayer]), CassetteLogical_1[iLayer], "EcalCassettePhys Front", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*------Put the cable-fibre mixture into the layer ------*/
      cableFibreMixPosition[iLayer] = steelCassettePosition1[iLayer] + steel_cassette_hthickness + cablefibre_mix_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0,0, cableFibreMixPosition[iLayer]), CFmix_LayerLogical[iLayer], "EcalCFmix_LayerPhys", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put the PCB into the layer ----*/
      pcbPosition[iLayer] = cableFibreMixPosition[iLayer] + cablefibre_mix_hthickness + pcb_hthickness;
      new G4PVPlacement(0 , G4ThreeVector(0, 0, pcbPosition[iLayer]), PCBLayerLogical[iLayer], "EcalPCBLayerPhys", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put first 3M foil Layer into the complete layer ----*/
      foil3MPosition1[iLayer] = pcbPosition[iLayer] + pcb_hthickness + foil_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0,foil3MPosition1[iLayer]), FoilLayerLogical_1[iLayer], "EcalFoilLayerPhys front", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*--- Put sensitive part (i.e. scintillator plate) into the layer ---*/
      std::stringstream stringForLayerNo;
      stringForLayerNo << (iLayer + 1);

      scintillatorPosition[iLayer] = foil3MPosition1[iLayer] + foil_hthickness + poly_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0, scintillatorPosition[iLayer]), ScintLayerLogical[iLayer], G4String("EcalScintLayerPhys ") + G4String(stringForLayerNo.str()), WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put 3M foil into the layer ----*/
      foil3MPosition2[iLayer] = scintillatorPosition[iLayer] + poly_hthickness + foil_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0 ,0, foil3MPosition2[iLayer]), FoilLayerLogical_2[iLayer], "EcalFoilLayerPhys rear", WholeLayerLogical[iLayer], 0, 1, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put airgap front into the layer ----*/
      gapPosition[iLayer] = foil3MPosition2[iLayer] + foil_hthickness + gap_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0, gapPosition[iLayer]), GapLayerLogical[iLayer], "EcalGapPhys", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put cassette rear plate into the layer ----*/
      steelCassettePosition2[iLayer] = gapPosition[iLayer] + gap_hthickness + steel_cassette_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0, steelCassettePosition2[iLayer]), CassetteLogical_2[iLayer], "EcalCassettePhys Rear", 0, 1, checkForOverlappingVolumes);

      /***********************************************************/
      /*---- Put airgap back into the layer ----*/
      airgapPosition2[iLayer] = steelCassettePosition2[iLayer] + steel_cassette_hthickness + airgap_hthickness;
      new G4PVPlacement(0, G4ThreeVector(0, 0, airgapPosition2[iLayer]), AirgapLayerLogical_2[iLayer], "EcalAirgapPhys Rear", WholeLayerLogical[iLayer], 0, 1, checkForOverlappingVolumes);

    }/*end loop over ECAL layers*/

    #ifdef TBECAL4D_DEBUG
    G4cout<<" \n\n TBecal4d::PlaceEcalElementsIntoLayer() "<<G4endl;
    for (G4int iLayer = 0; iLayer < ECAL_n_layers; ++iLayer)
    {
      G4cout<<"  Layer "<<(iLayer+1)<<G4endl;
      G4cout << "       airgap 1 at:                 " << airgapPosition1[iLayer] << " mm" << G4endl;
      G4cout << "       first steel cassette at:     " << steelCassettePosition1[iLayer] << " mm" << G4endl;
      G4cout << "       cable-fibre mixture at:      " << cableFibreMixPosition[iLayer] << " mm" << G4endl;
      G4cout << "       PCB at:                      " << pcbPosition[iLayer] << " mm" << G4endl;
      G4cout << "       3M foil 1 at:                " << foil3MPosition1[iLayer] << " mm" << G4endl;
      G4cout << "       scintillator at:             " << scintillatorPosition[iLayer] << " mm" << G4endl;
      G4cout << "       3M foil 2 at:                " << foil3MPosition2[iLayer] << " mm" << G4endl;
      G4cout << "       Gap at:                      " << gapPosition[iLayer] << " mm" << G4endl;
      G4cout << "       second steel cassette at:    " << steelCassettePosition2[iLayer] << " mm" << G4endl;
      G4cout << "       airgap 2 at:                 " << airgapPosition2[iLayer] << " mm" << G4endl;

    }

    #endif
  }

  /*================================================================================*/
  /*                                                                                */
  /*     Check if the given string contains a valid sensitive layer pattern,        */
  /*     i.e. if the string has a length equal to the total number of layers        */
  /*     and the string contains '1/2' (instrumented layer) and '0'              */
  /*     (uninstrumented layer)                                                     */
  /*                                                                                */
  /*================================================================================*/

  G4bool TBecal4d::isValidSensitiveLayerPattern(G4String sensitiveLayerPattern)
  {

    if ( ((G4int)sensitiveLayerPattern.length()) != ECAL_n_layers ){
      cerr<<"WRONG NUMBER OF ENTRIES IN THE PATTERN"<<endl;
      return false;
    }

    G4bool valid = true;

    for(G4int i = 0; i < ECAL_n_layers; ++i)
    {
      if ( (sensitiveLayerPattern.data()[i]!='0') && (sensitiveLayerPattern.data()[i]!='1') && (sensitiveLayerPattern.data()[i]!='2'))
      {
        cerr<<"WRONG CHARACTER IN THE PATTERN"<<endl;
        valid = false;
        break;
      }
    }

    return valid;

  }

  /*================================================================================*/
  /*                                                                                */
  /*   Fill a vector of integers with the content of the sensitive layer            */
  /*   pattern string                                                               */
  /*                                                                                */
  /*================================================================================*/

  std::vector<G4int> TBecal4d::getSensitiveLayerPatternVector(G4String sensitiveLayerPattern)
  {
    std::vector<G4int> patternVector;

    for(G4int i = 0; i < ECAL_n_layers; ++i)
    {
      G4String digit(sensitiveLayerPattern.data()[i]);
      patternVector.push_back(std::atoi(digit.data()));
    }

    return patternVector;

  }
