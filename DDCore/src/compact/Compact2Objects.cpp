// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/IDDescriptor.h"
#include "XML/DocumentHandler.h"
#include "xercesc/util/XMLURL.hpp"

#include "Conversions.h"
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TGeoMaterial.h"
#include "Reflex/PluginService.h"

#include <climits>
#include <iostream>
#include <set>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
namespace {
  static UInt_t unique_mat_id = 0xAFFEFEED;
}

namespace DD4hep { namespace Geometry {
  struct Compact;
  struct Materials;
  struct Includes;
  struct GdmlFile;
  typedef DD4hep::IDDescriptor IDDescriptor;
  
  template <typename T> Handle<> toObject(LCDD& lcdd, const XML::Handle_t& xml);
  
  template <> Ref_t toRefObject<Constant>(lcdd_t& lcdd, const xml_h& e)  {
    xml_ref_t    constant(e);
    TNamed*      obj = new TNamed(constant.attr<string>(_A(name)).c_str(),
                                  constant.attr<string>(_A(value)).c_str()); 
    Ref_t        cons(obj);
    _toDictionary(obj->GetName(),obj->GetTitle());
    lcdd.addConstant(cons);
    return cons;
  }
  
  template <> Ref_t toRefObject<Atom>(lcdd_t& /* lcdd */, const xml_h& e)  {
    /* <element Z="29" formula="Cu" name="Cu" >
     <atom type="A" unit="g/mol" value="63.5456" />
     </element>
     */
    xml_ref_t    elem(e);
    TGeoManager* mgr      = gGeoManager;
    XML::Tag_t   eltname  = elem.name();
    TGeoElementTable* tab = mgr->GetElementTable();
    TGeoElement*  element = tab->FindElement(eltname.c_str());
    if ( !element )  {
      xml_ref_t atom(elem.child(_X(atom)));
      tab->AddElement(elem.attr<string>(_A(name)).c_str(),
                      elem.attr<string>(_A(formula)).c_str(),
                      elem.attr<int>(_A(Z)),
                      atom.attr<int>(_A(value))
                      );
      element = tab->FindElement(eltname.c_str());
    }
    return Ref_t(element);
  }
  
  template <> Ref_t toRefObject<Material>(lcdd_t& /* lcdd */, const xml_h& e)  {
    /*  <material name="Air">
     <D type="density" unit="g/cm3" value="0.0012"/>
     <fraction n="0.754" ref="N"/>
     <fraction n="0.234" ref="O"/>
     <fraction n="0.012" ref="Ar"/>
     </material>
     <element Z="29" formula="Cu" name="Cu" >
     <atom type="A" unit="g/mol" value="63.5456" />
     </element>
     */
    xml_ref_t      m(e);
    TGeoManager*   mgr      = gGeoManager;
    XML::Tag_t     mname    = m.name();
    const char*    matname  = mname.c_str();
    xml_h     density  = m.child(XML::Tag_D);
    TGeoElementTable* table = mgr->GetElementTable();
    TGeoMaterial*     mat   = mgr->GetMaterial(matname);
    TGeoMixture*      mix   = dynamic_cast<TGeoMixture*>(mat);
    xml_coll_t        composites(m,_X(fraction));
    set<string> elts;
    if ( 0 == mat )  {
      mat = mix = new TGeoMixture(matname,composites.size(),density.attr<double>(_A(value)));
      //mgr->AddMaterial(mat);
    }
    if ( mix )  {
      for(Int_t i=0, n=mix->GetNelements(); i<n; ++i)
        elts.insert(mix->GetElement(i)->GetName());
    }
    for(; composites; ++composites)  {
      std::string nam = composites.attr<string>(_X(ref));
      TGeoElement*   element;
      if ( elts.find(nam) == elts.end() )  {
        double fraction = composites.attr<double>(_X(n));
        if ( 0 != (element=table->FindElement(nam.c_str())) )
          mix->AddElement(element,fraction);
        else if ( 0 != (mat=mgr->GetMaterial(nam.c_str())) )
          mix->AddElement(mat,fraction);
        else  {
          throw runtime_error("Something going very wrong. Undefined material:"+nam);
        }
      }
    }
    TGeoMedium* medium = mgr->GetMedium(matname);
    if ( 0 == medium )  {
      --unique_mat_id;
      medium = new TGeoMedium(matname,unique_mat_id,mat);
      medium->SetTitle("material");
      medium->SetUniqueID(unique_mat_id);
    }
    return Ref_t(medium);
  }
  
