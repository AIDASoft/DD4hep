//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-02-02
//  \version 1.0
//
//==========================================================================
#ifndef DD4HEP_CONDITIONS_CONDIITONSSNAPSHOTROOTLOADER_H
#define DD4HEP_CONDITIONS_CONDIITONSSNAPSHOTROOTLOADER_H

// Framework include files
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsRootPersistency.h"
#include "DD4hep/Printout.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond  {

    /// Implementation of a stack of conditions assembled before application
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionsSnapshotRootLoader : public ConditionsDataLoader   {
      std::vector<ConditionsRootPersistency*> buffers;
      void load_source  (const std::string& nam);
    public:
      /// Default constructor
      ConditionsSnapshotRootLoader(Detector& description, ConditionsManager mgr, const std::string& nam);
      /// Default destructor
      virtual ~ConditionsSnapshotRootLoader();
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_single(key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range( key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Optimized update using conditions slice data
      virtual size_t load_many(  const IOV& /* req_validity */,
                                 RequiredItems&  /* work         */,
                                 LoadedItems&    /* loaded       */,
                                 IOV&       /* conditions_validity */)
      {
        except("ConditionsLoader","+++ update: Invalid call!");
        return 0;
      }
    };
  }    /* End namespace cond                             */
}      /* End namespace dd4hep                            */
#endif /* DD4HEP_CONDITIONS_CONDIITONSSNAPSHOTROOTLOADER_H  */

//#include "ConditionsSnapshotRootLoader.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/detail/ConditionsInterna.h"

#include "TFile.h"

// C/C++ include files
#include <string>

// Forward declartions
using std::string;
using namespace dd4hep;
using namespace dd4hep::cond;

namespace {
  void* create_loader(Detector& description, int argc, char** argv)   {
    const char* name = argc>0 ? argv[0] : "XMLLoader";
    ConditionsManagerObject* mgr = (ConditionsManagerObject*)(argc>0 ? argv[1] : 0);
    return new ConditionsSnapshotRootLoader(description,ConditionsManager(mgr),name);
  }
}
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_Conditions_root_snapshot_Loader,create_loader)

/// Standard constructor, initializes variables
ConditionsSnapshotRootLoader::ConditionsSnapshotRootLoader(Detector& description, ConditionsManager mgr, const std::string& nam) 
: ConditionsDataLoader(description, mgr, nam)
{
}

/// Default Destructor
ConditionsSnapshotRootLoader::~ConditionsSnapshotRootLoader() {
  buffers.clear();
} 

void ConditionsSnapshotRootLoader::load_source(const std::string& nam)  {
  TFile* f = TFile::Open(nam.c_str());
  std::unique_ptr<ConditionsRootPersistency> p =
    ConditionsRootPersistency::load(f,"Conditions");
  buffers.push_back(p.release());
}

size_t ConditionsSnapshotRootLoader::load_single(key_type key,
                                                 const IOV& req_validity,
                                                 RangeConditions& conditions)
{
  size_t len = conditions.size();
  for(const auto& s : m_sources )
    load_source(s.first);

  m_sources.clear();
  return 0;
}

size_t ConditionsSnapshotRootLoader::load_range(key_type key,
                                                const IOV& req_validity,
                                                RangeConditions& conditions)
{
  for(const auto& s : m_sources )
    load_source(s.first);
  m_sources.clear();
  return 0;
}

