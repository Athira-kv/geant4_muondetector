#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include <vector>

class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name);
    virtual ~SensitiveDetector();
    
    virtual void Initialize(G4HCofThisEvent*);
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    virtual void EndOfEvent(G4HCofThisEvent*);
    
    static void ResetFileFlag();  // Reset flag for new run
    
private:
    std::vector<G4ThreeVector> fHitPositions;
    std::vector<G4double> fHitEnergies;
    std::vector<G4double> fHitTimes;
    
    static bool fFileInitialized;  // Static member to track if file has header
};

#endif
