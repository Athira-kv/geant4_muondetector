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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "porting.h"
#include "useriface.h"
#include "isotopedb.h"

using namespace std;
using namespace radsrc;

void OpenDatFile(std::ifstream& is, const char* filename, PathFlags pf) {
   const char* home = getenv("RADSRC_HOME");
   std::string searchdirs[3];
   int n_search, i, defpath = -1;
   std::string trial_path;
   if (pf == PF_DEFAULT) {
      n_search = 1;
      if (home) {
         searchdirs[0] = home;
         searchdirs[0] += "/lib";
         defpath = 0;
      }
      else {
         searchdirs[0] = "./lib";
      }
   }
   else {
      searchdirs[0] = ".";
      searchdirs[1] = "./lib";
      n_search = 2;
      if (home) {
         searchdirs[2] = home;
         searchdirs[2] += "/lib";
         defpath = 2;
         n_search = 3;
      }
   }

   for (i = 0; i < n_search; i++) {
      trial_path = searchdirs[i];
      trial_path += '/';
      trial_path += filename;
      is.open(trial_path.c_str());
      if (is.is_open()) return;
      is.clear();
   }
   std::cerr << "Unable to locate '" << filename << "' in any of the following locations:" << std::endl;
   for (i = 0; i < n_search; i++) {
      std::cerr << "   '" << searchdirs[i].c_str() << "'";
      if (i==defpath) std::cerr << " ($RADSRC_HOME/lib) ";
      std::cerr << std::endl;
   }
   if (home == 0) std::cerr << "Consider setting RADSRC_HOME environment variable to the location" << std::endl << " of the RadSource distribution." << std::endl;;
   exit(-1);
}

void GetLine(std::istream* is, std::string& str) {
   int n;
   do {
      str.resize(0);
      while (!is->fail()) {
         char c;
         is->get(c);
         if (c != '\n') str += c; else break;
      }
      n = str.size();
   } while (n &&  str[0] == '#');
   if (n && str[n-1] == '\r') str.resize(n-1);
}

void GetString(std::istream* is,     //....stream to read input from
   std::string& str,     //....return string
   bool interactive,     //....flag that we are conversing with the user
   const char *msg,      //....message to display on stdout (if interactive)
   const char *def_str,   //....default string value
   std::ostream& input_log

   )
{
   str = "";
   do {
      if (is->fail()) {
         std::cerr << "Premature end of input." << std::endl;
         exit(-1);
      }
      if (interactive) {
         std::cout << msg;
         if (def_str) std::cout << '(' << def_str << ')';
         std::cout << ": ";
      }
      GetLine(is,str);
      if (*str.rbegin() == '\r') str.erase(str.length()-1);
      if (str.length() > 0) {
         input_log << str.c_str() << std::endl;
      }
      if (str.length() == 0 && def_str) {
         str = def_str;
         input_log << std::endl;
      }
   } while (str.length() == 0); // loop if no entry and no default
}

void GetPositiveValue(std::istream* is, int& x, bool interactive, const char *msg, std::ostream& input_log) {
   std::string str;
   do {
      if (is->fail()) {
         std::cerr << "Premature end of input." << std::endl;
         exit(-1);
      }
      x = -1;
      if (interactive)
         std::cout << msg << ": ";
      GetLine(is,str);
      x = atoi(str.c_str());
   } while (x <= 0);
   input_log << str.c_str() << std::endl;
}

void GetNonNegativeValue(std::istream* is, double& x, bool interactive, const char *msg, std::ostream& input_log) {
   std::string str;
   do {
      if (is->fail()) {
         std::cerr << "Premature end of input." << std::endl;
         exit(-1);
      }

      x = -1;
      if (interactive) std::cout << msg << ": ";
      GetLine(is,str);
      x = atof(str.c_str());
   } while (x < 0.0);
   input_log << str.c_str() << std::endl;
}

void GetRange(std::istream* is, double& min, double& max, bool interactive, std::ostream& input_log) {
   std::ostringstream tmpinput;
   do {
      if (tmpinput.str() != "" && interactive) cout << "Please re-enter (max > min >= 0)" << endl;
      tmpinput.str().erase();
      tmpinput.clear();
      GetNonNegativeValue(is, min, interactive, "Enter minimum value (keV)", tmpinput);
      GetNonNegativeValue(is, max, interactive, "Enter maximum value (keV)", tmpinput);
   } while (max <= min || min < 0);
   if (interactive) cout << "Range is now: (" << min << ", " << max << ")" << endl;
   input_log << tmpinput.str().c_str();
}

