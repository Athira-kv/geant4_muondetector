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


#include "porting.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <time.h>

#include "decaycomputer.h"
#include "isotopedb.h"

namespace radsrc
{
   const double CDecayComputer::m_secs_per_year = 3.1536E+07;

   class CDecayComputer::CPathInfo {
   public:
      CPathInfo(void) : decayrate(0), fraction(0) { }
      CPathInfo(const CIsotope& iso, double d, double f) : isotope(iso), decayrate(d), fraction(f) { }
      void setInfo(const CIsotope& iso, double d, double f) { isotope = iso; decayrate = d; fraction = f; }
      std::vector<HighPrecisionType> a; // tmp storage, coefficient for partial sum of exponentials
      CIsotope isotope;
      double decayrate; // element decay rate
      double fraction; // branching ratio to next isotope in chain
   };
   
   CDecayComputer::CDecayComputer() : m_database(0), m_age(0)
   {
      m_database = CDatabaseManager::getIsotopeDatabase(CDatabaseManager::LEGACY);
      m_inputMixture.setDatabase(m_database);
      m_agedMixture.setDatabase(m_database);
      m_inputMixtureUnits = ATOM_FRACTION;
   }
   
   CDecayComputer::CDecayComputer(const CDecayComputer& dc) :
         m_inputMixture(dc.m_inputMixture),
         m_agedMixture(dc.m_agedMixture),
         m_solution(dc.m_solution),
         m_database(dc.m_database),
         m_inputMixtureUnits(dc.m_inputMixtureUnits),
         m_age(dc.m_age)
   {
   }
   
   CDecayComputer& CDecayComputer::operator=(const CDecayComputer& dc)
   {
      m_inputMixture = dc.m_inputMixture;
      m_agedMixture = dc.m_agedMixture;
      m_inputMixtureUnits = dc.m_inputMixtureUnits;
      m_database = dc.m_database;
      m_solution = dc.m_solution;
      m_age = dc.m_age;
      return *this;
   }
   
   CDecayComputer::~CDecayComputer(void)
   {
      // all memory is automatically deleted
   }
   
   /*! N.B. only Atomic Fraction is accepted as input at this time,
    *  so no conversion is actually done.
    *  This function is called after input has concluded.
    *  \todo Allow other input units besides atomic fraction.
    */
   void
   CDecayComputer::normalizeInputUnits(void)
   {
      if (m_inputMixtureUnits != ATOM_FRACTION) {
         throw CRadSourceException();
      }
   }

   CIsotopeMixture& CDecayComputer::ageMixture(double age) {
      if (m_database == 0) {
         throw CRadSourceException();
      }
      m_agedMixture.clear();
      ChainPath helper;
      age = age * m_secs_per_year; 
      CIsotopeMixture::const_iterator it = m_inputMixture.begin();
      m_solution.clear();
      while (it != m_inputMixture.end()) {
         helper.clear();
         buildAgingChain(age,helper,it->first,it->second);
         ++it;
      }
      m_age = age;
      return m_agedMixture;
   }
   
   /* compute the amount of the current last isotope in the current chain */
   /* ** Assumes all half-lives are unique! ** */
   void CDecayComputer::computeAddAgedIsotope(double age, const CIsotope& iso, double atomicFraction, ChainPath& pathHistory) {
      HighPrecisionType quantity;
      int n = pathHistory.size();
      std::vector<HighPrecisionType>& cur_coeffs = pathHistory.back().a;
      double child_decay = pathHistory.back().decayrate;
      int i;
      if (n == 1) {
         // This code will only execute once in a decay graph
         pathHistory[0].a.resize(1);
         pathHistory[0].a[0] = 1.0;
      } else {
         CPathInfo& parent_info = pathHistory[n-2];
         double parent_decay = parent_info.decayrate * parent_info.fraction;
         pathHistory.back().a.resize(n);
         HighPrecisionType sum;
         sum = 0.0;
         for (i = 0; i < n-1; i++) {
            if (child_decay == pathHistory[i].decayrate) std::cout << "Error, halflives equal" << std::endl;
            cur_coeffs[i] = parent_info.a[i] * parent_decay / (child_decay - pathHistory[i].decayrate);
            sum += cur_coeffs[i];
         }
         cur_coeffs[n-1] = - sum;
      }
      quantity = 0;
      m_solution[pathHistory.back().isotope].setIsotope(pathHistory.back().isotope);
      m_solution[pathHistory.back().isotope].addChainParent(pathHistory[0].isotope);
      SContribution contribution;
      for (i = 0; i < n; i++) {
         contribution.m_isotope = pathHistory[i].isotope;
         contribution.m_decayrate = pathHistory[i].decayrate;
         contribution.m_coefficient = cur_coeffs[i]*atomicFraction;
         m_solution[pathHistory.back().isotope].addContribution(contribution);
      }
       for (i = 0; i < n-1; i++) {
         quantity += cur_coeffs[i] * Exp(-pathHistory[i].decayrate*age);
      }
      if (child_decay) {
         quantity += cur_coeffs[n-1] * Exp(-child_decay*age);
      } else {
         quantity += cur_coeffs[n-1];
      }
      if (quantity < 0) quantity = 0;
      m_agedMixture[pathHistory.back().isotope] += atomicFraction * quantity;
   }
   
