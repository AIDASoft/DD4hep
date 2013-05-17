// $Id$
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
#include "XML/DocumentHandler.h"

#if DD4HEP_USE_PYROOT
  #include "TPython.h"
#endif
#ifndef __TIXML__
#include "xercesc/dom/DOMException.hpp"
namespace DD4hep { namespace XML {  
  typedef xercesc::DOMException  XmlException;
}}
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
  if ( m_volManager.isValid() )  {
    delete m_volManager.ptr();
    m_volManager = VolumeManager();
  }
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
    VolumeManager m_volManager;
    DetElement    m_world;
    ShapePatcher(VolumeManager m, DetElement e) : GeoScan(e), m_volManager(m), m_world(e) {}
    void patchShapes()  {
      GeoHandler::Data& data = *m_data;
      string nam;
      cout << "++ Patching names of anonymous shapes...." << endl;
      for(GeoHandler::Data::const_reverse_iterator i=data.rbegin(); i != data.rend(); ++i)   {
	int level = (*i).first;
        const GeoHandler::Data::mapped_type& v = (*i).second;
        for(GeoHandler::Data::mapped_type::const_iterator j=v.begin(); j != v.end(); ++j) {
          const TGeoNode* n = *j;
          TGeoVolume* v     = n->GetVolume();
          TGeoShape*  s     = v->GetShape();
	  const char* sn    = s->GetName();
          if ( 0 == sn || 0 == ::strlen(sn) ) {
            nam = v->GetName();
            nam += "_shape";
            s->SetName(nam.c_str());
          }
          else if ( 0 == ::strcmp(sn,s->IsA()->GetName()) ) {
            nam = v->GetName();
            nam += "_shape";
            s->SetName(nam.c_str());
          }
	  else {
	    nam = sn;
	    if ( nam.find("_shape") == string::npos ) nam += "_shape";
            s->SetName(nam.c_str());
	  }
          if ( s->IsA() == TGeoCompositeShape::Class() ) {
            TGeoCompositeShape* c = (TGeoCompositeShape*)s;
            const TGeoBoolNode* boolean = c->GetBoolNode();
            s  = boolean->GetLeftShape();
	    sn = s->GetName();
	    if ( 0 == sn || 0 == ::strlen(sn) ) {
              s->SetName((nam+"_left").c_str());
	    }
            else if ( 0 == ::strcmp(sn,s->IsA()->GetName()) ) {
              s->SetName((nam+"_left").c_str());
	    }
            s  = boolean->GetRightShape();
	    sn = s->GetName();
	    if ( 0 == sn || 0 == ::strlen(sn) ) {
              s->SetName((nam+"_right").c_str());
	    }
            else if ( 0 == ::strcmp(s->GetName(),s->IsA()->GetName()) ) {
              s->SetName((nam+"_right").c_str());
            }
          }
        }
      }
    }

    typedef DetElement::Children _C;
    DetElement findElt(DetElement e, PlacedVolume pv)  {
      const _C& children = e.children();
      if ( e.placement().ptr() == pv.ptr() ) return e;
      for(_C::const_iterator i=children.begin(); i!=children.end(); ++i)   {
	DetElement de = (*i).second;
	if ( de.placement().ptr() == pv.ptr() ) return de;
      }
      for(_C::const_iterator i=children.begin(); i!=children.end(); ++i)   {
	DetElement de = findElt((*i).second,pv);
	if ( de.isValid() )  return de;
      }
      return DetElement();
    }

    unsigned long long int encode_cell(Readout ro, const PlacedVolume::VolIDs& ids)   {
      unsigned long long value = ~0x0ull;
      const IDDescriptor& en = ro.idSpec();
      for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
	const PlacedVolume::VolID& id = (*i);
	value &= en.field(id.first).encode(id.second);
      }
      return value;
    }

    typedef void (::ShapePatcher::*Func_t)(DetElement parent, DetElement e,const TGeoNode* n, 
					   const PlacedVolume::VolIDs& ids, 
					   const vector<const TGeoNode*>& nodes,
					   const vector<const TGeoNode*>& elt_nodes);

    void scanNode(Func_t func,
		  DetElement parent, DetElement e, PlacedVolume pv, 
		  const PlacedVolume::VolIDs& vol_ids, 
		  vector<const TGeoNode*>& nodes,
		  vector<const TGeoNode*>& elt_nodes)
    {
      const TGeoNode* node = dynamic_cast<const TGeoNode*>(pv.ptr());
      if ( node )  {
	Int_t ndau = node->GetNdaughters();
	PlacedVolume::VolIDs ids(vol_ids);
	const PlacedVolume::VolIDs& ids_node = pv.volIDs();

	nodes.push_back(node);
	elt_nodes.push_back(node);
	ids.PlacedVolume::VolIDs::Base::insert(ids.end(),ids_node.begin(),ids_node.end());
	(this->*func)(parent, e, node, ids, nodes, elt_nodes);
	for(Int_t idau=0; idau<ndau; ++idau)  {
	  TGeoNode* daughter = node->GetDaughter(idau);
	  if ( dynamic_cast<const PlacedVolume::Object*>(daughter) ) {
	    PlacedVolume pv_dau = Ref_t(daughter);
	    DetElement   de_dau = findElt(e,daughter);
	    if ( de_dau.isValid() )  {
	      vector<const TGeoNode*> dau_nodes;
	      scanNode(func, parent, de_dau, pv_dau, ids, nodes, dau_nodes);
	    }
	    else {
	      scanNode(func, parent, e, pv_dau, ids, nodes, elt_nodes);
	    }
	  }
	}
	elt_nodes.pop_back();
	nodes.pop_back();
      }
    }
    void scanVolumes(Func_t func)    {
      const _C& children = m_world.children();
      for(_C::const_iterator i=children.begin(); i!=children.end(); ++i)   {
	DetElement   de = (*i).second;
	PlacedVolume pv = de.placement();
	if ( pv.isValid() )  {
	  PlacedVolume::VolIDs ids;
	  vector<const TGeoNode*> nodes, elt_nodes;
	  scanNode(func, de, de, pv, ids, nodes, elt_nodes);
	  continue;
	}
	cout << "++ Detector element " << de.name() << " has no placement." << endl;
      }
    }

    void print_node(DetElement parent, DetElement e,const TGeoNode* n, 
		    const PlacedVolume::VolIDs& ids, 
		    const vector<const TGeoNode*>& nodes,
		    const vector<const TGeoNode*>& elt_nodes);

    GeoScan& printVolumes()  {
      scanVolumes(&ShapePatcher::print_node);
      return *this;
    }
  };

  void ShapePatcher::print_node(DetElement parent, 
				DetElement e,
				const TGeoNode* n, 
				const PlacedVolume::VolIDs& ids, 
				const vector<const TGeoNode*>& nodes,
				const vector<const TGeoNode*>& elt_nodes)
  {
    static int s_count = 0;
    Readout ro = parent.readout();
    const IDDescriptor& en = ro.idSpec();
    IDDescriptor::VolumeID volume_id = encode_cell(ro,ids);
    PlacedVolume pv = Ref_t(n);
    bool sensitive  = pv.volume().isSensitive();

    if ( !sensitive ) return;
    ++s_count;
    cout << s_count << ": " << e.name() << " de:" << e.ptr() << " ro:" << ro.ptr() 
	 << " pv:" << n->GetName() << " id:" << (void*)volume_id << " : ";
    for(PlacedVolume::VolIDs::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
      const PlacedVolume::VolID& id = (*i);
      IDDescriptor::Field f = ro.idSpec().field(id.first);
      int value = en.field(id.first).decode(volume_id);
      cout << id.first << "=" << id.second << "," << value 
	   << " [" << f.first << "," << f.second << "] ";
    }
    cout << " Sensitive:" << (sensitive ? "YES" : "NO");
    if ( sensitive )   {
      VolumeManager section = m_volManager.subdetector(volume_id);
      VolumeManager::Context* ctxt = section.lookupContext(volume_id|0xDEAD);
      if ( ctxt->placement.ptr() != pv.ptr() )  {
	cout << " !!!!! No Volume found!" << endl;
      }
      cout << " Pv:" << pv.ptr() << " <> " << ctxt->placement.ptr();
    }
    cout << endl;
#if 0
    cout << s_count << ": " << e.name() << " Detector GeoNodes:";
    for(vector<const TGeoNode*>::const_iterator j=nodes.begin(); j!=nodes.end();++j)
      cout << (void*)(*j) << " ";
    cout << endl;
    cout << s_count << ": " << e.name() << " Element  GeoNodes:";
    for(vector<const TGeoNode*>::const_iterator j=elt_nodes.begin(); j!=elt_nodes.end();++j)
      cout << (void*)(*j) << " ";
    cout << endl;
#endif
  }
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
    m_volManager = VolumeManager("World", m_world);
    ShapePatcher patcher(m_volManager,m_world);
    patcher.patchShapes();
    //patcher.printVolumes();
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
#else
  XML::Handle_t xml_root = XML::DocumentHandler().load(xmlfile).root();
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
  catch(const XML::XmlException& e)  {
    cout << "XML-DOM Exception:" << XML::_toString(e.msg) << endl;
    throw runtime_error("XML-DOM Exception:\""+XML::_toString(e.msg)+"\" while parsing "+xmlfile);
  } 
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
    throw runtime_error("Exception:\""+string(e.what())+"\" while parsing "+xmlfile);
  }
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
    throw runtime_error("UNKNOWN exception while parsing "+xmlfile);
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
    long result = ROOT::Reflex::PluginService::Create<long>(fac,(LCDD*)this,argc,argv);
    if ( 0 == result ) {
      throw runtime_error("apply-plugin: Failed to locate plugin "+fac);
    }
    result = *(long*)result;
    if ( result != 1 ) {
      throw runtime_error("apply-plugin: Failed to execute plugin "+fac);
    }
  }
  catch(const XML::XmlException& e)  {
    cout << "XML-DOM Exception:" << XML::_toString(e.msg) << endl;
    throw runtime_error("XML-DOM Exception:\""+XML::_toString(e.msg)+"\" with plugin:"+fac);
  } 
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
    throw runtime_error("Exception:\""+string(e.what())+"\" with plugin:"+fac);
  }
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
    throw runtime_error("UNKNOWN exception from plugin:"+fac);
  }
}
