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
#ifndef DD4HEP_DDALIGN_ALIGNMENTREGISTER_H
#define DD4HEP_DDALIGN_ALIGNMENTREGISTER_H

// Framework includes
#include "DD4hep/Alignments.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Detector.h"
#include "DDAlign/AlignmentsManager.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {
  
    // Forward declarations
    class AlignmentUpdateCall;

    /// Create lignment dependencies from conditions
    /**
     *   Conditions analyser to select alignments.and create the
     *   corresponding alignment condition dependencies.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDALIGN
     */
    class AlignmentsRegister : public DetElement::Processor {
    public:
      AlignmentsManager      alignmentMgr;
      AlignmentUpdateCall*   updateCall;
      Conditions::UserPool*  user_pool;

      /// Initializing constructor
      AlignmentsRegister(AlignmentsManager m, AlignmentUpdateCall* c, UserPool* p);
      /// Default destructor
      virtual ~AlignmentsRegister();
      /// Callback to output conditions information
      virtual int processElement(DetElement de);
    };
  }       /* End namespace Alignments              */
}         /* End namespace DD4hep                  */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTREGISTER_H    */
