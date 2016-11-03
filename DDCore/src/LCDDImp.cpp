// $Id$
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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/LCDDHelper.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/VolumeManagerInterna.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "LCDDImp.h"

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

#if DD4HEP_USE_PYROOT
#include "TPython.h"
#endif
#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace DD4hep {
  namespace XML {
    typedef xercesc::DOMException XmlException;
  }
}
#endif

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;
namespace {
  struct TypePreserve {
    LCDDBuildType& m_t;
    TypePreserve(LCDDBuildType& t)
      : m_t(t) {
    }
    ~TypePreserve() {
      m_t = BUILD_NONE;
    }
  };
  static LCDD* s_lcdd = 0;

  void lcdd_unexpected()    {
    try  {
      throw;
    }  catch( std::exception& e){
      std::cout << "\n"
                << "**************************************************** \n"
                << "*  A runtime error has occured :                     \n"
                << "*    " << e.what()   << std::endl
                << "*  the program will have to be terminated - sorry.   \n"
                << "**************************************************** \n"
                << std::endl ;

      std::set_unexpected( std::unexpected ) ;
      std::set_terminate( std::terminate ) ;
      // this provokes ROOT seg fault and stack trace (comment out to avoid it)
      exit(1) ;
    }
  }
}

LCDD& LCDD::getInstance() {
  if (!s_lcdd)
    s_lcdd = new LCDDImp();
  return *s_lcdd;
}

/// Destroy the instance
void LCDD::destroyInstance() {
  if (s_lcdd)
    delete s_lcdd;
  s_lcdd = 0;
}

