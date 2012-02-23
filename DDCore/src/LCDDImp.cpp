// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "LCDDImp.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

#include "TGeoManager.h"
#include "TGeoMatrix.h"

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

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();
  return *s_lcdd; 
}

LCDDImp::LCDDImp() : m_worldVol(), m_trackingVol()  {
}

Volume LCDDImp::pickMotherVolume(const DetElement&) const  {     // throw if not existing
  return m_worldVol;
}

LCDD& LCDDImp::addVolume(const Ref_t& x)    {
  m_structure.append<TGeoVolume>(x);
  return *this;
}

LCDD& LCDDImp::addSolid(const Ref_t& x)    {
  m_structure.append<TGeoShape>(x);
  return *this;
}

void LCDDImp::convertMaterials(const string& fname)  {
  //convertMaterials(XML::DocumentHandler().load(fname).root());
}

//void LCDDImp::convertMaterials(XML::Handle_t materials)  {
//  Converter<Materials>(*this)(materials);
//}

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

void LCDDImp::endDocument()  {
  LCDD& lcdd = *this;
  Volume world  = volume("world_volume");
  Volume trkVol = volume("tracking_volume");
  Material  air = material("Air");

  world.setMaterial(air);
  trkVol.setMaterial(air);

  Region trackingRegion(lcdd,"TrackingRegion");
  trackingRegion.setThreshold(1);
  trackingRegion.setStoreSecondaries(true);
  add(trackingRegion);
  trkVol.setRegion(trackingRegion);
    
  // Set the world volume to invisible.
  VisAttr worldVis(lcdd,"WorldVis");
  worldVis.setVisible(false);
  world.setVisAttributes(worldVis);
  add(worldVis);
  
  // Set the tracking volume to invisible.
  VisAttr trackingVis(lcdd,"TrackingVis");
  trackingVis.setVisible(false);               
  trkVol.setVisAttributes(trackingVis);
  add(trackingVis); 
}

void LCDDImp::create()  {
  gGeoManager = new TGeoManager();
}

void LCDDImp::init()  {
  LCDD& lcdd = *this;
  Box worldSolid(lcdd,"world_box","world_x","world_y","world_z");
  Material vacuum = material("Vacuum");
  Volume world(lcdd,"world_volume",worldSolid,vacuum);

  Tube trackingSolid(lcdd,"tracking_cylinder",
		     0.,
		     _toDouble("tracking_region_radius"),
		     _toDouble("2*tracking_region_zmax"),2*M_PI);
  Volume tracking(lcdd,"tracking_volume",trackingSolid, vacuum);
  //-->world.placeVolume(tracking);

  //Ref_t ref_world(lcdd,"world",world.refName());
  //m_setup.append(ref_world);
  m_worldVol       = world;
  m_trackingVol    = tracking;
  m_materialAir    = material("Air");
  m_materialVacuum = material("Vacuum");
  gGeoManager->SetTopVolume(m_worldVol);
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

void LCDDImp::dump() const  {
  TGeoManager* mgr = gGeoManager;
  mgr->CloseGeometry();
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");
  cout << "Total number of object verifications:" << num_object_validations() << endl;
  //Printer<const LCDD*>(*this,cout)(this);
}

