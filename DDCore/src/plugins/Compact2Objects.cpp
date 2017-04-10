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

// Framework includes
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/FieldTypes.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/objects/SegmentationsInterna.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ObjectsInterna.h"

#include "XML/DocumentHandler.h"
#include "XML/Utilities.h"

// Root/TGeo include files
#include "TGeoManager.h"
#include "TGeoMaterial.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <iomanip>
#include <set>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep {
  namespace Geometry {
    class Debug;
    class Isotope;
    class Plugin;
    class Compact;
    class Includes;
    class GdmlFile;
    class Property;
    class XMLFile;
    class JsonFile;
    class AlignmentFile;
    class DetElementInclude {};
  }
  template <> void Converter<Debug>::operator()(xml_h element) const;
  template <> void Converter<Plugin>::operator()(xml_h element) const;
  template <> void Converter<Constant>::operator()(xml_h element) const;
  template <> void Converter<Material>::operator()(xml_h element) const;
  template <> void Converter<Atom>::operator()(xml_h element) const;
  template <> void Converter<Isotope>::operator()(xml_h element) const;
  template <> void Converter<VisAttr>::operator()(xml_h element) const;
  template <> void Converter<AlignmentEntry>::operator()(xml_h element) const;
  template <> void Converter<Region>::operator()(xml_h element) const;
  template <> void Converter<Readout>::operator()(xml_h element) const;
  template <> void Converter<Segmentation>::operator()(xml_h element) const;
  template <> void Converter<LimitSet>::operator()(xml_h element) const;
  template <> void Converter<Property>::operator()(xml_h element) const;
  template <> void Converter<CartesianField>::operator()(xml_h element) const;
  template <> void Converter<SensitiveDetector>::operator()(xml_h element) const;
  template <> void Converter<DetElement>::operator()(xml_h element) const;
  template <> void Converter<GdmlFile>::operator()(xml_h element) const;
  template <> void Converter<JsonFile>::operator()(xml_h element) const;
  template <> void Converter<XMLFile>::operator()(xml_h element) const;
  template <> void Converter<AlignmentFile>::operator()(xml_h element) const;
  template <> void Converter<Header>::operator()(xml_h element) const;
  template <> void Converter<DetElementInclude>::operator()(xml_h element) const;
  template <> void Converter<Compact>::operator()(xml_h element) const;
}

namespace {
  static UInt_t unique_mat_id = 0xAFFEFEED;
  void throw_print(const string& msg) {
    printout(ERROR, "Compact", msg.c_str());
    throw runtime_error(msg);
  }
  bool s_debug_readout      = false;
  bool s_debug_regions      = false;
  bool s_debug_limits       = false;
  bool s_debug_visattr      = false;
  bool s_debug_isotopes     = false;
  bool s_debug_elements     = false;
  bool s_debug_materials    = false;
  bool s_debug_segmentation = false;
}

static Ref_t create_ConstantField(lcdd_t& /* lcdd */, xml_h e) {
  CartesianField obj;
  xml_comp_t field(e), strength(e.child(_U(strength)));
  string t = e.attr<string>(_U(field));
  ConstantField* ptr = new ConstantField();
  ptr->type = ::toupper(t[0]) == 'E' ? CartesianField::ELECTRIC : CartesianField::MAGNETIC;
  ptr->direction.SetX(strength.x());
  ptr->direction.SetY(strength.y());
  ptr->direction.SetZ(strength.z());
  obj.assign(ptr, field.nameStr(), field.typeStr());
  return obj;
}
DECLARE_XMLELEMENT(ConstantField,create_ConstantField)

static Ref_t create_SolenoidField(lcdd_t& lcdd, xml_h e) {
  xml_comp_t c(e);
  bool has_inner_radius = c.hasAttr(_U(inner_radius));
  bool has_outer_radius = c.hasAttr(_U(outer_radius));

  if (!has_inner_radius && !has_outer_radius) {
    throw_print("Compact2Objects[ERROR]: For a solenoidal field at least one of the "
                " xml attributes inner_radius of outer_radius MUST be set.");
  }
  CartesianField obj;
  SolenoidField* ptr = new SolenoidField();
  //
  // This logic is a bit weird, but has it's origin in the compact syntax:
  // If no "inner_radius" is given, the "outer_radius" IS the "inner_radius"
  // and the "outer_radius" is given by one side of the world volume's box
  //
  if (has_inner_radius && has_outer_radius) {
    ptr->innerRadius = c.attr<double>(_U(inner_radius));
    ptr->outerRadius = c.attr<double>(_U(outer_radius));
  }
  else if (has_inner_radius) {
    Box box = lcdd.worldVolume().solid();
    ptr->innerRadius = c.attr<double>(_U(inner_radius));
    ptr->outerRadius = box.x();
  }
  else if (has_outer_radius) {
    Box box = lcdd.worldVolume().solid();
    ptr->innerRadius = c.attr<double>(_U(outer_radius));
    ptr->outerRadius = box.x();
  }
  if (c.hasAttr(_U(inner_field)))
    ptr->innerField = c.attr<double>(_U(inner_field));
  if (c.hasAttr(_U(outer_field)))
    ptr->outerField = c.attr<double>(_U(outer_field));
  if (c.hasAttr(_U(zmax)))
    ptr->maxZ = c.attr<double>(_U(zmax));
  else
    ptr->maxZ = lcdd.constant<double>("world_side");
  if (c.hasAttr(_U(zmin)))
    ptr->minZ = c.attr<double>(_U(zmin));
  else
    ptr->minZ = -ptr->maxZ;
  obj.assign(ptr, c.nameStr(), c.typeStr());
  return obj;
}
DECLARE_XMLELEMENT(SolenoidMagnet,create_SolenoidField)
// This is the plugin required for slic: note the different name
DECLARE_XMLELEMENT(solenoid,create_SolenoidField)

static Ref_t create_DipoleField(lcdd_t& /* lcdd */, xml_h e) {
  xml_comp_t c(e);
  CartesianField obj;
  DipoleField* ptr = new DipoleField();
  double lunit = c.hasAttr(_U(lunit)) ? c.attr<double>(_U(lunit)) : 1.0;
  double funit = c.hasAttr(_U(funit)) ? c.attr<double>(_U(funit)) : 1.0;
  double val, mult = funit;

  if (c.hasAttr(_U(zmin)))
    ptr->zmin = _multiply<double>(c.attr<string>(_U(zmin)), lunit);
  if (c.hasAttr(_U(zmax)))
    ptr->zmax = _multiply<double>(c.attr<string>(_U(zmax)), lunit);
  if (c.hasAttr(_U(rmax)))
    ptr->rmax = _multiply<double>(c.attr<string>(_U(rmax)), lunit);
  for (xml_coll_t coll(c, _U(dipole_coeff)); coll; ++coll, mult /= lunit) {
    xml_dim_t coeff = coll;
    if ( coeff.hasAttr(_U(value)) )
      val = coll.attr<double>(_U(value)) * mult;
    else if ( coeff.hasAttr(_U(coefficient)) )
      val = coeff.coefficient() * mult;
    else
      val = _multiply<double>(coll.text(), mult);
    ptr->coefficents.push_back(val);
  }
  obj.assign(ptr, c.nameStr(), c.typeStr());
  return obj;
}
DECLARE_XMLELEMENT(DipoleMagnet,create_DipoleField)

