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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/Detector.h"
#include "DD4hep/Path.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/ConditionDerived.h"

#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsOperators.h"
#include "DDCond/ConditionsManagerObject.h"

#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBConversion.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

using DDDB::DDDBDocument;

/// Anonymous namespace for plugins
namespace  {

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
    CallContext() = default;
  };
  
  /// Specialized conditions update callback for alignments
  /**
   *  Used by clients to update a condition.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionUpdate1 : public ConditionUpdateCall {
  public:
    CallContext& context;
    ConditionUpdate1(CallContext& c) : context(c)  {
      ++context.numCall1;
    }
    virtual ~ConditionUpdate1() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext& ) override final {
      printout(context.level,"ConditionUpdate1","++ Building dependent condition: %s",key.name.c_str());
      Condition    target(key.name,"Alignment");
      target.bind<AlignmentData>();
      return target;
    }
    /// Interface to client Callback in order to update the condition
    void resolve(Condition target, ConditionUpdateContext& ctxt) override final {
      try  {
        AlignmentData& data = target.get<AlignmentData>();
        Condition    cond0  = ctxt.condition(ctxt.key(0));
        const Delta& delta  = cond0.get<Delta>();
        data.delta          = delta;
        data.flag           = AlignmentData::HAVE_NONE;
        ++context.numBuild1;
      }
      catch(const exception& exc)   {
        ++context.numFail1;
        printout(ERROR,"ConditionUpdate2","++ Failed to build condition %s: %s",
                 ctxt.key(0).name.c_str(), exc.what());
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
  class ConditionUpdate2 : public ConditionUpdateCall {
  public:
    CallContext& context;
    ConditionUpdate2(CallContext& c) : context(c)  {
      ++context.numCall2;
    }
    virtual ~ConditionUpdate2() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext&)  override final  {
      printout(context.level,"ConditionUpdate2","++ Building dependent condition: %s",key.name.c_str());
      Condition     target(key.name,"Alignment");
      target.bind<AlignmentData>();
      return target;
    }
    /// Interface to client Callback in order to update the condition
    void resolve(Condition target, ConditionUpdateContext& ctxt) override final {
      try  {
        AlignmentData& data  = target.get<AlignmentData>();
        Condition     cond0  = ctxt.condition(ctxt.key(0));
        const Delta&  delta0 = cond0.get<Delta>();
        const AlignmentData& data1  = ctxt.get<AlignmentData>(ctxt.key(1));
        data.delta           = delta0;
        data.delta           = data1.delta;
        data.flag            = AlignmentData::HAVE_NONE;
        ++context.numBuild2;
      }
      catch(const exception& exc)   {
        ++context.numFail2;
        printout(ERROR,"ConditionUpdate2","++ Failed to build condition %s: %s",
                 ctxt.key(0).name.c_str(), exc.what());
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
  class ConditionUpdate3 : public ConditionUpdateCall {
  public:
    CallContext& context;
    ConditionUpdate3(CallContext& c) : context(c)  {
      ++context.numCall3;
    }
    virtual ~ConditionUpdate3() {    }
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, ConditionUpdateContext&)  override final  {
      printout(context.level,"ConditionUpdate3","++ Building dependent condition: %s",key.name.c_str());
      Condition    target(key.name,"Alignment");
      target.bind<AlignmentData>();
      return target;
    }
    /// Interface to client Callback in order to update the condition
    void resolve(Condition target, ConditionUpdateContext& ctxt) override final {
      try  {
        AlignmentData& data  = target.get<AlignmentData>();
        Condition     cond0 = ctxt.condition(ctxt.key(0));
        const Delta& delta0 = cond0.get<Delta>();
        const AlignmentData&  data1  = ctxt.get<AlignmentData>(ctxt.key(1));
        const AlignmentData&  data2  = ctxt.get<AlignmentData>(ctxt.key(2));
        data.delta          = delta0;
        data.delta          = data1.delta;
        data.delta          = data2.delta;
        data.flag           = AlignmentData::HAVE_NONE;
        ++context.numBuild3;
      }
      catch(const exception& exc)   {
        ++context.numFail3;
        printout(ERROR,"ConditionUpdate3","++ Failed to build condition %s: %s",
                 ctxt.key(0).name.c_str(), exc.what());
      }
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
    ConditionsManager m_manager;
    RangeConditions   m_allConditions;
    PrintLevel        m_level = INFO;
    CallContext       m_context;
    Content           content;

    /// Default constructor
    ConditionsSelector() = delete;
    /// Initializing constructor
    ConditionsSelector(const string& nam, ConditionsManager mgr, PrintLevel lvl)
      : m_name(nam), m_manager(mgr), m_level(lvl)
    {
      m_context.level = lvl;
      content.reset(new ConditionsContent());
      Operators::collectAllConditions(mgr, m_allConditions);
    }
    /// Default destructor
    virtual ~ConditionsSelector()   {
      printout(INFO,"Conditions","++ DDDB: Total number of conditions:   %ld", content->conditions().size());
      printout(INFO,"Conditions","++ DDDB: Total number of dependencies: %ld", content->derived().size());
      printout(INFO,"Conditions","++ DDDB: Number of Type1 instances:    %ld", m_context.numCall1);
      printout(INFO,"Conditions","++ DDDB: Number of Type1 callbacks:    %ld", m_context.numBuild1);
      printout(INFO,"Conditions","++ DDDB: Number of Type1 failures:     %ld", m_context.numFail1);
      printout(INFO,"Conditions","++ DDDB: Number of Type2 instances:    %ld", m_context.numCall2);
      printout(INFO,"Conditions","++ DDDB: Number of Type2 callbacks:    %ld", m_context.numBuild2);
      printout(INFO,"Conditions","++ DDDB: Number of Type2 failures:     %ld", m_context.numFail2);
      printout(INFO,"Conditions","++ DDDB: Number of Type3 instances:    %ld", m_context.numCall3);
      printout(INFO,"Conditions","++ DDDB: Number of Type3 callbacks:    %ld", m_context.numBuild3);
      printout(INFO,"Conditions","++ DDDB: Number of Type3 failures:     %ld", m_context.numFail3);
      printout(INFO,"Conditions","++ DDDB: Total Number of instances:    %ld",
               m_context.numCall1+m_context.numCall2+m_context.numCall3);
      printout(INFO,"Conditions","++ DDDB: Total Number of callbacks:    %ld",
               m_context.numBuild1+m_context.numBuild2+m_context.numBuild3);
      printout(INFO,"Conditions","++ DDDB: Total Number of failures:     %ld",
               m_context.numFail1+m_context.numFail2+m_context.numFail3);
      content.reset();
    }

    RangeConditions findCond(const string& match)   {
      RangeConditions result;
      if ( !match.empty() )  {
        for( Condition cond : m_allConditions )  {
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
      char fmt[64];
      DDDB::DDDBCatalog* cat = 0;
      const DetElement::Children& c = de.children();

      ::snprintf(fmt,sizeof(fmt),"%%-%ds-> ",2*level+5);
      try  {
        ::sprintf(fmt,"%03d %%-%ds Detector: %%s #Dau:%%d VolID:%%p",level+1,2*level+1);
        printout(m_level,m_name,fmt,"",de.path().c_str(),int(c.size()),(void*)de.volumeID());
        cat = de.extension<DDDB::DDDBCatalog>();
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
            for( Condition cond : rc )  {
              ConditionKey key(de, cond->name);
              if ( key.hash == cond.key() && cond.typeInfo() == typeid(Delta) )  {
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
                printout(m_context.level,m_name,"Build [%ld] cond.deps: %s [%s # %s] -> %016llX",
                         rc.size(), cat->condition.c_str(), de.path().c_str(), cond.name(), cond->hash);
                content->insertDependency(build_1.release());
                content->insertDependency(build_2.release());
                content->insertDependency(build_3.release());
              }
              else  { // These conditions cannot be handled....
                printout(INFO,m_name,"Ignore condition: %s [%s # %s] -> Hash: %016llX Key: %016llX Type: %s",
                         cat->condition.c_str(), de.path().c_str(), cond.name(), cond->hash,
                         key.hash, cond.data().dataType().c_str());
              }
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
  long dddb_derived_alignments(Detector& description, int argc, char** argv) {
    PrintLevel level = INFO;
    long time = detail::makeTime(2016,4,1,12);
    for(int i=0; i<argc; ++i)  {
      if ( ::strcmp(argv[i],"-time")==0 )  {
        time = detail::makeTime(argv[++i],"%d-%m-%Y %H:%M:%S");
        printout(level,"DDDB","Setting event time in %s to %s [%ld]",
                 Path(__FILE__).filename().c_str(), argv[i-1], time);
      }
      else if ( ::strcmp(argv[i],"-print")==0 )  {
        level = dd4hep::printLevel(argv[++i]);
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

    ConditionsSelector selector("DDDB_Derived",ConditionsManager::from(description), level);
    int ret = selector.collectDependencies(description.world(), 0);
    if ( ret == 1 )  {
      ret = selector.computeDependencies(time);
    }
    return ret;
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DerivedCondTest,dddb_derived_alignments)
//==========================================================================
