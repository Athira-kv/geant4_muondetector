#include "EventAction.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

EventAction::EventAction() 
    : G4UserEventAction(), 
      fEventID(0), 
      fInitialEnergy(0),
      fTruePosition(G4ThreeVector()),
      fTrueDirection(G4ThreeVector()) {}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event) {
    fEventID = event->GetEventID();
    
    // Get initial muon energy and direction from primary particle
    G4PrimaryVertex* vertex = event->GetPrimaryVertex();
    if (vertex) {
        G4PrimaryParticle* primary = vertex->GetPrimary();
        if (primary) {
            fInitialEnergy = primary->GetKineticEnergy();
            fTruePosition = vertex->GetPosition();
            fTrueDirection = primary->GetMomentumDirection();
        }
    }
    
    if (fEventID % 100 == 0) {
        G4cout << ">>> Event " << fEventID 
               << " | E=" << fInitialEnergy/GeV << " GeV"
               << " | deg =" << std::acos(fTrueDirection.z())*180/3.14159 << "°"
               << G4endl;
    }
}

void EventAction::EndOfEventAction(const G4Event* event) {
    // Event summary
}
