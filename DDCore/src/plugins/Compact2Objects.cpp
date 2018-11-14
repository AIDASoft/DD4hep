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

// Framework includes
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/FieldTypes.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ObjectsInterna.h"

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
using namespace dd4hep;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class Debug;
  class World;
  class Isotope;
  class Plugin;
  class Compact;
  class Includes;
  class GdmlFile;
  class Property;
  class XMLFile;
  class JsonFile;
  class Parallelworld_Volume;
  class DetElementInclude;

  /// Converter instances implemented in this compilation unit
  template <> void Converter<Debug>::operator()(xml_h element) const;
  template <> void Converter<World>::operator()(xml_h element) const;
  template <> void Converter<Plugin>::operator()(xml_h element) const;
  template <> void Converter<Constant>::operator()(xml_h element) const;
  template <> void Converter<Material>::operator()(xml_h element) const;
  template <> void Converter<Atom>::operator()(xml_h element) const;
  template <> void Converter<Isotope>::operator()(xml_h element) const;
  template <> void Converter<VisAttr>::operator()(xml_h element) const;
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
  template <> void Converter<Header>::operator()(xml_h element) const;
  template <> void Converter<DetElementInclude>::operator()(xml_h element) const;
  template <> void Converter<Parallelworld_Volume>::operator()(xml_h element) const;
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
  bool s_debug_constants    = false;
  bool s_debug_include      = false;
}

