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
// $Id: ExN01PrimaryGeneratorAction.cc,v 1.5 2002/05/14 01:50:31 asaim Exp $
// GEANT4 tag $Name: geant4-07-00-patch-01 $
//

#include "ExN01PrimaryGeneratorAction.hh"
#include "ExN01PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"

ExN01PrimaryGeneratorAction::ExN01PrimaryGeneratorAction(G4double Radius, G4String FileName)
{
  outerRadiusOfTheBall = Radius;
  //
  //  The radsource classes are included in the header file
  //  ExH01PrimaryGeneratorAction.hh
  //  An instanciation of the Radsource library is invoked
  //  and the member function SetupDefault() is called which reads
  //  in the problem definition from an exteranl file.
  //  See the user guide for more details.
  //
  NewInput=true;
  //
  t1RadSource = radsrc::CApi::newSource();
  t1Good = radsrc::CApi::loadConfig(t1RadSource, (const std::string) FileName);
  if(t1Good) pRadSource=t1RadSource;
  //
  gunMessenger = new ExN01PrimaryGeneratorMessenger(this);
  //
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="gamma"));
}

ExN01PrimaryGeneratorAction::ExN01PrimaryGeneratorAction(G4double Radius)
{
  G4String FileName="";
  ExN01PrimaryGeneratorAction(Radius,FileName);
}

ExN01PrimaryGeneratorAction::~ExN01PrimaryGeneratorAction()
{
  delete particleGun;
  /*
  delete pRadSource;
  delete t1RadSource;
  delete t2RadSource;
  */
}

double ExN01PrimaryGeneratorAction::localran(void)
{
  return G4UniformRand();
}

void ExN01PrimaryGeneratorAction::RadsrcFromFile(G4String InputLine)
{
  t2RadSource = radsrc::CApi::newSource();
  t2Good = radsrc::CApi::loadConfig(t2RadSource, InputLine);
  if(t2Good) pRadSource=t2RadSource;
}

void ExN01PrimaryGeneratorAction::InputRadsrc(G4String InputLine)
{
  if(NewInput){
    t2RadSource = radsrc::CApi::newSource();
    NewInput=false;
  }
  radsrc::CApi::addConfig(t2RadSource, InputLine);
}

void ExN01PrimaryGeneratorAction::UpdateRadsrc()
{
  t2Good = radsrc::CApi::sourceConfig(t2RadSource);
  if(t2Good) pRadSource=t2RadSource;
  NewInput=true;
}

void ExN01PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // G4int i = anEvent->GetEventID();
  //G4cout << "in generate primaries:" << i << G4endl;
  //
  //  The member function GetPhoton() is invoked to sample the
  //  energy distribution previously defined.
  //
  if(pRadSource == NULL){
    G4Exception("ExN01PrimaryGeneratorAction::GeneratePrimaries",
		"RADSRC has not been defined",RunMustBeAborted,
		" see the interface manual to insure that your input is formatted correctly");
  }
  G4double energy = radsrc::CApi::getPhoton(pRadSource, localran ) * CLHEP::keV;
  particleGun->SetParticleEnergy(energy);
  //
  G4double phi = 2.0*CLHEP::pi*G4UniformRand();
  G4double costheta = 2.0*G4UniformRand()-1.0;
  G4double sintheta = pow(1.0-pow(costheta,2.0),0.5);
  G4double r = outerRadiusOfTheBall * pow(G4UniformRand(),1.0/3.0);
  particleGun->SetParticlePosition(G4ThreeVector(r*costheta,r*sintheta*cos(phi),r*sintheta*sin(phi)));
  //
  phi = 2.0*CLHEP::pi*G4UniformRand();
  costheta = 2.0*G4UniformRand()-1.0;
  sintheta = pow(1.0-pow(costheta,2.0),0.5);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(costheta,sintheta*cos(phi),sintheta*sin(phi)));
  particleGun->GeneratePrimaryVertex(anEvent);
}


