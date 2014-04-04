// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_ALIGNMENT_ALIGNMENTOPERATORS_H
#define DD4HEP_ALIGNMENT_ALIGNMENTOPERATORS_H

// Framework include files
#include "DD4hep/Alignment.h"
#include "DDAlign/AlignmentCache.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Alignment namespace declaration
   */
  namespace Geometry {

    /// Forward declarations
    class LCDD;


    /** @class AlignmentOperator AlignmentOperators.h DDAlign/AlignmentOperators.h
     *
     *
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct AlignmentOperator  {
      typedef AlignmentStack::StackEntry Entry;
      typedef AlignmentCache::Cache      Cache;
      typedef std::vector<Entry*>        Entries;
      typedef std::map<std::string,std::pair<TGeoPhysicalNode*,Entry*> > Nodes;
      AlignmentCache& cache;
      Nodes& nodes;
    public: 
      AlignmentOperator(AlignmentCache& c, Nodes& n) : cache(c), nodes(n) {}
      void insert(Alignment alignment)  const;
    };

    /** @class AlignmentSelector AlignmentOperators.h DDAlign/AlignmentOperators.h
     *
     *
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct AlignmentSelector : public AlignmentOperator {
    public: 
      const Entries& entries;
      AlignmentSelector(AlignmentCache& c, Nodes& n, const Entries& e) : AlignmentOperator(c,n), entries(e) {}
      ~AlignmentSelector() { }
      const AlignmentSelector& reset()   const { nodes.clear(); return *this; }
      void operator()(const AlignmentCache::Cache::value_type& e) const;
      void operator()(Entry* e) const;
    };

    template <typename T> struct AlignmentActor : public AlignmentOperator {
    public: 
      AlignmentActor(AlignmentCache& c, Nodes& n) : AlignmentOperator(c,n) { init(); }
      void init() {}
      void operator()(Nodes::value_type& e) const;
    };

    namespace DDAlign_standard_operations  {
      struct node_print;
      struct node_reset;
      struct node_align;
      struct node_delete;
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
