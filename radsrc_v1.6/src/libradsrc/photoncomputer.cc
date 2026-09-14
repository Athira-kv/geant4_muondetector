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
#include <set>
#include <sstream>
#include <time.h>

#include "photoncomputer.h"
#include "isotopedb.h"
#include "spline.h"


// Do we sample Brem by default?
#ifndef BREM_SAMPLING
#define DEFAULT_BREM_SAMPLING true
#else
#define DEFAULT_BREM_SAMPLING BREM_SAMPLING
#endif

namespace radsrc {

   namespace {
      int search(const std::vector<HighPrecisionType>& container, HighPrecisionType value, bool& inbounds) {
         int n = container.size();
         if (n == 0) { inbounds = false; return 0; }
         if (n == 1) { inbounds = (value == container[0]); return 0; }
         if (value < container[0]) { inbounds = false; return 0; }
         int i = 0;
         inbounds = true;
         while (i < n-1) {
            if (value >= container[i] && value < container[i+1]) return i;
            i++;
         }
         inbounds = false;
         return n-1;
      }
   }

   CPhotonComputer::CPhotonComputer(const CDecayComputer& dc) : m_decaycomputer(dc), m_sampleBrem(DEFAULT_BREM_SAMPLING) {
      m_linesRange[0] = m_linesRange[1] = 0.0; // sample all lines
   }

#if 0
   CPhotonComputer::CPhotonComputer(const CPhotonComputer& pc)  : m_decaycomputer(pc.m_decaycomputer), m_sampleBrem(pc.m_sampleBrem) {
   }

   CPhotonComputer& CPhotonComputer::operator=(const CPhotonComputer& pc) {
      return *this;
   }
#endif

   CPhotonComputer::~CPhotonComputer(void) {
      // all memory is automatically deleted
   }

   /*! Build list of photon lines from list of isotopes and their concentrations
    *
    *  Obtains a list of isotopes from the associated CDecayComputer object.
    *  The photon list of each branch of each isotope in the list is consulted
    *  and entered into a mapping along with intensity, which is cumulative.
    *  Finally the units are converted into photons/sec/gram.
    */
   void CPhotonComputer::buildGammas() {

#ifdef EMULATE_INTENSITY_ERROR
      std::cerr << "N.B. Libradsrc is computing gamma intensities with error emulation enabled." << std::endl;
      std::cerr << "This is intended for comparison with legacy applications only and should" << std::endl;
      std::cerr << "not be used in a production environment. To disable, undefine" << std::endl;
      std::cerr << "EMULATE_INTENSITY_ERROR and recompile." << std::endl;
#endif
      // Check for completed age computation
      if (m_decaycomputer.getAgedMixture().size()==0) return;

      // Loop through isotopes in list
      IsotopeMixture::const_iterator it = m_decaycomputer.getAgedMixture().begin();
      const CIsotopeData *pdata;
      while (it != m_decaycomputer.getAgedMixture().end()) {
         // Get Isotope Data
         pdata = m_decaycomputer.getIsotopeData(it->first);
         if (pdata == 0) {
            ++it;
            continue;
         }
         HighPrecisionType quantity = it->second;
         if (pdata) {
            // Loop over branches
            CIsotopeData::DecayIterator decayit, decayendit;
            decayit = pdata->decaysBegin();
            decayendit = pdata->decaysEnd();
            while (decayit != decayendit) {
               // Get Photon List
               std::vector<CPhoton>::const_iterator pit, eit;
               pit = decayit->beginPhotons();
               eit = decayit->endPhotons();
               // Loop over photons
               while (pit != eit) {
                  // creates if necessary, initialized to 0
                  if (pit->getEnergy() < m_linesRange[0] ||
                     (m_linesRange[1] > 0.0 && pit->getEnergy() > m_linesRange[1])) {
                     pit++;
                     continue;
                  }
                  CGammaEntryPrivate& entry =
                        m_gammamap[pit->getEnergy()];
                  entry.m_energy = pit->getEnergy();
#ifdef EMULATE_INTENSITY_ERROR
                  entry.m_intensity += log(2.0) / pdata->getHalflife() *
                        quantity * (pit->getFraction());
#else
                  entry.m_intensity += pdata->getDecayRate(decayit) *
                        quantity * (pit->getFraction());
#endif
                  entry.m_origins.insert(std::pair<CIsotope,CIsotope>
                        (pdata->getIsotope(),
                        decayit->getDaughter()));
                  ++pit;
               } // for photons in branch
               if (decayit->getNBremBins() != 0) {
                  double bremscale;
                  // hard coded for Pa-234m betas, as data is in
                  // Pa-234m photons / gram U-238
                  //! \todo move Pa-238m scaling to Brem-Read routine, Brem should be in units of per decay
                  bremscale = quantity / pdata->getHalflife() * 1.4E17;
                  addBrem(decayit->m_pBrem, bremscale);
               }
               ++decayit;
            } // while decay branch in isotope
         } // if pdata
         ++it;
      }
      double avemw = m_decaycomputer.getInputMixture().computeAverageMass();
      GammaMap::iterator git = m_gammamap.begin();
      m_gammas.clear();
      while (git != m_gammamap.end()) {
         m_gammas.push_back(&(git->second));
         git->second.m_intensity *= 6.023E23 / avemw;
         ++git;
      }
   }

