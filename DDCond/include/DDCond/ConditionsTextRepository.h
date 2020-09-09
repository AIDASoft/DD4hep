//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights For.
//
// reserved the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef DDCOND_CONDITIONSTEXTREPOSITORY_H
#define DDCOND_CONDITIONSTEXTREPOSITORY_H

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

    /// Forward declarations


    /// Base class to be implemented by objects to listen on condition callbacks
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsTextRepository  {
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
        Entry(const Entry& copy) = default;
        ~Entry() = default;
        Entry& operator=(const Entry& copy) = default;
      };

      /// Definition of the entry collection
      typedef std::vector<Entry> Data;

    public:
      /// Default constructor
      ConditionsTextRepository();
      /// Default destructor
      virtual ~ConditionsTextRepository();
      /// Save the textrepository to file
      int save(ConditionsManager m, const std::string& output)  const;
      /// Load the textrepository from file and fill user passed data structory
      int load(const std::string& input, Data& data)  const;
    };

  } /* End namespace detail               */
} /* End namespace dd4hep                   */

#endif // DDCOND_CONDITIONSTEXTREPOSITORY_H