  template <> Ref_t toRefObject<IDDescriptor>(lcdd_t& /* lcdd */, const xml_h& e)  {
    /*     <id>system:6,barrel:3,module:4,layer:8,slice:5,x:32:-16,y:-16</id>   */
    Value<TNamed,IDDescriptor>* id = new Value<TNamed,IDDescriptor>();
    string dsc = e.text();
    id->construct(dsc);
    id->SetTitle(dsc.c_str());
    return Ref_t(id);
  }
  
  template <> Ref_t toRefObject<Limit>(lcdd_t& lcdd, const xml_h& e)  {
    /*     <limit name="step_length_max" particles="*" value="5.0" unit="mm" />
     */
    Limit limit(lcdd,e.attr<string>(_A(name)));
    limit.setParticles(e.attr<string>(_A(particles)));
    limit.setValue(e.attr<double>(_A(value)));
    limit.setUnit(e.attr<string>(_A(unit)));
    return limit;
  }
  
  template <> Ref_t toRefObject<LimitSet>(lcdd_t& lcdd, const xml_h& e)  {
    /*      <limitset name="...."> ... </limitset>
     */
    LimitSet ls(lcdd,XML::Tag_t(e.attr<string>(_A(name))));
    for (xml_coll_t c(e,XML::Tag_limit); c; ++c)
      ls.addLimit(toRefObject<Limit>(lcdd,c));
    return ls;
  }
  
  /// Convert compact visualization attribute to LCDD visualization attribute
  template <> Ref_t toRefObject<VisAttr>(lcdd_t& lcdd, const xml_h& e)  {
    /*    <vis name="SiVertexBarrelModuleVis" alpha="1.0" r="1.0" g="0.75" b="0.76" drawingStyle="wireframe" showDaughters="false" visible="true"/>
     */
    VisAttr attr(lcdd,e.attr<string>(_A(name)));
    float r = e.hasAttr(_A(r)) ? e.attr<float>(_A(r)) : 1.0f;
    float g = e.hasAttr(_A(g)) ? e.attr<float>(_A(g)) : 1.0f;
    float b = e.hasAttr(_A(b)) ? e.attr<float>(_A(b)) : 1.0f;
    attr.setColor(r,g,b);
    if ( e.hasAttr(_A(alpha))         ) attr.setAlpha(e.attr<float>(_A(alpha)));
    if ( e.hasAttr(_A(visible))       ) attr.setVisible(e.attr<bool>(_A(visible)));
    if ( e.hasAttr(_A(lineStyle))     )   {
      string ls = e.attr<string>(_A(lineStyle));
      if ( ls == "unbroken" ) attr.setLineStyle(VisAttr::SOLID);
      if ( ls == "broken" ) attr.setLineStyle(VisAttr::DASHED);
    }
    else  {
      attr.setLineStyle(VisAttr::SOLID);
    }
    if ( e.hasAttr(_A(drawingStyle))  )   {
      string ds = e.attr<string>(_A(drawingStyle));
      if ( ds == "wireframe" ) attr.setDrawingStyle(VisAttr::WIREFRAME);
    }
    else  {
      attr.setDrawingStyle(VisAttr::WIREFRAME);
    }
    if ( e.hasAttr(_A(showDaughters)) ) attr.setShowDaughters(e.attr<bool>(_A(showDaughters)));
    return attr;
  }
  
