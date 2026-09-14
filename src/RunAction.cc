#include "RunAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <fstream>
#include <cstdio>  // For remove()

RunAction::RunAction() : G4UserRunAction() {
}

RunAction::~RunAction() {
}

void RunAction::BeginOfRunAction(const G4Run* run) {
    G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
    
    // Delete old file if it exists
    std::remove("hits_data.csv");
    
    // Create CSV file with header
    std::ofstream outfile("hits_data.csv", std::ios::out);
    if (outfile.is_open()) {
        outfile << "x_cm,y_cm,z_cm,edep_MeV,time_ns,detector_type,particle,detector_name" << std::endl;
        outfile.close();
        G4cout << "Created output file: hits_data.csv" << G4endl;
    } else {
        G4cerr << "ERROR: Could not create hits_data.csv" << G4endl;
    }
}

void RunAction::EndOfRunAction(const G4Run* run) {
    G4cout << "### Run " << run->GetRunID() << " end." << G4endl;
    G4cout << "Hit data saved to: hits_data.csv" << G4endl;
}
