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

// Framework include files
#include "DD4hep/AlignmentsCalculator.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/objects/AlignmentsInterna.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;
typedef AlignmentsCalculator::Result Result;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    /// Anonymous implementation classes
    namespace {
      static unsigned int alignment_hash = 0;
      static Delta        identity_delta;

      /// Alignment calculator.
      /**
       *  Uses internally the conditions mechanism to calculator the alignment conditions.
       *
       *  \author  M.Frank
       *  \version 1.0
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
        /// Compute the alignment delta for one detector element and it's alignment condition
        void computeDelta(const Delta& delta, TGeoHMatrix& tr_delta)  const;
        /// Compute the transformation from the closest detector element of the alignment to the world system
        Result to_world(Context& context, DetElement det, TGeoHMatrix& mat)  const;
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
        struct PathOrdering {
          bool operator()(const DetElement& a, const DetElement& b) const
          { return a.path() < b.path(); }
        };
        typedef Conditions::ConditionsMap                 ConditionsMap;
        typedef std::map<DetElement,size_t,PathOrdering>  DetectorMap;
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
  }       /* End namespace Alignments */
}         /* End namespace DD4hep     */

static PrintLevel s_PRINT = WARNING;
//static PrintLevel s_PRINT = INFO;

Result Calculator::to_world(Context&      context,
                            DetElement    det,
                            TGeoHMatrix&  delta_to_world)  const
{
  Result result;
  DetElement par = det.parent();

  while( par.isValid() )   {
    // Mapping creation mode:
    // If we find that the parent also got updated, directly take this transformation.
    // Since we compute top-down, it is already valid!
    Context::Keys::const_iterator i = context.keys.find(par.key());
    if ( i != context.keys.end() )  {
      Entry& e = context.entries[(*i).second];
      // The parent entry is (not yet) valid. need to compute it first
      if ( 0 == e.valid )  {
        result += compute(context, e);
      }
      AlignmentCondition cond(e.cond);
      AlignmentData&     align(cond.data());
      if ( s_PRINT <= INFO )  {
        ::printf("Multiply-left ALIGNMENT %s:", det.path().c_str()); delta_to_world.Print();
        ::printf("  with ALIGN(world) %s :", par.path().c_str());    align.worldDelta.Print();
      }
      delta_to_world.MultiplyLeft(&align.worldDelta);
      if ( s_PRINT <= INFO )  {
        ::printf("  Result :"); delta_to_world.Print();
      }
      ++result.computed;
      return result;
    }
    // Mapping update mode:
    // Check if there is already a transformation in the cache. If yes, take it.
    // We assume it is a good one, because higher level changes are already processed.
    AlignmentCondition cond = context.mapping.get(par,alignment_hash);
    if ( cond.isValid() )  {
      AlignmentData&     align(cond.data());
      if ( s_PRINT <= INFO )  {
        ::printf("Multiply-left ALIGNMENT %s:", det.path().c_str()); delta_to_world.Print();
        ::printf("  with ALIGN(world) %s :", par.path().c_str());    align.worldDelta.Print();
      }
      delta_to_world.MultiplyLeft(&align.worldDelta);
      if ( s_PRINT <= INFO )  {
        ::printf("  Result :"); delta_to_world.Print();
      }
      ++result.computed;
      return result;
    }
    // There is no special alignment for this detector element.
    // Hence to nominal (relative) transformation to the parent is valid
    if ( s_PRINT <= INFO )  {
      ::printf("Multiply-left ALIGNMENT %s:", det.path().c_str()); delta_to_world.Print();
      ::printf("  with NOMINAL(det) %s :",    par.path().c_str());
      par.nominal().detectorTransformation().Print();
    }
    delta_to_world.MultiplyLeft(&par.nominal().detectorTransformation());
    if ( s_PRINT <= INFO )  {
      ::printf("  Result :"); delta_to_world.Print();
    }
    par = par.parent();
  }
  ++result.computed;
  return result;
}