/// Default constructor
LCDDImp::LCDDImp() : LCDDData(), LCDDLoad(this), m_buildType(BUILD_NONE)
{

  std::set_unexpected( lcdd_unexpected ) ;
  std::set_terminate( lcdd_unexpected ) ;

  InstanceCount::increment(this);
  if (0 == gGeoManager) {
    gGeoManager = new TGeoManager("world", "LCDD Geometry");
  }
  {
    m_manager = gGeoManager;
    //m_manager->AddNavigator();
    //m_manager->SetCurrentNavigator(0);
    //cout << "Navigator:" << (void*)m_manager->GetCurrentNavigator() << endl;
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
LCDDImp::~LCDDImp() {
  if ( m_manager == gGeoManager ) gGeoManager = 0;
  destroyData(false);
  m_extensions.clear();
  InstanceCount::decrement(this);
}

// Load volume manager
void LCDDImp::imp_loadVolumeManager()   {
  destroyHandle(m_volManager);
  m_volManager = VolumeManager(*this, "World", world(), Readout(), VolumeManager::TREE);
}

/// Add an extension object to the LCDD instance
void* LCDDImp::addUserExtension(void* ptr, const std::type_info& info, void (*destruct)(void*)) {
  return m_extensions.addExtension(ptr,info,destruct);
}

/// Remove an existing extension object from the LCDD instance
void* LCDDImp::removeUserExtension(const std::type_info& info, bool destroy)  {
  return m_extensions.removeExtension(info,destroy);
}

/// Access an existing extension object from the LCDD instance
void* LCDDImp::userExtension(const std::type_info& info, bool alert) const {
  return m_extensions.extension(info,alert);
}

/// Register new mother volume using the detector name.
void LCDDImp::declareMotherVolume(const std::string& detector_name, const Volume& vol)  {
  if ( !detector_name.empty() )  {
    if ( vol.isValid() )  {
      HandleMap::const_iterator i = m_motherVolumes.find(detector_name);
      if (i == m_motherVolumes.end())   {
        m_motherVolumes.insert(make_pair(detector_name,vol));
        return;
      }
      throw runtime_error("LCDD: A mother volume to the detector "+detector_name+" was already registered.");
    }
    throw runtime_error("LCDD: Attempt to register invalid mother volume for detector:"+detector_name+" [Invalid-Handle].");
  }
  throw runtime_error("LCDD: Attempt to register mother volume to invalid detector [Invalid-detector-name].");
}

/// Access mother volume by detector element
Volume LCDDImp::pickMotherVolume(const DetElement& de) const {
  if ( de.isValid() )   {
    string de_name = de.name();
    HandleMap::const_iterator i = m_motherVolumes.find(de_name);
    if (i == m_motherVolumes.end())   {
      return m_worldVol;
    }
    return (*i).second;
  }
  throw runtime_error("LCDD: Attempt access mother volume of invalid detector [Invalid-handle]");
}

LCDD& LCDDImp::addDetector(const Ref_t& ref_det) {
  DetElement det_element(ref_det);
  LCDDHelper helper(this);
  DetElement existing_det = helper.detectorByID(det_element.id());

  if ( existing_det.isValid() )   {
    SensitiveDetector sd = helper.sensitiveDetector(existing_det);
    if ( sd.isValid() )   {
      stringstream str;
      str << "LCDD: The sensitive sub-detectors " << det_element.name() << " and "
          << existing_det.name() << " have the identical ID:" << det_element.id() << ".";
      printout(ERROR,"LCDD",str.str());
      throw runtime_error(str.str());
    }
  }
  m_detectors.append(ref_det);
  det_element->flag |= DetElement::Object::IS_TOP_LEVEL_DETECTOR;
  Volume volume = det_element.placement()->GetMotherVolume();
  if ( volume == m_worldVol )  {
    printout(DEBUG,"LCDD","Added detector %s to the world instance.",det_element.name());
    m_world.add(det_element);
    return *this;
  }
  // The detector's placement must be one of the existing detectors
  for(HandleMap::iterator i = m_detectors.begin(); i!=m_detectors.end(); ++i)  {
    DetElement parent((*i).second);
    Volume vol = parent.placement().volume();
    if ( vol == volume )  {
      printout(INFO,"LCDD","Added detector %s to the parent:%s.",det_element.name(),parent.name());
      parent.add(det_element);
      return *this;
    }
  }
  throw runtime_error("LCDD: The detector " + string(det_element.name()) + " has no known parent.");
}

/// Add a new constant by named reference to the detector description
LCDD& LCDDImp::addConstant(const Ref_t& x) {
  if ( strcmp(x.name(),"LCDD_InhibitConstants") == 0 )   {
    const char* title = x->GetTitle();
    char c = ::toupper(title[0]);
    m_inhibitConstants = (c=='Y' || c=='T' || c=='1');
  }
  m_define.append(x, false);
  return *this;
}

/// Retrieve a constant by it's name from the detector description
Constant LCDDImp::constant(const std::string& name) const {
  if ( !m_inhibitConstants )   {
    return getRefChild(m_define, name);
  }
  throw runtime_error("LCDD:constant("+name+"): Access to global constants is inhibited.");
}

/// Typed access to constants: access string values
string LCDDImp::constantAsString(const string& name) const {
  if ( !m_inhibitConstants )   {
    Ref_t c = constant(name);
    if (c.isValid())
      return c->GetTitle();
    throw runtime_error("LCDD:constantAsString: The constant " + name + " is not known to the system.");
  }
  throw runtime_error("LCDD:constantAsString("+name+"):: Access to global constants is inhibited.");
}

/// Typed access to constants: long values
long LCDDImp::constantAsLong(const string& name) const {
  if ( !m_inhibitConstants )   {
    return _toLong(constantAsString(name));
  }
  throw runtime_error("LCDD:constantAsLong("+name+"): Access to global constants is inhibited.");
}

/// Typed access to constants: double values
double LCDDImp::constantAsDouble(const string& name) const {
  if ( !m_inhibitConstants )   {
    return _toDouble(constantAsString(name));
  }
  throw runtime_error("LCDD:constantAsDouble("+name+"): Access to global constants is inhibited.");
}

/// Add a field component by named reference to the detector description
LCDD& LCDDImp::addField(const Ref_t& x) {
  m_field.add(x);
  m_fields.append(x);
  return *this;
}

/// Retrieve a matrial by it's name from the detector description
Material LCDDImp::material(const string& name) const {
  TGeoMedium* mat = m_manager->GetMedium(name.c_str());
  if (mat) {
    return Material(Ref_t(mat));
  }
  throw runtime_error("Cannot find a material referenced by name:" + name);
}

/// Internal helper to map detector types once the geometry is closed
void LCDDImp::mapDetectorTypes()  {
  for(HandleMap::const_iterator i=m_detectors.begin(); i!=m_detectors.end(); ++i)   {
    DetElement det((*i).second);
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
vector<string> LCDDImp::detectorTypes() const  {
  if ( m_manager->IsClosed() ) {
    vector<string> v;
    for(DetectorTypeMap::const_iterator i=m_detectorTypes.begin(); i!=m_detectorTypes.end(); ++i)  
      v.push_back((*i).first);
    return v;
  }
  throw runtime_error("detectorTypes: Call only available once the geometry is closed!");
}

/// Access a set of subdetectors according to the sensitive type.
const vector<DetElement>& LCDDImp::detectors(const string& type, bool throw_exc)  {
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

vector<DetElement> LCDDImp::detectors(unsigned int includeFlag, unsigned int excludeFlag ) const  {
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
vector<DetElement> LCDDImp::detectors(const string& type1,
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

Handle<TObject> LCDDImp::getRefChild(const HandleMap& e, const string& name, bool do_throw) const {
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
  struct ShapePatcher: public GeoScan {
    VolumeManager m_volManager;
    DetElement m_world;
    ShapePatcher(VolumeManager m, DetElement e)
      : GeoScan(e), m_volManager(m), m_world(e) {
    }
    void patchShapes() {
      GeoHandler::Data& data = *m_data;
      char text[32];
      string nam;
      printout(INFO,"LCDD","+++ Patching names of anonymous shapes....");
      for (GeoHandler::Data::const_reverse_iterator i = data.rbegin(); i != data.rend(); ++i) {
        const GeoHandler::Data::mapped_type& v = (*i).second;
        for (GeoHandler::Data::mapped_type::const_iterator j = v.begin(); j != v.end(); ++j) {
          const TGeoNode* n = *j;
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

void LCDDImp::endDocument() {
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
}

void LCDDImp::init() {
  if (!m_world.isValid()) {
    TGeoManager* mgr = m_manager;
    Box worldSolid("world_x", "world_y", "world_z");
    printout(INFO,"LCDD"," *********** created World volume with size : %7.0f %7.0f %7.0f",
             worldSolid->GetDX(), worldSolid->GetDY(), worldSolid->GetDZ());

    m_materialAir = material("Air");
    m_materialVacuum = material("Vacuum");

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
    PlacedVolume pv = m_worldVol.placeVolume(tracking);
    m_trackers.setPlacement(pv);
    m_world.add(m_trackers);
#endif
    m_detectors.append(m_world);
    m_manager->SetTopVolume(m_worldVol.ptr());
    m_world.setPlacement(PlacedVolume(mgr->GetTopNode()));
  }
}

/// Read any geometry description or alignment file
void LCDDImp::fromXML(const string& xmlfile, LCDDBuildType build_type) {
  TypePreserve build_type_preserve(m_buildType = build_type);
#if DD4HEP_USE_PYROOT
  string cmd;
  TPython::Exec("import lcdd");
  cmd = "lcdd.fromXML('" + xmlfile + "')";
  TPython::Exec(cmd.c_str());
#else
  processXML(xmlfile,0);
#endif
}

/// Read any geometry description or alignment file with external XML entity resolution
void LCDDImp::fromXML(const std::string& fname, UriReader* entity_resolver, LCDDBuildType build_type)  {
  TypePreserve build_type_preserve(m_buildType = build_type);
  processXML(fname,entity_resolver);
}

void LCDDImp::dump() const {
  TGeoManager* mgr = m_manager;
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");
}

/// Manipulate geometry using facroy converter
long LCDDImp::apply(const char* factory_type, int argc, char** argv) {
  string fac = factory_type;
  try {
    long result = PluginService::Create<long>(fac, (LCDD*) this, argc, argv);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(fac, (LCDD*) this, argc, argv);
      if ( 0 == result )  {
        throw runtime_error("DD4hep: apply-plugin: Failed to locate plugin " +
                            fac + ". " + dbg.missingFactory(fac));
      }
    }
    result = *(long*) result;
    if (result != 1) {
      throw runtime_error("DD4hep: apply-plugin: Failed to execute plugin " + fac);
    }
    return result;
  }
  catch (const XML::XmlException& e) {
    throw runtime_error(XML::_toString(e.msg) + "\nDD4hep: XML-DOM Exception with plugin:" + fac);
  }
  catch (const exception& e) {
    throw runtime_error(string(e.what()) + "\nDD4hep: with plugin:" + fac);
  }
  catch (...) {
    throw runtime_error("UNKNOWN exception from plugin:" + fac);
  }
  return EINVAL;
}
