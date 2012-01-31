#define _USE_MATH_DEFINES
#include "LCDDImp.h"
//#include "../compact/Conversions.h"
#include "Internals.h"
//#include "XML/DocumentHandler.h"


// C+_+ include files
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

//#include "XML/Evaluator.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TPython.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();

  return *s_lcdd; 
}

LCDDImp::LCDDImp() : m_worldVol(0), m_trackingVol(0), m_reflect(0), m_identity(0)  {
  //evaluator();
  //XML::tags_init();
}

Volume LCDDImp::pickMotherVolume(const Subdetector&) const  {     // throw if not existing
  return m_worldVol;
}

Handle_t LCDDImp::getRefChild(const HandleMap& e, const std::string& name, bool do_throw)  const  {
  HandleMap::const_iterator i = e.find(name);
  if ( i != e.end() )  {
    return (*i).second;
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+name);
  }
  return 0;
}

template<class T> LCDD& 
LCDDImp::__add(const RefElement& x, ObjectHandleMap& m,Int_t (TGeoManager::*func)(T*))  {
  T* obj = dynamic_cast<T*>(x.ptr());
  if ( obj )  {
    TGeoManager *mgr = document();
    if ( mgr )  {
      m.append(x);
      if ( func ) (mgr->*func)(obj);
      return *this;
    }
    throw InvalidObjectError("Attempt to add object to invalid TGeoManager instance.");
  }
  throw InvalidObjectError("Attempt to add an object, which is of the wrong type.");
}

LCDD& LCDDImp::addVolume(const RefElement& x)    {
  //return __add(x,m_structure,&TGeoManager::AddVolume);
  m_structure.append(x);
  return *this;
}
#include "TGeoTube.h"
#include "TGeoPcon.h"
#include "TGeoCompositeShape.h"
LCDD& LCDDImp::addSolid(const RefElement& x)     {
  TGeoShape *o, *obj = dynamic_cast<TGeoShape*>(x.ptr());
  if ( (o=dynamic_cast<TGeoPcon*>(obj)) )  {
    m_structure.append<TGeoShape>(x);
  }
  else if ( (o=dynamic_cast<TGeoTube*>(obj)) )  {
    m_structure.append<TGeoShape>(x);
  }
  else if ( (o=dynamic_cast<TGeoCompositeShape*>(obj)) )  {
    m_structure.append<TGeoShape>(x);
  }
  else  {
    m_structure.append<TGeoShape>(x);
    //__add(x,m_structure,&TGeoManager::AddShape);
  }
#if 0
  cout << obj->GetName() << " " << (void*)obj
    << " Index:" << document()->GetListOfShapes()->IndexOf(obj) << endl;
#endif
  return *this;
}

LCDD& LCDDImp::addRotation(const RefElement& x)  {
  TGeoMatrix* obj = dynamic_cast<TGeoMatrix*>(x.ptr());
  if ( 0 == obj ) {
    cout << "+++ Attempt to store invalid matrix object:" << typeid(*x.ptr()).name() << endl;
  }
  if ( obj->IsRegistered() ) {
    cout << "+++ Attempt to register already registered matrix:" << obj->GetName() << endl;
  }
  // cout << "+++ Register Rotation[" << m_doc->GetListOfMatrices()->GetEntries() << "]:" << obj->GetName() << endl;

  return __add(x,m_rotations,&TGeoManager::AddTransformation);
  //m_rotations.append<TGeoMatrix>(x);
  //return *this;
}

LCDD& LCDDImp::addPosition(const RefElement& x)  {
  TGeoMatrix* obj = dynamic_cast<TGeoMatrix*>(x.ptr());
  if ( 0 == obj ) {
    cout << "+++ Attempt to store invalid matrix object:" << typeid(*x.ptr()).name() << endl;
  }
  if ( obj->IsRegistered() ) {
    cout << "+++ Attempt to register already registered matrix:" << obj->GetName() << endl;
  }
  /// cout << "+++ Register Position [" << m_doc->GetListOfMatrices()->GetEntries() << "]:" << obj->GetName() << endl;
  return __add(x,m_positions,&TGeoManager::AddTransformation);
}

void LCDDImp::endDocument()  {
  Document doc  = document(); 
  Volume world  = volume("world_volume");
  Volume trkVol = volume("tracking_volume");
  Material  air = material("Air");

  world.setMaterial(air);
  trkVol.setMaterial(air);

  Region trackingRegion(doc,"TrackingRegion");
  trackingRegion.setThreshold(1);
  trackingRegion.setStoreSecondaries(true);
  add(trackingRegion);
  trkVol.setRegion(trackingRegion);
    
  // Set the world volume to invisible.
  VisAttr worldVis(document(),"WorldVis");
  worldVis.setVisible(false);
  world.setVisAttributes(worldVis);
  add(worldVis);
  
  // Set the tracking volume to invisible.
  VisAttr trackingVis(document(),"TrackingVis");
  trackingVis.setVisible(false);               
  trkVol.setVisAttributes(trackingVis);
  add(trackingVis); 
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

Document LCDDImp::create()  {
  Document doc(new TGeoManager());
  m_doc  = doc;
  return doc;
}

Document LCDDImp::init()  {
  Document doc = m_doc;
  m_identity = Matrix(doc,"identity","identity");
  Box worldSolid(doc,"world_box","world_x","world_y","world_z");
  add(worldSolid);

  add(Rotation(doc,"identity_rot"));
  add(m_reflect = Rotation(doc,"reflect_rot",M_PI,0.,0.));
  add(Position(doc,"identity_pos"));

  Material air = material("Air");
  Volume world(doc,"world_volume",worldSolid,air);
  add(world);

  //Tube trackingSolid(doc,"tracking_cylinder",
	//	     0.,
	//	     _toDouble("tracking_region_radius"),
	//	     _toDouble("2*tracking_region_zmax"),M_PI);
  //add(trackingSolid);

  //Volume tracking(doc,"tracking_volume",trackingSolid,air);
  //add(tracking);
//  world.addPhysVol(PhysVol(doc,tracking,"tracking_volume"));

  //RefElement ref_world(doc,"world",world.refName());
  //m_setup.append(ref_world);
  m_worldVol    = world;
  //m_trackingVol = tracking;
  doc->SetTopVolume(value<TGeoVolume>(m_worldVol));
  return doc;
}


void LCDDImp::fromCompact(const std::string& xmlfile) {
  string cmd;
  TPython::Exec("import lcdd");

  cmd = "lcdd.fromCompact('" + xmlfile + "')";
  TPython::Exec(cmd.c_str());  
}


void LCDDImp::dump() const  {
  m_doc->CloseGeometry();
      
  m_doc->SetVisLevel(4);
  m_doc->SetVisOption(1);
  value<TGeoVolume>(m_worldVol)->Draw("ogl");

  //Printer<const LCDD*>(*this,cout)(this);
}

