//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DDCOND_CONDITIONSREPOSITORY_H
#define DDCOND_CONDITIONSREPOSITORY_H

// Framework include files
#include "DDCond/ConditionsManager.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declarations
  class IOV;
  class IOVType;

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    /// Base class to be implemented by objects to listen on condition callbacks
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsRepository  {
    public:
      /// Definition of a single Entry in the conditions repository
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Entry  {
      public:
        std::string name, address;
        Condition::key_type key = 0;
        Entry() = default;
        Entry(const Entry& e) = default;
        Entry& operator=(const Entry& e) = default;
      };

      /// Definition of the entry collection
      typedef std::vector<Entry> Data;

    public:
      /// Default constructor
      ConditionsRepository();
      /// Default destructor
      virtual ~ConditionsRepository();
      /// Save the repository to file
      int save(ConditionsManager m, const std::string& output)  const;
      /// Load the repository from file and fill user passed data structory
      int load(const std::string& input, Data& data)  const;
    };

  } /* End namespace cond             */
} /* End namespace dd4hep                   */

#endif // DDCOND_CONDITIONSREPOSITORY_H
