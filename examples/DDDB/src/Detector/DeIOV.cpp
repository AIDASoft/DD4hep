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
#include "Detector/DetectorElement.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/ConditionsDebug.h"
#include "DD4hep/AlignmentsPrinter.h"
#include "DD4hep/detail/AlignmentsInterna.h"

#include "Detector/DetectorElement_inl.h"
namespace gaudi {
  template std::map<dd4hep::DetElement, gaudi::detail::DeIOVObject*>
  DeHelpers::getChildConditions<gaudi::detail::DeIOVObject>(ConditionsMap& m, DetElement de, itemkey_type key, int flags);
}

#include <sstream>

using namespace gaudi;
using namespace gaudi::detail;

/// Helper to initialize the basic information
DeIOVObject* DeIOVObject::fill_info(DetElement de, Catalog* /* cat */)   {
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
  name      = Keys::deKeyName;
#endif
  item_key  = Keys::deKey;
  detector  = de;
  hash      = dd4hep::ConditionKey::KeyMaker(de, Keys::deKey).hash;
  return this;
}

/// Initialization of sub-classes
void DeIOVObject::initialize()    {
  if ( (de_flags&DeInit::INITIALIZED) == 0 )   {
    auto i = conditions.find(dd4hep::align::Keys::alignmentKey);
    de_flags |= DeInit::INITIALIZED;
    detectorAlignment = (i==conditions.end()) ? Condition() : (*i).second;
    if ( detectorAlignment.isValid() )   {
      toLocalMatrix = detectorAlignment.worldTransformation().Inverse();
      detectorAlignment.delta().computeMatrix(deltaMatrix);
    }
    return;
  }
  except("DeIov","initialize> Modifying a condition after initialization is not allowed!");
}

/// Printout method to stdout
void DeIOVObject::print(int indent, int flg)  const   {
  std::string prefix = DE::indent(indent);
  if ( flg&DePrint::STATIC)  {
    de_static.print(indent, flg&~DePrint::CHILDREN);
  }
  printout(INFO, "DeIOV",
           "%s+ Name:%s Hash:%016lX Type:%s Flags:%08X IOV:%s",
           prefix.c_str(), dd4hep::cond::cond_name(this).c_str(), hash,
           is_bound() ? data.dataType().c_str() : "<UNBOUND>",
           flags, iov ? iov->str().c_str()      : "--");
  if ( flg&DePrint::DETAIL)  { 
   printout(INFO, "DeIOV","%s+  >> Conditions:%ld Alignment:%s VolAlign:%ld",
             prefix.c_str(), conditions.size(),
             yes_no(detectorAlignment.isValid()),
             volumeAlignments.size());
    for(const auto& cc : conditions)  {
      Condition c = cc.second;
      printout(INFO, "DeIOV","%s+  >> Condition [%08X] %s Hash:%016X Flags:%08X Type:%s",
               prefix.c_str(), cc.first, dd4hep::cond::cond_name(c).c_str(), c.key(), c.flags(),
               c.is_bound() ? c.data().dataType().c_str() : "<UNBOUND>");
      if ( c->iov )  {
        printout(INFO, "DeIOV","%s+  >> + IOV:%s",
                 prefix.c_str(), c->iov ? c.iov().str().c_str()   : "--");
      }
    }
    if ( detectorAlignment.isValid() )   {
      char txt1[64], txt2[64], txt3[64];
      std::stringstream str;
      dd4hep::Alignment::Object* ptr = detectorAlignment.ptr();
      const dd4hep::AlignmentData& alignment_data = detectorAlignment.data();
      const dd4hep::Delta& D = alignment_data.delta;

      if ( D.hasTranslation() )
        ::snprintf(txt1,sizeof(txt1),"Tr: x:%g y:%g z:%g ",D.translation.x(), D.translation.Y(), D.translation.Z());
      else
        ::snprintf(txt1,sizeof(txt1),"Tr:    ------- ");
      if ( D.hasRotation() )
        ::snprintf(txt2,sizeof(txt2),"Rot: phi:%g psi:%g theta:%g ",D.rotation.Phi(), D.rotation.Psi(), D.rotation.Theta());
      else
      ::snprintf(txt2,sizeof(txt2),"Rot:   ------- ");
      if ( D.hasPivot() )
        ::snprintf(txt3,sizeof(txt3),"Rot: x:%g y:%g z:%g ",D.pivot.Vect().X(), D.pivot.Vect().Y(), D.pivot.Vect().Z());
      else
      ::snprintf(txt3,sizeof(txt3),"Pivot: ------- ");

      printout(INFO,"DeIOV","%s+  >> Aligment [%p] Typ:%s \tData:(%11s-%8s-%5s)",
               prefix.c_str(), detectorAlignment.ptr(),
               dd4hep::typeName(typeid(*ptr)).c_str(),
               D.hasTranslation() ? "Translation" : "",
               D.hasRotation() ? "Rotation" : "",
               D.hasPivot() ? "Pivot" : "");
      if ( D.hasTranslation() || D.hasRotation() || D.hasPivot() )  {
        printout(INFO,"DeIOV","%s+  >> Aligment-Delta %s %s %s",prefix.c_str(), txt1,txt2,txt3);
      }
    }
  }
}

/// Check (enforce) the validity of the alignment object if required
void DeIOVObject::checkAlignment()  const  {
  if ( !detectorAlignment.isValid() )   {
    dd4hep::except("DeIOVObject",
                   "Invalid alignment object! Cannot compute derived quantities.");
  }
}

/// Fill the child cache. May only be called while the condition is NOT active
void DeIOVObject::fillCache(ConditionsMap& m)    {
  if ( (de_flags&DeInit::INITIALIZED) == 0 )   {
    childCache = DeHelpers::getChildConditions<DeIOVObject>(m, detector, Keys::deKey, DeHelpers::ALL);
    return;
  }
  except("DeIov","fillCache> Modifying %d (%s) after initialization is not allowed!",
         Keys::deKeyName.c_str(), detector.path().c_str());
}

/// Access daughter elements: IOV dependent part
DeIOVObject* DeIOVObject::child(DetElement de)   const   {
  auto i = childCache.find(de);
  if ( i == childCache.end() )  {
    except("DeIOV","No such condition:%s for detector element:%s",
           Keys::deKeyName.c_str(), de.path().c_str());
  }
  return (*i).second;
}

/// Access condition by name
DeIOV::Condition DeIOVObject::condition(const std::string& nam)  const   {
  return this->condition(dd4hep::ConditionKey::itemCode(nam));
}

/// Access condition by name
DeIOV::Condition DeIOVObject::condition(const std::string& nam, bool throw_if)  const   {
  return this->condition(dd4hep::ConditionKey::itemCode(nam), throw_if);
}

/// Access condition by name
DeIOV::Condition DeIOVObject::condition(itemkey_type k)  const   {
  auto i = conditions.find(k);
  return (i == conditions.end()) ? (*i).second : Condition();
}

/// Access condition by name
DeIOV::Condition DeIOVObject::condition(itemkey_type k, bool throw_if)  const   {
  auto i = conditions.find(k);
  if (i != conditions.end())  {
    return (*i).second;
  }
  if ( throw_if )  {
    except("DeIOV","Attempt to access non-existing condition with key: %d.",k);
  }
 return Condition();
}
