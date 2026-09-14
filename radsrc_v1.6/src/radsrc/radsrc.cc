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
#include "porting.h"
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <math.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "radsource.h"
#include "dbmanager.h"
#include "bins.h"
#include "useriface.h"
#include "mcinput.h"

#ifndef LSFUZZ
#define LSFUZZ (0.1)
#endif

using namespace radsrc;
using namespace std;

double getTotalConcentration(radsrc::CIsotopeMixture& isomix);
void InputConfig(CRadSource& radsource, Options& options, std::ostream& input_log);
void PrintMixture(const radsrc::CIsotopeMixture& radsource);
void PrintGammas(CRadSource& radsource, bool selected=false);
void SelectLines(CRadSource& radsource, Options& options);
void SetBins(CRadSource& radsource, const Options::SubOptions& options, CPhotonComputer::BinSubject what);
void PrepareMCInput(const CRadSource& radsource, const Options& options, MCInput& mci);
void SetTallyBins(MCInput& mci, const Options& options, const CRadSource& radsource);
void WriteLinesFile(const std::string& filename, const std::string& header, CRadSource& radsource);

int main(int argc, const char *argv[]) {

   if (CDatabaseManager::getIsotopeDatabase(CDatabaseManager::LEGACY) == 0) {
      std::cerr
         << "Unable to load database. Exiting..."
         << std::endl;
      return -1;
   }

   Options options;

   ParseCommandLine(argc,argv,options);

   CRadSource radsource;
   std::ifstream filestream;
   std::ostringstream input_log;
   if (options.interactive) {
      options.inputstream = &std::cin;
   } else {
      options.inputstream = &filestream;
      filestream.open(options.configfile.c_str());
      if (!filestream.is_open()) {
         std::cerr << "Unable to open config file '" << options.configfile.c_str() << "'." << std::endl;
         exit(-1);
      }
   }

   //
   // Input the initial Mixture and Compute aged mixture
   //
   InputConfig(radsource, options, input_log);

   if (!options.quiet) {
      // Print the Initial mixture
      cout << endl;
      std::cout << "Input composition: (fractional units)" << std::endl;
      PrintMixture(radsource.getDecayComputer().getInputMixture());
      std::cout << "Age: " << radsource.getDecayComputer().getAge() << " s, "
        << radsource.getDecayComputer().getAge() /
           radsource.getDecayComputer().m_secs_per_year
        << " yrs " << std::endl;

      /*
      // Print the aged mixture
      std::cout << std::endl;
      std::cout << "Aged Composition: (fractional units)" << std::endl;
      PrintMixture(radsource.getDecayComputer().getAgedMixture());
      */
   }

   // Check the total isotope concentration is at least 99.9% but not
   // more than 100.1%
   //
   CIsotopeMixture isotopeMix = radsource.getDecayComputer().getInputMixture();
   //PrintMixture(isotopeMix);
   double totalCon = getTotalConcentration(isotopeMix);
   //std::cout << "Total Concentration: " << totalCon <<
   //             "  diff = " << fabs(totalCon - 1.0) << std::endl;
   if (totalCon == 0.0) {
      std::cerr << "\nError: The total isotope concentration must be " <<
                "between 99.9% and 100%\n";
      return -1;
   }
   if (totalCon < 0.999) {
      std::cout << "\nWarning: The total isotope concentration is less than 99.9%\n" <<
        "  but unspecified isotopes are assumed to be inert and\n" <<
        "  processing is continuing.\n\n";
   }

   if (totalCon > 1.0) {
      std::cerr << "\nError: The total isotope concentration cannot be " <<
                "greater than 100%\n";
      return -1;
   }

   //
   // Input the desired binning energy ranges
   //
   GetOptions(options, input_log);

   std::ofstream input_log_file("output.def");
   if (input_log_file.is_open()) {
      input_log_file << input_log.str().c_str();
      input_log_file.close();
   }

   if (options.do_mci) {
      SetBins(radsource,options.brembin_options,CPhotonComputer::BIN_BREM);
      SetBins(radsource,options.gammabin_options,CPhotonComputer::BIN_GAMMA);

      if (options.lines != BIN_NONE) {
         options.tallyu_min = radsource.getPhotonComputer().getBinnedGammas().m_energy.front();
         options.tallyu_max = radsource.getPhotonComputer().getBinnedGammas().m_energy.back();
      } // else use defaults or user-specified range
   }

   try {
      radsource.getPhotonComputer().computeGammas();
   } catch (...) {
      std::cerr << "Problem computing gamma distributions from aged mixture." << std::endl;
      exit(-1);
   }

   if (options.interactive) {
      std::cout << radsource.getPhotonComputer().getNGammas() << " lines computed";
      if (options.source_max) {
         std::cout << " between " << options.source_min << " keV and " << options.source_max << " keV";
      }
      std::cout << "." << std::endl;
      std::cout << "Total Gamma Line Intensity: " << radsource.getPhotonComputer().getLineIntensity() << " ph/s/gm" << std::endl;
      if (options.sample_brem) {
         std::cout << "Total Bremsstrahlung Intensity: " << radsource.getPhotonComputer().getBremIntensity() << " ph/s/gm" << std::endl;
      }
   }
   MCInput mci;

   if (options.do_mci) {
      SelectLines(radsource,options);


      mci.header = radsource.getReport("C ");

      /*
      // print out the mci header
      if (!options.quiet) {
         std::cout << mci.header.c_str(); // implicit EOL
      }
      */

      PrepareMCInput(radsource, options, mci);

      MCNPInputFile mcnp;
      mcnp.WriteFile("output.mci", mci);

      COGInputFile cog;
      mci.header = radsource.getReport("$ ");
      cog.WriteFile("output.cog", mci);

      if (options.interactive) {
         if (mci.vGammaEnergies.size()) {
            std::cout << mci.vGammaEnergies.size() << " unbinned lines between "
               << mci.vGammaEnergies.front() << " keV and "
               << mci.vGammaEnergies.back() << " keV" << std::endl;
         }
         std::cout << "Intensity of Unbinned lines is " << mci.totals[MCInput::LINE_TOTAL] << std::endl;
         std::cout << "Intensity of Binned lines is " << mci.totals[MCInput::BINNED_TOTAL] << std::endl;
         std::cout << "Intensity of Bremsstrahlung " << mci.totals[MCInput::BREM_TOTAL] << std::endl;
         std::cout << "Total Intensity of all sources is " << mci.totals[MCInput::TOTAL_TOTAL] << std::endl;
      }
   }

   mci.header = radsource.getReport("");
   WriteLinesFile("output.lin", mci.header,radsource);

   if (options.interactive) {
     cout << endl;
     cout << endl;
     cout << "Gamma-ray spectrum and mixture composition written to files:" << std::endl;
     cout << "  output.lin = Human readable list of lines" << std::endl;
     if (options.do_mci) {
        cout << "  output.mci = MCNP/MCNPX input format" << std::endl;
        cout << "  output.cog = COG input format" << std::endl;
     }
     cout << "  output.def = Recording of user input" << std::endl;
     cout << endl;
   }

   return 0;
}

