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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/InstanceCount.h"
#include "DDAlign/AlignmentsManager.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsPool.h"

#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;

using DDDB::Document;
using Geometry::LCDD;
using Geometry::DetElement;
using Geometry::Position;
using Geometry::RotationZYX;
using Geometry::Transform3D;
using Geometry::Translation3D;

/// Anonymous namespace for plugins
namespace  {

  using Alignments::AlignmentsManager;

  /// Specialized conditions update callback for alignments
  /**
   *  Used by clients to update a condition.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class AlignmentUpdate : public ConditionUpdateCall  {
    typedef AbstractMap::Params::value_type::second_type Param;
    typedef Alignments::AlignmentData Data;
  public:
    AlignmentUpdate()  {    }
    virtual ~AlignmentUpdate() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context)  {
      Alignments::AlignmentCondition target(key.name);
      Data&        data = target.data();
      Condition    cond = context.condition(0);
      AbstractMap& src  = cond.get<AbstractMap>();
      const Param& par  = src.firstParam().second;
      DetElement   det  = context.dependency.detector;
      printout(DEBUG,"AlignmentUpdate","++ Building dependent condition: %s Detector [%d]: %s ",
               key.name.c_str(), det.level(), det.path().c_str());
      if ( par.typeInfo() == typeid(Data::Delta) )  {
        const Data::Delta& delta = src.first<Data::Delta>();
        data.delta = delta;
        data.flag  = Data::HAVE_NONE;
      }
      else  {
        printout(INFO,"AlignmentUpdate","++ Failed to access alignment-Delta from %s",
                 cond->value.c_str());
        DDDB::ConditionPrinter()(cond);
      }
      data.detector = det;
      AlignmentsManager::newEntry(context, det, &context.dependency, target);
      // A callback returning no condition is illegal!
      // Need to crosscheck though if the alignment IOV interval of parents may be
      // smaller then the daughter IOV interval.
      // I though expect, that this is a purely academic case.
      return target;
    }
  };

  /// DDDB Conditions analyser to select alignments.
  /**
   *   \author  M.Frank
   *   \version 1.0
   *   \date    31/03/2016
   *   \ingroup DD4HEP_DDDB
   */
  class AlignmentSelector  {
  public:
    typedef ConditionsManager::Dependencies Dependencies;
    LCDD&             lcdd;
    AlignmentUpdate*  updateCall;
    long int          time;
    string            m_name;

    /// Initializing constructor
    AlignmentSelector(LCDD& l, long t) : lcdd(l), time(t), m_name("Selector")   {
      // The alignment update call can be re-used over and over. It has not state.
      updateCall = new AlignmentUpdate();
    }
    /// Default destructor
    virtual ~AlignmentSelector()   {
      releasePtr(updateCall);
    }
    /// Recursive alignment collector
    long collect(DetElement de, dd4hep_ptr<UserPool>& user_pool, AlignmentsManager& am, int level)  {
      char fmt[64];
      try  {
        DDDB::Catalog* cat = de.extension<DDDB::Catalog>();
        if ( !cat->condition.empty() )  {
          ConditionKey key(cat->condition);
          Condition cond = user_pool->get(key.hash);
          if ( cond.isValid() )   {
            DependencyBuilder b(ConditionKey(cat->condition+"/Tranformations"), updateCall->addRef());
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
    long collect(ConditionsManager manager, AlignmentsManager& context)  {
      const IOVType* iovType = manager.iovType("epoch");
      IOV  iov(iovType, IOV::Key(time,time));
      dd4hep_ptr<UserPool> user_pool;
      manager.prepare(iov, user_pool);
      return collect(lcdd.world(), user_pool, context, 0);
    }
    
    int computeDependencies(ConditionsManager manager, AlignmentsManager& am)  {
      const IOVType* iovType = manager.iovType("epoch");
      for(int i=0; i<10; ++i)  {   {
          int t = time + i*3600;
          IOV  iov(iovType, IOV::Key(t,t));
          dd4hep_ptr<UserPool> user_pool;
          long num_expired = manager.prepare(iov, user_pool);
          printout(INFO,"Conditions",
                   "+++ ConditionsUpdate: Updated %ld conditions... IOV:%s",
                   num_expired, iov.str().c_str());
          am.compute(user_pool);
          user_pool->clear();
        }
        DD4hep::InstanceCount::dump();
      }
      return 1;
    }
  };

  /// Plugin function
  long dddb_derived_alignments(LCDD& lcdd, int argc, char** argv) {
    long int long init_time = DDDB::DDDBReader::makeTime(2016,4,1,12);
    if ( argc>0 )  {
      struct tm tm;
      char* c = ::strptime(argv[0],"%d-%m-%Y %H:%M:%S",&tm);
      if ( 0 == c )   {
        except("DerivedAlignmentsTest","Invalid time format given for update:%s "
               " should be: %d-%m-%Y %H:%M:%S", argv[0]);
      }
    }
    AlignmentSelector selector(lcdd, init_time);
    AlignmentsManager alig_manager("Test");
    ConditionsManager cond_manager(ConditionsManager::from(lcdd));
    int ret = 0;
    try {
      ret = selector.collect(cond_manager,alig_manager);
      if ( ret == 1 )  {
        ret = selector.computeDependencies(cond_manager,alig_manager);
      }
    }
    catch(...)   {
    }
    alig_manager.destroy();
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DerivedAlignmentsTest,dddb_derived_alignments)
//==========================================================================