   void CPhotonComputer::computeGammas(int sortmethod) {
      if (m_gammamap.size() == 0)
         buildGammas();
      if (sortmethod == ENERGY) {
        std::stable_sort(m_gammas.begin(), m_gammas.end(),CGammaEntryPrivate::EnergyCompare);
      } else if (sortmethod == INTENSITY) {
        std::stable_sort(m_gammas.begin(), m_gammas.end(),CGammaEntryPrivate::IntensityCompare);
      }
      computeCumulatives();
   }

   void CPhotonComputer::setBinning(BinSubject what, const std::vector<double>& bins) {
      CBinnedData *pBins;
      if (what == BIN_BREM) pBins = &m_bremmstrahlung;
      else pBins = &m_binnedGammas;
      pBins->m_bintype = CBinnedData::BOUNDARIES;
      pBins->m_energy = bins;
      pBins->m_intensity.assign(bins.size(),0.0);
   }

   void CPhotonComputer::setBinning(BinSubject what, const double *ptr, int n) {
      CBinnedData *pBins;
      if (what == BIN_BREM) pBins = &m_bremmstrahlung;
      else pBins = &m_binnedGammas;
      pBins->m_bintype = CBinnedData::BOUNDARIES;
      pBins->m_energy.assign(ptr,ptr+n);
      pBins->m_intensity.assign(n,0.0);
   }

   void CPhotonComputer::selectGammas(const std::vector<double>& lines, double fuzz) {
     selectGammas(lines.begin(),lines.end(),fuzz);
   }

   void CPhotonComputer::selectGammas(const double *lines, int n_lines, double fuzz) {
     selectGammas(lines,lines+n_lines,fuzz);
   }

   template<class Iterator> void CPhotonComputer::selectGammas(Iterator itLine, Iterator end, double fuzz) {
     GammaMap::const_iterator it;
     int n = m_binnedGammas.m_energy.size();
     int i;
     it = m_gammamap.begin();
     while(it != m_gammamap.end()) {
        i = m_binnedGammas.findBin(it->second.m_energy);
        if (i >=0 && i < n) m_binnedGammas.m_intensity[i]
           += it->second.m_intensity;
        ++it;
     }
     m_selectedGammas.resize(0);
     std::set<const CGammaEntryPrivate*> selected_lines; // unique set of selected lines
     while (itLine != end) {
        GammaMap::const_iterator start = m_gammamap.begin();
        GammaMap::const_iterator end = m_gammamap.end();
        CPhotonComputer::CLineSelectionCmp cmp(*itLine,fuzz);
        while ( (it = std::find_if(start,end,cmp)) != end) {
           start = it; start++;
           selected_lines.insert(&(it->second));
        }
        ++itLine;
     }
     std::set<const CGammaEntryPrivate*>::const_iterator sit = selected_lines.begin();
     while (sit != selected_lines.end()) {
           m_selectedGammas.push_back(*sit);
           i = m_binnedGammas.findBin((*sit)->m_energy);
           if (i >=0 && i < n) m_binnedGammas.m_intensity[i]
              -= (*sit)->m_intensity;
        sit++;
     }
     std::sort(m_selectedGammas.begin(), m_selectedGammas.end(), 
                CGammaEntryPrivate::EnergyCompare);
     computeCumulatives();
   }

