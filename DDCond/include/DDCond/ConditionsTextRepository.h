// $Id: ConditionsTextrepository.h 2336 2016-09-07 17:27:40Z markus.frank@cern.ch $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_CONDITIONS_CONDITIONSTEXTREPOSITORY_H
#define DD4HEP_CONDITIONS_CONDITIONSTEXTREPOSITORY_H

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

    /// Forward declarations


    /// Base class to be implemented by objects to listen on condition callbacks
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsTextRepository  {
    public:
      class Entry  {
      public:
        std::string name, address;
        Condition::key_type key = 0;
        Entry() = default;
        Entry(const Entry& copy) = default;
        ~Entry() = default;
        Entry& operator=(const Entry& copy) = default;
      };
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

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_CONDITIONSTEXTREPOSITORY_H  */
