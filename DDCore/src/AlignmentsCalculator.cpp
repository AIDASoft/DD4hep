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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/AlignmentsCalculator.h"
#include "DD4hep/detail/AlignmentsInterna.h"

using namespace dd4hep;
using namespace dd4hep::align;
typedef AlignmentsCalculator::Result Result;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace align {

    /// Anonymous implementation classes
    namespace {
      static Delta        identity_delta;

      /// Alignment calculator.
      /**
       *  Uses internally the conditions mechanism to calculator the alignment conditions.
       *
       *  \author  M.Frank
       *  \version 2.0
       *  \ingroup DD4HEP_ALIGNMENTS
       */
      class Calculator {
      public:
        class Entry;
        class Context;

      public:
        /// Initializing constructor
        Calculator() = default;
        /// Default destructor
        ~Calculator() = default;
        /// Compute all alignment conditions of the lower levels
        Result compute(Context& context, Entry& entry) const;
        /// Resolve child dependencies for a given context
        void resolve(Context& context, DetElement child) const;
      };

      class Calculator::Entry  {
      public:
        DetElement::Object*         det   = 0;
        const Delta*                delta = 0;
        AlignmentCondition::Object* cond  = 0;
        unsigned char               key   = 0, valid = 0, created = 0, _pad[1];
        Entry(DetElement d, const Delta* del) : det(d.ptr()), delta(del), key(d.key())  {}
      };

      class Calculator::Context  {
      public:
        typedef std::map<DetElement,size_t,AlignmentsCalculator::PathOrdering>  DetectorMap;
        typedef std::map<unsigned int,size_t>             Keys;
        typedef std::vector<Entry>                        Entries;

        DetectorMap    detectors;
        Keys           keys;
        Entries        entries;
        ConditionsMap& mapping;
        Context(ConditionsMap& m) : mapping(m)  {
          InstanceCount::increment(this);
        }
        ~Context()  {
          InstanceCount::decrement(this);
        }
        void insert(DetElement det, const Delta* delta)   {
          if ( det.isValid() )  {
            Entry entry(det,delta);
            detectors.insert(std::make_pair(det, entries.size()));
            keys.insert(std::make_pair(entry.key, entries.size()));
            entries.insert(entries.end(), entry);
            return;
          }
          except("AlignContext","Failed to add entry: invalid detector handle!");
        }
      };
    }
  }       /* End namespace align */
}         /* End namespace dd4hep     */

//static PrintLevel s_PRINT = DEBUG;
//static PrintLevel s_PRINT = INFO;
static PrintLevel s_PRINT = WARNING;

/// Callback to output alignments information
int AlignmentsCalculator::Scanner::operator()(DetElement de, int)  const  {
  if ( de.isValid() )  {
    Condition::key_type key(ConditionKey::KeyMaker(de.key(),align::Keys::deltaKey).hash);
    Condition c = context.condition(key, false);
    if ( c.isValid() )  {
      const Delta& d = c.get<Delta>();
      deltas.insert(std::make_pair(de,&d));
      if ( iov ) iov->iov_intersection(c->iov->key());
    }
    return 1;
  }
  return 0;  
}

