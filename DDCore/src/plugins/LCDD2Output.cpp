// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "XML/Conversions.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Objects.h"
#include "DD4hep/IDDescriptor.h"

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace DD4hep::Geometry;

namespace DD4hep {

  template <> void Printer<Constant>::operator()(const Constant& val)  const  {
    os << "++ Constant:" << val.toString() << endl;
  }

  template <> void Printer<Material>::operator()(const Material& mat)  const  {
    os << "++ Medium:  " << mat.toString() << "|  " << endl;
    mat->Print();
  }

  template <> void Printer<VisAttr>::operator()(const VisAttr& val)  const  {
    os << "++ VisAttr: " << val.toString() << endl;
  }

  template <> void Printer<Readout>::operator()(const Readout& val)  const  {
    os << "++ Readout: ";
    val->Print();
  }

  template <> void Printer<Region>::operator()(const Region& val)  const  {
    os << "++ Region:  ";
    val->Print();
  }

  template <> void Printer<Rotation>::operator()(const Rotation& val)  const  {
    os << "++ Rotation: ";
    //val->Print();
  }

  template <> void Printer<Position>::operator()(const Position& val)  const  {
    os << "++ Position: ";
    //val->Print();
  }

  template <> void Printer<LimitSet>::operator()(const LimitSet& val)  const  {
    const LimitSet::Object& o = val.limits();
    os << "++ LimitSet: " << val.name() << endl;
    val->TNamed::Print();
    for(LimitSet::Object::const_iterator i=o.begin(); i!=o.end(); ++i) {
      os << "++    Limit:" << (*i).name << " " << (*i).particles 
	 << " [" << (*i).unit << "] " << (*i).content << " " << (*i).value << endl;
    }
  }

  template <> void Printer<DetElement>::operator()(const DetElement& val)  const  {
    DetElement::Object* obj = val.data<DetElement::Object>();
    if ( obj )  {
      char text[256];
      const DetElement& sd = val;
      PlacedVolume plc = sd.placement();
      bool rdo = sd.readout().isValid();
      bool vis = plc.isValid();
      bool env = plc.isValid();
      bool mat = plc.isValid();
      ::snprintf(text,sizeof(text),"ID:%-3d Combine Hits:%3s Readout:%s Material:%s Envelope:%s VisAttr:%s",
		sd.id(), yes_no(sd.combineHits()), 
		rdo ? sd.readout()->GetName()  : yes_no(rdo),
		mat ? plc.material()->GetName() : yes_no(mat),
		env ? plc.motherVol()->GetName() : yes_no(env),
		yes_no(vis)
		);
      os << prefix << "+= DetElement: " << val->GetName() << " " << val.type() << endl;
      os << prefix << "|               " << text << endl;

      if ( vis )   {
	VisAttr attr = plc.volume().visAttributes();
	VisAttr::Object* v = attr.data<VisAttr::Object>();
	TColor* col = gROOT->GetColor(v->color);
	char text[256];
	::snprintf(text,sizeof(text)," RGB:%-8s [%d] %7.2f  Style:%d %d ShowDaughters:%3s Visible:%3s",
		  col->AsHexString(), v->color, col->GetAlpha(), int(v->drawingStyle), int(v->lineStyle),
		  v->showDaughters ? "YES" : "NO", v->visible ? "YES" : "NO");
	os << prefix << "|               VisAttr:  " << setw(32) << left << attr.name() << text << endl;
      }
      if ( plc.isValid() )  {
	Volume vol = plc.volume();
	Solid    s = vol.solid();
	Material m = vol.material();
	::snprintf(text,sizeof(text),"Volume:%s Shape:%s Material:%s",
		  vol->GetName(), s.isValid() ? s.name() : "Unknonw", m.isValid() ? m->GetName() : "Unknown"
		  );
	os << prefix << "+-------------  " << text << endl;
      }
      const DetElement::Children& ch = sd.children();
      for(DetElement::Children::const_iterator i=ch.begin(); i!=ch.end(); ++i)
	Printer<DetElement>(lcdd,os,prefix+"| ")((*i).second);
      return;
    }
  }