   void CPhotonComputer::addBrem(const CDecayMode::CBrem *pBrem, double scale) {
     int i, n, nmax;
     CSpline spline;
     nmax = pBrem->m_nbins;
     if (nmax < 2 || m_bremmstrahlung.m_energy.size() < 2) return;

     n = nmax;
     CSpline::ValArray x(n+1),y(n+1);
     
     x[0] = pBrem->m_binbounds[0]/2;
     y[0] = y[n-1] = 0.0;
     for (i = 0; i < n; i++) {
        x[i+1] = (pBrem->m_binbounds[i] + pBrem->m_binbounds[i+1])/2;
        y[i+1] = pBrem->m_intensity[i];
     }
     double max_energy = x.back();
     spline.AttachValues(x,y);

     std::vector<double> newx, newy;
     for (i = 0; i < static_cast<int>(m_bremmstrahlung.m_intensity.size()); i++) {
        double energy = 
           (m_bremmstrahlung.m_energy[i] + m_bremmstrahlung.m_energy[i+1])/2;
        newx.push_back(energy);
        newy.push_back(spline(energy));
        if (energy > max_energy) break;
     }
     if (newy[0] < 0) newy[0] = 0;

     int max;
     for (max = 1; max < static_cast<int>(newy.size()); max++) {
        if (newy[max] <= 0) {
           while (i < static_cast<int>(newy.size())) newy[i++] = 0;
           break;
        }
     }

      CBinnedData tmpbins;
      tmpbins.m_energy = m_bremmstrahlung.m_energy;
      tmpbins.m_intensity.resize(m_bremmstrahlung.m_energy.size());
      double ubb,lbb,ubn,lbn;
      int ib,in;
      ib = 0; in = 0;
      lbn = tmpbins.m_energy[0];
      ubn = tmpbins.m_energy[1];
      lbb = pBrem->m_binbounds[0];
      ubb = pBrem->m_binbounds[1];
      double oldtotal,newtotal;
      oldtotal = newtotal = 0.0;
      
      // This algorithm requires all bin widths to be > 0
      while (ib < nmax && in < max) {
         double f;
         int db,dn;
         // b bin is entirely below n bin
         if (ubb <= lbn) {
            ib++;
            lbb = pBrem->m_binbounds[ib];
            ubb = pBrem->m_binbounds[ib+1];
            continue;
         }
          // b bin is entirely above n bin
         if (lbb >= ubn) {
            in++;
            lbn = tmpbins.m_energy[in];
            ubn = tmpbins.m_energy[in+1];
            continue;
         }
         double bremkev = pBrem->m_binbounds[ib+1]-pBrem->m_binbounds[ib];
         db = dn = 0;
         if (lbb >= lbn && ubb <= ubn) { // b bin is entirely within n bin
            f = 1;
            db = 1;
         }
         else if (lbb <= lbn && ubb >= ubn) { // n bin is entirely within b bin
            f = (ubn-lbn)/bremkev;
            dn = 1;
         }
         else if (ubb < ubn) { // lbb <= lbn < ubb < ubn
            f = (ubb-lbn)/bremkev;
            db = 1;
            lbn = ubb;
         } else { // lbn < lbb < ubn <= ubb
            f = (ubn-lbb)/bremkev;
            lbb = ubn;
            dn = 1;
         }
         tmpbins.m_intensity[in] += f * newy[in];
         newtotal += f * newy[in];
         oldtotal += f * pBrem->m_intensity[ib];

         if (dn) {
            in++;
            lbn = tmpbins.m_energy[in];
            ubn = tmpbins.m_energy[in+1];
         }
         if (db) {
            ib++;
            lbb = pBrem->m_binbounds[ib];
            ubb = pBrem->m_binbounds[ib+1];
         }
      } // end while

      // Truncate at the first 0 (!)
      for (max = 1; max < static_cast<int>(tmpbins.m_energy.size())-1; max++) {
         if (tmpbins.m_intensity[max] <= 0.0) {
            break;
         }
      }

      for (i = 0; i < max && i < static_cast<int>(m_bremmstrahlung.m_intensity.size())-2 ; i++) {
         m_bremmstrahlung.m_intensity[i+1] +=
            tmpbins.m_intensity[i] * oldtotal / newtotal * scale;
      }
      m_bremmstrahlung.m_intensity[0] = 0.0;
   }


