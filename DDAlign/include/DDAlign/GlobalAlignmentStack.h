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
#ifndef DDALIGN_GLOBALALIGNMENTSTACK_H
#define DDALIGN_GLOBALALIGNMENTSTACK_H

// Framework include files
#include "DD4hep/Alignments.h"
#include "DD4hep/AlignmentData.h"
//#include "DD4hep/Objects.h"
#include "DD4hep/Memory.h"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace align {

    /// Alignment Stack object definition
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class GlobalAlignmentStack  {
    public:
      enum Flags {
        OVERLAP_DEFINED     = 1<<20,
        MATRIX_DEFINED      = 1<<21,
        CHECKOVL_DEFINED    = 1<<22,
        CHECKOVL_VALUE      = 1<<23,
        RESET_VALUE         = 1<<24,
        RESET_CHILDREN      = 1<<25,
        ____LLLAST          = 1<<31
      };
      //#if 0
      /// Stack entry definition
      /**
       *  \author   M.Frank
       *  \version  1.0
       *  \ingroup  DD4HEP_ALIGN
       */
      class StackEntry {
      public:
        /// Reference to the detector element
        DetElement        detector;
        /// Delta transformation to be applied
        Delta             delta;
        /// Path to the misaligned volume
        std::string       path;
        /// Parameter for overlap checking
        double            overlap = 0.0;

        /// Default constructor
        StackEntry() = delete;
        /// Fully initializing constructor
        StackEntry(DetElement p, const std::string& placement, const Delta& t, double ov);
        /// Copy constructor
        StackEntry(const StackEntry& e);
        /// Default destructor
        virtual ~StackEntry();

        /// Assignment operator
        StackEntry& operator=(const StackEntry& e) = default;
#if 0
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

        /// Set flag to reset the entry to its ideal geometrical position
        StackEntry& setReset(bool new_value=true);
        /// Set flag to reset the entry's children to their ideal geometrical position
        StackEntry& setResetChildren(bool new_value=true);
        /// Set flag to check overlaps
        StackEntry& setOverlapCheck(bool new_value=true);
        /// Set the precision for the overlap check (otherwise the default is 0.001 cm)
        StackEntry& setOverlapPrecision(double precision=0.001);
#endif
      };
      //#endif
      //typedef AlignmentData StackEntry;
      typedef std::map<std::string, StackEntry*> Stack;

    protected:
      /// The subdetector specific map of alignments caches
      Stack m_stack;

      /// Default constructor
      GlobalAlignmentStack();
    public:

      /// Default destructor. Careful with this one:
      virtual ~GlobalAlignmentStack();

      /// Static client accessor
      static GlobalAlignmentStack& get();
      /// Create an alignment stack instance. The creation of a second instance will be refused.
      static void create();
      /// Check existence of alignment stack
      static bool exists();

      /// Check if the overlap flag checking is enabled
      static bool overlapDefined(const StackEntry& e)    {  return e.delta.checkFlag(OVERLAP_DEFINED);  }
      /// Check if the overlap flag checking is enabled
      static bool checkOverlap(const StackEntry& e)      {  return e.delta.checkFlag(CHECKOVL_DEFINED); }
      /// Check if the overalp value is present
      static bool overlapValue(const StackEntry& e)      {  return e.delta.checkFlag(CHECKOVL_VALUE);   }
      /// Check if this alignment entry has a non unitary transformation matrix
      static bool hasMatrix(const StackEntry& e)         {  return e.delta.checkFlag(MATRIX_DEFINED);   }
      /// Check flag if the node location should be reset
      static bool needsReset(const StackEntry& e)        {  return e.delta.checkFlag(RESET_VALUE);      }
      /// Check flag if the node location and all children should be reset
      static bool resetChildren(const StackEntry& e)     {  return e.delta.checkFlag(RESET_CHILDREN);   }

      /// Add a new entry to the cache. The key is the placement path
      bool insert(const std::string& full_path, dd4hep_ptr<StackEntry>& new_entry);
      /// Add a new entry to the cache. The key is the placement path. The placement path must be set in the entry
      bool insert(dd4hep_ptr<StackEntry>& new_entry);
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

  }       /* End namespace detail                        */
}         /* End namespace dd4hep                          */
#endif // DDALIGN_GLOBALALIGNMENTSTACK_H
