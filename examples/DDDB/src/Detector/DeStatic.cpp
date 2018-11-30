//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================

// Framework include files
#include "Detector/DetectorElement_inl.h"
#include "DD4hep/Printout.h"
#include "DDDB/DDDBConversion.h"
#include "DD4hep/ConditionsDebug.h"

namespace gaudi {
  template std::map<dd4hep::DetElement, gaudi::detail::DeStaticObject*>
  DeHelpers::getChildConditions<gaudi::detail::DeStaticObject>(ConditionsMap& m, DetElement de, itemkey_type key, int flags);
}

using namespace gaudi;
using namespace gaudi::detail;

/// Helper to initialize the basic information
DeStaticObject* DeStaticObject::fill_info(DetElement de, Catalog* cat)    {
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
  name      = Keys::staticKeyName;
#endif
  detector = de;
  geometry = de.placement();
  key      = Keys::staticKey;
  clsID    = cat->classID;
  catalog  = cat;
  for( const auto& p : cat->params )
    parameters.set(p.first, p.second.second, p.second.first);
  return this;
}

/// Initialization of sub-classes
void DeStaticObject::initialize()   {
  if ( (de_flags&DeInit::INITIALIZED) == 0 )   {
    de_flags |= DeInit::INITIALIZED;
    return;
  }
  except("DeStatic","initialize> Modifying a condition after initialization is not allowed!");
}

/// Printout method to stdout
void DeStaticObject::print(int indent, int flg)  const   {
  std::string prefix = DE::indent(indent);
  printout(INFO, "DeStatic", "%s*========== Detector:%s",
           prefix.c_str(), detector.path().c_str());
  if ( flg & DePrint::PARAMS )  {
    for( const auto& p : parameters.params() )   {
      printout(INFO, "DeStatic", "%s+ Param: %s -> %s [%s]",
               prefix.c_str(), p.first.c_str(),
               p.second.value.c_str(), p.second.type.c_str());
    }
  }
  printout(INFO, "DeStatic",
           "%s+ Name:%s Hash:%016lX Type:%s Flags:%08X %s%s",
           prefix.c_str(), dd4hep::cond::cond_name(this).c_str(), hash,
           is_bound() ? data.dataType().c_str() : "<UNBOUND>",
           flags, iov ? "" : "IOV:", iov ? "" : "---");
  if ( iov )  {
    printout(INFO, "DeStatic","%s+ IOV: %s", prefix.c_str(), iov->str().c_str());
  }
  if ( flg & DePrint::BASICS )  {
    const DetElement::Children& c = detector.children();
    printout(INFO, "DeStatic", "%s+ Detector:%s Class:%d key:%08X #Dau:%d",
             prefix.c_str(), detector.name(), clsID, key, int(c.size()));
  }
}

/// Access daughter elements: Static part
DeStaticObject* DeStaticObject::child(DetElement de)   const   {
  auto i = childCache.find(de);
  if ( i == childCache.end() )  {
    except("DeStatic","child> No such condition:%s for detector element:%s",
           Keys::staticKeyName.c_str(), de.path().c_str());
  }
  return (*i).second;
}

/// Fill the child cache. May only be called while the condition is NOT active
void DeStaticObject::fillCache(ConditionsMap& m)    {
  if ( (de_flags&DeInit::INITIALIZED) == 0 )   {
    childCache = DeHelpers::getChildConditions<DeStaticObject>(m, detector,Keys::deKey, DeHelpers::ALL);
    return;
  }
  except("DeIov","fillCache> Modifying % (%s) after initialization is not allowed!",
         Keys::staticKeyName.c_str(), detector.path().c_str());
}

/// Access parameters directory
const ParameterMap::Parameters& DeStaticObject::params()  const    {
  return this->parameters.params();
}

/// Access single parameter
const ParameterMap::Parameter&
DeStaticObject::parameter(const std::string& nam, bool throw_if_not_present)   const    {
  return this->parameters.parameter(nam, throw_if_not_present);
}