  template <> Elt_t toObject<GridXYZ>(lcdd_t& lcdd, const xml_h& e)  {
    GridXYZ obj(lcdd);
    if ( e.hasAttr(_A(gridSizeX)) ) obj.setGridSizeX(e.attr<float>(_A(gridSizeX)));
    if ( e.hasAttr(_A(gridSizeY)) ) obj.setGridSizeY(e.attr<float>(_A(gridSizeY)));
    if ( e.hasAttr(_A(gridSizeZ)) ) obj.setGridSizeZ(e.attr<float>(_A(gridSizeZ)));
    return obj;
  }
  template <> Elt_t toObject<GlobalGridXY>(lcdd_t& lcdd, const xml_h& e)  {
    GlobalGridXY obj(lcdd);
    if ( e.hasAttr(_A(gridSizeX)) ) obj.setGridSizeX(e.attr<float>(_A(gridSizeX)));
    if ( e.hasAttr(_A(gridSizeY)) ) obj.setGridSizeY(e.attr<float>(_A(gridSizeY)));
    return obj;
  }
  
  template <> Elt_t toObject<CartesianGridXY>(lcdd_t& lcdd, const xml_h& e)  {
    CartesianGridXY obj(lcdd);
    if ( e.hasAttr(_A(gridSizeX)) ) obj.setGridSizeX(e.attr<double>(_A(gridSizeX)));
    if ( e.hasAttr(_A(gridSizeY)) ) obj.setGridSizeY(e.attr<double>(_A(gridSizeY)));
    return obj;
  }
  
  template <> Elt_t toObject<ProjectiveCylinder>(lcdd_t& lcdd, const xml_h& e)  {
    ProjectiveCylinder obj(lcdd);
    if ( e.hasAttr(_A(phiBins))   ) obj.setPhiBins(e.attr<int>(_A(phiBins)));
    if ( e.hasAttr(_A(thetaBins)) ) obj.setThetaBins(e.attr<int>(_A(thetaBins)));
    return obj;
  }
  
  template <> Elt_t toObject<NonProjectiveCylinder>(lcdd_t& lcdd, const xml_h& e)  {
    NonProjectiveCylinder obj(lcdd);
    if ( e.hasAttr(_A(gridSizePhi)) ) obj.setThetaBinSize(e.attr<double>(_A(gridSizePhi)));
    if ( e.hasAttr(_A(gridSizeZ))   ) obj.setPhiBinSize(e.attr<double>(_A(gridSizeZ)));
    return obj;
  }
  
  template <> Elt_t toObject<ProjectiveZPlane>(lcdd_t& lcdd, const xml_h& e)  {
    ProjectiveZPlane obj(lcdd);
    if ( e.hasAttr(_A(phiBins))   ) obj.setThetaBins(e.attr<int>(_A(phiBins)));
    if ( e.hasAttr(_A(thetaBins)) ) obj.setPhiBins(e.attr<int>(_A(thetaBins)));
    return obj;
  }
  
  template <> Elt_t toObject<Segmentation>(lcdd_t& lcdd, const xml_h& e)  {
    string seg_typ = e.attr<string>(_A(type));
    if ( seg_typ == "GridXYZ" )
      return toObject<GridXYZ>(lcdd,e);
    else if ( seg_typ == "GlobalGridXY" )
      return toObject<GlobalGridXY>(lcdd,e);
    else if ( seg_typ == "CartesianGridXY" )
      return toObject<CartesianGridXY>(lcdd,e);
    else if ( seg_typ == "RegularNgonCartesianGridXY" )
      return toObject<GridXYZ>(lcdd,e);
    else if ( seg_typ == "EcalBarrelCartesianGridXY" )
      return toObject<CartesianGridXY>(lcdd,e);
    else if ( seg_typ == "ProjectiveCylinder" )
      return toObject<ProjectiveCylinder>(lcdd,e);
    else if ( seg_typ == "NonProjectiveCylinder" )
      return toObject<NonProjectiveCylinder>(lcdd,e);
    else if ( seg_typ == "ProjectiveZPlane" )
      return toObject<ProjectiveZPlane>(lcdd,e);
    else 
      cout << "Request to create UNKNOWN segmentation of type:" << seg_typ << endl;
    return Elt_t(0);
  }
  
