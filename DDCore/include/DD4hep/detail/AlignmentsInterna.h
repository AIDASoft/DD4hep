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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the Alignments include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DETAIL_ALIGNMENTSINTERNA_H
#define DD4HEP_DETAIL_ALIGNMENTSINTERNA_H

// Framework include files
#include "DD4hep/IOV.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/detail/ConditionsInterna.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Alignment;

  /// Alignments internal namespace declaration
  /** Internaly defined datastructures are not presented to the
   *  user directly, but are used by dedicated views.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_ALIGNMENTS
   */
  namespace detail {

    /// The data class behind an alignments handle.
    /**
     *  See AlignmentsInterna.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class AlignmentObject : public detail::ConditionObject    {
    public:
      /// Cached pointer to the bound conditions data, since these may be accessed very frequently
      AlignmentData* alignment_data = 0;  //! This variable is internally bound and not ROOT persistent!

    public:
      /// Default constructor. Alignment data wuill be bound to the heap.
      AlignmentObject();
      /// Copy constructor
      AlignmentObject(const AlignmentObject& copy) = delete;
      /// Standard constructor with user provided payload pointer to bind alignment data
      AlignmentObject(const std::string& nam, const std::string& tit, void* payload, size_t len);
      /// Standard Destructor
      virtual ~AlignmentObject();
      /// Assignment operator
      AlignmentObject& operator=(const AlignmentObject& copy) = delete;
      /// Clear data
      void clear();
      /// Accessor to the alignment data. Cannot be NULL. Initialized in the constructor(s)
      AlignmentData& values()  {  return *alignment_data;  }
    };
  }       /* End namespace detail                     */
}         /* End namespace dd4hep                      */
#endif // DD4HEP_DETAIL_ALIGNMENTSINTERNA_H
