/* For timing studies
v 1.0 2012,01.08
Shaojun Exp.
add absober switch to build either W or Fe.
v1.0 2014,15.09
Marco Ramilli
to simulate TB 2014 setup
*/

#include "Control.hh"
#include "TBhcal4d.hh"
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

#include <sstream>

//#define TBHCAL4D_DEBUG

/*
Geometry driver for test-beam configuration 2014-2015

layer configuration steerable:
0: empty layer, just absorber
1: shower Start Finder, 1 HBU
2: full layer, 4 HBUs

new steerable parameter: n of cells of the Shower Start Finder

new Sensitive Detector for the Shower Start Finder

*/

INSTANTIATE(TBhcal4d)

/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

TBhcal4d::TBhcal4d() : VSubDetectorDriver("TBhcal4d","TBhcal"),
db(0),
_aGeometryEnvironment("","",NULL, NULL),
config_angle(0)
{
  checkForOverlappingVolumes = false;
}

/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

TBhcal4d::~TBhcal4d()
{

}

/*================================================================================*/
/*                                                                                */
/*     Main function                                                              */
/*                                                                                */
/*================================================================================*/

G4bool TBhcal4d::ContextualConstruct(const CGAGeometryEnvironment &aGeometryEnvironment, G4LogicalVolume *WorldLog)
{
  G4cout<<"\n ***************************************************************"<<G4endl;
  G4cout<<" *                                                             *"<<G4endl;
  G4cout<<" *    Build HCAL based on the TBhcal4d driver                  *"<<G4endl;
  G4cout<<" *                                                             *"<<G4endl;
  G4cout<<" ***************************************************************"<<G4endl;

  /*Obtain the pointer to our database via the Environment object*/
  _aGeometryEnvironment = aGeometryEnvironment;
  db = new Database(_aGeometryEnvironment.GetDBName());

  WorldLogical = WorldLog;

  G4cout<<"\n Before FetchMySQLVariables"<<G4endl;
  FetchMySQLVariables();

  G4cout<<"\n before DefineHcalMaterials "<<G4endl;
  DefineHcalMaterials();

  G4cout<<"\n before BuildHcalElements "<<G4endl;
  BuildHcalElements();

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

  for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
  {
    G4double distance = 0; /*distance of this layer from the origin*/
    G4double layerThickness = 0;
    G4double absorberThickness = 0;
    if (isTungstenAbs)
    {
      layerThickness    = (tungsten_hthickness + steel_support_hthickness + scinCassette_hthickness) * 2.;
      absorberThickness = tungsten_hthickness * 2.;
    }
    else if (isIronAbs)
    {
      layerThickness    = (steel_hthickness + scinCassette_hthickness) * 2.;
      absorberThickness = steel_hthickness * 2.;
    }
    G4double cellSize0 = 30. * mm; /*cell size along the x-axis*/
    G4double cellSize1 = 30. * mm; /*cell size along the y-axis*/

    gearParam->layerLayout().positionLayer(distance, layerThickness, cellSize0, cellSize1, absorberThickness);

  }/*end loop over iLayer*/

  /* write parameters to GearManager*/
  MokkaGear* gearMgr = MokkaGear::getMgr() ;
  gearMgr->setHcalEndcapParameters( gearParam ) ;
  /*-------------------------------------------------------------------*/

  G4bool cokay = false;
  G4bool cokay2 = true;
  G4bool cokay3 = true;

  G4bool cokay1 = PlaceHcalLayers(WorldLogical);
  if(Add_Scint > 0)
  cokay2 = BuildScintElements(WorldLogical);
  if(Add_Mat > 0)
  cokay3 = BuildAddElements(WorldLogical);

  if(cokay1 && cokay2 && cokay3)
  cokay = true;

  delete db;
  G4cout << "\nDone building TBhcal4d" << G4endl;

  return cokay;
}


