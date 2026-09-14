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
// $Id: ExN01DetectorConstruction.cc,v 1.8 2003/10/06 08:59:11 maire Exp $
// GEANT4 tag $Name: geant4-07-00-patch-01 $
//

#include "ExN01DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"

ExN01DetectorConstruction::ExN01DetectorConstruction()
 :  experimentalHall_log(0), ball_log(0),
    experimentalHall_phys(0), ball_phys(0)
{
  //------------------------------ 10kg ball of DU
  // 4/3 pi r^3 * 18.95gm/CLHEP::cm3 = 10kg

  outerRadiusOfTheBall = pow(10000./18.95/(4.0*3.1415/3.0),0.333333)*CLHEP::cm;
}

ExN01DetectorConstruction::~ExN01DetectorConstruction()
{
}

G4VPhysicalVolume* ExN01DetectorConstruction::Construct()
{

  //------------------------------------------------------ materials

  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;

  G4Material* U238 = 
  new G4Material("U238", z= 92., a= 238.0289*CLHEP::g/CLHEP::mole, density= 18.95*CLHEP::g/CLHEP::cm3);

  G4Material* Vacuum = 
  new G4Material("Galactic", z= 1., a= 1.01*CLHEP::g/CLHEP::mole, density= 1.0e-25*CLHEP::g/CLHEP::cm3);


  //------------------------------------------------------ volumes

  //------------------------------ experimental hall (world volume)
  //------------------------------ beam line along x axis

  G4double expHall_x = 3.0*CLHEP::m;
  G4double expHall_y = 3.0*CLHEP::m;
  G4double expHall_z = 3.0*CLHEP::m;
  G4Box* experimentalHall_box
    = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
  experimentalHall_log = new G4LogicalVolume(experimentalHall_box,
                                             Vacuum,"expHall_log",0,0,0);
  experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),
                                      experimentalHall_log,"expHall",0,false,0);

  //------------------------------ 10kg ball of DU

  G4Orb* DU_ball = new G4Orb("DU_ball",outerRadiusOfTheBall);
  ball_log = new G4LogicalVolume(DU_ball,U238,"ball_log",0,0,0);
  ball_phys = new G4PVPlacement(0,
             G4ThreeVector(),
             ball_log,"ball",experimentalHall_log,false,0);

  //------------------------------------------------------------------

  return experimentalHall_phys;
}

G4double ExN01DetectorConstruction::GetRadius(void) {
  return outerRadiusOfTheBall;
}