/// Compute the alignment delta for one detector element and it's alignment condition
void Calculator::computeDelta(const Delta& delta,
                              TGeoHMatrix& tr_delta)  const
{
  const Position&        pos = delta.translation;
  const Translation3D&   piv = delta.pivot;
  const RotationZYX&     rot = delta.rotation;

  switch(delta.flags)   {
  case Delta::HAVE_TRANSLATION+Delta::HAVE_ROTATION+Delta::HAVE_PIVOT:
    Geometry::_transform(tr_delta, Transform3D(Translation3D(pos)*piv*rot*(piv.Inverse())));
    break;
  case Delta::HAVE_TRANSLATION+Delta::HAVE_ROTATION:
    Geometry::_transform(tr_delta, Transform3D(rot,pos));
    break;
  case Delta::HAVE_ROTATION+Delta::HAVE_PIVOT:
    Geometry::_transform(tr_delta, Transform3D(piv*rot*(piv.Inverse())));
    break;
  case Delta::HAVE_ROTATION:
    Geometry::_transform(tr_delta, rot);
    break;
  case Delta::HAVE_TRANSLATION:
    Geometry::_transform(tr_delta, pos);
    break;
  default:
    break;
  }
}

/// Compute all alignment conditions of the lower levels
Result Calculator::compute(Context& context, Entry& e)   const  {
  Result result;
  DetElement det = e.det;

  if ( e.valid == 1 )  {
    printout(DEBUG,"ComputeAlignment","================ IGNORE %s (already valid)",det.path().c_str());
    return result;
  }
  AlignmentCondition c = context.mapping.get(e.det, alignment_hash);
  AlignmentCondition cond = c.isValid() ? c : AlignmentCondition("alignment");
  AlignmentData&     align = cond.data();
  const Delta*       delta = e.delta ? e.delta : &identity_delta;
  TGeoHMatrix        tr_delta;

  printout(DEBUG,"ComputeAlignment",
           "============================== Compute transformation of %s",det.path().c_str());
  e.valid = 1;
  e.cond  = cond.ptr();
  computeDelta(*delta, tr_delta);
  align.delta         = *delta;
  align.worldDelta    = tr_delta;
  result += to_world(context, det, align.worldDelta);
  align.worldTrafo    = det.nominal().worldTransformation()*align.worldDelta;
  align.detectorTrafo = det.nominal().detectorTransformation()*tr_delta;
  align.trToWorld     = Geometry::_transform(&align.worldDelta);
  // Update mapping if the condition is freshly created
  if ( !c.isValid() )  {
    e.created = 1;
    context.mapping.insert(e.det, alignment_hash, cond);
  }
  if ( s_PRINT <= INFO )  {  
    printout(INFO,"ComputeAlignment","Level:%d Path:%s DetKey:%08X: Cond:%s key:%16llX IOV:%s",
             det.level(), det.path().c_str(), det.key(),
             yes_no(e.delta != 0), (long long int)cond.key(), cond.iov().str().c_str());
    if ( s_PRINT <= DEBUG )  {  
      ::printf("DetectorTrafo: '%s' -> '%s' ",det.path().c_str(), det.parent().path().c_str());
      det.nominal().detectorTransformation().Print();
      ::printf("Delta:       '%s' ",det.path().c_str()); tr_delta.Print();
      ::printf("World-Delta: '%s' ",det.path().c_str()); align.worldDelta.Print();
      ::printf("Nominal:     '%s' ",det.path().c_str()); det.nominal().worldTransformation().Print();
      ::printf("Result:      '%s' ",det.path().c_str()); align.worldTrafo.Print();
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

/// Access the default alignment name
unsigned int AlignmentsCalculator::alignment_item_key()   {
  if ( 0 == alignment_hash )  {
    Conditions::ConditionKey k(0,"alignment");
    alignment_hash = k.item_key();
  }
  return alignment_hash;
}

/// Compute all alignment conditions of the internal dependency list
Result AlignmentsCalculator::compute(const Deltas& deltas, Alignments& alignments)  const  {
  Result  result;
  Calculator obj;
  Calculator::Context context(alignments);
  if ( 0 == alignment_hash )  {
    Conditions::ConditionKey k(0,"alignment");
    alignment_hash = k.item_key();
  }
  for( const auto& i : deltas )
    context.insert(i.first, &(i.second));
  for( const auto& i : deltas )
    obj.resolve(context,i.first);
  for( auto& i : context.entries )
    result += obj.compute(context, i);
  for( auto& i : context.entries )   {
    if ( i.created )   {
      Conditions::Condition c(i.cond);
      Conditions::ConditionKey::KeyMaker m(c.key());
      alignments.insert(DetElement(i.det), m.values.item_key, c);
    }
  }
  return result;
}
