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
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTOPERATORS_H
#define DD4HEP_ALIGNMENT_ALIGNMENTOPERATORS_H

// Framework include files
#include "DD4hep/Alignment.h"
#include "DDAlign/AlignmentCache.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Forward declarations
    class LCDD;


    /// Base class for alignment functors
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentOperator  {
    public:
      typedef AlignmentStack::StackEntry Entry;
      typedef AlignmentCache::Cache      Cache;
      typedef std::vector<Entry*>        Entries;
      typedef std::map<std::string,std::pair<TGeoPhysicalNode*,Entry*> > Nodes;
      AlignmentCache& cache;
      Nodes& nodes;
    public:
      /// Initializing functor constructor
      AlignmentOperator(AlignmentCache& c, Nodes& n) : cache(c), nodes(n) {}
      /// Insert alignment entry
      void insert(Alignment alignment)  const;
    };

    /// Select alignment operations according to certain criteria
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class AlignmentSelector : public AlignmentOperator {
    public:
      const Entries& entries;
      /// Initializing functor constructor
      AlignmentSelector(AlignmentCache& c, Nodes& n, const Entries& e) : AlignmentOperator(c,n), entries(e) {}
      ~AlignmentSelector() { }
      const AlignmentSelector& reset()   const { nodes.clear(); return *this; }
      /// Function callback for cache entries
      void operator()(const AlignmentCache::Cache::value_type& e) const;
      /// Function callback for entries
      void operator()(Entry* e) const;
    };

    /// Act on selected alignment entries
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    template <typename T> class AlignmentActor : public AlignmentOperator {
    public:
      /// Initializing functor constructor
      AlignmentActor(AlignmentCache& c, Nodes& n) : AlignmentOperator(c,n) { init(); }
      void init() {}
      /// Function callback for entries
      void operator()(Nodes::value_type& e) const;
    };

    /// Helper namespace to specialize functionality  \ingroup  DD4HEP_ALIGN
    namespace DDAlign_standard_operations  {
      class node_print;
      class node_reset;
      class node_align;
      class node_delete;
    }

    // Specializations
    template <> void AlignmentActor<DDAlign_standard_operations::node_print>::init();
    template <> void AlignmentActor<DDAlign_standard_operations::node_print>::operator() (Nodes::value_type& n)  const;
    template <> void AlignmentActor<DDAlign_standard_operations::node_delete>::operator()(Nodes::value_type& n)  const;
    template <> void AlignmentActor<DDAlign_standard_operations::node_reset>::operator() (Nodes::value_type& n)  const;
    template <> void AlignmentActor<DDAlign_standard_operations::node_align>::operator() (Nodes::value_type& n)  const;

  } /* End namespace Geometry        */
} /* End namespace DD4hep            */
#endif    /* DD4HEP_ALIGNMENT_ALIGNMENTOPERATORS_H       */
