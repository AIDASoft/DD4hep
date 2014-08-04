// $Id: VolumeManager.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Handle.inl"
#include "DD4hep/objects/VolumeManagerInterna.h"

using namespace DD4hep;
using namespace DD4hep::Geometry;

DD4HEP_INSTANTIATE_HANDLE_NAMED(VolumeManagerObject);

/// Default constructor
VolumeManagerContext::VolumeManagerContext()
    : identifier(0), mask(~0x0ULL) {
}

/// Default destructor
VolumeManagerContext::~VolumeManagerContext() {
}

/// Default constructor
VolumeManagerObject::VolumeManagerObject()
  : top(0), system(0), sysID(0), detMask(~0x0ULL), flags(VolumeManager::NONE) {
}

/// Default destructor
VolumeManagerObject::~VolumeManagerObject() {
  /// Cleanup volume tree
  for_each(volumes.begin(), volumes.end(), destroyObjects(volumes));
  volumes.clear();
  /// Cleanup dependent managers
  for_each(managers.begin(), managers.end(), destroyHandles(managers));
  managers.clear();
  subdetectors.clear();
}

/// Update callback when alignment has changed (called only for subdetectors....)
void VolumeManagerObject::update(unsigned long tags, DetElement& det, void* param)   {
  if ( DetElement::CONDITIONS_CHANGED == (tags&DetElement::CONDITIONS_CHANGED) )
    printout(DEBUG,"VolumeManager","+++ Conditions update %s param:%p",det.path().c_str(),param);
  if ( DetElement::PLACEMENT_CHANGED == (tags&DetElement::PLACEMENT_CHANGED) )  
    printout(DEBUG,"VolumeManager","+++ Alignment update %s param:%p",det.path().c_str(),param);

  for(Volumes::iterator i=volumes.begin(); i != volumes.end(); ++i)  {
    Context* c = (*i).second;
    printout(DEBUG,"VolumeManager","+++ Alignment update %s",c->placement.name());
    
  }
}

/// Search the locally cached volumes for a matching ID
VolumeManager::Context* VolumeManagerObject::search(const VolumeID& id) const {
  Context* context = 0;
  VolumeID volume_id(id);
  volume_id &= detMask;
  Volumes::const_iterator i = volumes.find(volume_id);
  if (i != volumes.end())  {
    context = (*i).second;
  }
  return context;
}
