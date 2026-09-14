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


/* legacydata.cc
 *
 * Read in a legacy .GAM file and store all of it's information it
 * in a structure.
 *
 * Created 7/24/2006 L.H.
 * Modified to use an index of root-level files 1/25/2008
 *
 */

#include "porting.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>

#include "radsrcerr.h"
#include "legacydata.h"
#include "legacyfiles.h"

namespace radsrc {
   enum Exceptions { Error };
   extern const char *LegacyDataDir;
   bool LegacyFileHasChains(const char *, const std::vector<std::string>&);
   extern const char *DefaultLegacyDataDir;

   std::string MakeFilename(const char *dir, const char *isotope, bool brem=false) {
      std::string filename = dir;
      filename += '/';
      filename += isotope;
      if (brem)
         filename += ".BRM";
      else
         filename += ".GAM";
      return filename;
   }

   int ParseTypeString(const char c) {
      switch(c) {
         case 'S': return 0;
         case 'A': return 1;
         case 'B': return 2;
         case 'I': return 3;
         default:
            throw Error;
      }
   //   return 0; // Unreachable
   }

   void SizeString(char *ptr, int pos) {
      ptr[pos] = '\0';
      pos--;
      while (pos >= 0 && ptr[pos] == ' ') {
         ptr[pos] = '\0';
         pos--;
      }
      int i = 0;
      while (i < pos && ptr[i] == ' ') i++;
      if (i == 0)
         return;
      int j = 0;
      while (i <= pos) {
        ptr[j] = ptr[i];
        ++i;
        ++j;
      }
      ptr[j] = '\0';
   }

   void GetLine(std::ifstream& fs, std::string& buffer, int minlen) {
      std::getline(fs,buffer);
      if (!fs.good() || buffer.length() < static_cast<unsigned int>(minlen)) {
         throw Error;
      }
   }

   void EatLine(std::ifstream& fs) {
      char c;
      while (!fs.fail()) {
         c = fs.get();
         if (c == '\r' || c == '\n') break;
      }
      if (!fs.fail() && c == '\r') {
         c = fs.get();
         if (c != '\n') fs.unget();
      }
   }

   void ReadLegacyIndex(const char* dir, std::vector<std::string>& idlist)
   {
      std::string filename = dir;
      filename += "/Index";
      std::ifstream index(filename.c_str());
      if (!index.is_open()) throw CRadSourceException();
      std::string str;
 
      while(!index.fail()) {
         index >> str;
         if (str.length() == 0) continue;
         if (str[0] == '#') {
            EatLine(index);
            continue;
         }
         idlist.push_back(str);
      }
   }

