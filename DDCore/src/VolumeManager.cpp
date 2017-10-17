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
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/VolumeManagerInterna.h"

// C/C++ includes
#include <set>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

DD4HEP_INSTANTIATE_HANDLE_NAMED(VolumeManagerObject);


namespace {

  static bool s_useAllocator = false;

  class ContextExtension  {
  public:
    /// The placement of the (sensitive) volume
    PlacedVolume placement{0};
    /// The transformation of space-points to the coordinate system of the closests detector element
    TGeoHMatrix toElement;
    /// Default constructor
    ContextExtension() = default;
    /// Default destructor
    ~ContextExtension() = default;
  };
    
  static size_t ALLOCATE_SIZE = 1000;
  class VolumeContextAllocator  {

  public:
    struct Small   {
      unsigned char  context[sizeof(VolumeManagerContext)];
      unsigned int   chunk = 0;
      unsigned short  slot = 0;
      unsigned char   type = 0;
      unsigned char   used = 0;
      /// Default constructor
      Small() : type(1)   {}
      /// Default destructor
      virtual ~Small() = default;
      /// Inhibit copy constructor
      Small(const Small& copy) = delete;
      /// Inhibit assignment operator
      Small& operator=(const Small& copy) = delete;
    };
    struct Large : public Small  {
      unsigned char extension[sizeof(ContextExtension)];
      /// Default constructor
      Large() : Small()  { type = 2; }
      /// Default destructor
      virtual ~Large() = default;
      /// Inhibit copy constructor
      Large(const Large& copy) = delete;
      /// Inhibit assignment operator
      Large& operator=(const Large& copy) = delete;
    };

    std::vector<std::pair<size_t,Small*> > small;
    std::vector<std::pair<size_t,Large*> > large;
    std::list<Small*> free_small;
    std::list<Large*> free_large;

