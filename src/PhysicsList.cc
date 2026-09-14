#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList() {
    // EM physics
    RegisterPhysics(new G4EmStandardPhysics());
    // Decay physics
    RegisterPhysics(new G4DecayPhysics());
    
    SetDefaultCutValue(0.7*mm);
}

PhysicsList::~PhysicsList() {}