static Ref_t create_MultipoleField(lcdd_t& lcdd, xml_h e) {
  xml_dim_t c(e), child;
  CartesianField obj;
  MultipoleField* ptr = new MultipoleField();
  double lunit = c.hasAttr(_U(lunit)) ? c.attr<double>(_U(lunit)) : 1.0;
  double funit = c.hasAttr(_U(funit)) ? c.attr<double>(_U(funit)) : 1.0;
  double val, mult = funit, bz = 0.0;
  RotationZYX rot;
  Position pos;

  if (c.hasAttr(_U(Z))) bz = c.Z() * funit;
  if ((child = c.child(_U(position), false))) {   // Position is not mandatory!
    pos.SetXYZ(child.x(), child.y(), child.z());
  }
  if ((child = c.child(_U(rotation), false))) {   // Rotation is not mandatory
    rot.SetComponents(child.z(), child.y(), child.x());
  }
  if ((child = c.child(_U(shape), false))) {      // Shape is not mandatory
    string type = child.typeStr();
    ptr->volume = XML::createShape(lcdd, type, child);
  }
  ptr->B_z = bz;
  ptr->transform = Transform3D(rot,pos).Inverse();
  for (xml_coll_t coll(c, _U(coefficient)); coll; ++coll, mult /= lunit) {
    xml_dim_t coeff = coll;
    if ( coll.hasAttr(_U(value)) )
      val = coll.attr<double>(_U(value)) * mult;
    else
      val = coeff.coefficient(0.0) * mult;
    ptr->coefficents.push_back(val);
    val = coeff.skew(0.0) * mult;
    ptr->skews.push_back(val);
  }
  obj.assign(ptr, c.nameStr(), c.typeStr());
  return obj;
}
DECLARE_XMLELEMENT(MultipoleMagnet,create_MultipoleField)

static long create_Compact(lcdd_t& lcdd, xml_h element) {
  Converter<Compact>converter(lcdd);
  converter(element);
  return 1;
}
DECLARE_XML_DOC_READER(lccdd,create_Compact)

/** Convert parser debug flags.
 */
template <> void Converter<Debug>::operator()(xml_h e) const {
  for (xml_coll_t coll(e, _U(type)); coll; ++coll) {
    string nam = coll.attr<string>(_U(name));
    int    val = coll.attr<int>(_U(value));
    if      ( nam.substr(0,6) == "isotop" ) s_debug_isotopes     = (0 != val);
    else if ( nam.substr(0,6) == "elemen" ) s_debug_elements     = (0 != val);
    else if ( nam.substr(0,6) == "materi" ) s_debug_materials    = (0 != val);
    else if ( nam.substr(0,6) == "visatt" ) s_debug_visattr      = (0 != val);
    else if ( nam.substr(0,6) == "region" ) s_debug_regions      = (0 != val);
    else if ( nam.substr(0,6) == "readou" ) s_debug_readout      = (0 != val);
    else if ( nam.substr(0,6) == "limits" ) s_debug_limits       = (0 != val);
    else if ( nam.substr(0,6) == "segmen" ) s_debug_segmentation = (0 != val);
  }
}
  
/** Convert/execute plugin objects from the xml (plugins)
 *
 *
 */
template <> void Converter<Plugin>::operator()(xml_h e) const {
  xml_comp_t plugin(e);
  vector<char*> argv;
  vector<string> arguments;
  string name = plugin.nameStr();
  for (xml_coll_t coll(e, _U(arg)); coll; ++coll) {
    string val = coll.attr<string>(_U(value));
    arguments.push_back(val);
  }
  for (xml_coll_t coll(e, _U(argument)); coll; ++coll) {
    string val = coll.attr<string>(_U(value));
    arguments.push_back(val);
  }
  for(vector<string>::iterator i=arguments.begin(); i!=arguments.end(); ++i)
    argv.push_back(&((*i)[0]));
  lcdd.apply(name.c_str(),int(argv.size()), &argv[0]);
}

/** Convert compact constant objects (defines)
 *
 *
 */
template <> void Converter<Constant>::operator()(xml_h e) const {
  xml_ref_t constant(e);
  string nam = constant.attr<string>(_U(name));
  string val = constant.attr<string>(_U(value));
  string typ = constant.hasAttr(_U(type)) ? constant.attr<string>(_U(type)) : "number";
  Constant c(nam, val, typ);
  _toDictionary(nam, val, typ);
  lcdd.addConstant(c);
}
/** Convert compact constant objects (defines)
 *
 *
 */
template <> void Converter<Header>::operator()(xml_h e) const {
  xml_comp_t c(e);
  Header h(e.attr<string>(_U(name)), e.attr<string>(_U(title)));
  h.setUrl(e.attr<string>(_U(url)));
  h.setAuthor(e.attr<string>(_U(author)));
  h.setStatus(e.attr<string>(_U(status)));
  h.setVersion(e.attr<string>(_U(version)));
  h.setComment(e.child(_U(comment)).text());
  lcdd.setHeader(h);
}

/** Convert compact material/element description objects
 *
 *  Materials:
 *  <material name="Air">
 *    <D type="density" unit="g/cm3" value="0.0012"/>
 *    <fraction n="0.754" ref="N"/>
 *    <fraction n="0.234" ref="O"/>
 *    <fraction n="0.012" ref="Ar"/>
 *  </material>
 *
 *  Elements:
 *  <element Z="29" formula="Cu" name="Cu" >
 *    <atom type="A" unit="g/mol" value="63.5456" />
 *  </element>
 *
 */
