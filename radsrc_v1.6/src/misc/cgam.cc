/* 
Copyright (c) 2006-2014 Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory 
UCRL-CODE-224806.

All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

o Redistributions of source code must retain the above copyright notice, this list of conditions and the disclaimer below.

o Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the disclaimer (as noted below) in the documentation and/or other materials provided with the distribution.

o Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
Additional BSD Notice

1. This notice is required to be provided under our contract with the U.S. Department of Energy (DOE). This work was produced at Lawrence Livermore National Laboratory under Contract No. DE-AC52-07NA27344 with the DOE. 

2. Neither the United States Government nor Lawrence Livermore National Security, LLC nor any of their employees, makes any warranty, express or implied, or assumes any liability or responsibility for the accuracy, completeness, or usefulness of any information, apparatus, product, or process disclosed, or represents that its use would not infringe privately-owned rights. 

3. Also, reference herein to any specific commercial products, process, or services by trade name, trademark, manufacturer or otherwise does not necessarily constitute or imply its endorsement, recommendation, or favoring by the United States Government or Lawrence Livermore National Security, LLC. The views and opinions of authors expressed herein do not necessarily state or reflect those of the United States Government or Lawrence Livermore National Security, LLC, and shall not be used for advertising or product endorsement purposes.
*/


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <iostream>
#include "radsource.h"
#include "isotopedb.h"
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include "decaycomputer.h"
#include "photoncomputer.h"
#include "dbmanager.h"

using namespace radsrc;

void PrintIsotopes(const CIsotopeDatabase*);
void TabulateIsotopes(const CIsotopeDatabase*);
void Age(CDecayComputer& dc, COldRadSource& oldradsource, double age);
void Gammas(COldRadSource& oldradsource);
void Gammas(CPhotonComputer& pc);
void PrintAllDecays(const CRadSource& radsource, const CIsotope& parent);

//double rng() { return drand48(); }

