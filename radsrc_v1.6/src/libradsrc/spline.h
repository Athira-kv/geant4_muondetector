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


/* C++ wrapper for cubic spline routines
 * Limitations:
 *   minimum 3 points
 *   "not-a-knot" boundary conditions only
 */

#ifndef SPLINE_H_INCLUDE
#define SPLINE_H_INCLUDE

#include <vector>

namespace radsrc {
   class CSpline {
   public:
      typedef std::vector<double> ValArray;
      CSpline(void) : m_valid(false) { }
      ~CSpline(void) { }
   //
      void Clear(void) { m_Coeffs.clear(); m_valid = false; }
      bool AttachValues(const ValArray& x, const ValArray& y) {
         if (x.size() < 3 || x.size() != y.size()) Clear();
         else { m_pX = &x; m_pY = &y; CubicSpline(); }
         return m_valid;
      }
      double operator()(double x) const { return Evaluate(x); }
      double Evaluate(double x) const;
      bool IsValid(void) const { return m_valid; }
   //
   private:
      struct _coeffs {
         double c[3];
      };
      const ValArray *m_pX, *m_pY;
      std::vector<_coeffs> m_Coeffs;
      bool m_valid;
      const double& X(int i) const { return (*m_pX)[i]; }
      const double& Y(int i) const { return (*m_pY)[i]; }
      const double& C(int p, int i) const {
         if (i == 0) throw; return m_Coeffs[p].c[i-1];
      } 
      double& C(int p, int i) {
         if (i == 0) throw; return m_Coeffs[p].c[i-1];
      } 
      void CubicSpline(void);
      int Interval(double x) const;
   };
}
#endif