  template <> Ref_t toRefObject<Readout>(lcdd_t& lcdd, const xml_h& e)  {
    /* <readout name="HcalBarrelHits">
     <segmentation type="RegularNgonCartesianGridXY" gridSizeX="3.0*cm" gridSizeY="3.0*cm" />
     <id>system:6,barrel:3,module:4,layer:8,slice:5,x:32:-16,y:-16</id>
     </readout>
     */
    xml_h   id, seg;
    string  name = e.attr<string>(_A(name));
    Readout ro(lcdd,name);
    if ( (seg=e.child(_X(segmentation),false)) )  { // Segmentation is not mandatory!
      string seg_typ = seg.attr<string>(_A(type));
      ro.setSegmentation(toObject<Segmentation>(lcdd,seg));
    }
    if ( (id=e.child(_X(id))) )  {
      Ref_t idSpec = toRefObject<IDDescriptor>(lcdd,id);
      idSpec->SetName(ro.name());
      ro.setIDDescriptor(idSpec);
      lcdd.addIDSpec(idSpec);
    }
    return ro;
  }
  
  namespace  {
    template <typename T> static Ref_t toRefObject(LCDD& lcdd, const xml_h& xml, SensitiveDetector& sens) 
    {  return toRefObject<T>(lcdd,xml,sens); }
  }
  
  template <> Ref_t toRefObject<SensitiveDetector>(lcdd_t& lcdd, const xml_h& e)  {
    string    nam = e.attr<string>(_A(name));
    string    typ = e.attr<string>(_A(type));
    
    if      ( e.hasAttr("calorimeterType") ) typ = "calorimeter";
    else if ( typ.find("Tracker") != string::npos ) typ = "tracker";
    else if ( nam.find("Tracker") != string::npos ) typ = "tracker";
    
    if ( e.hasAttr(_A(readout)) )  {
      Readout            ro = lcdd.readout(e.attr<string>(_A(readout)));
      Segmentation      seg = ro.segmentation();
      SensitiveDetector  sd = SensitiveDetector(lcdd,typ,nam);
      if ( seg.isValid() )  {
        sd.setSegmentation(seg);
      }
      sd.setHitsCollection(ro.name());
      sd.setIDSpec(ro.idSpec());
      lcdd.addSensitiveDetector(sd);
      return sd;
    }
    return SensitiveDetector();
  }
  
  template <> Ref_t toRefObject<Region>(lcdd_t& /* lcdd */, const xml_h& e)  {
    xml_ref_t compact(e);
    return Ref_t(0);
  }
  
