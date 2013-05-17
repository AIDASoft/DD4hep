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
    pair<VolumeID,VolumeID> encoding(const IDDescriptor iddesc, const PlacedVolume::VolIDs& ids )  const  {
      VolumeID volume_id = ~0x0ull, mask = 0;
      for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
	const PlacedVolume::VolID& id = (*i);
	IDDescriptor::Field f = iddesc.field(id.first);
	volume_id &= f.encode(id.second);
	mask |= f.mask;
      }
      return make_pair(volume_id,mask);
    }

    void add_entry(DetElement parent, DetElement e,const TGeoNode* n, 
		  const PlacedVolume::VolIDs& ids, const Chain& nodes, const Chain& elt_nodes)
    {
      Readout ro = parent.readout();
      IDDescriptor iddesc = ro.idSpec();
      VolumeManager section = m_volManager.addSubdetector(parent);
      pair<VolumeID,VolumeID> code = encoding(iddesc, ids);

      // This is the block, we effectively have to save for each physical volume with a VolID
      VolumeManager::Context* context = new VolumeManager::Context;
      DetElement::Object& o = parent._data();
      context->identifier = code.first;
      context->mask       = code.second;
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
    }
    void print_node(DetElement parent, DetElement e, const TGeoNode* n, 
		    const PlacedVolume::VolIDs& ids, const Chain& nodes, const Chain& elt_nodes)
    {
      static int s_count = 0;
      Readout ro = parent.readout();
      const IDDescriptor& en = ro.idSpec();
      PlacedVolume pv = Ref_t(n);
      bool sensitive  = pv.volume().isSensitive();
      pair<VolumeID,VolumeID> code = encoding(en, ids);
      IDDescriptor::VolumeID volume_id = code.first;

      if ( !sensitive ) return;
      ++s_count;
      cout << s_count << ": " << e.name() << " de:" << e.ptr() << " ro:" << ro.ptr() 
	   << " pv:" << n->GetName() << " id:" << (void*)volume_id << " : ";
      for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
	const PlacedVolume::VolID& id = (*i);
	IDDescriptor::Field f = ro.idSpec().field(id.first);
	int value = en.field(id.first).decode(volume_id);
	cout << id.first << "=" << id.second << "," << value 
	     << " [" << f.first << "," << f.second << "] ";
      }
      cout << " Sensitive:" << (sensitive ? "YES" : "NO");
      if ( sensitive )   {
	VolumeManager section = m_volManager.subdetector(volume_id);
	VolumeManager::Context* ctxt = section.lookupContext(volume_id|0xDEAD);
	if ( ctxt->placement.ptr() != pv.ptr() )  {
	  cout << " !!!!! No Volume found!" << endl;
	}
	cout << " Pv:" << pv.ptr() << " <> " << ctxt->placement.ptr();
      }
      cout << endl;
#if 0
      cout << s_count << ": " << e.name() << " Detector GeoNodes:";
      for(vector<const TGeoNode*>::const_iterator j=nodes.begin(); j!=nodes.end();++j)
	cout << (void*)(*j) << " ";
      cout << endl;
      cout << s_count << ": " << e.name() << " Element  GeoNodes:";
      for(vector<const TGeoNode*>::const_iterator j=elt_nodes.begin(); j!=elt_nodes.end();++j)
	cout << (void*)(*j) << " ";
      cout << endl;
#endif
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
VolumeManager::Object::Object() : top(0), sysID(0), flags(VolumeManager::NONE)  {
}

