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


#ifndef CONTRIBUTIONS_H_INCLUDE
#define CONTRIBUTIONS_H_INCLUDE
#include "porting.h"

#include <set>
#include <map>
#include "isotope.h"

namespace radsrc
{
   struct SContribution {
      CIsotope m_isotope;
      double m_decayrate;
      HighPrecisionType m_coefficient;
      SContribution(void) : m_coefficient(0) { }
   };
   
   
   class CBatemanSolution
   {
   public:
      CBatemanSolution(void) { }
      ~CBatemanSolution(void) { }
      const CIsotope& forIsotope(void) const;
      void addContribution(const SContribution& contribution);
      const std::set<CIsotope>& getChainParents(void) const;
      void setIsotope(const CIsotope& isotope);
      void addChainParent(const CIsotope& isotope);
      HighPrecisionType eval(double age) const;
   private:
      std::set<CIsotope> m_chainParents;
      std::map<CIsotope,SContribution> m_terms;
      CIsotope m_whoami;
   };

   inline const CIsotope&
   CBatemanSolution::forIsotope() const {
      return m_whoami;
   }
   inline const std::set<CIsotope>&
   CBatemanSolution::getChainParents() const
   {
      return m_chainParents;
   }
   inline void
   CBatemanSolution::setIsotope(const CIsotope&isotope)
   {
      m_whoami = isotope;
   }
   inline void
   CBatemanSolution::addChainParent(const CIsotope& isotope)
   {
      m_chainParents.insert(isotope);
   }
   inline void
   CBatemanSolution::addContribution(const SContribution& c)
   {
      SContribution& contribution = m_terms[c.m_isotope];
      contribution.m_isotope = c.m_isotope;
      contribution.m_decayrate = c.m_decayrate;
      contribution.m_coefficient += c.m_coefficient;
   }
}

#endif
