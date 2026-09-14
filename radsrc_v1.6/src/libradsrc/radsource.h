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


/* Classes for specifing a radioactive mixture and aging it */
#ifndef RADSOURCE_H_INCLUDE
#define RADSOURCE_H_INCLUDE

#include "porting.h"
#include <string>
#include <istream>
#include "decaycomputer.h"
#include "photoncomputer.h"

namespace radsrc {

   /*! \class CRadSource
    *  \brief Encapsulating class for various aspects of the decay radiation problem.
    *
    *  CRadSource simplifies the functions of configuring and executing a decay
    *  computation.  CRadSource provides several input methods for problem specifications:
    *  as a filename, as an open istream, or as a string, which may be delivered as
    *  substrings. The CDecayComputer and CPhotonComputer objects are initializaed
    *  according to the configuration string.  Configuration files can also be specified
    *  by environment variable.
    *
    *  CRadSource also provides a summary of the decay calculation either as a formatted
    *  multi-line string or as a vector of lines.
    */
   class CRadSource {
   public:
      // Constructors, etc.
      CRadSource(void);
      ~CRadSource(void);

      // Public Member Functions
      //! Configure from a file
      bool loadConfig(const char *filename=0);

      //! Configure from an istream
      bool loadConfig(std::istream& isInput);

      //! Load a default configuration
      //! \deprecated Lifetime uncertain. May be useful enough to keep.
      bool setupDefault(void);

      //! Get Report as lines or string
      std::string getReport(const char *pPrefix = 0) const;
      void getReport(std::vector<std::string>& vReportLines) const;

      //! Get the CDecayComputer object
      CDecayComputer& getDecayComputer(void);
      //! Get the CDecayComputer object
      const CDecayComputer& getDecayComputer(void) const;

      //! Get the CPhotonComputer object
      CPhotonComputer& getPhotonComputer(void);
      //! Get the CPhotonComputer object
      const CPhotonComputer& getPhotonComputer(void) const;

      //! Append a string to the accumulating string of configuration instructions
      void addConfigInfo(const char *str);

      //! Append a string to the accumulating string of configuration instructions
      void addConfigInfo(const std::string& str);

      //! Parse configuration string
      int processConfigInfo(void);

      //! Get the current database in use
      const CIsotopeDatabase* getIsotopeDatabase(void) const;
   private:
      CDecayComputer m_decayComputer;
      CPhotonComputer m_photonComputer; // must initialize after m_decayComputer!
      std::string config;

      //! character based input of whitespace terminated words
      void getWord(std::istream&, std::string&);

      // CPhotonComputer is not copyable
      CRadSource(const CRadSource&);
      CRadSource& operator=(const CRadSource&);
   };

   inline CPhotonComputer&
   CRadSource::getPhotonComputer()
   {
      return m_photonComputer;
   }

   inline CDecayComputer&
   CRadSource::getDecayComputer()
   {
      return m_decayComputer;
   }

   inline const CPhotonComputer&
   CRadSource::getPhotonComputer() const
   {
      return m_photonComputer;
   }

   inline const CDecayComputer&
   CRadSource::getDecayComputer() const
   {
      return m_decayComputer;
   }
   inline void
   CRadSource::addConfigInfo(const char *str)
   {
      config += str;
   }
   inline void
   CRadSource::addConfigInfo(const std::string& str)
   {
      config += str;
   }
   inline const CIsotopeDatabase*
   CRadSource::getIsotopeDatabase(void) const
   {
      return m_decayComputer.getIsotopeDatabase();
   }

}
#endif

