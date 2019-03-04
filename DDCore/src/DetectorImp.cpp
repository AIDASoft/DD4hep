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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/DetectorTools.h"

#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/VolumeManagerInterna.h"
#include "DD4hep/detail/OpticalSurfaceManagerInterna.h"
#include "DetectorImp.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <mutex>

// ROOT inlcude files
#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TClass.h"

#include "XML/DocumentHandler.h"

#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace dd4hep {
  namespace xml {
    typedef xercesc::DOMException XmlException;
  }
}
#endif

using namespace dd4hep;
using namespace std;

ClassImp(DetectorImp)

namespace {
  struct TypePreserve {
    DetectorBuildType& m_t;
    TypePreserve(DetectorBuildType& t)
      : m_t(t) {
    }
    ~TypePreserve() {
      m_t = BUILD_NONE;
    }
  };
  struct Instances  {
    recursive_mutex  lock;
    map<string, Detector*> detectors;
    Instances() = default;
    ~Instances() = default;
    Detector* get(const string& name)   {
      auto i = detectors.find(name);
      return i==detectors.end() ? 0 : (*i).second;
    }
    void insert(const string& name, Detector* detector)   {
      auto i = detectors.find(name);
      if ( i==detectors.end() )   {
        detectors.insert(make_pair(name,detector));
        return;
      }
      except("DD4hep","Cannot insert detector instance %s [Already present]",name.c_str());
    }
    Detector* remove(const string& name)   {
      auto i = detectors.find(name);
      if ( i==detectors.end() )  {
        detectors.erase(i);
        return (*i).second;
      }
      return 0;
    }
  } s_instances;

  void description_unexpected()    {
    try  {
      throw;
    }  catch( exception& e )  {
      cout << "\n"
           << "**************************************************** \n"
           << "*  A runtime error has occured :                     \n"
           << "*    " << e.what()   << endl
           << "*  the program will have to be terminated - sorry.   \n"
           << "**************************************************** \n"
           << endl ;

      set_unexpected(std::unexpected);
      set_terminate(std::terminate);
      // this provokes ROOT seg fault and stack trace (comment out to avoid it)
      exit(1) ;
    }
  }
}

string dd4hep::versionString(){
  string vs("vXX-YY") ;
  sprintf( &vs[0] , "v%2.2d-%2.2d", DD4HEP_MAJOR_VERSION, DD4HEP_MINOR_VERSION  ) ;
  return vs;
}

unique_ptr<Detector> Detector::make_unique(const std::string& name)   {
  Detector* description = new DetectorImp(name);
  return unique_ptr<Detector>(description);
}

Detector& Detector::getInstance(const std::string& name)   {
  lock_guard<recursive_mutex> lock(s_instances.lock);
  Detector* description = s_instances.get(name);
  if ( 0 == description )   {
    gGeoManager = 0;
    description = new DetectorImp(name);
    s_instances.insert(name,description);
  }
  return *description;
}

/// Destroy the instance
void Detector::destroyInstance(const std::string& name) {
  lock_guard<recursive_mutex> lock(s_instances.lock);
  Detector* description = s_instances.remove(name);
  if (description)
    delete description;
}

/// Default constructor used by ROOT I/O
DetectorImp::DetectorImp()
  : TNamed(), DetectorData(), DetectorLoad(this), m_buildType(BUILD_NONE)
{
}

/// Initializing constructor
DetectorImp::DetectorImp(const string& name)
  : TNamed(), DetectorData(), DetectorLoad(this), m_buildType(BUILD_NONE)
{
  SetTitle("DD4hep detector description object");
  set_unexpected( description_unexpected ) ;
  set_terminate( description_unexpected ) ;
  InstanceCount::increment(this);
  //if ( gGeoManager ) delete gGeoManager;
  m_manager = new TGeoManager(name.c_str(), "Detector Geometry");
  {
    gGeoManager = m_manager;
#if 0 //FIXME: eventually this should be set to 1 - needs fixes in examples ...
    TGeoElementTable*	table = m_manager->GetElementTable();
    table->TGeoElementTable::~TGeoElementTable();
    new(table) TGeoElementTable();
    // This will initialize the table without filling:
    table->AddElement("VACUUM","VACUUM"   ,0,   0, 0.0);
    table->Print();
#endif
  }
  if ( 0 == gGeoIdentity )
  {
    gGeoIdentity = new TGeoIdentity();
  }
  m_surfaceManager = new detail::OpticalSurfaceManagerObject(*this);

  VisAttr attr("invisible");
  attr.setColor(0.5, 0.5, 0.5);
  attr.setAlpha(1);
  attr.setLineStyle(VisAttr::SOLID);
  attr.setDrawingStyle(VisAttr::SOLID);
  attr.setVisible(false);
  attr.setShowDaughters(true);
  addVisAttribute(attr);
  m_invisibleVis = attr;
}

