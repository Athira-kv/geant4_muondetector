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
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include "mcinput.h"

MCNPInputFile::MCNPInputFile() : m_scale(0.001)
{
}

MCNPInputFile::~MCNPInputFile()
{
}

void MCNPInputFile::WriteFile(const std::string& filename, MCInput& InputValues) const {
   std::ofstream fsMCIFile(filename.c_str());
   if (fsMCIFile.is_open() == false) {
      return;
   }

   if (InputValues.header.size()) {
      fsMCIFile << InputValues.header.c_str();
      if (InputValues.vGammaEnergies.size()) {
         fsMCIFile << std::endl << "C " << InputValues.vGammaEnergies.size()
            << " unbinned lines between " << InputValues.vGammaEnergies.front()
            << " keV and " << InputValues.vGammaEnergies.back()
            << " keV" << std::endl;
      }
      fsMCIFile << "C Intensity of Unbinned lines is " << InputValues.totals[MCInput::LINE_TOTAL] << std::endl;
      fsMCIFile << "C Intensity of Binned lines is " << InputValues.totals[MCInput::BINNED_TOTAL] << std::endl;
      fsMCIFile << "C Intensity of Bremmstrahlung " << InputValues.totals[MCInput::BREM_TOTAL] << std::endl;
      fsMCIFile << "C Total intensity of all sources is " << InputValues.totals[MCInput::TOTAL_TOTAL] << std::endl;
      fsMCIFile << "C ========================================================";
      fsMCIFile << std::endl;
   }

   char buffer[256];

   // If we have discrete Gammas
   if (InputValues.vGammaEnergies.size()) {
      sprintf(buffer,"SC1      DISCRETE GAMMA LINE energies (MeV) -%5lu lines:",
         InputValues.vGammaEnergies.size());
      fsMCIFile << buffer << std::endl;
      WriteData(InputValues.vGammaEnergies,fsMCIFile,"SI1      L", m_scale);
      fsMCIFile << "C " << std::endl;
      fsMCIFile << "SC1      ASSOCIATED photon intensities (photons/sec/gm):" << std::endl;
      WriteData(InputValues.vGammaIntensities,fsMCIFile,"SP1      D");
      sprintf(buffer,"%12E",InputValues.totals[MCInput::LINE_TOTAL]);
      fsMCIFile << "C        The integrated intensity for this input type is "
         << buffer << " Photons/s/gm" << std::endl;
      fsMCIFile << "C " << std::endl;
      fsMCIFile << "C " << std::endl;

   }

   // If we have Binned Gammmas
   if (InputValues.vGammaBinBoundaries.size()) {
      sprintf(buffer,"SC2        Energy boundries (MeV) for BINNED GAMMA LINES -%4lu bins:",
         InputValues.vGammaBinBoundaries.size()-1);
      fsMCIFile << buffer << std::endl;
      WriteData(InputValues.vGammaBinBoundaries,fsMCIFile,"SI2        H", m_scale);
      fsMCIFile << "C " << std::endl;
      fsMCIFile << "SC2        ASSOCIATED photon intensities (photons/sec/gm):" << std::endl;
      WriteData(InputValues.vGammaBinIntensities,fsMCIFile,"SP2        D");
      sprintf(buffer,"%12E",InputValues.totals[MCInput::BINNED_TOTAL]);
      fsMCIFile << "C        The integrated intensity for this input type is "
         << buffer << " Photons/s/gm" << std::endl;
      fsMCIFile << "C " << std::endl;
      fsMCIFile << "C " << std::endl;
   }

   // If we have Brem
   if (InputValues.vBremBinBoundaries.size()) {
      sprintf(buffer,"SC3      BREMSSTRAHLUNG energy bin boundries (MeV) -%4lu bins:",
         InputValues.vBremBinBoundaries.size()-1);
      fsMCIFile << buffer << std::endl;
      WriteData(InputValues.vBremBinBoundaries,fsMCIFile,"SI3      H", m_scale);
      fsMCIFile << "C " << std::endl;
      fsMCIFile << "SC3      ASSOCIATED photon intensities (photons/sec/gm):" << std::endl;
      WriteData(InputValues.vBremIntensities,fsMCIFile,"SP3      D");
      sprintf(buffer,"%12E",InputValues.totals[MCInput::BREM_TOTAL]);
      fsMCIFile << "C        The integrated intensity for this input type is "
         << buffer << " Photons/s/gm" << std::endl;
      fsMCIFile << "C" << std::endl;
      fsMCIFile << "C" << std::endl;
   }

   // Tally
   fsMCIFile << "FU104    0 1 nt $ Tally SCATTERED and UNSCATTERED flux separately" << std::endl;;
   fsMCIFile << "C" << std::endl;
   sprintf(buffer,"E104     $ UNSCATTERED FLUX TALLY, energy bin boundries (MeV) -%5lu bins:",
      InputValues.vTallyUnscattered.size()-1);
   fsMCIFile << buffer << std::endl;
   WriteData(InputValues.vTallyUnscattered,fsMCIFile,"          ", m_scale);
   fsMCIFile << "C" << std::endl;
   fsMCIFile << "C" << std::endl;
   fsMCIFile << "FU204    0 1 nt $ Tally SCATTERED and UNSCATTERED flux separately" << std::endl;;
   fsMCIFile << "C" << std::endl;
   sprintf(buffer,"E204     $ SCATTERED FLUX TALLY, energy bin boundries (MeV) -%5lu bins:",
      InputValues.vTallyScattered.size()-1);
   fsMCIFile << buffer << std::endl;
   WriteData(InputValues.vTallyScattered,fsMCIFile,"          ", m_scale);
   fsMCIFile << "C" << std::endl;
   sprintf(buffer,"%12E",InputValues.totals[MCInput::TOTAL_TOTAL]);
   fsMCIFile << "C       The integrated intensity for ALL INPUT TYPES is " <<
      buffer << " Photons/s/gm" << std::endl;
   fsMCIFile.close();
}

