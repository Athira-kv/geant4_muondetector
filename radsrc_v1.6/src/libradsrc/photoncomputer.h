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
#ifndef PHOTONCOMPUTER_H_INCLUDE
#define PHOTONCOMPUTER_H_INCLUDE

#include <map>
#include <string>
#include <istream>
#include <iterator>
#include "porting.h"
#include "decaycomputer.h"

namespace radsrc {

   class CIsotopeDatabase;

   /*! \class CPhotonComputer
    *  \brief Computes the photon intensity from an aged mixture
    *
    *  Given an aged mixture, compute the intensity of radiation from each
    *  radioisotope present. A list of decays is retained to link the
    *  to the origins of the photons. Options for sorting and iterating
    *  through the list are provided. Also, a random photon may be sampled
    *  from the photon distributions.
    */
   class CPhotonComputer
   {
   public:
      class CGammaEntry;
      class CLineSelectionCmp;
      typedef std::map<CIsotope,double> IsotopeMixture;
      enum {ENERGY=0,INTENSITY=1};
      enum {PERMOLE,PERGRAM};

      private:
      // Local classes
      /*! \class CGammaEntryPrivate
        * \brief Minimum information for a photon: energy, intensity, originating decay(s)
        *
        * This class has no pointer to the decay chain information and is for
        * the exclusive use of CPhotonComputer. Photon information should be
        * access with CPhotonIterator.
        */
      class CGammaEntryPrivate {
         friend class CPhotonComputer;
         friend class CGammaEntry;
         friend class std::map<double,CGammaEntryPrivate>; // CPhotonComputer::GammaMap
         friend class CLineSelectionCmp;
      public:
         // properties
         double m_energy;
         HighPrecisionType m_intensity; // gammas / second / mole or /gram
         std::set<std::pair<CIsotope,CIsotope> > m_origins; //! Parent,Daughter
         /// Constructors
         CGammaEntryPrivate(void) : m_energy(0.0), m_intensity(0.0) { }
         CGammaEntryPrivate(double energy, HighPrecisionType intensity) :
               m_energy(energy), m_intensity(intensity) { }
         // Sorting comparisons
         static bool EnergyCompare(const CGammaEntryPrivate* left,
               const CGammaEntryPrivate *right)
               { return left->m_energy < right->m_energy; }
         static bool IntensityCompare(const CGammaEntryPrivate* left,
               const CGammaEntryPrivate *right)
               { return left->m_intensity > right->m_intensity; }
      };
      // Typedefs and Enums (private)
      typedef std::vector<const CGammaEntryPrivate *> GammaList;
      typedef std::map<double,CGammaEntryPrivate> GammaMap;

      public:

      class CPhotonIterator;
      /*! \class CGammaEntry
       *  \brief Class providing information about a specific photon line.
       *
       *  This class accesses the Bateman equation solution for
       *  information about the origins of a particular photon line.
       *
       *  N.B. changing either the CPhotonComputer or the CDecayComputer will
       *  invalidate the CGammaEntry object.
       *
       *  \throw CRadSourceException - occurs if you attempt to extract
       *  information from an uninitialized CGammaEntry object.
       */
      class CGammaEntry
      {
      public:
         typedef std::set<std::pair<CIsotope,CIsotope> > DecayList;
         typedef std::set<CIsotope> ParentList;
         CGammaEntry(void) : m_pdecayInfo(0), m_pgammaInfo(0) { }
         ~CGammaEntry(void) { }
         const ParentList& getChainParentIsotopes(void) const;
         const DecayList& getDecays(void) const;
         const ParentList& getParentIsotopes(void) const;
         std::string getParentDescription(void) const;
         double getEnergy(void) const;
         HighPrecisionType getIntensity(void) const;
         bool isValid(void) const;
      private:
         const CDecayComputer* m_pdecayInfo;
         const CGammaEntryPrivate* m_pgammaInfo;
         std::set<CIsotope> m_chainParents;
         std::set<CIsotope> m_parents;
         friend class CPhotonIterator;
         friend class CPhotonComputer;
         void computeParentIsotopes(void);
         void computeChainParentIsotopes(void);
         void newPosition(const CGammaEntryPrivate*);
      };