/// Standard destructor
DetectorImp::~DetectorImp() {
  if ( m_manager )  {
    lock_guard<recursive_mutex> lock(s_instances.lock);
    if ( m_manager == gGeoManager ) gGeoManager = 0;
    Detector* description = s_instances.get(m_manager->GetName());
    if ( 0 != description )   {
      s_instances.remove(m_manager->GetName());
    }
  }
  deletePtr(m_surfaceManager);
  destroyData(true);
  m_extensions.clear();
  InstanceCount::decrement(this);
}

/// ROOT I/O call
Int_t DetectorImp::saveObject(const char *name, Int_t option, Int_t bufsize) const   {
  Int_t nbytes = 0;
  try  {
    DetectorData::patchRootStreamer(TGeoVolume::Class());
    DetectorData::patchRootStreamer(TGeoNode::Class());
    nbytes = TNamed::Write(name, option, bufsize);
    DetectorData::unpatchRootStreamer(TGeoVolume::Class());
    DetectorData::unpatchRootStreamer(TGeoNode::Class());
    return nbytes;
  }
  catch (const exception& e) {
    DetectorData::unpatchRootStreamer(TGeoVolume::Class());
    DetectorData::unpatchRootStreamer(TGeoNode::Class());
    except("Detector","Exception %s while saving dd4hep::Detector object",e.what());
  }
  catch (...) {
    DetectorData::unpatchRootStreamer(TGeoVolume::Class());
    DetectorData::unpatchRootStreamer(TGeoNode::Class());
    except("Detector","UNKNOWN exception while saving dd4hep::Detector object.");
  }
  return nbytes;
}

// Load volume manager
void DetectorImp::imp_loadVolumeManager()   {
  detail::destroyHandle(m_volManager);
  m_volManager = VolumeManager(*this, "World", world(), Readout(), VolumeManager::TREE);
}

/// Add an extension object to the Detector instance
void* DetectorImp::addUserExtension(unsigned long long int key, ExtensionEntry* entry) {
  return m_extensions.addExtension(key,entry);
}

/// Remove an existing extension object from the Detector instance
void* DetectorImp::removeUserExtension(unsigned long long int key, bool destroy)  {
  return m_extensions.removeExtension(key,destroy);
}

/// Access an existing extension object from the Detector instance
void* DetectorImp::userExtension(unsigned long long int key, bool alert) const {
  return m_extensions.extension(key,alert);
}

/// Register new mother volume using the detector name.
void DetectorImp::declareParent(const string& detector_name, const DetElement& parent)  {
  if ( !detector_name.empty() )  {
    if ( parent.isValid() )  {
      auto i = m_detectorParents.find(detector_name);
      if (i == m_detectorParents.end())   {
        Volume parent_volume = parent.placement().volume();
        if ( parent_volume.isValid() )   {
          m_detectorParents.insert(make_pair(detector_name,parent));
          return;
        }
        except("DD4hep","+++ Failed to access valid parent volume of %s from %s",
               detector_name.c_str(), parent.name());
      }
      except("DD4hep",
             "+++ A parent to the detector %s was already registered.",
             detector_name.c_str());
    }
    except("DD4hep",
           "+++ Attempt to register invalid parent for detector: %s [Invalid-Handle].",
           detector_name.c_str());
  }
  except("DD4hep",
         "+++ Attempt to register parent to invalid detector [Invalid-detector-name].");
}