/// Compute all alignment conditions of the lower levels
Result Calculator::compute(Context& context, Entry& e)   const  {
  Result result;
  DetElement det = e.det;

  if ( e.valid == 1 )  {
    printout(DEBUG,"ComputeAlignment","================ IGNORE %s (already valid)",det.path().c_str());
    return result;
  }
  AlignmentCondition c = context.mapping.get(det, Keys::alignmentKey);
  AlignmentCondition cond = c.isValid() ? c : AlignmentCondition("alignment");
  AlignmentData&     align = cond.data();
  const Delta*       delta = e.delta ? e.delta : &identity_delta;
  TGeoHMatrix        transform_for_delta;

  printout(DEBUG,"ComputeAlignment",
           "============================== Compute transformation of %s",det.path().c_str());
  e.valid     = 1;
  e.cond      = cond.ptr();
  align.delta = *delta;
  delta->computeMatrix(transform_for_delta);
  result.multiply += 2;

  DetElement parent_det = det.parent();
  AlignmentCondition parent_cond = context.mapping.get(parent_det, Keys::alignmentKey);
  TGeoHMatrix parent_transform;
  if (parent_cond.isValid()) {
    AlignmentData&     parent_align = parent_cond.data();
    parent_transform = parent_align.worldTrafo;
  }
  else if ( det.parent().isValid() )   {
    parent_transform = parent_det.nominal().worldTransformation();
  }
  else {
    // The tranformation from the "world" to its parent is non-existing i.e. unity
  }

  align.detectorTrafo = det.nominal().detectorTransformation() * transform_for_delta;
  align.worldTrafo    = parent_transform * align.detectorTrafo;
  align.trToWorld     = detail::matrix::_transform(&align.worldTrafo);
  ++result.computed;
  result.multiply += 3;
  // Update mapping if the condition is freshly created
  if ( !c.isValid() )  {
    e.created = 1;
    cond->hash = ConditionKey(e.det,Keys::alignmentKey).hash;
    context.mapping.insert(e.det, Keys::alignmentKey, cond);
  }
  if ( s_PRINT <= INFO )  {
    printout(INFO,"ComputeAlignment","Level:%d Path:%s DetKey:%08X: Cond:%s key:%16llX",
             det.level(), det.path().c_str(), det.key(),
             yes_no(e.delta != 0), (long long int)cond.key());
    if ( s_PRINT <= DEBUG )  {
      ::printf("Nominal:     '%s' ", det.path().c_str());
      det.nominal().worldTransformation().Print();
      ::printf("Parent: '%s' -> '%s' ", det.path().c_str(), parent_det.path().c_str());
      parent_transform.Print();
      ::printf("DetectorTrafo: '%s' -> '%s' ", det.path().c_str(), det.parent().path().c_str());
      det.nominal().detectorTransformation().Print();
      ::printf("Delta:       '%s' ", det.path().c_str());
      transform_for_delta.Print();
      ::printf("Result:      '%s' ", det.path().c_str());
      align.worldTrafo.Print();
    }
  }
  return result;
}

/// Resolve child dependencies for a given context
void Calculator::resolve(Context& context, DetElement detector) const   {
  auto children = detector.children();
  auto item = context.detectors.find(detector);
  if ( item == context.detectors.end() ) context.insert(detector,0);
  for(const auto& c : children )
    resolve(context, c.second);
}

/// Optimized call using already properly ordered Deltas
Result AlignmentsCalculator::compute(const OrderedDeltas& deltas,
                                     ConditionsMap& alignments)  const
{
  Result  result;
  Calculator obj;
  Calculator::Context context(alignments);
  for( const auto& i : deltas )
    context.insert(i.first, i.second);
  for( const auto& i : deltas )
    obj.resolve(context,i.first);
  for( auto& i : context.entries )
    result += obj.compute(context, i);
  return result;
}

/// Compute all alignment conditions of the internal dependency list
Result AlignmentsCalculator::compute(const std::map<DetElement, Delta>& deltas,
                                     ConditionsMap& alignments)  const
{
  Result  result;
  Calculator::Context context(alignments);
  // This is a tricky one. We absolutely need the detector elements ordered
  // by their depth aka. the distance to /world.
  // Unfortunately one cannot use the raw pointer of the DetElement here,
  // But has to insert them in a map which is ordered by the DetElement path.
  //
  // Otherwise memory randomization gives us the wrong order and the
  // corrections are calculated in the wrong order ie. not top -> down the
  // hierarchy, but in "some" order depending on the pointer values!
  //
  OrderedDeltas ordered_deltas;
  for( const auto& i : deltas )
    ordered_deltas.insert(std::make_pair(i.first, &i.second));
  return compute(ordered_deltas, alignments);
}