void MCNPInputFile::WriteData(const std::vector<double>& array, std::ostream& os, const char *identifier, double scale) const {
   static const char blank[11] = "          ";
   char buffer[15];
   size_t i,j;
   os << identifier;
   for (j = 0; j < 4 && j < array.size(); j++) {
      sprintf(buffer," %13.6E", array[j]*scale);
      os << buffer;
   }
   os << std::endl;
   for (i = 4; i < array.size(); i+=4) {
      os << blank;
      for (j = 0; j < 4 && i+j < array.size(); j++) {
        sprintf(buffer," %13.6E", array[i+j]*scale);
        os << buffer;
      }
      os << std::endl;
   }
}

/* -------------------------------------------- */


COGInputFile::COGInputFile() : m_scale(0.001)
{
}

COGInputFile::~COGInputFile()
{
}

void COGInputFile::WriteFile(const std::string& filename, MCInput& InputValues) const {
   std::ofstream fsMCIFile(filename.c_str());
   if (fsMCIFile.is_open() == false) {
      return;
   }

   if (InputValues.header.size()) {
      fsMCIFile << InputValues.header.c_str();
      if (InputValues.vGammaEnergies.size()) {
         fsMCIFile << std::endl << "$ " << InputValues.vGammaEnergies.size()
            << " unbinned lines between " << InputValues.vGammaEnergies.front()
            << " keV and " << InputValues.vGammaEnergies.back()
            << " keV" << std::endl;
      }
      fsMCIFile << "$ Total Intensity of Unbinned lines is " << InputValues.totals[MCInput::LINE_TOTAL] << std::endl;
      fsMCIFile << "$ Total Intensity of Binned lines is " << InputValues.totals[MCInput::BINNED_TOTAL] << std::endl;
      fsMCIFile << "$ Intensity of Bremmstrahlung " << InputValues.totals[MCInput::BREM_TOTAL] << std::endl;
      fsMCIFile << "$ Total intensity of all sources is " << InputValues.totals[MCInput::TOTAL_TOTAL] << std::endl;
      fsMCIFile << "$ ========================================================";
      fsMCIFile << std::endl;
   }

   char buffer[256];
   int deckcounter = 1;

   // If we have discrete Gammas
   if (InputValues.vGammaEnergies.size()) {
      sprintf(buffer,"     DEFINE ENERGY =   %d  PHOTON\nLINE       $%7lu Lines", deckcounter,
         InputValues.vGammaEnergies.size());
      fsMCIFile << buffer << std::endl;
      WriteData(InputValues.vGammaEnergies,InputValues.vGammaIntensities,false,fsMCIFile, m_scale);
      fsMCIFile << "$" << std::endl;
      fsMCIFile << "$  " << deckcounter << "    ASSOCIATED photon intensities (photons/sec/gm):" << std::endl;
      sprintf(buffer,"%12E",InputValues.totals[MCInput::LINE_TOTAL]);
      fsMCIFile << "$        The integrated intensity for this input type is "
         << buffer << " Photons/s/gm" << std::endl;
      fsMCIFile << "$ " << std::endl;
      fsMCIFile << "$ " << std::endl;
      deckcounter++;

   }

   // If we have Binned Gammmas
   if (InputValues.vGammaBinBoundaries.size()) {
      sprintf(buffer,"     DEFINE ENERGY =   %d  PHOTON\nBIN       $%7lu Bins", deckcounter,
         InputValues.vGammaBinBoundaries.size()-1);
      fsMCIFile << buffer << std::endl;
      WriteData(InputValues.vGammaBinBoundaries,InputValues.vGammaBinIntensities,true,fsMCIFile, m_scale);
      fsMCIFile << "$ " << std::endl;
      fsMCIFile << "$  " << deckcounter << "    ASSOCIATED photon intensities (photons/sec/gm):" << std::endl;
      sprintf(buffer,"%12E",InputValues.totals[MCInput::BREM_TOTAL]);
      fsMCIFile << "$        The integrated intensity for this input type is "
         << buffer << " Photons/s/gm" << std::endl;
      fsMCIFile << "$" << std::endl;
      fsMCIFile << "$" << std::endl;
      deckcounter++;
   }

   // If we have Brem
   if (InputValues.vBremBinBoundaries.size()) {
      sprintf(buffer,"SC  %d    BREMSSTRAHLUNG energy bin boundries (MeV) -%4lu bins:", deckcounter,
         InputValues.vBremBinBoundaries.size()-1);
      fsMCIFile << buffer << std::endl;
      WriteData(InputValues.vBremBinBoundaries,InputValues.vBremIntensities,true,fsMCIFile, m_scale);
      fsMCIFile << "$ " << std::endl;
      fsMCIFile << "$  " << deckcounter << "    ASSOCIATED photon intensities (photons/sec/gm):" << std::endl;
      sprintf(buffer,"%12E",InputValues.totals[MCInput::BREM_TOTAL]);
      fsMCIFile << "$        The integrated intensity for this input type is "
         << buffer << " Photons/s/gm" << std::endl;
      fsMCIFile << "$" << std::endl;
      fsMCIFile << "$" << std::endl;
      deckcounter++;
   }

   // Tally
   fsMCIFile
      << "DETECTOR NUMBER = 104    TITLE = UNSCAT BINS" << std::endl
      << "     REACTION" << std::endl
      << "     DRF" << std::endl
      << "     MASKS" << std::endl
      << "     BIN ENERGY = PHOTON" << std::endl
      << std::endl
      << "DETECTOR NUMBER = ***    TITLE = UNSCAT BINS" << std::endl
      << "     REACTION" << std::endl
      << "     DRF" << std::endl
      << "     MASKS" << std::endl
      << "     BIN ENERGY = PHOTON" << std::endl
      << std::endl;

   WriteData(InputValues.vTallyUnscattered,fsMCIFile, m_scale);

   fsMCIFile
      << "DETECTOR NUMBER = 204    TITLE = SCATRD BINS" << std::endl
      << "     REACTION" << std::endl
      << "     DRF" << std::endl
      << "     MASKS" << std::endl
      << "     BIN ENERGY = PHOTON" << std::endl
      << std::endl
      << "DETECTOR NUMBER = 100    TITLE = SCATRD BINS" << std::endl
      << "     REACTION" << std::endl
      << "     DRF" << std::endl
      << "     MASKS" << std::endl
      << "     BIN ENERGY = PHOTON" << std::endl
      << std::endl;

   WriteData(InputValues.vTallyScattered,fsMCIFile, m_scale);

   fsMCIFile << "$" << std::endl;
   sprintf(buffer,"%12E",InputValues.totals[MCInput::TOTAL_TOTAL]);
   fsMCIFile << "$       The integrated intensity for ALL INPUT TYPES is " <<
      buffer << " Photons/s/gm" << std::endl;
   fsMCIFile.close();
}

void COGInputFile::WriteData(const std::vector<double>& array, std::ostream& os, double scale) const {
   char buffer[15];
   size_t i,j;
   for (i = 0; i < array.size(); i+=4) {
      os << "            ";
      for (j = 0; j < 4 && i+j < array.size(); j++) {
        sprintf(buffer," %13.6E", array[i+j]*scale);
        os << buffer;
      }
      os << std::endl;
   }
}
void COGInputFile::WriteData(const std::vector<double>& array1, const std::vector<double>& array2, bool binned, std::ostream& os, double scale) const {
   char buffer[40];
   size_t i,j;

   size_t n = array1.size();
   if (array2.size() != n) {
      std::cerr << "Array length mismatch...not written" << std::endl;
      return;
   }
   for (i = 0, j = binned?1:0; i < n; i++,j++) {
      if (j < n) sprintf(buffer, " %13.6E %13.6E", array1[i] * scale, array2[j]);
      else sprintf(buffer, " %13.6E", array1[i] * scale);
      os << buffer << std::endl;
   }
}

