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
#include "DD4hep/ConditionsData.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/AlignmentsProcessor.h"
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/detail/DetectorInterna.h"

#include "DDCond/ConditionsOperators.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsSlice.h"

// C/C++ include files
#include <memory>

using namespace std;
using namespace dd4hep;

#include "Kernel/VPConstants.h"
#include "Detector/DeVP.h"

/// Anonymous namespace for plugins
namespace {
  PrintLevel s_PrintLevel = INFO;
}

//==========================================================================
/// Anonymous namespace for plugins
namespace {

  using namespace dd4hep::cond;

  /// Basic entry point to print out the detector element hierarchy
  /**
   *  \author  M.Frank
   *  \version 1.0
   *  \date    01/04/2014
   */
  long dump_det_tree(Detector& description, int argc, char** argv) {

    using DDDB::DDDBCatalog;

    /// Callback object to print selective information
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2014
     */
    struct DumpActor {
      /// Container with all known conditions
      vector<pair<int,Condition> > m_allConditions;
      shared_ptr<ConditionsSlice>  m_slice;
      ConditionsManager            m_manager;
      string                       m_name;
      Detector&                    m_detDesc;

      /// Standard constructor
      DumpActor(Detector& l)
        : m_detDesc(l)
      {
        m_manager = ConditionsManager::from(m_detDesc);
        m_slice.reset(new ConditionsSlice(m_manager,shared_ptr<ConditionsContent>(new ConditionsContent())));
      }

      /// Standard destructor
      ~DumpActor()  {
      }

      /// __________________________________________________________________________________
      void printDetElement(int , DetElement )      {
      }
      long dump(DetElement de, int level)  {
        char fmt[64], text[512];
        const DetElement::Children& c = de.children();
        ::snprintf(fmt,sizeof(fmt),"%%-%ds-> ",2*level+5);
        ::snprintf(text,sizeof(text),fmt,"");
        try  {
          printDetElement(level, de);
        }
        catch(const exception& e)  {
          ::sprintf(fmt,"%03d %%-%ds WARNING from: %%s %%-20s %%s",level+1,2*level+3);
          printout(INFO, m_name, fmt, "", de.path().c_str(), "[NO CATALOG availible]",e.what());
        }
        catch(...)  {
          ::sprintf(fmt,"%03d %%-%ds WARNING from: %%s %%-20s",level+1,2*level+3);
          printout(INFO, m_name, fmt, "", de.path().c_str(), "[NO CATALOG availible]");
        }
        for (const auto& i : c)
          dump(i.second,level+1);
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
    DumpActor actor(description);
    return actor.dump(description.world(), 0);
  }
  
  template <int flag> long dump_detelement_tree(Detector& description, int argc, char** argv)
  {  return dump_det_tree(description,argc,argv);    }
} /* End anonymous namespace  */

DECLARE_APPLY(DDDB_DeVeloTest,dump_detelement_tree<0>)
//==========================================================================