// Calculate the total concentration of the isotope mixture
double getTotalConcentration(radsrc::CIsotopeMixture& isomix) {
  double sum = 0.0;
  radsrc::CIsotopeMixture::const_iterator it = isomix.begin();
  while (it != isomix.end()) {
    sum += it->second;
    ++it;
  }
  return sum;
}

void PrintMixture(const radsrc::CIsotopeMixture& isomix) {
  double sum = 0.0;
  radsrc::CIsotopeMixture::const_iterator it = isomix.begin();
  while (it != isomix.end()) {
    std::cout << it->first.toString().c_str() << "  "
              << it->second 
              << std::endl;
    sum += it->second;
    ++it;
  }
  std::cout << "Total: " << sum << std::endl;
}

void PrintGammas(CRadSource& radsource, bool selected) {
   std::cout << "Gammas sorted by Energy" << std::endl;
   std::cout << " E(kev)    I(#/s/gm)   Emitting decay(s)" << std::endl;;
   std::cout << "--------  -----------  -------------------------" << std::endl;
   CPhotonComputer::CPhotonIterator it, end;
   if (selected) {
      it = radsource.getPhotonComputer().beginSelectedGammas();
      end = radsource.getPhotonComputer().endSelectedGammas();
   }
   else {
      it = radsource.getPhotonComputer().beginGammas();
      end = radsource.getPhotonComputer().endGammas();
   }
   while (it != end) {
      std::cout
        << std::setw(8) << it->getEnergy() << "  "
        << std::setw(11) << it->getIntensity() << "  ";
      CPhotonComputer::CGammaEntry::DecayList::const_iterator dit,dend;
      dit = it->getDecays().begin();
      dend = it->getDecays().end();
      while (dit != dend) {
         std::cout << dit->first.toString().c_str() << "->" << dit->second.toString().c_str();
         ++dit;
         if (dit != dend) {
            std::cout << ", ";
         }
      }
      std::cout << std::endl;
      ++it;
   }
}


