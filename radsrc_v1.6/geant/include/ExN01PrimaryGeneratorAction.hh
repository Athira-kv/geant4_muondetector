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
// $Id: ExN01PrimaryGeneratorAction.hh,v 1.4 2002/01/09 17:23:48 ranjard Exp $
// GEANT4 tag $Name: geant4-07-00-patch-01 $
//

#ifndef ExN01PrimaryGeneratorAction_h
#define ExN01PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "radsource.h"
#include "cpp_api.h"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class ExN01PrimaryGeneratorMessenger;

class ExN01PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    ExN01PrimaryGeneratorAction(G4double Radius);
    ExN01PrimaryGeneratorAction(G4double Radius, G4String FileName);
    ~ExN01PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);
    static double localran(void);
    void UpdateRadsrc();
    void InputRadsrc(G4String InputLine);
    void RadsrcFromFile(G4String InputLine);

  private:
    G4ParticleGun* particleGun;
    ExN01PrimaryGeneratorMessenger* gunMessenger;
    radsrc::CRadSource* pRadSource;
    radsrc::CRadSource* t1RadSource;
    radsrc::CRadSource* t2RadSource;
    G4double outerRadiusOfTheBall;
    G4bool t1Good,t2Good,NewInput;
};

#endif