   /* Read a .GAM file */
   bool ReadLegacyData(CLegacyData& legacydata, const char *dir, const char *isotope, const std::vector<std::string>& parent_list) {
      int i,j;
      std::string buffer;
      std::string filename = MakeFilename(dir, isotope);
      std::ifstream fs(filename.c_str());
      if (!fs.is_open()) {
         return false;
      }

      try {
        // skip 1 line
        GetLine(fs,buffer,0);
        // Read isotope and mass
        GetLine(fs,buffer,55);
        strncpy(legacydata.m_Isotope,buffer.c_str()+19,7);
        SizeString(legacydata.m_Isotope,7);
        legacydata.m_AtomicMass = atof(buffer.c_str()+54);
        // skip 5 lines
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        GetLine(fs,buffer,0); GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        // Read # gammas
        GetLine(fs,buffer,16);
        int n_gammas = atoi(buffer.c_str()+ 16);
        legacydata.m_ParentGammas.resize(0);
        // skip 1 line
        GetLine(fs,buffer,0);
        // Read Abundance type
        GetLine(fs,buffer,24);
        char type = buffer[24];
        if (type != '%' && type != 'R' && type != 'r') throw Error;
        // skip 1 line
        GetLine(fs,buffer,0);
        if (type != '%') {
           for (i = 0; i < n_gammas; i++) GetLine(fs,buffer,0);
           n_gammas = 0;
        }
        else {
           i = 0;
           while (i < n_gammas) {
              GetLine(fs,buffer,29);
              CLegacyGamma gamma;
              const char *bufstr = buffer.c_str();
              gamma.m_energy = atof(bufstr+5);
              gamma.m_fraction = atof(bufstr+17)/100.0;
              gamma.m_uncertainty = atof(bufstr+28)/100.0;
              if (buffer.length() > 41) {
                 strncpy(gamma.m_comment,bufstr+41,5);
                 SizeString(gamma.m_comment,5);
              }
              legacydata.m_ParentGammas.push_back(gamma);
              ++i;
           }
        }
        // skip 1 line
        GetLine(fs,buffer,0);
        // Read systematic error
        GetLine(fs,buffer,31);
        legacydata.m_systematic = atof(buffer.c_str()+30)/100.0;
        // Read Chains
        if (LegacyFileHasChains(isotope,parent_list)) {
           // skip 5 lines
           GetLine(fs,buffer,0); GetLine(fs,buffer,0);
           GetLine(fs,buffer,0); GetLine(fs,buffer,0); GetLine(fs,buffer,0);
           // Read # branches
           GetLine(fs,buffer,29);
           int n_branches = atoi(buffer.c_str()+ 26);
           legacydata.m_Branches.resize(n_branches);
           // skip 4 lines
           GetLine(fs,buffer,0); GetLine(fs,buffer,0);
           GetLine(fs,buffer,0); GetLine(fs,buffer,0);
           i = 0;
           while (i < n_branches) {
              // skip 1 line
              GetLine(fs,buffer,0);
              // Read number of members
              GetLine(fs,buffer,15);
              int n_members = atoi(buffer.c_str()+15);
              legacydata.m_Branches[i].resize(n_members);
              j = 0;
              while (j < n_members) {
                 CLegacyBranchMember& member = legacydata.m_Branches[i][j];
                 GetLine(fs,buffer,86);
                 const char *bufstr = buffer.c_str();
                 strncpy(member.m_Isotope,bufstr+5,7);
                 SizeString(member.m_Isotope,7);
                 strncpy(member.m_DataFile,bufstr+15,12);
                 SizeString(member.m_DataFile,12);
                 strncpy(member.m_BremFile,bufstr+28,12);
                 SizeString(member.m_BremFile,12);
                 member.m_HalfLife = atof(bufstr+40);
                 member.m_HLUncertainty = atof(bufstr+54);
                 member.m_Fraction = atof(bufstr+67);
                 member.m_FracUncertainty = atof(bufstr+78);
                 member.m_fAdd = (bufstr[86] == 'Y' || bufstr[86] == 'y');
                 member.m_Type = ParseTypeString(bufstr[61]);
                 j++;
              }
              ++i;
          }
        } // if has decay chains
      }
      catch (...) {
         fs.close();
         return false;
      }
      fs.close();
      return true;
   }

   bool ReadLegacyBrem(CLegacyBrem& legacybrem, const char *dir, const char *isotope) {
      std::string buffer;
      std::string filename = MakeFilename(dir, isotope,true);
      std::ifstream fs(filename.c_str());
      if (!fs.is_open()) {
         return false;
      }
      try {
        const char *bufferptr;
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        GetLine(fs,buffer,0);

        GetLine(fs,buffer,0);
        legacybrem.m_nbins = atoi(buffer.c_str());
        GetLine(fs,buffer,0);
        int n_spectra = atoi(buffer.c_str());

        if (n_spectra != 1) {
           std::cerr << "Warning: Multiple spectra in brem file not supported" << std::endl;
           std::cerr << "Only first spectrum read" << std::endl;
        }
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        int i,j;
        legacybrem.m_bins.resize(legacybrem.m_nbins+1);
        legacybrem.m_intensity.resize(legacybrem.m_nbins);
        for (i = 0; i <= legacybrem.m_nbins; i+=7) {
           GetLine(fs,buffer,1);
           bufferptr = buffer.c_str();
           for (j = 0; j < 7 && i+j <= legacybrem.m_nbins; j++) {
              legacybrem.m_bins[i+j] = atof(bufferptr + 11*j);
           }
        }
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        GetLine(fs,buffer,0); GetLine(fs,buffer,0);
        for (i = 0; i < legacybrem.m_nbins; i+=7) {
           GetLine(fs,buffer,1);
           bufferptr = buffer.c_str();
           for (j = 0; j < 7 && i+j < legacybrem.m_nbins; j++) {
              legacybrem.m_intensity[i+j] = atof(bufferptr + 11*j);
           }
        }
      }
      catch (...) {
         fs.close();
         return false;
      }
      fs.close();
      return true;
   }
}

      