void SetBins(CRadSource& radsource, const Options::SubOptions& options, CPhotonComputer::BinSubject what) {
   std::vector<double> vtmp;
   CPhotonComputer& pc = radsource.getPhotonComputer();
   for (int foo = 0; foo < 2; foo++) {
      switch (options.bin_option) {
      case BIN_DEFAULT:
//         pc.setBinning(what,DefaultBins,NDEFAULTBINS+1);
         ReadBins(options.filename,vtmp,PF_DEFAULT);
         pc.setBinning(what,vtmp);
         break;
      case BIN_FILE:
         ReadBins(options.filename,vtmp,PF_SEARCH);
         pc.setBinning(what,vtmp);
         break;
      case BIN_TART:
         pc.setBinning(what,TartBins,NTARTBINS+1);
         break;
      case BIN_MORSE:
         pc.setBinning(what,MorseBins,NMORSEBINS+1);
         break;
      case BIN_GADRAS:
         pc.setBinning(what,GADRASBins());
         break;
      case BIN_EQUISPACE:
         EquiBins(vtmp,options.min,options.max,options.nbins);
         pc.setBinning(what,vtmp);
         break;
       case BIN_SQRT:
         SqrtBins(vtmp,options);
         pc.setBinning(what,vtmp);
         break;
       case BIN_NONE:
       default:
         break;
      }
   }
}

void InputConfig(CRadSource& radsource, Options& options, std::ostream& input_log) {
   if (options.interactive) {
      std::cout << std::endl;
      std::cout << "Enter sources on one or multiple lines." << std::endl;
      std::cout << "(the total must be between 99.9% and 100%)." << std::endl;
      std::cout << "Terminate the input by specifying the desired age in years." << std::endl;
      std::cout << std::endl;
      std::cout << "Example1: U238 95.0 U235 5.0  Age 10" << std::endl;
      std::cout << std::endl;
      std::cout << "Example2: Pa-234m 5e-14" << std::endl;
      std::cout << "          Pa-234 2e-14" << std::endl;
      std::cout << "          U-238 1.0" << std::endl;
      std::cout << "          Age 5.5" << std::endl;
      std::cout << std::endl;
   }
   std::stringstream ss;
   std::string str;
   while (!options.inputstream->fail()) {
      if (options.interactive)
         std::cout << "Enter <isotope> <percent> or AGE <years> --> ";
      GetLine(options.inputstream,str);
      ss << str;
      ss << ' ';
      input_log << str.c_str();
      input_log << std::endl;
      if (str.find("age") != std::string::npos ||
          str.find("Age") != std::string::npos ||
          str.find("AGE") != std::string::npos) break; // normal exit
      if (str.find("quit") != std::string::npos ||
          str.find("Quit") != std::string::npos ||
          str.find("QUIT") != std::string::npos) exit(-1); // user abort
   }
   ss.seekg(0);
   bool success;
   try {
      success = radsource.loadConfig(ss);
   } catch(...) {
      success = false;
   }
   if (success == false) {
      str.resize(0);
      ss.seekg(0);
      ss >> str;
      std::cerr << "Error parsing your problem input and/or computing decay chain." << std::endl;
      std::cerr << "Total configuration input was:" << std::endl << ss.str().c_str() << std::endl;
      exit (-1);
   }
   options.sample_brem = radsource.getPhotonComputer().sampleBrem();
   radsource.getPhotonComputer().getLinesRange(options.source_min,options.source_max);

}

