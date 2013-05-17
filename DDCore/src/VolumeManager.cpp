// $Id: VolumeManager.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#include "DD4hep/VolumeManager.h"

// C/C++ includes
#include <sstream>

using namespace std;
using namespace DD4hep::Geometry;

#define VOLUME_IDENTIFIER(id,mask)  id
//#define VOLUME_IDENTIFIER(id,mask)  id,mask

namespace {
  struct Populator  {
    typedef VolumeManager::VolumeID VolumeID;
    typedef vector<const TGeoNode*> Chain;
    /// Reference to the volume manager to be populated
    VolumeManager m_volManager;

    /// Default constructor
    Populator(VolumeManager vm) : m_volManager(vm) {}

    /// Populate the Volume manager
    void populate(DetElement e)   {
      const DetElement::Children& c = e.children();
      for(DetElement::Children::const_iterator i=c.begin(); i!=c.end(); ++i)   {
	DetElement   de = (*i).second;
	PlacedVolume pv = de.placement();
	if ( pv.isValid() )  {
	  PlacedVolume::VolIDs ids;
	  Chain node_chain, elt_nodes;
	  scanPhysicalVolume(de, de, pv, ids, node_chain, elt_nodes);
	  continue;
	}
	cout << "++ Detector element " << de.name() << " has no placement." << endl;
      }
    }
    /// Find a detector element below with the corresponding placement
    DetElement findElt(DetElement e, PlacedVolume pv)  {
      const DetElement::Children& c = e.children();
      if ( e.placement().ptr() == pv.ptr() ) return e;
      for(DetElement::Children::const_iterator i=c.begin(); i!=c.end(); ++i)   {
	DetElement de = (*i).second;
	if ( de.placement().ptr() == pv.ptr() ) return de;
      }
      for(DetElement::Children::const_iterator i=c.begin(); i!=c.end(); ++i)   {
	DetElement de = findElt((*i).second,pv);
	if ( de.isValid() )  return de;
      }
      return DetElement();
    }
    /// Scan a single physical volume and look for sensitive elements below
    void scanPhysicalVolume(DetElement parent, DetElement e, PlacedVolume pv, 
		  PlacedVolume::VolIDs ids, Chain& node_chain, Chain& elt_nodes)
    {
      const TGeoNode* node = pv.ptr();
      if ( node )  {
	node_chain.push_back(node);
	elt_nodes.push_back(node);
	ids.PlacedVolume::VolIDs::Base::insert(ids.end(),pv.volIDs().begin(),pv.volIDs().end());
	if ( pv.volume().isSensitive() )  {
	  add_entry(parent, e, node, ids, node_chain, elt_nodes);
	}
	for(Int_t idau=0, ndau=node->GetNdaughters(); idau<ndau; ++idau)  {
	  TGeoNode* daughter = node->GetDaughter(idau);
	  if ( dynamic_cast<const PlacedVolume::Object*>(daughter) ) {
	    Chain dau_nodes;
	    PlacedVolume pv_dau = Ref_t(daughter);
	    DetElement   de_dau = findElt(e,daughter);
	    if ( de_dau.isValid() )
	      scanPhysicalVolume(parent, de_dau, pv_dau, ids, node_chain, dau_nodes);
	    else
	      scanPhysicalVolume(parent, e, pv_dau, ids, node_chain, elt_nodes);
	  }
	}
	elt_nodes.pop_back();
	node_chain.pop_back();
      }
    }
    void add_entry(DetElement parent, DetElement e,const TGeoNode* n, 
		  const PlacedVolume::VolIDs& ids, const Chain& nodes, const Chain& elt_nodes )
    {
      Readout ro = parent.readout();
      IDDescriptor iddesc = ro.idSpec();
      VolumeManager section = m_volManager.addSubdetector(parent);
      VolumeID volume_id = ~0x0ull, mask = 0;
      for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
	const PlacedVolume::VolID& id = (*i);
	IDDescriptor::Field f = iddesc.field(id.first);
	volume_id &= f.encode(id.second);
	mask |= f.mask;
      }
      // This is the block, we effectively have to save for each physical volume with a VolID
      VolumeManager::Context* context = new VolumeManager::Context;
      DetElement::Object& o = parent._data();
      context->identifier = volume_id;
      context->mask       = mask;
      context->detector   = parent;
      context->element    = e;
      context->placement  = Ref_t(n);
      for(size_t i=nodes.size(); i>1; --i)  {  // Omit the placement of the parent DetElement
	TGeoMatrix* m = nodes[i-1]->GetMatrix();
	context->toWorld.MultiplyLeft(m);
      }
      context->toDetector = context->toWorld;
      context->toDetector.MultiplyLeft(nodes[0]->GetMatrix());
      context->toWorld.MultiplyLeft(o.worldTransformation());
      section.adoptPlacement(context);
      m_volManager.adoptPlacement(context);
    }

  };
}

