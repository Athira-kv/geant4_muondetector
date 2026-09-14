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


#ifndef DBMANAGER_H_INCLUDE
#define DBMANAGER_H_INCLUDE
#include "porting.h"
#include <string>
#include <map>

namespace radsrc
{
   class CIsotopeDatabase;
   /*  -------------------------------------------------------------------- */
   /*! \class CDatabaseManager
    *  \brief Loads databases on demand and provides CIsotopeDatabase objects.
    *
    *  The CDatabase Manager is a singleton that maintains a list of all
    *  loaded databases. A database is identfied by a DatabaseIdentifier,
    *  which is a DatabaseType and a (possibly empty) information string.
    *  This information string will usually be a filename, but is interpreted
    *  by the instantiating routine alone. Instantiated databases are stored
    *  in a mapping of (hopefully) unique databases.
    */
   /*  -------------------------------------------------------------------- */
   class CDatabaseManager
   {
   public:
      // Enums and Typedefs
      //! Types of database formats supported \todo Add ENSDF support with NSDFLIB
      enum DatabaseType {
         LEGACY,        //!< GAMGEN .GAM and .BRM files
         ENSDF,         //!< ENSDF
         ENSDF_ERRATA   //!< ENSDF with a corrections file
      };
      typedef std::pair<int,std::string> DatabaseIdentifier;

      // Public static methods
      static CDatabaseManager* getDatabaseManager(void);
      static CIsotopeDatabase* getIsotopeDatabase(int type, std::string info="");
   private:
      // Properties
      static CDatabaseManager* m_this;
      std::map<DatabaseIdentifier,CIsotopeDatabase*> m_databases;

      // Private constructor / destructor / copy / assign
      CDatabaseManager(void) { }
      ~CDatabaseManager(void);
      CDatabaseManager(const CDatabaseManager&);
      CDatabaseManager& operator=(const CDatabaseManager&);
   };

   inline CDatabaseManager*
   CDatabaseManager::getDatabaseManager()
   {
      if (CDatabaseManager::m_this == 0) {
         CDatabaseManager::m_this = new CDatabaseManager();
      }
      return CDatabaseManager::m_this;
   }
}

#endif