/// Access mother volume by detector element
Volume DetectorImp::pickMotherVolume(const DetElement& de) const {
  if ( de.isValid() )   {
    string de_name = de.name();
    auto i = m_detectorParents.find(de_name);
    if (i == m_detectorParents.end())   {
      if ( m_worldVol.isValid() )  {
        return m_worldVol;
      }
      except("DD4hep",
             "+++ The world volume is not (yet) valid. "
             "Are you correctly building detector %s?",
             de.name());
    }
    if ( (*i).second.isValid() )  {
      Volume vol = (*i).second.volume();
      if ( vol.isValid() )  {
        return vol;
      }
    }
    except("DD4hep",
           "+++ The mother volume of %s is not valid. "
           "Are you correctly building detectors?",
           de.name());
  }
  except("DD4hep","Detector: Attempt access mother volume of invalid detector [Invalid-handle]");
  return 0;
}

/// Retrieve a subdetector element by it's name from the detector description
DetElement DetectorImp::detector(const std::string& name) const  {
  HandleMap::const_iterator i = m_detectors.find(name);
  if (i != m_detectors.end()) {
    return (*i).second;
  }
  DetElement de = detail::tools::findElement(*this,name);
  return de;
}

Detector& DetectorImp::addDetector(const Handle<NamedObject>& ref_det) {
  DetElement det_element(ref_det);
  DetectorHelper helper(this);
  DetElement existing_det = helper.detectorByID(det_element.id());

  if ( existing_det.isValid() )   {
    SensitiveDetector sd = helper.sensitiveDetector(existing_det);
    if ( sd.isValid() )   {
      stringstream str;
      str << "Detector: The sensitive sub-detectors " << det_element.name() << " and "
          << existing_det.name() << " have the identical ID:" << det_element.id() << ".";
      except("DD4hep",str.str());
    }
  }
  m_detectors.append(ref_det);
  det_element->flag |= DetElement::Object::IS_TOP_LEVEL_DETECTOR;
  Volume volume = det_element.placement()->GetMotherVolume();
  if ( volume == m_worldVol )  {
    printout(DEBUG,"DD4hep","+++ Detector: Added detector %s to the world instance.",
             det_element.name());
    m_world.add(det_element);
    return *this;
  }
  /// Check if the parent is part of the compounds
  auto ipar = m_detectorParents.find(det_element.name());
  if (ipar != m_detectorParents.end())   {
    DetElement parent = (*ipar).second;
    parent.add(det_element);
    printout(DEBUG,"DD4hep","+++ Detector: Added detector %s to parent %s.",
             det_element.name(), parent.name());
    return *this;
  }

  // The detector's placement must be one of the existing detectors
  for(HandleMap::iterator idet = m_detectors.begin(); idet != m_detectors.end(); ++idet)  {
    DetElement parent((*idet).second);
    Volume vol = parent.placement().volume();
    if ( vol == volume )  {
      printout(INFO,"DD4hep","+++ Detector: Added detector %s to the parent:%s.",
               det_element.name(),parent.name());
      parent.add(det_element);
      return *this;
    }
  }
  except("DD4hep","+++ Detector: The detector %s has no known parent.", det_element.name());
  throw runtime_error("Detector-Error"); // Never called....
}

/// Add a new constant by named reference to the detector description
Detector& DetectorImp::addConstant(const Handle<NamedObject>& x) {
  if ( strcmp(x.name(),"Detector_InhibitConstants") == 0 )   {
    const char* title = x->GetTitle();
    char c = ::toupper(title[0]);
    m_inhibitConstants = (c=='Y' || c=='T' || c=='1');
  }
  m_define.append(x, false);
  return *this;
}

/// Retrieve a constant by it's name from the detector description
Constant DetectorImp::constant(const string& name) const {
  if ( !m_inhibitConstants )   {
    return getRefChild(m_define, name);
  }
  throw runtime_error("Detector:constant("+name+"): Access to global constants is inhibited.");
}

/// Typed access to constants: access string values
string DetectorImp::constantAsString(const string& name) const {
  if ( !m_inhibitConstants )   {
    Handle<NamedObject> c = constant(name);
    if (c.isValid())
      return c->GetTitle();
    throw runtime_error("Detector:constantAsString: The constant " + name + " is not known to the system.");
  }
  throw runtime_error("Detector:constantAsString("+name+"):: Access to global constants is inhibited.");
}

/// Typed access to constants: long values
long DetectorImp::constantAsLong(const string& name) const {
  if ( !m_inhibitConstants )   {
    return _toLong(constantAsString(name));
  }
  throw runtime_error("Detector:constantAsLong("+name+"): Access to global constants is inhibited.");
}