  template <typename T> void PrintMap<T>::operator()()  const {
    Printer<T> p(lcdd,os);
    os << "++" << endl << "++          " << text << endl << "++" << endl;
    for (LCDD::HandleMap::const_iterator i=cont.begin(); i != cont.end(); ++i) 
      p((*i).second);
  }

  void dumpTopVolume();
  template <> void Printer<const LCDD*>::operator()(const LCDD*const&)  const  {
    //Header(lcdd.header()).fromCompact(doc,compact.child(Tag_info),Strng_t("In memory"));
#if 0
    PrintMap<Constant  > (lcdd,os,lcdd.constants(),    "List of Constants")();
    PrintMap<Material  > (lcdd,os,lcdd.materials(),    "List of Materials")();
    PrintMap<VisAttr   > (lcdd,os,lcdd.visAttributes(),"List of Visualization attributes")();
    PrintMap<Position  > (lcdd,os,lcdd.positions(),    "List of Positions")();
    PrintMap<Rotation  > (lcdd,os,lcdd.rotations(),    "List of Rotations")();
    PrintMap<LimitSet  > (lcdd,os,lcdd.readouts(),     "List of Readouts")();
    PrintMap<Region    > (lcdd,os,lcdd.regions(),      "List of Regions")();
    PrintMap<DetElement> (lcdd,os,lcdd.detectors(),    "List of DetElements")();
#endif
    //PrintMap<DetElement>(lcdd,os,lcdd.detectors(),   "List of DetElements")();
    //PrintMap<VisAttr   > (lcdd,os,lcdd.visAttributes(),"List of Visualization attributes")();
    //mpTopVolume();
  }

  void dumpVolume(TGeoVolume* vol, int level);

  void dumpNode(TGeoNode* n, int level) {
    TGeoMatrix*  mat = n->GetMatrix();
    TGeoVolume*  vol = n->GetVolume();
    TGeoMedium*  med = vol->GetMedium();
    TGeoShape*   shape = vol->GetShape();
    TObjArray* nodes = vol->GetNodes();
    for(int i=0; i<level;++i) cout << " ";
    cout << " ++Node:|"  << n->GetName() << "| ";
    cout << " Volume: "  << vol->GetName() 
	 << " material:" << med->GetName() 
	 << " shape:"    << shape->GetName()
	 << endl;
    for(int i=0; i<level;++i) cout << " ";
    const Double_t* tr = mat->GetTranslation();
    cout << "         matrix:|"   << mat->GetName() << "|"
	 << mat->IsTranslation()
	 << mat->IsRotation() 
	 << mat->IsScale() 
	 << " tr:x=" << tr[0] << " y=" << tr[1] << " z=" << tr[2];
    if ( mat->IsRotation() ) {
      Double_t theta,phi,psi;
      TGeoRotation rot(*mat);
      rot.GetAngles(phi,theta,psi);
      cout << " rot: theta:" << theta << " phi:" << phi << " psi:" << psi;
    }
    cout << endl;
    PlacedVolume plv(n);
    for(int i=0; i<level;++i) cout << " ";
    cout << "         volume:" << plv.toString();
    cout << endl;
    TIter next(nodes);
    TGeoNode *geoNode;
    while ((geoNode = (TGeoNode *) next())) {
      dumpNode(geoNode,level+1);
    }
  }

  void dumpVolume(TGeoVolume* vol, int level) {
    TObjArray*   nodes = vol->GetNodes();
    TGeoMedium*  med = vol->GetMedium();
    TGeoShape*   shape = vol->GetShape();

    for(int i=0; i<level;++i) cout << " ";
    cout << "++Volume: " << vol->GetName() 
	 << " material:" << med->GetName() 
	 << " shape:"    << shape->GetName()
	 << endl;
    TIter next(nodes);
    TGeoNode *geoNode;
    while ((geoNode = (TGeoNode *) next())) {
      dumpNode(geoNode,level+1);
    }
  }

  void dumpTopVolume() {
    dumpVolume(gGeoManager->GetTopVolume(),0);
  }
}
