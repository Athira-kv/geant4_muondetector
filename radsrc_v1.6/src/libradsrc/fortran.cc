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


/*  FORTRAN and C++ Interface
 *
 */

#include "porting.h"

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <memory.h>

#include "cpp_api.h"
//#include "radsource.h"

/*! \file fortran.cc
 *  Simplified interface to the libradsrc library for FORTRAN.
 *
 *  In these examples,
 *  \code
 *  LOGICAL SUCCESS
 *  INTEGER*8 HANDLE
 *  CHARACTER*n FILENAME, CONFIGSTRING
 *  INTEGER LENGTH, Z, A, M, N, NMAX
 *  DOUBLE PRECISION CONCENTRATION, E, AGE, LINES[2][NMAX], FOURV[4], RNG
 *  INTRINSIC/EXTRINSIC RNG
 *  \endcode
 *
 *  Initialization will generally follow one of the following forms:
 *  \code
 *  CALL RSNEWSOURCE(HANDLE)
 *  CALL ADDISOTOPE(HANDLE,92,238,0,100D0)
 *  SUCCESS = MIX(HANDLE,25D0)
 *  \endcode
 *  \code
 *  CALL RSNEWSOURCE(HANDLE)
 *  SUCCESS = RSLOADCONFIG(HANDLE,'config.txt')
 *  \endcode
 *  \code
 *  CALL RSNEWSOURCE(HANDLE)
 *  DO
 *    CALL ADDCONFIG(HANDLE,CONFIGSTRING)
 *  END DO
 *  SUCCESS = RSSOURCECONFIG(HANDLE)
 *  \endcode
 */
using namespace radsrc;

