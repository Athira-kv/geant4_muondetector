#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"
#include "SensitiveDetector.hh"
#include "G4Tubs.hh"

#include "DCConfig.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    G4NistManager* nist = G4NistManager::Instance();
    
    // ================= MATERIALS =================
    
    // World material (vacuum)
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic"); // vacuum
    
    // Scintillator material
    G4Material* scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    
    // Drift chamber gas: Ar + CH4 (90% Ar, 10% CH4)
    // Get base materials
    G4Material* Ar = nist->FindOrBuildMaterial("G4_Ar");
    G4Material* CH4 = nist->FindOrBuildMaterial("G4_METHANE");
    
    // Create gas mixture
    G4double density = 0.90 * Ar->GetDensity() + 0.10 * CH4->GetDensity();
    G4Material* driftGas = new G4Material("ArCH4", density, 2);
    driftGas->AddMaterial(Ar, 0.90);   // 90% Argon by mass
    driftGas->AddMaterial(CH4, 0.10);  // 10% Methane by mass

    // ================= WORLD VOLUME =================
    
    G4double worldSize = 5*m;
    G4Box* solidWorld = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
    fWorldPhys = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);
    
    // ================= SCINTILLATOR DETECTORS =================

    // Scintillator dimensions
    G4double scintX = 1.3*m;
    G4double scintY = 0.3*m;
    G4double scintZ = 16*cm;

    // Gap between scintillators (outer edge to outer edge)
    G4double scint_gap = 2.4*m;

    // Calculate center positions
    G4double scint1_z = scint_gap/2 + scintZ/2;   // 1.2m + 8cm  = 1.28 m 

    G4double scint3_z = -(scint_gap/2 + scintZ/2); // -1.28 m

    G4double scint2_z = scint1_z - scintZ - 5.0*cm; // 1.28m - 0.16m - 0.05m = 1.07m
    
    // Create scintillator solid
    G4Box* solidScint = new G4Box("Scintillator", scintX/2, scintY/2, scintZ/2);

    // Logical volumes for scintillators
    G4LogicalVolume* logicScint1 = new G4LogicalVolume(solidScint, scintMat, "Scintillator1");
    G4LogicalVolume* logicScint2 = new G4LogicalVolume(solidScint, scintMat, "Scintillator2");
    G4LogicalVolume* logicScint3 = new G4LogicalVolume(solidScint, scintMat, "Scintillator3");
    
    // Place scintillators
    new G4PVPlacement(0, G4ThreeVector(0, 0, scint1_z), logicScint1,
		      "Scintillator1", logicWorld, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0, 0, scint2_z), logicScint2,
		      "Scintillator2", logicWorld, false, 1);

    new G4PVPlacement(0, G4ThreeVector(0, 0, scint3_z), logicScint3,
		      "Scintillator3", logicWorld, false, 2);

    // ================= DRIFT CHAMBERS =================

    // parametrized DC separation for resolution study

    //G4double dc_separation = 10*cm;
    G4double dc_separation = DC_SEPARATION_CM * cm;
    G4double sc_dc_gap = 5.0* cm;
    G4double obj_height = 100.0* cm;
    
    // Drift chamber dimensions
    G4double driftX = 20*cm;
    G4double driftY = 20*cm;
    G4double driftZ = 13.0*cm;

    // Create drift chamber solid
    G4Box* solidDrift = new G4Box("DriftChamber", driftX/2, driftY/2, driftZ/2);

    // Logical volumes for drift chambers
    G4LogicalVolume* logicDrift1 = new G4LogicalVolume(solidDrift, driftGas, "DriftChamber1");
    G4LogicalVolume* logicDrift2 = new G4LogicalVolume(solidDrift, driftGas, "DriftChamber2");
    G4LogicalVolume* logicDrift3 = new G4LogicalVolume(solidDrift, driftGas, "DriftChamber3");
    G4LogicalVolume* logicDrift4 = new G4LogicalVolume(solidDrift, driftGas, "DriftChamber4");

    /// for different studies we keep different detectors fixed.
    // a) for studying the rate as a function of DC sep, fix the inner DC and move the outer DC apart
    // b) for studying the resolution as a function of DC sep, fix the outer DC, and move the inner DC


    //**************************************************************************************************************************///
    // for a) the following setup works
    
    //fix DC2 at z = 50cm and DC3 at z = -50 cm for object height = 100cm
    G4double drift2_z = driftZ/2 + obj_height/2; // 6.5 + 50 cm = 0.565 m
    G4double drift3_z = -driftZ/2 - obj_height/2; // -50cm - 6.5 cm = -0.565 m 
    
    // DC1 separated above DC2 by dc sep
    G4double drift1_z = drift2_z + driftZ + dc_separation; // 0.565 + 0.13 + sep
    // DC4 moves down below DC3 by dc sep
    G4double drift4_z = drift3_z - driftZ - dc_separation; // -0.565 - 0.13 - sep

    std::cout<<" dc separation = "<<dc_separation<<std::endl;
    // the max separation between DC1 and DC4 should not be closer than 5cm from the sc1 and sc2 respectively, also consider thickness 
    if (  ( (scint1_z - drift1_z) < 20.0  || (drift4_z - scint3_z) < 20.0)){
      std::cout<<" too close to scintillators "<<std::endl;
      drift1_z = scint1_z - scintZ/2 - sc_dc_gap - driftZ/2; // 1.085
      drift4_z = scint3_z + scintZ/2 + sc_dc_gap + driftZ/2;//-1.085
    }	 
    //*************************************************************************************************************************////


    // Place drift chambers
    new G4PVPlacement(0, G4ThreeVector(0, 0, drift1_z), logicDrift1,
		      "DriftChamber1", logicWorld, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0, 0, drift2_z), logicDrift2,
		      "DriftChamber2", logicWorld, false, 1);

    new G4PVPlacement(0, G4ThreeVector(0, 0, drift3_z), logicDrift3,
		      "DriftChamber3", logicWorld, false, 2);

    new G4PVPlacement(0, G4ThreeVector(0, 0, drift4_z), logicDrift4,
		      "DriftChamber4", logicWorld, false, 3);


    /*
    // ================= TARGET OBJECT (for muon tomography) =================
    // Hollow container with cylindrical samples of different Z materials

    // Container material (thin aluminum shell)
    G4Material* containerMat = nist->FindOrBuildMaterial("G4_Al");

    // Sample materials - different Z values for contrast
    G4Material* leadMat = nist->FindOrBuildMaterial("G4_Pb");      // Z=82, high-Z
    G4Material* tungsten = nist->FindOrBuildMaterial("G4_W");      // Z=74, high-Z
    G4Material* iron = nist->FindOrBuildMaterial("G4_Fe");         // Z=26, medium-Z
    G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");     // Z=13, low-Z
    G4Material* carbon = nist->FindOrBuildMaterial("G4_C");        // Z=6, very low-Z
    G4Material* uranium = nist->FindOrBuildMaterial("G4_U");       // Z=92, very high-Z (if you want to detect SNM)

    // ================= HOLLOW CONTAINER =================
    // Outer dimensions of container
    G4double containerX = 20*cm;
    G4double containerY = 20*cm; 
    G4double containerZ = 30*cm;  // Height along beam axis (shorter for slice imaging)
    G4double wallThickness = 3*mm;

    // Outer box
    G4Box* solidContainerOuter = new G4Box("ContainerOuter", 
					   containerX/2, 
					   containerY/2, 
					   containerZ/2);

    // Inner box (air cavity)
    G4Box* solidContainerInner = new G4Box("ContainerInner", 
					   containerX/2 - wallThickness, 
					   containerY/2 - wallThickness, 
					   containerZ/2 - wallThickness);

    // Create container logical volume
    G4LogicalVolume* logicContainerOuter = new G4LogicalVolume(solidContainerOuter, 
							       containerMat, 
							       "Container");

    // Create air-filled interior
    G4LogicalVolume* logicContainerInner = new G4LogicalVolume(solidContainerInner, 
							       worldMat, 
							       "ContainerInterior");

    // Place container at origin
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), 
		      logicContainerOuter, "Container", logicWorld, false, 0);

    // Place air interior inside container
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), 
		      logicContainerInner, "ContainerInterior", logicContainerOuter, false, 0);

    // ================= CYLINDRICAL SAMPLES =================
    // Arranged in a pattern visible in XY slice (like Figure 2)
    // Cylinders oriented along Z axis (vertical, along muon direction)

    G4double cylRadius = 2.0*cm;   // Sample radius
    G4double cylHeight = containerZ - 2*wallThickness - 1*cm;  // Slightly shorter than container

    // Define cylinder positions and materials
    // Arranged in a grid pattern for clear XY slice visualization
    struct CylinderSample {
      G4Material* mat;
      G4ThreeVector pos;
      G4String name;
      G4Colour color;
      G4double radius;  // Allow different radii
    };

    // Configuration 1: Simple 4-cylinder arrangement (like Figure 2a)
    // Good for testing basic reconstruction
    CylinderSample samples_config1[] = {
      {tungsten,  G4ThreeVector(-5*cm,  5*cm, 0), "Sample_W",   G4Colour(0.4, 0.4, 0.4, 1.0), 2.0*cm},
      {leadMat,   G4ThreeVector( 5*cm,  5*cm, 0), "Sample_Pb",  G4Colour(0.2, 0.2, 0.6, 1.0), 2.0*cm},
      {iron,      G4ThreeVector(-5*cm, -5*cm, 0), "Sample_Fe",  G4Colour(0.6, 0.3, 0.1, 1.0), 2.0*cm},
      {aluminum,  G4ThreeVector( 5*cm, -5*cm, 0), "Sample_Al",  G4Colour(0.8, 0.8, 0.8, 1.0), 2.0*cm}
    };
    int nSamples_config1 = 4;

    // Configuration 2: More complex arrangement with different sizes
    // Better for testing algorithm robustness
    CylinderSample samples_config2[] = {
      {uranium,   G4ThreeVector( 0*cm,  6*cm, 0), "Sample_U",   G4Colour(0.0, 0.8, 0.0, 1.0), 1.5*cm},  // Center top - uranium (SNM)
      {tungsten,  G4ThreeVector(-6*cm,  0*cm, 0), "Sample_W",   G4Colour(0.4, 0.4, 0.4, 1.0), 2.0*cm},  // Left
      {leadMat,   G4ThreeVector( 6*cm,  0*cm, 0), "Sample_Pb",  G4Colour(0.2, 0.2, 0.6, 1.0), 2.0*cm},  // Right
      {iron,      G4ThreeVector(-4*cm, -5*cm, 0), "Sample_Fe",  G4Colour(0.6, 0.3, 0.1, 1.0), 1.5*cm},  // Bottom left
      {aluminum,  G4ThreeVector( 4*cm, -5*cm, 0), "Sample_Al",  G4Colour(0.8, 0.8, 0.8, 1.0), 2.5*cm},  // Bottom right - larger
      {carbon,    G4ThreeVector( 0*cm,  0*cm, 0), "Sample_C",   G4Colour(0.1, 0.1, 0.1, 1.0), 1.0*cm}   // Center - small carbon
    };
    int nSamples_config2 = 6;

    // ===== SELECT CONFIGURATION HERE =====
    // Change to samples_config2 and nSamples_config2 for more complex setup
    auto* samples = samples_config1;
    int nSamples = nSamples_config1;

    // Create and place cylinder samples
    for (int i = 0; i < nSamples; i++) {
      G4Tubs* solidCyl = new G4Tubs(samples[i].name, 
				    0,                    // inner radius
				    samples[i].radius,    // outer radius
				    cylHeight/2,          // half height
				    0,                    // start angle
				    360*deg);             // spanning angle
    
    G4LogicalVolume* logicCyl = new G4LogicalVolume(solidCyl, 
                                                     samples[i].mat, 
                                                     samples[i].name);
    
    // Place inside the air-filled container interior
    new G4PVPlacement(0, 
                      samples[i].pos, 
                      logicCyl, 
                      samples[i].name, 
                      logicContainerInner,  // Mother volume is the air interior
                      false, 
                      i);
    
    // Visualization
    G4VisAttributes* cylVis = new G4VisAttributes(samples[i].color);
    cylVis->SetForceSolid(true);
    logicCyl->SetVisAttributes(cylVis);
    
    G4cout << "Placed sample: " << samples[i].name 
           << " at (" << samples[i].pos.x()/cm << ", " 
           << samples[i].pos.y()/cm << ", " 
           << samples[i].pos.z()/cm << ") cm"
           << " with radius " << samples[i].radius/cm << " cm" << G4endl;
    }

    // ================= CONTAINER VISUALIZATION =================
    // Container walls - semi-transparent blue
    G4VisAttributes* containerVis = new G4VisAttributes(G4Colour(0.3, 0.3, 0.8, 0.3));
    containerVis->SetForceSolid(true);
    logicContainerOuter->SetVisAttributes(containerVis);

    // Container interior (air) - invisible or very faint
    G4VisAttributes* interiorVis = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.05));
    interiorVis->SetForceSolid(false);
    logicContainerInner->SetVisAttributes(interiorVis);

    G4cout << "\n=== Muon Tomography Target Configuration ===" << G4endl;
    G4cout << "Container: " << containerX/cm << " x " << containerY/cm << " x " << containerZ/cm << " cm" << G4endl;
    G4cout << "Wall thickness: " << wallThickness/mm << " mm (Aluminum)" << G4endl;
    G4cout << "Number of samples: " << nSamples << G4endl;
    G4cout << "============================================\n" << G4endl;
    */
        
    // ================= VISUALIZATION ATTRIBUTES =================
    
    // World - invisible
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
    
    // Scintillator 1 - cyan, 50% transparent
    G4VisAttributes* scint1Vis = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.5));
    scint1Vis->SetForceSolid(true);
    logicScint1->SetVisAttributes(scint1Vis);
    
    // Scintillator 2 - magenta, 50% transparent
    G4VisAttributes* scint2Vis = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0, 0.5));
    scint2Vis->SetForceSolid(true);
    logicScint2->SetVisAttributes(scint2Vis);

    // Scintillator 3 - cyan, 50% transparent                                                            
    G4VisAttributes* scint3Vis = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.5));
    scint3Vis->SetForceSolid(true);
    logicScint3->SetVisAttributes(scint3Vis);

    
    // Drift chamber 1 - yellow, 50% transparent
    G4VisAttributes* drift1Vis = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.5));
    drift1Vis->SetForceSolid(true);
    logicDrift1->SetVisAttributes(drift1Vis);
    // Drift chamber 2 - yellow, 50% transparent
    G4VisAttributes* drift2Vis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.5));
    drift2Vis->SetForceSolid(true);
    logicDrift2->SetVisAttributes(drift2Vis);
    
    // Drift chamber 3 - green, 50% transparent
    G4VisAttributes* drift3Vis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.5));
    drift3Vis->SetForceSolid(true);
    logicDrift3->SetVisAttributes(drift3Vis);
    // Drift chamber 4 - green, 50% transparent
    G4VisAttributes* drift4Vis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.5));
    drift4Vis->SetForceSolid(true);
    logicDrift4->SetVisAttributes(drift4Vis);
    
    return fWorldPhys;
}

void DetectorConstruction::ConstructSDandField() {
    // Get sensitive detector manager
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    
    // Create sensitive detectors
    SensitiveDetector* scintSD = new SensitiveDetector("ScintillatorSD");
    SensitiveDetector* driftSD = new SensitiveDetector("DriftChamberSD");
    
    // Register with manager
    sdManager->AddNewDetector(scintSD);
    sdManager->AddNewDetector(driftSD);
    
    // Attach to logical volumes
    SetSensitiveDetector("Scintillator1", scintSD);
    SetSensitiveDetector("Scintillator2", scintSD);
    SetSensitiveDetector("Scintillator3", scintSD);
    SetSensitiveDetector("DriftChamber1", driftSD);
    SetSensitiveDetector("DriftChamber2", driftSD);
    SetSensitiveDetector("DriftChamber3", driftSD);
    SetSensitiveDetector("DriftChamber4", driftSD);
}
