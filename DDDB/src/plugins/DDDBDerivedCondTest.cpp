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
#include "DD4hep/Path.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/Alignments.h"

#include "DDCond/ConditionsSlice.h"
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

  /// Helper containing shared typedefs
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class DataTypes  {
  public:
    typedef AbstractMap                      Map;
    typedef Map::Params                      Params;
    typedef Params::value_type::second_type  Param;
    typedef Alignments::AlignmentData        Data;
    typedef Alignments::Delta                Delta;
    typedef DDDB::ConditionPrinter           _Printer;
  };

  /// Helper containing shared context
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  struct CallContext {
    long              numCall1 = 0;
    long              numCall2 = 0;
    long              numCall3 = 0;
    long              numFail1 = 0;
    long              numFail2 = 0;
    long              numFail3 = 0;
    long              numBuild1 = 0;
    long              numBuild2 = 0;
    long              numBuild3 = 0;
    long              numAlignments = 0;
    long              numNoCatalogs = 0;
    PrintLevel        level = INFO;
    DDDB::ConditionPrinter printer;
    CallContext() : printer(0,"Conditions") {}
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
    CallContext& context;
    ConditionUpdate1(CallContext& c) : context(c)  {
      ++context.numCall1;
    }
    virtual ~ConditionUpdate1() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& ctxt)  {
      printout(context.level,"ConditionUpdate1","++ Building dependent condition: %s",key.name.c_str());
      Condition    target(key.name,"Alignment");
      Data&        data  = target.bind<Data>();
      Condition    cond0 = ctxt.condition(0);
      const Map&   src0  = cond0.get<Map>();
      const Param& par0  = src0.firstParam().second;
      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta = src0.first<Delta>();
        data.delta         = delta;
        data.flag          = Data::HAVE_NONE;
        ++context.numBuild1;
      }
      else  {
        ++context.numFail1;
        printout(context.level,"ConditionUpdate1","++ Failed to access Delta from %s",
                 cond0->value.c_str());
        context.printer(cond0);
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
    CallContext& context;
    ConditionUpdate2(CallContext& c) : context(c)  {
      ++context.numCall2;
    }
    virtual ~ConditionUpdate2() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& ctxt)  {
      printout(context.level,"ConditionUpdate2","++ Building dependent condition: %s",key.name.c_str());
      Condition     target(key.name,"Alignment");
      Data&         data  = target.bind<Data>();
      Condition     cond0 = ctxt.condition(0);
      const Map&    src0  = cond0.get<Map>();      
      const Param&  par0  = src0.firstParam().second;

      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta0 = src0.first<Delta>();
        const Data&  delta1 = ctxt.get<Data>(1);
        data.delta          = delta0;
        data.delta          = delta1.delta;
        data.flag           = Data::HAVE_NONE;
        ++context.numBuild2;
      }
      else  {
        ++context.numFail2;
        printout(context.level,"ConditionUpdate2","++ Failed to access Delta from %s",
                 cond0->value.c_str());
        context.printer(cond0);
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
    CallContext& context;
    ConditionUpdate3(CallContext& c) : context(c)  {
      ++context.numCall3;
    }
    virtual ~ConditionUpdate3() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& ctxt)  {
      printout(context.level,"ConditionUpdate3","++ Building dependent condition: %s",key.name.c_str());
      Condition    target(key.name,"Alignment");
      Data&        data  = target.bind<Data>();
      Condition    cond0 = ctxt.condition(0);
      const Map&   src0  = cond0.get<Map>();
      const Param& par0  = src0.firstParam().second;

      if ( par0.typeInfo() == typeid(Delta) )  {
        const Delta& delta0 = src0.first<Delta>();
        const Data&  delta1 = ctxt.get<Data>(1);
        const Data&  delta2 = ctxt.get<Data>(2);
        data.delta          = delta0;
        data.delta          = delta1.delta;
        data.delta          = delta2.delta;
        data.flag           = Data::HAVE_NONE;
        ++context.numBuild3;
      }
      else  {
        ++context.numFail3;
        printout(context.level,"ConditionUpdate2","++ Failed to access Delta from %s",
                 cond0->value.c_str());
        context.printer(cond0);
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
    typedef std::shared_ptr<ConditionsContent> Content;
    string            m_name;
    RangeConditions   m_allConditions;
    ConditionsManager m_manager;
    PrintLevel        m_level = INFO;
    CallContext       m_context;
    Content           content;

    /// Default constructor
    ConditionsSelector() = delete;
    /// Initializing constructor
    ConditionsSelector(ConditionsManager mgr, PrintLevel lvl)
      : m_manager(mgr), m_level(lvl)
    {
      m_context.level = lvl;
      content.reset(new ConditionsContent());
      Operators::collectAllConditions(mgr, m_allConditions);
    }
    /// Default destructor
    virtual ~ConditionsSelector()   {
      printout(INFO,"Conditions","+++ Total number of conditions:   %ld", content->conditions().size());
      printout(INFO,"Conditions","+++ Total number of dependencies: %ld", content->derived().size());
      printout(INFO,"Conditions","+++ Number of Type1 instances:    %ld", m_context.numCall1);
      printout(INFO,"Conditions","+++ Number of Type1 callbacks:    %ld", m_context.numBuild1);
      printout(INFO,"Conditions","+++ Number of Type1 failures:     %ld", m_context.numFail1);
      printout(INFO,"Conditions","+++ Number of Type2 instances:    %ld", m_context.numCall2);
      printout(INFO,"Conditions","+++ Number of Type2 callbacks:    %ld", m_context.numBuild2);
      printout(INFO,"Conditions","+++ Number of Type2 failures:     %ld", m_context.numFail2);
      printout(INFO,"Conditions","+++ Number of Type3 instances:    %ld", m_context.numCall3);
      printout(INFO,"Conditions","+++ Number of Type3 callbacks:    %ld", m_context.numBuild3);
      printout(INFO,"Conditions","+++ Number of Type3 failures:     %ld", m_context.numFail3);
      printout(INFO,"Conditions","+++ Total Number of instances:    %ld",
               m_context.numCall1+m_context.numCall2+m_context.numCall3);
      printout(INFO,"Conditions","+++ Total Number of callbacks:    %ld",
               m_context.numBuild1+m_context.numBuild2+m_context.numBuild3);
      printout(INFO,"Conditions","+++ Total Number of failures:     %ld",
               m_context.numFail1+m_context.numFail2+m_context.numFail3);
      content.reset();
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
      string pref = m_context.printer.prefix;
      const DetElement::Children& c = de.children();

      ::snprintf(fmt,sizeof(fmt),"%%-%ds-> ",2*level+5);
      ::snprintf(text,sizeof(text),fmt,"");
      m_context.printer.setPrefix(text);

      try  {
        ::sprintf(fmt,"%03d %%-%ds Detector: %%s #Dau:%%d VolID:%%p",level+1,2*level+1);
        printout(m_level,m_name,fmt,"",de.path().c_str(),int(c.size()),(void*)de.volumeID());
        cat = de.extension<DDDB::Catalog>();
        ::sprintf(fmt,"%03d %%-%ds %%-20s -> %%s", level+1, 2*level+3);
        //
        // We use here the alignment entry from the DDDB catalog:
        // This ensures, that we are only bound by the name and not by any other means!
        //
        if ( !cat->condition.empty() )  {
          RangeConditions rc = findCond(cat->condition);
          printout(m_level,m_name,fmt,"","Alignment:    ", 
                   rc.empty() ? (cat->condition+"  !!!UNRESOLVED!!!").c_str() : cat->condition.c_str());
          if ( !rc.empty() )   {
            for(RangeConditions::const_iterator ic=rc.begin(); ic!=rc.end(); ++ic)   {
              Condition    cond = *ic;
              ConditionKey key(de, cond->name);
              ConditionKey target1(de,cond->name+"/derived_1");
              ConditionKey target2(de,cond->name+"/derived_2");
              ConditionKey target3(de,cond->name+"/derived_3");
              DependencyBuilder build_1(de, cond->name+"/derived_1", new ConditionUpdate1(m_context));
              DependencyBuilder build_2(de, cond->name+"/derived_2", new ConditionUpdate2(m_context));
              DependencyBuilder build_3(de, cond->name+"/derived_3", new ConditionUpdate3(m_context));
              build_1.add(key);

              build_2.add(key);
              build_2.add(target1);

              build_3.add(key);
              build_3.add(target1);
              build_3.add(target2);
              printout(INFO,m_name,"Building [%ld] condition dependencies for: %s [%s # %s] -> %lld [%016llX]",
                       rc.size(), cat->condition.c_str(), de.path().c_str(), cond.name(), cond->hash, cond->hash);
              content->insertDependency(build_1.release());
              content->insertDependency(build_2.release());
              content->insertDependency(build_3.release());
            }
          }
          ++m_context.numAlignments;
        }
      }
      catch(...)  {
        ::sprintf(fmt,"%03d %%-%ds %%s%%-20s -> %%s",level+1,2*level+3);
        printout(m_level,m_name, fmt, "", de.path().c_str(), "NO CATALOG availible!", "");
        ++m_context.numNoCatalogs;
      }
      m_context.printer.setPrefix(pref);
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
        collectDependencies((*i).second,level+1);
      return 1;
    }

    int computeDependencies(long time)  {
      const IOVType* iovType = m_manager.iovType("epoch");
      shared_ptr<ConditionsSlice> slice(new ConditionsSlice(m_manager,content));
      IOV  iov(iovType, IOV::Key(time,time));
      m_manager.prepare(iov, *slice);
      printout(m_level,"Conditions",
               "+++ ConditionsUpdate: Updated %ld conditions... IOV:%s",
               long(slice->pool->size()), iov.str().c_str());
      slice->pool->clear();
      return 1;
    }
  };

  //========================================================================
  /// Plugin function
  /// Load, compute and access derived conditions
  long dddb_derived_alignments(LCDD& lcdd, int argc, char** argv) {
    PrintLevel level = INFO;
    long time = makeTime(2016,4,1,12);
    for(int i=0; i<argc; ++i)  {
      if ( ::strcmp(argv[i],"-time")==0 )  {
        time = makeTime(argv[++i],"%d-%m-%Y %H:%M:%S");
        printout(level,"DDDB","Setting event time in %s to %s [%ld]",
                 Path(__FILE__).filename().c_str(), argv[i-1], time);
      }
      else if ( ::strcmp(argv[i],"-print")==0 )  {
        level = DD4hep::printLevel(argv[++i]);
        printout(level,"DDDB","Setting print level in %s to %s [%d]",
                 Path(__FILE__).filename().c_str(), argv[i-1], level);
      }
      else if ( ::strcmp(argv[i],"--help")==0 )      {
        printout(level,"Plugin-Help","Usage: DDDB_DerivedCondTest --opt [--opt]        ");
        printout(level,"Plugin-Help","  -time  <string>     Set event time Format: \"%%d-%%m-%%Y %%H:%%M:%%S\"");
        printout(level,"Plugin-Help","  -print <value>      Printlevel for output      ");
        printout(level,"Plugin-Help","  -help               Print this help message    ");
        ::exit(EINVAL);
      }
    }

    ConditionsSelector selector(ConditionsManager::from(lcdd), level);
    int ret = selector.collectDependencies(lcdd.world(), 0);
    if ( ret == 1 )  {
      ret = selector.computeDependencies(time);
    }
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DerivedCondTest,dddb_derived_alignments)
//==========================================================================
