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

#if DD4HEP_USE_PYROOT
  #include "TPython.h"
#elif DD4HEP_USE_XERCESC
  #include "compact/Conversions.h"
  #include "XML/DocumentHandler.h"
#endif

namespace DD4hep  { namespace Geometry { struct Compact; }}
using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;
namespace {
  struct TopDetElement : public DetElement {
    TopDetElement(const std::string& nam, Volume vol) : DetElement(nam,0) { _data().volume = vol;    }
  };
}

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();
  return *s_lcdd; 
}

LCDDImp::LCDDImp() : m_world(), m_trackers(), m_worldVol(), m_trackingVol()  {
}

Volume LCDDImp::pickMotherVolume(const DetElement&) const  {     // throw if not existing
  return m_worldVol;
}

LCDD& LCDDImp::addDetector(const Ref_t& x)    { 
  m_detectors.append(x);
  m_world.add(DetElement(x));
  return *this;
}

void LCDDImp::convertMaterials(const string& fname)  {
  //convertMaterials(XML::DocumentHandler().load(fname).root());
}

void LCDDImp::addStdMaterials()   {
  convertMaterials("file:../cmt/elements.xml");
  convertMaterials("file:../cmt/materials.xml");
}


Handle<TObject> LCDDImp::getRefChild(const HandleMap& e, const std::string& name, bool do_throw)  const  {
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
    }
  };
}

void LCDDImp::endDocument()  {
  LCDD& lcdd = *this;
  Material  air = material("Air");

  m_worldVol.setMaterial(air);
  m_trackingVol.setMaterial(air);

  Region trackingRegion(lcdd,"TrackingRegion");
  trackingRegion.setThreshold(1);
  trackingRegion.setStoreSecondaries(true);
  add(trackingRegion);
  m_trackingVol.setRegion(trackingRegion);
    
  // Set the world volume to invisible.
  VisAttr worldVis(lcdd,"WorldVis");
  worldVis.setVisible(false);
  m_worldVol.setVisAttributes(worldVis);
  add(worldVis);
  
  // Set the tracking volume to invisible.
  VisAttr trackingVis(lcdd,"TrackingVis");
  trackingVis.setVisible(false);               
  m_trackingVol.setVisAttributes(trackingVis);
  add(trackingVis); 

  /// Since we allow now for anonymous shapes,
  /// we will rename them to use the name of the volume they are assigned to
  TGeoManager* mgr = gGeoManager;
  gGeoManager->SetTopVolume(m_worldVol);
  mgr->CloseGeometry();
  m_world.setPlacement(PlacedVolume(mgr->GetTopNode()));
  ShapePatcher(m_world)();
}

void LCDDImp::create()  {
  gGeoManager = new TGeoManager();
}

void LCDDImp::init()  {
  LCDD& lcdd = *this;
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
}

void LCDDImp::fromCompact(const std::string& xmlfile) {
#if DD4HEP_USE_PYROOT
  string cmd;
  TPython::Exec("import lcdd");
  cmd = "lcdd.fromCompact('" + xmlfile + "')";
  TPython::Exec(cmd.c_str());  
#elif DD4HEP_USE_XERCESC
  XML::Handle_t compact = XML::DocumentHandler().load(xmlfile).root();
  try {
    Converter<Compact>(*this)(compact);
  }
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
  }
  catch(xercesc::DOMException& e)  {
    cout << "XML-DOM Exception:" << XML::_toString(e.msg) << endl;
  } 
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
  }
#endif
}

void LCDDImp::applyAlignment()   {
}

#include "SimpleGDMLWriter.h"
//#include "Geant4Converter.h"
#include "GeometryTreeDump.h"

void LCDDImp::dump() const  {
  TGeoManager* mgr = gGeoManager;
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");

  // SimpleGDMLWriter handler(cout);
  // Geant4Converter handler;
  //GeometryTreeDump handler;
  //handler.create(m_world);
}
