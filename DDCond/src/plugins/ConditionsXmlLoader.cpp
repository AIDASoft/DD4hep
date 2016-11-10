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
#ifndef DD4HEP_CONDITIONS_XMLCONDITONSLOADER_H
#define DD4HEP_CONDITIONS_XMLCONDITONSLOADER_H

// Framework include files
#include "DDCond/ConditionsDataLoader.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions  {

    /// Implementation of a stack of conditions assembled before application
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionsXmlLoader : public ConditionsDataLoader   {
      typedef std::vector<Condition> Buffer;
      Buffer m_buffer;

      size_t load_source  (const std::string& nam,
                           key_type key,
                           const iov_type& req_validity,
                           RangeConditions& conditions);
    public:
      /// Default constructor
      ConditionsXmlLoader(LCDD& lcdd, ConditionsManager mgr, const std::string& nam);
      /// Default destructor
      virtual ~ConditionsXmlLoader();
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load(key_type key,
                          const iov_type& req_validity,
                          RangeConditions& conditions);
      /// Load  a condition set given a Detector Element and the conditions name according to their validity
      virtual size_t load_range(key_type key,
                                const iov_type& req_validity,
                                RangeConditions& conditions);
      /// Update a range of conditions according to the required IOV
      virtual size_t update(const iov_type& req_validity,
                            RangeConditions& conditions,
                            iov_type& iov_intersection);
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_XMLCONDITONSLOADER_H  */

//#include "ConditionsXmlLoader.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/PluginCreators.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsEntry.h"

// C/C++ include files
#include <string>

// Forward declartions
using std::string;
using namespace DD4hep;
using namespace DD4hep::Conditions;

namespace {
  void* create_loader(DD4hep::Geometry::LCDD& lcdd, int argc, char** argv)   {
    const char* name = argc>0 ? argv[0] : "XMLLoader";
    ConditionsManagerObject* mgr = (ConditionsManagerObject*)(argc>0 ? argv[1] : 0);
    return new ConditionsXmlLoader(lcdd,ConditionsManager(mgr),name);
  }
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_Conditions_xml_Loader,create_loader)

/// Standard constructor, initializes variables
ConditionsXmlLoader::ConditionsXmlLoader(LCDD& lcdd, ConditionsManager mgr, const std::string& nam) 
: ConditionsDataLoader(lcdd, mgr, nam)
{
}

/// Default Destructor
ConditionsXmlLoader::~ConditionsXmlLoader() {
} 

size_t ConditionsXmlLoader::load_source(const std::string& nam,
                                        key_type key,
                                        const iov_type& req_validity,
                                        RangeConditions& conditions)
{
  size_t len = conditions.size();
  string fac = "XMLConditionsParser";
  XML::DocumentHolder doc(XML::DocumentHandler().load(nam));
  XML::Handle_t       handle = doc.root();
  ConditionsStack     stack;
  char* argv[] = { (char*)handle.ptr(), (char*)&stack, 0};
  void* result = DD4hep::createPlugin(fac, m_lcdd, 2, argv, 0);
  if ( result == &m_lcdd )  { // All OK.
    for (ConditionsStack::iterator c=stack.begin(); c!=stack.end(); ++c)  {
      Entry* e = (*c);
      Condition condition = queueUpdate(e);
      delete e;
      if ( condition.isValid() )   {
        if ( key == condition->hash )  {
          if ( req_validity.contains(condition.iov()) )   {
            conditions.push_back(condition);
            continue;
          }
        }
        m_buffer.push_back(condition);
      }
    }
  }
  m_sources.erase(m_sources.begin());
  stack.clear();
  return conditions.size()-len;
}

size_t ConditionsXmlLoader::load(key_type key,
                                 const iov_type& req_validity,
                                 RangeConditions& conditions)
{
  size_t len = conditions.size();
  if ( m_buffer.empty() && !m_sources.empty() )  {
    return load_source(m_sources.begin()->first, key, req_validity, conditions);
  }
  for (Buffer::iterator j=m_buffer.begin(); j!=m_buffer.end(); ++j)  {
    Condition condition = *j;
    const IOV* iov = condition->iov;
    if ( IOV::partial_match(req_validity,*iov) )  {
      if ( key == condition->hash )  {
        conditions.push_back(condition);
        m_buffer.erase(j);
        return conditions.size()-len;
      }
    }
  }
  return conditions.size()-len;
}

size_t ConditionsXmlLoader::load_range(key_type key,
                                       const iov_type& req_validity,
                                       RangeConditions& conditions)
{
  size_t len = conditions.size();
  while ( !m_sources.empty() )  {
    load_source(m_sources.begin()->first, key, req_validity, conditions);
  }
  std::vector<Condition> keep;
  for (Buffer::iterator j=m_buffer.begin(); j!=m_buffer.end(); ++j)  {
    Condition condition = *j;
    const IOV* iov = condition->iov;
    if ( IOV::partial_match(req_validity,*iov) )  {
      if ( key == condition->hash )  {
        conditions.push_back(condition);
      }
    }
    keep.push_back(condition);
  }
  m_buffer = keep;
  return conditions.size()-len;
}

/// Update a range of conditions according to the required IOV
size_t ConditionsXmlLoader::update(const IOV&,RangeConditions&, IOV&)  {
  except("ConditionsXmlLoader","+++ update: Invalid call!");
  return 0;
}
