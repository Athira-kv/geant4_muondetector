#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class EventAction : public G4UserEventAction {
public:
    EventAction();
    virtual ~EventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
    void SetInitialEnergy(G4double energy) { fInitialEnergy = energy; }
    G4double GetInitialEnergy() const { return fInitialEnergy; }
    
    void SetTruePosition(G4ThreeVector pos) { fTruePosition = pos; }
    G4ThreeVector GetTruePosition() const { return fTruePosition; }
    
    void SetTrueDirection(G4ThreeVector dir) { fTrueDirection = dir; }
    G4ThreeVector GetTrueDirection() const { return fTrueDirection; }
    
private:
    G4int fEventID;
    G4double fInitialEnergy;
    G4ThreeVector fTruePosition;
    G4ThreeVector fTrueDirection;
};

#endif
