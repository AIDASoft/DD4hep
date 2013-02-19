// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/GeoHandler.h"
#include "LCDDImp.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TClass.h"
#include "Reflex/PluginService.h"

#if DD4HEP_USE_PYROOT
  #include "TPython.h"
#elif DD4HEP_USE_XERCESC
  #include "XML/DocumentHandler.h"
#endif

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;
namespace {
  struct TopDetElement : public DetElement {
    TopDetElement(const string& nam, Volume vol) : DetElement(nam,/* "structure", */0) { _data().volume = vol;    }
  };
  struct TypePreserve {
    LCDDBuildType& m_t;
    TypePreserve(LCDDBuildType& t) : m_t(t) { }
    ~TypePreserve() { m_t = BUILD_NONE; }
  };
}

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();
  return *s_lcdd; 
}

/// Default constructor
LCDDImp::LCDDImp() 
  : m_world(), m_trackers(), m_worldVol(), m_trackingVol(), m_field("global"),
    m_buildType(BUILD_NONE)
{
  m_properties = new Properties();
  //if ( 0 == gGeoManager )
  {
    gGeoManager = new TGeoManager();
    gGeoManager->AddNavigator();
    gGeoManager->SetCurrentNavigator(0);
    cout << "Navigator:" << (void*)gGeoManager->GetCurrentNavigator() << endl;
  }
  //if ( 0 == gGeoIdentity ) 
  {
    gGeoIdentity = new TGeoIdentity();
  }
  VisAttr attr("invisible");
  attr.setColor(0.5,0.5,0.5);
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
  if ( m_properties ) delete m_properties;
  m_properties = 0;
}

Volume LCDDImp::pickMotherVolume(const DetElement&) const  {     // throw if not existing
  return m_worldVol;
}

LCDD& LCDDImp::addDetector(const Ref_t& x)    { 
  m_detectors.append(x);
  m_world.add(DetElement(x));
  return *this;
}

/// Typed access to constants: access string values
string LCDDImp::constantAsString(const string& name) const {
  Ref_t c = constant(name);
  if ( c.isValid() ) return c->GetTitle();
  throw runtime_error("LCDD: The constant "+name+" is not known to the system.");
}

/// Typed access to constants: long values
long LCDDImp::constantAsLong(const string& name) const {
  return _toLong(constantAsString(name));
}

/// Typed access to constants: double values
double LCDDImp::constantAsDouble(const string& name) const {
  return _toDouble(constantAsString(name));
}

/// Add a field component by named reference to the detector description
LCDD& LCDDImp::addField(const Ref_t& x) {
  m_field.add(x);
  m_fields.append(x);
  return *this;
}

Handle<TObject> LCDDImp::getRefChild(const HandleMap& e, const string& name, bool do_throw)  const  {
  HandleMap::const_iterator i = e.find(name);
  if ( i != e.end() )  {
    return (*i).second;
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+name);
  }
  return 0;
}
namespace {
  struct ShapePatcher : public GeoScan {
    ShapePatcher(DetElement e) : GeoScan(e) {}
    GeoScan& operator()()  {
      GeoHandler::Data& data = *m_data;
      string nam;
      for(GeoHandler::Data::const_reverse_iterator i=data.rbegin(); i != data.rend(); ++i)   {
        const GeoHandler::Data::mapped_type& v = (*i).second;
        for(GeoHandler::Data::mapped_type::const_iterator j=v.begin(); j != v.end(); ++j) {
          const TGeoNode* n = *j;
          TGeoVolume* v = n->GetVolume();
          TGeoShape*  s = v->GetShape();
          if ( 0 == ::strcmp(s->GetName(),s->IsA()->GetName()) ) {
            nam = v->GetName();
            nam += "_shape";
            s->SetName(nam.c_str());
          }
          else {
            nam = s->GetName();
          }
          if ( s->IsA()->Class() == TGeoCompositeShape::Class() ) {
            TGeoCompositeShape* c = (TGeoCompositeShape*)s;
            const TGeoBoolNode* boolean = c->GetBoolNode();
            s = boolean->GetLeftShape();
            if ( 0 == ::strcmp(s->GetName(),s->IsA()->GetName()) ) {
              nam = v->GetName();
              nam += "_left";
              s->SetName(nam.c_str());
            }
            s = boolean->GetRightShape();
            if ( 0 == ::strcmp(s->GetName(),s->IsA()->GetName()) ) {
              nam = v->GetName();
              nam += "_right";
              s->SetName(nam.c_str());
            }
          }
        }
      }
      return *this;
    }
  };
}

