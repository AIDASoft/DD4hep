// $Id: $
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
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTSTACK_H
#define DD4HEP_ALIGNMENT_ALIGNMENTSTACK_H

// Framework include files
#include "DD4hep/Alignments.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Memory.h"


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {

    /// Alignment Stack object definition
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentStack  {
    public:
      enum {
        OVERLAP_DEFINED     = 1<<20,
        MATRIX_DEFINED      = 1<<21,
        CHECKOVL_DEFINED    = 1<<22,
        CHECKOVL_VALUE      = 1<<23,
        RESET_VALUE         = 1<<24,
        RESET_CHILDREN      = 1<<25,
        ____LLLAST          = 1<<31
      } Flags;

      /// Stack entry definition
      /**
       *  \author   M.Frank
       *  \version  1.0
       *  \ingroup  DD4HEP_ALIGN
       */
      struct StackEntry {
        /// Reference to the detector element
        DetElement        detector;
        /// Delta transformation to be applied
        Delta             delta;
        /// Path to the misaligned volume
        std::string       path;
        /// Parameter for overlap checking
        double            overlap;

        /// Fully initializing constructor
        StackEntry(DetElement p, const std::string& placement, const Delta& t, double ov);
        /// Copy constructor
        StackEntry(const StackEntry& e);
        /// Default destructor
        virtual ~StackEntry();

        /// Assignment operator
        StackEntry& operator=(const StackEntry& e);

        /// Check if the overlap flag checking is enabled
        bool overlapDefined() const    {  return delta.checkFlag(OVERLAP_DEFINED);  }
        /// Check if the overlap flag checking is enabled
        bool checkOverlap() const      {  return delta.checkFlag(CHECKOVL_DEFINED); }
        /// Check if the overalp value is present
        bool overlapValue() const      {  return delta.checkFlag(CHECKOVL_VALUE);   }
        /// Check if this alignment entry has a non unitary transformation matrix
        bool hasMatrix() const         {  return delta.checkFlag(MATRIX_DEFINED);   }
        /// Check flag if the node location should be reset
        bool needsReset() const        {  return delta.checkFlag(RESET_VALUE);      }
        /// Check flag if the node location and all children should be reset
        bool resetChildren() const     {  return delta.checkFlag(RESET_CHILDREN);   }

        /// Set flag to reset the entry to it's ideal geometrical position
        StackEntry& setReset(bool new_value=true);
        /// Set flag to reset the entry's children to their ideal geometrical position
        StackEntry& setResetChildren(bool new_value=true);
        /// Set flag to check overlaps
        StackEntry& setOverlapCheck(bool new_value=true);
        /// Set the precision for the overlap check (otherwise the default is 0.001 cm)
        StackEntry& setOverlapPrecision(double precision=0.001);
      };
      typedef std::map<std::string, StackEntry*> Stack;

    protected:
      /// The subdetector specific map of alignments caches
      Stack m_stack;

      /// Default constructor
      AlignmentStack();
    public:

      /// Default destructor. Careful with this one:
      virtual ~AlignmentStack();

      /// Static client accessor
      static AlignmentStack& get();
      /// Create an alignment stack instance. The creation of a second instance will be refused.
      static void create();
      /// Check existence of alignment stack
      static bool exists();
      /// Add a new entry to the cache. The key is the placement path
      static bool insert(const std::string& full_path, dd4hep_ptr<StackEntry>& new_entry);
      /// Add a new entry to the cache. The key is the placement path. The placement path must be set in the entry
      static bool insert(dd4hep_ptr<StackEntry>& new_entry);
      /// Clear data content and remove the slignment stack
      void release();
      /// Access size of the alignment stack
      size_t size() const  {  return m_stack.size(); }
      /// Add a new entry to the cache. The key is the placement path
      bool add(dd4hep_ptr<StackEntry>& new_entry);
      /// Retrieve an alignment entry of the current stack
      dd4hep_ptr<StackEntry> pop();
      /// Get all path entries to be aligned. Note: transient!
      std::vector<const StackEntry*> entries() const;
    };

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_ALIGNMENT_ALIGNMENTSTACK_H       */
