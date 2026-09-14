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


/*  C++ Interface
 */

#include "porting.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "radsource.h"
#include "cpp_api.h"

namespace radsrc {
   CRadSource* CApi::newSource(void) {
      CRadSource *pRadSource = new CRadSource;
      /*
      pRadSource->getRadSource()->m_InputMixtureUnits =
         CRadSource::ATOM_FRACTION;
         */
      return pRadSource;
   }

   int CApi::loadConfig(CRadSource* pRadSource, const std::string& filename)
   {
      if (!pRadSource) return 0;
      bool success = pRadSource->loadConfig(filename.c_str());
      if (!success) return 0;
      // This code is the same as in mix_()
      // Bin it
      std::vector<double> bins;
      for (int i = 0; i < 131; i++) {
         bins.push_back(50.+i*50);
      }
      pRadSource->getPhotonComputer().setBinning(CPhotonComputer::BIN_BREM,bins);

      // Compute discrete, brem, and cumulative distros
      pRadSource->getPhotonComputer().computeGammas(CPhotonComputer::INTENSITY);
      return 1;
   }

   void CApi::addIsotope(CRadSource* pRadSource, int z,
      int a, int m, double perc) {
      if (!pRadSource) return;
      CIsotope isotope(z,a,m);
      pRadSource->getDecayComputer().addInputItem(isotope,perc);
   }
    
   int CApi::mix(CRadSource* pRadSource, double age) {
      if (!pRadSource) return 0;
      pRadSource->getDecayComputer().ageMixture(age);

      // Bin it
      std::vector<double> bins;
      for (int i = 0; i < 131; i++) {
         bins.push_back(50.+i*50);
      }
      pRadSource->getPhotonComputer().setBinning(CPhotonComputer::BIN_BREM,bins);

      // Compute discrete, brem, and cumulative distros
      pRadSource->getPhotonComputer().computeGammas(CPhotonComputer::INTENSITY);
      return 1;
   }

   void CApi::sort(CRadSource* pRadSource, int field) {
      if (!pRadSource) return;
      if (field == 1) pRadSource->getPhotonComputer().computeGammas(CPhotonComputer::ENERGY);
      else if (field == 2) pRadSource->getPhotonComputer().computeGammas(CPhotonComputer::INTENSITY);
      return;
   }
   int CApi::nLines(const CRadSource* pRadSource) {
      if (!pRadSource) return 0;
      return pRadSource->getPhotonComputer().getNGammas();
   }
   void CApi::getLines(CRadSource* pRadSource, double lines[][2], int nmax) {
      if (!pRadSource) return;
      CPhotonComputer::CPhotonIterator it;
      it = pRadSource->getPhotonComputer().beginGammas();
      int i = 0;
      while (it != pRadSource->getPhotonComputer().endGammas() && i < nmax) {
         lines[i][0] = it->getEnergy();
         lines[i][1] = it->getIntensity();
         ++it;
         ++i;
      }
   }
   void CApi::sampleBrem(CRadSource* pRadSource, bool f) {
      if (!pRadSource) return;
      pRadSource->getPhotonComputer().sampleBrem(f);
   }
   double CApi::getPhoton(const CRadSource* pRadSource, double (*prng)(void)) {
      if (!pRadSource) return 0.0;
      return pRadSource->getPhotonComputer().getPhoton(prng);
   }

   // return energy, px, py, pz
   void CApi::get4V(const CRadSource* pRadSource, double e[4], double (*prng)(void)) {
      if (!pRadSource) {
         e[0] = e[1] = e[2] = e[3] = 0.0;
         return;
      }
      pRadSource->getPhotonComputer().getFourVector(e,prng);
   }

   double CApi::getPhoton(const CRadSource* pRadSource, float (*prng)(void)) {
      if (!pRadSource) return 0.0;
      return pRadSource->getPhotonComputer().getPhoton(prng);
   }

   // return energy, px, py, pz
   void CApi::get4V(const CRadSource* pRadSource, double e[4], float (*prng)(void)) {
      if (!pRadSource) {
         e[0] = e[1] = e[2] = e[3] = 0.0;
         return;
      }
      pRadSource->getPhotonComputer().getFourVector(e,prng);
   }

   // Store a summary into a character variable
   std::string CApi::getReport(const CRadSource* pRadSource) {
      if (!pRadSource) {
         return 0;
      }
      std::string report = pRadSource->getReport("RadSrc: ");
      const char lines[] = "------------------------\n";
      report.insert(0,lines);
      report.append(lines);
      return report;
   }

   void CApi::addConfig(CRadSource* pRadSource, const std::string& str) {
      if (!pRadSource) return;
      std::string tmp = str;
      tmp += ' ';
      pRadSource->addConfigInfo(tmp);
   }

   int CApi::sourceConfig(CRadSource* pRadSource) {
      if (!pRadSource) {
         return false;
      }
      int success = pRadSource->processConfigInfo();
      if (!success) return 0;
      // This code is the same as in mix_()
      // Bin it
      std::vector<double> bins;
      for (int i = 0; i < 131; i++) {
         bins.push_back(50.+i*50);
      }
      pRadSource->getPhotonComputer().setBinning(CPhotonComputer::BIN_BREM,bins);
 
      // Compute discrete, brem, and cumulative distros
      pRadSource->getPhotonComputer().computeGammas(CPhotonComputer::INTENSITY);
      return 1;

   }
}