   /* Walk all paths of the decay graph
   * helper contains all parents of the current isotope in the current chain
   */
   void CDecayComputer::buildAgingChain(double age, ChainPath& pathHistory, CIsotope iso, double atomicFraction) {
      int n; // n is the number of decay branches
      n = 1;
      CPathInfo info;
      const CIsotopeData* pisotopeData;
      try {
         while (n==1) {
            // isotopedata is the contributing isotope
            pisotopeData = m_database->getIsotopeData(iso);
            if (pisotopeData == 0) {  // Error in problem input, quit
               throw (int) 0;
            }
            info.setInfo(iso,0.0,0.0);
            n = pisotopeData->getNDecayModes();
            if (n == 0) { // stable, calc last element and return
               pathHistory.push_back(info);
               computeAddAgedIsotope(age,iso,atomicFraction,pathHistory);
               return;
            }
            // one or more children
            info.setInfo(iso,pisotopeData->getDecayRate(),1.0);
            pathHistory.push_back(info);
            computeAddAgedIsotope(age,iso,atomicFraction,pathHistory);
            iso = pisotopeData->decaysBegin()->getDaughter();
         }
      }
      catch (CRadSourceException) {
         // Missing Isotope
         return;
      }
      // if here, n >= 2 and parent has been pushed, assuming n = 1

      // branch point is here. reset to branch point to follow n branches
      int pos_index = pathHistory.size();
      CIsotopeData::DecayIterator it, endit;
      it = pisotopeData->decaysBegin();
      endit = pisotopeData->decaysEnd();
      while (it != endit) {
         // branch fraction for this branch
         pathHistory.back().fraction = it->getBranchFraction();
         buildAgingChain(age,pathHistory, it->getDaughter(), atomicFraction);
         // reset to branch point
         pathHistory.resize(pos_index);
         ++it;
      }
   }
   
   
   void CDecayComputer::addInputItem(const CIsotope& iso, double amount) {
      m_inputMixture[iso] = amount;
   }
   
   void CDecayComputer::clear() {
      m_inputMixture.clear();
   }
   
   const CIsotopeData* CDecayComputer::getIsotopeData(const CIsotope& isotope) const {
      return m_database->getIsotopeData(isotope);
   }
   DecayChainSet& CDecayComputer::computeDecayChains(const CIsotope& iso) const {
      DecayChainSet *pdecayset = new DecayChainSet;
      pdecayset->resize(1);
      buildChain(*pdecayset,iso);
      return *pdecayset;
   }

   void CDecayComputer::buildChain(DecayChainSet& decaychain, CIsotope iso) const {
      int cur_chain = decaychain.size()-1;
      int n;
      const CIsotopeData *pisotopeData;
      n = 1;
      CDecay decayinfo;
      try {
         while (n==1) {
            pisotopeData = getIsotopeData(iso);
            decayinfo.m_Parent = iso;
            decayinfo.m_pDecay = 0;
            n = pisotopeData->getNDecayModes();
            if (n == 0) {
               decaychain[cur_chain].push_back(decayinfo);
               return;
            }
            decayinfo.m_pDecay = &(*pisotopeData->decaysBegin());
            decaychain[cur_chain].push_back(decayinfo);
            iso = decayinfo.m_pDecay->getDaughter();
         }
      }
      catch (CRadSourceException) {
         return;
      }
      // if here, n >= 2 and branching parent has been pushed
      int pos_index = decaychain[cur_chain].size();
      CIsotopeData::DecayIterator it, endit;
      it = pisotopeData->decaysBegin();
      endit = pisotopeData->decaysEnd();
      while (it != endit) {
          decayinfo.m_pDecay = &(*it);
          decaychain.back().push_back(decayinfo);
          buildChain(decaychain,it->getDaughter());
          // clone off previous change up to before the parent
          decaychain.resize(decaychain.size()+1);
          decaychain.back().assign(
             decaychain[cur_chain].begin(),
             decaychain[cur_chain].begin()+pos_index);
          ++it;
      }
   }
}
