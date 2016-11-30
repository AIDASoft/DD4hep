//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_CONDITIONS_MULTICONDITONSLOADER_H
#define DD4HEP_CONDITIONS_MULTICONDITONSLOADER_H

// Framework include files
#include "DDCond/ConditionsDataLoader.h"
#include "DD4hep/Printout.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions  {

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
      ConditionsDataLoader* load_source(const std::string& nam,const iov_type& req_validity);

    public:
      /// Default constructor
      ConditionsMultiLoader(LCDD& lcdd, ConditionsManager mgr, const std::string& nam);
      /// Default destructor
      virtual ~ConditionsMultiLoader();
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_single(key_type key,
                                 const iov_type& req_validity,
                                 RangeConditions& conditions);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range( key_type key,
                                 const iov_type& req_validity,
                                 RangeConditions& conditions);
      /// Optimized update using conditions slice data
      virtual size_t load_many(  const iov_type& /* req_validity */,
                                 EntryVector&    /* work         */,
                                 EntryVector&    /* loaded       */,
                                 EntryVector&    /* missing      */,
                                 iov_type&       /* conditions_validity */)
      {
        except("ConditionsLoader","+++ update: Invalid call!");
        return 0;
      }
    };
  }     /* End namespace Geometry                     */
}       /* End namespace DD4hep                       */
#endif  /* DD4HEP_CONDITIONS_MULTICONDITONSLOADER_H   */

//  AIDA Detector description implementation for LCD
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
//#include "ConditionsMultiLoader.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/PluginCreators.h"
#include "DDCond/ConditionsManager.h"

// Forward declartions
using std::string;
using namespace DD4hep::Conditions;

namespace {
  void* create_loader(DD4hep::Geometry::LCDD& lcdd, int argc, char** argv)   {
    const char* name = argc>0 ? argv[0] : "MULTILoader";
    ConditionsManager::Object* mgr = (ConditionsManager::Object*)(argc>0 ? argv[1] : 0);
    return new ConditionsMultiLoader(lcdd,ConditionsManager(mgr),name);
  }
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_Conditions_multi_Loader,create_loader)

/// Standard constructor, initializes variables
ConditionsMultiLoader::ConditionsMultiLoader(LCDD& lcdd, ConditionsManager mgr, const string& nam) 
: ConditionsDataLoader(lcdd, mgr, nam)
{
}

/// Default Destructor
ConditionsMultiLoader::~ConditionsMultiLoader() {
} 

ConditionsDataLoader* 
ConditionsMultiLoader::load_source(const std::string& nam,
                                   const iov_type& req_validity)
{
  OpenSources::iterator iop = m_openSources.find(nam);
  if ( iop == m_openSources.end() )  {
    size_t idx = nam.find(':');
    if ( idx == string::npos )   {
      except("ConditionsMultiLoader","Invalid data source specification: "+nam);
    }
    string ident = nam.substr(0,idx);
    Loaders::iterator ild = m_loaders.find(ident);
    ConditionsDataLoader* loader = 0;
    if ( ild == m_loaders.end() )  {
      string typ = "DD4hep_Conditions_"+ident+"_Loader";
      string fac = ident+"_ConditionsDataLoader";
      const void* argv[] = {fac.c_str(), m_mgr.ptr(), 0};
      loader = createPlugin<ConditionsDataLoader>(typ,m_lcdd,2,argv);
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

/// Load  a condition set given a Detector Element and the conditions name according to their validity
size_t ConditionsMultiLoader::load_range(key_type key,
                                         const iov_type& req_validity,
                                         RangeConditions& conditions)
{
  size_t len = conditions.size();
  // No better idea: Must chack all sources to find the required condition
  for(Sources::const_iterator i=m_sources.begin(); i != m_sources.end(); ++i)  {
    const IOV& iov = (*i).second;
    if ( iov.type == req_validity.type )  {
      if ( IOV::key_partially_contained(iov.keyData,req_validity.keyData) )  { 
        const string& nam = (*i).first;
        ConditionsDataLoader* loader = load_source(nam, req_validity);
        loader->load_range(key, req_validity, conditions);
      }
    }
  }
  return conditions.size() - len;
}


size_t ConditionsMultiLoader::load_single(key_type key,
                                          const iov_type& req_validity,
                                          RangeConditions& conditions)
{
  size_t len = conditions.size();
  // No better idea: Must chack all sources to find the required condition
  for(Sources::const_iterator i=m_sources.begin(); i != m_sources.end(); ++i)  {
    const IOV& iov = (*i).second;
    if ( iov.type == req_validity.type )  {
      if ( IOV::key_partially_contained(iov.keyData,req_validity.keyData) )  {
        const string& nam = (*i).first;
        ConditionsDataLoader* loader = load_source(nam, req_validity);
        loader->load_single(key, req_validity, conditions);
      }
    }
  }
  return conditions.size() - len;
}