template <> void Converter<Material>::operator()(xml_h e) const {
  xml_ref_t m(e);
  TGeoManager&      mgr = lcdd.manager();
  xml_tag_t         mname = m.name();
  const char*       matname = mname.c_str();
  TGeoElementTable* table = mgr.GetElementTable();
  TGeoMaterial*     mat = mgr.GetMaterial(matname);
  TGeoMixture*      mix = dynamic_cast<TGeoMixture*>(mat);
  xml_coll_t        fractions(m, _U(fraction));
  xml_coll_t        composites(m, _U(composite));

  if (0 == mat) {
    TGeoMaterial* comp_mat;
    TGeoElement*  comp_elt;
    xml_h  density     = m.child(_U(D), false);
    double dens_val    = density.ptr() ? density.attr<double>(_U(value)) : 0.0;
    double dens_unit   = 1.0;

    if ( !density.ptr() ) {
      throw_print("Compact2Objects[ERROR]: material without density tag ( <D  unit=\"g/cm3\" value=\"..\"/> ) provided: "
                  + string( matname ) ) ;
    }
    if ( density.hasAttr(_U(unit)) )   {
      dens_unit = density.attr<double>(_U(unit))/XML::_toDouble(_Unicode(gram/cm3));
    }
    if ( dens_unit != 1.0 )  {
      cout << matname << " Density unit:" << dens_unit;
      if ( dens_unit != 1.0 ) cout << " " << density.attr<string>(_U(unit));
      cout << " Density Value raw:" << dens_val << " normalized:" << (dens_val*dens_unit) << endl;
      dens_val *= dens_unit;
    }
    printout(s_debug_materials ? ALWAYS : DEBUG, "Compact",
             "++ Converting material %-16s  Density: %.3f.",matname, dens_val);
#if 0
    cout << "Gev    " << XML::_toDouble(_Unicode(GeV)) << endl;
    cout << "sec    " << XML::_toDouble(_Unicode(second)) << endl;
    cout << "nsec   " << XML::_toDouble(_Unicode(nanosecond)) << endl;
    cout << "kilo   " << XML::_toDouble(_Unicode(kilogram)) << endl;
    cout << "kilo   " << XML::_toDouble(_Unicode(joule*s*s/(m*m))) << endl;
    cout << "meter  " << XML::_toDouble(_Unicode(meter)) << endl;
    cout << "ampere " << XML::_toDouble(_Unicode(ampere)) << endl;
    cout << "degree " << XML::_toDouble(_Unicode(degree)) << endl;
#endif
    //throw 1;
    mat = mix = new TGeoMixture(matname, composites.size(), dens_val);
    size_t ifrac = 0;
    vector<double> composite_fractions;
    double composite_fractions_total = 0.0;
    for (composites.reset(); composites; ++composites)   {
      string nam = composites.attr<string>(_U(ref));
      double fraction = composites.attr<double>(_U(n));
      if (0 != (comp_mat = mgr.GetMaterial(nam.c_str())))
        fraction *= comp_mat->GetA();
      else if (0 != (comp_elt = table->FindElement(nam.c_str())))
        fraction *= comp_elt->A();
      else
        except("Compact2Objects","Converting material: %s Element missing: %s",mname.c_str(),nam.c_str());
      composite_fractions_total += fraction;
      composite_fractions.push_back(fraction);
    }
    for (composites.reset(), ifrac=0; composites; ++composites, ++ifrac) {
      string nam = composites.attr<string>(_U(ref));
      double fraction = composite_fractions[ifrac]/composite_fractions_total;
      if (0 != (comp_mat = mgr.GetMaterial(nam.c_str())))
        mix->AddElement(comp_mat, fraction);
      else if (0 != (comp_elt = table->FindElement(nam.c_str())))
        mix->AddElement(comp_elt, fraction);
    }
    for (fractions.reset(); fractions; ++fractions) {
      string nam = fractions.attr<string>(_U(ref));
      double fraction = fractions.attr<double>(_U(n));
      if (0 != (comp_mat = mgr.GetMaterial(nam.c_str())))
        mix->AddElement(comp_mat, fraction);
      else if (0 != (comp_elt = table->FindElement(nam.c_str())))
        mix->AddElement(comp_elt, fraction);
      else
        throw_print("Compact2Objects[ERROR]: Converting material:" + mname + " Element missing: " + nam);
    }
    mix->SetRadLen(0e0);

    //fg: calling SetDensity for TGeoMixture results in incorrect radLen and intLen ( computed only from first element ) 
    // // Update estimated density if not provided.
    // if ( has_density )   {
    //   mix->SetDensity(dens_val);
    // }
    // else if (!has_density && mix && 0 == mix->GetDensity()) {
    //   double dens = 0.0;
    //   for (composites.reset(), ifrac=0; composites; ++composites, ++ifrac) {
    //     string nam = composites.attr<string>(_U(ref));
    //     comp_mat = mgr.GetMaterial(nam.c_str());
    //     dens += composites.attr<double>(_U(n)) * comp_mat->GetDensity();
    //   }
    //   for (fractions.reset(); fractions; ++fractions) {
    //     string nam = fractions.attr<string>(_U(ref));
    //     comp_mat = mgr.GetMaterial(nam.c_str());
    //     dens += composites.attr<double>(_U(n)) * comp_mat->GetDensity();
    //   }
    //   printout(WARNING, "Compact", "++ Material: %s with NO density. "
    //            "Set density to %7.3 g/cm**3", matname, dens);
    //   mix->SetDensity(dens);
    // }
    
  }
  TGeoMedium* medium = mgr.GetMedium(matname);
  if (0 == medium) {
    --unique_mat_id;
    medium = new TGeoMedium(matname, unique_mat_id, mat);
    medium->SetTitle("material");
    medium->SetUniqueID(unique_mat_id);
  }
  // TGeo has no notion of a material "formula"
  // Hence, treat the formula the same way as the material itself
  if (m.hasAttr(_U(formula))) {
    string form = m.attr<string>(_U(formula));
    if (form != matname) {
      medium = mgr.GetMedium(form.c_str());
      if (0 == medium) {
        --unique_mat_id;
        medium = new TGeoMedium(form.c_str(), unique_mat_id, mat);
        medium->SetTitle("material");
        medium->SetUniqueID(unique_mat_id);
      }
    }
  }
}

/** Convert compact isotope objects
 *
 *   <isotope name="C12" Z="2" N="12"/>
 *     <atom unit="g/mole" value="12"/>
 *   </isotope>
 */
template <> void Converter<Isotope>::operator()(xml_h e) const {
  xml_dim_t isotope(e);
  TGeoManager&      mgr  = lcdd.manager();
  string            nam  = isotope.nameStr();
  TGeoElementTable* tab  = mgr.GetElementTable();
  TGeoIsotope*      iso  = tab->FindIsotope(nam.c_str());

  // Create the isotope object in the event it is not yet present from the XML data
  if ( !iso )   {
    xml_ref_t atom(isotope.child(_U(atom)));
    int    n     = isotope.attr<int>(_U(N));
    int    z     = isotope.attr<int>(_U(Z));
    double value = atom.attr<double>(_U(value));
    string unit  = atom.attr<string>(_U(unit));
    double a     = value * _multiply<double>(unit,"mol/g");
    iso = new TGeoIsotope(nam.c_str(), z, n, a);
    printout(s_debug_isotopes ? ALWAYS : DEBUG, "Compact",
             "++ Converting isotope  %-16s  Z:%3d N:%3d A:%8.4f [g/mol]",
             iso->GetName(), iso->GetZ(), iso->GetN(), iso->GetA());
  }
  else  {
    printout(WARNING, "Compact",
             "++ Isotope %-16s  Z:%3d N:%3d A:%8.4f [g/mol] ALREADY defined. [Ignore definition]",
             iso->GetName(), iso->GetZ(), iso->GetN(), iso->GetA());
  }
}