      /*! A const_iterator for the photon list that provides more details
       *
       *  Iterates through the sorted list of photons. It dereferences to a
       *  const CGammaEntry object relevant to the current photon.
       *
       *  N.B. changing either the CPhotonComputer or the CDecayComputer will
       *  invalidate both the CPhotonIterator and the CGammaEntry objects.
       */
      class CPhotonIterator : public std::iterator<std::input_iterator_tag, CGammaEntry> {
      public:
         // Constructors, etc
         CPhotonIterator(void);
         ~CPhotonIterator(void);
         CPhotonIterator(const CPhotonIterator&);
         CPhotonIterator& operator=(const CPhotonIterator&);

         // Operators
         bool operator==(const CPhotonIterator& it) const;
         bool operator!=(const CPhotonIterator& it) const; 
         CPhotonIterator& operator++(void);
         CPhotonIterator operator++(int);
         const CGammaEntry& operator*(void) const;
         const CGammaEntry* operator->(void) const;
      private:
         // Typedef
         typedef GammaList::const_iterator GammaListIterator;
         CPhotonIterator(const GammaListIterator& begin,
               const GammaListIterator& end, const CDecayComputer* decay);
         bool isValid(void) const;
         // Properties
         CGammaEntry m_gammaEntry; // dereference target
         bool m_invalid;
         GammaListIterator m_here;
         GammaListIterator m_end;
         const CDecayComputer* m_pdecayInfo;
         friend class CPhotonComputer;
      };

      class CBinnedData {
      public:
         enum {CENTERS,BOUNDARIES};
         int m_bintype;
         std::vector<double> m_energy;     // n+1 values
         std::vector<HighPrecisionType> m_intensity;  // n values
         std::vector<HighPrecisionType> m_cumulative; // n+1 values
         HighPrecisionType m_sum;
         int findBin(double x) {
            int n = m_energy.size();
            if (n == 0 || x < m_energy[0]) return -1;
            for (int i = 1; i < n; i++) {
               if (x < m_energy[i] && x >= m_energy[i-1]) return i;
            }
            return n;
         }
      };

      class CLineSelectionCmp {
      public:
         CLineSelectionCmp(void) : m_energy(0.0), m_fuzz(0.0) { }
         CLineSelectionCmp(double energy, double fuzz=0.0) : m_energy(energy), m_fuzz(fuzz) { }
         void set(double energy, double fuzz=0.0) { m_energy = energy; m_fuzz=fuzz; }
         bool operator()(const GammaMap::value_type& arg) const { return fabs(m_energy-arg.second.m_energy) <= m_fuzz; }
      private:
         double m_energy, m_fuzz;
      };


      // Constructors, etc.
      CPhotonComputer(const CDecayComputer&);
      ~CPhotonComputer(void);

      // Public Methods
      void computeGammas(int sortparam=ENERGY);
      CPhotonIterator beginGammas(void) const;
      CPhotonIterator endGammas(void) const;
      CPhotonIterator beginSelectedGammas(void) const;
      CPhotonIterator endSelectedGammas(void) const;
      int getNGammas(void) const;
      int getNSelected(void) const;

      enum BinSubject {BIN_BREM, BIN_GAMMA};
      void setBinning(BinSubject what, const std::vector<double>&);
      void setBinning(BinSubject what, const double *, int);
      void selectGammas(const std::vector<double>& v, double fuzz=0.0);
      void selectGammas(const double *lines=0, int n=0, double fuzz=0.0);

      void setLinesRange(double min, double max);
      void getLinesRange(double& min, double& max) const;

