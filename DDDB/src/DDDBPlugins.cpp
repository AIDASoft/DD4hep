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
#include "DD4hep/DetAlign.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/objects/DetectorInterna.h"

#include "DDCond/ConditionsOperators.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"

using namespace std;
using namespace DD4hep;
using DD4hep::Geometry::LCDD;

/// Anonymous namespace for plugins
namespace {
  PrintLevel s_PrintLevel = INFO;
}

/// Plugin function
static long dddb_plugin_print_level(LCDD& /* lcdd */, int argc, char** argv) {
  for(int i=0; i<argc; ++i)  {
    if ( ::strcmp(argv[i],"--print")==0 )  {
      s_PrintLevel = DD4hep::printLevel(argv[++i]);
      printout(INFO,"DDDB","Setting print level for %s to %s [%d]",__FILE__,argv[i-1],s_PrintLevel);
      return 1;
    }
  }
  return 0;
}
DECLARE_APPLY(DDDB_PluginLevel,dddb_plugin_print_level)

/// Plugin function
static long dddb_dump_conditions(LCDD& lcdd, int , char** ) {
  const void* args[] = { "-processor", "DDDB_ConditionsPrinter", "-prefix", "DDDB", "-printlevel", "INFO", "-end-processor", 0};
  lcdd.apply("DD4hep_ConditionsDump", 7, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsDump,dddb_dump_conditions)
//==========================================================================
/// Plugin function
static long dddb_dump_conditions_summary(LCDD& lcdd, int , char** ) {
  const void* args[] = { "-processor", "DDDB_ConditionsPrinter", "-prefix", "DDDB", "-printlevel", "DEBUG", "-end-processor", 0};
  lcdd.apply("DD4hep_ConditionsDump", 7, (char**)args);
  return 1;
}
DECLARE_APPLY(DDDB_ConditionsSummary,dddb_dump_conditions_summary)
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
  long dump_det_tree(LCDD& lcdd, int flag, int argc, char** argv) {

    using Conditions::RangeConditions;
    using Conditions::AbstractMap;
    using Conditions::Condition;
    using Geometry::DetElement;

    using DDDB::ConditionPrinter;
    using DDDB::Catalog;


    /// Callback object to print selective information
    /**
     *  @author  M.Frank
     *  @version 1.0
     *  @date    01/04/2014
     */
    struct DumpActor {
      struct Counters  final {
        long totConditions = 0;
        long numConditions = 0;
        long numAlignments = 0;
        long numDetElements = 0;
        long numDetAlignmentKeys = 0;
        long numDetConditionKeys = 0;
        long numNoCatalogs = 0;
        long numDetPlacements = 0;
        Counters() = default;
        ~Counters() = default;
        void reset() {
          totConditions=numConditions=numAlignments=numNoCatalogs=0;
          numDetElements=numDetConditionKeys=numDetAlignmentKeys=0;
          numDetPlacements=0;
        }
      };

      /// Container with all known conditions
      vector<pair<int,Condition> >  m_allConditions;
      ConditionPrinter m_printer;
      ConditionPrinter m_condPrinter;
      ConditionPrinter m_alignPrinter;
      Counters         m_counters;
      int              m_flag;
      bool             m_sensitivesOnly;
      bool             m_dumpConditions;
      string           m_name;
      LCDD&            m_lcdd;
      IOV              m_iov;

      /// Standard constructor
      DumpActor(LCDD& l, int flg, bool sens, bool dmp)
        : m_printer("DDDBDetectors"), m_condPrinter("DDDBConditions"), m_alignPrinter("DDDBAlignments"),
          m_flag(flg), m_sensitivesOnly(sens), m_dumpConditions(dmp), m_lcdd(l), m_iov(0)
      {
        m_printer.setPrintLevel(s_PrintLevel);
        m_condPrinter.setPrintLevel(s_PrintLevel);
        m_alignPrinter.setPrintLevel(s_PrintLevel);
      }

      /// Standard destructor
      ~DumpActor()  {
        printout(INFO,m_name,"++ DDDB: Number of DetElements in the geometry: %8ld",m_counters.numDetElements);
        printout(INFO,m_name,"++ DDDB: Number of DetElement placements:       %8ld",m_counters.numDetPlacements);
        printout(INFO,m_name,"++ DDDB: Number of DetElement condition keys:   %8ld",m_counters.numDetConditionKeys);
        printout(INFO,m_name,"++ DDDB: Number of DetElement alignment keys:   %8ld",m_counters.numDetAlignmentKeys);
        if ( m_flag > 1 )  {
          printout(INFO,m_name,"++ DDDB: Number of DetElements without catalog: %8ld",m_counters.numNoCatalogs);
          printout(INFO,m_name,"++ DDDB: Number of attached conditions:         %8ld",m_counters.numConditions);
          printout(INFO,m_name,"++ DDDB: Number of attached alignments:         %8ld",m_counters.numAlignments);
          if ( m_flag > 2 )  {
            printout(INFO,m_name,"++ DDDB: Total number of conditions:            %8ld",m_counters.totConditions);
          }
        }
        printout(INFO,m_name,"*********************************************************************************");
      }

      /// Initialization
      DumpActor& init()  {
        m_counters.reset();
        m_allConditions.clear();

        if ( m_flag >= 3 )   {
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
            printout(INFO,m_name,"**************** DDDB Detector dump: ALL Conditions *****************************");
            for(Condition cond : rc ) m_printer(cond);
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
                           bool with_placement=false,
                           bool with_keys=false,
                           bool with_values=false)
      {
        char fmt[128];
        const DetElement::Children& c = de.children();
        ++m_counters.numDetElements;
        ::sprintf(fmt,"%03d %%-%ds Detector: %%s #Dau:%%d VolID:%%p Place:%%p",level+1,2*level+1);
        printout(s_PrintLevel, m_name, fmt, "", de.path().c_str(), int(c.size()),
                 (void*)de.volumeID(), (void*)de.placement().ptr());
        if ( de.placement().isValid() ) {
          ++m_counters.numDetPlacements;
        }
        if ( de.hasConditions() )  {
          Conditions::DetConditions dc(de);
          m_counters.numDetConditionKeys += dc.conditions().numKeys();
        }
        if ( de.hasAlignments() )  {
          Alignments::DetAlign da(de);
          m_counters.numDetAlignmentKeys += da.alignments().numKeys();
        }
        if ( with_placement )  {
          ::sprintf(fmt,"%03d %%-%ds Placement: %%s",level+1,2*level+3);
          printout(s_PrintLevel,m_name,fmt,"",de.placementPath().c_str());
        }
        if ( (with_keys || with_values) && de.hasConditions() )  {
          Conditions::DetConditions dc(de);
          Conditions::Container cont = dc.conditions();
          ::sprintf(fmt,"%03d %%-%ds Key: %%16llX -> %%16llX -> %%s",level+1,2*level+3);
          for(const auto& i : cont->keys )  {
            if ( with_keys )   {
              printout(s_PrintLevel,m_name,fmt,"",i.first,i.second.first, i.second.second.c_str());
            }
            if ( with_values )   {
              Condition::key_type key = i.second.first;
              Condition cond = dc.get(key, m_iov);
              m_printer(cond);
            }
          }
        }
      }
      /// __________________________________________________________________________________
      void printConditionInfo(int level, Catalog* cat, bool with_elements=false)   {
        if ( cat && !cat->conditioninfo.empty() )   {
          char fmt[128];
          ++m_counters.numConditions;
          ::sprintf(fmt,"%03d %%-%ds Cond:%%-20s -> %%s",level+1,2*level+3);
          for(const auto& i : cat->conditioninfo )  {
            const string& cond_name = i.second;
            if ( with_elements )  {
              RangeConditions rc = findCond(cond_name);
              printout(s_PrintLevel,m_name,fmt,"",i.first.c_str(), 
                       rc.empty() ? (cond_name+"  !!!UNRESOLVED!!!").c_str() : cond_name.c_str());
              for(Condition cond : rc ) m_condPrinter(cond);
              continue;
            }
            printout(s_PrintLevel,m_name,fmt,"",i.first.c_str(),cond_name.c_str());
          }
        }
      }
      /// __________________________________________________________________________________
      void printAlignment(int level, Catalog* cat, bool with_values=false)   {
        if ( cat && !cat->condition.empty() )  {
          char fmt[128];
          ::sprintf(fmt,"%03d %%-%ds %%-20s -> %%s",level+1,2*level+3);
          ++m_counters.numAlignments;
          if ( with_values )  {
            RangeConditions rc = findCond(cat->condition);
            printout(s_PrintLevel,m_name,fmt,"","Alignment:", 
                     rc.empty() ? (cat->condition+"  !!!UNRESOLVED!!!").c_str() : cat->condition.c_str());
            for(const auto& i : rc)
              m_printer(i);
            return;
          }
          printout(s_PrintLevel,m_name,fmt,"","Alignment:",cat->condition.c_str());
        }
      }
      /// __________________________________________________________________________________
      long dump(DetElement de, int level)  {
        char fmt[64], text[512];
        Catalog* cat = 0;
        const DetElement::Children& c = de.children();
        ::snprintf(fmt,sizeof(fmt),"%%-%ds-> ",2*level+5);
        ::snprintf(text,sizeof(text),fmt,"");
        m_printer.setPrefix(text);
        try  {
          if ( !m_sensitivesOnly || 0 != de.volumeID() )  {
            switch(m_flag)  {
            case 0:
              printDetElement(level, de, false, false);
              break;
            case 1:
              printDetElement(level, de, false, false);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, false);
              printConditionInfo(level, cat, false);
              break;
            case 2:
              printDetElement(level, de, true, true);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, false);
              printConditionInfo(level, cat, false);
              break;
            case 3:
              printDetElement(level, de, false, false);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, true);
              break;
            case 4:
              printDetElement(level, de, true, true);
              cat = de.extension<Catalog>();
              printAlignment(level, cat, true);
              printConditionInfo(level, cat, true);
              break;
            case 5:
              printDetElement(level, de, true, true, true);
              break;
            default:
              break;
            }
          }
        }
        catch(const exception& e)  {
          ::sprintf(fmt,"%03d %%-%ds Exception from: %%s %%-20s %%s",level+1,2*level+3);
          printout(INFO, m_name, fmt, "", de.path().c_str(), "[NO CATALOG availible]",e.what());
          ++m_counters.numNoCatalogs;
        }
        catch(...)  {
          ::sprintf(fmt,"%03d %%-%ds Exception from: %%s %%-20s",level+1,2*level+3);
          printout(INFO, m_name, fmt, "", de.path().c_str(), "[NO CATALOG availible]");
          ++m_counters.numNoCatalogs;
        }
        for (const auto& i : c)
          dump(i.second,level+1);
        return 1;
      }
    };
    bool dump_sensitive_only = false, dump_all_cond = false;
    for(int i=0; i<argc; ++i)  {
      if ( ::strcmp(argv[i],"-sensitive")==0 )   { dump_sensitive_only = true; }
      else if ( ::strcmp(argv[i],"-dump")==0 )   { dump_all_cond = true;       }
      else if ( ::strcmp(argv[i],"-print")==0 )  {
        s_PrintLevel = DD4hep::printLevel(argv[++i]);
        printout(INFO,"DDDB","Setting print level for %s to %s [%d]",__FILE__,argv[i-1],s_PrintLevel);
      }
      else if ( ::strcmp(argv[i],"--help")==0 )      {
        printout(INFO,"Plugin-Help","Usage: DDDBDetectorDump --opt [--opt]            ");
        printout(INFO,"Plugin-Help","  -sensitive          Only use sensitive elements");
        printout(INFO,"Plugin-Help","  -dump               Print conditions inventory ");
        printout(INFO,"Plugin-Help","  -print <value>      Printlevel for output      ");
        printout(INFO,"Plugin-Help","  -help               Print this help message    ");
        return 0;
      }
    }
    DumpActor actor(lcdd, flag, dump_sensitive_only, dump_all_cond);
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
  
  template <int flag> long dump_detelement_tree(LCDD& lcdd, int argc, char** argv)
  {  return dump_det_tree(lcdd,flag,argc,argv);    }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DetectorDump,dump_detelement_tree<0>)
DECLARE_APPLY(DDDB_DetectorVolumeDump,dump_detelement_tree<1>)
DECLARE_APPLY(DDDB_DetectorConditionKeysDump,dump_detelement_tree<2>)
DECLARE_APPLY(DDDB_DetectorAlignmentDump,dump_detelement_tree<3>)
DECLARE_APPLY(DDDB_DetectorConditionDump,dump_detelement_tree<4>)
DECLARE_APPLY(DDDB_DetElementConditionDump,dump_detelement_tree<5>)
//==========================================================================
