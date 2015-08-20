// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/VolumeManagerInterna.h"

// C/C++ includes
#include <set>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace {

  struct Populator {
    typedef PlacedVolume::VolIDs VolIDs;
    typedef vector<TGeoNode*> Chain;
    /// Reference to the LCDD instance
    LCDD& m_lcdd;
    /// Reference to the volume manager to be populated
    VolumeManager m_volManager;
    /// Set of already added entries
    set<VolumeID> m_entries;
    /// Default constructor
    Populator(LCDD& lcdd, VolumeManager vm)
      : m_lcdd(lcdd), m_volManager(vm) {
    }

    /// Populate the Volume manager
    void populate(DetElement e) {
      const DetElement::Children& c = e.children();
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
        DetElement de = (*i).second;
        PlacedVolume pv = de.placement();
        if (pv.isValid()) {
          VolIDs ids;
          Chain chain;
          SensitiveDetector sd;
          m_entries.clear();
          scanPhysicalVolume(de, de, pv, ids, sd, chain);
          continue;
        }
        printout(WARNING, "VolumeManager", "++ Detector element %s of type %s has no placement.", 
                 de.name(), de.type().c_str());
      }
    }
    /// Find a detector element below with the corresponding placement
    DetElement findElt(DetElement e, PlacedVolume pv) {
      const DetElement::Children& c = e.children();
      if (e.placement().ptr() == pv.ptr())
        return e;
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
        DetElement de = (*i).second;
        if (de.placement().ptr() == pv.ptr())
          return de;
      }
      for (DetElement::Children::const_iterator i = c.begin(); i != c.end(); ++i) {
        DetElement de = findElt((*i).second, pv);
        if (de.isValid())
          return de;
      }
      return DetElement();
    }
    /// Scan a single physical volume and look for sensitive elements below
    size_t scanPhysicalVolume(DetElement& parent, DetElement e, PlacedVolume pv, 
                              VolIDs ids, SensitiveDetector& sd, Chain& chain)
    {
      TGeoNode* node = pv.ptr();
      size_t count = 0;
      if (node) {
        Volume vol = pv.volume();
        chain.push_back(node);
        VolIDs pv_ids = pv.volIDs();
        ids.VolIDs::Base::insert(ids.end(), pv_ids.begin(), pv_ids.end());
        bool got_readout = false;
        if ( vol.isSensitive() )  {
          sd = vol.sensitiveDetector();
          Readout ro = sd.readout();
          if ( sd.isValid() && ro.isValid() )   {
            got_readout = true;
            add_entry(sd, parent, e, node, ids, chain);
            ++count;
          }
          else {
            printout(WARNING, "VolumeManager",
                     "%s: Strange constellation volume %s is sensitive, but has no readout! sd:%p",
                     parent.name(), pv.volume().name(), sd.ptr());
          }
        }
        for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
          TGeoNode* daughter = node->GetDaughter(idau);
          PlacedVolume placement(daughter);
          if ( placement.data() ) {
            size_t cnt;
            PlacedVolume pv_dau = Ref_t(daughter);
            DetElement de_dau = findElt(e, daughter);
            if ( de_dau.isValid() ) {
              Chain dau_chain;
              cnt = scanPhysicalVolume(parent, de_dau, pv_dau, ids, sd, dau_chain);
            }
            else {
              cnt = scanPhysicalVolume(parent, e, pv_dau, ids, sd, chain);
            }
            // There was a sensitive daughter volume, also add the parent entry.
            if ( count == 0 && cnt > 0 && sd.isValid() && !pv_ids.empty()) {
              add_entry(sd, parent, e, node, ids, chain);
            }
            count += cnt;
          }
          else  {
            throw runtime_error("Invalid not instrumented placement:"+string(daughter->GetName())+
                                " [Internal error -- bad detector constructor]");
          }
        }
        if ( count == 0 )   { 
          sd = SensitiveDetector(0);
        }
        else if ( count > 0 && sd.isValid() )   {
          // We recuperate volumes from lower levels by reusing the subdetector
          // This only works if there is exactly one sensitive detector per subdetector!
          // I hate this, but I could not talk Frank out of this!  M.F.
          Readout ro = sd.readout();
          if ( ro.isValid() ) {
            IDDescriptor iddesc = ro.idSpec();
            pair<VolumeID, VolumeID> det_encoding = encoding(iddesc,ids);
            printout(VERBOSE,"VolumeManager","++++ %-11s  SD:%s VolID=%p Mask=%p",e.path().c_str(),
                     got_readout ? "RECUPERATED" : "REGULAR", sd.name(),
                     (void*)det_encoding.first, (void*)det_encoding.second);
            e.object<DetElement::Object>().volumeID = det_encoding.first;
          }
        }
        chain.pop_back();
      }
      return count;
    }

    /// Compute the encoding for a set of VolIDs within a readout descriptor
    pair<VolumeID, VolumeID> encoding(const IDDescriptor iddesc, const VolIDs& ids) const {
      VolumeID volume_id = 0, mask = 0;
      for (VolIDs::const_iterator i = ids.begin(); i != ids.end(); ++i) {
        const PlacedVolume::VolID& id = (*i);
        IDDescriptor::Field f = iddesc.field(id.first);
        VolumeID msk = f->mask();
        int offset = f->offset();
        volume_id |= f->value(id.second << offset) << offset;
        mask |= msk;
      }
      return make_pair(volume_id, mask);
    }
    void add_entry(SensitiveDetector sd, DetElement parent, DetElement e, 
                   const TGeoNode* n, const VolIDs& ids, Chain& nodes) 
    {
      if ( sd.isValid() )   {
        Readout ro = sd.readout();
        IDDescriptor iddesc = ro.idSpec();
        pair<VolumeID, VolumeID> code = encoding(iddesc, ids);

        if (m_entries.find(code.first) == m_entries.end()) {
          string        sd_name      = sd.name();
          DetElement    sub_detector = m_lcdd.detector(sd_name);
          VolumeManager section      = m_volManager.addSubdetector(sub_detector, ro);
          // This is the block, we effectively have to save for each physical volume with a VolID
          VolumeManager::Context* context = new VolumeManager::Context;
          context->identifier = code.first;
          context->mask       = code.second;
          context->detector   = parent;
          context->placement  = PlacedVolume(n);
          context->element    = e;
          context->volID      = ids;
          context->path       = nodes;
          for (size_t i = nodes.size(); i > 1; --i) {   // Omit the placement of the parent DetElement
            TGeoMatrix* m = nodes[i - 1]->GetMatrix();
            context->toWorld.MultiplyLeft(m);
          }
          context->toDetector = context->toWorld;
          context->toDetector.MultiplyLeft(nodes[0]->GetMatrix());
          context->toWorld.MultiplyLeft(&parent.worldTransformation());
          if (!section.adoptPlacement(context)) {
            print_node(sd, parent, e, n, ids, nodes);
          }
          m_entries.insert(code.first);
        }
      }
    }

    void print_node(SensitiveDetector sd, DetElement parent, DetElement e,
                    const TGeoNode* n, const VolIDs& ids, const Chain& /* nodes */)
    {
      static int s_count = 0;
      Readout ro = sd.readout();
      const IDDescriptor& en = ro.idSpec();
      PlacedVolume pv = Ref_t(n);
      bool sensitive = pv.volume().isSensitive();
      pair<VolumeID, VolumeID> code = encoding(en, ids);
      VolumeID volume_id = code.first;

      //if ( !sensitive ) return;
      ++s_count;
      stringstream log;
      log << s_count << ": " << parent.name() << ": " << e.name() 
          << " ro:" << ro.ptr() << " pv:" << n->GetName() << " id:"
          << (void*) volume_id << " : ";
      for (VolIDs::const_iterator i = ids.begin(); i != ids.end(); ++i) {
        const PlacedVolume::VolID& id = (*i);
        IDDescriptor::Field f = ro.idSpec().field(id.first);
        VolumeID value = f->value(volume_id);
        log << id.first << "=" << id.second << "," << value 
            << " [" << f->offset() << "," << f->width() << "] ";
      }
      log << " Sensitive:" << yes_no(sensitive);
      printout(DEBUG, "VolumeManager", log.str().c_str());
#if 0
      log.str("");
      log << s_count << ": " << e.name() << " Detector GeoNodes:";
      for(vector<const TGeoNode*>::const_iterator j=nodes.begin(); j!=nodes.end();++j)
        log << (void*)(*j) << " ";
      printout(DEBUG,"VolumeManager",log.str().c_str());
#endif
    }
  };
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(LCDD& lcdd, const string& nam, DetElement elt, Readout ro, int flags) {
  Object* obj_ptr = new Object();
  assign(obj_ptr, nam, "VolumeManager");
  if (elt.isValid()) {
    Populator p(lcdd, *this);
    obj_ptr->detector = elt;
    obj_ptr->id = ro.isValid() ? ro.idSpec() : IDDescriptor();
    obj_ptr->top = obj_ptr;
    obj_ptr->flags = flags;
    p.populate(elt);
  }
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(DetElement sub_detector, Readout ro)  {
  Object* obj_ptr = new Object();
  obj_ptr->detector = sub_detector;
  obj_ptr->id = ro.isValid() ? ro.idSpec() : IDDescriptor();
  assign(obj_ptr, sub_detector.name(), "VolumeManager");
}

/// Add a new Volume manager section according to a new subdetector
VolumeManager VolumeManager::addSubdetector(DetElement det, Readout ro) {
  if (isValid()) {
    Object& o = _data();
    if (!det.isValid()) {
      throw runtime_error("DD4hep: VolumeManager::addSubdetector: Only valid subdetectors "
                          "are allowed. [Invalid DetElement]");
    }
    Detectors::const_iterator i = o.subdetectors.find(det);
    if (i == o.subdetectors.end()) {
      string det_name = det.name();
      // First check all pre-conditions
      if (!ro.isValid()) {
        throw runtime_error("DD4hep: VolumeManager::addSubdetector: Only subdetectors with a "
                            "valid readout descriptor are allowed. [Invalid DetElement:" + det_name + "]");
      }
      PlacedVolume pv = det.placement();
      if (!pv.isValid()) {
        throw runtime_error("DD4hep: VolumeManager::addSubdetector: Only subdetectors with a "
                            "valid placement are allowed. [Invalid DetElement:" + det_name + "]");
      }
      VolIDs::Base::const_iterator vit = pv.volIDs().find("system");
      if (vit == pv.volIDs().end()) {
        throw runtime_error("DD4hep: VolumeManager::addSubdetector: Only subdetectors with "
                            "valid placement VolIDs are allowed. [Invalid DetElement:" + det_name + "]");
      }

      i = o.subdetectors.insert(make_pair(det, VolumeManager(det,ro))).first;
      const PlacedVolume::VolID& id = (*vit);
      VolumeManager m = (*i).second;
      IDDescriptor::Field field = ro.idSpec().field(id.first);
      if (!field) {
        throw runtime_error("DD4hep: VolumeManager::addSubdetector: IdDescriptor of " + 
                            string(det.name()) + " has no field " + id.first);
      }
      Object& mo = m._data();
      mo.top = o.top;
      mo.flags = o.flags;
      mo.system = field;
      mo.sysID = id.second;
      mo.detMask = mo.sysID;
      o.managers[mo.sysID] = m;
      det.callAtUpdate(DetElement::PLACEMENT_CHANGED|DetElement::PLACEMENT_DETECTOR,
                       &mo,&Object::update);
    }
    return (*i).second;
  }
  throw runtime_error("DD4hep: VolumeManager::addSubdetector: "
                      "Failed to add subdetector section. [Invalid Manager Handle]");
}

/// Access the volume manager by cell id
VolumeManager VolumeManager::subdetector(VolumeID id) const {
  if (isValid()) {
    const Object& o = _data();
    /// Need to perform a linear search, because the "system" tag width may vary between subdetectors
    for (Detectors::const_iterator j = o.subdetectors.begin(); j != o.subdetectors.end(); ++j) {
      const Object& mo = (*j).second._data();
      //VolumeID sys_id = mo.system.decode(id);
      VolumeID sys_id = mo.system->value(id << mo.system->offset());
      if (sys_id == mo.sysID)
        return (*j).second;
    }
    throw runtime_error("DD4hep: VolumeManager::subdetector(VolID): "
                        "Attempt to access unknown subdetector section.");
  }
  throw runtime_error("DD4hep: VolumeManager::subdetector(VolID): "
                      "Cannot assign ID descriptor [Invalid Manager Handle]");
}

/// Access the top level detector element
DetElement VolumeManager::detector() const {
  if (isValid()) {
    return _data().detector;
  }
  throw runtime_error("DD4hep: VolumeManager::detector: Cannot access DetElement [Invalid Handle]");
}

/// Access IDDescription structure
IDDescriptor VolumeManager::idSpec() const {
  return _data().id;
}

/// Register physical volume with the manager (normally: section manager)
bool VolumeManager::adoptPlacement(VolumeID /* sys_id */, Context* context) {
  stringstream err;
  Object& o = _data();
  VolumeID vid = context->identifier;
  PlacedVolume pv = context->placement;
  Volumes::iterator i = o.volumes.find(vid);
#if 0
  if ( (context->identifier&context->mask) != context->identifier ) {
    err << "Bad context mask:" << (void*)context->mask << " id:" << (void*)context->identifier
        << " pv:" << pv.name() << " Sensitive:"
        << yes_no(pv.volume().isSensitive()) << endl;
    goto Fail;
  }
#endif
  if (i == o.volumes.end()) {
    o.volumes[vid] = context;
    o.detMask |= context->mask;
    err << "Inserted new volume:" << setw(6) << left << o.volumes.size() << " Ptr:" << (void*) pv.ptr() << " ["
        << pv.name() << "]" << " ID:" << (void*) context->identifier << " Mask:" << (void*) context->mask;
    printout(VERBOSE, "VolumeManager", err.str().c_str());
    return true;
  }
  err << "+++ Attempt to register duplicate volID " << (void*) context->identifier
      << " Mask:" << (void*) context->mask
      << " to detector " << o.detector.name()
      << " ptr:" << (void*) pv.ptr()
      << " Name:" << pv.name()
      << " Sensitive:" << yes_no(pv.volume().isSensitive()) << endl;
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  err.str("");
  err << " !!!!!                      ++++ VolIDS ";
  const VolIDs::Base& id_vector = context->volID;
  for (VolIDs::Base::const_iterator vit = id_vector.begin(); vit != id_vector.end(); ++vit)
    err << (*vit).first << "=" << (*vit).second << "; ";
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  err.str("");
  context = (*i).second;
  pv = context->placement;
  err << " !!!!!               +++ Clashing volID " << (void*) context->identifier
      << " Mask:" << (void*) context->mask
      << " to detector " << o.detector.name()
      << " ptr:" << (void*) pv.ptr()
      << " Name:" << pv.name()
      << " Sensitive:" << yes_no(pv.volume().isSensitive()) << endl;
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  err.str("");

  goto Fail;
 Fail: {
    err << " !!!!!                      ++++ VolIDS ";
    const VolIDs::Base& ids = context->volID;
    for (VolIDs::Base::const_iterator vit = ids.begin(); vit != ids.end(); ++vit)
      err << (*vit).first << "=" << (*vit).second << "; ";
  }
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  // throw runtime_error(err.str());
  return false;
}

/// Register physical volume with the manager (normally: section manager)
bool VolumeManager::adoptPlacement(Context* context) {
  stringstream err;
  if (isValid()) {
    Object& o = _data();
    if (context) {
      if ((o.flags & ONE) == ONE) {
        VolumeManager top(Ref_t(o.top));
        return top.adoptPlacement(context);
      }
      if ((o.flags & TREE) == TREE) {
        bool isTop = ptr() == o.top;
        if (!isTop) {
          VolumeID sys_id = o.system->value(context->identifier);
          if (sys_id == o.sysID) {
            return adoptPlacement(sys_id, context);
          }
          VolumeManager top(Ref_t(o.top));
          return top.adoptPlacement(context);
        }
        for (Managers::iterator j = o.managers.begin(); j != o.managers.end(); ++j) {
          Object& m = (*j).second._data();
          VolumeID sid = m.system->value(context->identifier);
          if ((*j).first == sid) {
            return (*j).second.adoptPlacement(sid, context);
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
 Fail: throw runtime_error("DD4hep: " + err.str());
  return false;
}

/// Lookup the context, which belongs to a registered physical volume.
VolumeManager::Context* VolumeManager::lookupContext(VolumeID volume_id) const {
  if (isValid()) {
    Context* c = 0;
    const Object& o = _data();
    bool is_top = o.top == ptr();
    bool one_tree = (o.flags & ONE) == ONE;
    if (!is_top && one_tree) {
      return VolumeManager(Ref_t(o.top)).lookupContext(volume_id);
    }
    VolumeID id = volume_id;
    /// First look in our own volume cache if the entry is found.
    c = o.search(id);
    if (c)
      return c;
    /// Second: look in the subdetector volume cache if the entry is found.
    if (!one_tree) {
      for (Detectors::const_iterator j = o.subdetectors.begin(); j != o.subdetectors.end(); ++j) {
        if ((c = (*j).second._data().search(id)) != 0)
          return c;
      }
    }
    stringstream err;
    err << "VolumeManager::lookupContext: Failed to search Volume context [Unknown identifier]" 
        << (void*) volume_id;
    throw runtime_error("DD4hep: " + err.str());
  }
  throw runtime_error("DD4hep: VolumeManager::lookupContext: "
                      "Failed to search Volume context [Invalid Manager Handle]");
}

/// Lookup a physical (placed) volume identified by its 64 bit hit ID
PlacedVolume VolumeManager::lookupPlacement(VolumeID volume_id) const {
  Context* c = lookupContext(volume_id);
  return c->placement;
}

/// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
DetElement VolumeManager::lookupDetector(VolumeID volume_id) const {
  Context* c = lookupContext(volume_id);
  return c->detector;
}

/// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
DetElement VolumeManager::lookupDetElement(VolumeID volume_id) const {
  Context* c = lookupContext(volume_id);
  return c->element;
}

/// Access the transformation of a physical volume to the world coordinate system
const TGeoMatrix& VolumeManager::worldTransformation(VolumeID volume_id) const {
  Context* c = lookupContext(volume_id);
  return c->toWorld;
}

/// Enable printouts for debugging
std::ostream& DD4hep::Geometry::operator<<(std::ostream& os, const VolumeManager& m) {
  const VolumeManager::Object& o = *m.data<VolumeManager::Object>();
  VolumeManager::Object* top = dynamic_cast<VolumeManager::Object*>(o.top);
  bool isTop = top == &o;
  //bool hasTop = (o.flags & VolumeManager::ONE) == VolumeManager::ONE;
  //bool isSdet = (o.flags & VolumeManager::TREE) == VolumeManager::TREE && top != &o;
  string prefix(isTop ? "" : "++  ");
  os << prefix << (isTop ? "TOP Level " : "Secondary ") << "Volume manager:" 
     << &o << " " << o.detector.name() << " IDD:"
     << o.id.toString() << " SysID:" << (void*) o.sysID << " " 
     << o.managers.size() << " subsections " << o.volumes.size()
     << " placements ";
  if (!(o.managers.empty() && o.volumes.empty()))
    os << endl;
  for (VolumeManager::Volumes::const_iterator i = o.volumes.begin(); i != o.volumes.end(); ++i) {
    const VolumeManager::Context* c = (*i).second;
    PlacedVolume pv = c->placement;
    os << prefix << "PV:" << setw(32) << left << pv.name() 
       << " id:" << setw(18) << left << (void*) c->identifier << " mask:"
       << setw(18) << left << (void*) c->mask << endl;
  }
  for (VolumeManager::Managers::const_iterator i = o.managers.begin(); i != o.managers.end(); ++i)
    os << prefix << (*i).second << endl;
  return os;
}

// #if 0

// It was wishful thinking, the implementation of the reverse lookups would be as simple.
// Hence the folling calls are removed for the time being.

// Markus Frank

// /** This set of functions is required when reading/analyzing
//  *  already created hits which have a VolumeID attached.
//  */
// /// Lookup the context, which belongs to a registered physical volume.
// Context* lookupContext(PlacedVolume vol) const throw();
// /// Access the physical volume identifier from the placed volume
// VolumeID lookupID(PlacedVolume vol) const;
// /// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
// DetElement lookupDetector(PlacedVolume vol) const;
// /// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
// DetElement lookupDetElement(PlacedVolume vol) const;
// /// Access the transformation of a physical volume to the world coordinate system
// const TGeoMatrix& worldTransformation(PlacedVolume vol) const;

// /// Lookup the context, which belongs to a registered physical volume.
// VolumeManager::Context* VolumeManager::lookupContext(PlacedVolume pv) const throw() {
//   if ( isValid() ) {
//     Context* c = 0;
//     const Object& o = _data();
//     if ( o.top != ptr() && (o.flags&ONE) == ONE ) {
//       return VolumeManager(Ref_t(o.top)).lookupContext(pv);
//     }
//     /// First look in our own volume cache if the entry is found.
//     c = o.search(pv);
//     if ( c ) return c;
//     /// Second: look in the subdetector volume cache if the entry is found.
//     for(Detectors::const_iterator j=o.subdetectors.begin(); j != o.subdetectors.end(); ++j) {
//       if ( (c=(*j).second._data().search(pv)) != 0 )
//       return c;
//     }
//     throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Unknown identifier]");
//   }
//   throw runtime_error("VolumeManager::lookupContext: Failed to search Volume context [Invalid Manager Handle]");
// }

// /// Access the physical volume identifier from the placed volume
// VolumeManager::VolumeID VolumeManager::lookupID(PlacedVolume vol) const {
//   Context* c = lookupContext(vol);
//   return c->identifier;
// }

// /// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
// DetElement VolumeManager::lookupDetector(PlacedVolume vol) const {
//   Context* c = lookupContext(vol);
//   return c->detector;
// }

// /// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
// DetElement VolumeManager::lookupDetElement(PlacedVolume vol) const {
//   Context* c = lookupContext(vol);
//   return c->element;
// }

// /// Access the transformation of a physical volume to the world coordinate system
// const TGeoMatrix& VolumeManager::worldTransformation(PlacedVolume vol) const {
//   Context* c = lookupContext(vol);
//   return c->toWorld;
// }

// #endif
