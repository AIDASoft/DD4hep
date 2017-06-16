//==========================================================================
//  AIDA Detector description implementation 
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
#include "DD4hep/DetElement.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/AlignmentsCalculator.h"

// C/C++ include files
#include <set>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace align {
  
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
      class Entry;
      /// Shortcut definitions
      typedef std::map<Condition::key_type,Entry*>  UsedConditions;

    public:
      /// Reference to the detector description object
      Detector&                      description;
      /// Reference to the alignment manager object
      ConditionsMap& slice;
      /// detaill work stack of cached deltas
      UsedConditions             used;

    protected:      
      /// Implementation: Add a new entry to the transaction stack.
      std::pair<Condition::key_type,Entry*> _set(DetElement det, const Delta& delta);

    public:
      /// No default constructor
      AlignmentsCalib() = delete;
      /// No copy constructor
      AlignmentsCalib(const AlignmentsCalib& copy) = delete;
      /// Initializing constructor
      AlignmentsCalib(Detector& description, ConditionsMap& mapping);
      /// Default destructor
      virtual ~AlignmentsCalib();
      /// No assignment operator
      AlignmentsCalib& operator=(const AlignmentsCalib& copy) = delete;
      /// No move assignment operator
      AlignmentsCalib& operator=(AlignmentsCalib&& copy) = delete;

      /// (1) Add a new entry to an existing DetElement structure.
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
       * Note: has no effect on the real alignment conditions
       *  as long as the callbacks are not executed,
       *  which is triggered by the "commit" call.
       *  The delta is only cached locally.
       *
       *  The resulting alignment key is returned to the client. If NULL: Failure
       */
      Condition::key_type set(DetElement det, const Delta& delta);

      /// (2) Add a new entry to an existing DetElement structure.
      /**
       * Note: has no effect on the real alignment conditions
       *  as long as the callbacks are not executed,
       *  which is triggered by the "commit" call.
       *  The delta is only cached locally.
       *
       *  The alignment key is returned to the client. If NULL: Failure
       */
      Condition::key_type set(const std::string& path, const Delta& delta);

      /// Clear all delta data in the caches transaction stack.
      void clearDeltas();

      /// We clear the entire cached stack of used entries.
      void clear();

      /// Convenience only: Access detector element by path
      DetElement detector(const std::string& path)  const;

      /// Commit all pending transactions. Returns number of altered entries
      AlignmentsCalculator::Result commit();
    };    
  }       /* End namespace align              */
}         /* End namespace dd4hep                  */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTCALIB_H       */
