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
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/Alignments.h"

#include "DDCond/ConditionsAccess.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsInterna.h"
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

  class AlignmentTypes  {
  public:
    typedef AbstractMap                      Map;
    typedef Map::Params                      Params;
    typedef Params::value_type::second_type  Param;
    typedef Alignments::AlignmentData        Data;
    typedef Alignments::Delta                Delta;
    typedef DDDB::ConditionPrinter           _Printer;
  };

  struct UserData;
  struct Entry {
    UserData*            data;
    DetElement::Object*  det;
    DetElement::Object*  par;
    ConditionDependency* dep;
    int det_key, par_key, top;
  };
  struct UserData {
    static bool det_lexical_ordering(DetElement a, DetElement b)
    {    return std::less(a.path(), b.path());                }
    typedef std::map<DetElement,size_t,det_lexical_ordering>       DetectorMap;
    typedef std::map<unsigned int,size_t>      DetectorKeys;
    typedef std::vector<Entry>                 Entries;
    DetectorMap  detectors;
    DetectorKeys keys;
    Entries      entries;
  };
  struct _Oper : public ConditionsDependencyCollection::Functor {
    void operator()(UserData& data, const Dependencies::value_type& e) const  {
      const Dependency* dep = e.second.get();
      DetElement det = dep->detector;
      if ( det.isValid() )  {
        Entry entry;
        unsigned int key = det.key();
        entry.data = &data;
        entry.top  = 0;
        entry.dep  = e.second.get();
        entry.det  = det.ptr();
        entry.par  = det.parent().ptr();
        entry.det_key = key;
        entry.par_key = det.parent().key();
        data.detectors.insert(make_pair(det,data.entries.size()));
        data.keys.insert(make_pair(key,data.entries.size()));
        data.entries.insert(data.entries.end(),entry);
      }
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
  class AlignmentUpdate : public ConditionUpdateCall, public AlignmentTypes  {
  public:
    AlignmentUpdate()   {    }
    virtual ~AlignmentUpdate() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context)  {
      Condition    target(key.name,"Alignment");
      Data&        data  = target.bind<Data>();
      Condition    cond0 = context.condition(0);
      const Map&   src0  = cond0.get<Map>();
      const Param& par0  = src0.firstParam().second;
      DetElement   det0  = context.dependency.detector;
      printout(INFO,"AlignmentUpdate","++ Building dependent condition: %s Detector [%d]: %s ",
               key.name.c_str(), det0.level(), det0.path().c_str());
      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta = src0.first<Delta>();
        data.delta         = delta;
        data.flag          = Data::HAVE_NONE;
      }
      else  {
        printout(INFO,"AlignmentUpdate","++ Failed to access alignment-Delta from %s",
                 cond0->value.c_str());
        _Printer()(cond0);
      }
      data.detector = context.dependency.detector;
      //data.condition   = target;
      return target;
    }
  };

  void __print(DetElement det,
               UserData& data,
               int level)
  {
    char fmt[128];
    auto i=data.keys.find(det.key());
    const char* has_alignment = i==data.keys.end() ? "NO " : "YES";
    size_t siz =  i==data.keys.end() ? 0 : 1;
    ::snprintf(fmt,sizeof(fmt),"%%d %%%ds %%p [%%d] %%s %%08X: %%s ",2*level);
    printout(INFO,"Conditions",fmt,
             det.level(), "", det.ptr(), siz, has_alignment,
             det.key(), det.path().c_str());
    const DetElement::Children& children = det.children();
    for(auto c=children.begin(); c!=children.end(); ++c)    {
      DetElement child = (*c).second;
      __print(child, data, level+1);
    }
  }
  
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
    AlignmentSelector(ConditionsAccess mgr) : m_manager(mgr)    {
      Operators::collectAllConditions(mgr, m_allConditions);
    }

    virtual ~AlignmentSelector()   {
      m_allDependencies.clear();
    }

    void addDependency(ConditionDependency* dependency)   {
      m_allDependencies.insert(dependency->target.hash, dependency);
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

    AlignmentUpdate* m_update;
    long collectDependencies(DetElement de, AlignmentUpdate* update, int level)  {
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
            DependencyBuilder build_1(target1, update->addRef());
            build_1->detector = de;
            for(RangeConditions::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)   {
              Condition    cond = *ic;
              ConditionKey key(cond->value);
              build_1.add(key);
            }
            addDependency(build_1.release());
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
        collectDependencies((*i).second, update, level+1);
      return 1;
    }

    int computeDependencies(long time)  {
      dd4hep_ptr<UserPool> user_pool;
      const Dependencies& dependencies = m_allDependencies;
      const IOVType* iovType = m_manager.iovType("epoch");
      IOV  iov(iovType, IOV::Key(time,time));
      long num_expired = m_manager.prepare(iov, user_pool, dependencies);
      printout(INFO,"Conditions",
               "+++ ConditionsUpdate: Updated %ld conditions... IOV:%s",
               num_expired, iov.str().c_str());
      
      UserData data;
      data.entries.reserve(dependencies.size());
      dependencies.for_each(DependencyCollector<UserData,_Oper>(data,_Oper()));

      string prev("-----");
      for(auto i=data.detectors.begin(); i!=data.detectors.end(); ++i)  {
        Entry& e = data.entries[(*i).second];
        DetElement    det = e.det;
        unsigned int  key = det.key();
        const string& p   = det.path();
        size_t idx = p.find(prev);
        if ( idx == 0 )  {
          //printout(INFO,"Conditions","***** %d %p %08X: %s ",
          //         det.level(), det.ptr(), key, det.path().c_str());
          continue;
        }
        prev = p;
        printout(INFO,"Conditions","%d %p %08X: %s ",
                 det.level(), det.ptr(), key, det.path().c_str());
        e.top = 1;
      }

      printout(INFO,"Conditions","Working down the tree....");
      for(auto i=data.detectors.begin(); i!=data.detectors.end(); ++i)  {
        Entry& e = data.entries[(*i).second];
        if ( e.top )  {
          DetElement det = e.det;
          printout(INFO,"Conditions","%d %p %08X: %s ",
                   det.level(), det.ptr(), det.key(), det.path().c_str());
          __print(det, data, 0);
        }
      }
      user_pool->clear();
      return 1;
    }
  };

  /// Plugin function
  long dddb_derived_alignments(LCDD& lcdd, int argc, char** argv) {
    long int long init_time = argc>0 ? *(long*)argv[0] : 0;//DDDB::DDDBReader::makeTime(2016,4,1,12);
    ConditionsManager manager = ConditionsManager::from(lcdd);
    AlignmentSelector selector(manager);
    AlignmentUpdate* update = new AlignmentUpdate();
    int ret = selector.collectDependencies(lcdd.world(), update, 0);
    if ( ret == 1 )  {
      ret = selector.computeDependencies(init_time);
    }
    delete update;
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DerivedAlignmentsTest,dddb_derived_alignments)
//==========================================================================