void LCDDImp::endDocument()  {
  TGeoManager* mgr = gGeoManager;
  if ( !mgr->IsClosed() ) {
    LCDD& lcdd = *this;
    Material  air = material("Air");

    m_worldVol.setMaterial(air);
    m_trackingVol.setMaterial(air);

    Region trackingRegion("TrackingRegion");
    trackingRegion.setThreshold(1);
    trackingRegion.setStoreSecondaries(true);
    add(trackingRegion);
    m_trackingVol.setRegion(trackingRegion);
    
    // Set the world volume to invisible.
    VisAttr worldVis("WorldVis");
    worldVis.setVisible(false);
    m_worldVol.setVisAttributes(worldVis);
    add(worldVis);
  
    // Set the tracking volume to invisible.
    VisAttr trackingVis("TrackingVis");
    trackingVis.setVisible(false);               
    m_trackingVol.setVisAttributes(trackingVis);
    add(trackingVis); 

    /// Since we allow now for anonymous shapes,
    /// we will rename them to use the name of the volume they are assigned to
    //gGeoManager->SetTopVolume(m_worldVol);
    mgr->CloseGeometry();
    m_world.setPlacement(PlacedVolume(mgr->GetTopNode()));
    ShapePatcher(m_world)();
  }
}

void LCDDImp::init()  {
  if ( !m_world.isValid() ) {
    Box worldSolid("world_box","world_x","world_y","world_z");
    Material vacuum = material("Vacuum");
    Volume world("world_volume",worldSolid,vacuum);
    Tube trackingSolid("tracking_cylinder",
		       0.,
		       _toDouble("tracking_region_radius"),
		       _toDouble("2*tracking_region_zmax"),2*M_PI);
    Volume tracking("tracking_volume",trackingSolid, vacuum);
    m_world          = TopDetElement("world",world);
    m_trackers       = TopDetElement("tracking",tracking);
    m_worldVol       = world;
    m_trackingVol    = tracking;
    m_materialAir    = material("Air");
    m_materialVacuum = material("Vacuum");
    m_detectors.append(m_world);
    m_world.add(m_trackers);
    gGeoManager->SetTopVolume(m_worldVol);
  }
}

void LCDDImp::fromXML(const string& xmlfile, LCDDBuildType build_type) {
  TypePreserve build_type_preserve(m_buildType=build_type);
#if DD4HEP_USE_PYROOT
  string cmd;
  TPython::Exec("import lcdd");
  cmd = "lcdd.fromXML('" + xmlfile + "')";
  TPython::Exec(cmd.c_str());  
#elif DD4HEP_USE_XERCESC
  const XML::Handle_t xml_root = XML::DocumentHandler().load(xmlfile).root();
  string tag = xml_root.tag();
  try {
    LCDD* lcdd = this;
    string type = tag + "_XML_reader";
    long result = ROOT::Reflex::PluginService::Create<long>(type,lcdd,&xml_root);
    if ( 0 == result ) {
      throw runtime_error("Failed to locate plugin to interprete files of type"
			  " \""+tag+"\" - no factory:"+type);
    }
    result = *(long*)result;
    if ( result != 1 ) {
      throw runtime_error("Failed to parse the XML file "+xmlfile+" with the plugin "+type);
    }
  }
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
    throw runtime_error("Exception:"+string(e.what())+" while parsing "+xmlfile);
  }
  catch(const XML::XmlException& e)  {
    cout << "XML-DOM Exception:" << XML::_toString(e.msg) << endl;
    throw runtime_error("XML-DOM Exception:"+XML::_toString(e.msg)+" while parsing "+xmlfile);
  } 
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
    throw runtime_error("UNKNOWN excetion while parsing "+xmlfile);
  }
#endif
}

void LCDDImp::dump() const  {
  TGeoManager* mgr = gGeoManager;
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");
}

/// Manipulate geometry using facroy converter
void LCDDImp::apply(const char* factory_type, int argc, char** argv)   {
  string fac = factory_type;
  try {
    LCDD* lcdd = this;
    char* fname = 0;
    long result = ROOT::Reflex::PluginService::Create<long>(fac,lcdd,argc,argv);
    if ( 0 == result ) {
      throw runtime_error("Failed to locate plugin to apply "+fac);
    }
    result = *(long*)result;
    if ( result != 1 ) {
      throw runtime_error("Failed to execute plugin to apply "+fac);
    }
  }
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
    throw runtime_error("Exception:"+string(e.what())+" while applying plugin:"+fac);
  }
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
    throw runtime_error("UNKNOWN excetion while applying plugin:"+fac);
  }
}
