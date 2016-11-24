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
#ifndef DD4HEP_DDALIGN_DDALIGNFORWARDCALL_H
#define DD4HEP_DDALIGN_DDALIGNFORWARDCALL_H

// Framework includes
#include "DDAlign/AlignmentUpdateCall.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDAlign conversion stuff
  namespace Alignments  {
    
    /// Specialized conditions forward callback for DDAlign alignments
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENT
     */
    class DDAlignForwardCall : public AlignmentUpdateCall  {
    public:
      /// Default constructor
      DDAlignForwardCall() = default;
      /// Default destructor
      virtual ~DDAlignForwardCall() = default;
      /// Interface to client Callback in order to forward the condition
      virtual Condition operator()(const ConditionKey& key, const UpdateContext& context);
    };

  }    /* End namespace Alignments            */
}      /* End namespace DD4hep                */
#endif /* DD4HEP_DDALIGN_DDALIGNFORWARDCALL_H */
