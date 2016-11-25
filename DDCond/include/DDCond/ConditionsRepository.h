//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_CONDITIONS_CONDITIONSREPOSITORY_H
#define DD4HEP_CONDITIONS_CONDITIONSREPOSITORY_H

// Framework include files
#include "DDCond/ConditionsManager.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Forward declarations
  class IOV;
  class IOVType;

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Base class to be implemented by objects to listen on condition callbacks
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsRepository  {
    public:
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Entry  {
      public:
        std::string name, address;
        Condition::key_type key;
        Entry() {}
        Entry(const Entry& e) : name(e.name), address(e.address), key(e.key) {}
        Entry& operator=(const Entry& e) {
          if ( this != &e )  {
            key = e.key;
            name = e.name;
            address = e.address;
          }
          return *this;
        }
      };
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

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_CONDITIONSREPOSITORY_H  */
