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
#include <math.h>
#include "bins.h"

// Default bins are deprecrated and will be deleted in a future revision
/* keV */
const double DefaultBins[NDEFAULTBINS+1] = {
 .300000e+02,  .400000e+02,  .500000e+02,  .600000e+02,  .700000e+02,
 .800000e+02,  .900000E+02, 1.023200E+02, 1.149000E+02, 1.279000E+02,
1.412000E+02, 1.547622E+02, 1.686104E+02, 1.827396E+02, 1.971438E+02,
2.118176E+02, 2.267561E+02, 2.419549E+02, 2.574097E+02, 2.731169E+02,
2.890728E+02, 3.052741E+02, 3.217179E+02, 3.384013E+02, 3.553215E+02,
3.724761E+02, 3.898626E+02, 4.074788E+02, 4.253226E+02, 4.433919E+02,
4.616849E+02, 4.801996E+02, 4.989345E+02, 5.178877E+02, 5.370578E+02,
5.564430E+02, 5.760422E+02, 5.958537E+02, 6.158764E+02, 6.361089E+02,
6.565500E+02, 6.771985E+02, 6.980532E+02, 7.191130E+02, 7.403768E+02,
7.618437E+02, 7.835127E+02, 8.053828E+02, 8.274528E+02, 8.497221E+02,
8.721899E+02, 8.948550E+02, 9.177169E+02, 9.407747E+02, 9.640275E+02,
9.874746E+02, 1.011115E+03, 1.034949E+03, 1.058975E+03, 1.083192E+03,
1.107600E+03, 1.132199E+03, 1.156987E+03, 1.181964E+03, 1.207129E+03,
1.232482E+03, 1.258023E+03, 1.283750E+03, 1.309663E+03, 1.335762E+03,
1.362046E+03, 1.388514E+03, 1.415167E+03, 1.442003E+03, 1.469022E+03,
1.496224E+03, 1.523608E+03, 1.551174E+03, 1.578921E+03, 1.606849E+03,
1.634958E+03, 1.663247E+03, 1.691715E+03, 1.720363E+03, 1.749189E+03,
1.778194E+03, 1.807378E+03, 1.836739E+03, 1.866277E+03, 1.895992E+03,
1.925884E+03, 1.955953E+03, 1.986197E+03, 2.016618E+03, 2.047213E+03,
2.077984E+03, 2.108929E+03, 2.140049E+03, 2.171343E+03, 2.202811E+03,
2.234452E+03, 2.266267E+03, 2.298254E+03, 2.330415E+03, 2.362748E+03,
2.395252E+03, 2.427929E+03, 2.460778E+03, 2.493797E+03, 2.526988E+03,
2.560350E+03, 2.593882E+03, 2.627585E+03, 2.661458E+03, 2.695500E+03,
2.729713E+03, 2.764095E+03, 2.798646E+03, 2.833366E+03, 2.868254E+03,
2.903311E+03, 2.938537E+03, 2.973931E+03, 3.009492E+03, 3.045221E+03,
3.081118E+03, 3.117182E+03, 3.153413E+03, 3.189811E+03, 3.226376E+03,
3.263107E+03, 3.300000E+03,
};

const double TartBins[NTARTBINS+1] = {
   73.50,   80.67,   90.54,  100.00,  107.60,       
  117.90,  128.90,  140.50,  152.90,  166.00,
  179.90,  194.30,  209.60,  225.60,  242.50,
  260.10,  278.50,  297.80,  318.60,  338.80,
  360.60,  383.20,  406.80,  431.20,  456.50,
  482.80,  510.00,  538.20,  567.30,  597.40,
  628.50,  660.60,  693.80,  727.00,  763.20,
  799.50,  837.00,  875.50,  915.00,  955.90,
 1000.00, 1041.00, 1085.00, 1131.00, 1177.00,
 1225.00, 1274.00, 1325.00, 1377.00, 1430.00,
 1484.00, 1540.00, 1602.00, 1655.00, 1715.00,
 1776.00, 1802.00, 2027.00, 2281.00, 2566.00,
 2600.00, 2887.00, 3247.00, 3653.00, 4110.00,
 4624.00
};

const double MorseBins[NMORSEBINS+1] = {
      125.00,  141.70,  160.60,  182.00,  206.30,
      233.80,  264.90,  300.30,  340.30,  385.70,
      437.20,  495.50,  561.60,  636.50,  721.30,
      817.60,  926.60, 1050.20, 1190.30, 1349.00,
     1528.90, 1732.90, 1964.00, 2225.90, 2522.80,
     2859.30, 3240.70, 3762.90, 4162.80, 4718.00,
     5347.20, 6060.40, 6868.80, 7784.90, 8823.20,
    10000.00 
};

