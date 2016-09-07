// $Id$
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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/LCDD.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DDCond/ConditionsPool.h"
#include "DDAlign/AlignmentsManager.h"

#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBAlignmentUpdateCall.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Alignments::AlignmentsManager;

/// Anonymous namespace for plugins
namespace  {

  /// DDDB Conditions analyser to select alignments.
  /**
   *   \author  M.Frank
   *   \version 1.0
   *   \date    31/03/2016
   *   \ingroup DD4HEP_DDDB
   */
  class AlignmentSelector  {
  public:
    ConditionUpdateCall* updateCall;
    LCDD&                lcdd;
    string               m_name;

    /// Initializing constructor
    AlignmentSelector(LCDD& l) : lcdd(l), m_name("Selector")   {
      // The alignment update call can be re-used over and over. It has not state.
      updateCall = new DDDB::DDDBAlignmentUpdateCall();
    }
    /// Default destructor
    virtual ~AlignmentSelector()   {
      releasePtr(updateCall);
    }
    /// Recursive alignment collector
    long collect(DetElement de, dd4hep_ptr<UserPool>& user_pool, AlignmentsManager& am, int level)
    {
      char fmt[64];
      try  {
        DDDB::Catalog* cat = de.extension<DDDB::Catalog>();
        if ( !cat->condition.empty() )  {
          ConditionKey key(cat->condition);
          Condition cond = user_pool->get(key.hash);
          if ( cond.isValid() )   {
            ConditionKey k(cat->condition+"/Tranformations");
            //
            // The alignment access through the DetElement object is optional!
            // It is slow and deprecated. The access using the UserPool directly
            // is highly favored.
            //
            Alignments::DetAlign align(de);
            align.alignments()->addKey(k.name);
            align.alignments()->addKey("Alignment",k.name);
            //
            // Now add the dependency to the alignmant manager
            DependencyBuilder b(k, updateCall->addRef());
            b->detector = de;
            b.add(ConditionKey(cond->value));
            am.adoptDependency(b.release());
          }
          else  {
            ::sprintf(fmt,"%03d %%-%ds %%-20s -> %%s",level+1,2*level+3);
            printout(INFO,m_name,fmt,"","Alignment:    ", 
                     !cond.isValid() ? (cat->condition+"  !!!UNRESOLVED!!!").c_str() : cat->condition.c_str());
          }
        }
      }
      catch(...)  {
        ::sprintf(fmt,"%03d %%-%ds %%s%%-20s -> %%s",level+1,2*level+3);
        printout(INFO,m_name, fmt, "", de.path().c_str(), "NO CATALOG availible!", "");
      }
      const DetElement::Children& c = de.children();
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
        collect((*i).second, user_pool, am, level+1);
      return 1;
    }
    /// Initial collector call
    long collect(ConditionsManager manager, AlignmentsManager& context, long time)  {
      const IOVType* iovType = manager.iovType("epoch");
      IOV  iov(iovType, IOV::Key(time,time));
      dd4hep_ptr<UserPool> user_pool;
      manager.prepare(iov, user_pool);
      return collect(lcdd.world(), user_pool, context, 0);
    }
    /// Compute dependent alignment conditions
    int computeDependencies(dd4hep_ptr<UserPool>& user_pool,
                            ConditionsManager conds,
                            AlignmentsManager align,
                            long time)
    {
      const IOVType* iovType = conds.iovType("epoch");
      IOV  iov(iovType, IOV::Key(time,time));
      long num_expired = conds.prepare(iov, user_pool);
      printout(INFO,"Conditions",
               "+++ ConditionsUpdate: Updated %ld conditions... IOV:%s",
               num_expired, iov.str().c_str());
      align.compute(user_pool);
      return 1;
    }
    /// Access dependent alignment conditions from DetElement object using global and local keys
    int access(ConditionsManager conds,AlignmentsManager align,long time)  {
      typedef ConditionsManager::Dependencies Deps;
      dd4hep_ptr<UserPool> pool;
      int   ret  = computeDependencies(pool, conds, align, time);
      if ( ret == 1 )  {
        const Deps& deps = align.knownDependencies();
        int count = 0;
        for(Deps::const_iterator i=deps.begin(); i!=deps.end(); ++i)   {
          const ConditionDependency* d = (*i).second.get();
          if ( d->detector.hasAlignments() )   {
            UserPool& p = *(pool.get());
            Alignments::DetAlign     det(d->detector);
            const ConditionKey&      k = d->target;
            Alignments::Container    c = det.alignments();
            {
              Alignments::Alignment    a = c.get(k.hash,p);
              const Alignments::Delta& D = a.data().delta;
              printout(INFO,"Alignment","++ [%08X] (%11s-%8s-%5s) Cond:%p '%s'", k.hash,
                       D.hasTranslation() ? "Translation" : "",
                       D.hasRotation() ? "Rotation" : "",
                       D.hasPivot() ? "Pivot" : "",
                       a.data().hasCondition() ? a.data().condition.ptr() : 0,
                       k.name.c_str());
              ++count;
            }
            {
              Alignments::Alignment    a = c.get("Alignment",p);
              const Alignments::Delta& D = a.data().delta;
              printout(INFO,"Alignment","++ [%08X] (%11s-%8s-%5s) Cond:%p 'Alignment'", k.hash,
                       D.hasTranslation() ? "Translation" : "",
                       D.hasRotation() ? "Rotation" : "",
                       D.hasPivot() ? "Pivot" : "",
                       a.data().hasCondition() ? a.data().condition.ptr() : 0);
              ++count;
            }
          }
        }
        printout(INFO,m_name,"++ Accessed %d conditions from the DetElement objects.",count);
      }
      return ret;
    }
  };
  //========================================================================
  long make_time(int argc, char** argv) {
    long int time = makeTime(2016,4,1,12);
    if ( argc>0 )  {
      struct tm tm;
      char* c = ::strptime(argv[0],"%d-%m-%Y %H:%M:%S",&tm);
      if ( 0 == c )   {
        except("DerivedAlignmentsTest","Invalid time format given for update:%s "
               " should be: %d-%m-%Y %H:%M:%S", argv[0]);
      }
      time = ::mktime(&tm);
    }
    return time;
  }
}

//==========================================================================
namespace  {
  /// Plugin function
  /// Load dependent alignment conditions according to time stamps.
  long dddb_derived_alignments(LCDD& lcdd, int argc, char** argv) {
    long time = make_time(argc, argv);
    AlignmentsManager align("Test");
    AlignmentSelector selector(lcdd);
    ConditionsManager conds(ConditionsManager::from(lcdd));
    int ret = selector.collect(conds,align,time);
    if ( ret == 1 )  {
      for(int i=0; i<10; ++i)  {  {
          long ti = time + i*3600;
          dd4hep_ptr<UserPool> pool;
          ret = selector.computeDependencies(pool,conds,align,ti);
          pool->clear();
        }
        DD4hep::InstanceCount::dump();
      }
    }
    align.destroy();
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DerivedAlignmentsTest,dddb_derived_alignments)
//==========================================================================

namespace  {
  /// Plugin function
  /// Access dependent alignment conditions from DetElement object using global and local keys
  long dddb_access_alignments(LCDD& lcdd, int argc, char** argv) {
    long time = make_time(argc, argv);
    AlignmentsManager align("Test");
    AlignmentSelector selector(lcdd);
    ConditionsManager conds(ConditionsManager::from(lcdd));
    int ret = selector.collect(conds,align,time);
    if ( ret == 1 )  {
      ret = selector.access(conds,align,time);
    }
    align.destroy();
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_AlignmentsAccessTest,dddb_access_alignments)