/** Convert compact atom objects (periodic elements)
 *
 *   <element Z="4" formula="Be" name="Be" >
 *     <atom type="A" unit="g/mol" value="9.01218" />
 *   </element>
 *   or
 *   <element name="C">
 *     <fraction n="0.9893" ref="C12"/>
 *     <fraction n="0.0107" ref="C13"/>
 *   </element>
 * 
 *   Please note: 
 *   Elements may consist of a mixture of isotopes!
 */
template <> void Converter<Atom>::operator()(xml_h e) const {
  xml_ref_t         elem(e);
  xml_tag_t         name = elem.name();
  TGeoManager&      mgr  = lcdd.manager();
  TGeoElementTable* tab  = mgr.GetElementTable();
  TGeoElement*      elt  = tab->FindElement(name.c_str());
  if ( !elt ) {
    if ( elem.hasChild(_U(atom)) )  {
      xml_ref_t atom(elem.child(_U(atom)));
      string formula = elem.attr<string>(_U(formula));
      double value   = atom.attr<double>(_U(value));
      string unit    = atom.attr<string>(_U(unit));
      int    z       = elem.attr<int>(_U(Z));
      double a       = value*_multiply<double>(unit,"mol/g");
      printout(s_debug_elements ? ALWAYS : DEBUG, "Compact",
               "++ Converting element  %-16s  [%-3s] Z:%3d A:%8.4f [g/mol]",
               name.c_str(), formula.c_str(), z, a);
      tab->AddElement(name.c_str(), formula.c_str(), z, a);
    }
    else  {
      int num_isotopes = 0;
      string formula   = elem.hasAttr(_U(formula)) ? elem.attr<string>(_U(formula)) : name.str();
      for( xml_coll_t i(elem,_U(fraction)); i; ++i)
        ++num_isotopes;
      elt = new TGeoElement(name.c_str(), formula.c_str(), num_isotopes);
      tab->AddElement(elt);
      for( xml_coll_t i(elem,_U(fraction)); i; ++i)  {
        double frac = i.attr<double>(_U(n));
        string ref  = i.attr<string>(_U(ref));
        TGeoIsotope* iso = tab->FindIsotope(ref.c_str());
        if ( !iso )  {
          except("Compact","Element %s cannot be constructed. Isotope '%s' (fraction:%f) missing!",
                 name.c_str(), ref.c_str(), frac);
        }
        printout(s_debug_elements ? ALWAYS : DEBUG, "Compact",
                 "++ Converting element  %-16s  Add isotope: %-16s fraction:%.4f.",
                 name.c_str(), ref.c_str(), frac);
        elt->AddIsotope(iso, frac);
      }
      printout(s_debug_elements ? ALWAYS : DEBUG, "Compact",
               "++ Converted  element  %-16s  [%-3s] Z:%3d A:%8.4f [g/mol] with %d isotopes.",
               name.c_str(), formula.c_str(), elt->Z(), elt->A(), num_isotopes);
    }
    elt = tab->FindElement(name.c_str());
    if (!elt) {
      throw_print("Failed to properly insert the Element:"+name+" into the element table!");
    }
  }
  else  {
    printout(WARNING, "Compact", "++ Element %-16s  Z:%3d N:%3d A:%8.4f [g/mol] ALREADY defined. [Ignore definition]",
             elt->GetName(), elt->Z(), elt->N(), elt->A());
  }
}

/** Convert compact visualization attribute to LCDD visualization attribute
 *
 *  <vis name="SiVertexBarrelModuleVis"
 *       alpha="1.0" r="1.0" g="0.75" b="0.76"
 *       drawingStyle="wireframe"
 *       showDaughters="false"
 *       visible="true"/>
 */
template <> void Converter<VisAttr>::operator()(xml_h e) const {
  VisAttr attr(e.attr<string>(_U(name)));
  float r = e.hasAttr(_U(r)) ? e.attr<float>(_U(r)) : 1.0f;
  float g = e.hasAttr(_U(g)) ? e.attr<float>(_U(g)) : 1.0f;
  float b = e.hasAttr(_U(b)) ? e.attr<float>(_U(b)) : 1.0f;

  printout(s_debug_visattr ? ALWAYS : DEBUG, "Compact",
           "++ Converting VisAttr  structure: %-16s. R=%.3f G=%.3f B=%.3f",
           attr.name(), r, g, b);
  attr.setColor(r, g, b);
  if (e.hasAttr(_U(alpha)))
    attr.setAlpha(e.attr<float>(_U(alpha)));
  if (e.hasAttr(_U(visible)))
    attr.setVisible(e.attr<bool>(_U(visible)));
  if (e.hasAttr(_U(lineStyle))) {
    string ls = e.attr<string>(_U(lineStyle));
    if (ls == "unbroken")
      attr.setLineStyle(VisAttr::SOLID);
    else if (ls == "broken")
      attr.setLineStyle(VisAttr::DASHED);
  }
  else {
    attr.setLineStyle(VisAttr::SOLID);
  }
  if (e.hasAttr(_U(drawingStyle))) {
    string ds = e.attr<string>(_U(drawingStyle));
    if (ds == "wireframe")
      attr.setDrawingStyle(VisAttr::WIREFRAME);
    else if (ds == "solid")
      attr.setDrawingStyle(VisAttr::SOLID);
  }
  else {
    attr.setDrawingStyle(VisAttr::SOLID);
  }
  if (e.hasAttr(_U(showDaughters)))
    attr.setShowDaughters(e.attr<bool>(_U(showDaughters)));
  else
    attr.setShowDaughters(true);
  lcdd.addVisAttribute(attr);
}

/** Specialized converter for compact AlignmentEntry objects.
 *
 *  <alignment name="<path/to/object>"  shortcut="short_cut_name">
 *    <position x="x-value" y="y-value" z="z-value"/>
 *    <rotation theta="theta-value" phi="phi-value" psi="psi-value"/>
 *  </alignment>
 */
template <> void Converter<AlignmentEntry>::operator()(xml_h e) const {
  xml_comp_t child(e);
  string path = e.attr<string>(_U(name));
  bool check = e.hasAttr(_U(check));
  bool overlap = e.hasAttr(_U(overlap));
  AlignmentEntry alignment(path);
  Position pos;
  RotationZYX rot;
  if ((child = e.child(_U(position), false))) {   // Position is not mandatory!
    pos.SetXYZ(child.x(), child.y(), child.z());
  }
  if ((child = e.child(_U(rotation), false))) {   // Rotation is not mandatory
    rot.SetComponents(child.z(), child.y(), child.x());
  }
  if (overlap) {
    double ovl = e.attr<double>(_U(overlap));
    alignment.align(pos, rot, check, ovl);
  }
  else {
    alignment.align(pos, rot, check);
  }
  lcdd.addAlignment(alignment);
}

