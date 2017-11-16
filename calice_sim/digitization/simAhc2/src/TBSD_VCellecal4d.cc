// $Header: /Mokka-07-07-p05/source/Geometry/tbeam/src/TBSD_VCell4d.cc,v 1.0 2012/01/06  Shaojun Exp $
// Implemention for AHCAL timing studies.
// Eldwan Brianne
// 19.10.15
// Implementation ecalSD (Only strips in Transverse -- rotation of SD in model needed for Horizontal strips)

// module enums
#include "Control.hh"
#include "TBSD_VCellecal4d.hh"
#include "TBecal4d.hh"
#include "CalHit.hh"
#include "CGADefs.h"
#include "EncoderTB.hh"

#include "G4SDManager.hh"
#include "G4TouchableHandle.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4NavigationHistory.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

// comment-in for verbose debugging output from SD
//#define TBSD_DEBUG 0

TBSD_VCellecal4d::TBSD_VCellecal4d(G4String sdname, VSubDetectorDriver* this_detector, G4double* gridsize, G4int nCellX, G4int nCellY, G4int dLayer, G4int modID, G4int applyBirksLawTemp, G4double ecalTimeCutTemp, G4double zBeginDetectorTemp)

:  VSensitiveDetector(sdname),
HCID(-1),
SDName(sdname),
hitsColl(0)
{

  G4cout << "SD create <" << SDName << ">" << G4endl;

  collectionName.insert(sdname);

  origin = G4ThreeVector();

  hitMap = new TBHitMap;

  if (SDName=="ecalSD") {
    SetGridSize(gridsize);
    SetNCellXY(nCellX, nCellY);
    SetDepthToLayer(dLayer);
    SetModuleID(modID);
    #if TBSD_DEBUG
    G4cout << "grid_sizePtr = " << gridDim[0] << " " << gridDim[1] << G4endl;
    #endif
  }
  else {
    G4cerr << "SDName " << SDName << " is unknown type for module enum." << G4endl;
    Control::Abort("TBSD_VCellecal4d - Unknown SDName",
    MOKKA_ERROR_BAD_SENSITIVE_DETECTOR_NAME);
  }

  // set cal dims derived from above
  assert((*gridDim) > 0);
  assert(ncell_xy[0] > 0 || ncell_xy[1] >0);
  xDim = (G4double) ncell_xy[0] *(gridDim[0]);
  yDim = (G4double) ncell_xy[1] *(gridDim[1]);

  // DEBUG: print parameters
  #ifdef TBSD_DEBUG
  G4cout << "n_cell_x " << ncell_xy[0] << G4endl;
  G4cout << "n_cell_y " << ncell_xy[1] << G4endl;
  G4cout << "gridDim " << gridDim[0] << " " << gridDim[1] << G4endl;
  G4cout << "depthToLayer " << depthToLayer << G4endl;
  G4cout << "cal xDim " << xDim << G4endl;
  G4cout << "cal yDim " << yDim << G4endl << G4endl;
  #endif

  theEncoder = new EncoderTB();
  /*****************************************************************/
  applyBirksLaw = applyBirksLawTemp;
  ecalTimeCut   = ecalTimeCutTemp;

  if (applyBirksLaw != 0)
  {
    #if G4VERSION_NUMBER >= 1001
    emSaturation = new G4EmSaturation(0);
    #else
    emSaturation = new G4EmSaturation();
    #endif
  }
  if (ecalTimeCut > 0)
  zBeginDetector = zBeginDetectorTemp;
  /*****************************************************************/

}

TBSD_VCellecal4d::~TBSD_VCellecal4d()
{
  hitMap->clear();
  delete hitMap;
}


void TBSD_VCellecal4d::Initialize(G4HCofThisEvent* HCE)
{
  #ifdef TBSD_DEBUG
  G4cout << "SD Init: " << SDName << G4endl;
  #endif

  hitsColl = new TBHitsCollection(SDName, collectionName[0]);

  if (HCID < 0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  }

  HCE->AddHitsCollection(HCID, hitsColl);

  #ifdef TBSD_DEBUG
  G4cout << "HCID: " << HCID << G4endl;
  #endif

  hitMap->clear();
}


