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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/DetectorProcessor.h"

#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsCleanup.h"
#include "DDCond/ConditionsManager.h"

#include "DDDB/DDDBHelper.h"
#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBConversion.h"

#include "Detector/DeVelo.h"
#include "Detector/DeAlignmentCall.h"
#include "Detector/DeVeloConditionCalls.h"

using namespace std;
using namespace gaudi;


/// Anonymous namespace for plugins
namespace {
  dd4hep::PrintLevel s_PrintLevel = INFO;
}

//==========================================================================
/// Anonymous namespace for plugins
namespace {

  /// Basic entry point to print out the detector element hierarchy
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  long dump_det(dd4hep::Detector& description, int argc, char** argv) {

    /// Callback object to print selective information
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    struct DumpActor {
      dd4hep::Detector&                             m_detDesc;
      dd4hep::DetElement                            m_de;
      unique_ptr<VeloUpdateContext>                 m_context;
      dd4hep::cond::ConditionsManager               m_manager;
      shared_ptr<dd4hep::cond::ConditionsContent>   m_content;
      const dd4hep::IOVType*                        m_iovtype = 0;
      dd4hep::DDDB::DDDBReader*                     m_reader = 0;

      /// Configure file based reader
      void configReader(long iov_start, long iov_end)   {
        try   {
          dd4hep::IOV iov_range(m_iovtype);
          iov_range.keyData.first  = iov_start;
          iov_range.keyData.second = iov_end;
          m_reader->property("ValidityLower").set(iov_range.keyData.first);
          m_reader->property("ValidityUpper").set(iov_range.keyData.second);
          printout(INFO,"ConditionsManager","+++ Configure file based reader for IOV:%s", 
                   iov_range.str().c_str());
        }
        catch(...)  {
        }
      }


      /// Standard constructor
      DumpActor(dd4hep::Detector& dsc, const string& path) : m_detDesc(dsc)     {
        dd4hep::DDDB::DDDBHelper* helper = dsc.extension<dd4hep::DDDB::DDDBHelper>(false);
        shared_ptr<dd4hep::cond::ConditionsSlice>   slice;
        shared_ptr<dd4hep::cond::ConditionsContent> cont;
        vector<pair<dd4hep::DetElement, int> >      elts;

        m_de      = dd4hep::detail::tools::findElement(m_detDesc, path);
        m_manager = dd4hep::cond::ConditionsManager::from(m_detDesc);
        m_iovtype = m_manager.iovType("epoch");
        m_reader  = helper->reader<dd4hep::DDDB::DDDBReader>();
        configReader(dd4hep::detail::makeTime(2008,1,1), dd4hep::detail::makeTime(2018,12,31));

        dd4hep::IOV iov(m_iovtype, 0);
        cont.reset(new dd4hep::cond::ConditionsContent());
        slice.reset(new dd4hep::cond::ConditionsSlice(m_manager,cont));
        dd4hep::cond::fill_content(m_manager, *cont, *m_iovtype);
        dd4hep::cond::ConditionsManager::Result res = m_manager.prepare(iov, *slice);
        printout(dd4hep::ALWAYS,"ConditionsManager","Total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of IOV %s",
                 res.total(), res.selected, res.loaded, res.computed, res.missing, iov.str().c_str());

        m_context.reset(new VeloUpdateContext());
        m_content.reset(new dd4hep::cond::ConditionsContent());
        dd4hep::DetectorScanner(dd4hep::detElementsCollector(elts), m_de);    
        dd4hep::cond::DependencyBuilder align_builder(m_detDesc.world(),
                                                      Keys::alignmentsComputedKey,
                                                      new DeAlignmentCall(m_de));
        auto* dep = align_builder.release();
        dep->target.hash = Keys::alignmentsComputedKey;
        m_content->insertDependency(dep);

        std::unique_ptr<DeVeloStaticConditionCall> static_update(new DeVeloStaticConditionCall());
        for(const auto& e : elts)   {
          dd4hep::DetElement de = e.first;
          dd4hep::DDDB::DDDBCatalog* cat = de.extension<dd4hep::DDDB::DDDBCatalog>();
          dd4hep::Condition::detkey_type det_key = de.key();
          dd4hep::ConditionKey::KeyMaker lower(det_key, dd4hep::Condition::FIRST_ITEM_KEY);
          dd4hep::ConditionKey::KeyMaker upper(det_key, dd4hep::Condition::LAST_ITEM_KEY);
          cout << "Processing " << e.second << " class " << cat->classID << "  -> " << de.path() << endl;
          m_context->detectors.insert(make_pair(det_key,make_pair(de,cat)));
          {
            auto first = cont->conditions().lower_bound(lower.hash);
            for(; first != cont->conditions().end() && (*first).first <= upper.hash; ++first)  {
              std::unique_ptr<dd4hep::cond::ConditionsLoadInfo> ptr((*first).second->clone());
              m_content->insertKey((*first).first, ptr);
            }
          }
          {
            auto first = cont->derived().lower_bound(lower.hash);
            for(; first != cont->derived().end() && (*first).first <= upper.hash; ++first)
              m_content->insertDependency((*first).second);
          }

          dd4hep::cond::DependencyBuilder static_builder(de, Keys::staticKey, static_update->addRef());
          m_content->insertDependency(static_builder.release());

          dd4hep::cond::ConditionUpdateCall* call = (e.first == m_de)
            ? new DeVeloConditionCall(de, cat, m_context.get())
            : new DeVeloIOVConditionCall(de, cat, m_context.get());
          dd4hep::cond::DependencyBuilder iov_builder(de, Keys::deKey, call);
          m_content->insertDependency(iov_builder.release());
        }
        static_update.release()->release();
        m_manager.clear();
      }

      /// Standard destructor
      ~DumpActor()  {
      }

      /// __________________________________________________________________________________
      long dump()  {
        shared_ptr<dd4hep::cond::ConditionsSlice> slice;
        dd4hep::cond::ConditionsIOVPool* iovp = m_manager.iovPool(*m_iovtype);
        
        printout(INFO,"ConditionsManager","+++ Dump pools at dump:");
        for(const auto& e : iovp->elements)
          e.second->print();
        printout(INFO,"ConditionsManager","+++ Starting conditions dump loop");
        long start_time = dd4hep::detail::makeTime(2016,5,20,0,0,0);
        for(size_t i=0; i<10; ++i)   {
          long stamp = start_time + i*3600 + 1800; // Middle of 1 hour run
          dd4hep::IOV iov(m_iovtype, stamp);
          configReader(stamp-1800, stamp+1799);    // Run duration 1 hour - 1 second
          slice.reset(new dd4hep::cond::ConditionsSlice(m_manager, m_content));
          m_context->alignments_done = dd4hep::Condition();
          dd4hep::cond::ConditionsManager::Result res = m_manager.prepare(iov, *slice, m_context.get());
          printout(dd4hep::ALWAYS,"ConditionsManager","Total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of IOV %s",
                   res.total(), res.selected, res.loaded, res.computed, res.missing, iov.str().c_str());
          for(const auto& e : iovp->elements)
            e.second->print();
          DeVelo devp = slice->get(m_de,Keys::deKey);
          devp.print(0,DePrint::ALL);

          DetectorElement<DeVelo> detVP = devp;
          cout << "TEST: Got detector element:"
               << " " << detVP.conditions().size()
               << " " << detVP.detector().path()
               << " " << detVP.detectorAlignment().name()
               << " " << detVP.conditions().size()
               << endl;
          try  {
            DeVeloGeneric deGEN = devp;
            cout << " " << deGEN.conditions().size()
                 << " " << deGEN.children().size()
                 << endl;
          }
          catch(const exception& e)  {
            cout << "Exception(This is GOOD!): " << e.what() << endl;
          }
        }
        m_manager.clean(dd4hep::cond::ConditionsFullCleanup());
        printout(dd4hep::ALWAYS,"ServiceTest","Test finished....");
        return 1;
      }
    };

    dd4hep::setPrintFormat("%-18s %5s %s");
    for(int i=0; i<argc; ++i)  {
      if ( ::strcmp(argv[i],"-print")==0 )  {
        s_PrintLevel = dd4hep::printLevel(argv[++i]);
        printout(INFO,"DDDB","Setting print level for %s to %s [%d]",__FILE__,argv[i-1],s_PrintLevel);
      }
      else if ( ::strcmp(argv[i],"--help")==0 )      {
        printout(INFO,"Plugin-Help","Usage: DDDBDetectorDump --opt [--opt]            ");
        printout(INFO,"Plugin-Help","  -print <value>      Printlevel for output      ");
        printout(INFO,"Plugin-Help","  -help               Print this help message    ");
        return 0;
      }
    }
    DumpActor actor(description, "/world/LHCb/BeforeMagnetRegion/Velo");
    return actor.dump();
  }
} /* End anonymous namespace  */

using namespace dd4hep;
DECLARE_APPLY(DDDB_DeVeloTest,dump_det)
//==========================================================================
