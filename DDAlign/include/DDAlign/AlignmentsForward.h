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
#ifndef DD4HEP_DDALIGN_ALIGNMENTFORWARD_H
#define DD4HEP_DDALIGN_ALIGNMENTFORWARD_H

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

    /// Create alignment dependencies for child elements if a parent has re-alignments
    /**
     *   Conditions analyser to select alignments and create the
     *   corresponding alignment condition dependencies.
     *
     *   This class has the same interface like AlignmentsRegister.
     *   Please see AlignmentsRegister.h for further information.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDALIGN
     */
    class AlignmentsForward : public DetElement::Processor {
    public:
      /// Reference to the alignment manager object
      AlignmentsManager      alignmentMgr;
      /// The callback to be registered for the update mechanism
      AlignmentUpdateCall*   updateCall;
      /// Conditions pool used to access the basic conditions object
      Conditions::UserPool*  user_pool;
      /// Extension property to construct the name of the alignment condition
      std::string            extension;
      /// Name of the alignment alias for the detector elements alignment object
      std::string            alias;
      /// Flag if an alias to the real alignment object should be registered
      bool                   haveAlias;

      /// Initializing constructor
      AlignmentsForward(AlignmentsManager m, AlignmentUpdateCall* c, UserPool* p);
      /// Default destructor
      virtual ~AlignmentsForward();
      /// Callback to output conditions information
      virtual int processElement(DetElement de);
      /// Overloadable: call to construct the alignment conditions name.
      /**
       *   Specialize for user defined implementation.
       *
       *   Default implementation returns "de.path()+extension"
       *   Please note, that the corrsponding implementation of
       *   'AlignmentsRegister::construct_name' must match
       */
      virtual std::string construct_name(DetElement de) const;
    };
  }       /* End namespace Alignments              */
}         /* End namespace DD4hep                  */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTFORWARD_H     */
