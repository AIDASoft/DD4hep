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
#ifndef DD4HEP_DDALIGN_ALIGNMENTMANAGER_H
#define DD4HEP_DDALIGN_ALIGNMENTMANAGER_H

// Framework include files
#include "DD4hep/Alignments.h"
#include "DD4hep/ConditionDerived.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {
    class UserPool;
    class ConditionsSlice;
    class ConditionDependency;
    class ConditionUpdateContext;
    class ConditionsDependencyCollection;
  }
  
  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    /// Forward declarations
    class AlignContext;
    class AlignmentsManager;
    class AlignmentsManagerObject;

    /// Alignment manager instance to handle alignment dependencies
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentsManager : public Handle<AlignmentsManagerObject> {
    public:
      /// Standard object type
      typedef AlignmentsManagerObject                    Object;
      /// Conditions user pool definition
      typedef Conditions::UserPool                       Pool;
      /// Conditions slice definitions
      typedef Conditions::ConditionsSlice                Slice;
      /// Conditions derivation context
      typedef Conditions::ConditionUpdateContext         Context;
      /// Alignments re-use conditions dependency definition from the conditions manager
      typedef Conditions::ConditionDependency            Dependency;
      /// Alignments re-use the dependency collections
      typedef Conditions::ConditionsDependencyCollection Dependencies;
      
      /// Result of a computation call to the alignments manager
      /**
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_CONDITIONS
       */
      class Result  {
      public:
        size_t computed = 0;
        size_t missing  = 0;
        Result() = default;
        Result(const Result& result) = default;
        Result& operator=(const Result& result) = default;
        size_t total() const { return computed+missing; }
      };

    public:

      /// Static accessor if installed as an extension
      /**
       * Note:
       *
       * There may very well be several instances of the
       * alignment manager. Do not think this is a sort of
       * 'static constructor'. This would be a big mistake!.
       */
      template <typename T> static AlignmentsManager from(T& host);

      /// Default constructor
      AlignmentsManager() = default;
      /// Copy constructor
      AlignmentsManager(const AlignmentsManager& copy) = default;
      /// Constructor to be used for proper handles
      AlignmentsManager(const Handle<Object>& e) : Handle<Object>(e) {}
      /// Constructor to be used for proper handles
      template <typename Q> AlignmentsManager(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Initializing constructor. Creates the object!
      AlignmentsManager(const std::string& name);
      /// Initializing constructor. Creates the object!
      AlignmentsManager(char const* name);
      /// Assignment operator
      AlignmentsManager& operator=(const AlignmentsManager& mgr) = default;
      /// Delete the manager. Be careful: this affects all referencing handles!
      void destroy();
      /// Compute all alignment conditions of the internal dependency list
      Result compute(Slice& slice)  const;
      /// Compute all alignment conditions of the internal dependency list
      Result compute(Slice& slice, const Dependencies& dependencies)  const;
      /// Compute all alignment conditions of the dependency list
      /** Assume that source and target conditions were updated externally. */
      Result computeDirect(Slice& slice, const Dependencies& dependencies)  const;
      /// Register new updated derived alignment during the computation step
      static void newEntry(const Context& parameter,
                           const Dependency* dep,
                           AlignmentCondition& con);
    };


    /// Alignment manager.
    /**
     *  Uses internally the conditions mechanism to manager the alignment conditions.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class AlignmentsManagerObject : public NamedObject {
    public:
      typedef AlignmentsManager::Pool         Pool;
      typedef AlignmentsManager::Slice        Slice;
      typedef AlignmentsManager::Result       Result;
      typedef AlignmentsManager::Dependencies Dependencies;

    protected:
      /// Compute the transformation from the closest detector element of the alignment to the world system
      Result to_world(AlignContext& new_alignments, Pool& pool, DetElement det, TGeoHMatrix& mat)  const;
      /// Compute all alignment conditions of the lower levels
      Result compute(AlignContext& new_alignments, Pool& pool, DetElement child) const;

    public:
      /// Initializing constructor
      AlignmentsManagerObject();
      /// Default destructor
      virtual ~AlignmentsManagerObject();
      /// Compute all alignment conditions. Dependency list created from slice information
      Result compute(Slice& slice) const;
      /// Compute all alignment conditions of the dependency list
      Result compute(Slice& slice, const Dependencies& dependencies)  const;
      /// Compute all alignment conditions of the dependency list
      /** Assume that source and target conditions were updated externally. */
      Result computeDirect(Slice& slice, const Dependencies& dependencies)  const;
    };
    
  }       /* End namespace Geometry                    */
}         /* End namespace DD4hep                      */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTMANAGER_H         */
