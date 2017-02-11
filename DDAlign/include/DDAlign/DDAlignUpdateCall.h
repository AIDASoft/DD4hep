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
#ifndef DD4HEP_DDALIGN_DDALIGNUPDATECALL_H
#define DD4HEP_DDALIGN_DDALIGNUPDATECALL_H

// Framework includes
#include "DDAlign/AlignmentsUpdateCall.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDAlign conversion stuff
  namespace Alignments  {
    
    /// Specialized conditions update callback for DDAlign alignments
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENT
     */
    class DDAlignUpdateCall : public AlignmentsUpdateCall  {
    public:
      /// Default constructor
      DDAlignUpdateCall() = default;
      /// Default destructor
      virtual ~DDAlignUpdateCall() = default;
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, const UpdateContext& context);
    };

  }    /* End namespace Alignments           */
}      /* End namespace DD4hep               */
#endif /* DD4HEP_DDALIGN_DDALIGNUPDATECALL_H */
