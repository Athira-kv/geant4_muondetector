#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

SteppingAction::SteppingAction() : G4UserSteppingAction() {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    // Get energy deposition
    G4double edep = step->GetTotalEnergyDeposit();
    
    if (edep > 0) {
        // Get volume information
        G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetPhysicalVolume();
        G4String volumeName = volume->GetName();
        
        // Only print for detector volumes
        if (volumeName.contains("Detector")) {
            G4Track* track = step->GetTrack();
            G4String particleName = track->GetDefinition()->GetParticleName();
            G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
            
            G4cout << "Particle: " << particleName 
                   << " | Volume: " << volumeName
                   << " | Energy deposited: " << edep/MeV << " MeV"
                   << " | Position: (" 
                   << position.x()/cm << ", "
                   << position.y()/cm << ", "
                   << position.z()/cm << ") cm"
                   << G4endl;
        }
    }
}