namespace radsrc {
   class CRadSource;
}
namespace {
   //! Copy a native pointer to 8 bytes of storage
   void PointerToHandle(CRadSource* pRadSource, char *pHandle) {
      int i;
      char *pcPointer = reinterpret_cast<char *>(&pRadSource);
      for (i = 0; i < static_cast<int>(sizeof(CRadSource*)); i++) {
         pHandle[i] = pcPointer[i];
      }
      while(i < 8) {
         pHandle[i++] = 0;
      }
   }
   //! Copy up to 8 bytes into a native pointer
   CRadSource* HandleToPointer(const char *pHandle) {
      volatile CRadSource* pRadSource;
      char *pcPointer = reinterpret_cast<char *>(&pRadSource);
      for (int i = 0; i < static_cast<int>(sizeof(CRadSource*)); i++) {
         pcPointer[i] = pHandle[i];
      }
      return const_cast<CRadSource*>(pRadSource);
   }
}
extern "C" {
   /*! \brief Create a new decay calculation problem.
    *  \return an opaque handle to the problem definition.
    *
    *  Invoke with:
    *  \code
    *  CALL RSNEWSOURCE(HANDLE)
    *  \endcode
    */
   void rsnewsource_(char *pHandle) {
      PointerToHandle(CApi::newSource(), pHandle);
   }

   /*! \brief Load the problem configuration from a file and perform decay and photon calculations.
    *
    *  Invoke with:
    *  \code
    *  SUCCESS = RSLOADCONFIG(HANDLE,FILENAME)
    *  SUCCESS = RSLOADCONFIG(HANDLE,'filename.txt')
    *  SUCCESS = RSLOADCONFIG(HANDLE,'') for default location
    *  \endcode
    */
   int rsloadconfig_(char *pHandle, char* ptr, int len)
   {
      std::string filename(ptr,len);
      return CApi::loadConfig(HandleToPointer(pHandle),filename);
   }
    
   /*! \brief Add an isotope to the input mixture
    *
    *  Invoke with:
    *  \code
    *  CALL RSADDISOTOPE(HANDLE,Z,A,M,CONCENTRATION)
    *  \endcode
    */
    void rsaddisotope_(char *pHandle, const int& z,
      const int& a, const int& m, const double& perc) {
      CApi::addIsotope(HandleToPointer(pHandle), z, a, m, perc);
   }
    
   /*! \brief Age the input mixture
    *
    *  Invoke with:
    *  \code
    *  SUCCESS = RSMIX(HANDLE,AGE)
    *  \endcode
    */
   int rsmix_(char *pHandle, const double& age) {
      return CApi::mix(HandleToPointer(pHandle), age);
   }

   /*! \brief Sort the photon list
    *
    *  Invoke with:
    *  \code
    *  CALL RSSORT(HANDLE,1) for energy
    *  CALL RSSORT(HANDLE,2) for intensity
    *  \endcode
    */
   void rssort_(char *pHandle, const int& field) {
      CApi::sort(HandleToPointer(pHandle), field);
   }

   /*! \brief Get the total number of discrete lines.
    *
    *  Invoke with:
    *  \code
    *  N = RSNLINES(HANDLE)
    *  \endcode
    */
   int rsnlines_(const char *pHandle) {
      return CApi::nLines(HandleToPointer(pHandle));
   }
   /*! \brief Get the discrete lines as energy-intensity pairs, with nmax lines.
    *
    *  Invoke with:
    *  \code
    *  CALL RSGETLINES(HANDLE, LINES, NMAX)
    *  \endcode
    */
   void rsgetlines_(char *pHandle, double lines[][2], const int& nmax) {
      CApi::getLines(HandleToPointer(pHandle), lines, nmax);
   }
   /*! \brief Enable/disable sampling of the bremstrahlung distribution (if present)
    *
    * Invode with:
    * \code
    * CALL RSSAMPLEBREM(1)
    * CALL RSSAMPLEBREM(0)
    * \endcode
    * to enable/disable sampling, respectively.
    */
    void rssamplebrem_(const char *pHandle, const int& flag) {
       CApi::sampleBrem(HandleToPointer(pHandle), flag!=0);
    }

   /*! \brief Get a random photon energy in keV using a double precision PRNG function.
    *
    *  Invoke with:
    *  \code
    *  E = RSGETPHOTON(HANDLE, RNG)
    *  \endcode
    */
   double rsgetphoton_(const char *pHandle, double (*prng)(void)) {
      return CApi::getPhoton(HandleToPointer(pHandle), prng);
   }

   /*! \brief Get a random photon energy and momentum in keV using a double precision PRNG function.
    *
    *  Invoke with:
    *  \code
    *  CALL RSGET4V(HANDLE, FOURV, RNG)
    *  \endcode
    */
   void rsget4v_(const char *pHandle, double e[4], double (*prng)(void)) {
      CApi::get4V(HandleToPointer(pHandle), e, prng);
   }

   /*! \brief Get a random photon energy in keV using a double precision PRNG function.
    *
    *  Invoke with:
    *  \code
    *  E = RSGETPHOTON(HANDLE, RNG)
    *  \endcode
    */
   double rsgtrphoton_(const char *pHandle, float (*prng)(void)) {
      return CApi::getPhoton(HandleToPointer(pHandle), prng);
   }

   /*! \brief Get a random photon energy and momentum in keV using a double precision PRNG function.
    *
    *  Invoke with:
    *  \code
    *  CALL RSGET4V(HANDLE, FOURV, RNG)
    *  \endcode
    */
   void rsgtr4v_(const char *pHandle, double e[4], float (*prng)(void)) {
      CApi::get4V(HandleToPointer(pHandle), e, prng);
   }

   /*! \brief Obtain a summary of the calculation and place it in a character variable.
    *
    *  Over-long summaries will be truncated with '...'.
    *
    *  Invoke with:
    *  \code
    *  LENGTH = RSGETREPORT(HANDLE,BUFFER)
    *  \endcode
    */
   int rsgetreport_(const char *pHandle, char *ptr, int len) {
      std::string report = CApi::getReport(HandleToPointer(pHandle));
      int i = 0;
      while (i < len && i < static_cast<int>(report.size())) {
         ptr[i] = report[i];
         i++;
      }
      if (len < static_cast<int>(report.size())) {
         ptr[len-1] = '\n';
         if (len >= 4) {
            ptr[len-2] = ptr[len-3] = ptr[len-4] = '.';
         }
         return len;
      }
      return report.size();
   }
   

   /*! \brief Provide additional configuration information, which is appended to previously supplied strings.
    *
    *  \warning A space is inserted before the new information, so strings must end on word boundaries.
    *
    *  Invoke with:
    *  \code
    *  CALL RSADDCONFIG(HANDLE,CONFIGSTRING)
    *  CALL RSADDCONFIG(HANDLE,'U-238 100')
    *  \endcode
    */
   void rsaddconfig_(char *pHandle, char* ptr, int len) {
      std::string input(ptr,len);
      input += ' ';
      CApi::addConfig(HandleToPointer(pHandle), input);
   }

   /*! \brief Parse accumulated configuration information and perform decay and photon calculations
    *
    *  Invoke with:
    *  \code
    *  SUCCESS = RSSOURCECONFIG(HANDLE)
    *  \endcode
    */
   bool rssourceconfig_(char *pHandle) {
      return CApi::sourceConfig(HandleToPointer(pHandle)) != 0;
   }
}