/// Compute all alignment conditions of the internal dependency list
Result AlignmentsCalculator::compute(const std::map<DetElement, const Delta*>& deltas,
                                     ConditionsMap& alignments)  const
{
  Result  result;
  Calculator::Context context(alignments);
  // This is a tricky one. We absolutely need the detector elements ordered
  // by their depth aka. the distance to /world.
  // Unfortunately one cannot use the raw pointer of the DetElement here,
  // But has to insert them in a map which is ordered by the DetElement path.
  //
  // Otherwise memory randomization gives us the wrong order and the
  // corrections are calculated in the wrong order ie. not top -> down the
  // hierarchy, but in "some" order depending on the pointer values!
  //
  OrderedDeltas ordered_deltas;
  for( const auto& i : deltas )
    ordered_deltas.insert(i);
  return compute(ordered_deltas, alignments);
}

/// Helper: Extract all Delta-conditions from the conditions map
size_t AlignmentsCalculator::extract_deltas(cond::ConditionUpdateContext& ctxt,
                                            ExtractContext& extract_context,
                                            OrderedDeltas& deltas,
                                            IOV* effective_iov)   const
{
  return extract_deltas(ctxt.world(), ctxt, extract_context, deltas, effective_iov);
}

/// Helper: Extract all Delta-conditions from the conditions map starting at a certain sub-tree
size_t AlignmentsCalculator::extract_deltas(DetElement start,
                                            cond::ConditionUpdateContext& ctxt,
                                            ExtractContext& extract_context,
                                            OrderedDeltas& deltas,
                                            IOV* effective_iov)   const
{
  if ( !extract_context.empty() )   {
    struct Scanner : public Condition::Processor   {
      OrderedDeltas& delta_conditions;
      ExtractContext& extract_context;
      IOV* effective_iov = 0;
      /// Constructor
      Scanner(OrderedDeltas& d, ExtractContext& e, IOV* eff_iov)
        : delta_conditions(d), extract_context(e), effective_iov(eff_iov) {}
      /// Conditions callback for object processing
      virtual int process(Condition c)  const override  {
        ConditionKey::KeyMaker km(c->hash);
        if ( km.values.item_key == align::Keys::deltaKey )   {
          auto idd = extract_context.find(km.values.det_key);
          if ( idd != extract_context.end() )   {
            const Delta& d  = c.get<Delta>();
            DetElement   de = idd->second;
            delta_conditions.insert(std::make_pair(de,&d));
            if (effective_iov) effective_iov->iov_intersection(c->iov->key());
            return 1;
          }
          // Fatal or not? Depends if the context should be re-usable or the projection
          //except("extract_deltas","++ Inconsistency between extraction context and conditions content!!");
        }
        return 0;
      }
    };
    Scanner scanner(deltas,extract_context,effective_iov);
    ctxt.resolver->conditionsMap().scan(scanner);
    return deltas.size();
  }
  DetectorScanner().scan(AlignmentsCalculator::Scanner(ctxt,deltas,effective_iov),start);
  for( const auto& d : deltas ) extract_context.insert(std::make_pair(d.first.key(), d.first));
  return deltas.size();
}

/// Helper: Extract all Delta-conditions from the conditions map
size_t AlignmentsCalculator::extract_deltas(cond::ConditionUpdateContext& ctxt,
                                            OrderedDeltas& deltas,
                                            IOV* effective_iov)   const
{
  return extract_deltas(ctxt.world(), ctxt, deltas, effective_iov);
}


/// Helper: Extract all Delta-conditions from the conditions map
size_t AlignmentsCalculator::extract_deltas(DetElement start,
                                            cond::ConditionUpdateContext& ctxt,
                                            OrderedDeltas& deltas,
                                            IOV* effective_iov)   const
{
  DetectorScanner().scan(AlignmentsCalculator::Scanner(ctxt,deltas,effective_iov),start);
  return deltas.size();
}

// The map is used by the Alignments calculator
typedef AlignmentsCalculator::OrderedDeltas OrderedMap;
// Have only a weak reference here!
inline std::ostream& operator << (std::ostream& s, const DetElement& )   { return s; }

#include "Parsers/Parsers.h"
DD4HEP_DEFINE_PARSER_DUMMY(OrderedMap)
#include "DD4hep/detail/BasicGrammar_inl.h"
#include "DD4hep/detail/ConditionsInterna.h"
DD4HEP_DEFINE_PARSER_GRAMMAR(OrderedMap,eval_none<OrderedMap>)
DD4HEP_DEFINE_CONDITIONS_TYPE(OrderedMap)