/** Specialized converter for compact region objects.
 *
 */
template <> void Converter<Region>::operator()(xml_h elt) const {
  xml_dim_t  e = elt;
  Region     region(e.nameStr());
  vector<string>&limits = region.limits();
  xml_attr_t cut = elt.attr_nothrow(_U(cut));
  xml_attr_t threshold = elt.attr_nothrow(_U(threshold));
  xml_attr_t store_secondaries = elt.attr_nothrow(_U(store_secondaries));
  double ene = e.eunit(1.0), len = e.lunit(1.0);

  printout(s_debug_regions ? ALWAYS : DEBUG, "Compact",
           "++ Converting region   structure: %s.",region.name());
  if ( cut )  {
    region.setCut(elt.attr<double>(cut)*len);
  }
  if ( threshold )  {
    region.setThreshold(elt.attr<double>(threshold)*ene);
  }
  if ( store_secondaries )  {
    region.setStoreSecondaries(elt.attr<bool>(store_secondaries));
  }
  for (xml_coll_t user_limits(e, _U(limitsetref)); user_limits; ++user_limits)
    limits.push_back(user_limits.attr<string>(_U(name)));
  lcdd.addRegion(region);
}


/** Specialized converter for compact readout objects.
 *
 * <readout name="HcalBarrelHits">
 *   <segmentation type="RegularNgonCartesianGridXY" gridSizeX="3.0*cm" gridSizeY="3.0*cm" />
 *   <id>system:6,barrel:3,module:4,layer:8,slice:5,x:32:-16,y:-16</id>
 * </readout>
 */
template <> void Converter<Segmentation>::operator()(xml_h seg) const {
  string type = seg.attr<string>(_U(type));
  string name = seg.hasAttr(_U(name)) ? seg.attr<string>(_U(name)) : string();
  std::pair<Segmentation,IDDescriptor>* opt = _option<pair<Segmentation,IDDescriptor> >();

  BitField64* bitfield = opt->second.ptr();
  Segmentation segment(type, name, bitfield);
  if (segment.isValid()) {
    typedef Segmentation::Parameters _PARS;
    const _PARS& pars = segment.parameters();
    printout(s_debug_segmentation ? ALWAYS : DEBUG, "Compact",
             "++ Converting segmentation structure: %s of type %s.",name.c_str(),type.c_str());
    for(_PARS::const_iterator it = pars.begin(); it != pars.end(); ++it) {
      Segmentation::Parameter p = *it;
      XML::Strng_t pNam(p->name());
      if ( seg.hasAttr(pNam) ) {
        string pType = p->type();
        if ( pType.compare("int") == 0 ) {
          typedef DDSegmentation::TypedSegmentationParameter<int> ParInt;
          static_cast<ParInt*>(p)->setTypedValue(seg.attr<int>(pNam));
        } else if ( pType.compare("float") == 0 ) {
          typedef DDSegmentation::TypedSegmentationParameter<float> ParFloat;
          static_cast<ParFloat*>(p)->setTypedValue(seg.attr<float>(pNam));
        } else if ( pType.compare("doublevec") == 0 ) {
          vector<double> valueVector;
          string par = seg.attr<string>(pNam);
          printout(s_debug_segmentation ? ALWAYS : DEBUG, "Compact",
                   "++ Converting this string structure: %s.",par.c_str());
          vector<string> elts = DDSegmentation::splitString(par);
          for (vector<string>::const_iterator j = elts.begin(); j != elts.end(); ++j) {
            if ((*j).empty()) continue;
            valueVector.push_back(_toDouble((*j)));
          }
          typedef DDSegmentation::TypedSegmentationParameter< vector<double> > ParDouVec;
          static_cast<ParDouVec*>(p)->setTypedValue(valueVector);
        } else if ( pType.compare("double" ) == 0) {
          typedef DDSegmentation::TypedSegmentationParameter<double>ParDouble;
          static_cast<ParDouble*>(p)->setTypedValue(seg.attr<double>(pNam));
        } else {
          p->setValue(seg.attr<string>(pNam));
        }
      } else if (not p->isOptional()) {
        throw_print("FAILED to create segmentation: " + type +
                    ". Missing mandatory parameter: " + p->name() + "!");
      }
    }
    long key_min = 0, key_max = 0;
    DDSegmentation::Segmentation* base = segment->segmentation;
    for(xml_coll_t sub(seg,_U(segmentation)); sub; ++sub)   {
      std::pair<Segmentation,IDDescriptor> sub_object(Segmentation(),opt->second);
      Converter<Segmentation> sub_conv(lcdd,param,&sub_object);
      sub_conv(sub);
      if ( sub_object.first.isValid() )  {
        Segmentation sub_seg = sub_object.first;
        xml_dim_t s(sub);
        if ( sub.hasAttr(_U(key_value)) ) {
          key_min = key_max = s.key_value();
        }
        else if ( sub.hasAttr(_U(key_min)) && sub.hasAttr(_U(key_max)) )  {
          key_min = s.key_min();
          key_max = s.key_max();
        }
        else  {
          stringstream tree;
          XML::dump_tree(sub,tree);
          throw_print("Nested segmentations: Invalid key specification:"+tree.str());
        }
        printout(s_debug_segmentation ? ALWAYS : DEBUG,"Compact",
                 "++ Segmentation [%s/%s]: Add sub-segmentation %s [%s]",
                 name.c_str(), type.c_str(), 
                 sub_seg->segmentation->name().c_str(),
                 sub_seg->segmentation->type().c_str());
        base->addSubsegmentation(key_min, key_max, sub_seg->segmentation);
        sub_seg->segmentation = 0;
        delete sub_seg.ptr();
      }
    }
  }
  opt->first = segment;
}

/** Specialized converter for compact readout objects.
 *
 * <readout name="HcalBarrelHits">
 *   <segmentation type="RegularNgonCartesianGridXY" gridSizeX="3.0*cm" gridSizeY="3.0*cm" />
 *   <id>system:6,barrel:3,module:4,layer:8,slice:5,x:32:-16,y:-16</id>
 * </readout>
 */