// Default lines are deprecated and will be deleted in a future revision
/* keV */
const double DefaultLines[NDEFAULTLINES] = {
   59.536,    //  Am-241   Pu-241
   92.279,    //  Np-237   Pu-241    Th-231   Pu-239    Other(s)
    92.35,    //  Np-237   Pu-241    Th-231   Pu-239    Other(s)
    92.78,    //  Th-234   Pu-242
    93.35,    //  U-235    Pu-239    U-233    Pu-241    Other(s)
   94.651,    //  Pu-239   Pu-239    Pu-241   Pu-241    Other(s)
   95.863,    //  Np-237   Pu-241    Th-231   Pu-239    Other(s)
   98.434,    //  Pu-239   Pu-239    Pu-241   Pu-241    Other(s)
  102.966,    //  Am-241   Pu-241    U-237    Pu-241
  103.035,    //  Pu-239   Pu-239
  105.362,    //  U-235    Pu-239    U-233    Pu-241    Other(s)
  108.166,    //  Np-237   Pu-241    Th-231   Pu-239    Other(s)
   108.99,    //  U-235    Pu-239    U-233    Pu-241    Other(s)
  111.025,    //  Pu-239   Pu-239    Pu-241   Pu-241    Other(s)
  111.897,    //  Np-237   Pu-241    Th-231   Pu-239    Other(s)
    112.8,    //  Th-234   Pu-242
  114.866,    //  Pu-239   Pu-239    Pu-241   Pu-241    Other(s)
  129.296,    //  Pu-239   Pu-239
   143.76,    //  U-235    Pu-239
  148.567,    //  Pu-241   Pu-241
   163.33,    //  U-235    Pu-239
   185.72,    //  U-235    Pu-239
   194.94,    //  U-235    Pu-239
   202.11,    //  U-235    Pu-239
  203.540,    //  Pu-239   Pu-239
   205.31,    //  U-235    Pu-239
  208.000,    //  U-237    Pu-241    Am-241   Pu-241
  238.578,    //  Pb-212   Pu-240
   240.76,    //  Ra-224   Pu-240    Ac-225   Pu-241
   258.18,    //  Pu-238   Pu-238    Pa-234m  Pu-242
   267.54,    //  U-237    Pu-241    Am-241   Pu-241
  332.853,    //  Pu-239   Pu-239
  335.430,    //  Am-241   Pu-241    U-237    Pu-241
   344.98,    //  Pu-239   Pu-239
  345.921,    //  U-235    Pu-239
  367.048,    //  Pu-239   Pu-239
   368.61,    //  U-237    Pu-241    Am-241   Pu-241
  368.605,    //  Am-241   Pu-241    U-237    Pu-241
  375.039,    //  Pu-239   Pu-239
   380.16,    //  Pu-239   Pu-239
  382.732,    //  Pu-239   Pu-239
  387.874,    //  U-235    Pu-239
  392.914,    //  Pu-239   Pu-239
   393.12,    //  Pu-239   Pu-239
  413.713,    //  Pu-239   Pu-239
  422.598,    //  Pu-239   Pu-239
  451.481,    //  Pu-239   Pu-239
  510.606,    //  Tl-208   Pu-240
  583.022,    //  Tl-208   Pu-240    Po-212   Pu-240
   617.43,    //  Pu-239   Pu-239
   618.94,    //  Am-241   Pu-241
  619.011,    //  Am-241   Pu-241
   640.15,    //  Pu-239   Pu-239
  645.969,    //  Pu-239   Pu-239
   652.99,    //  Am-241   Pu-241
  658.919,    //  Pu-239   Pu-239
  662.456,    //  Am-241   Pu-241
   688.77,    //  Am-241   Pu-241
  721.962,    //  Am-241   Pu-241
    722.7,    //  Am-241   Pu-241
   727.25,    //  Pu-238   Pu-238    Pa-234m  Pu-242    Other(s)
  742.817,    //  Pu-238   Pu-238    Pa-234m  Pu-242    Other(s)
   756.42,    //  Pu-239   Pu-239
  766.412,    //  Pu-238   Pu-238    Pa-234m  Pu-242    Other(s)
   769.19,    //  Pu-239   Pu-239
    860.3,    //  Tl-208   Pu-240
   1001.0,    //  Pu-238   Pu-238    Pa-234m  Pu-242
  1620.66,    //  Bi-212   Pu-240
  1737.73,    //  Pa-234m  Pu-242
  1831.36,    //  Pa-234m  Pu-242
  2614.35     //  Tl-208   Pu-240    Po-212   Pu-240
};

const std::vector<double>& GADRASBins() {
   static std::vector<double> v;
   static bool init=false;
   if (init == false) {
      v.resize(1001);
      for (int i = 1; i <= 1001; i++) {
         v[i-1] = 0.1 * pow( (double) i, 1.5);
      }
   }
   return v;
}

void EquiBins(std::vector<double>& v, double min, double max, int n) {
   v.resize(n+1);
   for (int i = 0; i <= n; i++) {
     v[i] = min + i * (max - min) / n;
   }
   v[n] = max;
}

// dE is proportional to slope * sqrt(E) + pedestal
// require E[i+1] - E[i] = K * dE | E=E[i]
// Given E[min], E[max]
void SqrtBins(std::vector<double>& v, const Options::SubOptions& suboptions) {
   v.resize(suboptions.nbins+1);
   double range = suboptions.max - suboptions.min;
   double k = range / suboptions.nbins /
      (suboptions.slope * sqrt(suboptions.min) + suboptions.pedestal);
   v[0] = suboptions.min;
   double err;
   do {
      for (int i = 1; i <= suboptions.nbins; i++) {
         v[i] = v[i-1] + k * 
            (suboptions.slope * sqrt(v[i-1]) + suboptions.pedestal);
      }
      err = fabs(suboptions.max - v[suboptions.nbins]);
      double crange = v[suboptions.nbins] - v[0];
      k *= range/crange;
   } while (err > 0.01);
}
