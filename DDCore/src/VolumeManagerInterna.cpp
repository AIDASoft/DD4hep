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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/VolumeManagerInterna.h"

using namespace dd4hep;
using namespace dd4hep::detail;

DD4HEP_INSTANTIATE_HANDLE_NAMED(VolumeManagerObject);

/// Default destructor
VolumeManagerObject::~VolumeManagerObject() {
  /// Cleanup volume tree
  destroyObjects(volumes);
  /// Cleanup dependent managers
  destroyHandles(managers);
  managers.clear();
  subdetectors.clear();
}

/// Update callback when alignment has changed (called only for subdetectors....)
void VolumeManagerObject::update(unsigned long tags, DetElement& det, void* param)   {
  if ( DetElement::CONDITIONS_CHANGED == (tags&DetElement::CONDITIONS_CHANGED) )
    printout(DEBUG,"VolumeManager","+++ Conditions update %s param:%p",det.path().c_str(),param);
  if ( DetElement::PLACEMENT_CHANGED == (tags&DetElement::PLACEMENT_CHANGED) )
    printout(DEBUG,"VolumeManager","+++ Alignment update %s param:%p",det.path().c_str(),param);
  
  for(const auto& i : volumes )
    printout(DEBUG,"VolumeManager","+++ Alignment update %s",i.second->placement().name());
}

/// Search the locally cached volumes for a matching ID
VolumeManagerContext* VolumeManagerObject::search(const VolumeID& vol_id) const {
  auto i = volumes.find(vol_id&detMask);
  return (i == volumes.end()) ? 0 : (*i).second;
}