   void CPhotonComputer::computeCumulatives() {
      int i, n;

// Discrete first
      n = m_gammas.size();
      m_discrete_cumulative.resize(n+1);
      m_discrete_cumulative[0] = 0.0;
      for (i = 0; i < n; i++) {
         m_discrete_cumulative[i+1] = m_discrete_cumulative[i]
            + m_gammas[i]->m_intensity;
      }
      m_total_discrete = m_discrete_cumulative.back();
// Then Brem
      n = m_bremmstrahlung.m_intensity.size();
      m_bremmstrahlung.m_cumulative.resize(n+1);
      m_bremmstrahlung.m_cumulative[0] = 0.0;
      if (n) {
         for (i = 0; i < n; i++) {
            m_bremmstrahlung.m_cumulative[i+1] = m_bremmstrahlung.m_cumulative[i]
               + m_bremmstrahlung.m_intensity[i];
         }
      }
      m_bremmstrahlung.m_sum = m_bremmstrahlung.m_cumulative.back();
// Then Binned
      n = m_binnedGammas.m_intensity.size();
      m_binnedGammas.m_cumulative.resize(n+1);
      m_binnedGammas.m_cumulative[0] = 0.0;
      if (n) {
         for (i = 0; i < n; i++) {
            m_binnedGammas.m_cumulative[i+1] = m_binnedGammas.m_cumulative[i]
               + m_binnedGammas.m_intensity[i];
         }
      }
      m_binnedGammas.m_sum = m_binnedGammas.m_cumulative.back();
   }


   // Many random number version
   // could be done with 1 random number
   double CPhotonComputer::getPhoton(double (*rng)(void)) const {
      bool inbounds;
      int i;
      HighPrecisionType x = rng() * (m_total_discrete + m_bremmstrahlung.m_sum);
      if (x < m_total_discrete || m_sampleBrem == false) { // discrete
         x = rng() * m_total_discrete;
         i = search(m_discrete_cumulative, x, inbounds);
         return m_gammas[i]->m_energy;
      }
      x = rng() * m_bremmstrahlung.m_sum;
      i = search(m_bremmstrahlung.m_cumulative, x, inbounds);
      double deltaE = m_bremmstrahlung.m_energy[i+1] - m_bremmstrahlung.m_energy[i];
      x = rng() * deltaE;
      return m_bremmstrahlung.m_energy[i] + x;
   }

