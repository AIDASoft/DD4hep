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
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/AlignmentsCalculator.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DDCond/ConditionsSlice.h"

#include "DDDB/DDDBConversion.h"

#include "TStatistic.h"
#include "TTimeStamp.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Alignments::AlignmentsCalculator;

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
    typedef std::shared_ptr<ConditionsContent> Content;
    std::map<DetElement,Condition::key_type> deltas;
    LCDD&                lcdd;
    string               m_name;
    PrintLevel           m_level = INFO;
    long                 m_installCount = 0;
    long                 m_accessCount = 0;
    TStatistic           acc_stat, comp_stat;
    Content              content;
    
    /// Initializing constructor
    AlignmentSelector(LCDD& l, PrintLevel p)
      : lcdd(l), m_name("DDDBAlignments"), m_level(p), acc_stat("Access"), comp_stat("Compute")   {
      content.reset(new ConditionsContent());
    }
    /// Default destructor
    virtual ~AlignmentSelector()   {
      printout(INFO,m_name,"++ Installed alignment calls:     %ld", m_installCount);
      printout(INFO,m_name,"++ Number of accessed alignments: %ld", m_accessCount);
      content.reset();
    }
    /// Recursive alignment collector
    long collect(DetElement de, ConditionsSlice& slice, int level)
    {
      char fmt[64];
      try  {
        DDDB::Catalog* cat = de.extension<DDDB::Catalog>();
        if ( !cat->condition.empty() )  {
          ConditionKey key(de, cat->condition);
          Condition    cond = slice.pool->get(key.hash);
          if ( cond.isValid() )   {
            deltas.insert(make_pair(de,cond->hash));
            ++m_installCount;
          }
          else  {
            ::sprintf(fmt,"%03d %%-%ds %%-20s -> %%s",level+1,2*level+3);
            printout(m_level,m_name,fmt,"","Alignment:    ", 
                     !cond.isValid() ? (cat->condition+"  !!!UNRESOLVED!!!").c_str() : cat->condition.c_str());
          }
        }
      }
      catch(const exception& e)  {
        ::sprintf(fmt,"%03d %%-%ds %%s: %%-20s  Exception: %%s",level+1,2*level+3);
        printout(ERROR,m_name, fmt, "", de.path().c_str(), "[NO CATALOG availible]", e.what());
      }
      catch(...)  {
        ::sprintf(fmt,"%03d %%-%ds %%s: %%-20s",level+1,2*level+3);
        printout(ERROR,m_name, fmt, "", de.path().c_str(), "[NO CATALOG availible]");
      }
      const DetElement::Children& c = de.children();
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i)
        collect((*i).second, slice, level+1);
      return 1;
    }
    /// Initial collector call
    long collect(ConditionsManager manager, const IOV& iov)  {
      shared_ptr<ConditionsSlice> slice(new ConditionsSlice(manager,content));
      Conditions::fill_content(manager,*content,*iov.iovType);

      manager.prepare(iov, *slice);
      int res = collect(lcdd.world(), *slice, 0);
      return res;
    }
    /// Compute dependent alignment conditions
    int computeDependencies(shared_ptr<ConditionsSlice>& slice,
                            ConditionsManager manager,
                            const IOV& iov)
    {
      AlignmentsCalculator align;
      AlignmentsCalculator::Deltas align_deltas;
      slice.reset(new ConditionsSlice(manager,content));
      TTimeStamp acc_start;
      ConditionsManager::Result cres = manager.prepare(iov, *slice);
      TTimeStamp acc_stop;
      acc_stat.Fill(acc_stop.AsDouble()-acc_start.AsDouble());
      throw "TODO: select deltas from conditions";
      TTimeStamp comp_start;
      AlignmentsCalculator::Result ares = align.compute(align_deltas,*slice);
      TTimeStamp comp_stop;
      comp_stat.Fill(comp_stop.AsDouble()-comp_start.AsDouble());
      printout(INFO,"DDDBAlign",
               "++ AlignmentManager: %7ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
               cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, 
               ares.computed, ares.missing, iov.str().c_str());
      return 1;
    }
    /// Access dependent alignment conditions from DetElement object using global and local keys
    int access(ConditionsManager manager,const IOV& iov)  {
      shared_ptr<ConditionsSlice> slice;
      int ret = computeDependencies(slice, manager, iov);
      if ( ret == 1 )  {
        int count = 0;
        throw "Fix-me!!!";
#if 0
        for(Deps::const_iterator i=deps.begin(); i!=deps.end(); ++i)   {
          const ConditionDependency* d = (*i).second.get();
          if ( d->detector.hasAlignments() )   {
            Alignments::DetAlign     det(d->detector);
            const ConditionKey&      k = d->target;
            Alignments::Container    c = det.alignments();
            {
              Alignments::Alignment    a = c.get(k.hash,*slice->pool);
              const Alignments::Delta& D = a.data().delta;
              printout(m_level,"Alignment","++ [%16llX] (%11s-%8s-%5s) Cond:%p '%s'", k.hash,
                       D.hasTranslation() ? "Translation" : "",
                       D.hasRotation() ? "Rotation" : "",
                       D.hasPivot() ? "Pivot" : "",
                       a.data().hasCondition() ? a.data().condition.ptr() : 0,
                       k.name.c_str());
              ++count;
              ++m_accessCount;
            }
            {
              Alignments::Alignment    a = c.get("Alignment",*slice->pool);
              const Alignments::Delta& D = a.data().delta;
              printout(m_level,"Alignment","++ [%16llX] (%11s-%8s-%5s) Cond:%p 'Alignment'", k.hash,
                       D.hasTranslation() ? "Translation" : "",
                       D.hasRotation() ? "Rotation" : "",
                       D.hasPivot() ? "Pivot" : "",
                       a.data().hasCondition() ? a.data().condition.ptr() : 0);
              ++count;
              ++m_accessCount;              
            }
          }
        }
#endif
        printout(INFO,m_name,"++ Accessed %d conditions from the DetElement objects.",count);
      }
      return ret;
    }
  };
}

