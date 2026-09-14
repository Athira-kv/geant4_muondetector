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


/* legacy.h
 * 
 * Structures to hold the information in a legacy .GAM file
 * 
 * Created 7/24/2006 Larry Hiller
 *
 */ 

#ifndef LEGACY_H_INCLUDE
#define LEGACY_H_INCLUDE

#include <vector>
#include <string>

namespace radsrc {
   struct CLegacyGamma {
      double m_energy; // keV
      double m_fraction;
      double m_uncertainty;
      char m_comment[6];
      CLegacyGamma(void) { m_comment[0] = m_comment[1] = m_comment[2] =
         m_comment[3] = m_comment[4] = m_comment[5] = '\0';
      }
   };

   struct CLegacyBrem {
       std::vector<double> m_bins;
       std::vector<double> m_intensity;
       int m_nbins;
   };

   struct CLegacyBranchMember {
       char m_Isotope[8];
       char m_DataFile[13];
       char m_BremFile[13];
       double m_HalfLife; // sec
       double m_HLUncertainty;
       int m_Type;  // alpha or beta or IT or stable;
       double m_Fraction;
       double m_FracUncertainty;
       bool m_fAdd;
   };

   struct CLegacyData {
      char m_Isotope[8];
      double m_AtomicMass;
      double m_systematic;
      std::vector<CLegacyGamma> m_ParentGammas;
      std::vector<std::vector<CLegacyBranchMember> > m_Branches;
   };

   bool ReadLegacyData(CLegacyData& legacydata, const char *dir, const char *isotope, const std::vector<std::string>& parent_list);
   bool ReadLegacyBrem(CLegacyBrem& legacybrem, const char *dir, const char *isotope);
   inline CLegacyData* ReadLegacyData(const char *dir, const char *isotope, const std::vector<std::string>& list) {
      CLegacyData *ptr = new CLegacyData;
      if (ptr && ReadLegacyData(*ptr,dir,isotope,list)) return ptr;
      return 0;
   }
   void ReadLegacyIndex(const char *, std::vector<std::string>&); 
}
#endif

