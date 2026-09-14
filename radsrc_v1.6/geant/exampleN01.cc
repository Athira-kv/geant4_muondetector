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
// $Id: exampleN01.cc,v 1.4 2002/01/09 17:23:47 ranjard Exp $
// GEANT4 tag $Name: geant4-07-00-patch-01 $
//
// 
// --------------------------------------------------------------
//      GEANT 4 - exampleN01 
//
// --------------------------------------------------------------
// Comments
//
// 
// --------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"

#include "ExN01DetectorConstruction.hh"
#include "ExN01PhysicsList.hh"
#include "ExN01PrimaryGeneratorAction.hh"
#include "ExN01EventAction.hh"
#include "ExN01SteppingAction.hh"


int main(int argc, char** argv)
{
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;

  // set mandatory initialization classes
  ExN01DetectorConstruction* Det = new ExN01DetectorConstruction;
  G4cout << "Calculated Radius = " << Det->GetRadius() << G4endl;
  runManager->SetUserInitialization(Det);
  runManager->SetUserInitialization(new ExN01PhysicsList);

  // set mandatory user action class
  G4String FileName="";
  ExN01PrimaryGeneratorAction* Gen = 
    new ExN01PrimaryGeneratorAction(Det->GetRadius(),FileName);
  runManager->SetUserAction(Gen);
  ExN01EventAction* eventaction = new ExN01EventAction();
  runManager->SetUserAction(eventaction);
  runManager->SetUserAction(new ExN01SteppingAction(eventaction));

  // Initialize G4 kernel
  runManager->Initialize();

  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();

  if(argc==1)
    {
      G4UIsession* session = new G4UIterminal;
      UI->ApplyCommand("/control/execute prerun.G4mac");
      session->SessionStart();
      delete session;
    }
  else
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command+fileName);
    }

  //  UI->ApplyCommand("/run/verbose 1");
  //  UI->ApplyCommand("/event/verbose 1");
  //  UI->ApplyCommand("/tracking/verbose 1");

  //  UI->ApplyCommand("/radsrc/input U235 100.0");
  //  UI->ApplyCommand("/radsrc/input AGE 15");
  //  UI->ApplyCommand("/radsrc/update");

  // start a run
  //int numberOfEvent = 100;
  //runManager->BeamOn(numberOfEvent);

  // job termination
  delete runManager;
  return 0;
}