//==========================================================================
namespace  {

  /// Plugin function: Load dependent alignment conditions according to time stamps.
  long dddb_derived_alignments(LCDD& lcdd, int argc, char** argv) {
    int turns = 1;
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
      else if ( ::strcmp(argv[i],"-turns")==0 )  {
        turns = ::atol(argv[++i]);
      }
      else if ( ::strcmp(argv[i],"--help")==0 )      {
        printout(level,"Plugin-Help","Usage: DDDB_DerivedAlignmentsTest --opt [--opt]        ");
        printout(level,"Plugin-Help","  -time  <string>     Set event time Format: \"%%d-%%m-%%Y %%H:%%M:%%S\"");
        printout(level,"Plugin-Help","  -print <value>      Printlevel for output      ");
        printout(level,"Plugin-Help","  -help               Print this help message    ");
        ::exit(EINVAL);
      }
    }
    int ret;
    {
      AlignmentSelector selec(lcdd, level);
      ConditionsManager manager(ConditionsManager::from(lcdd));
      TStatistic        cr_stat("Initialize"), re_acc_stat("Reaccess");
      const IOVType*    iovType = manager.iovType("epoch");
      {
        TTimeStamp start;
        IOV  iov(iovType, time);
        ret = selec.collect(manager,iov);
        TTimeStamp stop;
        cr_stat.Fill(stop.AsDouble()-start.AsDouble());
      }
      if ( ret == 1 )  {
        for(int i=0; i<turns; ++i)  {  {
            IOV  iov(iovType, time + (i+1)*3600);
            shared_ptr<ConditionsSlice> slice1, slice2;
            ret = selec.computeDependencies(slice1,manager,iov);
            slice2.reset(new ConditionsSlice(manager,selec.content));
            TTimeStamp start;
            ConditionsManager::Result cres = manager.prepare(iov, *slice2);
            TTimeStamp stop;
            re_acc_stat.Fill(stop.AsDouble()-start.AsDouble());
            printout(INFO,"DDDBAlign",
                     "++ REACCESS:         %7ld conditions (S:%ld,L:%ld,C:%ld,M:%ld)             for IOV:%-12s",
                     cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, iov.str().c_str());
          }
        }
      }
      printout(INFO,"Statistics","+======= Summary: # of Runs: %3d ==========================================",turns);
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               cr_stat.GetName(), cr_stat.GetMean(), cr_stat.GetMeanErr(), cr_stat.GetRMS(), cr_stat.GetN());
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               selec.acc_stat.GetName(), selec.acc_stat.GetMean(), selec.acc_stat.GetMeanErr(),
               selec.acc_stat.GetRMS(), selec.acc_stat.GetN());
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               selec.comp_stat.GetName(), selec.comp_stat.GetMean(), selec.comp_stat.GetMeanErr(),
               selec.comp_stat.GetRMS(), selec.comp_stat.GetN());
      printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
               re_acc_stat.GetName(), re_acc_stat.GetMean(), re_acc_stat.GetMeanErr(), re_acc_stat.GetRMS(), re_acc_stat.GetN());
      printout(INFO,"Statistics","+=========================================================================");
    }
    InstanceCount::dump();
    return ret;
  }
}   /* End anonymous namespace  */
DECLARE_APPLY(DDDB_DerivedAlignmentsTest,dddb_derived_alignments)
//==========================================================================

namespace  {
  /// Plugin function: Access dependent alignment conditions from DetElement object using global and local keys
  long dddb_access_alignments(LCDD& lcdd, int argc, char** argv) {
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
        printout(level,"Plugin-Help","Usage: DDDB_AlignmentsAccessTest --opt [--opt]        ");
        printout(level,"Plugin-Help","  -time  <string>     Set event time Format: \"%%d-%%m-%%Y %%H:%%M:%%S\"");
        printout(level,"Plugin-Help","  -print <value>      Printlevel for output      ");
        printout(level,"Plugin-Help","  -help               Print this help message    ");
        ::exit(EINVAL);
      }
    }
    AlignmentSelector selec(lcdd,level);
    ConditionsManager manager(ConditionsManager::from(lcdd));
    const IOVType* iovType = manager.iovType("epoch");
    IOV  iov(iovType, time);
    int ret = selec.collect(manager,iov);
    if ( ret == 1 )  {
      ret = selec.access(manager,iov);
    }
    return ret;
  }
}   /* End anonymous namespace  */
DECLARE_APPLY(DDDB_AlignmentsAccessTest,dddb_access_alignments)
//==========================================================================

