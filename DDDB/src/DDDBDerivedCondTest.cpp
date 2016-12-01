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
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/Alignments.h"

#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsAccess.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DDCond/ConditionsOperators.h"

#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;

using DDDB::Document;
using Geometry::LCDD;
using Geometry::DetElement;

/// Anonymous namespace for plugins
namespace  {

  class DataTypes  {
  public:
    typedef AbstractMap                      Map;
    typedef Map::Params                      Params;
    typedef Params::value_type::second_type  Param;
    typedef Alignments::AlignmentData        Data;
    typedef Alignments::Delta                Delta;
    typedef DDDB::ConditionPrinter           _Printer;
  };

  /// Specialized conditions update callback for alignments
  /**
   *  Used by clients to update a condition.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionUpdate1 : public ConditionUpdateCall, public DataTypes  {
  public:
    ConditionUpdate1()          {    }
    virtual ~ConditionUpdate1() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context)  {
      printout(INFO,"ConditionUpdate1","++ Building dependent condition: %s",key.name.c_str());
      Condition    target(key.name,"Alignment");
      Data&        data  = target.bind<Data>();
      Condition    cond0 = context.condition(0);
      const Map&   src0  = cond0.get<Map>();
      const Param& par0  = src0.firstParam().second;
      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta = src0.first<Delta>();
        data.delta         = delta;
        data.flag          = Data::HAVE_NONE;
      }
      else  {
        printout(INFO,"ConditionUpdate1","++ Failed to access Delta from %s",
                 cond0->value.c_str());
        _Printer()(cond0);
      }
      //data.condition   = target;
      return target;
    }
  };
  /// Specialized conditions update callback for alignments
  /**
   *  Used by clients to update a condition.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionUpdate2 : public ConditionUpdateCall, public DataTypes  {
  public:
    ConditionUpdate2()          {    }
    virtual ~ConditionUpdate2() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context)  {
      printout(INFO,"ConditionUpdate2","++ Building dependent condition: %s",key.name.c_str());
      Condition     target(key.name,"Alignment");
      Data&         data  = target.bind<Data>();
      Condition     cond0 = context.condition(0);
      const Map&    src0  = cond0.get<Map>();      
      const Param&  par0  = src0.firstParam().second;

      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta0 = src0.first<Delta>();
        const Data&  delta1 = context.get<Data>(1);
        data.delta          = delta0;
        data.delta          = delta1.delta;
        data.flag           = Data::HAVE_NONE;
      }
      else  {
        printout(INFO,"ConditionUpdate2","++ Failed to access Delta from %s",
                 cond0->value.c_str());
        _Printer()(cond0);
      }
      //data.condition   = target;
      return target;
    }
  };
  /// Specialized conditions update callback for alignments
  /**
   *  Used by clients to update a condition.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionUpdate3 : public ConditionUpdateCall, public DataTypes  {
  public:
    ConditionUpdate3()          {    }
    virtual ~ConditionUpdate3() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context)  {
      printout(INFO,"ConditionUpdate3","++ Building dependent condition: %s",key.name.c_str());
      Condition    target(key.name,"Alignment");
      Data&        data  = target.bind<Data>();
      Condition    cond0 = context.condition(0);
      const Map&   src0  = cond0.get<Map>();
      const Param& par0  = src0.firstParam().second;

      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta0 = src0.first<Delta>();
        const Data&  delta1 = context.get<Data>(1);
        const Data&  delta2 = context.get<Data>(2);
        data.delta          = delta0;
        data.delta          = delta1.delta;
        data.delta          = delta2.delta;
        data.flag           = Data::HAVE_NONE;
      }
      else  {
        printout(INFO,"ConditionUpdate2","++ Failed to access Delta from %s",
                 cond0->value.c_str());
        _Printer()(cond0);
      }
      //data.condition = target;
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
  class ConditionsSelector  {
  public:
    typedef ConditionsDependencyCollection Dependencies;
    string            m_name;
    RangeConditions   m_allConditions;
    Dependencies      m_allDependencies;
    ConditionsManager m_manager;
    struct Counters {
      size_t numAlignments;
      size_t numNoCatalogs;
      void reset() { numAlignments=numNoCatalogs=0; }
    } m_counters;

    /// Initializing constructor
    ConditionsSelector(ConditionsAccess mgr) : m_manager(mgr) {
      Operators::collectAllConditions(mgr, m_allConditions);
    }

    virtual ~ConditionsSelector()   {
      m_allDependencies.clear();
    }

    RangeConditions findCond(const string& match)   {
      RangeConditions result;
      if ( !match.empty() )  {
        for(RangeConditions::iterator ic=m_allConditions.begin(); ic!=m_allConditions.end(); ++ic)  {
          Condition cond = (*ic);
          size_t idx = cond->value.find(match);
          if ( idx == 0 )  {
            if (cond->value.length() == match.length() )   {
              result.push_back(cond);
            }
            else if ( cond->value[match.length()] == '/' )  {
              size_t idq = cond->value.find('/',match.length()+1);
              if ( idq == string::npos )  {
                result.push_back(cond);
              }
            }
          }
        }
      }
      return result;
    }
    long collectDependencies(DetElement de, int level)  {
      char fmt[64], text[256];
      DDDB::Catalog* cat = 0;
      DDDB::ConditionPrinter prt;
      const DetElement::Children& c = de.children();

      ::snprintf(fmt,sizeof(fmt),"%%-%ds-> ",2*level+5);
      ::snprintf(text,sizeof(text),fmt,"");
      prt.setPrefix(text);

      try  {
        ::sprintf(fmt,"%03d %%-%ds Detector: %%s #Dau:%%d VolID:%%p",level+1,2*level+1);
        printout(INFO,m_name,fmt,"",de.path().c_str(),int(c.size()),(void*)de.volumeID());
        cat = de.extension<DDDB::Catalog>();
        ::sprintf(fmt,"%03d %%-%ds %%-20s -> %%s",level+1,2*level+3);
        if ( !cat->condition.empty() )  {
          RangeConditions rc = findCond(cat->condition);
          printout(INFO,m_name,fmt,"","Alignment:    ", 
                   rc.empty() ? (cat->condition+"  !!!UNRESOLVED!!!").c_str() : cat->condition.c_str());
          if ( !rc.empty() )   {
            ConditionKey target1(cat->condition+"/derived_1");
            ConditionKey target2(cat->condition+"/derived_2");
            ConditionKey target3(cat->condition+"/derived_3");
            DependencyBuilder build_1(target1, new ConditionUpdate1());
            DependencyBuilder build_2(target2, new ConditionUpdate2());
            DependencyBuilder build_3(target3, new ConditionUpdate3());

            build_1->detector = de;
            build_2->detector = de;
            build_3->detector = de;
            for(RangeConditions::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)   {
              Condition    cond = *ic;
              ConditionKey key(cond->value);
              build_1.add(key);

              build_2.add(key);
              build_2.add(target1);

              build_3.add(key);
              build_3.add(target1);
              build_3.add(target2);
            }
            m_allDependencies.insert(build_1.release());
            m_allDependencies.insert(build_2.release());
            m_allDependencies.insert(build_3.release());
          }
          ++m_counters.numAlignments;
        }
      }
      catch(...)  {
        ::sprintf(fmt,"%03d %%-%ds %%s%%-20s -> %%s",level+1,2*level+3);
        printout(INFO,m_name, fmt, "", de.path().c_str(), "NO CATALOG availible!", "");
        ++m_counters.numNoCatalogs;
      }
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
        collectDependencies((*i).second,level+1);
      return 1;
    }

    int computeDependencies(long time)  {
      const ConditionsDependencyCollection& deps = m_allDependencies;
      const IOVType* iovType = m_manager.iovType("epoch");
      dd4hep_ptr<ConditionsSlice> slice(createSlice(m_manager,*iovType));
      IOV  iov(iovType, IOV::Key(time,time));
      slice->insert(deps);
      m_manager.prepare(iov, *slice);
      printout(INFO,"Conditions",
               "+++ ConditionsUpdate: Updated %ld conditions... IOV:%s",
               long(slice->pool()->size()), iov.str().c_str());
      slice->pool()->clear();
      return 1;
    }
  };

  //========================================================================
  /// Plugin function
  /// Load, compute and access derived conditions
  long dddb_derived_alignments(LCDD& lcdd, int argc, char** argv) {
    long time = argc>0 ? makeTime(argv[0],"%d-%m-%Y %H:%M:%S") : makeTime(2016,4,1,12);
    ConditionsManager manager = ConditionsManager::from(lcdd);
    ConditionsSelector selector(manager);
    int ret = selector.collectDependencies(lcdd.world(), 0);
    if ( ret == 1 )  {
      ret = selector.computeDependencies(time);
    }
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DerivedCondTest,dddb_derived_alignments)
//==========================================================================