template <> void Converter<Readout>::operator()(xml_h e) const {
  xml_h seg = e.child(_U(segmentation), false);
  xml_h id = e.child(_U(id));
  string name = e.attr<string>(_U(name));
  std::pair<Segmentation,IDDescriptor> opt;
  Readout ro(name);
  
  if (id) {
    //  <id>system:6,barrel:3,module:4,layer:8,slice:5,x:32:-16,y:-16</id>
    opt.second = IDDescriptor(id.text());
    opt.second->SetName(ro.name());
    lcdd.addIDSpecification(opt.second);
  }
  if (seg) {   // Segmentation is not mandatory!
    Converter<Segmentation>(lcdd,param,&opt)(seg);
    opt.first->setName(name);
  }
  /// The next 2 if-clauses are a bit tricky, because they are not commutativ.
  /// The segmentation MUST be set first - THEN the ID descriptor, since it will
  /// update the segmentation.
  if ( opt.first.isValid() )   {
    ro.setSegmentation(opt.first);
  }
  if ( opt.second.isValid() )  {
    ro.setIDDescriptor(opt.second);
  }
  
  printout(s_debug_readout ? ALWAYS : DEBUG,
           "Compact", "++ Converting readout  structure: %-16s. %s%s",
           ro.name(), id ? "ID: " : "", id ? id.text().c_str() : "");
  
  for(xml_coll_t colls(e,_U(hits_collections)); colls; ++colls)   {
    string hits_key;
    if ( colls.hasAttr(_U(key)) ) hits_key = colls.attr<string>(_U(key));
    for(xml_coll_t coll(colls,_U(hits_collection)); coll; ++coll)   {
      xml_dim_t c(coll);
      string coll_name = c.nameStr();
      string coll_key  = hits_key;
      long   key_min = 0, key_max = 0;

      if ( c.hasAttr(_U(key)) )   {
        coll_key = c.attr<string>(_U(key));
      }
      if ( c.hasAttr(_U(key_value)) )   {
        key_max = key_min = c.key_value();
      }
      else if ( c.hasAttr(_U(key_min)) && c.hasAttr(_U(key_max)) )  {
        key_min = c.key_min();
        key_max = c.key_max();
      }
      else   {
        stringstream tree;
        XML::dump_tree(e,tree);
        throw_print("Reaout: Invalid specificatrion for multiple hit collections."+tree.str());
      }
      printout(s_debug_readout ? ALWAYS : DEBUG,"Compact",
               "++ Readout[%s]: Add hit collection %s [%s]  %d-%d",
               ro.name(), coll_name.c_str(), coll_key.c_str(), key_min, key_max);
      HitCollection hits(coll_name, coll_key, key_min, key_max);
      ro->hits.push_back(hits);
    }
  }
  lcdd.addReadout(ro);
}

/** Specialized converter for compact LimitSet objects.
 *
 *      <limitset name="....">
 *        <limit name="step_length_max" particles="*" value="5.0" unit="mm" />
 *  ... </limitset>
 */
template <> void Converter<LimitSet>::operator()(xml_h e) const {
  LimitSet ls(e.attr<string>(_U(name)));
  printout(s_debug_limits ? ALWAYS : DEBUG, "Compact",
           "++ Converting LimitSet structure: %s.",ls.name());
  for (xml_coll_t c(e, _U(limit)); c; ++c) {
    Limit limit;
    limit.particles = c.attr<string>(_U(particles));
    limit.name = c.attr<string>(_U(name));
    limit.content = c.attr<string>(_U(value));
    limit.unit = c.attr<string>(_U(unit));
    limit.value = _multiply<double>(limit.content, limit.unit);
    ls.addLimit(limit);
  }
  lcdd.addLimitSet(ls);
}

/** Specialized converter for generic LCDD properties
 *
 *      <properties>
 *        <attributes name="key" type="" .... />
 *  ... </properties>
 */
template <> void Converter<Property>::operator()(xml_h e) const {
  string name = e.attr<string>(_U(name));
  LCDD::Properties& prp = lcdd.properties();
  if (name.empty()) {
    throw_print("Failed to convert properties. No name given!");
  }
  vector<xml_attr_t>a = e.attributes();
  if (prp.find(name) == prp.end()) {
    prp.insert(make_pair(name, LCDD::PropertyValues()));
  }
  for (vector<xml_attr_t>::iterator i = a.begin(); i != a.end(); ++i) {
    pair<string, string>val(xml_tag_t(e.attr_name(*i)).str(), e.attr<string>(*i));
    prp[name].insert(val);
  }
}

/** Specialized converter for electric and magnetic fields
 *
 *  Uses internally a plugin to allow flexible field descriptions.
 *
 *     <field type="ConstantField" name="Myfield" field="electric">
 *       <strength x="0" y="0" z="5"/>
 *     </field>
 */
template <> void Converter<CartesianField>::operator()(xml_h e) const {
  string msg = "updated";
  string name = e.attr<string>(_U(name));
  string type = e.attr<string>(_U(type));
  CartesianField field = lcdd.field(name);
  if (!field.isValid()) {
    // The field is not present: We create it and add it to LCDD
    field = Ref_t(PluginService::Create<NamedObject*>(type, &lcdd, &e));
    if (!field.isValid()) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &lcdd, &e);
      throw_print("Failed to create field object of type " + type + ". " + dbg.missingFactory(type));
    }
    lcdd.addField(field);
    msg = "created";
  }
  type = field.type();
  // Now update the field structure with the generic part ie. set it's properties
  CartesianField::Properties& prp = field.properties();
  for (xml_coll_t c(e, _U(properties)); c; ++c) {
    string props_name = c.attr<string>(_U(name));
    vector<xml_attr_t>a = c.attributes();
    if (prp.find(props_name) == prp.end()) {
      prp.insert(make_pair(props_name, CartesianField::PropertyValues()));
    }
    for (vector<xml_attr_t>::iterator i = a.begin(); i != a.end(); ++i) {
      pair<string, string>val(xml_tag_t(c.attr_name(*i)).str(), c.attr<string>(*i));
      prp[props_name].insert(val);
    }
    if (c.hasAttr(_U(global)) && c.attr<bool>(_U(global))) {
      lcdd.field().properties() = prp;
    }
  }
  printout(INFO, "Compact", "++ Converted field: Successfully %s field %s [%s]", msg.c_str(), name.c_str(), type.c_str());
}

/** Update sensitive detectors from group tags.
 *
 *  Handle xml sections of the type:
 *  <sd name="MuonBarrel"
 *      type="Geant4Calorimeter"
 *      ecut="100.0*MeV"
 *      verbose="true"
 *      hit_aggregation="position"
 *      limits="limit-set-reference"
 *      region="region-name-reference">
 *  </sd>
 *
 */