//
// returns true if the user responds "y" to the printed message
//
bool GetYN(std::istream* is, bool interactive, std::ostream& input_log, bool def_value=true){
  std::string answer;
  std::string def = def_value?"y":"n";
  GetString(is, answer, interactive, " y/n ", def.c_str(), input_log);
  if( answer.length() && (answer[0] == 'Y' || answer[0] == 'y') ) return true;
  return false;
}

static const int N_Options = 7;
const char *BinOptions[N_Options] = {
   "Default Binning", "Read from a file", "Equal spaced bins",
   "Proportional to Energy Width", "TART 65 bins (NaI)",
   "MORSE 35 bins (NaI)", "GADRAS 1000 bins (HPGe)"
};

const int BinOptionEnums[N_Options] = {BIN_DEFAULT, BIN_FILE, BIN_EQUISPACE,
   BIN_SQRT, BIN_TART, BIN_MORSE, BIN_GADRAS};

// Note: same as brem, same as gamma are disabled
void SelectBinning(std::istream* is, const char *msg, bool interactive, Options::SubOptions& suboption, std::ostream& input_log) {
   int i;
   char stringbuf[256];
   if (interactive) {
      std::cout << "\nSelect binning options for " << msg << ':' << std::endl << std::endl;
      for (i = 0; i < N_Options; i++) {
         std::cout << "  " << i+1 << ") " << BinOptions[i] << std::endl;
      }
   }
   int n_options = N_Options;
   int option;
   std::ostringstream tmpinput;
   
   if (interactive)
      sprintf(stringbuf,"Select (1-%d)",n_options);

   do {
      tmpinput.str().erase();
      tmpinput.clear();
      GetPositiveValue(is, option, interactive, stringbuf, tmpinput);
   } while(option > n_options || option < 1);
   input_log << tmpinput.str().c_str();

   option = BinOptionEnums[option-1];
   suboption.bin_option = (Binning) option;

   // Oops.  I passed in the default value in the destination, which 
   // the user will trash prematurely
   const std::string deffilename = suboption.filename;

   // Collect extra info
   switch(suboption.bin_option) {
   case BIN_FILE:
      GetString(is, suboption.filename, interactive, "Enter filename", deffilename.c_str(), input_log);
      break;
   case BIN_EQUISPACE:
      GetPositiveValue(is, suboption.nbins, interactive, "# of bins", input_log);
      if (interactive)
         std::cout << "Enter range (keV)" << std::endl;
      GetRange(is, suboption.min,suboption.max,interactive,input_log);
      break;
   case BIN_SQRT:
      GetPositiveValue(is, suboption.nbins,interactive,"# of bins",input_log);
      if (interactive)
         std::cout << "Enter range (keV)" << std::endl;
      GetRange(is, suboption.min,suboption.max,interactive, input_log);
      if (interactive)
         std::cout << "Enter Slope and Pedestal (dE ~ s Sqrt(E) + p)" << std::endl;
      GetNonNegativeValue(is, suboption.slope,interactive,"Slope",input_log);
      GetNonNegativeValue(is, suboption.pedestal,interactive,"Pedestal",input_log);
      break;
   default:
      break;
   }
}
   
void GetOptions(Options& options, std::ostream& input_log) {
   options.brembin_options.filename  = "dfltbrem.dat";
   options.gammabin_options.filename = "dfltbins.dat";
   options.tallybin_options.filename = "dflttaly.dat";
   const char* linessel_filename     = "dfltlins.dat";

   // Default energy range for unscatter gamma tally
   if (options.source_max == 0) {
      options.tallyu_min=90.;   //keV
      options.tallyu_max=3300.; //keV
   }
   else {
      options.tallyu_min=options.source_min; 
      options.tallyu_max=options.source_max;
   }

   if (options.interactive) {
      std::cout << "Do you want to produce Monte Carlo input decks? ";
   }
   if(!GetYN(options.inputstream,options.interactive, input_log, true)) {
      options.do_mci = false;
      return;
   }
   options.do_mci = true;

   if (options.sample_brem) {
      // Ask for Bremsstrahlung binning only if the option "brem" is ON
      SelectBinning(options.inputstream, "Bremsstrahlung", options.interactive,
                    options.brembin_options,input_log);
    }
   
   if (options.interactive) {
      std::cout << "\nSelect binning options for gamma lines:"
         << std::endl << std::endl;
      std::cout << "  1) Keep all lines discrete" << std::endl;
      std::cout << "  2) Bin all lines" << std::endl;
      std::cout << "  3) Keep the default lines" << std::endl;
      std::cout << "  4) Read the line list from a file" << std::endl;
   }

   std::ostringstream tmpinput;
   do {
      tmpinput.clear();
      tmpinput.str().erase();
      GetPositiveValue(options.inputstream,options.lines,options.interactive,"Select (1-4)", tmpinput);
   } while(options.lines > 4 || options.lines < 1);
   input_log << tmpinput.str().c_str();
   options.linesfile="";
   switch(options.lines) {
   case 4:
      options.lines = BIN_FILE;
      GetString(options.inputstream,options.linesfile,options.interactive,"Enter Filename",linessel_filename,input_log);
      SelectBinning(options.inputstream,"gammas",options.interactive,
                    options.gammabin_options,input_log);
      break;
   case 3:
      options.lines = BIN_DEFAULT;
      options.linesfile = linessel_filename;
      SelectBinning(options.inputstream,"gammas",options.interactive,
                    options.gammabin_options,input_log);
      break;
   case 2:
      options.lines = BIN_ALL;
      SelectBinning(options.inputstream,"gammas",options.interactive,
                    options.gammabin_options,input_log);
      break;
   case 1:
      options.lines = BIN_NONE;
      if (options.interactive)
	std::cout << "\nDo you want to use the default energy range "
		  << "(" 
		  << options.tallyu_min << " , "
                  << options.tallyu_max << ")" << std::endl
		  << "for the gamma line sources and unscattered tally?";
      if( ! GetYN(options.inputstream,options.interactive, input_log, true) )
        GetRange(options.inputstream,options.tallyu_min,options.tallyu_max,
                 options.interactive,input_log);
      break;
   default:
      break;
   }  
   if (options.lines != BIN_NONE && options.interactive) {
      std::cout << "\nEnergy range for gamma sources and unscattered tally set to match gamma binning." << std::endl;
   }


   SelectBinning(options.inputstream,"scattered tally",options.interactive,
                 options.tallybin_options,input_log);
} 