   void CPhotonComputer::getFourVector(double e[4], double (*rng)(void)) const {
      double pi = 4 * atan(1.0);
      e[0] = getPhoton(rng);
      e[3] = 1.0 - 2*rng();
      double phi = 2 * pi * rng();
      double rho = e[0] * sqrt(1-e[3]*e[3]);
      e[1] = rho * cos(phi);
      e[2] = rho * sin(phi);
      e[3] *= e[0];
   }
   double CPhotonComputer::getPhoton(float (*rng)(void)) const {
      bool inbounds;
      int i;
      HighPrecisionType x = rng() * (m_total_discrete + m_bremmstrahlung.m_sum);
      if (x < m_total_discrete || m_sampleBrem == false) { // discrete
         x = rng() * m_total_discrete;
         i = search(m_discrete_cumulative, x, inbounds);
         return m_gammas[i]->m_energy;
      }
      x = rng() * m_bremmstrahlung.m_sum;
      i = search(m_bremmstrahlung.m_cumulative, x, inbounds);
      double deltaE = m_bremmstrahlung.m_energy[i+1] - m_bremmstrahlung.m_energy[i];
      x = rng() * deltaE;
      return m_bremmstrahlung.m_energy[i] + x;
   }

   void CPhotonComputer::getFourVector(double e[4], float (*rng)(void)) const {
      double pi = 4 * atan(1.0);
      e[0] = getPhoton(rng);
      e[3] = 1.0 - 2*rng();
      double phi = 2 * pi * rng();
      double rho = e[0] * sqrt(1-e[3]*e[3]);
      e[1] = rho * cos(phi);
      e[2] = rho * sin(phi);
      e[3] *= e[0];
   }
   CPhotonComputer::CPhotonIterator
   CPhotonComputer::beginGammas() const
   {
      return CPhotonIterator(m_gammas.begin(),m_gammas.end(),&m_decaycomputer);
   }

   CPhotonComputer::CPhotonIterator
   CPhotonComputer::endGammas() const
   {
      return CPhotonIterator();
   }

   CPhotonComputer::CPhotonIterator
   CPhotonComputer::beginSelectedGammas() const
   {
      return CPhotonIterator(m_selectedGammas.begin(),m_selectedGammas.end(),&m_decaycomputer);
   }

   CPhotonComputer::CPhotonIterator
   CPhotonComputer::endSelectedGammas() const
   {
      return CPhotonIterator();
   }

   // *--------------------- CPhotonIterator ----------------------*
   CPhotonComputer::CPhotonIterator::CPhotonIterator(void) : m_pdecayInfo(0)
   {
   }
   CPhotonComputer::CPhotonIterator::~CPhotonIterator(void)
   {
   }
   CPhotonComputer::CPhotonIterator&
   CPhotonComputer::CPhotonIterator::operator=(const CPhotonComputer::CPhotonIterator& it)
   {
      m_gammaEntry = it.m_gammaEntry;
      m_pdecayInfo = it.m_pdecayInfo;
      m_here = it.m_here;
      m_end = it.m_end;
      return *this;
   }

   CPhotonComputer::CPhotonIterator::CPhotonIterator(const CPhotonComputer::CPhotonIterator& it) :
         m_gammaEntry(it.m_gammaEntry),
         m_here(it.m_here),
         m_end(it.m_end), m_pdecayInfo(it.m_pdecayInfo)
   {
   }
   bool
   CPhotonComputer::CPhotonIterator::operator==(const CPhotonComputer::CPhotonIterator& it) const
   {
      bool valid = it.isValid();
      if (isValid()) {
         if (valid) {
            return m_here == it.m_here && m_end == it.m_end &&
               m_pdecayInfo == it.m_pdecayInfo;
         } else {
            return false; // I'm valid, it's not
         }
      }
      return !valid;
   }

   bool
   CPhotonComputer::CPhotonIterator::operator!=(const CPhotonComputer::CPhotonIterator& it) const
   {
      bool valid = it.isValid();
      if (isValid()) {
         if (valid) {
            return m_here != it.m_here || m_end != it.m_end ||
               m_pdecayInfo != it.m_pdecayInfo;
         } else {
            return true; // I'm valid, it's not
         }
      }
      return valid;
   }
   CPhotonComputer::CPhotonIterator&
   CPhotonComputer::CPhotonIterator::operator++(void)
   {
      ++m_here;
      if (isValid()) {
            m_gammaEntry.newPosition(*m_here);
      }
      else {
         m_gammaEntry.m_pgammaInfo = 0;
      }
      return *this;
   }
   const CPhotonComputer::CGammaEntry&
   CPhotonComputer::CPhotonIterator::operator*(void) const
   {
      return m_gammaEntry;
   }
   const CPhotonComputer::CGammaEntry*
   CPhotonComputer::CPhotonIterator::operator->(void) const
   {
      return &m_gammaEntry;
   }