template <> void Converter<SensitiveDetector>::operator()(xml_h element) const {
  string name = element.attr<string>(_U(name));
  try {
    SensitiveDetector sd = lcdd.sensitiveDetector(name);

    xml_attr_t type = element.attr_nothrow(_U(type));
    if (type) {
      sd.setType(element.attr<string>(type));
    }
    xml_attr_t verbose = element.attr_nothrow(_U(verbose));
    if (verbose) {
      sd.setVerbose(element.attr<bool>(verbose));
    }
    xml_attr_t combine = element.attr_nothrow(_U(combine_hits));
    if (combine) {
      sd.setCombineHits(element.attr<bool>(combine));
    }
    xml_attr_t limits = element.attr_nothrow(_U(limits));
    if (limits) {
      string l = element.attr<string>(limits);
      LimitSet ls = lcdd.limitSet(l);
      if (!ls.isValid()) {
        throw_print("Converter<SensitiveDetector>: Request for non-existing limitset:" + l);
      }
      sd.setLimitSet(ls);
    }
    xml_attr_t region = element.attr_nothrow(_U(region));
    if (region) {
      string r = element.attr<string>(region);
      Region reg = lcdd.region(r);
      if (!reg.isValid()) {
        throw_print("Converter<SensitiveDetector>: Request for non-existing region:" + r);
      }
      sd.setRegion(reg);
    }
    xml_attr_t hits = element.attr_nothrow(_U(hits_collection));
    if (hits) {
      sd.setHitsCollection(element.attr<string>(hits));
    }
    xml_attr_t ecut = element.attr_nothrow(_U(ecut));
    xml_attr_t eunit = element.attr_nothrow(_U(eunit));
    if (ecut && eunit) {
      double value = _multiply<double>(_toString(ecut), _toString(eunit));
      sd.setEnergyCutoff(value);
    }
    else if (ecut) {   // If no unit is given , we assume the correct Geant4 unit is used!
      sd.setEnergyCutoff(element.attr<double>(ecut));
    }
    printout(DEBUG, "Compact", "SensitiveDetector-update: %-18s %-24s Hits:%-24s Cutoff:%f7.3f", sd.name(),
             (" [" + sd.type() + "]").c_str(), sd.hitsCollection().c_str(), sd.energyCutoff());
    xml_attr_t sequence = element.attr_nothrow(_U(sequence));
    if (sequence) {
    }
  }
  catch (const exception& e) {
    printout(ERROR, "Compact", "++ FAILED    to convert sensitive detector: %s: %s", name.c_str(), e.what());
  }
  catch (...) {
    printout(ERROR, "Compact", "++ FAILED    to convert sensitive detector: %s: %s", name.c_str(), "UNKNONW Exception");
  }
}

static void setChildTitles(const pair<string, DetElement>& e) {
  DetElement parent = e.second.parent();
  const DetElement::Children& children = e.second.children();
  if (::strlen(e.second->GetTitle()) == 0) {
    e.second->SetTitle(parent.isValid() ? parent.type().c_str() : e.first.c_str());
  }
  for_each(children.begin(), children.end(), setChildTitles);
}

