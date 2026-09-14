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


/* Classes for storing information about isotopes */

#ifndef ISOTOPE_H_INCLUDE
#define ISOTOPE_H_INCLUDE
#include <string>
#include <vector>
#include <math.h>

#include "dbmanager.h"  // temporary include for friendship

namespace radsrc {
   /*  -------------------------------------------------------------------- */
   /*! \class CIsotope
    *  \brief Unique identification of each isotope (isomer)
    */
   /*  -------------------------------------------------------------------- */

   class CIsotope {
   public:
      // Constructors, etc.

      //! Null Isotope
      CIsotope(void) : Z(0), A(0), m(0) { } 
      //! Construct Isotope
      CIsotope(int z, int a, int mm=0) : Z(z), A(a), m(mm) { } 

      // operators

      //! Two isotopes are equal
      bool operator==(const CIsotope& iso) const  { return Z==iso.Z && A==iso.A && m==iso.m; }
      //! Order Isotopes first by Z, then A, then m
      bool operator<(const CIsotope& iso) const { return Z<iso.Z || (Z==iso.Z && A<iso.A) ||
         (Z==iso.Z && A==iso.A && m<iso.m);
      }

      // Public Methods

      //! get atomic number (Z)
      int getAtomicNumber(void) const { return Z; }
      //! get mass number (A)
      int getMassNumber(void) const { return A; }
      //! get metastable designation (m)
      int getMetastableNumber(void) const { return m; }
      //! \brief write the canonical name of the isotope to a char buffer as a nul-terminated string
      //! \param[out] str buffer to contain the isotope name
      void toString(char *str) const;
      //! \brief write the canonical name of the isotope to a STL string
      //! \param[out] str string to contain the isotope name
      void toString(std::string& str) const { char tmp[10]; toString(tmp); str=tmp; }
      //! \brief write the canonical name of the isotope to a STL string
      //! \return string to contain the isotope name
      std::string toString() const { char tmp[10]; toString(tmp); return tmp; }
      //! \brief Assign isotope from variations of the isotope name in a nul-terminated C string
      //! \param[in] str string containing the isotope name
      //! \return a reference to this object
      CIsotope& fromString(const char *str);
      //! \brief Assign isotope from variations of the isotope name in a STL string
      //! \param[in] str string containing the isotope name
      //! \return a reference to this object
      CIsotope& fromString(const std::string& str) { return fromString(str.c_str()); }
      //! is this a valid isotope (conversion from strings can fail)
      bool isValid(void) const { return Z > 0 && A > 0; }
   private:
      int Z; //!< The atomic number.
      int A; //!< The mass number.
      int m; //!< The metastable state.
   };

}
#endif

