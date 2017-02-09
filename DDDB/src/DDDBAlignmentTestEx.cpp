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
#include "DD4hep/DetAlign.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DDCond/ConditionsSlice.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DDAlign/AlignmentsManager.h"

#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBAlignmentUpdateCall.h"

#include "TStatistic.h"
#include "TTimeStamp.h"

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
    PrintLevel           m_level = INFO;
    long                 m_installCount = 0;
    long                 m_accessCount = 0;
    TStatistic           acc_stat, comp_stat;

    struct UCall : public Alignments::AlignmentsUpdateCall  {
      AlignmentsManager manager;
      UCall(AlignmentsManager m) : manager(m) {}
      virtual ~UCall() = default;
      Condition operator()(const ConditionKey& key, const UpdateContext& context)
      {
        Condition        cond = context.condition(0);
        AbstractMap&     src  = cond.get<AbstractMap>();
        OpaqueDataBlock& par  = src.firstParam().second;
        DetElement       det  = context.dependency.detector;
        printout(DEBUG,"AlignmentUpdate","++ Building dependent condition: %s Detector [%d]: %s ",
                 key.name.c_str(), det.level(), det.path().c_str());
        if ( par.typeInfo() == typeid(Data::Delta) )  {
          const Data::Delta& delta = src.first<Data::Delta>();
          return AlignmentsUpdateCall::handle(key, context, cond.key(), delta);
        }
        // Somehow the condition is not of type Data::Delta. This is an ERROR.
        // Here only print and return an empty alignment condition.
        // Otherwise the return is not accepted!
        // TODO: To be decided how to handle this error
        Alignments::AlignmentCondition target(key.name);
        Data& data = target.data();
        data.detector = det;
        printout(INFO,"AlignmentUpdate","++ Failed to access alignment-Delta from %s",
                 cond->value.c_str());
        ConditionsPrinter("AlignmentUpdate")(cond);
        return target;
      }
    };

    /// Initializing constructor
    AlignmentSelector(LCDD& l, PrintLevel p, AlignmentsManager m)
      : lcdd(l), m_name("DDDBAlignments"), m_level(p), acc_stat("Access"), comp_stat("Compute")   {
      // The alignment update call can be re-used over and over. It has not state.
      updateCall = new UCall(m);
    }
    /// Default destructor
    virtual ~AlignmentSelector()   {
      printout(INFO,m_name,"++ Installed alignment calls:     %ld", m_installCount);
      printout(INFO,m_name,"++ Number of accessed alignments: %ld", m_accessCount);
      releasePtr(updateCall);
    }
    /// Recursive alignment collector
    long collect(DetElement de, dd4hep_ptr<UserPool>& user_pool, ConditionsSlice& slice, int level)
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
            DependencyBuilder b(k, updateCall);
            b->detector = de;
            b.add(ConditionKey(cond->value));
            slice.insert(b.release());
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
        collect((*i).second, user_pool, slice, level+1);
      return 1;
    }
    /// Initial collector call
    long collect(ConditionsManager manager, const IOV& iov)  {
      dd4hep_ptr<ConditionsSlice> slice(createSlice(manager,*iov.iovType));
      manager.prepare(iov, *slice);
      int res = collect(lcdd.world(), slice->pool, *slice, 0);
      return res;
    }
    /// Compute dependent alignment conditions
    int computeDependencies(dd4hep_ptr<ConditionsSlice>& slice,
                            ConditionsManager conds,
                            AlignmentsManager align,
                            const IOV& iov)
    {
      slice.adopt(createSlice(conds,*iov.iovType));
      TTimeStamp acc_start;
      ConditionsManager::Result cres = conds.prepare(iov, *slice);
      TTimeStamp acc_stop;
      acc_stat.Fill(acc_stop.AsDouble()-acc_start.AsDouble());
      TTimeStamp comp_start;
      AlignmentsManager::Result ares = align.compute(*slice);
      TTimeStamp comp_stop;
      comp_stat.Fill(comp_stop.AsDouble()-comp_start.AsDouble());
      printout(INFO,"DDDBAlign",
               "++ AlignmentManager: %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
               cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, 
               ares.computed, ares.missing, iov.str().c_str());
      return 1;
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
    AlignmentsManager align(AlignmentsManager::from(lcdd));
    ConditionsManager conds(ConditionsManager::from(lcdd));
    AlignmentSelector selec(lcdd, level, align);
    TStatistic cr_stat("Initialize"), re_acc_stat("Reaccess");
    const IOVType* iovType = conds.iovType("epoch");
    int ret;
    {
      TTimeStamp start;
      IOV  iov(iovType, time);
      ret = selec.collect(conds,iov);
      TTimeStamp stop;
      cr_stat.Fill(stop.AsDouble()-start.AsDouble());
    }
    if ( ret == 1 )  {
      for(int i=0; i<turns; ++i)  {  {
          long ti = time + (i+1)*3600;
          IOV  iov(iovType, ti);
          dd4hep_ptr<ConditionsSlice> slice;
          ret = selec.computeDependencies(slice,conds,align,iov);
          slice->reset();
          dd4hep_ptr<ConditionsSlice> slice2;
          TTimeStamp start;
          slice2.adopt(createSlice(conds,*iov.iovType));
          conds.prepare(iov, *slice2);
          TTimeStamp stop;
          re_acc_stat.Fill(stop.AsDouble()-start.AsDouble());
        }
        DD4hep::InstanceCount::dump();
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
    return ret;
  }
}   /* End anonymous namespace  */
DECLARE_APPLY(DDDB_DerivedAlignmentsTestEx,dddb_derived_alignments)
//==========================================================================