    VolumeContextAllocator()  {}
    static VolumeContextAllocator* instance()  {
      static VolumeContextAllocator _s;
      return &_s;
    }
    void clear()  {
      free_small.clear();
      for( auto& i : small ) delete i.second;
      small.clear();

      free_large.clear();
      for( auto& i : large ) delete i.second;
      large.clear();
    }
    void freeBlock(void* ctxt)    {
      Small* s = (Small*)ctxt;
      switch(s->type)   {
      case 1:
        s->used = 0;
        free_small.push_back(s);
        return;
      case 2:
        s->used = 0;
        free_large.push_back((Large*)s);
        return;
      default:
        printout(ERROR,"VolumeManager",
                 "++ Hit invalid context slot:%p type:%4X",
                 (void*)s, s->type);
        return;
      }
    }
    void* alloc_large()  {
      if ( s_useAllocator )  {
        if ( !free_large.empty() )  {
          Large* entry = free_large.back();
          free_large.pop_back();
          entry->used = 1;
          return entry->context;
        }
        Large* blk = new Large[ALLOCATE_SIZE];
        for(size_t i=0; i<ALLOCATE_SIZE;++i) free_large.push_back(&blk[i]);
        large.push_back(make_pair(ALLOCATE_SIZE,blk));
        return alloc_large();
      }
      return new VolumeContextAllocator::Large();
    }
    void* alloc_small()  {
      if ( s_useAllocator )  {
        if ( !free_small.empty() )  {
          Small* entry = free_small.back();
          free_small.pop_back();
          entry->used = 1;
          return entry->context;
        }
        Small* blk = new Small[ALLOCATE_SIZE];
        for(size_t i=0; i<ALLOCATE_SIZE;++i) free_small.push_back(&blk[i]);
        small.push_back(make_pair(ALLOCATE_SIZE,blk));
        return alloc_small();
      }
      return new VolumeContextAllocator::Small();
    }
  };
  inline ContextExtension* _getExtension(const VolumeManagerContext* ctxt)  {
    VolumeContextAllocator::Large* p = (VolumeContextAllocator::Large*)ctxt;
    return (ContextExtension*)p->extension;
  }
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace detail {

    /// Helper class to populate the volume manager
    /**
     *  \author  M.Frank
     *  \version 1.0
     */
    class VolumeManager_Populator {
      typedef PlacedVolume::VolIDs VolIDs;
      typedef vector<TGeoNode*> Chain;
      typedef pair<VolumeID, VolumeID> Encoding;
      /// Reference to the Detector instance
      Detector& m_detDesc;
      /// Reference to the volume manager to be populated
      VolumeManager m_volManager;
      /// Set of already added entries
      set<VolumeID> m_entries;
      /// Debug flag
      bool          m_debug    = false;
      /// Node counter
      size_t        m_numNodes = 0;

    public:
      /// Default constructor
      VolumeManager_Populator(Detector& description, VolumeManager vm)
        : m_detDesc(description), m_volManager(vm)
      {
        m_debug = (0 != ::getenv("DD4HEP_VOLMGR_DEBUG"));
      }

      /// Access node count
      size_t numNodes()  const  {   return m_numNodes;  }

      /// Populate the Volume manager
      void populate(DetElement e) {
        //const char* typ = 0;//::getenv("VOLMGR_NEW");
        SensitiveDetector parent_sd;
        if ( e->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR )  {
          parent_sd = m_detDesc.sensitiveDetector(e.name());
        }
        //printout(INFO, "VolumeManager", "++ Executing %s plugin manager version",typ ? "***NEW***" : "***OLD***");
        for (const auto& i : e.children() )  {
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
              sd = m_detDesc.sensitiveDetector(parent.name());
            else if ( (e->flag&DetElement::Object::HAVE_SENSITIVE_DETECTOR) )
              sd = m_detDesc.sensitiveDetector(e.name());
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
            if ( !have_encoding && !compound )   {
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
          const auto& id = (*i);
          const BitFieldElement* f = iddesc.field(id.first);
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
          const auto& id = (*i);
          const BitFieldElement* f = iddesc.field(id.first);
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
            DetElement    sub_detector = m_detDesc.detector(sd_name);
            VolumeManager section      = m_volManager.addSubdetector(sub_detector, ro);

            //m_debug = true;
            // This is the block, we effectively have to save for each physical volume with a VolID
            void* mem = nodes.empty()
              ? VolumeContextAllocator::instance()->alloc_small()
              : VolumeContextAllocator::instance()->alloc_large();
            VolumeManagerContext* context = new(mem) VolumeManagerContext;
            context->identifier = code.first;
            context->mask       = code.second;
            context->element    = e;
            context->flag       = nodes.empty() ? 0 : 1;
            if ( context->flag )  {
              ContextExtension* ext = new(_getExtension(context)) ContextExtension();
              ext->placement  = PlacedVolume(n);
              for (size_t i = nodes.size(); i > 1; --i) {   // Omit the placement of the parent DetElement
                TGeoMatrix* m = nodes[i-1]->GetMatrix();
                ext->toElement.MultiplyLeft(m);
              }
            }
            if ( !section.adoptPlacement(context) || m_debug )  {
              print_node(sd, parent, e, n, code, nodes);
            }
            m_entries.insert(code.first);
            ++m_numNodes;
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
  }       /* End namespace detail                */
}         /* End namespace dd4hep                */

/// Default destructor
VolumeManagerContext::~VolumeManagerContext() {
  if ( 0 == flag ) return;
  _getExtension(this)->~ContextExtension();
}

/// Acces the sensitive volume placement
PlacedVolume VolumeManagerContext::elementPlacement()  const   {
  return element.placement();
}

/// Acces the sensitive volume placement
PlacedVolume VolumeManagerContext::volumePlacement()  const   {
  if ( 0 == flag )
    return element.placement();
  return _getExtension(this)->placement;
}

/// Access the transformation to the closest detector element
const TGeoHMatrix& VolumeManagerContext::toElement()  const   {
  static TGeoHMatrix identity;
  return ( 0 == flag ) ? identity : _getExtension(this)->toElement;
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(Detector& description, const string& nam, DetElement elt, Readout ro, int flags) {
  printout(INFO, "VolumeManager", " - populating volume ids - be patient ..."  );
  size_t node_count = 0;
  Object* obj_ptr = new Object();
  assign(obj_ptr, nam, "VolumeManager");
  if (elt.isValid()) {
    detail::VolumeManager_Populator p(description, *this);
    obj_ptr->detector = elt;
    obj_ptr->id    = ro.isValid() ? ro.idSpec() : IDDescriptor();
    obj_ptr->top   = obj_ptr;
    obj_ptr->flags = flags;
    p.populate(elt);
    node_count = p.numNodes();
  }
  printout(INFO, "VolumeManager", " - populating volume ids - done. %ld nodes.",node_count);
}

/// Initializing constructor to create a new object
VolumeManager::VolumeManager(DetElement sub_detector, Readout ro)  {
  Object* obj_ptr = new Object();
  obj_ptr->detector = sub_detector;
  obj_ptr->id = ro.isValid() ? ro.idSpec() : IDDescriptor();
  assign(obj_ptr, sub_detector.name(), "VolumeManager");
}

VolumeManager VolumeManager::getVolumeManager(Detector& description) {
  if( not description.volumeManager().isValid() ) {
    description.apply("DD4hepVolumeManager", 0, 0);
  }
  return description.volumeManager();
}

/// Add a new Volume manager section according to a new subdetector
VolumeManager VolumeManager::addSubdetector(DetElement det, Readout ro) {
  if (isValid()) {
    Object& o = _data();
    if (!det.isValid()) {
      throw runtime_error("dd4hep: VolumeManager::addSubdetector: Only valid subdetectors "
                          "are allowed. [Invalid DetElement]");
    }
    auto i = o.subdetectors.find(det);
    if (i == o.subdetectors.end()) {
      string det_name = det.name();
      // First check all pre-conditions
      if (!ro.isValid()) {
        throw runtime_error("dd4hep: VolumeManager::addSubdetector: Only subdetectors with a "
                            "valid readout descriptor are allowed. [Invalid DetElement:" + det_name + "]");
      }
      PlacedVolume pv = det.placement();
      if (!pv.isValid()) {
        throw runtime_error("dd4hep: VolumeManager::addSubdetector: Only subdetectors with a "
                            "valid placement are allowed. [Invalid DetElement:" + det_name + "]");
      }
      auto vit = pv.volIDs().find("system");
      if (vit == pv.volIDs().end()) {
        throw runtime_error("dd4hep: VolumeManager::addSubdetector: Only subdetectors with "
                            "valid placement VolIDs are allowed. [Invalid DetElement:" + det_name + "]");
      }

      i = o.subdetectors.insert(make_pair(det, VolumeManager(det,ro))).first;
      const auto& id = (*vit);
      VolumeManager m = (*i).second;
      const BitFieldElement* field = ro.idSpec().field(id.first);
      if (!field) {
        throw runtime_error("dd4hep: VolumeManager::addSubdetector: IdDescriptor of " + 
                            string(det.name()) + " has no field " + id.first);
      }
      Object& mo = m._data();
      mo.top     = o.top;
      mo.flags   = o.flags;
      mo.system  = field;
      mo.sysID   = id.second;
      mo.detMask = mo.sysID;
      o.managers[mo.sysID] = m;
      det.callAtUpdate(DetElement::PLACEMENT_CHANGED|DetElement::PLACEMENT_DETECTOR,
                       &mo,&Object::update);
    }
    return (*i).second;
  }
  throw runtime_error("dd4hep: VolumeManager::addSubdetector: "
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
    throw runtime_error("dd4hep: VolumeManager::subdetector(VolID): "
                        "Attempt to access unknown subdetector section.");
  }
  throw runtime_error("dd4hep: VolumeManager::subdetector(VolID): "
                      "Cannot assign ID descriptor [Invalid Manager Handle]");
}

/// Access the top level detector element
DetElement VolumeManager::detector() const {
  if (isValid()) {
    return _data().detector;
  }
  throw runtime_error("dd4hep: VolumeManager::detector: Cannot access DetElement [Invalid Handle]");
}

/// Access IDDescription structure
IDDescriptor VolumeManager::idSpec() const {
  return _data().id;
}

/// Register physical volume with the manager (normally: section manager)
bool VolumeManager::adoptPlacement(VolumeID sys_id, VolumeManagerContext* context) {
  stringstream err;
  Object&  o      = _data();
  VolumeID vid    = context->identifier;
  VolumeID mask   = context->mask;
  PlacedVolume pv = context->elementPlacement();
  auto i = o.volumes.find(vid);

  if ( (vid&mask) != vid ) {
    err << "Bad context mask:" << (void*)mask
        << " id:" << (void*)vid
        << " pv:" << pv.name()
        << " Sensitive:" << yes_no(pv.volume().isSensitive())
        << endl;
    goto Fail;
  }

  if ( i == o.volumes.end()) {
    o.volumes[vid] = context;
    o.detMask |= mask;
    err << "Inserted new volume:" << setw(6) << left << o.volumes.size()
        << " Ptr:"  << (void*) pv.ptr()
        << " ["     << pv.name() << "]"
        << " id:"   << setw(16) << hex << right << setfill('0') << vid  << dec << setfill(' ')
        << " mask:" << setw(16) << hex << right << setfill('0') << mask << dec << setfill(' ')
        << " Det:"  << setw(4)  << hex << right << setfill('0') << context->element.volumeID()
        << " / "    << setw(4)  << sys_id << dec << setfill(' ') << ": " << context->element.path();
    printout(VERBOSE, "VolumeManager", err.str().c_str());
    //printout(ALWAYS, "VolumeManager", err.str().c_str());
    return true;
  }
  err << "+++ Attempt to register duplicate"
      << " id:"   << setw(16) << hex << right << setfill('0') << vid  << dec << setfill(' ')
      << " mask:" << setw(16) << hex << right << setfill('0') << mask << dec << setfill(' ')
      << " to detector " << o.detector.name()
      << " ptr:" << (void*) pv.ptr()
      << " Name:" << pv.name()
      << " Sensitive:" << yes_no(pv.volume().isSensitive())
      << endl;
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  err.str("");
  context = (*i).second;
  //pv = context->placement;
  err << " !!!!!               +++ Clashing"
      << " id:"   << setw(16) << hex << right << setfill('0') << vid  << dec << setfill(' ')
      << " mask:" << setw(16) << hex << right << setfill('0') << mask << dec << setfill(' ')
      << " to detector " << o.detector.name()
      << " ptr:" << (void*) pv.ptr()
      << " Name:" << pv.name()
      << " Sensitive:" << yes_no(pv.volume().isSensitive())
      << endl;
 Fail:
  printout(ERROR, "VolumeManager", "%s", err.str().c_str());
  // throw runtime_error(err.str());
  return false;
}

/// Register physical volume with the manager (normally: section manager)
bool VolumeManager::adoptPlacement(VolumeManagerContext* context) {
  stringstream err;
  if ( isValid() ) {
    Object& o = _data();
    if ( context )   {
      if ( (o.flags & ONE) == ONE ) {
        VolumeManager top(o.top);
        return top.adoptPlacement(context);
      }
      if ( (o.flags & TREE) == TREE ) {
        bool isTop = ptr() == o.top;
        if ( !isTop ) {
          VolumeID sys_id = o.system->value(context->identifier);
          if ( sys_id == o.sysID ) {
            return adoptPlacement(sys_id, context);
          }
          VolumeManager top(o.top);
          return top.adoptPlacement(context);
        }
        for( auto& j : o.managers )  {
          Object& m = j.second._data();
          VolumeID sid = m.system->value(context->identifier);
          if ( j.first == sid ) {
            return j.second.adoptPlacement(sid, context);
          }
        }
      }
      return false;
    }
    except("VolumeManager","dd4hep: Failed to add new physical volume to detector: %s "
           "[Invalid Context]", o.detector.name());
  }
  except("VolumeManager","dd4hep: Failed to add new physical volume [Invalid Manager Handle]");
  return false;
}

/// Lookup the context, which belongs to a registered physical volume.
VolumeManagerContext* VolumeManager::lookupContext(VolumeID volume_id) const {
  if (isValid()) {
    VolumeManagerContext* c = 0;
    const Object& o = _data();
    bool is_top = o.top == ptr();
    bool one_tree = (o.flags & ONE) == ONE;
    if ( !is_top && one_tree ) {
      return VolumeManager(o.top).lookupContext(volume_id);
    }
    VolumeID id = volume_id;
    /// First look in our own volume cache if the entry is found.
    c = o.search(id);
    if (c)
      return c;
    /// Second: look in the subdetector volume cache if the entry is found.
    if (!one_tree) {
      for (const auto& j : o.subdetectors )  {
        if ((c = j.second._data().search(id)) != 0)
          return c;
      }
    }
    except("VolumeManager","lookupContext: Failed to search Volume context %016llX [Unknown identifier]", (void*)volume_id);
  }
  except("VolumeManager","lookupContext: Failed to search Volume context [Invalid Manager Handle]");
  return 0;
}

/// Lookup a physical (placed) volume identified by its 64 bit hit ID
PlacedVolume VolumeManager::lookupDetElementPlacement(VolumeID volume_id) const {
  VolumeManagerContext* c = lookupContext(volume_id); // Throws exception if not found!
  return c->elementPlacement();
}

/// Lookup a physical (placed) volume identified by its 64 bit hit ID
PlacedVolume VolumeManager::lookupVolumePlacement(VolumeID volume_id) const {
  VolumeManagerContext* c = lookupContext(volume_id); // Throws exception if not found!
  return c->volumePlacement();
}

/// Lookup a top level subdetector detector element according to a contained 64 bit hit ID
DetElement VolumeManager::lookupDetector(VolumeID volume_id) const {
  if (isValid()) {
    const Object& o = _data();
    VolumeID      sys_id = 0;
    if ( o.system )   {
      sys_id = o.system->value(volume_id);
    }
    else  {
      for (const auto& j : o.subdetectors )  {
        if ( j.second->system )  {
          VolumeID vid = volume_id&j.second->system->mask();
          if ( (volume_id&j.second->sysID) == vid )  {
            sys_id = j.second->sysID;
            break;
          }
        }
      }
    }
    VolumeID det_id = (volume_id&sys_id);
    VolumeManagerContext* c = lookupContext(det_id); // Throws exception if not found!
    return c->element;
  }
  except("VolumeManager","lookupContext: Failed to search Volume context [Invalid Manager Handle]");
  return DetElement();
}

/// Lookup the closest subdetector detector element in the hierarchy according to a contained 64 bit hit ID
DetElement VolumeManager::lookupDetElement(VolumeID volume_id) const {
  VolumeManagerContext* c = lookupContext(volume_id); // Throws exception if not found!
  return c->element;
}

/// Access the transformation of a physical volume to the world coordinate system
const TGeoMatrix&
VolumeManager::worldTransformation(const ConditionsMap& mapping,
                                   VolumeID volume_id) const
{
  VolumeManagerContext* c = lookupContext(volume_id); // Throws exception if not found!
  Alignment a = mapping.get(c->element,align::Keys::alignmentKey);
  return a.worldTransformation();
}

/// Enable printouts for debugging
std::ostream& dd4hep::operator<<(std::ostream& os, const VolumeManager& m) {
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
  for ( const auto& i : o.volumes ) {
    const VolumeManagerContext* c = i.second;
    os << prefix
       << "Element:" << setw(32) << left << c->element.path()
      //<< " pv:"     << setw(32) << left << c->placement().name()
       << " id:"     << setw(18) << left << (void*) c->identifier
       << " mask:"   << setw(18) << left << (void*) c->mask
       << endl;
  }
  for( const auto& i : o.managers )
    os << prefix << i.second << endl;
  return os;
}

/// Default destructor
VolumeManagerObject::~VolumeManagerObject() {
  /// Cleanup volume tree
  if ( s_useAllocator )  {
    VolumeContextAllocator::instance()->clear();
  }
  else   {
    destroyObjects(volumes);
  }
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
    printout(DEBUG,"VolumeManager","+++ Alignment update %s",i.second->elementPlacement().name());
}

/// Search the locally cached volumes for a matching ID
VolumeManagerContext* VolumeManagerObject::search(const VolumeID& vol_id) const {
  auto i = volumes.find(vol_id&detMask);
  return (i == volumes.end()) ? 0 : (*i).second;
}

