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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DDDBALIGNMENTUPDATECALL_H
#define DD4HEP_DDDB_DDDBALIGNMENTUPDATECALL_H

// Framework includes
#include "DD4hep/ConditionDerived.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {
    
    /// Specialized conditions update callback for DDDB alignments
    /**
     *  Used by clients to update an alignment condition.
     *  The input data, a condition with an AbstractMap data structure
     *  containing the alignment delta data.
     *
     *  The condition must be fully computed in two steps:
     *  - firstly it is created here and the alignment delta copied.
     *  - secondly, once all updates are processed, the world-transformations
     *    for the alignment object(s) are computed by the AlignmentManager
     *    objects.
     *  The AlignmentManager also keeps track of newly created alignment 
     *  conditions and is informed in the callback.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class DDDBAlignmentUpdateCall : public Conditions::ConditionUpdateCall  {
    public:
      typedef Conditions::Condition              Condition;
      typedef Conditions::ConditionKey           ConditionKey;
      typedef Conditions::ConditionUpdateContext UpdateContext;
    public:
      /// Default constructor
      DDDBAlignmentUpdateCall();
      /// Default destructor
      virtual ~DDDBAlignmentUpdateCall();
      /// Interface to client Callback in order to update the condition
      virtual Condition operator()(const ConditionKey& key, const UpdateContext& context);
    };

  }    /* End namespace DDDB                   */
}      /* End namespace DD4hep                  */
#endif /* DD4HEP_DDDB_DDDBALIGNMENTUPDATECALL_H */
