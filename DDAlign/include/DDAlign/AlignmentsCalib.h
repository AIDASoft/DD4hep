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
#ifndef DD4HEP_DDALIGN_ALIGNMENTCALIB_H
#define DD4HEP_DDALIGN_ALIGNMENTCALIB_H

// Framework includes
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"

#include "DDCond/ConditionsSlice.h"
#include "DDAlign/AlignmentsManager.h"

// C/C++ include files
#include <set>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {
    // Forward declarations
    class ConditionsSlice;
  }
  
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {
  
    // Forward declarations
    class AlignmentsUpdateCall;
    
    /// Calib alignment dependencies from conditions
    /**
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/01/2017
     *   \ingroup DD4HEP_DDALIGN
     */
    class AlignmentsCalib {
    public:
      /// Shortcut the ConditionsSlice type
      typedef Conditions::Condition            Condition;
      typedef Conditions::ConditionKey         ConditionKey;
      typedef Conditions::ConditionsSlice      Slice;
      typedef Conditions::ConditionDependency  Dependency;
      typedef Conditions::ConditionsDescriptor Descriptor;
      typedef Condition::key_type              key_type;
      typedef Slice::ConditionsProxy           Proxy;
      typedef Geometry::DetElement             DetElement;
      
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/01/2017
       *   \ingroup DD4HEP_DDALIGN
       */
      struct Entry   {
        Delta       delta;
        Descriptor* source = 0;
        Descriptor* target = 0;
        int         dirty  = 0;
        Entry() = default;
        Entry(const Entry& c) = default;
        Entry(Descriptor* s, Descriptor* d) : delta(), source(s), target(d), dirty(0)  {}
        Entry(Descriptor* s, Descriptor* d, const Delta& del) : delta(del), source(s), target(d), dirty(0)  {}
        Entry& operator=(const Entry& c) = default;
      };
      typedef std::map<key_type,Entry*>    UsedConditions;

    public:
      LCDD&                  lcdd;
      /// Reference to the alignment manager object
      Slice&                 slice;
      UsedConditions         used;
      AlignmentsUpdateCall*  derivationCall  = 0;
      AlignmentsUpdateCall*  propagationCall = 0;
      
    protected:
      /// Propagate all Delta parameters to the source conditions
      bool propagateDeltas();
      /// Update Dependencies between the source conditions and the computations
      bool updateDependencies();
      /// Compute all dependent conditions from the Delta parameters
      AlignmentsManager::Result computeDependencies();

      /// Implementation: Add a new entry to the transaction list
      key_type  _insert(const std::pair<Condition::key_type,Entry*>& e);
      /// Implementation: Register newly created condition to user-pool, slice and manager
      Condition _register(Condition cond)  const;
      /// Implementation: Add a new raw(delta)-condition to the transaction stack.
      Condition _create_source(key_type key, const std::string& nam)  const;
      /// Implementation: Add a new alignment-condition to the transaction stack.
      Condition _create_target(DetElement detector, key_type key, const std::string& nam)  const;

      /// Implementation: Add a new entry to the transaction stack.
      key_type _use(AlignmentCondition alignment);

    public:

      /// No default constructor
      AlignmentsCalib() = delete;

      /// No copy constructor
      AlignmentsCalib(const AlignmentsCalib& copy) = delete;

      /// Initializing constructor
      AlignmentsCalib(LCDD& lcdd, Slice& slice);

      /// Default destructor
      virtual ~AlignmentsCalib();

      /// No assignment operator
      AlignmentsCalib& operator=(const AlignmentsCalib& copy) = delete;

      /// (1) Add a new entry to the transaction stack.
      /** 
       *  Note: 
       *  The alignment must be a valid object bound to an alignment condition.
       *  Otherwise the call shal throw an exception.
       *
       *  Note:
       *  Such alignments are only present if they are created through the usual
       *  loading mechanism.
       *
       *  The resulting alignment key is returned to the client. If NULL: Failure
       */
      key_type use(Alignment alignment);

      /// (2) Add a new entry to an existing DetElement structure.
      /**
       *  This call does several actions:
       *
       *  A check is performed if the alignment with this identifier
       *  already exists: if YES, continue with use(Alignment alignment).
       *  If NO, then:
       *  1) a Condition object is added using the given 'name'.
       *     This condition is added to the ConditionsManager's repository 
       *     and the ConditionsSlice. 
       *     'name' MUST denote a unique conditions identifier.
       *  1.1) A conditions key with the id 'name' is added to the DetElement's 
       *       conditions container.
       *       
       *  2) an AlignmentCondition object is added to the
       *     ConditionsManager's repository and the used ConditionsSlice.
       *     Hereby as a name 'align_name' is used.
       *     If empty, the default results to: align_name = name+"/Transformations".
       *
       *     Then for align_name:
       *  2.1) A conditions key with the id 'align_name' is added to the DetElement's 
       *       conditions container.
       *  2.2) A conditions key-alias named "Alignment" is added to the 
       *       DetElement's conditions container.
       *  2.3) An alignment key with the id 'align_name' is added to the DetElement's 
       *       alignments container.
       *  2.4) A alignment key-alias named "Alignment" is added to the 
       *       DetElement's alignments container.
       *  2.5) A callback dependency is added to the ConditionsSlice's dependency list.
       *       The callback dependency uses the call 'callback'.
       *       If the callback is not registered, an exception is thrown.
       *
       *  The resulting alignment key is returned to the client. If NULL: Failure
       */
      key_type use(DetElement detector, const std::string& name, const std::string& align_name="");

      /// (3) Add a new entry to an existing DetElement structure.
      /**
       *  Shortcut call equivalent to:
       *  key_type use(detector, detector.path()+"#alignment","")
       *
       *  The alignment key is returned to the client. If NULL: Failure
       */
      key_type use(DetElement detector);

      /// (4) Add a new entry to an existing DetElement structure.
      /**
       *  The alignment key is returned to the client. If NULL: Failure
       */
      key_type use(const std::string& detector, const std::string& name, const std::string& align_name="");

      /// (5) Add a new entry to an existing DetElement structure.
      /**
       *  The alignment key is returned to the client. If NULL: Failure
       */
      key_type use(const std::string& detector);
      
      /// Set a new delta value in the transaction stack.
      /** Note: has no effect on the real alignment conditions
       *  as long as the callbacks are not executed,
       *  which is triggered by the "commit" call.
       *  The delta is only cached locally.
       */
      bool setDelta(Alignment::key_type key_val, const Delta& delta);

      /// Set a new delta value identified by the conditions-key
      bool setDelta(const ConditionKey& key, const Delta& delta);

      /// Clear all delta data in the caches transaction stack.
      void clearDeltas();

      /// We clear the entire cached stack of used entries.
      void clear();

      /// Commit all pending transactions. Returns number of altered entries
      AlignmentsManager::Result commit();
    };
    
  }       /* End namespace Alignments              */
}         /* End namespace DD4hep                  */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTCALIB_H       */