static Ref_t create_ConstantField(Detector& /* description */, xml_h e) {
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

static Ref_t create_SolenoidField(Detector& description, xml_h e) {
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
    Box box = description.worldVolume().solid();
    ptr->innerRadius = c.attr<double>(_U(inner_radius));
    ptr->outerRadius = box.x();
  }
  else if (has_outer_radius) {
    Box box = description.worldVolume().solid();
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
    ptr->maxZ = description.constant<double>("world_side");
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

static Ref_t create_DipoleField(Detector& /* description */, xml_h e) {
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

static Ref_t create_MultipoleField(Detector& description, xml_h e) {
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
    ptr->volume = xml::createShape(description, type, child);
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

static long load_Compact(Detector& description, xml_h element) {
  Converter<Compact>converter(description);
  converter(element);
  return 1;
}
DECLARE_XML_DOC_READER(lccdd,load_Compact)
DECLARE_XML_DOC_READER(compact,load_Compact)

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
    else if ( nam.substr(0,6) == "consta" ) s_debug_constants    = (0 != val);
    else if ( nam.substr(0,6) == "includ" ) s_debug_include      = (0 != val);
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
  string type = "default";
  xml_attr_t typ_attr = e.attr_nothrow(_U(type));
  if ( typ_attr )   {
    type = e.attr<string>(_U(type));
  }
  if ( type == "default" )  {
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
    description.apply(name.c_str(),int(argv.size()), &argv[0]);
    return;
  }
  // Call a custom plugin taking the xml element as an argument
  long result = PluginService::Create<long>(name, &description, &e);
  if (0 == result) {
    PluginDebug dbg;
    result = PluginService::Create<long>(name, &description, &e);
    if ( 0 == result )  {
      except("Compact","++ Failed to locate plugin %s - no factory: %s",
             name.c_str(), dbg.missingFactory(name).c_str());
    }
  }
  result = *(long*) result;
  if (result != 1) {
    except("Compact","++ Failed to execute plugin %s", name.c_str());
  }
}

/** Convert compact constant objects (defines)
 *
 *
 */
template <> void Converter<Constant>::operator()(xml_h e) const {
  if ( e.tag() != "include" )   {
    xml_ref_t constant(e);
    string nam = constant.attr<string>(_U(name));
    string val = constant.attr<string>(_U(value));
    string typ = constant.hasAttr(_U(type)) ? constant.attr<string>(_U(type)) : "number";
    Constant c(nam, val, typ);
    _toDictionary(nam, val, typ);
    description.addConstant(c);
    if ( s_debug_constants )   {
      printout(ALWAYS, "Compact",
               "++ Converting constant %-16s = %-32s [%s]", nam.c_str(), val.c_str(), typ.c_str());
    }
    return;
  }
  xml::DocumentHolder doc(xml::DocumentHandler().load(e, e.attr_value(_U(ref))));
  if ( s_debug_include )   {
    printout(ALWAYS, "Compact","++ Processing xml document %s.",doc.uri().c_str());
  }
  xml_h root = doc.root();
  xml_coll_t(root, _U(define)).for_each(_U(constant), Converter<Constant>(description));
  xml_coll_t(root, _U(constant)).for_each(Converter<Constant>(description));
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
  description.setHeader(h);
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
  xml_ref_t         x_mat(e);
  TGeoManager&      mgr = description.manager();
  xml_tag_t         mname = x_mat.name();
  const char*       matname = mname.c_str();
  TGeoElementTable* table = mgr.GetElementTable();
  TGeoMaterial*     mat = mgr.GetMaterial(matname);
  TGeoMixture*      mix = dynamic_cast<TGeoMixture*>(mat);
  xml_coll_t        fractions(x_mat, _U(fraction));
  xml_coll_t        composites(x_mat, _U(composite));

  if (0 == mat) {
    TGeoMaterial* comp_mat;
    TGeoElement*  comp_elt;
    xml_h  density     = x_mat.child(_U(D), false);
    double dens_val    = density.ptr() ? density.attr<double>(_U(value)) : 0.0;
    double dens_unit   = 1.0;

    if ( !density.ptr() ) {
      throw_print("Compact2Objects[ERROR]: material without density tag ( <D  unit=\"g/cm3\" value=\"..\"/> ) provided: "
                  + string( matname ) ) ;
    }
    if ( density.hasAttr(_U(unit)) )   {
      dens_unit = density.attr<double>(_U(unit))/xml::_toDouble(_Unicode(gram/cm3));
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
    cout << "Gev    " << xml::_toDouble(_Unicode(GeV)) << endl;
    cout << "sec    " << xml::_toDouble(_Unicode(second)) << endl;
    cout << "nsec   " << xml::_toDouble(_Unicode(nanosecond)) << endl;
    cout << "kilo   " << xml::_toDouble(_Unicode(kilogram)) << endl;
    cout << "kilo   " << xml::_toDouble(_Unicode(joule*s*s/(m*m))) << endl;
    cout << "meter  " << xml::_toDouble(_Unicode(meter)) << endl;
    cout << "ampere " << xml::_toDouble(_Unicode(ampere)) << endl;
    cout << "degree " << xml::_toDouble(_Unicode(degree)) << endl;
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,12,0)
    mix->ComputeDerivedQuantities();
#endif
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
  if (x_mat.hasAttr(_U(formula))) {
    string form = x_mat.attr<string>(_U(formula));
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
  TGeoManager&      mgr  = description.manager();
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
    printout(s_debug_isotopes ? WARNING : DEBUG, "Compact",
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
  TGeoManager&      mgr  = description.manager();
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
    printout(s_debug_elements ? WARNING : DEBUG, "Compact",
             "++ Element %-16s  Z:%3d N:%3d A:%8.4f [g/mol] ALREADY defined. [Ignore definition]",
             elt->GetName(), elt->Z(), elt->N(), elt->A());
  }
}

/** Convert compact visualization attribute to Detector visualization attribute
 *
 *  <vis name="SiVertexBarrelModuleVis"
 *       alpha="1.0" r="1.0" g="0.75" b="0.76"
 *       drawingStyle="wireframe"
 *       showDaughters="false"
 *       visible="true"/>
 */
template <> void Converter<VisAttr>::operator()(xml_h e) const {
  VisAttr attr(e.attr<string>(_U(name)));
  float red   = e.hasAttr(_U(r)) ? e.attr<float>(_U(r)) : 1.0f;
  float green = e.hasAttr(_U(g)) ? e.attr<float>(_U(g)) : 1.0f;
  float blue  = e.hasAttr(_U(b)) ? e.attr<float>(_U(b)) : 1.0f;

  printout(s_debug_visattr ? ALWAYS : DEBUG, "Compact",
           "++ Converting VisAttr  structure: %-16s. R=%.3f G=%.3f B=%.3f",
           attr.name(), red, green, blue);
  attr.setColor(red, green, blue);
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
  description.addVisAttribute(attr);
}

/** Specialized converter for compact region objects.
 *
 */
template <> void Converter<Region>::operator()(xml_h elt) const {
  xml_dim_t       e = elt;
  Region          region(e.nameStr());
  vector<string>& limits       = region.limits();
  xml_attr_t      cut          = elt.attr_nothrow(_U(cut));
  xml_attr_t      threshold    = elt.attr_nothrow(_U(threshold));
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
  description.addRegion(region);
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

  const BitFieldCoder* bitfield = &opt->second->decoder;
  Segmentation segment(type, name, bitfield);
  if ( segment.isValid() ) {
    const DDSegmentation::Parameters& pars = segment.parameters();
    printout(s_debug_segmentation ? ALWAYS : DEBUG, "Compact",
             "++ Converting segmentation structure: %s of type %s.",name.c_str(),type.c_str());
    for(const auto p : pars )  {
      xml::Strng_t pNam(p->name());
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
          for (const string& spar : elts )  {
            if ( spar.empty() ) continue;
            valueVector.push_back(_toDouble(spar));
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
      Converter<Segmentation> sub_conv(description,param,&sub_object);
      sub_conv(sub);
      if ( sub_object.first.isValid() )  {
        Segmentation sub_seg = sub_object.first;
        xml_dim_t x_seg(sub);
        if ( sub.hasAttr(_U(key_value)) ) {
          key_min = key_max = x_seg.key_value();
        }
        else if ( sub.hasAttr(_U(key_min)) && sub.hasAttr(_U(key_max)) )  {
          key_min = x_seg.key_min();
          key_max = x_seg.key_max();
        }
        else  {
          stringstream tree;
          xml::dump_tree(sub,tree);
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
    opt.second = IDDescriptor(name,id.text());
    description.addIDSpecification(opt.second);
  }
  if (seg) {   // Segmentation is not mandatory!
    Converter<Segmentation>(description,param,&opt)(seg);
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
        xml::dump_tree(e,tree);
        throw_print("Reaout: Invalid specificatrion for multiple hit collections."+tree.str());
      }
      printout(s_debug_readout ? ALWAYS : DEBUG,"Compact",
               "++ Readout[%s]: Add hit collection %s [%s]  %d-%d",
               ro.name(), coll_name.c_str(), coll_key.c_str(), key_min, key_max);
      HitCollection hits(coll_name, coll_key, key_min, key_max);
      ro->hits.push_back(hits);
    }
  }
  description.addReadout(ro);
}

/** Specialized converter for compact LimitSet objects.
 *
 *      <limitset name="....">
 *        <limit name="step_length_max" particles="*" value="5.0" unit="mm" />
 *  ... </limitset>
 */
template <> void Converter<LimitSet>::operator()(xml_h e) const {
  Limit limit;
  LimitSet ls(e.attr<string>(_U(name)));
  printout(s_debug_limits ? ALWAYS : DEBUG, "Compact",
           "++ Converting LimitSet structure: %s.",ls.name());
  for (xml_coll_t c(e, _U(limit)); c; ++c) {
    limit.name      = c.attr<string>(_U(name));
    limit.particles = c.attr<string>(_U(particles));
    limit.content   = c.attr<string>(_U(value));
    limit.unit      = c.attr<string>(_U(unit));
    limit.value     = _multiply<double>(limit.content, limit.unit);
    ls.addLimit(limit);
  }
  limit.name      = "cut";
  for (xml_coll_t c(e, _U(cut)); c; ++c) {
    limit.particles = c.attr<string>(_U(particles));
    limit.content   = c.attr<string>(_U(value));
    limit.unit      = c.attr<string>(_U(unit));
    limit.value     = _multiply<double>(limit.content, limit.unit);
    ls.addCut(limit);
  }
  description.addLimitSet(ls);
}

/** Specialized converter for generic Detector properties
 *
 *      <properties>
 *        <attributes name="key" type="" .... />
 *  ... </properties>
 */
template <> void Converter<Property>::operator()(xml_h e) const {
  string            name = e.attr<string>(_U(name));
  Detector::Properties& prp  = description.properties();
  if ( name.empty() )
    throw_print("Failed to convert properties. No name given!");

  vector<xml_attr_t> a = e.attributes();
  if ( prp.find(name) == prp.end() )
    prp.insert(make_pair(name, Detector::PropertyValues()));

  for (xml_attr_t i : a )
    prp[name].insert(make_pair(xml_tag_t(e.attr_name(i)).str(),e.attr<string>(i)));
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
  string msg  = "updated";
  string name = e.attr<string>(_U(name));
  string type = e.attr<string>(_U(type));
  CartesianField field = description.field(name);
  if ( !field.isValid() ) {
    // The field is not present: We create it and add it to Detector
    field = Ref_t(PluginService::Create<NamedObject*>(type, &description, &e));
    if ( !field.isValid() ) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &description, &e);
      throw_print("Failed to create field object of type "+type + ". "+dbg.missingFactory(type));
    }
    description.addField(field);
    msg = "created";
  }
  type = field.type();
  // Now update the field structure with the generic part ie. set it's properties
  CartesianField::Properties& prp = field.properties();
  for ( xml_coll_t c(e, _U(properties)); c; ++c ) {
    string props_name = c.attr<string>(_U(name));
    vector<xml_attr_t>a = c.attributes();
    if ( prp.find(props_name) == prp.end() ) {
      prp.insert(make_pair(props_name, Detector::PropertyValues()));
    }
    for ( xml_attr_t i : a )
      prp[props_name].insert(make_pair(xml_tag_t(c.attr_name(i)).str(), c.attr<string>(i)));

    if (c.hasAttr(_U(global)) && c.attr<bool>(_U(global))) {
      description.field().properties() = prp;
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
    SensitiveDetector sd = description.sensitiveDetector(name);
    xml_attr_t type = element.attr_nothrow(_U(type));
    if ( type )  {
      sd.setType(element.attr<string>(type));
    }
    xml_attr_t verbose = element.attr_nothrow(_U(verbose));
    if ( verbose ) {
      sd.setVerbose(element.attr<bool>(verbose));
    }
    xml_attr_t combine = element.attr_nothrow(_U(combine_hits));
    if ( combine ) {
      sd.setCombineHits(element.attr<bool>(combine));
    }
    xml_attr_t limits = element.attr_nothrow(_U(limits));
    if ( limits ) {
      string l = element.attr<string>(limits);
      LimitSet ls = description.limitSet(l);
      if (!ls.isValid()) {
        throw_print("Converter<SensitiveDetector>: Request for non-existing limitset:" + l);
      }
      sd.setLimitSet(ls);
    }
    xml_attr_t region = element.attr_nothrow(_U(region));
    if ( region ) {
      string r = element.attr<string>(region);
      Region reg = description.region(r);
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
  xml_attr_t attr_ignore = element.attr_nothrow(_U(ignore));
  if ( attr_ignore )   {
    bool ignore_det = element.attr<bool>(_U(ignore));
    if ( ignore_det )  {
      printout(INFO, "Compact",
               "+++ Do not build subdetector:%s [ignore flag set]",
               name.c_str());
      return;
    }
  }
  try {
    string par_name;
    xml_attr_t attr_par = element.attr_nothrow(_U(parent));
    xml_elt_t  elt_par(0);
    if (attr_par)
      par_name = element.attr<string>(attr_par);
    else if ( (elt_par=element.child(_U(parent),false)) )
      par_name = elt_par.attr<string>(_U(name));
    if ( !par_name.empty() ) {
      // We have here a nested detector. If the mother volume is not yet registered
      // it must be done here, so that the detector constructor gets the correct answer from
      // the call to Detector::pickMotherVolume(DetElement).
      if ( par_name[0] == '$' ) par_name = xml::getEnviron(par_name);
      DetElement parent = description.detector(par_name);
      if ( !parent.isValid() )  {
        except("Compact","Failed to access valid parent detector of %s",name.c_str());
      }
      description.declareParent(name, parent);
    }
    xml_attr_t attr_ro  = element.attr_nothrow(_U(readout));
    SensitiveDetector sd;
    Segmentation seg;
    if ( attr_ro )   {
      Readout ro = description.readout(element.attr<string>(attr_ro));
      if (!ro.isValid()) {
        throw runtime_error("No Readout structure present for detector:" + name);
      }
      seg = ro.segmentation();
      sd = SensitiveDetector(name, "sensitive");
      sd.setHitsCollection(ro.name());
      sd.setReadout(ro);
      description.addSensitiveDetector(sd);
    }
    Ref_t sens = sd;
    DetElement det(Ref_t(PluginService::Create<NamedObject*>(type, &description, &element, &sens)));
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
      PluginService::Create<NamedObject*>(type, &description, &element, &sens);
      throw runtime_error("Failed to execute subdetector creation plugin. " + dbg.missingFactory(type));
    }
    description.addDetector(det);
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
  xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h root = doc.root();
  if ( s_debug_include )   {
    printout(ALWAYS, "Compact","++ Processing xml document %s.",doc.uri().c_str());
  }
  if ( root.tag() == "materials" || root.tag() == "elements" )   {
    xml_coll_t(root, _U(isotope)).for_each(Converter<Isotope>(this->description,0,0));
    xml_coll_t(root, _U(element)).for_each(Converter<Atom>(this->description));
    xml_coll_t(root, _U(material)).for_each(Converter<Material>(this->description));
    return;
  }
  this->description.fromXML(doc.uri());
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<JsonFile>::operator()(xml_h element) const {
  string base = xml::DocumentHandler::system_directory(element);
  string file = element.attr<string>(_U(ref));
  vector<char*>  argv{&file[0],&base[0]};
  description.apply("DD4hep_JsonProcessor",int(argv.size()), &argv[0]);
}

/// Read alignment entries from a seperate file in one of the include sections of the geometry
template <> void Converter<XMLFile>::operator()(xml_h element) const {
  this->description.fromXML(element.attr<string>(_U(ref)));
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<World>::operator()(xml_h element) const {
  xml_det_t  world(element);
  xml_comp_t shape = world.child(_U(shape));
  VisAttr    vis = description.visAttributes("WorldVis");
  Material   mat = world.hasAttr(_U(material))
    ? description.material(world.attr<string>(_U(material))) : description.air();

  if ( !vis.isValid() ) vis = description.invisible();
  /// Create the shape and the corresponding volume
  Solid    sol(shape.createShape());
  Volume   vol("world_volume", sol, mat);
  vol.setVisAttributes(vis);
  description.manager().SetTopVolume(vol.ptr());
  printout(INFO, "Compact", "++ Converted successfully world %s. vis:%s material:%s.",
           vol.name(), vis.name(), mat.name());
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<Parallelworld_Volume>::operator()(xml_h element) const {
  xml_det_t    parallel(element);
  xml_comp_t   shape  = parallel.child(_U(shape));
  xml_dim_t    pos    = element.child(_U(position),false);
  xml_dim_t    rot    = element.child(_U(rotation),false);
  string       name   = element.attr<string>(_U(name));
  string       path   = element.attr<string>(_U(anchor));
  bool         conn   = element.attr<bool>(_U(connected),false);
  DetElement   anchor(detail::tools::findElement(description, path));
  Position     position = pos ? Position(pos.x(), pos.y(), pos.z())    : Position();
  RotationZYX  rotation = rot ? RotationZYX(rot.z(), rot.y(), rot.x()) : RotationZYX();

  Material mat = parallel.hasAttr(_U(material))
    ? description.material(parallel.attr<string>(_U(material)))
    : description.air();
  VisAttr vis = parallel.hasAttr(_U(vis))
    ? description.invisible()
    : description.visAttributes(parallel.visStr());

  if ( !anchor.isValid() )   {
    except("Parallelworld_Volume",
           "++ FAILED    Cannot identify the anchor of the tracking volume: '%s'",
           path.c_str());
  }

  /// Create the shape and the corresponding volume
  Transform3D  tr_volume(detail::matrix::_transform(anchor.nominal().worldTransformation().Inverse()));
  Solid        sol(shape.createShape());
  Volume       vol(name, sol, mat);
  Volume       par = conn ? description.worldVolume() : description.parallelWorldVolume();
  PlacedVolume pv;

  /// In case the volume is connected, we may use visualization
  vol.setVisAttributes(vis);
  /// Need to inhibit that this artifical volume gets translated to Geant4 (connected only)!
  vol.setFlagBit(Volume::VETO_SIMU);
  
  /// Now place the volume in the anchor frame
  Transform3D trafo = tr_volume * Transform3D(rotation,position); // Is this the correct order ?
  pv = par.placeVolume(vol, trafo);
  if ( !pv.isValid() )   {
    except("Parallelworld_Volume",
           "++ FAILED    to place the tracking volume inside the anchor '%s'",path.c_str());
  }
  if ( name == "tracking_volume" )   {
    description.setTrackingVolume(vol);
  }
  printout(INFO, "Compact", "++ Converted successfully parallelworld_volume %s. anchor: %s vis:%s.",
           vol.name(), anchor.path().c_str(), vis.name());
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<DetElementInclude>::operator()(xml_h element) const {
  string type = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("xml");
  if ( type == "xml" )  {
    xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
    if ( s_debug_include )   {
      printout(ALWAYS, "Compact","++ Processing xml document %s.",doc.uri().c_str());
    }
    xml_h node = doc.root();
    string tag = node.tag();
    if ( tag == "lccdd" )
      Converter<Compact>(this->description)(node);
    else if ( tag == "define" )
      xml_coll_t(node, _U(constant)).for_each(Converter<Constant>(this->description));
    else if ( tag == "readouts" )
      xml_coll_t(node, _U(readout)).for_each(Converter<Readout>(this->description));
    else if ( tag == "regions" )
      xml_coll_t(node, _U(region)).for_each(Converter<Region>(this->description));
    else if ( tag == "limitsets" )
      xml_coll_t(node, _U(limitset)).for_each(Converter<LimitSet>(this->description));
    else if ( tag == "display" )
      xml_coll_t(node,_U(vis)).for_each(Converter<VisAttr>(this->description));
    else if ( tag == "detector" )
      Converter<DetElement>(this->description)(node);
    else if ( tag == "detectors" )
      xml_coll_t(node,_U(detector)).for_each(Converter<DetElement>(this->description));
  }
  else if ( type == "json" )  {
    Converter<JsonFile>(this->description)(element);
  }
  else if ( type == "gdml" )  {
    Converter<GdmlFile>(this->description)(element);
  }
  else if ( type == "xml-extended" )  {
    Converter<XMLFile>(this->description)(element);
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
  bool close_document = true;
  bool close_geometry = true;

  if (element.hasChild(_U(debug)))
    (Converter<Debug>(description))(xml_h(compact.child(_U(debug))));

  if ( steer_geometry )   {
    xml_elt_t steer = compact.child(_U(geometry));
    if ( steer.hasAttr(_U(open))  )
      open_geometry  = steer.attr<bool>(_U(open));
    if ( steer.hasAttr(_U(close)) )
      close_document = steer.attr<bool>(_U(close));
    if ( steer.hasAttr(_U(close_geometry)) )
      close_geometry = steer.attr<bool>(_U(close_geometry));
    for (xml_coll_t clr(steer, _U(clear)); clr; ++clr) {
      string nam = clr.hasAttr(_U(name)) ? clr.attr<string>(_U(name)) : string();
      if ( nam.substr(0,6) == "elemen" )   {
        TGeoElementTable*	table = description.manager().GetElementTable();
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
  
  xml_coll_t(compact, _U(define)).for_each(_U(include), Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(define)).for_each(_U(constant), Converter<Constant>(description));
  xml_coll_t(compact, _U(includes)).for_each(_U(gdmlFile), Converter<GdmlFile>(description));

  if (element.hasChild(_U(info)))
    (Converter<Header>(description))(xml_h(compact.child(_U(info))));

  xml_coll_t(compact, _U(materials)).for_each(_U(element), Converter<Atom>(description));
  xml_coll_t(compact, _U(materials)).for_each(_U(material), Converter<Material>(description));
  xml_coll_t(compact, _U(properties)).for_each(_U(attributes), Converter<Property>(description));
  xml_coll_t(compact, _U(display)).for_each(_U(include), Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(display)).for_each(_U(vis), Converter<VisAttr>(description));

  if (element.hasChild(_U(world)))
    (Converter<World>(description))(xml_h(compact.child(_U(world))));

  if ( open_geometry ) description.init();
  xml_coll_t(compact, _U(limits)).for_each(_U(limitset), Converter<LimitSet>(description));

  printout(DEBUG, "Compact", "++ Converting readout  structures...");
  xml_coll_t(compact, _U(readouts)).for_each(_U(readout), Converter<Readout>(description));
  printout(DEBUG, "Compact", "++ Converting region   structures...");
  xml_coll_t(compact, _U(regions)).for_each(_U(region), Converter<Region>(description));
  printout(DEBUG, "Compact", "++ Converting included files with subdetector structures...");
  xml_coll_t(compact, _U(detectors)).for_each(_U(include), Converter<DetElementInclude>(description));
  printout(DEBUG, "Compact", "++ Converting detector structures...");
  xml_coll_t(compact, _U(detectors)).for_each(_U(detector), Converter<DetElement>(description));
  xml_coll_t(compact, _U(include)).for_each(Converter<DetElementInclude>(this->description));

  xml_coll_t(compact, _U(includes)).for_each(_U(xml), Converter<XMLFile>(description));
  xml_coll_t(compact, _U(fields)).for_each(_U(field), Converter<CartesianField>(description));
  xml_coll_t(compact, _U(sensitive_detectors)).for_each(_U(sd), Converter<SensitiveDetector>(description));
  xml_coll_t(compact, _U(parallelworld_volume)).for_each(Converter<Parallelworld_Volume>(description));

  if ( --num_calls == 0 && close_document )  {
    ::snprintf(text, sizeof(text), "%u", xml_h(element).checksum(0));
    description.addConstant(Constant("compact_checksum", text));
    description.endDocument(close_geometry);
  }
  xml_coll_t(compact, _U(plugins)).for_each(_U(plugin), Converter<Plugin> (description));
}

#ifdef _WIN32
  template Converter<Plugin>;
  template Converter<Constant>;
  template Converter<Material>;
  template Converter<Atom>;
  template Converter<VisAttr>;
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
  template Converter<Header>;
  template Converter<DetElementInclude>;
  template Converter<Compact>;

#endif