G4bool TBSD_VCellecal4d::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  #ifdef TBSD_DEBUG
  G4cout << G4endl << "TBSD_VCellecal4d::ProcessHits(): " << SDName << G4endl;
  #endif

  G4double edep;

  if ( (edep=aStep->GetTotalEnergyDeposit()) <= 0 &&
  aStep->GetTrack()->GetDefinition()->GetParticleType() !="geantino") {
    return true;
  }
  /***********************************************************/
  if (applyBirksLaw != 0)
  {
    G4double attenuatedEnergy = this->GetBirksAttenuatedEnergy(aStep);
    #ifdef TBSD_DEBUG
    G4cout << "  engyDeposition: " << edep/keV << " keV"
    << "  response after Birk: "  << attenuatedEnergy/keV << " keV"
    << G4endl;
    #endif
    edep = attenuatedEnergy;
  }
  /***********************************************************/


  // get PreStepPoint
  const G4StepPoint *preStepPoint = aStep->GetPreStepPoint();

  // get TouchableHandle
  G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();

  // get GlobalHitPosition
  G4ThreeVector theGlobalPos = preStepPoint->GetPosition();

  G4ThreeVector thePosition =
  (aStep->GetPreStepPoint()->GetPosition()+
  aStep->GetPostStepPoint()->GetPosition())*0.5;

  #ifdef TBSD_DEBUG
  G4cout << "Global Hit pos " << theGlobalPos << G4endl;
  G4cout << "Post Position " << aStep->GetPostStepPoint()->GetPosition() << G4endl;
  #endif

  // get Layer Number from Touchable based on depthToLayer
  G4int n_lay = theTouchable->GetHistory()->GetVolume(depthToLayer)->GetCopyNo();

  // get global Volume center of active layer
  G4ThreeVector GlobalVolumeCenter = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(origin);

  // get local hit position using touchable with theGlobalPos
  G4ThreeVector LocalHitPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(theGlobalPos);

  G4int xbin = 0;
  G4int ybin = 0;

  //For Vertical Strips (rotation needed for Horizontal strips!!)
  // compute "natural" x and y bins of the local hit
  xbin = int(floor(LocalHitPos.x() / gridDim[0] ));
  ybin = int(floor(LocalHitPos.y() / gridDim[1] ));


  #ifdef TBSD_DEBUG
  G4cout << "Layer: " << n_lay << G4endl;
  G4cout << "xbin " << xbin << G4endl;
  G4cout << "ybin " << ybin << G4endl;
  #endif

  G4double localXPos = 0;
  G4double localYPos = 0;

  //For Vertical Strips (rotation needed for Horizontal strips!!)
  // compute x and y local coord. from natural bins
  localXPos = (double(xbin) + .5) * (gridDim[0]);
  localYPos = (double(ybin) + .5) * (gridDim[1]);

  // set localCellPos from coord. calc
  G4ThreeVector localCellPos(localXPos, localYPos, 0.);

  // compute GlobalCellPos based on touchable with localCellPos
  G4ThreeVector GlobalCellPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(localCellPos);

  // DEBUG: result should be within gridSize
  #ifdef TBSD_DEBUG
  G4ThreeVector checkPos = GlobalCellPos - theGlobalPos;
  G4cout << "checkPos=GlobalCellPos-theGlobalPos <" << checkPos << ">" << G4endl;
  #endif

  #ifdef TBSD_DEBUG
  G4cout << "GlobalCellPos " << GlobalCellPos << G4endl;
  G4cout << "GlobalVolumeCenter " << GlobalVolumeCenter << G4endl;
  G4cout << "LocalHitPos " << LocalHitPos << G4endl;
  #endif

  // use the local coords to calculate the i, j cell IDs
  SetCellID( localXPos, localYPos);

  // incr i ID by 1
  G4int i_x = cellID[0] + 1;

  // incr j ID by 1
  G4int j_y = cellID[1] + 1;

  // pack i, j, k cell coordinates into a unique, int cellid
  int cellId = 0 ;
  cellId |= (  i_x  << 0  ) ;
  cellId |= (  j_y  << 8  ) ;
  cellId |= (  n_lay  << 16  ) ;

  // get the PDG encoding for the particle
  G4int PDG=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  // get the global time
  G4double globalTime = aStep->GetTrack()->GetGlobalTime() ;

  #ifdef TBSD_DEBUG
  G4cout<<"Global Time: "<< globalTime <<G4endl;
  #endif

  G4int parentID = aStep->GetTrack()->GetParentID();
  G4String PDGName = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName();
  //G4int PDGPDG1 = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
  //G4int PDGPDG2 = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  //G4ProcessManager* pm = aStep->GetTrack()->GetDefinition()->GetProcessManager();
  //G4ProcessVector* pv = pm->GetProcessList();


  //for (int idx=0; idx < pv->size(); idx++){
  //  G4cout<<"ProcessList: " << (((*pv)[idx])->GetProcessType()) <<G4endl;
  //}


  #ifdef TBSD_DEBUG
  G4cout<<"parentID: "<< parentID <<" PDGName: "<< PDGName <<" PDG: "<< PDG <<G4endl;
  #endif

  /******************************************************************/
  if (ecalTimeCut > 0 && zBeginDetector != 0)
  {
    G4EventManager* pEventManager= G4EventManager::GetEventManager();
    const G4Event *event = pEventManager->GetConstCurrentEvent();
    //position of primary vertex (i.e. of particle gun...)
    G4double zPrimaryGenerator = event->GetPrimaryVertex()->GetZ0();

    G4double timeOutsideDetector = (zBeginDetector - zPrimaryGenerator)/c_light;
    G4double timeInsideDetector = (globalTime - timeOutsideDetector)*ns;

    #ifdef TBSD_DEBUG
    G4cout<<"zPrimaryGenerator="<<G4BestUnit(zPrimaryGenerator, "Length")<<G4endl;
    G4cout<<"globalTime="<<G4BestUnit(globalTime, "Time")<<G4endl;
    G4cout<<"zBeginDetector="<<G4BestUnit(zBeginDetector, "Length")<<G4endl;
    G4cout<<"c_light="<<c_light<<G4endl;
    G4cout<<"timeOutsideDetector="<<G4BestUnit(timeOutsideDetector, "Time")<<G4endl;
    G4cout<<"timeInsideDetector="<<G4BestUnit(timeInsideDetector, "Time")<<G4endl;
    G4cout<<G4endl;
    #endif
    //do not accept hits in ECAL which are later than Ecal_time_cut
    if (timeInsideDetector > ecalTimeCut) return true;
  }
  /******************************************************************/

  float * sp = 0;
  if(Control::LCIODetailedShowerMode) {
    sp = new float[3];
    sp[0] = thePosition[0];
    sp[1] = thePosition[1];
    sp[2] = thePosition[2];
  }


  // FindHit will ++edep if it finds the cell
  if (!FindHit(cellId, parentID, edep , PDG, globalTime, sp) &&
  !( (i_x > ncell_xy[0]) || (j_y > ncell_xy[1]) ) )
  {

    #ifdef TBSD_DEBUG
    G4cout << "cellId_i before the Hit: " << (cellId & 0xff) << G4endl;
    G4cout << "cellId_j before the Hit: " << ( (cellId >> 8) & 0xff)<< G4endl;
    G4cout << "cellId_k before the Hit: " <<  ( (cellId >> 16) & 0xff) << G4endl;
    #endif

    // assert id w/in valid range
    //assert((cellId & 0xff)<=1000);

    // GM: from now on we'll use EncoderTB , but we'll keep the order
    // in the CellID of the output files as it was in Proto_cellHit
    cell_ids theCode = theEncoder->encode(0, 0,
      (cellId & 0xff),         // i_x
      ((cellId >> 8) & 0xff),  // j_y
      n_lay,
      0);

      // create CellHit object from hit class defined for the Prototype
      CalHit *theHit = new CalHit(moduleID,// MODULE (see enum @ Control.hh)| P
      1,        // WI (glob index i)             | S
      0,        // WJ (glob index j)             | M
      (cellId & 0xff),  // I  (local index i)    | I
      ( (cellId >> 8) & 0xff),// J(local index j)| J
      n_lay,    // layer                         | K
      0,
      GlobalCellPos.x(),// GlobalCellPosX
      GlobalCellPos.y(),// GlobalCellPosY
      GlobalCellPos.z(),// GlobalCellPosZ
      edep,             // edep
      Control::primaryId,// Primary ID
      PDG,               // PDG encoding
      globalTime,        // global time
      theCode,
      sp); //stepPosition


      hitMap->insert(TBHitMapPair(cellId, theHit));
    }

    return true;
  }

  G4bool TBSD_VCellecal4d::FindHit(G4int cellId, G4int parentID, G4double edep, G4int PDG, G4double globalTime, float* sp )
  {
    TBHitMapIterator iter;

    for (iter=hitMap->begin(); iter != hitMap->end(); iter++) {
      if ( ( (iter->first)-cellId) == 0) {
        //(iter->second)->AddEdep( Control::primaryId, PDG, edep, globalTime/ns ) ;
        (iter->second)->AddEdep( parentID, PDG, edep, globalTime/ns, sp ) ;
        return true;
      }
    }
    return false;
  }

  void TBSD_VCellecal4d::SetCellID( G4float localPosX, G4float localPosY){
    // CRP Calculate i_x, j_y position of virtual cell from the
    //     location of the edep and the geometrical parameters
    //     stored in the db, ncell_xz and the grid size

    cellID[0] =  (G4int) floor( ncell_xy[0] * ( localPosX  + xDim / 2. )
    /  xDim ) ;
    cellID[1] =  (G4int) floor( ncell_xy[1] * ( localPosY  + yDim / 2. )
    /  yDim ) ;

    #ifdef TBSD_DEBUG
    if ( (cellID[0] > 89) || (cellID[1] > 89)) G4cout << "WARNING" << G4endl;
    G4cout << "localPosX = " << localPosX << G4endl;
    G4cout << "xDim = " << xDim << G4endl;
    G4cout << "cellID[0] = " << cellID[0] << G4endl;
    G4cout << "localPosY = " << localPosY << G4endl;
    G4cout << "yDim = " << yDim << G4endl;
    G4cout << "cellID[1] = " << cellID[1] << G4endl;
    #endif
  }

  void TBSD_VCellecal4d::SetGridSize(G4double* gridsize)
  {
    gridDim[0] = gridsize[0];
    gridDim[1] = gridsize[1];
  }

  void TBSD_VCellecal4d::SetNCellXY(G4int nx, G4int ny)
  {
    // set n cell
    ncell_xy[0] = nx;
    ncell_xy[1] = ny;
  }

  void TBSD_VCellecal4d::SetDepthToLayer(G4int d)
  {
    depthToLayer = d;
  }

  void TBSD_VCellecal4d::SetModuleID(G4int m)
  {
    moduleID = m;
  }

  G4int* TBSD_VCellecal4d::GetNCellXY()
  {
    // get n cell
    return ncell_xy;
  }

  G4int TBSD_VCellecal4d::GetDepthToLayer()
  {
    return depthToLayer;
  }

  G4int TBSD_VCellecal4d::GetApplyBirksLaw()
  {
    return applyBirksLaw;
  }

  G4double TBSD_VCellecal4d::GetZBeginDetector()
  {
    return zBeginDetector;
  }

  G4double TBSD_VCellecal4d::GetEcalTimeCut()
  {
    return ecalTimeCut;
  }

  void TBSD_VCellecal4d::EndOfEvent(G4HCofThisEvent* HCE)
  {
    TBHitMapIterator iter;

    for ( iter=hitMap->begin();
    iter != hitMap->end();
    ++iter) {
      hitsColl->insert(iter->second);
    }

    hitMap->clear();

    HCE->AddHitsCollection(HCID, hitsColl);
  }

  G4double TBSD_VCellecal4d::GetBirksAttenuatedEnergy(const G4Step* aStep)
  {
    G4double energyDeposition = aStep->GetTotalEnergyDeposit();
    G4double length = aStep->GetStepLength();
    G4double niel = aStep->GetNonIonizingEnergyDeposit();
    const G4Track* track = aStep->GetTrack();
    const G4ParticleDefinition* particle = track->GetDefinition();
    const G4MaterialCutsCouple* couple = track->GetMaterialCutsCouple();

    G4double engyVis = emSaturation->VisibleEnergyDeposition(particle,
      couple,
      length,
      energyDeposition,
      niel);

      return engyVis;
    }