  template <> void Converter<Constant>::operator()(const xml_h& element)  const  {
    lcdd.addConstant(toRefObject<to_type>(lcdd,element));
  }
  template <> void Converter<Material>::operator()(const xml_h& element)  const  {
    lcdd.addMaterial(toRefObject<to_type>(lcdd,element));
  }
  template <> void Converter<Atom>::operator()(const xml_h& element)  const  {
    //  lcdd.addMaterial(toRefObject<to_type>(lcdd,element));
    toRefObject<to_type>(lcdd,element);
  }
  template <> void Converter<VisAttr>::operator()(const xml_h& element)  const  {
    lcdd.addVisAttribute(toRefObject<to_type>(lcdd,element));
  }
  template <> void Converter<Region>::operator()(const xml_h& element)  const {
    lcdd.addRegion(toRefObject<to_type>(lcdd,element));
  }
  template <> void Converter<Readout>::operator()(const xml_h& element)  const {
    lcdd.addReadout(toRefObject<to_type>(lcdd,element));
  }
  template <> void Converter<LimitSet>::operator()(const xml_h& element)  const {
    lcdd.addLimitSet(toRefObject<to_type>(lcdd,element));
  }
  template <> void Converter<DetElement>::operator()(const xml_h& element)  const {
    static const char* req_dets = ::getenv("REQUIRED_DETECTORS");
    static const char* req_typs = ::getenv("REQUIRED_DETECTOR_TYPES");
    static const char* ign_dets = ::getenv("IGNORED_DETECTORS");
    static const char* ign_typs = ::getenv("IGNORED_DETECTOR_TYPES");
    string           type = element.attr<string>(_A(type));
    string           name = element.attr<string>(_A(name));
    string           name_match = ":"+name+":";
    string           type_match = ":"+type+":";
    if ( req_dets && !strstr(req_dets,name_match.c_str()) ) return;
    if ( req_typs && !strstr(req_typs,type_match.c_str()) ) return;
    if ( ign_dets &&  strstr(ign_dets,name_match.c_str()) ) return;
    if ( ign_typs &&  strstr(ign_typs,type_match.c_str()) ) return;
    try {
      SensitiveDetector  sd = toRefObject<SensitiveDetector>(lcdd,element);
      DetElement det(Handle<TNamed>(ROOT::Reflex::PluginService::Create<TNamed*>(type,&lcdd,&element,&sd)));
      
      if ( det.isValid() && element.hasAttr(_A(readout)) )  {
        string rdo = element.attr<string>(_A(readout));
        det.setReadout(lcdd.readout(rdo));
      }
      cout << (det.isValid() ? "Converted" : "FAILED    ")
      << " subdetector:" << name << " of type " << type << endl;
      lcdd.addDetector(det);
    }
    catch(const exception& e) {
      cout << "FAILED    to convert subdetector:" << name << " of type " << type << ": " << e.what() << endl;
    }
    catch(...) {
      cout << "FAILED    to convert subdetector:" << name << " of type " << type << ": UNKNONW Exception" << endl;
    }
  }
  
  template <> void Converter<Materials>::operator()(const xml_h& materials)  const  {
    xml_coll_t(materials,_X(element) ).for_each(Converter<Atom>(lcdd));
    xml_coll_t(materials,_X(material)).for_each(Converter<Material>(lcdd));
  }
  
  template <> void Converter<GdmlFile>::operator()(const xml_h& element)  const  {
    xercesc::XMLURL base(element.ptr()->getBaseURI());
    xercesc::XMLURL ref(base, element.attr_value(_A(ref)));
    xml_h materials = XML::DocumentHandler().load(_toString(ref.getURLText())).root();
    Converter<Materials>(this->lcdd)(materials);
  }
  
  template <> void Converter<Compact>::operator()(const xml_h& element)  const  {
    XML::Element compact(element);
    lcdd.create();
    xml_coll_t(compact,_X(includes) ).for_each(_X(gdmlFile),Converter<GdmlFile>(lcdd));
    //Header(lcdd.header()).fromCompact(doc,compact.child(Tag_info),Strng_t("In memory"));
    xml_coll_t(compact,_X(define)   ).for_each(_X(constant),Converter<Constant>(lcdd));
    xml_coll_t(compact,_X(materials)).for_each(_X(element), Converter<Atom>(lcdd));
    xml_coll_t(compact,_X(materials)).for_each(_X(material),Converter<Material>(lcdd));
    
    lcdd.init();
    xml_coll_t(compact,_X(limits)   ).for_each(_X(limitset),Converter<LimitSet>(lcdd));
    xml_coll_t(compact,_X(display)  ).for_each(_X(vis),     Converter<VisAttr>(lcdd));
    xml_coll_t(compact,_X(readouts) ).for_each(_X(readout), Converter<Readout>(lcdd));
    xml_coll_t(compact,_X(detectors)).for_each(_X(detector),Converter<DetElement>(lcdd));
    lcdd.endDocument();
  }
}}

#ifdef _WIN32
template Converter<Atom>;
template Converter<Compact>;
template Converter<Readout>;
template Converter<VisAttr>;
template Converter<Constant>;
template Converter<LimitSet>;
template Converter<Material>;
template Converter<Materials>;
template Converter<DetElement>;
#endif
