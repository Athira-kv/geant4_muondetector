//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: ExN01EventAction.cc,v 1.25 2005/07/22 15:27:33 maire Exp $
// GEANT4 tag $Name: geant4-07-01-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ExN01EventAction.hh"

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN01EventAction::ExN01EventAction()
  :Xinit(0),Yinit(0),Zinit(0),Einit(0),Efinal(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN01EventAction::~ExN01EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN01EventAction::BeginOfEventAction(const G4Event* evt)
{
 if (evt == NULL) {;}    // Dummy statement to prevent warning about unused var
 // initialisation per event
  Xinit = Yinit = Zinit = 0.0;
  Einit = 0.0;
  Efinal = -1.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN01EventAction::EndOfEventAction(const G4Event* evt)
{
  if (evt == NULL) {;}    // Dummy statement to prevent warning about unused var
  if (Efinal > 0.0) {
    G4cout 
      << "Photon exited source: Initial (x,y,z)="
      << " (" << Xinit  << " " << Yinit << " " << Zinit << ")"
      << " Einitial=" << Einit 
      << " Efinal=" << Efinal 
      << G4endl;
  }
  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

