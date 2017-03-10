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
#include "DD4hep/MatrixHelpers.h"
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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Helper class to populate the volume manager
    /**
     *  \author  M.Frank
     *  \version 1.0
     */
    class VolumeManager_Populator {
      typedef PlacedVolume::VolIDs VolIDs;
      typedef vector<TGeoNode*> Chain;
      typedef pair<VolumeID, VolumeID> Encoding;
      /// Reference to the LCDD instance
      LCDD& m_lcdd;
      /// Reference to the volume manager to be populated
      VolumeManager m_volManager;
      /// Set of already added entries
      set<VolumeID> m_entries;
      /// Debug flag
      bool          m_debug;

    public:
      /// Default constructor
      VolumeManager_Populator(LCDD& lcdd, VolumeManager vm)
        : m_lcdd(lcdd), m_volManager(vm)
      {
        m_debug = (0 != ::getenv("DD4HEP_VOLMGR_DEBUG"));
      }

      /// Populate the Volume manager
      void populate(DetElement e) {
        //const char* typ = 0;//::getenv("VOLMGR_NEW");
        const DetElement::Children& c = e.children();
        SensitiveDetector parent_sd;
        if ( e->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR )  {
          parent_sd = m_lcdd.sensitiveDetector(e.name());
        }
        //printout(INFO, "VolumeManager", "++ Executing %s plugin manager version",typ ? "***NEW***" : "***OLD***");
        for (const auto& i : c )  {
          DetElement de = i.second;
          PlacedVolume pv = de.placement();
          if (pv.isValid()) {
            Chain chain;
            Encoding coding(0, 0);
            SensitiveDetector sd = parent_sd;
            m_entries.clear();
            scanPhysicalVolume(de, de, pv, coding, sd, chain);
            continue;
          }
          printout(WARNING, "VolumeManager", "++ Detector element %s of type %s has no placement.", 
                   de.name(), de.type().c_str());
        }
      }
      /// Scan a single physical volume and look for sensitive elements below
      size_t scanPhysicalVolume(DetElement& parent, DetElement e, PlacedVolume pv, 
                                Encoding parent_encoding,
                                SensitiveDetector& sd, Chain& chain)
      {
        TGeoNode* node = pv.ptr();
        size_t count = 0;
        if (node) {
          Volume vol = pv.volume();
          const VolIDs& pv_ids   = pv.volIDs();
          Encoding vol_encoding  = parent_encoding;
          bool     is_sensitive  = vol.isSensitive();
          bool     have_encoding = pv_ids.empty();
          bool     compound      = e.type() == "compound";

          if ( compound )  {
            sd = SensitiveDetector(0);
            vol_encoding = Encoding();
          }
          else if ( !sd.isValid() )  {
            if ( is_sensitive )
              sd = vol.sensitiveDetector();
            else if ( (parent->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR) )
              sd = m_lcdd.sensitiveDetector(parent.name());
            else if ( (e->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR) )
              sd = m_lcdd.sensitiveDetector(e.name());
          }
          chain.push_back(node);
          if ( sd.isValid() && !pv_ids.empty() )   {
            Readout ro = sd.readout();
            if ( ro.isValid() )   {
              vol_encoding = update_encoding(ro.idSpec(), pv_ids, parent_encoding);
              have_encoding = true;
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
              PlacedVolume pv_dau(daughter);
              DetElement   de_dau;
              /// Check if this particular volume is the placement of one of the
              /// children of this detector element. If the daughter placement is also
              /// a detector child, then we must reset the node chain.
              for( const auto& de : e.children() )  {
                if ( de.second.placement().ptr() == daughter )  {
                  de_dau = de.second;
                  break;
                }
              }
              if ( de_dau.isValid() ) {
                Chain dau_chain;
                count += scanPhysicalVolume(parent, de_dau, pv_dau, vol_encoding, sd, dau_chain);
              }
              else {
                count += scanPhysicalVolume(parent, e, pv_dau, vol_encoding, sd, chain);
              }
            }
            else  {
              except("VolumeManager",
                     "Invalid not instrumented placement:"+string(daughter->GetName())+
                     " [Internal error -- bad detector constructor]");
            }
            if ( compound )  {
              sd = SensitiveDetector(0);
            }
          }
          if ( sd.isValid() )   {
            if ( !have_encoding )   {
              printout(ERROR, "VolumeManager","Element %s: Missing SD encoding. Volume manager won't work!",
                       e.path().c_str());
            }
            if ( is_sensitive || count > 0 )  {
              /// Distinguish logically the two following cases for our understanding....
              /// Though the treatment is identical
              if ( node == e.placement().ptr() )  {
                // These here are placement nodes, which at the same time are DetElement placements
                // 1) We recuperate volumes from lower levels by reusing the subdetector
                //    This only works if there is exactly one sensitive detector per subdetector!
                // 2) DetElements in the upper hierarchy of the sensitive also get al volume id,
                //    and the volume is registered. (to be discussed)
                //
                // I hate this, but I could not talk Frank out of this!  M.F.
                //
                e.object<DetElement::Object>().volumeID = vol_encoding.first;
              }
              else  {
                // These here are placement nodes, which are no DetElement placement
                // used e.g. to model a very fine grained sensitive volume structure
                // without always having DetElements.
              }
              add_entry(sd, parent, e, node, vol_encoding, chain);
              ++count;
              if ( m_debug )  {
                IDDescriptor id(sd.readout().idSpec());
                printout(INFO,"VolumeManager","Parent: %-44s id:%016llx Encoding: %s",
                         parent.path().c_str(), parent.volumeID(), id.str(parent_encoding.first,parent_encoding.second).c_str());
                printout(INFO,"VolumeManager","Element:%-44s id:%016llx Encoding: %s",
                         e.path().c_str(), e.volumeID(), id.str(vol_encoding.first,vol_encoding.second).c_str());
                printout(INFO, "VolumeManager", "%s SD:%s VolIDs:%s id:%016llx mask:%016llx",
                         node == e.placement().ptr() ? "DETELEMENT PLACEMENT" : "VOLUME PLACEMENT    ",
                         sd.name(), pv_ids.str().c_str(), vol_encoding.first, vol_encoding.second);
              }
            }
          }
          chain.pop_back();
        }
        return count;
      }

      /// Compute the encoding for a set of VolIDs within a readout descriptor
      static Encoding update_encoding(const IDDescriptor iddesc, const VolIDs& ids, const Encoding& initial)  {
        VolumeID volume_id = initial.first, mask = initial.second;
        for (VolIDs::const_iterator i = ids.begin(); i != ids.end(); ++i) {
          const PlacedVolume::VolID& id = (*i);
          IDDescriptor::Field f = iddesc.field(id.first);
          VolumeID msk = f->mask();
          int      off = f->offset();
          VolumeID val = id.second;    // Necessary to extend volume IDs > 32 bit
          volume_id   |= ((f->value(val << off) << off)&msk);
          mask        |= msk;
        }
        return make_pair(volume_id, mask);
      }
      /// Compute the encoding for a set of VolIDs within a readout descriptor
      static Encoding encoding(const IDDescriptor iddesc, const VolIDs& ids)  {
        VolumeID volume_id = 0, mask = 0;
        for (VolIDs::const_iterator i = ids.begin(); i != ids.end(); ++i) {
          const PlacedVolume::VolID& id = (*i);
          IDDescriptor::Field f = iddesc.field(id.first);
          VolumeID msk = f->mask();
          int      off = f->offset();
          VolumeID val = id.second;    // Necessary to extend volume IDs > 32 bit
          volume_id |= ((f->value(val << off) << off)&msk);
          mask      |= msk;
        }
        return make_pair(volume_id, mask);
      }

      void add_entry(SensitiveDetector sd, DetElement parent, DetElement e, 
                     const TGeoNode* n, const Encoding& code, Chain& nodes) 
      {
        if ( sd.isValid() )   {
          if (m_entries.find(code.first) == m_entries.end()) {
            Readout       ro           = sd.readout();
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
            for (size_t i = nodes.size(); i > 1; --i) {   // Omit the placement of the parent DetElement
              TGeoMatrix* m = nodes[i-1]->GetMatrix();
              context->toWorld.MultiplyLeft(m);
              //::printf("Element [%d]: ",int(i)); m->Print();
            }
            //            context->volID      = ids;
            //            context->path       = nodes;
            context->toDetector = context->toWorld;
            context->toDetector.MultiplyLeft(nodes[0]->GetMatrix());
            //context->toWorld.MultiplyLeft(&parent.nominal().worldTransformation());
            context->toWorld.MultiplyLeft(&e.nominal().worldTransformation());
            if ( !section.adoptPlacement(context) || m_debug )  {
              print_node(sd, parent, e, n, code, nodes);
              //context->toWorld.Print();
            }
            m_entries.insert(code.first);

            /** Comment this section to avoid too many computations .... 
             *  These are only consistentcyu tests
             */
            
            {
              int res1 = _matrixEqual(e->__worldTransformation(),e.nominal().worldTransformation());
              int res2 = _matrixEqual(e->__parentTransformation(),e.nominal().detectorTransformation());
              if ( res1 != MATRICES_EQUAL || res2 != MATRICES_EQUAL )  {
                stringstream log;
                // We HAVE to check at least once if the matrices from the original DetElement
                // and from the nominal alignment are identical....
                string p = "";
                for (size_t i=0; i<nodes.size(); ++i) {   // Omit the placement of the parent DetElement
                  p += "/";
                  p += nodes[i]->GetName();
                }

                log << "Alignment:";
                for(size_t j=0; j<e.nominal()->nodes.size(); ++j)
                  log << " " << (void*)e.nominal()->nodes[j].ptr();
                log << " Nodes:";
                for (size_t j = 0; j<nodes.size(); ++j) {
                  log << " " << (void*)nodes[j];
                }
                if ( res1 != MATRICES_EQUAL )  {
                  res1 = _matrixEqual(e->__worldTransformation(),e.nominal().worldTransformation());
                  const char* tag = (res1==MATRICES_DIFFER_TRANSLATION) ? "translation"
                    : (res1==MATRICES_DIFFER_ROTATION) ? "rotation"
                    : "translation+rotation";
                  printout(WARNING,"VolumeManager",
                           "+++ World matrix of %s // %s is NOT equal (%s)!  Pointers:%s",
                           e.placementPath().c_str(),p.c_str(),tag,log.str().c_str());
                }
                if ( res2 != MATRICES_EQUAL )  {
                  res2 = _matrixEqual(e->__parentTransformation(),e.nominal().detectorTransformation());
                  const char* tag = (res2==MATRICES_DIFFER_TRANSLATION) ? "translation"
                    : (res2==MATRICES_DIFFER_ROTATION) ? "rotation"
                    : "translation+rotation";
                  printout(WARNING,"VolumeManager",
                           "+++ Parent matrix of %s // %s is NOT equal (%s)! Pointers:%s ",
                           e.placementPath().c_str(),p.c_str(), tag, log.str().c_str());
                }
              }
            }
          }
        }
      }

      void print_node(SensitiveDetector sd, DetElement parent, DetElement e,
                      const TGeoNode* n, const Encoding& code, const Chain& nodes) const
      {
        PlacedVolume pv = n;
        Readout      ro = sd.readout();
        bool sensitive = pv.volume().isSensitive();

        //if ( !sensitive ) return;
        stringstream log;
        log << m_entries.size() << ": Detector: " << e.path()
            << " id:" << volumeID(code.first)
            << " Nodes(" << int(nodes.size()) << "):" << ro.idSpec().str(code.first,code.second);
        printout(m_debug ? INFO : DEBUG,"VolumeManager",log.str().c_str());
        //for(const auto& i : nodes )
        //  log << i->GetName() << "/";

        log.str("");
        log << m_entries.size() << ": " << parent.name()
            << " ro:" << ro.name() << " pv:" << n->GetName()
            << " Sensitive:" << yes_no(sensitive);
        printout(m_debug ? INFO : DEBUG, "VolumeManager", log.str().c_str());
      }
    };
  }       /* End namespace Geometry              */
}         /* End namespace DD4hep                */

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(LCDD& lcdd, const string& nam, DetElement elt, Readout ro, int flags) {
  printout(INFO, "VolumeManager", " - populating volume ids - be patient ..."  );
  Object* obj_ptr = new Object();
  assign(obj_ptr, nam, "VolumeManager");
  if (elt.isValid()) {
    VolumeManager_Populator p(lcdd, *this);
    obj_ptr->detector = elt;
    obj_ptr->id = ro.isValid() ? ro.idSpec() : IDDescriptor();
    obj_ptr->top = obj_ptr;
    obj_ptr->flags = flags;
    p.populate(elt);
  }
  printout(INFO, "VolumeManager", " - populating volume ids - done"  );
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(DetElement sub_detector, Readout ro)  {
  Object* obj_ptr = new Object();
  obj_ptr->detector = sub_detector;
  obj_ptr->id = ro.isValid() ? ro.idSpec() : IDDescriptor();
  assign(obj_ptr, sub_detector.name(), "VolumeManager");
}

VolumeManager VolumeManager::getVolumeManager(LCDD& lcdd) {
  if( not lcdd.volumeManager().isValid() ) {
    lcdd.apply("DD4hepVolumeManager", 0, 0);
  }
  return lcdd.volumeManager();
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
    for (const auto& j : o.subdetectors )  {
      const Object& mo = j.second._data();
      VolumeID      sys_id = mo.system->value(id << mo.system->offset());
      if ( sys_id == mo.sysID )
        return j.second;
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
  VolumeID mask = context->mask;
  PlacedVolume pv = context->placement;
  Volumes::iterator i = o.volumes.find(vid);

  if ( (vid&mask) != vid ) {
    err << "Bad context mask:" << (void*)mask
        << " id:" << (void*)vid
        << " pv:" << pv.name() << " Sensitive:"
        << yes_no(pv.volume().isSensitive()) << endl;
    goto Fail;
  }

  if (i == o.volumes.end()) {
    o.volumes[vid] = context;
    o.detMask |= mask;
    err << "Inserted new volume:" << setw(6) << left << o.volumes.size()
        << " Ptr:"  << (void*) pv.ptr()
        << " ["     << pv.name() << "]"
        << " id:"   << setw(16) << hex << right << setfill('0') << vid  << dec << setfill(' ')
        << " mask:" << setw(16) << hex << right << setfill('0') << mask << dec << setfill(' ')
        << " Det:"  << context->element.path();
    printout(VERBOSE, "VolumeManager", err.str().c_str());
    //printout(INFO, "VolumeManager", err.str().c_str());
    return true;
  }
  err << "+++ Attempt to register duplicate"
      << " id:"   << setw(16) << hex << right << setfill('0') << vid  << dec << setfill(' ')
      << " mask:" << setw(16) << hex << right << setfill('0') << mask << dec << setfill(' ')
      << " to detector " << o.detector.name()
      << " ptr:" << (void*) pv.ptr()
      << " Name:" << pv.name()
      << " Sensitive:" << yes_no(pv.volume().isSensitive()) << endl;
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  err.str("");
  context = (*i).second;
  pv = context->placement;
  err << " !!!!!               +++ Clashing"
      << " id:"   << setw(16) << hex << right << setfill('0') << vid  << dec << setfill(' ')
      << " mask:" << setw(16) << hex << right << setfill('0') << mask << dec << setfill(' ')
      << " to detector " << o.detector.name()
      << " ptr:" << (void*) pv.ptr()
      << " Name:" << pv.name()
      << " Sensitive:" << yes_no(pv.volume().isSensitive()) << endl;
 Fail:
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
       << " id:"   << setw(18) << left << (void*) c->identifier
       << " mask:" << setw(18) << left << (void*) c->mask
       << endl;
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
