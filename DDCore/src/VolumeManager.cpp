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
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"

// C/C++ includes
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

#define VOLUME_IDENTIFIER(id,mask)  id
//#define VOLUME_IDENTIFIER(id,mask)  id,mask

namespace {

  struct Populator  {
    typedef VolumeManager::VolumeID VolumeID;
    typedef vector<const TGeoNode*> Chain;

    struct DetCtxt  {
      DetElement        detector;
      Chain             chain;
      SensitiveDetector sd;
      DetCtxt() {}
      ~DetCtxt() {}
    };
    typedef map<TNamed*,DetCtxt*> DetContext;
    /// Reference to the LCDD instance
    LCDD&         m_lcdd;
    /// Reference to the volume manager to be populated
    VolumeManager m_volManager;
    /// Flag
    bool m_add;

    DetContext m_detContext;
    /// Default constructor
    Populator(LCDD& lcdd, VolumeManager vm) : m_lcdd(lcdd), m_volManager(vm), m_add(true)  {}

    /// Populate the Volume manager
    void populate(DetElement e, bool add=true)   {
      m_add = add;
      const DetElement::Children& c = e.children();
      for(DetElement::Children::const_iterator i=c.begin(); i!=c.end(); ++i)   {
	DetElement   de = (*i).second;
	PlacedVolume pv = de.placement();
	if ( pv.isValid() )  {
	  Chain                chain;
	  PlacedVolume::VolIDs ids;
	  m_detContext.clear();
	  scanPhysicalVolume(de, de, pv, ids, chain);
	  if ( add )  {
	    for(DetContext::const_iterator di=m_detContext.begin(); di != m_detContext.end(); ++di)  {	
	      Chain det_chain;
	      PlacedVolume::VolIDs ids;
	      DetElement det = (*di).second->detector;
	      const Chain& chain = (*di).second->chain;
	      if ( !det.parent().isValid() ) continue;
	      for(Chain::const_iterator ci=chain.begin(); ci!=chain.end(); ++ci)  {
		PlacedVolume pv = Ref_t(*ci);
		det_chain.push_back(*ci);
		ids.PlacedVolume::VolIDs::Base::insert(ids.end(),pv.volIDs().begin(),pv.volIDs().end());
		if ( pv.ptr() == det.placement().ptr() )  break;
	      }
	      if ( !ids.empty() )  {
		stringstream log;
		IDDescriptor            iddesc = (*di).second->sd.readout().idSpec();
		pair<VolumeID,VolumeID> code   = encoding(iddesc, ids);
		log << det.path() << " encoding:" << code.first << "," << code.second << " ids:";
		for(size_t m=0; m<ids.size(); ++m)
		  log << ids[m].first << "=" << ids[m].second << "  ";
		printout(DEBUG,"VolumeManager","++ Add DetElements:%s",log.str().c_str());
		add_entry((*di).second->sd, de, det, det.placement().ptr(), ids, det_chain);
	      }
	    }
	  }
	  for(DetContext::const_iterator di=m_detContext.begin(); di != m_detContext.end(); ++di)
	    delete (*di).second;
	  continue;
	}
	printout(WARNING,"VolumeManager","++ Detector element %s of type %s has no placement.",
		 de.name(),de.type().c_str());
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
    size_t scanPhysicalVolume(DetElement parent, DetElement e, PlacedVolume pv, 
			      PlacedVolume::VolIDs ids, Chain& chain)
    {
      const TGeoNode* node = pv.ptr();
      size_t count = 0;
      if ( node )  {
	Volume vol = pv.volume();
	chain.push_back(node);
	ids.PlacedVolume::VolIDs::Base::insert(ids.end(),pv.volIDs().begin(),pv.volIDs().end());
	if ( vol.isSensitive() )  {
	  SensitiveDetector sd = vol.sensitiveDetector();
	  Readout           ro = sd.readout();
	  if ( ro.isValid() )  {
	    if ( m_add )  {
	      add_entry(parent, e, node, ids, chain);
	      ++count;
	    }
	    else  {
	      find_entry(parent, e, node, ids, chain);
	    }
	  }
	  else  {
	    printout(WARNING,"VolumeManager",
		     "%s: Strange constellation volume %s is sensitive, but has no readout! sd:%p",
		     parent.name(), pv.volume().name(), sd.ptr());
	  }
	}
	for(Int_t idau=0, ndau=node->GetNdaughters(); idau<ndau; ++idau)  {
	  TGeoNode* daughter = node->GetDaughter(idau);
	  if ( dynamic_cast<const PlacedVolume::Object*>(daughter) ) {
	    size_t cnt;
	    PlacedVolume pv_dau = Ref_t(daughter);
	    DetElement   de_dau = findElt(e,daughter);
	    if ( de_dau.isValid() )  {
	      Chain dau_chain;
	      cnt = scanPhysicalVolume(parent, de_dau, pv_dau, ids, dau_chain);
	    }
	    else  {
	      cnt = scanPhysicalVolume(parent, e, pv_dau, ids, chain);
	    }
	    count += cnt;
	  }
	}
	chain.pop_back();
      }
      return count;
    }
    pair<VolumeID,VolumeID> encoding(const IDDescriptor iddesc, const PlacedVolume::VolIDs& ids )  const  {
      //VolumeID volume_id = ~0x0ULL, mask = 0;
      VolumeID volume_id = 0, mask = 0;
      for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
	const PlacedVolume::VolID& id = (*i);
	IDDescriptor::Field f = iddesc.field(id.first);
	VolumeID msk = f->mask();
	int offset = f->offset();
	// This pads the unused bits with '1' instead of '0': 
	// volume_id &= ~msk;
	volume_id |= f->value(id.second<<offset)<<offset;
	mask      |= msk;
	//volume_id &= f.encode(id.second);
	//mask |= f.mask;
      }
      return make_pair(volume_id,mask);
    }

    void add_entry(DetElement parent, DetElement e,const TGeoNode* n, 
		  const PlacedVolume::VolIDs& ids, const Chain& nodes)
    {
      Volume            vol     = PlacedVolume(n).volume();
      SensitiveDetector sd      = vol.sensitiveDetector();
      add_entry(sd, parent,e,n,ids,nodes);
    }

    void add_entry(SensitiveDetector sd, 
		   DetElement parent, DetElement e,const TGeoNode* n, 
		   const PlacedVolume::VolIDs& ids, const Chain& nodes)
    {
      Readout           ro      = sd.readout();
      IDDescriptor      iddesc  = ro.idSpec();
      VolumeManager     section = m_volManager.addSubdetector(parent,ro);
      pair<VolumeID,VolumeID> code = encoding(iddesc, ids);

      // This is the block, we effectively have to save for each physical volume with a VolID
      VolumeManager::Context* context = new VolumeManager::Context;
      DetElement::Object& o = parent._data();
      context->identifier = code.first;
      context->mask       = code.second;
      context->detector   = parent;
      context->element    = e;
      context->placement  = Ref_t(n);
      context->volID      = ids;
      context->path       = nodes;
      for(size_t i=nodes.size(); i>1; --i)  {  // Omit the placement of the parent DetElement
	TGeoMatrix* m = nodes[i-1]->GetMatrix();
	context->toWorld.MultiplyLeft(m);
      }
      context->toDetector = context->toWorld;
      context->toDetector.MultiplyLeft(nodes[0]->GetMatrix());
      context->toWorld.MultiplyLeft(o.worldTransformation());
      if ( parent.path().find("HcalBarrel") != string::npos ) print_node(sd,parent,e,n,ids,nodes);
      if ( !section.adoptPlacement(context) )  {
	print_node(sd,parent,e,n,ids,nodes);
      }
      if ( e.placement().ptr() != n )  {
	DetContext::const_iterator di = m_detContext.find(e.ptr());
	if ( di == m_detContext.end() )  {
	  DetCtxt* c  = new DetCtxt();
	  c->detector = e;
	  c->chain    = nodes;
	  c->sd       = sd;
	  m_detContext[e.ptr()] = c;
	}
      }
    }
    void find_entry(DetElement parent, DetElement e,const TGeoNode* n, 
		  const PlacedVolume::VolIDs& ids, const Chain& nodes)
    {
      Volume            vol     = PlacedVolume(n).volume();
      SensitiveDetector sd      = vol.sensitiveDetector();
      Readout           ro      = sd.readout();
      IDDescriptor      iddesc  = ro.idSpec();
      pair<VolumeID,VolumeID> code = encoding(iddesc, ids);
#if 0
      VolumeID id  = (VolumeID(rand())<<32) + VolumeID(rand());
      id &= ~code.second;
      id |=  code.second;
      VolumeID volID = id&code.first;
#else
      VolumeID volID = code.first;
#endif

      VolumeManager::Context* context = m_volManager.lookupContext(volID);
      stringstream log;
      if ( !context )  {
	log  << "CANNOT FIND volume:" 
	     << " Ptr:"  << (void*)n
	     << " ["     << n->GetName() << "]"
	     << " ID:"   << (void*)volID
	     << " Mask:" << (void*)code.second;
	printout(ERROR,"VolumeManager",log.str().c_str());
	return;
      }
      else if ( context->placement.ptr() != n )  {
	log  << "FOUND WRONG volume:" 
	     << " Ptr:"  << (void*)context->placement.ptr() 
	     << " instead of " << (void*)n
	     << " ["     << context->placement.name()
	     << " instead of " << n->GetName() << "]"
	     << " ID:"   << (void*)context->identifier 
	     << " instead of " << (void*)volID;
	printout(ERROR,"VolumeManager",log.str().c_str());
	return;
      }
      log  << "Found volume:" 
	   << " Ptr:"  << (void*)context->placement.ptr()
	   << " ["     << context->placement.name() << "]"
	   << " ID:"   << (void*)context->identifier 
	   << " Mask:" << (void*)context->mask;
      printout(DEBUG,"VolumeManager",log.str().c_str());
    }
    void print_node(DetElement parent, DetElement e, const TGeoNode* n, 
		    const PlacedVolume::VolIDs& ids, const Chain& nodes)
    {
      Volume              vol = PlacedVolume(n).volume();
      SensitiveDetector   sd = vol.sensitiveDetector();
      print_node(sd, parent, e, n, ids, nodes);
    }

    void print_node(SensitiveDetector sd, DetElement parent, DetElement e, const TGeoNode* n, 
		    const PlacedVolume::VolIDs& ids, const Chain& nodes)
    {
      static int s_count = 0;
      Readout             ro = sd.readout();
      const IDDescriptor& en = ro.idSpec();
      PlacedVolume        pv = Ref_t(n);
      bool                sensitive  = pv.volume().isSensitive();
      pair<VolumeID,VolumeID> code = encoding(en, ids);
      IDDescriptor::VolumeID volume_id = code.first;

      if ( !sensitive ) return;
      ++s_count;
      stringstream log;
      log << s_count << ": " << e.name() << " de:" << e.ptr() << " ro:" << ro.ptr() 
	  << " pv:" << n->GetName() << " id:" << (void*)volume_id << " : ";
      for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
	const PlacedVolume::VolID& id = (*i);
	IDDescriptor::Field f = ro.idSpec().field(id.first);
	VolumeID value = f->value(volume_id);
	log << id.first << "=" << id.second << "," << value 
	    << " [" << f->offset() << "," << f->width() << "] ";
      }
      log << " Sensitive:" << (sensitive ? "YES" : "NO");
      printout(INFO,"VolumeManager",log.str().c_str());
#if 0
      log.str("");
      log << s_count << ": " << e.name() << " Detector GeoNodes:";
      for(vector<const TGeoNode*>::const_iterator j=nodes.begin(); j!=nodes.end();++j)
	log << (void*)(*j) << " ";
      printout(INFO,"VolumeManager",log.str().c_str());
#endif
    }
  };
}