void WriteLinesFile(const std::string& filename, const std::string& header,CRadSource& radsource) {
   std::ofstream fsLinesFile;
   fsLinesFile.open(filename.c_str());
   if (fsLinesFile.is_open() != true) {
      std::cerr << "Unable to open lines file" << std::endl;
      exit (-1);
   }
   fsLinesFile << header.c_str(); // implict EOLs
   fsLinesFile << "=======================================================";
   fsLinesFile << std::endl;
   fsLinesFile << "Sorted by Energy:" << std::endl;
   radsource.getPhotonComputer().computeGammas(CPhotonComputer::ENERGY);
   CPhotonComputer::CPhotonIterator it, endit;
   it = radsource.getPhotonComputer().beginGammas();
   endit = radsource.getPhotonComputer().endGammas();

   fsLinesFile << " Energy   Intensity   Originating Initial Isotopes and Decay(s)" << std::endl;
   fsLinesFile << "-------- ------------ ----------------------------------------------" << std::endl;
   while (it != endit) {
      fsLinesFile
         << std::right << std::setw(10) << std::setprecision(4) << std::fixed << it->getEnergy() << ' '
         << std::right << std::setw(12) << std::setprecision(5) << std::scientific << it->getIntensity() << " | "
         << it->getParentDescription().c_str()
         << std::endl;
      ++it;
   }
   fsLinesFile << "Sorted by Intensity:" << std::endl;
   fsLinesFile << " Intensity    Energy  Originating Initial Isotopes and Decay(s)" << std::endl;
   fsLinesFile << "------------ -------- ----------------------------------------------" << std::endl;
   radsource.getPhotonComputer().computeGammas(CPhotonComputer::INTENSITY);
   it = radsource.getPhotonComputer().beginGammas();
   while (it != endit) {
      fsLinesFile
         << std::right << std::setw(12) << std::setprecision(5) << std::scientific << it->getIntensity() << ' '
         << std::right << std::setw(10) << std::setprecision(4) << std::fixed << it->getEnergy() << " | " 
         << it->getParentDescription().c_str()
         << std::endl;
      ++it;
   }
   fsLinesFile.close();
}

void SelectLines(CRadSource& radsource, Options& options) {
   std::vector<double> v;
   switch (options.lines) {
   case BIN_NONE:
      break;
   case BIN_ALL:
      radsource.getPhotonComputer().selectGammas((double *)0,0,LSFUZZ);
      break;
   case BIN_DEFAULT: // deprecated
//      radsource.getPhotonComputer().selectGammas(DefaultLines,NDEFAULTLINES,LSFUZZ);
      ReadLineFile(v,options.linesfile, PF_DEFAULT);
      radsource.getPhotonComputer().selectGammas(v,LSFUZZ);
      break;
   case BIN_FILE:
      ReadLineFile(v,options.linesfile, PF_SEARCH);
      radsource.getPhotonComputer().selectGammas(v,LSFUZZ);
      break;
   default:
      break;
   }
}
