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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/DetectorProcessor.h"

#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsSlice.h"
#include "DDDB/DDDBConversion.h"

// C/C++ include files
//#include <memory>

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

      /// Standard constructor
      DumpActor(dd4hep::Detector& l, const string& path) : m_detDesc(l)     {
        shared_ptr<dd4hep::cond::ConditionsSlice>   slice;
        shared_ptr<dd4hep::cond::ConditionsContent> cont;
        vector<pair<dd4hep::DetElement, int> >      elts;
        m_manager = dd4hep::cond::ConditionsManager::from(m_detDesc);
        m_de = dd4hep::detail::tools::findElement(m_detDesc, path);
        m_iovtype = m_manager.iovType("epoch");

        dd4hep::IOV iov(m_iovtype, 0);

        cont.reset(new dd4hep::cond::ConditionsContent());
        slice.reset(new dd4hep::cond::ConditionsSlice(m_manager,cont));
        dd4hep::cond::fill_content(m_manager, *cont, *m_iovtype);
        m_manager.prepare(iov, *slice);

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
          dd4hep::ConditionKey::KeyMaker lower(det_key, 0);
          dd4hep::ConditionKey::KeyMaker upper(det_key, ~0x0);
          cout << "Processing " << e.second << " class " << cat->classID << "  -> " << de.path() << endl;
          m_context->detectors.insert(make_pair(det_key,make_pair(de,cat)));
          {
            auto first = cont->conditions().lower_bound(lower.hash);
            for(; first != cont->conditions().end() && (*first).first <= upper.hash; ++first)
              m_content->insertKey((*first).first);
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
      }

      /// Standard destructor
      ~DumpActor()  {
      }

      /// __________________________________________________________________________________
      long dump()  {
        shared_ptr<dd4hep::cond::ConditionsSlice> slice;
        dd4hep::IOV iov(m_iovtype, 0);

        slice.reset(new dd4hep::cond::ConditionsSlice(m_manager, m_content));
        m_manager.prepare(iov, *slice, m_context.get());
        /*
        dd4hep::cond::ConditionsPrinter printer(slice.get());
        printer.lineLength = 180;
        dd4hep::DetectorScanner(printer, m_de);
        */
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
        return 1;
      }
    };

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