/*================================================================================*/
/*                                                                                */
/*       Fetch HCAL related MySQL variables                                       */
/*                                                                                */
/*================================================================================*/
void TBhcal4d::FetchMySQLVariables()
{

  /* config angle from environment object
  config_angle: angle reflecting the rotations for non normal incidence.*/
  config_angle                     = _aGeometryEnvironment.GetParameterAsDouble("HCAL_configuration_angle");
  config_angle                     = config_angle*deg;
  HCAL_Layer_ncell_x               = _aGeometryEnvironment.GetParameterAsInt("HCAL_layer_ncell_x");
  HCAL_Layer_ncell_y               = _aGeometryEnvironment.GetParameterAsInt("HCAL_layer_ncell_y");
  Hcal_cells_size                  = _aGeometryEnvironment.GetParameterAsDouble("Hcal_cells_size");
  HCAL_n_layers                    = _aGeometryEnvironment.GetParameterAsInt("Hcal_nlayers");

  Hcal_Radiator_Material           = _aGeometryEnvironment.GetParameterAsString("Hcal_radiator_material");

  HCAL_SSFLayer_ncell_x = UserInit::getInstance()->getInt("HCAL_SSFLayer_ncell_x");//temporarly defined as user Defined variables
  HCAL_SSFLayer_ncell_y = UserInit::getInstance()->getInt("HCAL_SSFLayer_ncell_y");//temporarly defined as user Defined variables
  beginWithAbsorber = UserInit::getInstance()->getInt("beginWithAbsorber");//temporarly defined as user Defined variables
  Add_Scint = UserInit::getInstance()->getInt("Add_Scint");
  Add_Mat = UserInit::getInstance()->getInt("Add_Mat");

  //Absorber thickness
  steel_hthickness = 17.2/2.0; //EUDET AHCAL

  if(Hcal_Radiator_Material == "tungsten")
  {
    isTungstenAbs = true;
    isIronAbs     = false;
  }
  else if(Hcal_Radiator_Material == "Iron")
  {
    isTungstenAbs = false;
    isIronAbs     = true;
  }
  else
  Control::Abort("TBHcal4d: invalid radiator material name. \nIt has to be either Iron or WMod!!!",MOKKA_ERROR_BAD_GLOBAL_PARAMETERS);


  /*we are dealing with a simple cell grid
  We only need to know the number of layers, the number of cells in x and z direction
  and the basic grid size to get the layering n
  */

  db->exec("select * from HCAL_tungsten_virt;");
  db->getTuple();

  /* The aluminium frame around the absorber is 1000x1000mm^2 */
  HCAL_Layer_X_dimension           = db->fetchInt("HCAL_Layer_X_dimension")/2;
  HCAL_Layer_Y_dimension           = db->fetchInt("HCAL_Layer_Y_dimension")/2;

  assert(HCAL_n_layers > 0);
  assert(HCAL_Layer_ncell_x >= 0 || HCAL_Layer_ncell_y >= 0);

  /*the beginning of the Hcal USER PARAMETER */
  // x_begin                          = db->fetchDouble("X_position_of_first_HCAL_layer");
  // y_begin                          = db->fetchDouble("Y_position_of_first_HCAL_layer");
  // z_begin                          = db->fetchDouble("Z_position_of_first_HCAL_layer");
  x_begin                          = UserInit::getInstance()->getDouble("X_position_of_first_HCAL_layer");
  y_begin                          = UserInit::getInstance()->getDouble("Y_position_of_first_HCAL_layer");
  z_begin                          = UserInit::getInstance()->getDouble("Z_position_of_first_HCAL_layer");

  /* get 'global' x and y translation from steering file (at run time) or database*/
  G4double XTranslation = _aGeometryEnvironment.GetParameterAsDouble("HcalTranslateX");
  G4double YTranslation = _aGeometryEnvironment.GetParameterAsDouble("HcalTranslateY");
  x_begin += XTranslation;
  y_begin += YTranslation;

  /* get rotation angle from steering file (at run time) or database*/
  rotationAngle = _aGeometryEnvironment.GetParameterAsDouble("HcalRotationAngle");
  rotationAngle = rotationAngle*deg;

  /*Absorber stuff*/
  db->exec("select * from HCAL_tungsten_absorber_virt;");
  db->getTuple();

  Octagonal_absorber_inner_radious[0] = db->fetchDouble("Octagonal_absorber_inner_radious_front");
  Octagonal_absorber_inner_radious[1] = db->fetchDouble("Octagonal_absorber_inner_radious_back");
  Octagonal_absorber_outer_radious[0] = db->fetchDouble("Octagonal_absorber_outer_radious_front");
  Octagonal_absorber_outer_radious[1] = db->fetchDouble("Octagonal_absorber_outer_radious_back");

  //Octagonal_absorber_z[0]             = db->fetchDouble("Octagonal_absorber_z_front");
  //Octagonal_absorber_z[1]             = db->fetchDouble("Octagonal_absorber_z_back");

  Octagonal_absorber_number_of_sides  = db->fetchInt("Octagonal_absorber_number_of_sides");
  Octagonal_absorber_number_of_layers = db->fetchInt("Octagonal_absorber_number_of_layers");

  /* Materials*/
  db->exec("select * from HCAL_tungsten_materials_virt;");
  db->getTuple();

  PCB_density				                   = db->fetchDouble("PCB_density")*g/cm3;
  PCB_silicon_fractiomass		           = db->fetchDouble("PCB_silicon_2.33_fractiomass");
  PCB_elO_fractionmass			           = db->fetchDouble("PCB_elO_fractionmass");
  PCB_graphite_fractionmass		         = db->fetchDouble("PCB_graphite_fractionmass");
  PCB_elH_fractionmass			           = db->fetchDouble("PCB_elH_fractionmass");
  PCB_elBr_fractionmass			           = db->fetchDouble("PCB_elBr_fractionmass");
  S235_density			                   = db->fetchDouble("S235_density")*g/cm3;
  S235_iron_fractionmass		           = db->fetchDouble("S235_iron_fractionmass");
  S235_graphite_fractionmass		       = db->fetchDouble("S235_graphite_fractionmass");
  S235_manganese_fractionmass		       = db->fetchDouble("S235_manganese_fractionmass");
  tungstenalloy_density			           = db->fetchDouble("tungsten_density")*g/cm3;
  coretun_density			                 = db->fetchDouble("tungsten_core_density")*g/cm3;
  tungsten_fractionmass	               = db->fetchDouble("tungsten_core_tungsten_fractionmass");
  nikel_fractionmass		               = db->fetchDouble("tungsten_nikel_fractionmass");
  nikel_density                        = db->fetchDouble("nikel_density")*g/cm3;
  copper_fractionmass		               = db->fetchDouble("tungsten_copper_fractionmass");
  PVC_density				                   = db->fetchDouble("PVC_density")*g/cm3;
  Polystyrole_density			             = db->fetchDouble("Polystyrole_density")*g/cm3;
  CF_Mix_density			                 = db->fetchDouble("CF_Mix_density")*g/cm3;
  CF_MIX_air_fractionmass		           = db->fetchDouble("CF_MIX_air_fractionmass");
  CF_MIX_PVC_fractionmass		           = db->fetchDouble("CF_MIX_PVC_fractionmass");
  CF_MIX_Polystyrole_fractionmass	     = db->fetchDouble("CF_MIX_Polystyrole_fractionmass");

  /* thicknesses*/
  db->exec("select * from HCAL_tungsten_layerthickness_virt;");
  db->getTuple();

  poly_hthickness                       = db->fetchDouble("poly_hthickness")/2;
  tungsten_hthickness                   = db->fetchDouble("tungsten_hthickness")/2;
  steel_support_hthickness              = db->fetchDouble("steel_support_thickness")/2;
  airgap_hthickness                     = UserInit::getInstance()->getDouble("airgap_hthickness")/2; //USER PARAMETER
  steel_cassette_hthickness             = db->fetchDouble("steel_cassette_hthickness")/2.;
  foil_hthickness                       = db->fetchDouble("foil_hthickness")/2.;
  pcb_hthickness                        = db->fetchDouble("pcb_hthickness")/2.;
  cablefibre_mix_hthickness             = db->fetchDouble("cablefibre_mix_hthickness")/2.;

  Octagonal_absorber_z[0]               = (-1) * tungsten_hthickness;
  Octagonal_absorber_z[1]               = tungsten_hthickness;

  /*--------------------------
  scintillator cassette + contents + air gaps:
  -----------------------------------*/
  scinCassette_hthickness =  airgap_hthickness   /* air gap in the front*/
  + airgap_hthickness                            /* air gap in the back*/
  + 2.0*steel_cassette_hthickness                /*2 times steel cassette*/
  + cablefibre_mix_hthickness                    /*cable-fibre mix*/
  + pcb_hthickness                               /*PCB*/
  + 2.0*foil_hthickness                          /*2 times 3M foil*/
  + poly_hthickness;                             /*scintillator*/

  /*---------------------------------------
  Introduce a string of numbers to deal with 2014 layer configuration:
  0--> no HBUs
  1--> 1 central HBU (shower start finder)
  2--> 4 HBUs
  ---------------------------------------*/
  /* set up vector for the sensitive layer pattern*/
  G4String sensitiveLayerPatternFromSteeringFile = _aGeometryEnvironment.GetParameterAsString("Hcal_layer_pattern");
  G4cout << "\n\n\n PATTERN length = " << sensitiveLayerPatternFromSteeringFile.length() << " , " << sensitiveLayerPatternFromSteeringFile << G4endl;

  /*---------------
  if steering command:
  /Mokka/init/globalModelParameter Hcal_layer_pattern 11111111111111111111111111111101010101
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
      G4cout << "'/Mokka/init/HCALLayerPattern' parameter is not valid in steering file. ABORTING MOKKA" << G4endl;
      exit(1);
    }
  }else
  {
    for (G4int i = 0; i < HCAL_n_layers; ++i) sensitiveLayerPatternVector.at(i) = 2;
  }

  //********** scintillator beamline ************

  if(Add_Scint > 0)
  {
    //Get all the variables from the steering file
    z_begin_Scint500x500_front = UserInit::getInstance()->getDouble("z_begin_Scint500x500_front");
    z_begin_Scint500x500_back = UserInit::getInstance()->getDouble("z_begin_Scint500x500_back");
    z_begin_Scint100x100_front = UserInit::getInstance()->getDouble("z_begin_Scint100x100_front");
    z_begin_Scint100x100_back = UserInit::getInstance()->getDouble("z_begin_Scint100x100_back");

    //Dimensions Big scint (50*50 cm2)
    X_dim_Scint500x500 = 500;
    Y_dim_Scint500x500 = 500;

    //Dimensions small scint (10*10 cm2)
    X_dim_Scint100x100 = 100;
    Y_dim_Scint100x100 = 100;

    //Thicknesses
    hthickness_Scint500x500 = UserInit::getInstance()->getDouble("thickness_Scint500x500")/2;
    hthickness_Scint100x100 = UserInit::getInstance()->getDouble("thickness_Scint100x100")/2;
  }

  if(Add_Mat > 0)
  {
    Radiator_AddMaterial = UserInit::getInstance()->getString("Radiator_AddMaterial");
    isLead = false;
    isIron = false;
    isAl = false;

    if(Radiator_AddMaterial == "Lead")
    isLead = true;
    else if(Radiator_AddMaterial == "Iron")
    isIron = true;
    else if(Radiator_AddMaterial == "Aluminium")
    isAl = true;
    else
    {
      G4cout << "'/Mokka/init/userInitString' parameter is not valid in steering file. Should be Lead, Iron or Aluminium! ABORTING MOKKA" << G4endl;
      exit(1);
    }

    z_begin_AddMaterial = UserInit::getInstance()->getDouble("z_begin_AddMaterial");
    hthickness_AddMaterial = UserInit::getInstance()->getDouble("thickness_AddMaterial")/2;

    X_dim_AddMaterial = (G4double) (HCAL_Layer_ncell_x * Hcal_cells_size);
    Y_dim_AddMaterial = (G4double) (HCAL_Layer_ncell_y * Hcal_cells_size);
  }
}

/*================================================================================*/
/*                                                                                */
/*  Define calculated variables, materials, solids, logical volumes               */
/*                                                                                */
/*================================================================================*/

void TBhcal4d::BuildHcalElements()
{
  /*Information needed when hits are processed later on*/
  /*Must be set before calling the constructor of the sensitive detector,
  which is done in SetSD()!*/
  SetDepthToLayer(1);

  /* create and register the sensitive detector before
  defining the sensitive logical volumes!
  */
  SetSD();

  //setting the dimenstion for HBU
  SSF_HBU_X_dimension= (G4double) (HCAL_SSFLayer_ncell_x *Hcal_cells_size);
  SSF_HBU_Y_dimension= (G4double) (HCAL_SSFLayer_ncell_y *Hcal_cells_size);

  /*calorimeter dimensions*/
  cal_hz = 0;
  cal_hx = (G4double) (HCAL_Layer_ncell_x * Hcal_cells_size)/2.;
  cal_hy = (G4double) (HCAL_Layer_ncell_y * Hcal_cells_size)/2.;

  HBU_hx = SSF_HBU_X_dimension/2.;
  HBU_hy = SSF_HBU_Y_dimension/2.;

  std::stringstream stringForLayerNo; /*string to save layer number*/

  //Layer Box
  G4Box              *WholeLayerSolid[MAX_TBHCAL_LAYERS]        = {NULL};
  G4Box              *WholeLayerSolid_SSF[MAX_TBHCAL_LAYERS]     = {NULL};
  G4Box              *WholeLayerAirSolid[MAX_TBHCAL_LAYERS]     = {NULL};

  //For Tungsten
  G4Polyhedra        *AbsLayerSolidW[MAX_TBHCAL_LAYERS]         = {NULL};
  G4Box              *AluminiumframeAll[MAX_TBHCAL_LAYERS]      = {NULL};
  G4SubtractionSolid *AluminiumframeSolid[MAX_TBHCAL_LAYERS]    = {NULL};
  G4Box              *SteelSupportSolid[MAX_TBHCAL_LAYERS]      = {NULL};

  //For a simple Iron absorber
  //Absorber Box
  G4Box              *AbsLayerSolidFe[MAX_TBHCAL_LAYERS]        = {NULL};

  /*the cassette (made of S235)*/
  G4Box *CassetteSolid = new G4Box("HcalCassetteSolid",             cal_hx, cal_hy, steel_cassette_hthickness);
  /*the cable-fibre mixture */
  G4Box *CFmix_LayerSolid = new G4Box("HcalCFmix_LayerSolid",       cal_hx, cal_hy, cablefibre_mix_hthickness);
  /*a PCB layer*/
  G4Box *PCBLayerSolid = new G4Box("HcalPCBLayerSolid",             cal_hx, cal_hy, pcb_hthickness);
  /*3M foil*/
  G4Box *FoilLayerSolid = new G4Box("Hcal3MFoilSolid",              cal_hx, cal_hy, foil_hthickness);
  /* Scintillator*/
  G4Box *ScintLayerSolid = new G4Box("HcalScintLayerSolid",         cal_hx, cal_hy, poly_hthickness);
  /* airgap*/
  G4Box *AirgapLayerSolid = new G4Box("HcalAirgapLayerSolid",       cal_hx, cal_hy, airgap_hthickness);

  /* Smaller version for Shower Start Finder*/
  G4Box *CassetteSolid_SSF = new G4Box("HcalCassetteSolid_SSF",             HBU_hx, HBU_hy, steel_cassette_hthickness);
  G4Box *CFmix_LayerSolid_SSF = new G4Box("HcalCFmix_LayerSolid_SSF",       HBU_hx, HBU_hy, cablefibre_mix_hthickness);
  G4Box *PCBLayerSolid_SSF = new G4Box("HcalPCBLayerSolid_SSF",             HBU_hx, HBU_hy, pcb_hthickness);
  G4Box *FoilLayerSolid_SSF = new G4Box("Hcal3MFoilSolid_SSF",              HBU_hx, HBU_hy, foil_hthickness);
  G4Box *ScintLayerSolid_SSF = new G4Box("HcalScintLayerSolid_SSF",         HBU_hx, HBU_hy, poly_hthickness);
  G4Box *AirgapLayerSolid_SSF = new G4Box("HcalAirgapLayerSolid_SSF",       HBU_hx, HBU_hy, airgap_hthickness);

  for (G4int i = 0; i < MAX_TBHCAL_LAYERS; ++i)
  {
    /**normal layers**/
    WholeLayerLogical[i]        = NULL;
    //SSF
    WholeLayer_SSF_Logical[i]   = NULL;
    /***empty layer***/
    WholeLayerAirLogical[i]     = NULL;

    //Absorber
    AbsLayerLogical[i]          = NULL;

    //Tunsten
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


    /**Shower Start Finder**/
    Cassette_SSF_Logical_1[i]        = NULL;//cassette front
    Cassette_SSF_Logical_2[i]        = NULL;//cassette back
    CFmix_Layer_SSF_Logical[i]       = NULL;//cable mix fibre
    PCBLayer_SSF_Logical[i]          = NULL;//PCB
    FoilLayer_SSF_Logical_1[i]       = NULL;//Foil front
    FoilLayer_SSF_Logical_2[i]       = NULL;//Foil back
    ScintLayer_SSF_Logical[i]        = NULL;//Scintillator
    AirgapLayer_SSF_Logical_1[i]     = NULL;//airgap front
    AirgapLayer_SSF_Logical_2[i]     = NULL;//airgap back
  }

  /*----------------------------------------------------------------------------*/
  for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
  {
    /* HCAL layer thickness */
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
      + cablefibre_mix_hthickness;                   /*cable-fibre mix*/
    }
    else if (isIronAbs)
    {
      layer_hthickness[iLayer] = poly_hthickness     /*scintillator*/
      + steel_hthickness                             /*absorber steel s235, 19mm*/
      + airgap_hthickness                        /*air gap in the front*/
      + airgap_hthickness                        /*air gap in the back*/
      + 2.0*steel_cassette_hthickness                /*2 times steel cassette*/
      + 2.0*foil_hthickness                          /*2 times 3M foil*/
      + pcb_hthickness                               /*PCB*/
      + cablefibre_mix_hthickness;                   /*cable-fibre mix*/
    }

    //Calorimeter half z
    cal_hz += layer_hthickness[iLayer];

    stringForLayerNo << (iLayer+1);
    G4cout << " layer "<< iLayer+ 1<< " thickness: " << layer_hthickness[iLayer]*2 << G4endl;

    /***create a layer filled with just air*******/
    WholeLayerAirSolid[iLayer]   = new G4Box("HcalLayerAirSolid", cal_hx, cal_hy, scinCassette_hthickness);
    WholeLayerAirLogical[iLayer] = new G4LogicalVolume(WholeLayerAirSolid[iLayer], air, G4String("HcalLayerAirLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    WholeLayerAirLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);

    /*create a whole for:
    air + steel cassette + CMF + PCB + 3M foil + scintillator + 3M foil + steel cassette + air*/

    /*create a layer filled with air*/
    WholeLayerSolid[iLayer]   = new G4Box("HcalLayerSolid", cal_hx, cal_hy, scinCassette_hthickness);
    WholeLayerLogical[iLayer] = new G4LogicalVolume(WholeLayerSolid[iLayer], air, G4String("HcalLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    WholeLayerLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);

    /*create a layer filled with air SSF*/
    WholeLayerSolid_SSF[iLayer]   = new G4Box("HcalLayerSolid_SSF", HBU_hx, HBU_hy, scinCassette_hthickness);
    WholeLayer_SSF_Logical[iLayer] = new G4LogicalVolume(WholeLayerSolid_SSF[iLayer], air, G4String("HcalSSFLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    WholeLayer_SSF_Logical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);

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

      AbsLayerLogical[iLayer] = new G4LogicalVolume(AbsLayerSolidW[iLayer], tungstenalloy, G4String("HcalAbsLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *AbsLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Blue());
      AbsLayerLogical[iLayer]->SetVisAttributes(AbsLayerLogicalvisAtt);

      /* And the aluminium frame around it*/
      AluminiumframeAll[iLayer]     = new G4Box("AluminiumframeAll", HCAL_Layer_X_dimension, HCAL_Layer_Y_dimension, tungsten_hthickness);
      AluminiumframeSolid[iLayer]   = new G4SubtractionSolid("AluminiumframeAll - AbsLayerSolidW", AluminiumframeAll[iLayer], AbsLayerSolidW[iLayer]);
      AluminiumframeLogical[iLayer] = new G4LogicalVolume(AluminiumframeSolid[iLayer], aluminium, G4String("HcalAluminiumframeLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      AluminiumframeLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);

      /*Steel Support*/
      SteelSupportSolid[iLayer] = new G4Box("SteelSupportSolid",cal_hx, cal_hy, steel_support_hthickness);
      SteelSupportLogical[iLayer] = new G4LogicalVolume(SteelSupportSolid[iLayer], S235, G4String("HcalSteelSupportLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      SteelSupportLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);

    }
    else if (isIronAbs)
    {
      /*create an steel absorber plate */
      AbsLayerSolidFe[iLayer]   = new G4Box("AbsLayerSolidFe", cal_hx, cal_hy, steel_hthickness);
      AbsLayerLogical[iLayer]   = new G4LogicalVolume(AbsLayerSolidFe[iLayer], S235, G4String("HcalAbsLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
      G4VisAttributes *AbsLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Blue());
      AbsLayerLogical[iLayer]->SetVisAttributes(AbsLayerLogicalvisAtt);
    }

    /**** airgap ***/
    AirgapLayerLogical_1[iLayer] = new G4LogicalVolume(AirgapLayerSolid, air, G4String("HcalAirgapLogical_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    AirgapLayerLogical_2[iLayer] = new G4LogicalVolume(AirgapLayerSolid, air, G4String("HcalAirgapLogical_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //AirgapLayerLogical_1[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    //AirgapLayerLogical_2[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *AirgapLayerLogicalvisAtt_1 = new G4VisAttributes(G4Colour::White());
    G4VisAttributes *AirgapLayerLogicalvisAtt_2 = new G4VisAttributes(G4Colour::White());
    AirgapLayerLogical_1[iLayer]->SetVisAttributes(AirgapLayerLogicalvisAtt_1);
    AirgapLayerLogical_2[iLayer]->SetVisAttributes(AirgapLayerLogicalvisAtt_2);

    /****casette layer front***/
    CassetteLogical_1[iLayer] = new G4LogicalVolume(CassetteSolid, S235, G4String("HcalCassetteLogical_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    CassetteLogical_2[iLayer] = new G4LogicalVolume(CassetteSolid, S235, G4String("HcalCassetteLogical_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //CassetteLogical_1[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    //CassetteLogical_2[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *CassetteLogicalvisAtt_1 = new G4VisAttributes(G4Colour::Blue());
    G4VisAttributes *CassetteLogicalvisAtt_2 = new G4VisAttributes(G4Colour::Blue());
    CassetteLogical_1[iLayer]->SetVisAttributes(CassetteLogicalvisAtt_1);
    CassetteLogical_2[iLayer]->SetVisAttributes(CassetteLogicalvisAtt_2);

    /*the cable-fibre mixture */
    CFmix_LayerLogical[iLayer] = new G4LogicalVolume(CFmix_LayerSolid, CF_MIX, G4String("HcalCFmix_LayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //CFmix_LayerLogical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *CFmix_LayerLogicalvisAtt = new G4VisAttributes(G4Colour::Green());
    CFmix_LayerLogical[iLayer]->SetVisAttributes(CFmix_LayerLogicalvisAtt);

    /*a pcb layer*/
    PCBLayerLogical[iLayer] = new G4LogicalVolume(PCBLayerSolid, PCB, G4String("HcalPCBLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    G4VisAttributes *PCBLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Green());
    PCBLayerLogical[iLayer]->SetVisAttributes(PCBLayerLogicalvisAtt);

    /*3M foil*/
    FoilLayerLogical_1[iLayer] = new G4LogicalVolume(FoilLayerSolid, Polystyrole, G4String("HcalFoilLayerLogical_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    FoilLayerLogical_2[iLayer] = new G4LogicalVolume(FoilLayerSolid, Polystyrole, G4String("HcalFoilLayerLogical_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //FoilLayerLogical_1[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    //FoilLayerLogical_2[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *FoilLayerLogicalvisAtt_1 = new G4VisAttributes(G4Colour::Red());
    G4VisAttributes *FoilLayerLogicalvisAtt_2 = new G4VisAttributes(G4Colour::Red());
    FoilLayerLogical_1[iLayer]->SetVisAttributes(FoilLayerLogicalvisAtt_1);
    FoilLayerLogical_2[iLayer]->SetVisAttributes(FoilLayerLogicalvisAtt_2);

    /* Scintillator*/
    ScintLayerLogical[iLayer] = new G4LogicalVolume(ScintLayerSolid, poly, G4String("HcalScintLayerLogical ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    G4VisAttributes *ScintLayerLogicalvisAtt = new G4VisAttributes(G4Colour::Cyan());
    ScintLayerLogical[iLayer]->SetVisAttributes(ScintLayerLogicalvisAtt);

    ScintLayerLogical[iLayer]->SetSensitiveDetector(hcalSD);

    //**************** Shower Start Finder *********************//////

    /**** airgap ***/
    AirgapLayer_SSF_Logical_1[iLayer] = new G4LogicalVolume(AirgapLayerSolid_SSF, air, G4String("HcalAirgapLogical_SSF_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    AirgapLayer_SSF_Logical_2[iLayer] = new G4LogicalVolume(AirgapLayerSolid_SSF, air, G4String("HcalAirgapLogical_SSF_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //AirgapLayer_SSF_Logical_1[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    //AirgapLayer_SSF_Logical_2[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *AirgapLayer_SSF_LogicalvisAtt_1 = new G4VisAttributes(G4Colour::White());
    G4VisAttributes *AirgapLayer_SSF_LogicalvisAtt_2 = new G4VisAttributes(G4Colour::White());
    AirgapLayer_SSF_Logical_1[iLayer]->SetVisAttributes(AirgapLayer_SSF_LogicalvisAtt_1);
    AirgapLayer_SSF_Logical_2[iLayer]->SetVisAttributes(AirgapLayer_SSF_LogicalvisAtt_2);

    /****casette layer front***/
    Cassette_SSF_Logical_1[iLayer] = new G4LogicalVolume(CassetteSolid_SSF, S235, G4String("HcalCassetteLogical_SSF_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    Cassette_SSF_Logical_2[iLayer] = new G4LogicalVolume(CassetteSolid_SSF, S235, G4String("HcalCassetteLogical_SSF_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //Cassette_SSF_Logical_1[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    //Cassette_SSF_Logical_2[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *Cassette_SSF_LogicalvisAtt_1 = new G4VisAttributes(G4Colour::Blue());
    G4VisAttributes *Cassette_SSF_LogicalvisAtt_2 = new G4VisAttributes(G4Colour::Blue());
    Cassette_SSF_Logical_1[iLayer]->SetVisAttributes(Cassette_SSF_LogicalvisAtt_1);
    Cassette_SSF_Logical_2[iLayer]->SetVisAttributes(Cassette_SSF_LogicalvisAtt_2);

    /*the cable-fibre mixture */
    CFmix_Layer_SSF_Logical[iLayer] = new G4LogicalVolume(CFmix_LayerSolid_SSF, CF_MIX, G4String("HcalCFmix_LayerLogical_SSF ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //CFmix_Layer_SSF_Logical[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *CFmix_Layer_SSF_LogicalvisAtt = new G4VisAttributes(G4Colour::Green());
    CFmix_Layer_SSF_Logical[iLayer]->SetVisAttributes(CFmix_Layer_SSF_LogicalvisAtt);

    /*a pcb layer*/
    PCBLayer_SSF_Logical[iLayer] = new G4LogicalVolume(PCBLayerSolid_SSF, PCB, G4String("HcalPCBLayerLogical_SSF ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    G4VisAttributes *PCBLayer_SSF_LogicalvisAtt = new G4VisAttributes(G4Colour::Green());
    PCBLayer_SSF_Logical[iLayer]->SetVisAttributes(PCBLayer_SSF_LogicalvisAtt);

    /*3M foil*/
    FoilLayer_SSF_Logical_1[iLayer] = new G4LogicalVolume(FoilLayerSolid_SSF, Polystyrole, G4String("HcalFoilLayerLogical_SSF_1 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    FoilLayer_SSF_Logical_2[iLayer] = new G4LogicalVolume(FoilLayerSolid_SSF, Polystyrole, G4String("HcalFoilLayerLogical_SSF_2 ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    //FoilLayer_SSF_Logical_1[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    //FoilLayer_SSF_Logical_2[iLayer]->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *FoilLayer_SSF_LogicalvisAtt_1 = new G4VisAttributes(G4Colour::Red());
    G4VisAttributes *FoilLayer_SSF_LogicalvisAtt_2 = new G4VisAttributes(G4Colour::Red());
    FoilLayer_SSF_Logical_1[iLayer]->SetVisAttributes(FoilLayer_SSF_LogicalvisAtt_1);
    FoilLayer_SSF_Logical_2[iLayer]->SetVisAttributes(FoilLayer_SSF_LogicalvisAtt_2);

    /* Scintillator*/
    ScintLayer_SSF_Logical[iLayer] = new G4LogicalVolume(ScintLayerSolid_SSF, poly, G4String("HcalScintLayerLogical_SSF ") + G4String(stringForLayerNo.str()), 0, 0, 0);
    G4VisAttributes *ScintLayer_SSF_LogicalvisAtt = new G4VisAttributes(G4Colour::Cyan());
    ScintLayer_SSF_Logical[iLayer]->SetVisAttributes(ScintLayer_SSF_LogicalvisAtt);

    ScintLayer_SSF_Logical[iLayer]->SetSensitiveDetector(hcalSD);

  }/*end loop over layers*/

  /*the z-position of the of the calorimeter*/
  z_place = z_begin + cal_hz;

  /*create first the whole detector, filled with air*/
  G4Box *DetectorSolid = new G4Box("HcalDetectorSolid", cal_hx, cal_hy, cal_hz);
  DetectorLogical = new G4LogicalVolume(DetectorSolid, air, "HcalDetectorLogical", 0, 0, 0);

  /*------------------------------------------------------------*/

  G4cout << "In BuildHcalElements(), before PlaceHcalElementsIntoLayer()" << G4endl;
  PlaceHcalElementsIntoLayer();

}

/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

void TBhcal4d::PlaceHcalElementsIntoLayer()
{

  G4double steelCassettePosition1[MAX_TBHCAL_LAYERS] = {0.};
  G4double steelCassettePosition2[MAX_TBHCAL_LAYERS] = {0.};
  G4double cableFibreMixPosition[MAX_TBHCAL_LAYERS]  = {0.};
  G4double pcbPosition[MAX_TBHCAL_LAYERS]            = {0.};
  G4double foil3MPosition1[MAX_TBHCAL_LAYERS]        = {0.};
  G4double foil3MPosition2[MAX_TBHCAL_LAYERS]        = {0.};
  G4double scintillatorPosition[MAX_TBHCAL_LAYERS]   = {0.};
  G4double airgapPosition1[MAX_TBHCAL_LAYERS]        = {0.};
  G4double airgapPosition2[MAX_TBHCAL_LAYERS]        = {0.};

  for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
  {
    /***********************************************************/
    /*---- Put airgap front into the layer ----*/
    airgapPosition1[iLayer] = - scinCassette_hthickness + airgap_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, airgapPosition1[iLayer]), AirgapLayerLogical_1[iLayer], "HcalAirgapPhys Front", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);
    //SSF
    new G4PVPlacement(0, G4ThreeVector(0, 0, airgapPosition1[iLayer]), AirgapLayer_SSF_Logical_1[iLayer], "HcalAirgapPhys SSF Front", WholeLayer_SSF_Logical[iLayer], 0, 0, checkForOverlappingVolumes);

    /***********************************************************/
    /*---- Put scintillator housing front plate into the layer ----*/
    steelCassettePosition1[iLayer] = airgapPosition1[iLayer] + airgap_hthickness + steel_cassette_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, steelCassettePosition1[iLayer]), CassetteLogical_1[iLayer], "HcalCassettePhys Front",
    WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, steelCassettePosition1[iLayer]), Cassette_SSF_Logical_1[iLayer], "HcalCassettePhys SSF Front",
    WholeLayer_SSF_Logical[iLayer], 0, 0, checkForOverlappingVolumes);

    /***********************************************************/
    /*------Put the cable-fibre mixture into the layer ------*/
    cableFibreMixPosition[iLayer] = steelCassettePosition1[iLayer] + steel_cassette_hthickness + cablefibre_mix_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0,0, cableFibreMixPosition[iLayer]), CFmix_LayerLogical[iLayer], "HcalCFmix_LayerPhys", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0,0, cableFibreMixPosition[iLayer]), CFmix_Layer_SSF_Logical[iLayer], "HcalCFmix_LayerPhys SSF",
    WholeLayer_SSF_Logical[iLayer], 0, 0, checkForOverlappingVolumes);

    /***********************************************************/
    /*---- Put the PCB into the layer ----*/
    pcbPosition[iLayer] = cableFibreMixPosition[iLayer] + cablefibre_mix_hthickness + pcb_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, pcbPosition[iLayer]), PCBLayerLogical[iLayer], "HcalPCBLayerPhys", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, pcbPosition[iLayer]), PCBLayer_SSF_Logical[iLayer], "HcalPCBLayerPhys SSF", WholeLayer_SSF_Logical[iLayer], 0, 0, checkForOverlappingVolumes);

    /***********************************************************/
    /*---- Put first 3M foil Layer into the complete layer ----*/
    foil3MPosition1[iLayer] = pcbPosition[iLayer] + pcb_hthickness + foil_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, foil3MPosition1[iLayer]), FoilLayerLogical_1[iLayer], "HcalFoilLayerPhys Front", WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, foil3MPosition1[iLayer]), FoilLayer_SSF_Logical_1[iLayer], "HcalFoilLayerPhys SSF Front", WholeLayer_SSF_Logical[iLayer], 0, 0, checkForOverlappingVolumes);

    /***********************************************************/
    /*--- Put sensitive part (i.e. scintillator plate) into the layer ---*/
    std::stringstream stringForLayerNo;
    stringForLayerNo << (iLayer + 1);

    scintillatorPosition[iLayer] = foil3MPosition1[iLayer] + foil_hthickness + poly_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, scintillatorPosition[iLayer]), ScintLayerLogical[iLayer], G4String("HcalScintLayerPhys ") + G4String(stringForLayerNo.str()), WholeLayerLogical[iLayer], 0, 0, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, scintillatorPosition[iLayer]), ScintLayer_SSF_Logical[iLayer], G4String("HcalScintLayerPhys SSF ") + G4String(stringForLayerNo.str()), WholeLayer_SSF_Logical[iLayer], 0, 0, checkForOverlappingVolumes);

    /***********************************************************/
    /*---- Put 3M foil into the layer ----*/
    foil3MPosition2[iLayer] = scintillatorPosition[iLayer] + poly_hthickness + foil_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0 ,0, foil3MPosition2[iLayer]), FoilLayerLogical_2[iLayer], "HcalFoilLayerPhys Rear", WholeLayerLogical[iLayer], 0, 1, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0 ,0, foil3MPosition2[iLayer]), FoilLayer_SSF_Logical_2[iLayer], "HcalFoilLayerPhys SSF Rear",
    WholeLayer_SSF_Logical[iLayer], 0, 1, checkForOverlappingVolumes);

    /***********************************************************/
    /*---- Put cassette rear plate into the layer ----*/
    steelCassettePosition2[iLayer] = foil3MPosition2[iLayer] + foil_hthickness + steel_cassette_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, steelCassettePosition2[iLayer]), CassetteLogical_2[iLayer], "HcalCassettePhys Rear",
    WholeLayerLogical[iLayer], 0, 1, checkForOverlappingVolumes);
    /*for Shower Start Finder*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, steelCassettePosition2[iLayer]), Cassette_SSF_Logical_2[iLayer], "HcalCassettePhys SSF Rear",
    WholeLayer_SSF_Logical[iLayer], 0, 1, checkForOverlappingVolumes);

    /***********************************************************/
    /*---- Put airgap rear into the layer ----*/
    airgapPosition2[iLayer] = steelCassettePosition2[iLayer] + steel_cassette_hthickness + airgap_hthickness;
    /*for normal layer*/
    new G4PVPlacement(0, G4ThreeVector(0, 0, airgapPosition2[iLayer]), AirgapLayerLogical_2[iLayer], "HcalAirgapPhys Rear", WholeLayerLogical[iLayer], 0, 1, checkForOverlappingVolumes);
    //SSF
    new G4PVPlacement(0, G4ThreeVector(0, 0, airgapPosition2[iLayer]), AirgapLayer_SSF_Logical_2[iLayer], "HcalAirgapPhys SSF Rear", WholeLayer_SSF_Logical[iLayer], 0, 1, checkForOverlappingVolumes);

  }/*end loop over HCAL layers*/


  G4cout<<" \n\n TBhcal4d::PlaceHcalElementsIntoLayer() "<<G4endl;
  for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
  {
    G4cout<<"  Layer "<<(iLayer+1)<<G4endl;
    G4cout << "       airgap 1 at:                 " << airgapPosition1[iLayer] << " mm" << G4endl;
    G4cout << "       first steel cassette at:     " << steelCassettePosition1[iLayer] << " mm" << G4endl;
    G4cout << "       cable-fibre mixture at:      " << cableFibreMixPosition[iLayer] << " mm" << G4endl;
    G4cout << "       PCB at:                      " << pcbPosition[iLayer] << " mm" << G4endl;
    G4cout << "       3M foil 1 at:                " << foil3MPosition1[iLayer] << " mm" << G4endl;
    G4cout << "       scintillator at:             " << scintillatorPosition[iLayer] << " mm" << G4endl;
    G4cout << "       3M foil 2 at:                " << foil3MPosition2[iLayer] << " mm" << G4endl;
    G4cout << "       second steel cassette at:    " << steelCassettePosition2[iLayer] << " mm" << G4endl;
    G4cout << "       airgap 2 at:                 " << airgapPosition2[iLayer] << " mm" << G4endl;
  }
}

/*================================================================================*/
/*                                                                                */
/*  We have to place the Hcal layers into the world in order to cope              */
/*  with the various configuration (i.e. impact) angles                           */
/*                                                                                */
/*================================================================================*/

G4bool TBhcal4d::PlaceHcalLayers(G4LogicalVolume *WorldLog)
{

  G4double inverseCosConfigAngle   = 1.0/cos(config_angle);

  /*coordinates of the middle of the layer*/
  G4double lay_x = x_begin;
  G4double lay_y = y_begin;
  G4double lay_z = z_begin + layer_hthickness[0] * inverseCosConfigAngle;

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

  G4cout<<"inverseCosConfigAngle: "<<inverseCosConfigAngle<<G4endl;
  G4cout << setprecision(6);
  G4cout << "x Position of layer: " << lay_x << G4endl;
  G4cout << "y Position of Layer: " << lay_y << G4endl;
  G4cout << "z Position of absorber: " << absorber_z <<"\n"<< G4endl;

  if (isTungstenAbs)
  {
    G4cout << "Z position of steel support: " << steel_support_z <<"\n"<< G4endl;
  }

  G4cout <<"inverseCosConfigAngle: "<<inverseCosConfigAngle<<G4endl;
  G4cout << "z Position of scinCassette: " << scinCassette_z <<"\n"<< G4endl;
  G4cout <<"scinCassette_hthickness: "<<scinCassette_hthickness<<endl;

  /*calculate full HCAL thickness*/
  G4double fullHCALThickness = 0;
  for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
  {
    fullHCALThickness += 2.0 * layer_hthickness[iLayer] * inverseCosConfigAngle;
  }

  G4cout << "Total HCAL thickness: " << fullHCALThickness << "\n\n" <<endl;

  /*helpers*/
  G4double deltaSteelSupport = 0;
  G4double deltaAbsorber     = 0;
  G4double deltaScinCassette = 0;

  if (isTungstenAbs)
  {
    deltaSteelSupport = fullHCALThickness/2. - steel_support_hthickness * inverseCosConfigAngle;
    deltaAbsorber     = fullHCALThickness/2. - (2.* steel_support_hthickness + tungsten_hthickness)  * inverseCosConfigAngle;

    deltaScinCassette = fullHCALThickness/2
    - 2*(tungsten_hthickness + steel_support_hthickness) * inverseCosConfigAngle - scinCassette_hthickness*inverseCosConfigAngle;
  }
  else if (isIronAbs)
  {
    deltaAbsorber     = fullHCALThickness/2. - steel_hthickness  * inverseCosConfigAngle;
    deltaScinCassette = fullHCALThickness/2  - scinCassette_hthickness * inverseCosConfigAngle;
  }
  G4double delta = fullHCALThickness/2. - layer_hthickness[0] * inverseCosConfigAngle;

  if (isTungstenAbs)
  {
    G4cout<<" FullHCALThickness: "<< fullHCALThickness <<"  deltaAbsorber: "<<deltaAbsorber<<" deltaSteelSupport: "<< deltaSteelSupport<<" deltaScinCassette: "<< deltaScinCassette <<"\n"<<G4endl;
  }
  else if (isIronAbs)
  {
    G4cout<<" FullHCALThickness: "<< fullHCALThickness <<"  deltaAbsorber: "<<deltaAbsorber<<" deltaScinCassette: "<< deltaScinCassette <<"\n"<<G4endl;
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

  for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
  {
    if(iLayer >= 1)
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

    #ifdef TBHCAL4D_DEBUG
    G4cout<<"=========================================layer: "<<(iLayer+1)<<endl;
    G4cout<<" xOffsetAbsorber = "<<xOffsetAbsorber<<G4endl;
    G4cout<<" zOffsetAbsorber = "<<zOffsetAbsorber << G4endl;
    if (isTungstenAbs)
    {
      G4cout<<" xOffsetSteelSupport = "<<xOffsetSteelSupport<<G4endl;
      G4cout<<" zOffsetSteelSupport = "<<zOffsetSteelSupport << G4endl;
      G4cout<<" tungsten_hthickness= "<<tungsten_hthickness<<G4endl;
      G4cout<<" steel_support_z = " << steel_support_z<<G4endl;
      G4cout<<" deltaSteelSupport = "<< deltaSteelSupport<<G4endl;
    }
    else if (isIronAbs)
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

    G4ThreeVector translateHcalAbsorber(lay_x + xOffsetAbsorber, lay_y, absorber_z + zOffsetAbsorber);
    G4ThreeVector translateHcalScinCassette(lay_x + xOffsetScinCassette, lay_y, scinCassette_z + zOffsetScinCassette);

    //rotation
    G4RotationMatrix* rotation = new G4RotationMatrix();
    rotation->rotateY(config_angle + rotationAngle);

    std::stringstream stringForLayerNo;
    stringForLayerNo << (iLayer + 1);

    /*place layer into logical volume reserved for the HCAL*/

    if(iLayer >= 1 || beginWithAbsorber > 0)
    {

      new G4PVPlacement(rotation, translateHcalAbsorber, AbsLayerLogical[iLayer], G4String("AbsorberLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);

      if (isTungstenAbs)
      {
        new G4PVPlacement(rotation, translateHcalAbsorber, AluminiumframeLogical[iLayer], G4String("AluminiumframeLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);

        G4ThreeVector translateHcalSteelSupport(lay_x + xOffsetSteelSupport, lay_y, steel_support_z + zOffsetSteelSupport);
        new G4PVPlacement(rotation, translateHcalSteelSupport, SteelSupportLogical[iLayer], G4String("SteelSupportLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
      }
    }

    if( sensitiveLayerPatternVector.at(iLayer) == 2 )
    {
      new G4PVPlacement(rotation, translateHcalScinCassette, WholeLayerLogical[iLayer], G4String("WholeLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
    }
    else if( sensitiveLayerPatternVector.at(iLayer) == 1 )
    {
      new G4PVPlacement(rotation, translateHcalScinCassette, WholeLayer_SSF_Logical[iLayer], G4String("WholeSSFLayerPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
    }
    else if(sensitiveLayerPatternVector.at(iLayer) == 0 )
    {
      new G4PVPlacement(rotation, translateHcalScinCassette, WholeLayerAirLogical[iLayer], G4String("WholeLayerAirPhys ") + G4String(stringForLayerNo.str()), WorldLog, 0, (iLayer+1), checkForOverlappingVolumes);
    }

  }/*-------------- end loop over HCAL layers ----------------------------------------*/

  return true;
}

/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

void TBhcal4d::SetSD()
{
  /*Birks law and time cut:
  default values: Hcal_apply_Birks_law = 1, Hcal_time_cut = 150. nsec
  */
  G4int Hcal_apply_Birks_law  = _aGeometryEnvironment.GetParameterAsInt("Hcal_apply_Birks_law");
  G4double Hcal_time_cut      = _aGeometryEnvironment.GetParameterAsDouble("Hcal_time_cut");

  G4double zBeginTemp = 0;
  if (Hcal_time_cut > 0) zBeginTemp = z_begin;
  else zBeginTemp = 0;

  /* create SD */
  hcalSD = new TBSD_VCell4d("hcalSD",
  GetGridSize(),
  HCAL_Layer_ncell_x,
  HCAL_Layer_ncell_y,
  GetDepthToLayer(),
  TBHCAL,
  Hcal_apply_Birks_law,
  Hcal_time_cut,
  zBeginTemp);

  //register
  RegisterSensitiveDetector(hcalSD);
}


/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

void TBhcal4d::Print()
{
  G4cout << "\n  ------> TBhcal4d parameters: <---------------------" << G4endl;
  G4cout<<"  HCAL begins at position ("<<G4BestUnit(x_begin, "Length") <<", "<<G4BestUnit(y_begin, "Length")<<", "<<G4BestUnit(z_begin, "Length")<<")"<<G4endl;
  G4cout<<"  HCAL dimensions: x="<<G4BestUnit(cal_hx*2, "Length") <<", y="<<G4BestUnit(cal_hy*2, "Length") <<", z="<<G4BestUnit(cal_hz*2, "Length") <<G4endl;
  G4cout<<"  HCAL placed at z="<<G4BestUnit(z_place, "Length")<<G4endl;

  G4cout<<"  Number of HCAL layers:   "<<HCAL_n_layers<<G4endl;
  G4cout<<"  HCAL rotation angle:     "<<G4BestUnit(rotationAngle, "Angle")<<G4endl;
  G4cout<<"  HCAL configuration angle:"<<G4BestUnit(config_angle, "Angle")<<G4endl;
  G4cout<<"  Number of cells in x:    "<<HCAL_Layer_ncell_x<<G4endl;
  G4cout<<"  Number of cells in z:    "<<HCAL_Layer_ncell_y<<G4endl;
  G4cout<<"  Number of cells in SSF x:    "<<HCAL_SSFLayer_ncell_x<<G4endl;
  G4cout<<"  Number of cells in SSF z:    "<<HCAL_SSFLayer_ncell_y<<G4endl;
  G4cout<<"  HCAL cell size:          "<<Hcal_cells_size<<G4endl;
  G4cout<<"  Scintillator (poly) thickness:          "<<G4BestUnit(poly_hthickness*2,           "Length")<<G4endl;
  G4cout<<"  Air gap thickness:                      "<<G4BestUnit(airgap_hthickness*2,       "Length")<<G4endl;
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

  G4cout<<"\n HCAL_n_layers: "<<HCAL_n_layers<<G4endl;
  G4cout << resetiosflags(ios::left);

  G4cout << " ===========     TBHcal4d materials  ===========      " << G4endl;
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
    G4cout<<"  Hcal_radiator_thickness has been updated! "<<G4endl;

    for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
    {
      G4cout<<setw(4)<<(iLayer+1)<<" "
      <<setw(7)<<G4BestUnit(tungsten_hthickness*2, "Length")<<" "
      <<setw(15)<<G4BestUnit(layer_hthickness[iLayer]*2, "Length")<<G4endl;
    }
  }
  else if (isIronAbs)
  {

    G4cout<<"\n  Layer" <<" Iron Absorber thickness"<<" Layer thickness"<<G4endl;
    G4cout<<"  Hcal_radiator_thickness has been updated! "<<G4endl;

    for (G4int iLayer = 0; iLayer < HCAL_n_layers; ++iLayer)
    {
      G4cout<<setw(4)<<(iLayer+1)<<" "
      <<setw(7)<<G4BestUnit(steel_hthickness*2, "Length")<<" "
      <<setw(15)<<G4BestUnit(layer_hthickness[iLayer]*2, "Length")<<G4endl;
    }
  }

  G4String layers, pattern;

  G4cout<<"  ----------------------------------------------------------"<<G4endl;

  if(Add_Scint > 0)
  {
    G4cout << "\n Scintillator information: " << G4endl
    << " z_begin_Scint500x500_front: "            << G4BestUnit(z_begin_Scint500x500_front, "Length") << G4endl
    << " z_begin_Scint500x500_back: "            << G4BestUnit(z_begin_Scint500x500_back, "Length") << G4endl
    << " X_dim_Scint500x500 " << G4BestUnit(X_dim_Scint500x500, "Length") << ", Y_dim_Scint500x500 " << G4BestUnit(Y_dim_Scint500x500, "Length") << G4endl
    << " thickness_Scint500x500: "            << G4BestUnit(hthickness_Scint500x500*2, "Length") << G4endl
    << " z_begin_Scint100x100_front: "            << G4BestUnit(z_begin_Scint100x100_front, "Length") << G4endl
    << " z_begin_Scint100x100_back: "            << G4BestUnit(z_begin_Scint100x100_back, "Length") << G4endl
    << " X_dim_Scint100x100 " << G4BestUnit(X_dim_Scint100x100, "Length") << ", Y_dim_Scint100x100 " << G4BestUnit(Y_dim_Scint100x100, "Length") << G4endl
    << " thickness_Scint100x100: "            << G4BestUnit(hthickness_Scint100x100*2, "Length") << G4endl
    << G4endl;
  }

  if(Add_Mat > 0)
  {
    G4cout << "\n Additional Material information: " << G4endl
    << " z_begin_AddMaterial: "            << G4BestUnit(z_begin_AddMaterial, "Length") << G4endl
    << " X_dim_AddMaterial " << G4BestUnit(X_dim_AddMaterial, "Length") << ", Y_dim_AddMaterial " << G4BestUnit(Y_dim_AddMaterial, "Length") << G4endl
    << " hthickness_AddMaterial: "            << G4BestUnit(hthickness_AddMaterial*2, "Length") << G4endl;
  }

}

/*================================================================================*/
/*                                                                                */
/*                                                                                */
/*                                                                                */
/*================================================================================*/

void TBhcal4d::DefineHcalMaterials() {
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

  /*The steel we are going to use in the Hcal: Material S235JR (old name St37)
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

  if(isLead)
  mat = CGAGeometryManager::GetMaterial("G4_Pb");
  if(isIron)
  mat = S235;
  if(isAl)
  mat = aluminium;

  G4cout<<"\n  -----------> TBhcal4d material properties <----------------"<<G4endl;
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
/*     Check if the given string contains a valid sensitive layer pattern,        */
/*     i.e. if the string has a length equal to the total number of layers        */
/*     and the string contains only '1' (instrumented layer) and '0'              */
/*     (uninstrumented layer)                                                     */
/*                                                                                */
/*================================================================================*/

G4bool TBhcal4d::isValidSensitiveLayerPattern(G4String sensitiveLayerPattern)
{

  if ( ((G4int)sensitiveLayerPattern.length()) != HCAL_n_layers ){
    cerr<<"WRONG NUMBER OF ENTRIES IN THE PATTERN"<<endl;
    return false;
  }

  G4bool valid = true;

  for(G4int i = 0; i < HCAL_n_layers; ++i)
  {
    if ( (sensitiveLayerPattern.data()[i]!='0') && (sensitiveLayerPattern.data()[i]!='1')&& (sensitiveLayerPattern.data()[i]!='2') )
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

std::vector<G4int> TBhcal4d::getSensitiveLayerPatternVector(G4String sensitiveLayerPattern)
{
  std::vector<G4int> patternVector;

  for(G4int i = 0; i < HCAL_n_layers; ++i)
  {
    G4String digit(sensitiveLayerPattern.data()[i]);
    patternVector.push_back(std::atoi(digit.data()));
  }

  return patternVector;

}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
//********** Trigger Scintillators in beam instrumentation    *****************/

G4bool TBhcal4d::BuildScintElements(G4LogicalVolume *WorldLogical)
{
  G4cout<<"\n ***************************************************************"<<G4endl;
  G4cout<<" *                                                             *"<<G4endl;
  G4cout<<" *    Build Beamline instrumentation AHCAL SPS July 2015 (H2)  *"<<G4endl;
  G4cout<<" *                                                             *"<<G4endl;
  G4cout<<" ***************************************************************"<<G4endl;

  G4bool doScintillator_500x500_front = ScintConstruct(WorldLogical, 0, 0, z_begin_Scint500x500_front, 1);
  G4bool doScintillator_500x500_back = ScintConstruct(WorldLogical, 0, 0, z_begin_Scint500x500_back, 2);
  G4bool doScintillator_100x100_front = ScintConstruct(WorldLogical, 0, 0, z_begin_Scint100x100_front, 3);
  G4bool doScintillator_100x100_back = ScintConstruct(WorldLogical, 0, 0, z_begin_Scint100x100_back, 4);

  G4bool doScintillators = false;
  if (doScintillator_500x500_front && doScintillator_500x500_back && doScintillator_100x100_front && doScintillator_100x100_back) doScintillators = true;

  return doScintillators;
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

G4bool TBhcal4d::ScintConstruct(G4LogicalVolume *WorldLogical, G4double x_place, G4double y_place, G4double z_place, G4int idscintillator)
{
  G4cout << " Building Scintillator " << idscintillator << G4endl;

  /* depth to layer*/
  SetDepthToLayer(1);

  // set Sensitive Detector
  SetSD(idscintillator);

  BuildElements(idscintillator);

  /* do build process*/
  G4bool cokay = PlaceScintElements(WorldLogical, x_place, y_place, z_place, idscintillator);
  return cokay;
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

void TBhcal4d::BuildElements(G4int idscintillator)
{
  sci_hx = X_dim_Scint500x500/2.;
  sci_hy = Y_dim_Scint500x500/2.;
  sci_hz = hthickness_Scint500x500;

  if (idscintillator == 1)
  {
    G4Box *ScintillatorSolid500x500_front = new G4Box("ScintillatorSolid500x500_front", sci_hx, sci_hy, sci_hz);
    ScintillatorLogical500x500_front = new G4LogicalVolume(ScintillatorSolid500x500_front, poly, "ScintillatorLogical500x500_front", 0, 0, 0);
    ScintillatorLogical500x500_front->SetSensitiveDetector(scintSD_500x500_front);
  }
  else if (idscintillator == 2)
  {
    G4Box *ScintillatorSolid500x500_back = new G4Box("ScintillatorSolid500x500_back", sci_hx, sci_hy, sci_hz);
    ScintillatorLogical500x500_back = new G4LogicalVolume(ScintillatorSolid500x500_back, poly, "ScintillatorLogical500x500_back", 0, 0, 0);
    ScintillatorLogical500x500_back->SetSensitiveDetector(scintSD_500x500_back);
  }
  else
  {
    sci_hx = X_dim_Scint100x100/2.;
    sci_hy = Y_dim_Scint100x100/2.;
    sci_hz = hthickness_Scint100x100;

    if(idscintillator == 3)
    {
      G4Box *ScintillatorSolid100x100_front = new G4Box("ScintillatorSolid100x100_front", sci_hx, sci_hy, sci_hz);
      ScintillatorLogical100x100_front = new G4LogicalVolume(ScintillatorSolid100x100_front, poly, "ScintillatorLogical100x100_front", 0, 0, 0);
      ScintillatorLogical100x100_front->SetSensitiveDetector(scintSD_100x100_front);
    }
    else
    {
      G4Box *ScintillatorSolid100x100_back = new G4Box("ScintillatorSolid100x100_back", sci_hx, sci_hy, sci_hz);
      ScintillatorLogical100x100_back = new G4LogicalVolume(ScintillatorSolid100x100_back, poly, "ScintillatorLogical100x100_back", 0, 0, 0);
      ScintillatorLogical100x100_back->SetSensitiveDetector(scintSD_100x100_back);
    }
  }

  G4cout << "\n Dimension of detector box " << G4endl;
  G4cout << " sci_hx: " << sci_hx*2 << " mm " << G4endl;
  G4cout << " sci_hy: " << sci_hy*2 << " mm " << G4endl;
  G4cout << " sci_hz: " << sci_hz   << " mm " << G4endl;
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

G4bool TBhcal4d::PlaceScintElements(G4LogicalVolume *WorldLogical, G4double x_place, G4double y_place, G4double z_place, G4int idscintillator)
{
  G4cout << "\n Placing Scintillator structure: Scint " << idscintillator << G4endl;
  G4cout << " x_place of Scintillator detector " << x_place << G4endl;
  G4cout << " y_place of Scintillator detector " << y_place << G4endl;
  G4cout << " z_place of Scintillator detector " << z_place << G4endl;

  G4ThreeVector translateScint(x_place, y_place, z_place);
  //rotation
  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateY(config_angle);

  G4VisAttributes *sciColour = new G4VisAttributes(G4Colour::Green());
  sciColour->SetVisibility(true);

  if(idscintillator == 1)
  {
    new G4PVPlacement(rotation, translateScint, ScintillatorLogical500x500_front, "ScintillatorPhys500x500_front", WorldLogical, 0, 0, checkForOverlappingVolumes);
    G4cout << "\n Placed Scintillator Scint " << idscintillator << G4endl;
    ScintillatorLogical500x500_front->SetVisAttributes(sciColour);
  }
  else if(idscintillator == 2)
  {
    new G4PVPlacement(rotation, translateScint, ScintillatorLogical500x500_back, "ScintillatorPhys500x500_back", WorldLogical, 0, 0, checkForOverlappingVolumes);
    G4cout << "\n Placed Scintillator Scint " << idscintillator << G4endl;
    ScintillatorLogical500x500_back->SetVisAttributes(sciColour);
  }
  else if(idscintillator == 3)
  {
    new G4PVPlacement(rotation, translateScint, ScintillatorLogical100x100_front, "ScintillatorPhys100x100_front", WorldLogical, 0, 0, checkForOverlappingVolumes);
    G4cout << "\n Placed Scintillator Scint " << idscintillator << G4endl;
    ScintillatorLogical100x100_front->SetVisAttributes(sciColour);
  }
  else
  {
    new G4PVPlacement(rotation, translateScint, ScintillatorLogical100x100_back, "ScintillatorPhys100x100_back", WorldLogical, 0, 0, checkForOverlappingVolumes);
    G4cout << "\n Placed Scintillator Scint " << idscintillator << G4endl;
    ScintillatorLogical100x100_back->SetVisAttributes(sciColour);
  }

  return true;
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

void TBhcal4d::SetSD(G4int idscintillator)
{
  G4String base = "scintSD";
  stringstream s;

  G4int xdim = (G4int) X_dim_Scint500x500;
  G4int ydim = (G4int) Y_dim_Scint500x500;

  if(idscintillator == 1)
  {
    s << base << "_500x500_front";

    /* create SD */
    scintSD_500x500_front = new TBSD_VCell4d(s.str(),
    1.0,
    xdim,
    ydim,
    GetDepthToLayer(),
    TBSCINT,
    0.,
    150,
    0);

    // register
    RegisterSensitiveDetector(scintSD_500x500_front);
  }
  else if(idscintillator == 2)
  {
    s << base << "_500x500_back";

    /* create SD */
    scintSD_500x500_back = new TBSD_VCell4d(s.str(),
    1.0,
    xdim,
    ydim,
    GetDepthToLayer(),
    TBSCINT,
    0.,
    150,
    0);

    // register
    RegisterSensitiveDetector(scintSD_500x500_back);
  }
  else
  {
    xdim = (G4int) X_dim_Scint100x100;
    ydim = (G4int) Y_dim_Scint100x100;

    if(idscintillator == 3)
    {
      s << base << "_100x100_front";

      /* create SD */
      scintSD_100x100_front = new TBSD_VCell4d(s.str(),
      1.0,
      xdim,
      ydim,
      GetDepthToLayer(),
      TBSCINT,
      0.,
      150,
      0);

      // register
      RegisterSensitiveDetector(scintSD_100x100_front);
    }
    else
    {
      s << base << "_100x100_back";

      /* create SD */
      scintSD_100x100_back = new TBSD_VCell4d(s.str(),
      1.0,
      xdim,
      ydim,
      GetDepthToLayer(),
      TBSCINT,
      0.,
      150,
      0);

      // register
      RegisterSensitiveDetector(scintSD_100x100_back);
    }
  }
}

/******************************************************************************/
/*                                                                            */
/*                                                                            */
/*                                                                            */
//*********************** Additional Material *******************************/

G4bool TBhcal4d::BuildAddElements(G4LogicalVolume *WorldLogical)
{
  G4cout<<"\n ***************************************************************"<<G4endl;
  G4cout<<" *                                                             *"<<G4endl;
  G4cout<<" *    Build Additional Material AHCAL SPS July 2015 (H2)       *"<<G4endl;
  G4cout<<" *                                                             *"<<G4endl;
  G4cout<<" ***************************************************************"<<G4endl;

  AddMaterial_hx = X_dim_AddMaterial/2.;
  AddMaterial_hy = Y_dim_AddMaterial/2.;
  AddMaterial_hz = hthickness_AddMaterial;

  G4Box *AddMaterialSolid = new G4Box("AddMaterialSolid", AddMaterial_hx, AddMaterial_hy, AddMaterial_hz);
  AddMaterialLogical = new G4LogicalVolume(AddMaterialSolid, mat, "AddMaterialLogical", 0, 0, 0);

  G4ThreeVector translate(0., 0., z_begin_AddMaterial);
  //rotation
  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateY(config_angle);

  G4VisAttributes *AddMaterialColour = new G4VisAttributes(G4Colour::Grey());
  AddMaterialColour->SetVisibility(true);

  new G4PVPlacement(rotation, translate, AddMaterialLogical, "AddMaterialLogical", WorldLogical, 0, 0, checkForOverlappingVolumes);
  G4cout << "\n Placed Additional Material at " << G4BestUnit(z_begin_AddMaterial, "Length") << G4endl;
  AddMaterialLogical->SetVisAttributes(AddMaterialColour);

  return true;
}
