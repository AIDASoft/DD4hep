//==========================================================================
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
#ifndef DD4HEP_CONDITIONS_MULTICONDITIONSLOADER_H
#define DD4HEP_CONDITIONS_MULTICONDITIONSLOADER_H

// Framework include files
#include <DDCond/ConditionsDataLoader.h>
#include <DD4hep/Printout.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond  {

    // Forward declarations
    class ConditionsHandler;

    /// Implementation of a stack of conditions assembled before application
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionsMultiLoader : public ConditionsDataLoader   {
      typedef std::map<std::string, ConditionsDataLoader*> Loaders;
      typedef std::map<std::string, ConditionsDataLoader*> OpenSources;

      Loaders m_loaders;
      OpenSources m_openSources;
      ConditionsDataLoader* load_source(const std::string& nam,const IOV& req_validity);

    public:
      /// Default constructor
      ConditionsMultiLoader(Detector& description, ConditionsManager mgr, const std::string& nam);
      /// Default destructor
      virtual ~ConditionsMultiLoader();
#if 0
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_single(key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range( key_type key,
                                 const IOV& req_validity,
                                 RangeConditions& conditions);
#endif
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
  }     /* End namespace detail                     */
}       /* End namespace dd4hep                       */
#endif  /* DD4HEP_CONDITIONS_MULTICONDITIONSLOADER_H   */

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

// Framework include files
//#include <ConditionsMultiLoader.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Factories.h>
#include <DD4hep/PluginCreators.h>
#include <DDCond/ConditionsManager.h>

// Forward declarations
using namespace dd4hep::cond;

namespace {
  void* create_loader(dd4hep::Detector& description, int argc, char** argv)   {
    const char* name = argc>0 ? argv[0] : "MULTILoader";
    ConditionsManager::Object* mgr = (ConditionsManager::Object*)(argc>0 ? argv[1] : 0);
    return new ConditionsMultiLoader(description,ConditionsManager(mgr),name);
  }
}
DECLARE_DD4HEP_CONSTRUCTOR(DD4hep_Conditions_multi_Loader,create_loader)

/// Standard constructor, initializes variables
ConditionsMultiLoader::ConditionsMultiLoader(Detector& description, ConditionsManager mgr, const std::string& nam) 
: ConditionsDataLoader(description, mgr, nam)
{
}

/// Default Destructor
ConditionsMultiLoader::~ConditionsMultiLoader() {
} 

ConditionsDataLoader* 
ConditionsMultiLoader::load_source(const std::string& nam,
                                   const IOV& req_validity)
{
  OpenSources::iterator iop = m_openSources.find(nam);
  if ( iop == m_openSources.end() )  {
    size_t idx = nam.find(':');
    if ( idx == std::string::npos )   {
      except("ConditionsMultiLoader","Invalid data source specification: "+nam);
    }
    std::string ident = nam.substr(0,idx);
    Loaders::iterator ild = m_loaders.find(ident);
    ConditionsDataLoader* loader = 0;
    if ( ild == m_loaders.end() )  {
      std::string typ = "DD4hep_Conditions_"+ident+"_Loader";
      std::string fac = ident+"_ConditionsDataLoader";
      const void* argv[] = {fac.c_str(), m_mgr.ptr(), 0};
      loader = createPlugin<ConditionsDataLoader>(typ,m_detector,2,argv);
      if ( !loader )  {
        except("ConditionsMultiLoader",
               "Failed to create conditions loader of type: "+typ+" to read:"+nam);
      }
    }
    else  {
      loader = (*ild).second;
    }
    loader->addSource(nam.substr(idx+1),req_validity);
    m_loaders[ident] = loader;
    m_openSources[nam] = loader;
    return loader;
  }
  return (*iop).second;
}
#if 0
/// Load  a condition set given a Detector Element and the conditions name according to their validity
size_t ConditionsMultiLoader::load_range(key_type key,
                                         const IOV& req_validity,
                                         RangeConditions& conditions)
{
  size_t len = conditions.size();
  // No better idea: Must chack all sources to find the required condition
  for(Sources::const_iterator i=m_sources.begin(); i != m_sources.end(); ++i)  {
    const IOV& iov = (*i).second;
    if ( iov.type == req_validity.type )  {
      if ( IOV::key_partially_contained(iov.keyData,req_validity.keyData) )  { 
        const std::string& nam = (*i).first;
        ConditionsDataLoader* loader = load_source(nam, req_validity);
        loader->load_range(key, req_validity, conditions);
      }
    }
  }
  return conditions.size() - len;
}


size_t ConditionsMultiLoader::load_single(key_type key,
                                          const IOV& req_validity,
                                          RangeConditions& conditions)
{
  size_t len = conditions.size();
  // No better idea: Must chack all sources to find the required condition
  for(Sources::const_iterator i=m_sources.begin(); i != m_sources.end(); ++i)  {
    const IOV& iov = (*i).second;
    if ( iov.type == req_validity.type )  {
      if ( IOV::key_partially_contained(iov.keyData,req_validity.keyData) )  {
        const std::string& nam = (*i).first;
        ConditionsDataLoader* loader = load_source(nam, req_validity);
        loader->load_single(key, req_validity, conditions);
      }
    }
  }
  return conditions.size() - len;
}
#endif