/// Default constructor
VolumeManager::Context::Context() : identifier(0), mask(~0x0ull) {
}

/// Default destructor
VolumeManager::Context::~Context()   {
}

/// Default constructor
VolumeManager::Object::Object() : sysID(0), top(0)  {
}

/// Default destructor
VolumeManager::Object::~Object()   {
  if ( top )   {
    for_each(volumes.begin(),volumes.end(),DeleteMapEntry<Volumes::value_type>());
    volumes.clear();
    subdetectors.clear();
    managers.clear();
  }
}

/// Search the locally cached volumes for a matching ID
VolumeManager::Context* VolumeManager::Object::search(const VolIdentifier& id)  const  {
  Context* context = 0;
  Volumes::const_iterator i = volumes.find(id);
  if ( i != volumes.end() )
    context = (*i).second;
  return context;
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(const string& nam, DetElement elt)
{
  assign(new Value<TNamed,Object>(),nam,"VolumeManager");
  if ( elt.isValid() )   {
    Populator p(*this);
    setDetector(elt);
    p.populate(elt);
    _data().top = 1;
  }
}

/// Add a new Volume manager section according to a new subdetector
VolumeManager VolumeManager::addSubdetector(DetElement detector)  {
  if ( isValid() )  {
    Object& o = _data();
    Detectors::const_iterator i=o.subdetectors.find(detector);
    if ( i == o.subdetectors.end() )  {
      // First check all pre-conditions
      Readout ro = detector.readout();
      if ( !ro.isValid() )  {
	throw runtime_error("VolumeManager::addSubdetector: Only subdetectors with a "
			    "valid readout descriptor are allowed. [Invalid DetElement]");
      }
      PlacedVolume pv = detector.placement();
      if ( !pv.isValid() )   {
	throw runtime_error("VolumeManager::addSubdetector: Only subdetectors with a "
			    "valid placement are allowed. [Invalid DetElement]");
      }
      PlacedVolume::VolIDs::Base::const_iterator vit = pv.volIDs().find("system");
      if ( vit == pv.volIDs().end() )   {
	throw runtime_error("VolumeManager::addSubdetector: Only subdetectors with "
			    "valid placement VolIDs are allowed. [Invalid DetElement]");
      }

      i = o.subdetectors.insert(make_pair(detector,VolumeManager(detector.name()))).first;
      const PlacedVolume::VolID& id = (*vit);
      VolumeManager m = (*i).second;
      IDDescriptor::Field field = ro.idSpec().field(id.first);
      Object& mo = m._data();
      m.setDetector(detector);
      mo.system = field;
      mo.sysID  = id.second;
      o.managers[mo.sysID] = m;
    }
    return (*i).second;
  }
  throw runtime_error("VolumeManager::addSubdetector: Failed to add subdetector section. [Invalid Handle]");
}

/// Access the volume manager by cell id
VolumeManager VolumeManager::subdetector(VolumeID id)  const   {
  if ( isValid() )  {
    const Object&  o = _data();
#if 0
    // This does not work!!!!
    VolumeID sys_id = o.system.decode(id);
    Managers::const_iterator i = o.managers.find(sys_id);
    if ( i != o.managers.end() )  {
      return (*i).second;
    }
#endif
    /// Need to perform a linear search, because the "system" tag width may vary between subdetectors
    for(Detectors::const_iterator j=o.subdetectors.begin(); j != o.subdetectors.end(); ++j)  {
      const Object& mo = (*j).second._data();
      VolumeID sys_id = mo.system.decode(id);
      if ( sys_id == mo.sysID )
	return (*j).second;
    }
    throw runtime_error("VolumeManager::subdetector(VolID): Attempt to access unknown subdetector section.");
  }
  throw runtime_error("VolumeManager::subdetector(VolID): Cannot assign ID descriptor [Invalid Handle]");
}

/// Assign the top level detector element to this manager
void VolumeManager::setDetector(DetElement e)   {
  if ( isValid() )  {
    if ( e.isValid() )  {
      Object&  o = _data();
      Readout ro = e.readout();
      o.id       = ro.isValid() ? ro.idSpec() : IDDescriptor();
      o.detector = e;
      return;
    }
    throw runtime_error("VolumeManager::setDetector: Cannot assign invalid detector element [Invalid Handle]");
  }
  throw runtime_error("VolumeManager::setDetector: Cannot assign detector element [Invalid Handle]");
}

/// Access the top level detector element
DetElement VolumeManager::detector() const   {
  if ( isValid() )  {
    return _data().detector;
  }
  throw runtime_error("VolumeManager::detector: Cannot access DetElement [Invalid Handle]");
}

/// Assign IDDescription to VolumeManager structure
void VolumeManager::setIDDescriptor(IDDescriptor new_descriptor)  const   {
  if ( isValid() )  {
    if ( new_descriptor.isValid() )  {   // Do NOT delete!
      _data().id = new_descriptor;
      return;
    }
  }
  throw runtime_error("VolumeManager::setIDDescriptor: Cannot assign ID descriptor [Invalid Handle]");
}

/// Access IDDescription structure
IDDescriptor VolumeManager::idSpec() const   {
  return _data().id;
}

/// Register physical volume with the manager (normally: section manager)
void VolumeManager::adoptPlacement(Context* context)   {
  stringstream err;
  if ( isValid() )  {
    Object& o = _data();
    if ( context )   {
      VolumeID sys_id = o.system.decode(context->identifier);
      PlacedVolume pv = context->placement;
      if ( sys_id == o.sysID )  {
	VolIdentifier vid(VOLUME_IDENTIFIER(context->identifier,context->mask));
	Volumes::iterator i = o.volumes.find(vid);
	if ( (context->identifier&context->mask) != context->identifier )   {
	  err << "Bad context mask!";
	  goto Fail;
	}
	if ( i == o.volumes.end() )   {
	  o.volumes[vid] = context;
	  cout << "Inserted new volume:" << o.volumes.size() 
	       << " ID:" << (void*)context->identifier << " Mask:" << (void*)context->mask << endl;
	  return;
	}
	err << "Attempt to register twice volume with identical volID to detector " << o.detector.name() 
	    << " pv:" << pv.name() << " Sensitive:" << (pv.volume().isSensitive() ? "YES" : "NO");
	goto Fail;
      }
      err << "The placement " << pv.name() << " does not belong to detector:" << o.detector.name();
      goto Fail;
    }
    err << "Failed to add new physical volume to detector:" << o.detector.name() << " [Invalid object]";
    goto Fail;
  }
  err << "Failed to add new physical volume [Invalid Handle]";
  goto Fail;
 Fail:
  cout << "VolumeManager::adoptPlacement: " << err.str() << endl;
  // throw runtime_error(err.str());
}  

/// Lookup the context, which belongs to a registered physical volume.
VolumeManager::Context* VolumeManager::lookupContext(VolumeID volume_id) const  throw()  {
  if ( isValid() )  {
    const Object& o = _data();
    VolIdentifier id(VOLUME_IDENTIFIER(volume_id,~0x0ull));
    /// First look in our own volume cache if the entry is found.
    Context* c = o.search(id);
    if ( c ) return c;
    /// Second look in the subdetector volume cache if the entry is found.
    for(Detectors::const_iterator j=o.subdetectors.begin(); j != o.subdetectors.end(); ++j)  {
      if ( (c=(*j).second._data().search(id)) != 0 )
	return c;
    }
    throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Unknown identifier]");
  }
  throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Invalid Handle]");
}

/// Lookup a physical (placed) volume identified by its 64 bit hit ID
PlacedVolume VolumeManager::lookupPlacement(VolumeID volume_id) const    {
  Context* c = lookupContext(volume_id);
  return c->placement;
}

/// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
DetElement   VolumeManager::lookupDetector(VolumeID volume_id)  const    {
  Context* c = lookupContext(volume_id);
  return c->detector;
}

/// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
DetElement   VolumeManager::lookupDetElement(VolumeID volume_id)  const   {
  Context* c = lookupContext(volume_id);
  return c->element;
}

/// Access the transformation of a physical volume to the world coordinate system
const TGeoMatrix& VolumeManager::worldTransformation(VolumeID volume_id)  const   {
  Context* c = lookupContext(volume_id);
  return c->toWorld;
}
