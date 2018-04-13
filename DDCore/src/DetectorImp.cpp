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
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/VolumeManagerInterna.h"
#include "DetectorImp.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>
#include <cerrno>

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
  static Detector* s_description = 0;

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

Detector& Detector::getInstance() {
  if (!s_description)
    s_description = new DetectorImp();
  return *s_description;
}

/// Destroy the instance
void Detector::destroyInstance() {
  if (s_description)
    delete s_description;
  s_description = 0;
}

/// Default constructor
DetectorImp::DetectorImp()
  : DetectorData(), DetectorLoad(this), m_buildType(BUILD_NONE)
{
  set_unexpected( description_unexpected ) ;
  set_terminate( description_unexpected ) ;
  
  InstanceCount::increment(this);
  if (0 == gGeoManager) {
    gGeoManager = new TGeoManager("world", "Detector Geometry");
  }
  {
    m_manager = gGeoManager;
#if 0 //FIXME: eventually this should be set to 1 - needs fixes in examples ...
    TGeoElementTable*	table = m_manager->GetElementTable();
    table->TGeoElementTable::~TGeoElementTable();
    new(table) TGeoElementTable();
    // This will initialize the table without filling:
    table->AddElement("VACUUM","VACUUM"   ,0,   0, 0.0);
    table->Print();
#endif
  }
  //if ( 0 == gGeoIdentity )
  {
    gGeoIdentity = new TGeoIdentity();
  }
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
  if ( m_manager == gGeoManager ) gGeoManager = 0;
  destroyData(false);
  m_extensions.clear();
  InstanceCount::decrement(this);
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
void DetectorImp::declareMotherVolume(const string& detector_name, const Volume& vol)  {
  if ( !detector_name.empty() )  {
    if ( vol.isValid() )  {
      auto i = m_motherVolumes.find(detector_name);
      if (i == m_motherVolumes.end())   {
        m_motherVolumes.insert(make_pair(detector_name,vol));
        return;
      }
      except("DD4hep",
             "+++ A mother volume to the detector %s was already registered.",
             detector_name.c_str());
    }
    except("DD4hep",
           "+++ Attempt to register invalid mother volume for detector: %s [Invalid-Handle].",
           detector_name.c_str());
  }
  except("DD4hep",
         "+++ Attempt to register mother volume to invalid detector [Invalid-detector-name].");
}

/// Access mother volume by detector element
Volume DetectorImp::pickMotherVolume(const DetElement& de) const {
  if ( de.isValid() )   {
    string de_name = de.name();
    auto i = m_motherVolumes.find(de_name);
    if (i == m_motherVolumes.end())   {
      if ( m_worldVol.isValid() )  {
        return m_worldVol;
      }
      except("DD4hep",
             "+++ The world volume is not (yet) valid. Are you correctly building detector %s?",
             de.name());
    }
    if ( (*i).second.isValid() )  {
      return (*i).second;
    }
    except("DD4hep",
           "+++ The mother volume of %s is not valid. Are you correctly building detectors?",
           de.name());
  }
  except("DD4hep","Detector: Attempt access mother volume of invalid detector [Invalid-handle]");
  return 0;
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
    printout(DEBUG,"Detector","Added detector %s to the world instance.",det_element.name());
    m_world.add(det_element);
    return *this;
  }
  // The detector's placement must be one of the existing detectors
  for(HandleMap::iterator i = m_detectors.begin(); i!=m_detectors.end(); ++i)  {
    DetElement parent((*i).second);
    Volume vol = parent.placement().volume();
    if ( vol == volume )  {
      printout(INFO,"Detector","Added detector %s to the parent:%s.",det_element.name(),parent.name());
      parent.add(det_element);
      return *this;
    }
  }
  except("DD4hep","Detector: The detector %s has no known parent.", det_element.name());
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
void DetectorImp::endDocument() {
  TGeoManager* mgr = m_manager;
  if (!mgr->IsClosed()) {
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
    ShapePatcher patcher(m_volManager, m_world);
    patcher.patchShapes();
    mapDetectorTypes();
  }
  m_state = READY;
}

/// Initialize the geometry and set the bounding box of the world volume
void DetectorImp::init() {
  if (!m_world.isValid()) {
    TGeoManager* mgr = m_manager;
    Constant     air_const = getRefChild(m_define, "Air", false);
    Constant     vac_const = getRefChild(m_define, "Vacuum", false);
    Box worldSolid("world_x", "world_y", "world_z");
    printout(INFO,"Detector"," *********** created World volume with size : %7.0f %7.0f %7.0f",
             worldSolid->GetDX(), worldSolid->GetDY(), worldSolid->GetDZ());
    m_materialAir    = material(air_const.isValid() ? air_const->GetTitle() : "Air");
    m_materialVacuum = material(vac_const.isValid() ? vac_const->GetTitle() : "Vacuum");

    Volume world_vol("world_volume", worldSolid, m_materialAir);
    m_world = DetElement(new WorldObject(*this,"world"));
    m_worldVol = world_vol;
    // Set the world volume to invisible.
    VisAttr worldVis("WorldVis");
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

#if 0
    Tube trackingSolid(0.,"tracking_region_radius","2*tracking_region_zmax",2*M_PI);
    Volume tracking("tracking_volume",trackingSolid, m_materialAir);
    m_trackers = TopDetElement("tracking");
    m_trackingVol = tracking;
    m_trackers.setPlacement(m_worldVol.placeVolume(tracking));
    m_world.add(m_trackers);
#endif

    m_detectors.append(m_world);
    m_manager->SetTopVolume(m_worldVol.ptr());
    m_world.setPlacement(mgr->GetTopNode());

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