int main(int argc, const char *argv[]) {
  int i;
//  AssertDatabaseLoaded();
//  PrintIsotopes();
  CRadSource radsource;
  CDecayComputer dc;
  CPhotonComputer pc(dc);

/*
  CIsotopeData *ptr;
  ptr = const_cast<CIsotopeData*>(pIsotopeDB->LookUpData(CIsotope(82,210)));
  ptr->m_halflife = 1;
  ptr->m_decays[0].m_fraction = .25;
  ptr->m_decays[1].m_fraction = .75;

  ptr = const_cast<CIsotopeData*>(pIsotopeDB->LookUpData(CIsotope(83,210)));
  ptr->m_halflife = 1.2;
  ptr->m_decays[0].m_fraction = 1/3.;
  ptr->m_decays[1].m_fraction = 2/3.;

  ptr = const_cast<CIsotopeData*>(pIsotopeDB->LookUpData(CIsotope(80,206)));
  ptr->m_halflife = 1.1;

  ptr = const_cast<CIsotopeData*>(pIsotopeDB->LookUpData(CIsotope(81,206)));
  ptr->m_halflife = 0.9;

  ptr = const_cast<CIsotopeData*>(pIsotopeDB->LookUpData(CIsotope(84,210)));
  ptr->m_halflife = 1.3;

  PrintIsotopes();
*/
  double age;

/*
  radsource.getDecayComputer().addInputItem(CIsotope(94,238),0.00020);
  radsource.getDecayComputer().addInputItem(CIsotope(94,239),0.93724);
  radsource.getDecayComputer().addInputItem(CIsotope(94,240),0.05960);
  radsource.getDecayComputer().addInputItem(CIsotope(94,241),0.00268);
  radsource.getDecayComputer().addInputItem(CIsotope(94,242),0.00028);
  dc.addInputItem(CIsotope(94,238),0.00020);
  dc.addInputItem(CIsotope(94,239),0.93724);
  dc.addInputItem(CIsotope(94,240),0.05960);
  dc.addInputItem(CIsotope(94,241),0.00268);
  dc.addInputItem(CIsotope(94,242),0.00028);
  age = 25;
*/
  radsource.getDecayComputer().addInputItem(CIsotope(82,210),1.0);
  dc.addInputItem(CIsotope(82,210),1.0);
  age = 1.0;
/*
  dc.addInputItem(CIsotope(82,210),1.0);
  dc.normalizeInputUnits();
  age = 1.0;
  std::cin.tie(&std::cout);
  std::cout << "Enter Source 'Xx-NNN': ";
  std::string isotope;
  std::getline(std::cin,isotope);
  CIsotope source;
  source.FromString(isotope);
  std::cout << "Using " << source.toString().c_str() << std::endl;
  dc.addInputItem(source,1.);
  dc.normalizeInputUnits();
  radsource.getDecayComputer().addInputItem(source,1.);
  radsource.getDecayComputer().normalizeInputUnits();
  std::cout << "Enter Age: ";
  std::cin >> age;
  age /= 365*24*3600;
*/
  radsource.getDecayComputer().ageMixture(age);
  dc.ageMixture(age);
  std::vector<double> bins;
  for (i = 0; i < 131; i++) {
    bins.push_back(50.+i*50);
  }
  radsource.getPhotonComputer().setBinning(CPhotonComputer::BIN_BREM,bins);
  pc.setBinning(CPhotonComputer::BIN_BREM,bins);
  Gammas(pc);
  std::cout << pc.getNGammas() << " Gammas" << std::endl;
  CPhotonComputer::CPhotonIterator it, end;
  it = pc.beginGammas();
  end = pc.endGammas();
  i = 0;
  std::cout << radsource.getReport().c_str();
  while (it != end) {
     const CPhotonComputer::CGammaEntry& entry = *it;
     std::cout << entry.getEnergy() << " keV, " << entry.getIntensity() << " ph/s/g" << std::endl;
     std::cout << "     Parents: ";
     std::set<CIsotope>::const_iterator iit, iend;
     iit = entry.getParentIsotopes().begin();
     iend = entry.getParentIsotopes().end();
     while (iit != iend) {
        std::cout << iit->toString().c_str() << ' ';
        ++iit;
     }
     std::cout << std::endl;
     std::cout << "     Chain Parents: ";
     iit = entry.getChainParentIsotopes().begin();
     iend = entry.getChainParentIsotopes().end();
     while (iit != iend) {
        std::cout << iit->toString().c_str() << ' ';
        ++iit;
     }
     std::cout << std::endl;
     std::cout << "     Decays: ";
     std::set<std::pair<CIsotope,CIsotope> >::const_iterator dit, dend;
     dit = entry.getDecays().begin();
     dend = entry.getDecays().end();
     while (dit != dend) {
        std::cout << dit->first.toString().c_str() << " -> " << dit->second.toString().c_str() << ' ';
        ++dit;
     }
     std::cout << std::endl;
     ++it;
  }

/*
  std::ofstream brem("brem.txt");
  for (i = 0; i < pc.getBrem().m_intensity.size(); i++) {
     brem
      << pc.getBrem().m_energy[i] << ' '
      << pc.getBrem().m_intensity[i] << std::endl;
  }
  brem.close();
*/


  //PrintAllDecays(radsource,CIsotope().fromString("u238"));

  // PrintIsotopes(radsource.getDecayComputer().getIsotopeDatabase());
  // TabulateIsotopes(radsource.getDecayComputer().getIsotopeDatabase());
  return 0;
}

void
PrintIsotopes(const CIsotopeDatabase* pIsotopeDB) {
   CIsotopeDatabase::IsotopeList::const_iterator it = pIsotopeDB->isotopesBegin();
   while (it != pIsotopeDB->isotopesEnd()) {
      CIsotopeData::DecayIterator decay1, decay2;
      const CIsotopeData& isodata = *(it->second);
      switch(isodata.getNDecayModes()) {
      case 0:
         std::cout
            << isodata.getName() << " is stable"
            << std::endl;
         break;
      case 1:
         decay1 = isodata.decaysBegin();
         std::cout
            << isodata.getName() << " decays to "
            << decay1->getDaughter().toString().c_str()
            << " hl = " << isodata.getHalflife()
            << " with " << decay1->getNPhotons() << " gammas"
            << std::endl;
         break;
      case 2:
         decay1 = isodata.decaysBegin();
         decay2 = decay1; ++decay2;
         std::cout
            << isodata.getName() << " decays to "
            << decay1->getDaughter().toString().c_str() << ", "
            << decay2->getDaughter().toString().c_str()
            << " hl = " << isodata.getHalflife()
            << " with ratios: " << decay1->getBranchFraction() << ", "
            << decay2->getBranchFraction()
            << " and " << decay1->getNPhotons()
            << ", " << decay2->getNPhotons() << " gammas"
            << std::endl;
         break;
      default:
         std::cout << isodata.getName() << " has " << isodata.getNDecayModes() << " decays"
            << " hl = " << isodata.getHalflife() << std::endl;
         break;
      }
      ++it;
   }
}

