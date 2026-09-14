#include "PrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>
#include "G4Event.hh"

#include "DCConfig.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {
    fParticleGun = new G4ParticleGun(1);
    
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* muon = particleTable->FindParticle("mu-");
    fParticleGun->SetParticleDefinition(muon);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}


G4double PrimaryGeneratorAction::SampleCosmicMuonEnergy() {
    G4double E_min = MIN_ENERGY_GEV*GeV;
    G4double E_max = 10.0*GeV;  // Cut off high energy tail
    
    // Uniform in log space (favors lower energies)
    G4double logE_min = std::log(E_min);
    G4double logE_max = std::log(E_max);
    G4double energy = std::exp(logE_min + G4UniformRand() * (logE_max - logE_min));
    
    return energy;
}

G4double PrimaryGeneratorAction::SampleCosmicZenithAngle() {

  // Sample from cos^2(\theta) distribution
    
    G4double cosTheta = std::pow(G4UniformRand(), 1.0/3.0);
    G4double theta = std::acos(cosTheta);
    if (theta > 10*deg) {
        // Reject and resample
      return SampleCosmicZenithAngle();
    }
    
    return theta;
}

G4double PrimaryGeneratorAction::SampleXpos(){
  // sample x posiiton uniformly 30cm x 30cm just above the scintillator
  double width = 15.0 ;//cm
  G4double x = 2*width*G4UniformRand() - width ;
   return x;
}

G4double PrimaryGeneratorAction::SampleYpos(){
  // sample y posiiton uniformly 30cm x 30cm just above the scintillator
  double width = 15.0; //cm
  G4double y = 2*width*G4UniformRand() - width ;
   return y;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

  
    // ========== ENERGY ==========
    // Sample from cosmic muon energy spectrum
    G4double energy = SampleCosmicMuonEnergy();
    fParticleGun->SetParticleEnergy(energy);
    
    
    // ========== DIRECTION ==========
    // Sample zenith angle from cos²(θ) distribution
    G4double theta = SampleCosmicZenithAngle();
    
    // Azimuthal angle: uniform (muons come from all horizontal directions)
    G4double phi = 2*M_PI * G4UniformRand();
    
    // Convert to direction vector (pointing downward)
    G4double px = std::sin(theta) * std::cos(phi);
    G4double py = std::sin(theta) * std::sin(phi);
    G4double pz = -std::cos(theta);  // Negative z = downward
    
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px, py, pz));
      
    // ========== POSITION ==========
    // Sample uniformly over detector acceptance area
    
    // Detector effective area: ~20x20 cm (drift chambers)
    //G4double detector_radius = 10*cm;  
    
    // Uniform circular distribution 
    //G4double r = detector_radius * std::sqrt(G4UniformRand());
    //G4double pos_phi = 2*M_PI * G4UniformRand();
    
    //G4double x = r * std::cos(pos_phi);
    //G4double y = r * std::sin(pos_phi);
    
    // Start position height
    G4double z = 1.4*m;  
    G4double x = SampleXpos();
    G4double y = SampleYpos();
    
    // DEBUG: Print every 100 events
    /*if (anEvent->GetEventID() % 100 == 0) {
        G4cout << "Event " << anEvent->GetEventID() 
               << ": Starting at (" << x/cm << ", " << y/cm << ") cm"
               << " energy = "<<energy<<" theta = "<<theta
               << G4endl;*/

    
    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
    
    
    // ========== CHARGE RATIO ==========
    // Randomly assign mu+ or mu- with realistic charge ratio (~1.3:1)
    if (G4UniformRand() < 0.565) {  // 56.5% mu+, 43.5% mu-
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        fParticleGun->SetParticleDefinition(particleTable->FindParticle("mu+"));
    } else {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        fParticleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
    }
    
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