   //! Constructor for use by CPhotonComputer
   CPhotonComputer::CPhotonIterator::CPhotonIterator(const GammaListIterator& begin,
         const GammaListIterator& end, const CDecayComputer* decay) :
         m_here(begin), m_end(end), m_pdecayInfo(decay)
   {
         m_gammaEntry.m_pdecayInfo = decay;
         if (isValid()) {
            m_gammaEntry.newPosition(*m_here);
         }
         else {
            m_gammaEntry.m_pgammaInfo = 0;
         }
   }

   bool
   CPhotonComputer::CPhotonIterator::isValid() const
   {
      return (m_pdecayInfo != 0) && (m_here != m_end);
   }

   // *--------------------- CGammaEntry ----------------------*

   void
   CPhotonComputer::CGammaEntry::newPosition(const CGammaEntryPrivate *ptr)
   {
      m_pgammaInfo = ptr;
      computeParentIsotopes();
      computeChainParentIsotopes();
   }
   void
   CPhotonComputer::CGammaEntry::computeParentIsotopes()
   {
      std::set<std::pair<CIsotope,CIsotope> >::const_iterator it, end;
      const std::set<std::pair<CIsotope,CIsotope> >& decays = getDecays();
      it = decays.begin();
      end = decays.end();
      m_parents.clear();
      while (it != end) {
         m_parents.insert(it->first);
         ++it;
      }
   }
   void
   CPhotonComputer::CGammaEntry::computeChainParentIsotopes()
   {
      std::set<CIsotope>::const_iterator it, end;
      it = m_parents.begin();
      end = m_parents.end();
      m_chainParents.clear();
      while (it != end) {
         const CIsotope& isotope = *it;
         const CBatemanSolution* solution = m_pdecayInfo->getSolution(isotope);
         if (solution) { // MSC++V6 doesn't have insert(iterator,iterator)
            std::set<CIsotope>::const_iterator i2, e2;
            i2 = solution->getChainParents().begin();
            e2 = solution->getChainParents().end();
            while (i2 != e2) {
               m_chainParents.insert(*i2);
               ++i2;
            }
         }
         ++it;
      }
   }
   const CPhotonComputer::CGammaEntry::DecayList&
   CPhotonComputer::CGammaEntry::getDecays() const
   {
      return m_pgammaInfo->m_origins;
   }
   const CPhotonComputer::CGammaEntry::ParentList&
   CPhotonComputer::CGammaEntry::getParentIsotopes() const
   {
      return m_parents;
   }
   const CPhotonComputer::CGammaEntry::ParentList&
   CPhotonComputer::CGammaEntry::getChainParentIsotopes() const
   {
      return m_chainParents;
   }
   double CPhotonComputer::CGammaEntry::getEnergy() const
   {
      return m_pgammaInfo->m_energy;
   }
   HighPrecisionType CPhotonComputer::CGammaEntry::getIntensity() const
   {
      return m_pgammaInfo->m_intensity;
   }
   std::string
   CPhotonComputer::CGammaEntry::getParentDescription(void) const
   {
      std::string description;
      ParentList::const_iterator pit, pend;
      DecayList::const_iterator dit, dend;
      pit = getChainParentIsotopes().begin();
      pend = getChainParentIsotopes().end();
      description += "[ ";
      while (pit != pend) {
         description += pit->toString();
         ++pit;
         if (pit != pend) description +=", ";
      }
      description += " ] ";
      dit = getDecays().begin();
      dend = getDecays().end();
      while (dit != dend) {
         description += dit->first.toString().c_str();
         description += " -> ";
         description += dit->second.toString().c_str();
         ++dit;
         if (dit != dend) description +=", ";
      }
      return description;
   }

}