void
Age(CDecayComputer& dc, double age) {
  dc.ageMixture(age);
  std::cout.setf(std::ios_base::scientific,std::ios_base::floatfield);
  double sum = 0.0;

  std::cout << "Using new objects:" << std::endl;
  std::cout << "Total: " << sum << std::endl;
  std::cout << std::endl;
  CIsotopeMixture::const_iterator it;
  it = dc.getAgedMixture().begin();
  sum = 0.0;
  while (it != dc.getAgedMixture().end()) {
    std::cout << it->first.toString().c_str() << "  "
      << it->second << std::endl;
    sum += it->second;
    ++it;
  }
  std::cout << "Total: " << sum << std::endl;
  std::cout << "Using eval():" << std::endl;
  it = dc.getAgedMixture().begin();
  sum = 0.0;
  while (it != dc.getAgedMixture().end()) {
    double quantity = (double) dc.getSolution(it->first)->eval(age);
    std::cout << it->first.toString().c_str() << "  "
      << quantity << std::endl;
    sum += quantity;
    ++it;
  }
  std::cout << "Total: " << sum << std::endl;
}


void
Gammas(CPhotonComputer& pc) {
   pc.computeGammas(CPhotonComputer::ENERGY); 
  
}

void
TabulateIsotopes(const CIsotopeDatabase* pIsotopeDB) {
   CIsotopeData::DecayIterator decay1, decay2;
   CIsotopeDatabase::IsotopeList::const_iterator it = pIsotopeDB->isotopesBegin();
   std::cout << "Isotope,Halflife,Daughter,Branch Ratio,# Photons,Daughter,Branch Ratio,# Photons" << std::endl;
   while (it != pIsotopeDB->isotopesEnd()) {
      const CIsotopeData& isodata = *(it->second);
      switch(isodata.getNDecayModes()) {
      case 0:
         std::cout << isodata.getName() << ",,,,,,," << std::endl;
         break;
      case 1:
         decay1 = isodata.decaysBegin();
         std::cout << isodata.getName()
            << ',' << isodata.getHalflife()
            << ',' << decay1->getDaughter().toString().c_str()
            << ',' << decay1->getBranchFraction()
            << ',' << decay1->getNPhotons()
            << ",,," << std::endl;
         break;
      case 2:
         decay1 = isodata.decaysBegin();
         decay2 = decay1; ++decay2;
         std::cout << isodata.getName()
            << ',' << isodata.getHalflife()
            << ',' << decay1->getDaughter().toString().c_str()
            << ',' << decay1->getBranchFraction()
            << ',' << decay1->getNPhotons()
            << ',' << decay2->getDaughter().toString().c_str()
            << ',' << decay2->getBranchFraction()
            << ',' << decay2->getNPhotons()
            << std::endl;
         break;
      default:
         std::cout << isodata.getName()
            << ',' << isodata.getHalflife()
            << ",>2,,,,," << std::endl;
         break;
      }
      ++it;
   }
}

void
PrintAllDecays(const CRadSource& radsource, const CIsotope& parent) {
   DecayChainSet& chains = radsource.getDecayComputer().computeDecayChains(parent);
   
   std::cout << "Computed " << chains.size() << " distinct decay chains for "
      << chains[0][0].m_Parent.toString().c_str() << std::endl;
   DecayChainSet::const_iterator chainit, endchainit;
   chainit = chains.begin();
   endchainit = chains.end();
   int i,j;
   i = 1;
   while(chainit != endchainit) {
      std::cout << "Branch " << i << ": " << std::endl;
      DecayChain::const_iterator decayit, enddecayit;
      decayit = chainit->begin();
      enddecayit = chainit->end();
      j = 1;
      while(decayit != enddecayit) {
         const CDecay& entry = *decayit;
         if (entry.m_pDecay) {
            std::cout << "  " << j << ": "
               << entry.m_Parent.toString().c_str() << " --> "
               << entry.m_pDecay->getDaughter().toString().c_str()
               << " with branch ratio " << entry.m_pDecay->getBranchFraction()
               << ", emits " << entry.m_pDecay->getNPhotons() << " photons"
               << "(hl=" << radsource.getDecayComputer().getIsotopeData(entry.m_Parent)->getHalflife() << ")"
               << std::endl;
            CDecayMode::PhotonIterator phit, endphit;
            phit = entry.m_pDecay->beginPhotons();
            endphit = entry.m_pDecay->endPhotons();
            while (phit != endphit) {
               std::cout << "Energy: " << phit->getEnergy()
                  << ", Probability per decay " << phit->getFraction()
                  << std::endl;
               ++phit;
            }
         } else {
            std::cout << "  " << j << ": "
               << entry.m_Parent.toString().c_str() << " (stable)" << std::endl;
         }
         ++decayit;
         j++;
      }
      std::cout << std::endl;
      ++chainit;
      i++;
   }
}
