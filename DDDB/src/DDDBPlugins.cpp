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
#include "DD4hep/DetConditions.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/objects/DetectorInterna.h"

#include "DDCond/ConditionsOperators.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"

using namespace std;
using namespace DD4hep;
using DD4hep::Geometry::LCDD;

/// Plugin function
static long dddb_dump_conditions(LCDD& lcdd, int , char** ) {
  const void* args[] = { "-processor", "DDDB_ConditionsPrinter", "-prefix", "DDDB", "-end-processor", 0};
  lcdd.apply("DD4hep_ConditionsDump", 5, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsDump,dddb_dump_conditions)
//==========================================================================


/// Anonymous namespace for plugins
namespace {

  /// Basic entry point to print out the detector element hierarchy
  /**
   *  @author  M.Frank
   *  @version 1.0
   *  @date    01/04/2014
   */
  long dddb_map_condition_keys2detelements(LCDD& lcdd, int /* argc */, char** /* argv */) {

    using DDDB::Catalog;
    using Geometry::DetElement;

    /// Callback object to recursively register all conditions to detector elements.
    /** Register all conditions to the corresponding detector elements
     *  in the geometry to the Conditons manager object.
     *
     *  @author  M.Frank
     *  @version 1.0
     *  @date    01/04/2014
     */
    struct Actor {

      /// Recursive callback
      long operator()(DetElement de)  {
        const DetElement::Children& c = de.children();
        Conditions::DetConditions dc(de);
        Conditions::Container::Object* conditions = dc.conditions().ptr();
        try  {
          Catalog* cat = de.extension<Catalog>();
          if ( !cat->condition.empty() )  {
            conditions->addKey(cat->condition);
            conditions->addKey("Alignment", cat->condition);
          }
          for(const auto& i : cat->conditioninfo )  {
            const string& cond_name = i.second;
            conditions->addKey(cond_name);
            conditions->addKey(i.first, cond_name);
          }
        }
        catch(...)   {
        }
        for (const auto& i : c )
          (*this)(i.second);
        return 1;
      }
    };
    return Actor()(lcdd.world());
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_AssignConditionsKeys,dddb_map_condition_keys2detelements)
//==========================================================================

/// Anonymous namespace for plugins
namespace {

  /// Basic entry point to print out the detector element hierarchy
  /**
   *  @author  M.Frank
   *  @version 1.0
   *  @date    01/04/2014
   */
  template <int flag> long dump_detelement_tree(LCDD& lcdd, int argc, char** argv) {

    using Conditions::RangeConditions;
    using Conditions::AbstractMap;
    using Conditions::Condition;
    using Geometry::DetElement;

    using DDDB::ConditionPrinter;
    using DDDB::Catalog;

    struct Counters {
      long totConditions;
      long numConditions;
      long numAlignments;
      long numNoCatalogs;
      void reset() { totConditions=numConditions=numAlignments=numNoCatalogs=0; }
    };

    /// Callback object to print selective information
    /**
     *  @author  M.Frank
     *  @version 1.0
     *  @date    01/04/2014
     */
    struct Actor {
      /// Container with all known conditions
      vector<pair<int,Condition> >  m_allConditions;
      Counters        m_counters;
      bool            m_sensitivesOnly;
      bool            m_dumpConditions;
      string          m_name;
      LCDD&           m_lcdd;
      IOV             m_iov;

      /// Standard constructor
      Actor(LCDD& l, bool sens, bool dmp) : m_sensitivesOnly(sens), m_dumpConditions(dmp), m_lcdd(l), m_iov(0) {}

      /// Standard destructor
      ~Actor()  {
        if ( flag > 1 )  {
          printout(INFO,m_name,"++ Number of DetElements without catalog: %8ld",m_counters.numNoCatalogs);
          printout(INFO,m_name,"++ Number of attached alignments:         %8ld",m_counters.numAlignments);
          if ( flag > 2 )  {
            printout(INFO,m_name,"++ Number of attached conditions:         %8ld",m_counters.numConditions);
            printout(INFO,m_name,"++ Total number of conditions:            %8ld",m_counters.totConditions);
          }
        }
#if 0
        printout(INFO,m_name,"**************** DDDB Detector dump: Conditions Usage ***************************");
        for(const auto& ic : m_allConditions )  {
          Condition cond = ic.second;
          const AbstractMap& data = cond.get<AbstractMap>();
          const DDDB::Document* doc = data.option<DDDB::Document>();
          if ( doc ) 
            printout(INFO,m_name,"++ Usage: %d Cond: %s/%s  -> %s [%08X]",
                     ic.first, doc->name.c_str(), cond->name.c_str(), cond->value.c_str(), cond->hash);
          else
            printout(INFO,m_name,"++ Usage: %d Cond: ---/%s  -> %s [%08X]",
                     ic.first, cond->name.c_str(), cond->value.c_str(), cond->hash);
        }
#endif
        printout(INFO,m_name,"*********************************************************************************");
      }

      /// Initialization
      Actor& init()  {
        m_counters.reset();
        m_allConditions.clear();

        if ( flag >= 3 )   {
          RangeConditions rc;
          Conditions::Operators::collectAllConditions(m_lcdd, rc);
          m_iov.reset().invert();
          m_iov.iovType = 0;
          for ( Condition cond : rc )   {
            m_allConditions.push_back(make_pair(0,cond));
            if ( !m_iov.iovType ) m_iov = cond.iov();
            else  m_iov.iov_intersection(cond.iov());
          }
          m_iov.set(m_iov.keyData.first);
          if ( m_dumpConditions )   {
            DDDB::ConditionPrinter prt;
            printout(INFO,m_name,"**************** DDDB Detector dump: ALL Conditions *****************************");
            for(Condition cond : rc ) prt(cond);
            printout(INFO,m_name,"*********************************************************************************");
          }
        }
        return *this;
      }

      /// __________________________________________________________________________________
      RangeConditions findCond(const string& match)   {
        RangeConditions result;
        if ( !match.empty() )  {
          for ( auto& ic : m_allConditions )  {
            Condition cond = ic.second;
            size_t idx = cond->value.find(match);
            if ( idx == 0 )  {
              if (cond->value.length() == match.length() )   {
                ic.first++;
                result.push_back(cond);
              }
              else if ( cond->value[match.length()] == '/' )  {
                size_t idq = cond->value.find('/',match.length()+1);
                if ( idq == string::npos )  {
                  ic.first++;
                  result.push_back(cond);
                }
              }
            }
          }
        }
        return result;
      }
      /// __________________________________________________________________________________
      void printDetElement(int level, DetElement de,
                           ConditionPrinter& prt,
                           bool with_placement=false,
                           bool with_keys=false,
                           bool with_values=false)
      {
        char fmt[128];
        const DetElement::Children& c = de.children();
        ::sprintf(fmt,"%03d %%-%ds Detector: %%s #Dau:%%d VolID:%%p Place:%%p",level+1,2*level+1);
        printout(INFO, m_name, fmt, "", de.path().c_str(), int(c.size()),
                 (void*)de.volumeID(), (void*)de.placement().ptr());
        if ( with_placement )  {
          ::sprintf(fmt,"%03d %%-%ds Placement: %%s",level+1,2*level+3);
          printout(INFO,m_name,fmt,"",de.placementPath().c_str());
        }
        if ( (with_keys || with_values) && de.hasConditions() )  {
          Conditions::DetConditions dc(de);
          Conditions::Container cont = dc.conditions();
          ::sprintf(fmt,"%03d %%-%ds Key: %%08X -> %%08X -> %%s",level+1,2*level+3);
          for(const auto& i : cont->keys )  {
            if ( with_keys )   {
              printout(INFO,m_name,fmt,"",i.first,i.second.first, i.second.second.c_str());
            }
            if ( with_values )   {
              Condition::key_type key = i.second.first;
              Condition cond = dc.get(key, m_iov);
              prt(cond);
            }
          }
        }
      }
      /// __________________________________________________________________________________
      void printConditionInfo(int level, Catalog* cat, ConditionPrinter& prt, bool with_elements=false)   {
        if ( cat && !cat->conditioninfo.empty() )   {
          char fmt[128];
          ::sprintf(fmt,"%03d %%-%ds Cond:%%-20s -> %%s",level+1,2*level+3);
          for(const auto& i : cat->conditioninfo )  {
            const string& cond_name = i.second;
            ++m_counters.numConditions;
            if ( with_elements )  {
              RangeConditions rc = findCond(cond_name);
              printout(INFO,m_name,fmt,"",i.first.c_str(), 
                       rc.empty() ? (cond_name+"  !!!UNRESOLVED!!!").c_str() : cond_name.c_str());
              for(Condition cond : rc ) prt(cond);
              continue;
            }
            printout(INFO,m_name,fmt,"",i.first.c_str(),cond_name.c_str());
          }
        }
      }
      /// __________________________________________________________________________________
      void printAlignment(int level, Catalog* cat, ConditionPrinter& prt, bool with_values=false)   {
        if ( cat && !cat->condition.empty() )  {
          char fmt[128];
          ::sprintf(fmt,"%03d %%-%ds %%-20s -> %%s",level+1,2*level+3);
          ++m_counters.numAlignments;
          if ( with_values )  {
            RangeConditions rc = findCond(cat->condition);
            printout(INFO,m_name,fmt,"","Alignment:", 
                     rc.empty() ? (cat->condition+"  !!!UNRESOLVED!!!").c_str() : cat->condition.c_str());
            for(const auto& i : rc)
              prt(i);
            return;
          }
          printout(INFO,m_name,fmt,"","Alignment:",cat->condition.c_str());
        }
      }
      /// __________________________________________________________________________________
      long dump(DetElement de, int level)  {
        char fmt[64], text[512];
        Catalog* cat = 0;
        ConditionPrinter prt;
        const DetElement::Children& c = de.children();
        ::snprintf(fmt,sizeof(fmt),"%%-%ds-> ",2*level+5);
        ::snprintf(text,sizeof(text),fmt,"");
        prt.setPrefix(text);
        try  {
          if ( !m_sensitivesOnly || 0 != de.volumeID() )  {
            switch(flag)  {
            case 0:
              printDetElement(level, de, prt, false, false);
              break;
            case 1:
              printDetElement(level, de, prt, false, false);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, prt, false);
              printConditionInfo(level, cat, prt, false);
              break;
            case 2:
              printDetElement(level, de, prt, true, true);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, prt, false);
              printConditionInfo(level, cat, prt, false);
              break;
            case 3:
              printDetElement(level, de, prt, false, false);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, prt, true);
              break;
            case 4:
              printDetElement(level, de, prt, true, true);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, prt, true);
              printConditionInfo(level, cat, prt, true);
              break;
            case 5:
              printDetElement(level, de, prt, true, true, true);
              break;
            default:
              break;
            }
          }
        }
        catch(...)  {
          ::sprintf(fmt,"%03d %%-%ds %%s%%-20s -> %%s",level+1,2*level+3);
          printout(INFO,m_name, fmt, "", de.path().c_str(), "NO CATALOG availible!", "");
          ++m_counters.numNoCatalogs;
        }
        for (const auto& i : c)
          dump(i.second,level+1);
        return 1;
      }
    };

    bool dump_sensitive_only = false, dump_all_cond = false;
    for(int i=0; i<argc; ++i)  {
      if ( ::strcmp(argv[i],"--sensitive")==0 ) { dump_sensitive_only = true; }
      if ( ::strcmp(argv[i],"--dump")==0      ) { dump_all_cond = true;       }
      if ( ::strcmp(argv[i],"--help")==0 ) {
        printout(INFO,"Plugin-Help","Usage: DDDBDetectorDump --opt [--opt]");
        printout(INFO,"Plugin-Help","  --sensitive          Only use sensitive elements");
        printout(INFO,"Plugin-Help","  --dump               Print conditions inventory");
        printout(INFO,"Plugin-Help","  --help               Print this help message");
        return 0;
      }
    }
    Actor actor(lcdd, dump_sensitive_only, dump_all_cond);
    actor.m_name = "DDDBDetectorDump";
    if ( flag == 1 )
      actor.m_name = "DDDBDetVolumeDump";
    else if ( flag == 2 )
      actor.m_name = "DDDBDetConditionKeyDump";
    else if ( flag == 3 )
      actor.m_name = "DDDBDetConditionDump";
    else if ( flag == 4 )
      actor.m_name = "DDDBDetectorDump";
    else if ( flag == 5 )
      actor.m_name = "DetElementConditionDump";
    printout(INFO,actor.m_name,"**************** DDDB Detector dump *****************************");
    return actor.init().dump(lcdd.world(), 0);
  }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(DDDB_DetectorVolumeDump,dump_detelement_tree<1>)
DECLARE_APPLY(DDDB_DetectorConditionKeysDump,dump_detelement_tree<2>)
DECLARE_APPLY(DDDB_DetectorAlignmentDump,dump_detelement_tree<3>)
DECLARE_APPLY(DDDB_DetectorConditionDump,dump_detelement_tree<4>)
DECLARE_APPLY(DDDB_DetElementConditionDump,dump_detelement_tree<5>)
//==========================================================================