/// Typed access to constants: double values
double DetectorImp::constantAsDouble(const string& name) const {
  if ( !m_inhibitConstants )   {
    return _toDouble(constantAsString(name));
  }
  throw runtime_error("Detector:constantAsDouble("+name+"): Access to global constants is inhibited.");
}

/// Add a field component by named reference to the detector description
Detector& DetectorImp::addField(const Handle<NamedObject>& x) {
  m_field.add(x);
  m_fields.append(x);
  return *this;
}

/// Retrieve a matrial by it's name from the detector description
Material DetectorImp::material(const string& name) const {
  TGeoMedium* mat = m_manager->GetMedium(name.c_str());
  if (mat) {
    return Material(mat);
  }
  throw runtime_error("Cannot find a material referenced by name:" + name);
}

/// Internal helper to map detector types once the geometry is closed
void DetectorImp::mapDetectorTypes()  {
  for( const auto& i : m_detectors )   {
    DetElement det(i.second);
    if ( det.parent().isValid() )  { // Exclude 'world'
      HandleMap::const_iterator j=m_sensitive.find(det.name());
      if ( j != m_sensitive.end() )  {
        SensitiveDetector sd((*j).second);
        m_detectorTypes[sd.type()].push_back(det);
      }
      else if ( det.type() == "compound" )  {
        m_detectorTypes[det.type()].push_back(det);      
      }
      else  {
        m_detectorTypes["passive"].push_back(det);      
      }
    }
  }
}

/// Access the availible detector types
vector<string> DetectorImp::detectorTypes() const  {
  if ( m_manager->IsClosed() ) {
    vector<string> v;
    for(DetectorTypeMap::const_iterator i=m_detectorTypes.begin(); i!=m_detectorTypes.end(); ++i)  
      v.push_back((*i).first);
    return v;
  }
  throw runtime_error("detectorTypes: Call only available once the geometry is closed!");
}

/// Access a set of subdetectors according to the sensitive type.
const vector<DetElement>& DetectorImp::detectors(const string& type, bool throw_exc)  {
  if ( m_manager->IsClosed() ) {
    if ( throw_exc )  {
      DetectorTypeMap::const_iterator i=m_detectorTypes.find(type);
      if ( i != m_detectorTypes.end() ) return (*i).second;
      throw runtime_error("detectors("+type+"): Detectors of this type do not exist in the current setup!");
    }
    // return empty vector instead of exception
    return m_detectorTypes[ type ] ;
  }
  throw runtime_error("detectors("+type+"): Detectors can only selected by type once the geometry is closed!");
}

vector<DetElement> DetectorImp::detectors(unsigned int includeFlag, unsigned int excludeFlag ) const  {
  if( ! m_manager->IsClosed() ) {
    throw runtime_error("detectors(typeFlag): Detectors can only selected by typeFlag once the geometry is closed!");
  }
  vector<DetElement> dets ;
  dets.reserve( m_detectors.size() ) ;
  
  for(HandleMap::const_iterator i=m_detectors.begin(); i!=m_detectors.end(); ++i)   {
    DetElement det((*i).second);
    if ( det.parent().isValid() )  { // Exclude 'world'
      
      //fixme: what to do with compounds - add their daughters  ?
      // ...

      if( ( det.typeFlag() &  includeFlag ) == includeFlag &&
	  ( det.typeFlag() &  excludeFlag ) ==  0 )
	dets.push_back( det ) ;
    }
  }
  return dets ;
}

/// Access a set of subdetectors according to several sensitive types.
vector<DetElement> DetectorImp::detectors(const string& type1,
                                          const string& type2,
                                          const string& type3,
                                          const string& type4,
                                          const string& type5 )  {
  if ( m_manager->IsClosed() ) {
    vector<DetElement> v;
    DetectorTypeMap::const_iterator i, end=m_detectorTypes.end();
    if ( !type1.empty() && (i=m_detectorTypes.find(type1)) != end )
      v.insert(v.end(),(*i).second.begin(),(*i).second.end());
    if ( !type2.empty() && (i=m_detectorTypes.find(type2)) != end )
      v.insert(v.end(),(*i).second.begin(),(*i).second.end());
    if ( !type3.empty() && (i=m_detectorTypes.find(type3)) != end )
      v.insert(v.end(),(*i).second.begin(),(*i).second.end());
    if ( !type4.empty() && (i=m_detectorTypes.find(type4)) != end )
      v.insert(v.end(),(*i).second.begin(),(*i).second.end());
    if ( !type5.empty() && (i=m_detectorTypes.find(type5)) != end ) 
      v.insert(v.end(),(*i).second.begin(),(*i).second.end());
    return v;
  }
  throw runtime_error("detectors("+type1+","+type2+",...): Detectors can only selected by type once the geometry is closed!");
}

