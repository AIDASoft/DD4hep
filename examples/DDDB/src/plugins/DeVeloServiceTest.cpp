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
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"

#include "DDDB/DDDBHelper.h"
#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBConversion.h"

#include "DetService.h"
#include "Detector/DeVelo.h"
#include "Detector/DeAlignmentCall.h"
#include "Detector/DeVeloConditionCalls.h"

using namespace std;
using namespace gaudi;


// C/C++ include files


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
      dd4hep::DetElement                            m_de;
      unique_ptr<VeloUpdateContext>                 m_context;
      
      dd4hep::DDDB::DDDBReader*                     m_reader = 0;
      shared_ptr<IDetService>                       m_service;
      
      /// Configure file based reader
      void configReader(long iov_start, long iov_end)   {
        try   {
          const IDetService::IOVType* iov_typ = m_service->iovType("epoch");
          dd4hep::IOV iov_range(iov_typ);
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
      DumpActor(dd4hep::Detector& dsc, const string& path)   {
        IDetService::Slice                     slice;
        IDetService::Content                   cont;
        vector<pair<dd4hep::DetElement, int> > elts;
        dd4hep::DDDB::DDDBHelper* helper = dsc.extension<dd4hep::DDDB::DDDBHelper>(false);
        dd4hep::cond::ConditionsManager manager = dd4hep::cond::ConditionsManager(dsc);

        m_de     = dd4hep::detail::tools::findElement(dsc, path);
        m_reader = helper->reader<dd4hep::DDDB::DDDBReader>();
        m_service.reset(new DetService(manager));
        const IDetService::IOVType* iov_typ = m_service->iovType("epoch");
        dd4hep::IOV                 iov(iov_typ,dd4hep::detail::makeTime(2018,1,1));
        IDetService::Content        content = m_service->openContent("DDDB");
        configReader(dd4hep::detail::makeTime(2008,1,1), dd4hep::detail::makeTime(2018,12,31));

        cont.reset(new dd4hep::cond::ConditionsContent());
        slice.reset(new dd4hep::cond::ConditionsSlice(manager,cont));
        dd4hep::cond::fill_content(manager, *cont, *iov_typ);
        dd4hep::cond::ConditionsManager::Result res = manager.prepare(iov, *slice);
        printout(dd4hep::ALWAYS,"Prepare","Total %ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) of IOV %s",
                 res.total(), res.selected, res.loaded, res.computed, res.missing, iov.str().c_str());

        m_context.reset(new VeloUpdateContext());
        dd4hep::DetectorScanner(dd4hep::detElementsCollector(elts), m_de);    
        dd4hep::cond::DependencyBuilder align_builder(dsc.world(),
                                                      Keys::alignmentsComputedKey,
                                                      make_shared<DeAlignmentCall>(m_de));
        auto* dep = align_builder.release();
        dep->target.hash = Keys::alignmentsComputedKey;
        m_service->addContent(content, dep);

        auto static_update = make_shared<DeVeloStaticConditionCall>();
        for(const auto& e : elts)   {
          dd4hep::DetElement de = e.first;
          dd4hep::DDDB::DDDBCatalog* cat = de.extension<dd4hep::DDDB::DDDBCatalog>();
          dd4hep::Condition::detkey_type det_key = de.key();
          dd4hep::ConditionKey::KeyMaker lower(det_key, dd4hep::Condition::FIRST_ITEM_KEY);
          dd4hep::ConditionKey::KeyMaker upper(det_key, dd4hep::Condition::LAST_ITEM_KEY);
          printout(DEBUG, "ServiceTest","Processing %ld class %d  -> %s",
                   e.second, cat->classID, de.path().c_str());
          m_context->detectors.insert(make_pair(det_key,make_pair(de,cat)));
          {
            auto first = cont->conditions().lower_bound(lower.hash);
            for(; first != cont->conditions().end() && (*first).first <= upper.hash; ++first)  {
              dd4hep::cond::ConditionsLoadInfo* ptr = (*first).second->addRef();
              m_service->addContent(content, (*first).first, *ptr->data<string>());
            }
          }
          {
            auto first = cont->derived().lower_bound(lower.hash);
            for(; first != cont->derived().end() && (*first).first <= upper.hash; ++first)   {
              m_service->addContent(content, (*first).second);
            }
          }

          dd4hep::cond::DependencyBuilder static_builder(de, Keys::staticKey, static_update);
          m_service->addContent(content, static_builder.release());

          shared_ptr<dd4hep::cond::ConditionUpdateCall> call = ( e.first == m_de )
            ? make_shared<DeVeloConditionCall>(de, cat, m_context.get())
            : make_shared<DeVeloIOVConditionCall>(de, cat, m_context.get());
          dd4hep::cond::DependencyBuilder iov_builder(de, Keys::deKey, call);
          m_service->addContent(content, iov_builder.release());
        }
        m_service->closeContent(content);
        manager.clear();
      }

      /// Standard destructor
      ~DumpActor()  {
      }

      /// __________________________________________________________________________________
      long dump()  {
        size_t num_round = 10;
        long   daq_start = dd4hep::detail::makeTime(2016,5,20,0,0,0);
        shared_ptr<dd4hep::cond::ConditionsSlice> slice;
        const IDetService::IOVType* iov_typ = m_service->iovType("epoch");

        printout(INFO,"ConditionsManager","+++ Starting conditions dump loop");
        for(size_t i=0; i<num_round; ++i)   {
          long event_stamp = daq_start + (i*3600) + 1800;
          dd4hep::IOV iov(iov_typ, event_stamp);
          configReader(event_stamp-1800, event_stamp+1799);    // Run duration 1 hour - 1 second
          // Reset context. Need at some point a better mechanism
          m_context->alignments_done = dd4hep::Condition();
          /// The next line is what would show up in the client code:
          slice = m_service->project("DDDB", m_context.get(), "epoch", event_stamp);
          printout(INFO,"ConditionsManager","+++ Prepared slice Round: %ld for IOV:%s", 
                   i, slice->iov().str().c_str());

          DeVelo devp = slice->get(m_de,Keys::deKey);
          //devp.print(0,DePrint::ALL);
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
        m_service->cleanup(dd4hep::cond::ConditionsFullCleanup());
        printout(dd4hep::ALWAYS,"TestSummary",
                 "Total %ld slices created and accessed during the test.", num_round);

        printout(dd4hep::ALWAYS,"ServiceTest","Test finished....");
        return 1;
      }
    };

    dd4hep::setPrintFormat("%-18s %5s %s");
    for(int i=0; i<argc; ++i)  {
      if ( ::strcmp(argv[i],"-print")==0 )  {
        s_PrintLevel = dd4hep::printLevel(argv[++i]);
        printout(INFO,"ServiceTest","Setting print level for %s to %s [%d]",__FILE__,argv[i-1],s_PrintLevel);
      }
      else if ( ::strcmp(argv[i],"--help")==0 )      {
        printout(INFO,"Plugin-Help","Usage: DDDB_DeVeloServiceTest --opt [--opt]            ");
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
DECLARE_APPLY(DDDB_DeVeloServiceTest,dump_det)
//==========================================================================
