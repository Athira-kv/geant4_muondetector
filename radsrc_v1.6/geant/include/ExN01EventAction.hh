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
// $Id: ExN01EventAction.hh,v 1.9 2005/07/22 15:27:33 maire Exp $
// GEANT4 tag $Name: geant4-07-01-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ExN01EventAction_h
#define ExN01EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ExN01EventAction : public G4UserEventAction
{
 public:
   ExN01EventAction();
  ~ExN01EventAction();

 public:
   void  BeginOfEventAction(const G4Event*);
   void    EndOfEventAction(const G4Event*);
  void SetInitX(G4double X) {Xinit = X;}
  void SetInitY(G4double Y) {Yinit = Y;}
  void SetInitZ(G4double Z) {Zinit = Z;}
  void SetInitE(G4double E) {Einit = E;}
  void SetFinalE(G4double E) {Efinal = E;}
    
  //   void AddAbs(G4double de, G4double dl) {EnergyAbs += de; TrackLAbs += dl;};
  // void AddGap(G4double de, G4double dl) {EnergyGap += de; TrackLGap += dl;};

  G4double  Xinit, Yinit, Zinit, Einit, Efinal;
                    
 private:

   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