Handle<NamedObject> DetectorImp::getRefChild(const HandleMap& e, const string& name, bool do_throw) const {
  HandleMap::const_iterator i = e.find(name);
  if (i != e.end()) {
    return (*i).second;
  }
  if (do_throw) {
    int cnt = 0;
    cout << "GetRefChild: Failed to find child with name: " << name
         << " Map contains " << e.size() << " elements." << endl;
    for(i=e.begin(); i!=e.end(); ++i)
      cout << "   " << cnt << "  " << (*i).first << endl;
    throw runtime_error("Cannot find a child with the reference name:" + name);
  }
  return 0;
}

namespace {
  struct ShapePatcher: public detail::GeoScan {
    VolumeManager m_volManager;
    DetElement m_world;
    ShapePatcher(VolumeManager m, DetElement e)
      : detail::GeoScan(e), m_volManager(m), m_world(e) {
    }
    void patchShapes() {
      auto&  data = *m_data;
      char   text[32];
      string nam;
      printout(INFO,"Detector","+++ Patching names of anonymous shapes....");
      for (auto i = data.rbegin(); i != data.rend(); ++i) {
        for( const TGeoNode* n : (*i).second )  {
          TGeoVolume* vol = n->GetVolume();
          TGeoShape*  s   = vol->GetShape();
          const char* sn  = s->GetName();
          ::snprintf(text,sizeof(text),"_shape_%p",(void*)s);
          if (0 == sn || 0 == ::strlen(sn)) {
            nam = vol->GetName();
            nam += text;
            s->SetName(nam.c_str());
          }
          else if (0 == ::strcmp(sn, s->IsA()->GetName())) {
            nam = vol->GetName();
            nam += text;
            s->SetName(nam.c_str());
          }
          else {
            nam = sn;
            if (nam.find("_shape") == string::npos)
              nam += text;
            s->SetName(nam.c_str());
          }
          if (s->IsA() == TGeoCompositeShape::Class()) {
            TGeoCompositeShape* c = (TGeoCompositeShape*) s;
            const TGeoBoolNode* boolean = c->GetBoolNode();
            s = boolean->GetLeftShape();
            sn = s->GetName();
            if (0 == sn || 0 == ::strlen(sn)) {
              s->SetName((nam + "_left").c_str());
            }
            else if (0 == ::strcmp(sn, s->IsA()->GetName())) {
              s->SetName((nam + "_left").c_str());
            }
            s = boolean->GetRightShape();
            sn = s->GetName();
            if (0 == sn || 0 == ::strlen(sn)) {
              s->SetName((nam + "_right").c_str());
            }
            else if (0 == ::strcmp(s->GetName(), s->IsA()->GetName())) {
              s->SetName((nam + "_right").c_str());
            }
          }
        }
      }
    }
  };

}

/// Finalize/close the geometry
void DetectorImp::endDocument(bool close_geometry)    {
  TGeoManager* mgr = m_manager;
  if ( close_geometry && !mgr->IsClosed() )  {
#if 0
    Region trackingRegion("TrackingRegion");
    trackingRegion.setThreshold(1);
    trackingRegion.setStoreSecondaries(true);
    add(trackingRegion);
    m_trackingVol.setRegion(trackingRegion);
    // Set the tracking volume to invisible.
    VisAttr trackingVis("TrackingVis");
    trackingVis.setVisible(false);
    m_trackingVol.setVisAttributes(trackingVis);
    add(trackingVis);
#endif
    /// Since we allow now for anonymous shapes,
    /// we will rename them to use the name of the volume they are assigned to
    mgr->CloseGeometry();
  }
  ShapePatcher patcher(m_volManager, m_world);
  patcher.patchShapes();
  mapDetectorTypes();
  m_state = READY;
}

