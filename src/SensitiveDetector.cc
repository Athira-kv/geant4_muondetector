#include "SensitiveDetector.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include <fstream>
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

// Initialize static member
bool SensitiveDetector::fFileInitialized = false;

SensitiveDetector::SensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name) {
}

SensitiveDetector::~SensitiveDetector() {}

void SensitiveDetector::ResetFileFlag() {
    fFileInitialized = false;
}

void SensitiveDetector::Initialize(G4HCofThisEvent*) {
    fHitPositions.clear();
    fHitEnergies.clear();
    fHitTimes.clear();
    
    if (!fFileInitialized) {
        std::ofstream outfile("hits_data.csv", std::ios::out | std::ios::trunc);
        if (outfile.is_open()) {
            outfile << "event_id,initial_energy_GeV,theta,true_x0_cm,true_y0_cm,true_z0_cm,"
                    << "true_theta_x_rad,true_theta_y_rad,true_theta_rad,"
                    << "x_cm,y_cm,z_cm,edep_MeV,time_ns,detector_type,particle,detector_name" 
                    << std::endl;
            outfile.close();
            fFileInitialized = true;
            G4cout << "=== Created hits_data.csv with header ===" << G4endl;
        }
    }
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return false;
    
    G4StepPoint* preStep = step->GetPreStepPoint();
    G4ThreeVector position = preStep->GetPosition();
    G4double time = preStep->GetGlobalTime();
    G4String detectorName = preStep->GetPhysicalVolume()->GetName();
    
    fHitPositions.push_back(position);
    fHitEnergies.push_back(edep);
    fHitTimes.push_back(time);
    
    G4Track* track = step->GetTrack();
    G4String particleName = track->GetDefinition()->GetParticleName();
    
    // Get event ID
    const G4Event* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    G4int eventID = event->GetEventID();
    
    // Get initial energy from the primary vertex
    G4double initialEnergy = 0.0;
    G4ThreeVector truePosition(0, 0, 0);
    G4ThreeVector trueDirection(0, 0, -1);
    G4double theta = 0.0;
    
    if (event) {
        G4PrimaryVertex* vertex = event->GetPrimaryVertex();
        if (vertex) {
            G4PrimaryParticle* primary = vertex->GetPrimary();
            if (primary) {
                initialEnergy = primary->GetKineticEnergy();
                truePosition = vertex->GetPosition();
                trueDirection = primary->GetMomentumDirection();
		theta = trueDirection.theta();
            }
        }
    }
    
    // Calculate true angles
    G4double true_theta_x = std::atan2(trueDirection.x(), -trueDirection.z());
    G4double true_theta_y = std::atan2(trueDirection.y(), -trueDirection.z());
    // Zenith angle: angle from vertical (downward = 0°)
    // For downward-going particles, use absolute value of z-component
    G4double true_theta = std::acos(std::abs(trueDirection.z()));  

    /*    // Print hit information (only for primary muon)
    if (track->GetTrackID() == 1) {
        G4cout << "HIT in " << detectorName 
               << " | E=" << initialEnergy/GeV << " GeV"
               << " | ΔE=" << edep/MeV << " MeV" << G4endl;
	       }*/
    
    // Encode detector type
    G4int detType = -1;
    if (detectorName.contains("Scintillator1")) detType = 0;
    else if (detectorName.contains("Scintillator2")) detType = 1;
    else if (detectorName.contains("DriftChamber1")) detType = 2;
    else if (detectorName.contains("DriftChamber2")) detType = 3;
    else if (detectorName.contains("DriftChamber3")) detType = 4;
    else if (detectorName.contains("DriftChamber4")) detType = 5;
    else if (detectorName.contains("Scintillator3")) detType = 6;
    
    
   // Write to CSV - use static file handle
    static std::ofstream outfile("hits_data.csv", std::ios::app);
    outfile << eventID << ","
        << initialEnergy/GeV << ","
	<< (CLHEP::pi - theta)*rad << ","
        << truePosition.x()/cm << ","
        << truePosition.y()/cm << ","
        << truePosition.z()/cm << ","
        << true_theta_x << ","
        << true_theta_y << ","
        << true_theta << ","
        << position.x()/cm << ","
        << position.y()/cm << ","
        << position.z()/cm << ","
        << edep/MeV << ","
        << time/ns << ","
        << detType << ","
        << particleName << ","
        << detectorName << "\n";   
    return true;
}



void SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
    G4int nHits = fHitPositions.size();
    if (nHits > 0) {
      // G4cout << "Event complete: " << nHits << " hits" << G4endl;
    }
}