      //! Get the binned brem (if any)
      const CBinnedData& getBrem(void) const;
      //! Get the binned gammas (if any)
      const CBinnedData& getBinnedGammas(void) const;

      double getBremIntensity(void) const;
      double getBinnedIntensity(void) const;
      double getUnbinnedIntensity(void) const;
      double getLineIntensity(void) const;
      double getTotalIntensity(void) const;

      // Get a random photon and momentum
      // These methods are instantiated in the library for double & float
      double getPhoton(double (*rng)(void)) const;
      void getFourVector(double e[4], double (*rng)(void)) const;
      double getPhoton(float (*rng)(void)) const;
      void getFourVector(double e[4], float (*rng)(void)) const;
      void sampleBrem(bool f);
      bool sampleBrem(void) const;

   private:

      // Properties
      GammaList m_gammas;
      GammaList m_selectedGammas;
      CBinnedData m_binnedGammas;
      CBinnedData m_bremmstrahlung;
      int m_GammaIntensityUnits;
      const CDecayComputer& m_decaycomputer;
      GammaMap m_gammamap;
      std::vector<HighPrecisionType> m_discrete_cumulative; // n+1 values
      HighPrecisionType m_total_discrete;
      bool m_sampleBrem;
      double m_linesRange[2];

      // Private Methods
      void buildGammas(void);
      void addBrem(const CDecayMode::CBrem *pBrem, double scale);
      void computeCumulatives();
      template<class Iterator> void selectGammas(Iterator itLine, Iterator end, double fuzz);

      // No default constructor
      CPhotonComputer(void);
      // No copy
      CPhotonComputer(const CPhotonComputer&);
      CPhotonComputer& operator=(const CPhotonComputer&);
   };

   inline bool
   CPhotonComputer::sampleBrem() const {
      return m_sampleBrem;
   }

   inline void
   CPhotonComputer::sampleBrem(bool f) {
      m_sampleBrem = f;
   }

   inline int
   CPhotonComputer::getNSelected(void) const
   {
      return m_selectedGammas.size();
   }

   inline void
   CPhotonComputer::setLinesRange(double min, double max) {
      m_linesRange[0] = min;
      m_linesRange[1] = max;
   }

   inline void
   CPhotonComputer::getLinesRange(double& min, double& max) const {
      min = m_linesRange[0];
      max = m_linesRange[1];
   }

   inline bool
   CPhotonComputer::CGammaEntry::isValid() const
   {
      return m_pdecayInfo != 0 && m_pgammaInfo != 0;
   }
   inline int
   CPhotonComputer::getNGammas() const
   {
      return m_gammas.size();
   }
   inline CPhotonComputer::CPhotonIterator
   CPhotonComputer::CPhotonIterator::operator++(int)
   {
      CPhotonIterator tmp(*this);
      ++(*this);
      return tmp;
   }
   inline const CPhotonComputer::CBinnedData&
   CPhotonComputer::getBrem() const
   {
      return m_bremmstrahlung;
   }
   inline const CPhotonComputer::CBinnedData&
   CPhotonComputer::getBinnedGammas() const
   {
      return m_binnedGammas;
   }
   inline double
   CPhotonComputer::getBremIntensity(void) const
   {
      if (m_bremmstrahlung.m_cumulative.size() && m_sampleBrem)
         return m_bremmstrahlung.m_cumulative.back();
      return 0.0;
   }
   inline double
   CPhotonComputer::getBinnedIntensity(void) const
   {
      if (m_binnedGammas.m_cumulative.size())
         return m_binnedGammas.m_cumulative.back();
      return 0.0;
   }
   inline double
   CPhotonComputer::getUnbinnedIntensity(void) const
   {
      return getLineIntensity() - getBinnedIntensity();
   }
   inline double
   CPhotonComputer::getLineIntensity(void) const
   {
      return m_total_discrete;
   }
   inline double
   CPhotonComputer::getTotalIntensity(void) const
   {
      return m_total_discrete + getBremIntensity();
   }
}
#endif