void ParseCommandLine(int argc, const char* argv[], Options& options) {
   int arg = 1; // current arg
   int posarg = 0; // positional argument

   options.quiet = false;
   options.interactive = true;
   while (arg < argc) {
      // check for switches
      if (argv[arg][0] == '-') {
         if (argv[arg][1] == '\0') {
            std::cerr << "Unrecognized switch" << std::endl;
            std::cerr << "Usage: radsrc [-hq] [config file]" << std::endl;
            exit(-1);
         }
         int switchpos = 1;
         while (argv[arg][switchpos] != '\0') {
            switch (argv[arg][switchpos]) {
            case 'h':
               std::cerr << "Usage: radsrc [-hq] [config file]" << std::endl;
               std::cerr << "  -h: this help" << std::endl;
               std::cerr << "  -q: quiet (suppress output to stdout)" << std::endl;
               std::cerr << "  config file: a file containing what would otherwise be input on stdin." << std::endl;
               std::cerr << "If the config file is not specified, radsrc accepts input interactively" << std::endl;
               std::cerr << "from stdin." << std::endl;
               exit(0);
            case 'q':
               options.quiet = true;
               break;
            default:
               std::cerr << "Unrecognized switch" << std::endl;
               std::cerr << "Usage: radsrc [-hq] [config file]" << std::endl;
               exit(-1);
            }
            switchpos++;
         }
         arg++;
         continue;
      } // end switches
      // positional argument
      switch(posarg) {
      case 0: // config file
         options.interactive = false;
         options.configfile = argv[arg];
         break;
      default:
         std::cerr << "Too many arguments." << std::endl;
         std::cerr << "Usage: radsrc [-hq] [config file]" << std::endl;
         exit(-1);
      }
      posarg++;
      arg++;
   } // while more args
}

void ReadBins(const std::string& filename, std::vector<double>& v, PathFlags pf) {
   std::ifstream ifs;
   OpenDatFile(ifs, filename.c_str(),pf);    std::string buffer;
   std::getline(ifs,buffer);    std::getline(ifs,buffer);
   std::getline(ifs,buffer); 
   std::getline(ifs,buffer); 
   std::getline(ifs,buffer);
   int n = atoi(buffer.c_str());
   if (n <= 0) {       std::cout << "Error parsing " << filename.c_str() << std::endl;
      exit(-1); 
   } 
   std::getline(ifs,buffer); 
   v.resize(n+1); 
   for (int i = 0; i < n+1; i++) {
      std::getline(ifs,buffer);
      v[i] = atof(buffer.c_str());
   }
}
void ReadLineFile(std::vector<double>& v, std::string filename, PathFlags pf) {
   v.resize(0);
   std::ifstream datfile;
   OpenDatFile(datfile,filename.c_str(),pf);
   int i;
   std::string line;
   for (i = 0; i < 5; i++) std::getline(datfile, line);
   int n_lines = atoi(line.c_str());
   std::getline(datfile,line);
   for (i = 0; i < n_lines; i++) {
      std::getline(datfile,line);
      if (datfile.bad()) break;
      double x = atof(line.c_str());
      v.push_back(x);
   }
   datfile.close();
}



