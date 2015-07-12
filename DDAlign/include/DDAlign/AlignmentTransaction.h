// $Id$
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
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTTRANSACTION_H
#define DD4HEP_ALIGNMENT_ALIGNMENTTRANSACTION_H

// Framework include files
#include "DD4hep/Primitives.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  namespace XML { class Handle_t; }

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class LCDD;
    class AlignmentCache;

    /// Manage alignment transaction to the cache for a given LCDD instance
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentTransaction {
    public:
      /// Internal flag to remember transaction contexts
      bool flag;
      /// Reference to the current LCDD instance
      LCDD& lcdd;
      /// Reference to the alignment cache
      AlignmentCache* m_cache;

      /// Default constructor
      AlignmentTransaction(LCDD& l, const XML::Handle_t& e);
      /// Default destructor
      ~AlignmentTransaction();
    };

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_ALIGNMENT_ALIGNMENTTRANSACTION_H       */