/// Default constructor
VolumeManager::Context::Context() : identifier(0), mask(~0x0ULL) {
}

/// Default destructor
VolumeManager::Context::~Context()   {
}

/// Default constructor
VolumeManager::Object::Object(LCDD& l) : lcdd(l), top(0), sysID(0), flags(VolumeManager::NONE)  {
}

/// Default destructor
VolumeManager::Object::~Object()   {
  Object* obj  = dynamic_cast<Object*>(top);
  bool   isTop =  obj == this;
  bool  hasTop = (flags&VolumeManager::ONE)==VolumeManager::ONE;
  bool  isSdet = (flags&VolumeManager::TREE)==VolumeManager::TREE && obj != this;
  /// Cleanup volume tree
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

/// Search the locally cached volumes for a matching physical volume
VolumeManager::Context* VolumeManager::Object::search(const PlacedVolume pv)  const  {
  PhysVolumes::const_iterator i = phys_volumes.find(pv.ptr());
  if ( i != phys_volumes.end() )
    return (*i).second;
  return 0;
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(LCDD& lcdd, const string& nam, DetElement elt, Readout ro, int flags)
{
  Object* ptr = new Object(lcdd);
  assign(ptr,nam,"VolumeManager");
  if ( elt.isValid() )   {
    Populator p(lcdd, *this);
    Object& o = _data();
    setDetector(elt, ro);
    o.top   = ptr;
    o.flags = flags;
    p.populate(elt);
    p.populate(elt,false);
  }
}

/// Add a new Volume manager section according to a new subdetector
VolumeManager VolumeManager::addSubdetector(DetElement detector, Readout ro)  {
  if ( isValid() )  {
    Object& o = _data();
    Detectors::const_iterator i=o.subdetectors.find(detector);
    if ( i == o.subdetectors.end() )  {
      string det_name = detector.name();
      // First check all pre-conditions
      if ( !ro.isValid() )  {
	throw runtime_error("VolumeManager::addSubdetector: Only subdetectors with a "
			    "valid readout descriptor are allowed. [Invalid DetElement:"+det_name+"]");
      }
      PlacedVolume pv = detector.placement();
      if ( !pv.isValid() )   {
	throw runtime_error("VolumeManager::addSubdetector: Only subdetectors with a "
			    "valid placement are allowed. [Invalid DetElement:"+det_name+"]");
      }
      PlacedVolume::VolIDs::Base::const_iterator vit = pv.volIDs().find("system");
      if ( vit == pv.volIDs().end() )   {
	throw runtime_error("VolumeManager::addSubdetector: Only subdetectors with "
			    "valid placement VolIDs are allowed. [Invalid DetElement:"+det_name+"]");
      }

      i = o.subdetectors.insert(make_pair(detector,VolumeManager(o.lcdd,detector.name()))).first;
      const PlacedVolume::VolID& id = (*vit);
      VolumeManager m = (*i).second;
      IDDescriptor::Field field = ro.idSpec().field(id.first);
      if ( !field )   {
	throw runtime_error("VolumeManager::addSubdetector: IdDescriptor of "+string(detector.name())+" has no field "+id.first);
      }
      Object& mo = m._data();
      m.setDetector(detector,ro);
      mo.top    = o.top;
      mo.flags  = o.flags;
      mo.system = field;
      mo.sysID  = id.second;
      o.managers[mo.sysID] = m;
    }
    return (*i).second;
  }
  throw runtime_error("VolumeManager::addSubdetector: Failed to add subdetector section. [Invalid Manager Handle]");
}

/// Access the volume manager by cell id
VolumeManager VolumeManager::subdetector(VolumeID id)  const   {
  if ( isValid() )  {
    const Object&  o = _data();
    /// Need to perform a linear search, because the "system" tag width may vary between subdetectors
    for(Detectors::const_iterator j=o.subdetectors.begin(); j != o.subdetectors.end(); ++j)  {
      const Object& mo = (*j).second._data();
      //VolumeID sys_id = mo.system.decode(id);
      VolumeID sys_id = mo.system->value(id<<mo.system->offset());
      if ( sys_id == mo.sysID )
	return (*j).second;
    }
    throw runtime_error("VolumeManager::subdetector(VolID): Attempt to access unknown subdetector section.");
  }
  throw runtime_error("VolumeManager::subdetector(VolID): Cannot assign ID descriptor [Invalid Manager Handle]");
}

/// Assign the top level detector element to this manager
void VolumeManager::setDetector(DetElement e, Readout ro)   {
  if ( isValid() )  {
    if ( e.isValid() )  {
      Object&  o = _data();
      o.id       = ro.isValid() ? ro.idSpec() : IDDescriptor();
      o.detector = e;
      return;
    }
    throw runtime_error("VolumeManager::setDetector: Cannot assign invalid detector element [Invalid Handle]");
  }
  throw runtime_error("VolumeManager::setDetector: Cannot assign detector element [Invalid Manager Handle]");
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
  throw runtime_error("VolumeManager::setIDDescriptor: Cannot assign ID descriptor [Invalid Manager Handle]");
}

/// Access IDDescription structure
IDDescriptor VolumeManager::idSpec() const   {
  return _data().id;
}

/// Register physical volume with the manager (normally: section manager)
bool VolumeManager::adoptPlacement(VolumeID sys_id, Context* context)   {
  stringstream err;
  Object&      o  = _data();
  PlacedVolume pv = context->placement;
  VolIdentifier vid(VOLUME_IDENTIFIER(context->identifier,context->mask));
  Volumes::iterator i = o.volumes.find(vid);
#if 0
  if ( (context->identifier&context->mask) != context->identifier )   {
    err << "Bad context mask:" << (void*)context->mask << " id:" << (void*)context->identifier
	<< " pv:" << pv.name() << " Sensitive:" 
	<< (pv.volume().isSensitive() ? "YES" : "NO") << endl;
    goto Fail;
  }
#endif
  if ( i == o.volumes.end() )   {
    o.volumes[vid] = context;
    o.phys_volumes[pv.ptr()] = context;
    err  << "Inserted new volume:" << setw(6) << left << o.volumes.size() 
	 << " Ptr:"  << (void*)context->placement.ptr()
	 << " ["     << context->placement.name() << "]"
	 << " ID:"   << (void*)context->identifier 
	 << " Mask:" << (void*)context->mask;
    printout(DEBUG,"VolumeManager",err.str().c_str());
    return true;
  }
  err << "Attempt to register twice volume with identical volID "
      << (void*)context->identifier << " Mask:" << (void*)context->mask
      << " to detector " << o.detector.name()
      << " ptr:" << (void*)pv.ptr() << " -- " << (*i).second->placement.ptr()
      << " pv:" << pv.name() << " clashes with " << (*i).second->placement.name()
      << " Sensitive:" << (pv.volume().isSensitive() ? "YES" : "NO") << endl;
  goto Fail;
 Fail:
  {
    err << "++++ VolIDS:";
    const PlacedVolume::VolIDs::Base& ids = context->volID;
    for(PlacedVolume::VolIDs::Base::const_iterator vit = ids.begin(); vit != ids.end(); ++vit)
      err << (*vit).first << "=" << (*vit).second << "; ";
  }
  printout(ERROR,"VolumeManager","++++[!!!] adoptPlacement: %s",err.str().c_str());
  // throw runtime_error(err.str());
  return false;
}

/// Register physical volume with the manager (normally: section manager)
bool VolumeManager::adoptPlacement(Context* context)   {
  stringstream err;
  if ( isValid() )  {
    Object& o = _data();
    if ( context )   {
      if ( (o.flags&ONE) == ONE )  {
	VolumeManager top(Ref_t(o.top));
	return top.adoptPlacement(context);
      }
      if ( (o.flags&TREE) == TREE )  {
	bool isTop = ptr() == o.top;
	if ( !isTop )   {
	  VolumeID sys_id = o.system->value(context->identifier);
	  if ( sys_id == o.sysID )  {
	    return adoptPlacement(sys_id,context);
	  }
	  VolumeManager top(Ref_t(o.top));
	  return top.adoptPlacement(context);
	}
	for(Managers::iterator j=o.managers.begin(); j != o.managers.end(); ++j)  {
	  Object& m = (*j).second._data();
	  VolumeID sid = m.system->value(context->identifier);
	  if ( (*j).first == sid )  {
	    return (*j).second.adoptPlacement(sid,context);
	  }
	}
      }
      return false;
    }
    err << "Failed to add new physical volume to detector:" << o.detector.name() << " [Invalid Context]";
    goto Fail;
  }
  err << "Failed to add new physical volume [Invalid Manager Handle]";
  goto Fail;
 Fail:
  throw runtime_error(err.str());
  return false;
}

/// Lookup the context, which belongs to a registered physical volume.
VolumeManager::Context* VolumeManager::lookupContext(VolumeID volume_id) const  throw()  {
  if ( isValid() )  {
    Context* c = 0;
    const Object& o = _data();
    bool is_top = o.top == ptr();
    bool one_tree = (o.flags&ONE) == ONE;
    if ( !is_top && one_tree )  {
      return VolumeManager(Ref_t(o.top)).lookupContext(volume_id);
    }
    VolIdentifier id(VOLUME_IDENTIFIER(volume_id,~0x0ULL));
    /// First look in our own volume cache if the entry is found.
    c = o.search(id);
    if ( c ) return c;
    /// Second: look in the subdetector volume cache if the entry is found.
    if ( !one_tree )  {
      for(Detectors::const_iterator j=o.subdetectors.begin(); j != o.subdetectors.end(); ++j)  {
	if ( (c=(*j).second._data().search(id)) != 0 )
	  return c;
      }
    }
    throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Unknown identifier]");
  }
  throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Invalid Manager Handle]");
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

/// Enable printouts for debugging
std::ostream& DD4hep::Geometry::operator<<(std::ostream& os, const VolumeManager& m)   {
  const VolumeManager::Object& o = *m.data<VolumeManager::Object>();
  VolumeManager::Object* top  = dynamic_cast<VolumeManager::Object*>(o.top);
  bool   isTop =  top == &o;
  bool  hasTop = (o.flags&VolumeManager::ONE)==VolumeManager::ONE;
  bool  isSdet = (o.flags&VolumeManager::TREE)==VolumeManager::TREE && top != &o;
  string prefix(isTop ? "" : "++  ");
  os << prefix 
     << (isTop ? "TOP Level " : "Secondary ") 
     << "Volume manager:" << &o  << " " << o.detector.name()
     << " IDD:"   << o.id.toString()
     << " SysID:" << (void*)o.sysID << " "
     << o.managers.size() << " subsections "
     << o.volumes.size()  << " placements ";
  if ( !(o.managers.empty() && o.volumes.empty()) ) os << endl;
  for(VolumeManager::Volumes::const_iterator i = o.volumes.begin(); i!=o.volumes.end(); ++i)  {
    const VolumeManager::Context* c = (*i).second;
    PlacedVolume pv = c->placement;
    os << prefix
       << "PV:"    << setw(32) << left << pv.name() 
       << " id:"   << setw(18) << left << (void*)c->identifier 
       << " mask:" << setw(18) << left << (void*) c->mask << endl;
  }
  for(VolumeManager::Managers::const_iterator i = o.managers.begin(); i!=o.managers.end(); ++i)
    os << prefix << (*i).second << endl;
  return os;
}


#if 0

      It was wishful thinking, the implementation of the reverse lookups would be as simple.
      Hence the folling calls are removed for the time being.

      Markus Frank


      /** This set of functions is required when reading/analyzing 
       *  already created hits which have a VolumeID attached.
       */
      /// Lookup the context, which belongs to a registered physical volume.
      Context*     lookupContext(PlacedVolume vol) const throw();
      /// Access the physical volume identifier from the placed volume
      VolumeID     lookupID(PlacedVolume vol) const;
      /// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
      DetElement   lookupDetector(PlacedVolume vol)  const;
      /// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
      DetElement   lookupDetElement(PlacedVolume vol)  const;
      /// Access the transformation of a physical volume to the world coordinate system
      const TGeoMatrix& worldTransformation(PlacedVolume vol)  const;

/// Lookup the context, which belongs to a registered physical volume.
VolumeManager::Context* VolumeManager::lookupContext(PlacedVolume pv) const throw()  {
  if ( isValid() )  {
    Context* c = 0;
    const Object& o = _data();
    if ( o.top != ptr() && (o.flags&ONE) == ONE )  {
      return VolumeManager(Ref_t(o.top)).lookupContext(pv);
    }
    /// First look in our own volume cache if the entry is found.
    c = o.search(pv);
    if ( c ) return c;
    /// Second: look in the subdetector volume cache if the entry is found.
    for(Detectors::const_iterator j=o.subdetectors.begin(); j != o.subdetectors.end(); ++j)  {
      if ( (c=(*j).second._data().search(pv)) != 0 )
	return c;
    }
    throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Unknown identifier]");
  }
  throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Invalid Manager Handle]");
}

/// Access the physical volume identifier from the placed volume
VolumeManager::VolumeID VolumeManager::lookupID(PlacedVolume vol) const   {
  Context* c = lookupContext(vol);
  return c->identifier;
}

/// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
DetElement   VolumeManager::lookupDetector(PlacedVolume vol)  const  {
  Context* c = lookupContext(vol);
  return c->detector;
}

/// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
DetElement   VolumeManager::lookupDetElement(PlacedVolume vol)  const   {
  Context* c = lookupContext(vol);
  return c->element;
}

/// Access the transformation of a physical volume to the world coordinate system
const TGeoMatrix& VolumeManager::worldTransformation(PlacedVolume vol)  const  {
  Context* c = lookupContext(vol);
  return c->toWorld;
}

#endif