/// Initialize the geometry and set the bounding box of the world volume
void DetectorImp::init() {
  if (!m_world.isValid()) {
    TGeoManager* mgr = m_manager;
    Constant     air_const = getRefChild(m_define, "Air", false);
    Constant     vac_const = getRefChild(m_define, "Vacuum", false);
    Box          worldSolid;
    
    m_materialAir    = material(air_const.isValid() ? air_const->GetTitle() : "Air");
    m_materialVacuum = material(vac_const.isValid() ? vac_const->GetTitle() : "Vacuum");

    m_worldVol = m_manager->GetTopVolume();
    if ( m_worldVol.isValid() )   {
      worldSolid = m_worldVol.solid();
      printout(INFO,"Detector", "*********** Use Top Node from manager as "
               "world volume [%s].  BBox: %4.0f %4.0f %4.0f",
               worldSolid->IsA()->GetName(),
               worldSolid->GetDX(), worldSolid->GetDY(), worldSolid->GetDZ());
    }
    else   {
      /// Construct the top level world element
      Solid parallelWorldSolid = Box("world_x", "world_y", "world_z");
      worldSolid = Box("world_x", "world_y", "world_z");
      m_worldVol = Volume("world_volume", worldSolid, m_materialAir);
      parallelWorldSolid->SetName("parallel_world_solid");
      printout(INFO,"Detector","*********** Created World volume with size: %4.0f %4.0f %4.0f",
               worldSolid->GetDX(), worldSolid->GetDY(), worldSolid->GetDZ());
    }
    m_world = DetElement(new WorldObject(*this,"world"));
    /// Set the world volume to invisible.
    VisAttr worldVis = visAttributes("WorldVis");
    if ( !worldVis.isValid() )  {
      worldVis = VisAttr("WorldVis");
      worldVis.setAlpha(1.0);
      worldVis.setVisible(false);
      worldVis.setShowDaughters(true);
      worldVis.setColor(1.0, 1.0, 1.0);
      worldVis.setLineStyle(VisAttr::SOLID);
      worldVis.setDrawingStyle(VisAttr::WIREFRAME);
      //m_worldVol.setVisAttributes(worldVis);
      m_worldVol->SetVisibility(kFALSE);
      m_worldVol->SetVisDaughters(kTRUE);
      m_worldVol->SetVisContainers(kTRUE);
      add(worldVis);
    }
    m_worldVol.setVisAttributes(worldVis);
    m_manager->SetTopVolume(m_worldVol.ptr());

    /// Set the top level volume to the TGeomanager
    m_detectors.append(m_world);
    m_world.setPlacement(mgr->GetTopNode());

    /// Construct the parallel world
    m_parallelWorldVol = Volume("parallel_world_volume", worldSolid, m_materialAir);

    /// Construct the field envelope
    m_field = OverlayedField("global");
    m_state = LOADING;
  }
}

/// Read any geometry description or alignment file
void DetectorImp::fromXML(const string& xmlfile, DetectorBuildType build_type) {
  TypePreserve build_type_preserve(m_buildType = build_type);
  processXML(xmlfile,0);
}

/// Read any geometry description or alignment file with external XML entity resolution
void DetectorImp::fromXML(const string& fname, xml::UriReader* entity_resolver, DetectorBuildType build_type)  {
  TypePreserve build_type_preserve(m_buildType = build_type);
  processXML(fname,entity_resolver);
}

void DetectorImp::dump() const {
  TGeoManager* mgr = m_manager;
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");
}

/// Manipulate geometry using facroy converter
long DetectorImp::apply(const char* factory_type, int argc, char** argv) {
  string fac = factory_type;
  try {
    long result = PluginService::Create<long>(fac, (Detector*) this, argc, argv);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(fac, (Detector*) this, argc, argv);
      if ( 0 == result )  {
        throw runtime_error("dd4hep: apply-plugin: Failed to locate plugin " +
                            fac + ". " + dbg.missingFactory(fac));
      }
    }
    result = *(long*) result;
    if (result != 1) {
      throw runtime_error("dd4hep: apply-plugin: Failed to execute plugin " + fac);
    }
    return result;
  }
  catch (const xml::XmlException& e) {
    throw runtime_error(xml::_toString(e.msg) + "\ndd4hep: XML-DOM Exception with plugin:" + fac);
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\ndd4hep: with plugin:" + fac);
  }
  catch (...) {
    throw runtime_error("UNKNOWN exception from plugin:" + fac);
  }
  return EINVAL;
}