template <> void Converter<DetElement>::operator()(xml_h element) const {
  static const char* req_dets = ::getenv("REQUIRED_DETECTORS");
  static const char* req_typs = ::getenv("REQUIRED_DETECTOR_TYPES");
  static const char* ign_dets = ::getenv("IGNORED_DETECTORS");
  static const char* ign_typs = ::getenv("IGNORED_DETECTOR_TYPES");
  string type = element.attr<string>(_U(type));
  string name = element.attr<string>(_U(name));
  string name_match = ":" + name + ":";
  string type_match = ":" + type + ":";

  if (req_dets && !strstr(req_dets, name_match.c_str()))
    return;
  if (req_typs && !strstr(req_typs, type_match.c_str()))
    return;
  if (ign_dets && strstr(ign_dets, name_match.c_str()))
    return;
  if (ign_typs && strstr(ign_typs, type_match.c_str()))
    return;
  try {
    xml_attr_t attr_par = element.attr_nothrow(_U(parent));
    if (attr_par) {
      // We have here a nested detector. If the mother volume is not yet registered
      // it must be done here, so that the detector constructor gets the correct answer from
      // the call to LCDD::pickMotherVolume(DetElement).
      string par_name = element.attr<string>(attr_par);
      DetElement parent_detector = lcdd.detector(par_name);
      if ( !parent_detector.isValid() )  {
        except("Compact","Failed to access valid parent detector of %s",name.c_str());
      }
      Volume parent_volume = parent_detector.placement().volume();
      if ( !parent_volume.isValid() )   {
        except("Compact","Failed to access valid parent volume of %s from %s",
               name.c_str(), par_name.c_str());
      }
      lcdd.declareMotherVolume(name, parent_volume);
    }
    xml_attr_t attr_ro  = element.attr_nothrow(_U(readout));
    SensitiveDetector sd;
    Segmentation seg;
    if ( attr_ro )   {
      Readout ro = lcdd.readout(element.attr<string>(attr_ro));
      if (!ro.isValid()) {
        throw runtime_error("No Readout structure present for detector:" + name);
      }
      seg = ro.segmentation();
      sd = SensitiveDetector(name, "sensitive");
      sd.setHitsCollection(ro.name());
      sd.setReadout(ro);
      lcdd.addSensitiveDetector(sd);
    }
    Ref_t sens = sd;
    DetElement det(Ref_t(PluginService::Create<NamedObject*>(type, &lcdd, &element, &sens)));
    if (det.isValid()) {
      setChildTitles(make_pair(name, det));
      if ( sd.isValid() )  {
        det->flag |= DetElement::Object::HAVE_SENSITIVE_DETECTOR;
      }
      if ( seg.isValid() )  {
        seg->sensitive = sd;
        seg->detector  = det;
      }
    }
    printout(det.isValid() ? INFO : ERROR, "Compact", "%s subdetector:%s of type %s %s",
             (det.isValid() ? "++ Converted" : "FAILED    "), name.c_str(), type.c_str(),
             (sd.isValid() ? ("[" + sd.type() + "]").c_str() : ""));

    if (!det.isValid()) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &lcdd, &element, &sens);
      throw runtime_error("Failed to execute subdetector creation plugin. " + dbg.missingFactory(type));
    }
    lcdd.addDetector(det);
    return;
  }
  catch (const exception& e) {
    printout(ERROR, "Compact", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), e.what());
    terminate();
  }
  catch (...) {
    printout(ERROR, "Compact", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), "UNKNONW Exception");
    terminate();
  }
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<GdmlFile>::operator()(xml_h element) const   {
  XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h materials = doc.root();
  xml_coll_t(materials, _U(isotope)).for_each(Converter<Isotope>(this->lcdd,0,0));
  xml_coll_t(materials, _U(element)).for_each(Converter<Atom>(this->lcdd));
  xml_coll_t(materials, _U(material)).for_each(Converter<Material>(this->lcdd));
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<JsonFile>::operator()(xml_h element) const {
  string base = XML::DocumentHandler::system_directory(element);
  string file = element.attr<string>(_U(ref));
  vector<char*>  argv{&file[0],&base[0]};
  lcdd.apply("DD4hep_JsonProcessor",int(argv.size()), &argv[0]);
}

/// Read alignment entries from a seperate file in one of the include sections of the geometry
template <> void Converter<XMLFile>::operator()(xml_h element) const {
  this->lcdd.fromXML(element.attr<string>(_U(ref)));
}

/// Read alignment entries from a seperate file in one of the include sections of the geometry
template <> void Converter<AlignmentFile>::operator()(xml_h element) const {
  XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h alignments = doc.root();
  xml_coll_t(alignments, _U(alignment)).for_each(Converter<AlignmentEntry>(this->lcdd));
  xml_coll_t(alignments, _U(include)).for_each(Converter<XMLFile>(this->lcdd));
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<DetElementInclude>::operator()(xml_h element) const {
  string type = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("xml");
  if ( type == "xml" )  {
    XML::DocumentHolder doc(XML::DocumentHandler().load(element, element.attr_value(_U(ref))));
    xml_h node = doc.root();
    string tag = node.tag();
    if ( tag == "lccdd" )
      Converter<Compact>(this->lcdd)(node);
    else if ( tag == "define" )
      xml_coll_t(node, _U(constant)).for_each(Converter<Constant>(this->lcdd));
    else if ( tag == "readouts" )
      xml_coll_t(node, _U(readout)).for_each(Converter<Readout>(this->lcdd));
    else if ( tag == "regions" )
      xml_coll_t(node, _U(region)).for_each(Converter<Region>(this->lcdd));
    else if ( tag == "limitsets" )
      xml_coll_t(node, _U(limitset)).for_each(Converter<LimitSet>(this->lcdd));
    else if ( tag == "display" )
      xml_coll_t(node,_U(vis)).for_each(Converter<VisAttr>(this->lcdd));
    else if ( tag == "detector" )
      Converter<DetElement>(this->lcdd)(node);
    else if ( tag == "detectors" )
      xml_coll_t(node,_U(detector)).for_each(Converter<DetElement>(this->lcdd));
  }
  else if ( type == "json" )  {
    Converter<JsonFile>(this->lcdd)(element);
  }
  else if ( type == "gdml" )  {
    Converter<GdmlFile>(this->lcdd)(element);
  }
  else if ( type == "xml-extended" )  {
    Converter<XMLFile>(this->lcdd)(element);
  }
  else  {
    except("Compact","++ FAILED    Invalid file type:%s. This cannot be processed!",type.c_str());
  }
}

template <> void Converter<Compact>::operator()(xml_h element) const {
  static int num_calls = 0;
  char text[32];

  ++num_calls;
  xml_elt_t compact(element);
  bool steer_geometry = compact.hasChild(_U(geometry));
  bool open_geometry  = true;
  bool close_geometry = true;

  if (element.hasChild(_U(debug)))
    (Converter<Debug>(lcdd))(xml_h(compact.child(_U(debug))));
  
  if ( steer_geometry )   {
    xml_elt_t steer = compact.child(_U(geometry));
    if ( steer.hasAttr(_U(open))  ) open_geometry  = steer.attr<bool>(_U(open));
    if ( steer.hasAttr(_U(close)) ) close_geometry = steer.attr<bool>(_U(close));
    for (xml_coll_t clr(steer, _U(clear)); clr; ++clr) {
      string nam = clr.hasAttr(_U(name)) ? clr.attr<string>(_U(name)) : string();
      if ( nam.substr(0,6) == "elemen" )   {
        TGeoElementTable*	table = lcdd.manager().GetElementTable();
        table->TGeoElementTable::~TGeoElementTable();
        new(table) TGeoElementTable();
        // This will initialize the table without filling:
        table->AddElement("VACUUM","VACUUM"   ,0,   0, 0.0);
        printout(INFO,"Compact",
                 "++ Cleared default ROOT TGeoElementTable contents. "
                 "Must now be filled from XML!");
      }
    }
  }
  
  xml_coll_t(compact, _U(define)).for_each(_U(include), Converter<DetElementInclude>(lcdd));
  xml_coll_t(compact, _U(define)).for_each(_U(constant), Converter<Constant>(lcdd));
  xml_coll_t(compact, _U(includes)).for_each(_U(gdmlFile), Converter<GdmlFile>(lcdd));

  if (element.hasChild(_U(info)))
    (Converter<Header>(lcdd))(xml_h(compact.child(_U(info))));

  xml_coll_t(compact, _U(materials)).for_each(_U(element), Converter<Atom>(lcdd));
  xml_coll_t(compact, _U(materials)).for_each(_U(material), Converter<Material>(lcdd));
  xml_coll_t(compact, _U(properties)).for_each(_U(attributes), Converter<Property>(lcdd));
  if ( open_geometry ) lcdd.init();
  xml_coll_t(compact, _U(limits)).for_each(_U(limitset), Converter<LimitSet>(lcdd));
  xml_coll_t(compact, _U(display)).for_each(_U(include), Converter<DetElementInclude>(lcdd));
  xml_coll_t(compact, _U(display)).for_each(_U(vis), Converter<VisAttr>(lcdd));
  printout(DEBUG, "Compact", "++ Converting readout  structures...");
  xml_coll_t(compact, _U(readouts)).for_each(_U(readout), Converter<Readout>(lcdd));
  printout(DEBUG, "Compact", "++ Converting region   structures...");
  xml_coll_t(compact, _U(regions)).for_each(_U(region), Converter<Region>(lcdd));
  printout(DEBUG, "Compact", "++ Converting included files with subdetector structures...");
  xml_coll_t(compact, _U(detectors)).for_each(_U(include), Converter<DetElementInclude>(lcdd));
  printout(DEBUG, "Compact", "++ Converting detector structures...");
  xml_coll_t(compact, _U(detectors)).for_each(_U(detector), Converter<DetElement>(lcdd));
  xml_coll_t(compact, _U(include)).for_each(Converter<DetElementInclude>(this->lcdd));

  xml_coll_t(compact, _U(includes)).for_each(_U(alignment), Converter<AlignmentFile>(lcdd));
  xml_coll_t(compact, _U(includes)).for_each(_U(xml), Converter<XMLFile>(lcdd));
  xml_coll_t(compact, _U(alignments)).for_each(_U(alignment), Converter<AlignmentEntry>(lcdd));
  xml_coll_t(compact, _U(fields)).for_each(_U(field), Converter<CartesianField>(lcdd));
  xml_coll_t(compact, _U(sensitive_detectors)).for_each(_U(sd), Converter<SensitiveDetector>(lcdd));
  ::snprintf(text, sizeof(text), "%u", xml_h(element).checksum(0));
  lcdd.addConstant(Constant("compact_checksum", text));
  if ( --num_calls == 0 && close_geometry )  {
    lcdd.endDocument();
  }
  xml_coll_t(compact, _U(plugins)).for_each(_U(plugin), Converter<Plugin> (lcdd));
}

#ifdef _WIN32
  template Converter<Plugin>;
  template Converter<Constant>;
  template Converter<Material>;
  template Converter<Atom>;
  template Converter<VisAttr>;
  template Converter<AlignmentEntry>;
  template Converter<Region>;
  template Converter<Readout>;
  template Converter<Segmentation>;
  template Converter<LimitSet>;
  template Converter<Property>;
  template Converter<CartesianField>;
  template Converter<SensitiveDetector>;
  template Converter<DetElement>;
  template Converter<GdmlFile>;
  template Converter<XMLFile>;
  template Converter<AlignmentFile>;
  template Converter<Header>;
  template Converter<DetElementInclude>;
  template Converter<Compact>;

#endif
