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

#include "radsource.h"
#include "isotopedb.h"

namespace radsrc {
   CRadSource::CRadSource(void) : m_photonComputer(m_decayComputer) {
   }

#if 0
   CRadSource::CRadSource(const CRadSource& rs) :
         m_decayComputer(rs.m_decayComputer), m_photonComputer(rs.m_photonComputer)
   {
   }

   CRadSource& CRadSource::operator=(const CRadSource& rs) {
      m_decayComputer = rs.m_decayComputer;
      m_photonComputer = rs.m_photonComputer;
      return *this;
   }
#endif

   CRadSource::~CRadSource(void) {
      // all memory is automatically deleted
   }

   /*!
    * This function is most useful when libradsrc is linked to applications that
    * have very limited options for users to interact with the library.
    * Performs the following steps:
    * - Loads a legacy database according to the RADSRC_LEGACYDATA or RADSRC_DATA
    * environment variables, or from ./data.
    * - Loads a configuration file from RADSRC_CONFIG.
    * - Sets up a simple binning profile for bremsstrahlung.
    * - Computes the decay concentrations
    * - Computes the photon distributions
    */

   bool CRadSource::setupDefault(void) {
      CIsotopeDatabase* pisotopedb = CDatabaseManager::getIsotopeDatabase(CDatabaseManager::LEGACY);
      if (pisotopedb == 0) {
         return false;
      }
      m_decayComputer.initialize(pisotopedb);
      if(!loadConfig()) {
       return 0;
      }
      std::vector<double> bins;
      for (int i = 0; i<131; i++) {
       bins.push_back(50.+i*50);
      }
      m_photonComputer.setBinning(CPhotonComputer::BIN_BREM,bins);
      m_photonComputer.computeGammas(CPhotonComputer::INTENSITY);
      return true;
   }

   /*!
    * Opens a specified path to read configuration information, or
    * if no path is given, then check RADSRC_CONFIG environment
    * variable for a filename.
    * \return true if successful, false if not.
    */
   bool CRadSource::loadConfig(const char *filename) {
      if (filename == 0 || *filename == '\0') {
         filename = getenv("RADSRC_CONFIG");
         if (filename == 0)
            return false;
      }
      std::ifstream ifs(filename,std::ios_base::binary);
      if (ifs.is_open() == false) return false;
      return loadConfig(ifs);
   } 

   inline bool isAgeKeyword(const std::string& word) {
      return word.length() == 3 &&
         (word[0] == 'a' || word[0] == 'A') &&
         (word[1] == 'g' || word[1] == 'G') &&
         (word[2] == 'e' || word[2] == 'E');
   }

   inline bool isBremKeyword(const std::string& word) {
      return word.length() == 4 &&
         (word[0] == 'b' || word[0] == 'B') &&
         (word[1] == 'r' || word[1] == 'R') &&
         (word[2] == 'e' || word[2] == 'E') &&
         (word[3] == 'm' || word[3] == 'M');
   }

   inline bool isRangeKeyword(const std::string& word) {
      return word.length() == 5 &&
         (word[0] == 'r' || word[0] == 'R') &&
         (word[1] == 'a' || word[1] == 'A') &&
         (word[2] == 'n' || word[2] == 'N') &&
         (word[3] == 'g' || word[3] == 'G') &&
         (word[4] == 'e' || word[4] == 'E');
   }


   /*!
    * Reads in a problem specification from an open istream.
    * Problem specifications are a series of isotopes and
    * concentrations (in percent by weight) followed by
    * the word "age" and a value in years. All sequences of
    * only whitespace are treated as a single word delimiter.
    * \return true if successful, false if not.
    */
   bool CRadSource::loadConfig(std::istream& isInput) {
      std::string word;
      double age;
      try {
      //bool need_argument = false;
         while (true) {
            getWord(isInput, word);
            if (isAgeKeyword(word)) {
               //need_argument = true;
               getWord(isInput, word);
               age = atof(word.c_str());
               break;
            }
            if (isBremKeyword(word)) {
               //need_argument = true;
               getWord(isInput,word);
               if (word == "off" || word == "OFF" || word == "Off")
                  m_photonComputer.sampleBrem(false);
               else if (word == "on" || word == "ON" || word == "On")
                  m_photonComputer.sampleBrem(true);
               else throw (int) 0;
               continue;
            }
            else if (isRangeKeyword(word)) {
               //need_argument = true;
               getWord(isInput,word);
               double min = atof(word.c_str());
               getWord(isInput,word);
               double max = atof(word.c_str());
               if (max == 0.0)
                   // Assume invalid string
                   throw (int) 0;
               else if (min <= max)
                   m_photonComputer.setLinesRange(min,max);
               else throw (int) 0;
               continue;
            }
            CIsotope isotope;
            isotope.fromString(word);
            if (isotope.getAtomicNumber() > 0) {
               getWord(isInput, word);
               double percent = atof(word.c_str());
               m_decayComputer.addInputItem(isotope, percent/100.0);
               continue;
            }
            throw (int) 0; // unrecognized keyword before AGE
         }
      }
      catch(...) {
         return false;
      }
      m_decayComputer.normalizeInputUnits();
      m_decayComputer.ageMixture(age);
      return true;
  }
   void CRadSource::getWord(std::istream& is, std::string& word) {
     word = "";
     char c;
     while (is.good()) {
        is.get(c);
        if (c == '#') {
           while (is.good()) {
              c = is.get();
              if (c == '\r' || c == '\n') break;
           }
           continue;
        }
        if (c > 32 && c < 127) break;
     }
     if (is.bad() || is.eof()) throw (int) 0;
     word = c;
     while (is.good()) {
        is.get(c);
        if (c <= 32) break;
        word += c;
     }
   }
   /*! Assemble a summary of the calculation, including the input
    *  mixture, the aged mixture, and the number of photon lines emitted.
    *  \param[out] pPrefix an optional string to be prepended to each line.
    *  \return a string containing formatted output
    */
   std::string CRadSource::getReport(const char *pPrefix) const {
      std::vector<std::string> vReportLines;
      std::string sReport;
      getReport(vReportLines);
      if (pPrefix == 0) {
         pPrefix = "";
      }
      for (int i = 0; i < static_cast<int>(vReportLines.size()); i++) {
         sReport += pPrefix;
         sReport += vReportLines[i];
      }
      return sReport;
   }

