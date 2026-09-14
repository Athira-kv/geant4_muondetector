#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();
    
    virtual void GeneratePrimaries(G4Event*);
    
private:
    G4ParticleGun* fParticleGun;
    
    // Cosmic muon sampling functions
    G4double SampleCosmicMuonEnergy();
    G4double SampleCosmicZenithAngle();
    G4double SampleXpos();
    G4double SampleYpos();
};

#endif