/// Default destructor
VolumeManager::Object::~Object()   {
  Object* obj  = dynamic_cast<Object*>(top);
  bool   isTop =  obj == this;
  bool  hasTop = (flags&VolumeManager::ONE)==VolumeManager::ONE;
  bool  isSdet = (flags&VolumeManager::TREE)==VolumeManager::TREE && obj != this;
  //cout << "Delete Volume manager: " << this << " " << detector.name() << endl;

  /// Cleanup volume tree
  if ( (isTop && hasTop) || (isSdet && !hasTop) )
    for_each(volumes.begin(),volumes.end(),DestroyObjects<VolIdentifier,Context*>());

  volumes.clear();
  /// Cleanup dependent managers
  for_each(managers.begin(),managers.end(),DestroyHandles<VolumeID,VolumeManager>());
  managers.clear();
  subdetectors.clear();
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
VolumeManager::VolumeManager(const string& nam, DetElement elt, int flags)
{
  Value<TNamed,Object>* ptr = new Value<TNamed,Object>();
  assign(ptr,nam,"VolumeManager");
  if ( elt.isValid() )   {
    Populator p(*this);
    Object& o = _data();
    setDetector(elt);
    o.top   = ptr;
    o.flags = flags;
    p.populate(elt);
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
      mo.top    = o.top;
      mo.flags  = o.flags;
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
void VolumeManager::adoptPlacement(VolumeID sys_id, Context* context)   {
  stringstream err;
  Object&      o  = _data();
  PlacedVolume pv = context->placement;
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
 Fail:
  cout << "VolumeManager::adoptPlacement: " << err.str() << endl;
  // throw runtime_error(err.str());
}

/// Register physical volume with the manager (normally: section manager)
void VolumeManager::adoptPlacement(Context* context)   {
  stringstream err;
  if ( isValid() )  {
    Object& o = _data();
    if ( context )   {
      VolumeID sys_id = o.system.decode(context->identifier);
      if ( sys_id == o.sysID )  {
	bool isTop = ptr() == o.top;
	if ( !isTop && (o.flags&ONE) == ONE )  {
	  VolumeManager top(Ref_t(o.top));
	  top.adoptPlacement(sys_id,context);
	}
	if ( (o.flags&TREE) == TREE )  {
	  adoptPlacement(sys_id,context);
	}
	return;
      }
      PlacedVolume pv = context->placement;
      err << "The placement " << pv.name() << " does not belong to detector:" << o.detector.name();
      goto Fail;
    }
    err << "Failed to add new physical volume to detector:" << o.detector.name() << " [Invalid object]";
    goto Fail;
  }
  err << "Failed to add new physical volume [Invalid Handle]";
  goto Fail;
 Fail:
  throw runtime_error(err.str());
}  

/// Lookup the context, which belongs to a registered physical volume.
VolumeManager::Context* VolumeManager::lookupContext(VolumeID volume_id) const  throw()  {
  if ( isValid() )  {
    Context* c = 0;
    const Object& o = _data();
    if ( o.top != ptr() && (o.flags&ONE) == ONE )  {
      return VolumeManager(Ref_t(o.top)).lookupContext(volume_id);
    }
    VolIdentifier id(VOLUME_IDENTIFIER(volume_id,~0x0ull));
    /// First look in our own volume cache if the entry is found.
    c = o.search(id);
    if ( c ) return c;
    /// Second: look in the subdetector volume cache if the entry is found.
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

#include <iomanip>
/// Enable printouts for debugging
std::ostream& DD4hep::Geometry::operator<<(std::ostream& os, const VolumeManager& m)   {
  const VolumeManager::Object& o = *m.data<VolumeManager::Object>();
  VolumeManager::Object* top  = dynamic_cast<VolumeManager::Object*>(o.top);
  bool   isTop =  top == &o;
  bool  hasTop = (o.flags&VolumeManager::ONE)==VolumeManager::ONE;
  bool  isSdet = (o.flags&VolumeManager::TREE)==VolumeManager::TREE && top != &o;
  string prefix(isTop ? "" : "++  ");
  cout << prefix 
       << (isTop ? "TOP Level " : "Secondary ") 
       << "Volume manager:" << &o  << " " << o.detector.name()
       << " IDD:"   << o.id.toString()
       << " SysID:" << (void*)o.sysID << " "
       << o.managers.size() << " subsections "
       << o.volumes.size()  << " placements ";
  if ( !(o.managers.empty() && o.volumes.empty()) ) cout << endl;
  for(VolumeManager::Volumes::const_iterator i = o.volumes.begin(); i!=o.volumes.end(); ++i)  {
    const VolumeManager::Context* c = (*i).second;
    PlacedVolume pv = c->placement;
    cout << prefix
	 << "PV:"    << setw(32) << left << pv.name() 
	 << " id:"   << setw(18) << left << (void*)c->identifier 
	 << " mask:" << setw(18) << left << (void*) c->mask << endl;
  }
  for(VolumeManager::Managers::const_iterator i = o.managers.begin(); i!=o.managers.end(); ++i)
    cout << prefix << (*i).second << endl;
  return os;
}