   /*! Assemble a summary of the calculation, including the input
    *  mixture, the aged mixture, and the number of photon lines emitted.
    *  \param[out] vReportLines a vector of strings containing lines of the report.
    */
   void CRadSource::getReport(std::vector<std::string>& vReportLines) const {
      std::stringstream ss;
      time_t now;
      time(&now);
      vReportLines.resize(0);
      vReportLines.push_back(std::string("RadSource Run: ") + ctime(&now)); // implict EOL
      vReportLines.push_back("\n");
      CIsotopeMixture::const_iterator it;
      it = m_decayComputer.getInputMixture().begin();

      // Print the input mixture
      vReportLines.push_back("Input Isotopes\n");
      double total;
      total = 0;
      while (it != m_decayComputer.getInputMixture().end()) {
         ss.str("");
         ss << it->first.toString().c_str() << "  " << it->second * 100 << "%\n";
         vReportLines.push_back(ss.str());
         total += it->second;
         ++it;
      }
      ss.str("");
      ss << "Total: " << total*100 << "%\n";
      vReportLines.push_back("\n");
      vReportLines.push_back(ss.str());
      ss.str("");
      ss << "Age: " << m_decayComputer.getAge() << " s, "
         << m_decayComputer.getAge() / m_decayComputer.m_secs_per_year
         << " yrs " << std::endl;
      vReportLines.push_back("\n");
      vReportLines.push_back(ss.str());

      // Print the computed aged mixture
      ss.str("");
      vReportLines.push_back("Output Isotopes\n");
      it = m_decayComputer.getAgedMixture().begin();
      total = 0;
      while (it != m_decayComputer.getAgedMixture().end()) {
         ss.str("");
         ss << it->first.toString().c_str() << "  " << it->second * 100 << "% ( from ";
         std::set<CIsotope>::const_iterator pit;
         const CBatemanSolution* pbs = m_decayComputer.getSolution(it->first);
         if (pbs == 0) {
            ss << "Huh? No entry for this isotope in the database!";
         }
         else {
            const std::set<CIsotope> chainparents = pbs->getChainParents();
            pit = chainparents.begin();
            while(pit != chainparents.end()) {
               ss << pit->toString().c_str() << ' ';
               ++pit;
            }
         }
         ss << ")" << std::endl;
         vReportLines.push_back(ss.str());
         total += it->second;
         ++it;
      }
      ss.str("");
      ss << "Total: " << total*100 << "%\n";
      vReportLines.push_back("\n");
      vReportLines.push_back(ss.str());

      double source_min, source_max;
      m_photonComputer.getLinesRange(source_min,source_max);
      ss.str("");
      ss << m_photonComputer.getNGammas() << " lines computed";
      if (source_max) {
         ss << " between " << source_min << " kev and " << source_max << " kev";
      }
      ss << ".\n";
      vReportLines.push_back(ss.str());

      ss.str("");
      ss << "Total Gamma Line Intensity: "
         << m_photonComputer.getLineIntensity() << " ph/s/gm\n";
      vReportLines.push_back(ss.str());
      if (m_photonComputer.sampleBrem()) {
         ss.str("");
         ss << "Total Bremsstrahlung Intensity: "
            << m_photonComputer.getBremIntensity() << " ph/s/gm\n";
         vReportLines.push_back(ss.str());
      }
      ss.str("");
      ss << "Total Intensity: "
         << m_photonComputer.getTotalIntensity() << " ph/s/gm\n";
      vReportLines.push_back(ss.str());
   }
   int CRadSource::processConfigInfo() {
      std::stringstream ss(config);
      return loadConfig(ss);
   }


}
