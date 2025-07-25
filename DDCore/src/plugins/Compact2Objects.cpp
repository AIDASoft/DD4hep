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
//
// Main conversion operations for the compact notation.
// - Create elements, materials, etc.
// - Calls detector construction factories.
//
//==========================================================================
//
// Framework includes
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/PropertyTable.h>
#include <DD4hep/OpticalSurfaces.h>
#include <DD4hep/OpticalSurfaceManager.h>
#include <DD4hep/IDDescriptor.h>
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/FieldTypes.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Path.h>
#include <DD4hep/Plugins.h>
#include <DD4hep/detail/SegmentationsInterna.h>
#include <DD4hep/detail/DetectorInterna.h>
#include <DD4hep/detail/ObjectsInterna.h>

#include <XML/DocumentHandler.h>
#include <XML/Utilities.h>

// Root/TGeo include files
#include <TGeoManager.h>
#include <TGeoMaterial.h>
#include <TGeoPhysicalConstants.h>
#include <TGDMLMatrix.h>
#include <TMath.h>

// C/C++ include files
#include <filesystem>
#include <iostream>
#include <climits>
#include <set>

using namespace dd4hep;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class Debug;
  class World;
  class Isotope;
  class Plugin;
  class Compact;
  class Includes;
  class IncludeFile;
  class Property;
  class XMLFile;
  class JsonFile;
  class PropertyConstant;
  class Parallelworld_Volume;
  class DetElementInclude;
  class STD_Conditions;
  
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
  template <> void Converter<OpticalSurface>::operator()(xml_h element) const;
  template <> void Converter<PropertyTable>::operator()(xml_h element) const;
  template <> void Converter<PropertyConstant>::operator()(xml_h element) const;
  template <> void Converter<DetElement>::operator()(xml_h element) const;
  template <> void Converter<STD_Conditions>::operator()(xml_h element) const;
  template <> void Converter<IncludeFile>::operator()(xml_h element) const;
  template <> void Converter<JsonFile>::operator()(xml_h element) const;
  template <> void Converter<XMLFile>::operator()(xml_h element) const;
  template <> void Converter<Header>::operator()(xml_h element) const;
  template <> void Converter<DetElementInclude>::operator()(xml_h element) const;
  template <> void Converter<Parallelworld_Volume>::operator()(xml_h element) const;
  template <> void Converter<Compact>::operator()(xml_h element) const;
}

namespace {
  static UInt_t unique_mat_id = 0xAFFEFEED;
  void throw_print(const std::string& msg) {
    printout(ERROR, "Compact", msg.c_str());
    throw std::runtime_error(msg);
  }
  class DebugOptions  {
  public:
    bool readout      = false;
    bool regions      = false;
    bool limits       = false;
    bool visattr      = false;
    bool isotopes     = false;
    bool elements     = false;
    bool materials    = false;
    bool segmentation = false;
    bool constants    = false;
    bool includes     = false;
    bool matrix       = false;
    bool surface      = false;
    bool detelements  = false;
    bool include_guard= true;
  } s_debug;
}

static Ref_t create_ConstantField(Detector& /* description */, xml_h e) {
  CartesianField obj;
  xml_comp_t field(e), strength(e.child(_U(strength)));
  std::string t = e.attr<std::string>(_U(field));
  ConstantField* ptr = new ConstantField();
  ptr->field_type = ::toupper(t[0]) == 'E' ? CartesianField::ELECTRIC : CartesianField::MAGNETIC;
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
  // This logic is a bit weird, but has its origin in the compact syntax:
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
  ptr->field_type = CartesianField::MAGNETIC;
  obj.assign(ptr, c.nameStr(), c.typeStr());
  return obj;
}
DECLARE_XMLELEMENT(SolenoidMagnet,create_SolenoidField)
// This is the plugin required for slic: note the different name
DECLARE_XMLELEMENT(solenoid,create_SolenoidField)

static Ref_t create_DipoleField(Detector& /* description */, xml_h e) {
  xml_comp_t c(e);
  CartesianField obj;
  DipoleField*   ptr   = new DipoleField();
  double         lunit = c.hasAttr(_U(lunit)) ? c.attr<double>(_U(lunit)) : 1.0;
  double         funit = c.hasAttr(_U(funit)) ? c.attr<double>(_U(funit)) : 1.0;
  double         val, mult = funit;

  if (c.hasAttr(_U(zmin)))
    ptr->zmin = _multiply<double>(c.attr<std::string>(_U(zmin)), lunit);
  if (c.hasAttr(_U(zmax)))
    ptr->zmax = _multiply<double>(c.attr<std::string>(_U(zmax)), lunit);
  if (c.hasAttr(_U(rmax)))
    ptr->rmax = _multiply<double>(c.attr<std::string>(_U(rmax)), lunit);
  for (xml_coll_t coll(c, _U(dipole_coeff)); coll; ++coll, mult /= lunit) {
    xml_dim_t coeff = coll;
    if ( coeff.hasAttr(_U(value)) )
      val = coll.attr<double>(_U(value)) * mult;
    else if ( coeff.hasAttr(_U(coefficient)) )
      val = coeff.coefficient() * mult;
    else
      val = _multiply<double>(coll.text(), mult);
    ptr->coefficents.emplace_back(val);
  }
  ptr->field_type = CartesianField::MAGNETIC;
  obj.assign(ptr, c.nameStr(), c.typeStr());
  return obj;
}
DECLARE_XMLELEMENT(DipoleMagnet,create_DipoleField)

static Ref_t create_MultipoleField(Detector& description, xml_h e) {
  xml_dim_t c(e), child;
  CartesianField  obj;
  MultipoleField* ptr = new MultipoleField();
  double          lunit = c.hasAttr(_U(lunit)) ? c.attr<double>(_U(lunit)) : 1.0;
  double          funit = c.hasAttr(_U(funit)) ? c.attr<double>(_U(funit)) : 1.0;
  double          val, mult = funit, bz = 0.0;
  RotationZYX     rot;
  Position        pos;

  if (c.hasAttr(_U(Z))) bz = c.Z() * funit;
  if ((child = c.child(_U(position), false))) {   // Position is not mandatory!
    pos.SetXYZ(child.x(), child.y(), child.z());
  }
  if ((child = c.child(_U(rotation), false))) {   // Rotation is not mandatory
    rot.SetComponents(child.z(), child.y(), child.x());
  }
  if ((child = c.child(_U(shape), false))) {      // Shape is not mandatory
    std::string type = child.typeStr();
    ptr->volume = xml::createShape(description, type, child);
  }
  ptr->B_z = bz;
  ptr->transform = Transform3D(rot,pos);
  for (xml_coll_t coll(c, _U(coefficient)); coll; ++coll, mult /= lunit) {
    xml_dim_t coeff = coll;
    if ( coll.hasAttr(_U(value)) )
      val = coll.attr<double>(_U(value)) * mult;
    else
      val = coeff.coefficient(0.0) * mult;
    ptr->coefficents.emplace_back(val);
    val = coeff.skew(0.0) * mult;
    ptr->skews.emplace_back(val);
  }
  ptr->field_type = CartesianField::MAGNETIC;
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

// We create out own type to avoid a class over the extension types
// attached to the Detector as a set of std::string is common.
class ProcessedFilesSet: public std::set<std::string> {};

/// Check whether a XML file was already processed
bool check_process_file(Detector& description, std::string filename) {

  // In order to have a global compact that is kept across plugin invocations
  // we add it as an extension to the the detector description.
  auto already_processed = description.extension<ProcessedFilesSet>( false );
  if ( !already_processed ) {
    already_processed = new ProcessedFilesSet( );
    description.addExtension<ProcessedFilesSet>(already_processed );
  }
  std::string npath = dd4hep::Path{filename}.normalize();
  if (already_processed->find(npath) != already_processed->end() ) {
    printout(INFO, "Compact","++ Already processed xml document %s.", npath.c_str());
    return true;
  }
  already_processed->insert(npath);
  return false;
}

/** Convert parser debug flags.
 */
template <> void Converter<Debug>::operator()(xml_h e) const {
  for (xml_coll_t coll(e, _U(type)); coll; ++coll) {
    int    val = coll.attr<int>(_U(value));
    std::string nam = coll.attr<std::string>(_U(name));
    if      ( nam.substr(0,6) == "isotop" ) s_debug.isotopes     = (0 != val);
    else if ( nam.substr(0,6) == "elemen" ) s_debug.elements     = (0 != val);
    else if ( nam.substr(0,6) == "materi" ) s_debug.materials    = (0 != val);
    else if ( nam.substr(0,6) == "visatt" ) s_debug.visattr      = (0 != val);
    else if ( nam.substr(0,6) == "region" ) s_debug.regions      = (0 != val);
    else if ( nam.substr(0,6) == "readou" ) s_debug.readout      = (0 != val);
    else if ( nam.substr(0,6) == "limits" ) s_debug.limits       = (0 != val);
    else if ( nam.substr(0,6) == "segmen" ) s_debug.segmentation = (0 != val);
    else if ( nam.substr(0,6) == "consta" ) s_debug.constants    = (0 != val);
    else if ( nam.substr(0,6) == "define" ) s_debug.constants    = (0 != val);
    else if ( nam.substr(0,6) == "includ" ) s_debug.includes     = (0 != val);
    else if ( nam.substr(0,6) == "matrix" ) s_debug.matrix       = (0 != val);
    else if ( nam.substr(0,6) == "surfac" ) s_debug.surface      = (0 != val);
    else if ( nam.substr(0,6) == "incgua" ) s_debug.include_guard= (0 != val);
    else if ( nam.substr(0,6) == "detele" ) s_debug.detelements  = (0 != val);
  }
}
  
/** Convert/execute plugin objects from the xml (plugins)
 *
 *
 */
template <> void Converter<Plugin>::operator()(xml_h e) const {
  xml_comp_t  plugin(e);
  std::string name = plugin.nameStr();
  std::string type = "default";

  if ( xml_attr_t typ_attr = e.attr_nothrow(_U(type)) )   {
    type = e.attr<std::string>(typ_attr);
  }
  if ( type == "default" )  {
    std::vector<char*> argv;
    std::vector<std::string> arguments;
    for (xml_coll_t coll(e, _U(arg)); coll; ++coll) {
      std::string val = coll.attr<std::string>(_U(value));
      arguments.emplace_back(val);
    }
    for (xml_coll_t coll(e, _U(argument)); coll; ++coll) {
      std::string val = coll.attr<std::string>(_U(value));
      arguments.emplace_back(val);
    }
    for(std::vector<std::string>::iterator i=arguments.begin(); i!=arguments.end(); ++i)
      argv.emplace_back(&((*i)[0]));
    description.apply(name.c_str(),int(argv.size()), argv.empty() ? nullptr : &argv[0]);
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
    std::string nam = constant.attr<std::string>(_U(name));
    std::string val = constant.attr<std::string>(_U(value));
    std::string typ = constant.hasAttr(_U(type)) ? constant.attr<std::string>(_U(type)) : "number";
    Constant c(nam, val, typ);
    _toDictionary(nam, val, typ);
    description.addConstant(c);
    if ( s_debug.constants )   {
      printout(ALWAYS, "Compact",
               "++ Converting constant %-16s = %-32s [%s]", nam.c_str(), val.c_str(), typ.c_str());
    }
    return;
  }
  xml::DocumentHolder doc(xml::DocumentHandler().load(e, e.attr_value(_U(ref))));
  if ( s_debug.includes )   {
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
  Header h = description.header();
  if( !h.isValid() )  {
    h = Header(e.attr<std::string>(_U(name)), e.attr<std::string>(_U(title), "Undefined"));
    h.setUrl(e.attr<std::string>(_U(url), "Undefined"));
    h.setAuthor(e.attr<std::string>(_U(author), "Undefined"));
    h.setStatus(e.attr<std::string>(_U(status), "development"));
    h.setVersion(e.attr<std::string>(_U(version), "Undefined"));
    h.setComment(e.hasChild(_U(comment)) ? e.child(_U(comment)).text() : "No Comment");
    description.setHeader(h);
    return;
  }
  printout(WARNING, "Compact","++ Overwriting/updating Header structure is very dangerous. Try to avoid this.");
  printout(WARNING, "Compact","++     Header definition in: %s", xml::DocumentHandler::system_path(e).c_str());
  if( e.hasAttr(_U(comment)) ) h.setComment(e.attr<std::string>(_U(comment)).c_str());
  if( e.hasAttr(_U(version)) ) h.setVersion(e.attr<std::string>(_U(version)).c_str());
  if( e.hasAttr(_U(author)) ) h.setAuthor(e.attr<std::string>(_U(author)).c_str());
  if( e.hasAttr(_U(status)) ) h.setStatus(e.attr<std::string>(_U(status)).c_str());
  if( e.hasAttr(_U(title)) ) h.setTitle(e.attr<std::string>(_U(title)).c_str());
  if( e.hasAttr(_U(name)) ) h.setName(e.attr<std::string>(_U(name)).c_str());
  if( e.hasAttr(_U(url)) ) h.setUrl(e.attr<std::string>(_U(url)).c_str());
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
  TGeoManager&      mgr     = description.manager();
  xml_tag_t         mname   = x_mat.name();
  const char*       matname = mname.c_str();
  TGeoElementTable* table   = mgr.GetElementTable();
  TGeoMaterial*     mat     = mgr.GetMaterial(matname);
  TGeoMixture*      mix     = dynamic_cast<TGeoMixture*>(mat);
  xml_coll_t        fractions (x_mat, _U(fraction));
  xml_coll_t        composites(x_mat, _U(composite));

  if (0 == mat) {
    TGeoMaterial* comp_mat;
    TGeoElement*  comp_elt;
    xml_h  density     = x_mat.child(_U(D), false);
    double dens_val    = density.ptr() ? density.attr<double>(_U(value)) : 0.0;
    double dens_unit   = 1.0;

    if ( !density.ptr() ) {
      throw_print("Compact2Objects[ERROR]: material without density tag ( <D  unit=\"g/cm3\" value=\"..\"/> ) provided: "
                  + std::string( matname ) ) ;
    }
    if ( density.hasAttr(_U(unit)) )   {
      dens_unit = density.attr<double>(_U(unit))/xml::_toDouble(_Unicode(gram/cm3));
    }
    if ( dens_unit != 1.0 )  {
      dens_val *= dens_unit;
      printout(s_debug.materials ? ALWAYS : DEBUG, "Compact","Density unit: %.3f [%s] raw: %.3f normalized: %.3f ",
               dens_unit, density.attr<std::string>(_U(unit)).c_str(), dens_val, (dens_val*dens_unit));
    }
    mat = mix = new TGeoMixture(matname, composites.size(), dens_val);
    std::size_t         ifrac = 0;
    std::vector<double> composite_fractions;
    double              composite_fractions_total = 0.0;
    for (composites.reset(); composites; ++composites)   {
      std::string nam      = composites.attr<std::string>(_U(ref));
      double      fraction = composites.attr<double>(_U(n));
      if (0 != (comp_mat = mgr.GetMaterial(nam.c_str())))
        fraction *= comp_mat->GetA();
      else if (0 != (comp_elt = table->FindElement(nam.c_str())))
        fraction *= comp_elt->A();
      else
        except("Compact2Objects","Converting material: %s Element missing: %s",mname.c_str(),nam.c_str());
      composite_fractions_total += fraction;
      composite_fractions.emplace_back(fraction);
    }
    for (composites.reset(), ifrac=0; composites; ++composites, ++ifrac) {
      std::string nam      = composites.attr<std::string>(_U(ref));
      double      fraction = composite_fractions[ifrac]/composite_fractions_total;
      if (0 != (comp_mat = mgr.GetMaterial(nam.c_str())))
        mix->AddElement(comp_mat, fraction);
      else if (0 != (comp_elt = table->FindElement(nam.c_str())))
        mix->AddElement(comp_elt, fraction);
    }
    for (fractions.reset(); fractions; ++fractions) {
      std::string nam      = fractions.attr<std::string>(_U(ref));
      double      fraction = fractions.attr<double>(_U(n));
      if (0 != (comp_mat = mgr.GetMaterial(nam.c_str())))
        mix->AddElement(comp_mat, fraction);
      else if (0 != (comp_elt = table->FindElement(nam.c_str())))
        mix->AddElement(comp_elt, fraction);
      else
        throw_print("Compact2Objects[ERROR]: Converting material:" + mname + " Element missing: " + nam);
    }
    xml_h  temperature = x_mat.child(_U(T), false);
    double temp_val    = description.stdConditions().temperature;
    if ( temperature.ptr() )   {
      double temp_unit = _toDouble("kelvin");
      if ( temperature.hasAttr(_U(unit)) )
        temp_unit = temperature.attr<double>(_U(unit));
      temp_val = temperature.attr<double>(_U(value)) * temp_unit;
    }
    xml_h  pressure     = x_mat.child(_U(P), false);
    double pressure_val = description.stdConditions().pressure;
    if ( pressure.ptr() )   {
      double pressure_unit = _toDouble("pascal");
      if ( pressure.hasAttr(_U(unit)) )
        pressure_unit = pressure.attr<double>(_U(unit));
      pressure_val = pressure.attr<double>(_U(value)) * pressure_unit;
    }
#if 0
    printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
             "++ ROOT raw temperature and pressure: %.3g %.3g",
             mat->GetTemperature(),mat->GetPressure());
#endif
    mat->SetTemperature(temp_val);
    mat->SetPressure(pressure_val);
    printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
             "++ Converting material %-16s  Density: %9.7g  Temperature:%9.7g [K] Pressure:%9.7g [hPa].",
             matname, dens_val, temp_val/dd4hep::kelvin, pressure_val/dd4hep::pascal/100.0);

    mix->SetRadLen(0e0);
    mix->ComputeDerivedQuantities();
    ///
    /// In case there were material properties specified: convert them here
    for(xml_coll_t properties(x_mat, _U(constant)); properties; ++properties) {
      xml_elt_t p = properties;
      if ( p.hasAttr(_U(ref)) )   {
        bool        err = kFALSE;
        std::string ref = p.attr<std::string>(_U(ref));
        mgr.GetProperty(ref.c_str(), &err); /// Check existence
        if ( err == kFALSE )  {
          std::string prop_nam = p.attr<std::string>(_U(name));
          mat->AddConstProperty(prop_nam.c_str(), ref.c_str());
          printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
                   "++            material %-16s  add constant property: %s  ->  %s.",
                   mat->GetName(), prop_nam.c_str(), ref.c_str());
          continue;
        }
        // ERROR
        throw_print("Compact2Objects[ERROR]: Converting material:" + mname + " ConstProperty missing in TGeoManager: " + ref);
      }
      else if ( p.hasAttr(_U(value)) )   {
        std::stringstream str;
        std::string       ref, prop_nam = p.attr<std::string>(_U(name));
        str << prop_nam << "_" << (void*)mat;
        ref = str.str();
        mgr.AddProperty(ref.c_str(), p.attr<double>(_U(value))); /// Check existence
        mat->AddConstProperty(prop_nam.c_str(), ref.c_str());
        printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
                 "++            material %-16s  add constant property: %s  ->  %s.",
                 mat->GetName(), prop_nam.c_str(), ref.c_str());
      }
      else if ( p.hasAttr(_U(option)) )   {
        std::string prop_nam = p.attr<std::string>(_U(name));
        std::string prop_typ = p.attr<std::string>(_U(option));
        mat->AddConstProperty(prop_nam.c_str(), prop_typ.c_str());
        printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
                 "++            material %-16s  add constant property: %s  ->  %s.",
                 mat->GetName(), prop_nam.c_str(), prop_typ.c_str());
      }
    }
    /// In case there were material properties specified: convert them here
    for(xml_coll_t properties(x_mat, _U(property)); properties; ++properties) {
      xml_elt_t p = properties;
      if ( p.hasAttr(_U(ref)) )   {
        std::string  ref     = p.attr<std::string>(_U(ref));
        TGDMLMatrix* gdmlMat = mgr.GetGDMLMatrix(ref.c_str());
        if ( gdmlMat )  {
          std::string prop_nam = p.attr<std::string>(_U(name));
          mat->AddProperty(prop_nam.c_str(), ref.c_str());
          printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
                   "++            material %-16s  add property: %s  ->  %s.",
                   mat->GetName(), prop_nam.c_str(), ref.c_str());
          continue;
        }
        // ERROR
        throw_print("Compact2Objects[ERROR]: Converting material:" + mname + " Property missing: " + ref);
      }
    }
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
    std::string form = x_mat.attr<std::string>(_U(formula));
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
  TGeoManager&      mgr = description.manager();
  std::string       nam = isotope.nameStr();
  TGeoElementTable* tab = mgr.GetElementTable();
  TGeoIsotope*      iso = tab->FindIsotope(nam.c_str());

  // Create the isotope object in the event it is not yet present from the XML data
  if ( !iso )   {
    xml_ref_t   atom(isotope.child(_U(atom)));
    std::string unit  = atom.attr<std::string>(_U(unit));
    double      value = atom.attr<double>(_U(value));
    double      a     = value * _multiply<double>(unit,"mol/g");
    int         n     = isotope.attr<int>(_U(N));
    int         z     = isotope.attr<int>(_U(Z));
    iso = new TGeoIsotope(nam.c_str(), z, n, a);
    printout(s_debug.isotopes ? ALWAYS : DEBUG, "Compact",
             "++ Converting isotope  %-16s  Z:%3d N:%3d A:%8.4f [g/mol]",
             iso->GetName(), iso->GetZ(), iso->GetN(), iso->GetA());
  }
  else  {
    printout(s_debug.isotopes ? WARNING : DEBUG, "Compact",
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
      xml_ref_t   atom(elem.child(_U(atom)));
      std::string formula = elem.attr<std::string>(_U(formula));
      double      value   = atom.attr<double>(_U(value));
      std::string unit    = atom.attr<std::string>(_U(unit));
      int         z       = elem.attr<int>(_U(Z));
      double      a       = value*_multiply<double>(unit,"mol/g");
      printout(s_debug.elements ? ALWAYS : DEBUG, "Compact",
               "++ Converting element  %-16s  [%-3s] Z:%3d A:%8.4f [g/mol]",
               name.c_str(), formula.c_str(), z, a);
      tab->AddElement(name.c_str(), formula.c_str(), z, a);
    }
    else  {
      int num_isotopes = 0;
      std::string formula = elem.hasAttr(_U(formula)) ? elem.attr<std::string>(_U(formula)) : name.str();
      for( xml_coll_t i(elem,_U(fraction)); i; ++i)
        ++num_isotopes;
      elt = new TGeoElement(name.c_str(), formula.c_str(), num_isotopes);
      tab->AddElement(elt);
      for( xml_coll_t i(elem,_U(fraction)); i; ++i)  {
        double       frac = i.attr<double>(_U(n));
        std::string  ref  = i.attr<std::string>(_U(ref));
        TGeoIsotope* iso  = tab->FindIsotope(ref.c_str());
        if ( !iso )  {
          except("Compact","Element %s cannot be constructed. Isotope '%s' (fraction: %.3f) missing!",
                 name.c_str(), ref.c_str(), frac);
        }
        printout(s_debug.elements ? ALWAYS : DEBUG, "Compact",
                 "++ Converting element  %-16s  Add isotope: %-16s fraction:%.4f.",
                 name.c_str(), ref.c_str(), frac);
        elt->AddIsotope(iso, frac);
      }
      printout(s_debug.elements ? ALWAYS : DEBUG, "Compact",
               "++ Converted  element  %-16s  [%-3s] Z:%3d A:%8.4f [g/mol] with %d isotopes.",
               name.c_str(), formula.c_str(), elt->Z(), elt->A(), num_isotopes);
    }
    elt = tab->FindElement(name.c_str());
    if (!elt) {
      throw_print("Failed to properly insert the Element:"+name+" into the element table!");
    }
  }
  else  {
    printout(s_debug.elements ? WARNING : DEBUG, "Compact",
             "++ Element %-16s  Z:%3d N:%3d A:%8.4f [g/mol] ALREADY defined. [Ignore definition]",
             elt->GetName(), elt->Z(), elt->N(), elt->A());
  }
}

/** Convert compact isotope objects
 *
 *   <std_conditions type="STP or NTP"> // type optional
 *     <item name="temperature" unit="kelvin" value="273.15"/>
 *     <item name="pressure"    unit="kPa" value="100"/>
 *   </std_conditions>
 */
template <> void Converter<STD_Conditions>::operator()(xml_h e) const {
  xml_dim_t cond(e);
  // Create the isotope object in the event it is not yet present from the XML data
  if ( cond.ptr() )   {
    if ( cond.hasAttr(_U(type)) )   {
      description.setStdConditions(cond.typeStr());
    }
    xml_h  temperature = cond.child(_U(T), false);
    double temp_val    = description.stdConditions().temperature;
    if ( temperature.ptr() )   {
      double temp_unit = _toDouble("kelvin");
      if ( temperature.hasAttr(_U(unit)) )
        temp_unit = temperature.attr<double>(_U(unit));
      temp_val = temperature.attr<double>(_U(value)) * temp_unit;
    }
    xml_h  pressure     = cond.child(_U(P), false);
    double pressure_val = description.stdConditions().pressure;
    if ( pressure.ptr() )   {
      double pressure_unit = _toDouble("pascal");
      if ( pressure.hasAttr(_U(unit)) )
        pressure_unit = pressure.attr<double>(_U(unit));
      pressure_val = pressure.attr<double>(_U(value)) * pressure_unit;
    }
    description.setStdConditions(temp_val, pressure_val);
    printout(s_debug.materials ? ALWAYS : DEBUG, "Compact",
             "+++ Material standard conditions: Temperature: %.3f Kelvin Pressure: %.3f hPa",
             temp_val/_toDouble("kelvin"), pressure_val/_toDouble("hPa"));
  }
}

/** Convert compact optical surface objects (defines)
 *
 *
 */
template <> void Converter<OpticalSurface>::operator()(xml_h element) const {
  xml_elt_t    e = element;
  TGeoManager& mgr = description.manager();
  std::string  sname = e.attr<std::string>(_U(name));
  std::string  ref, pname;
  
  // Defaults from Geant4
  OpticalSurface::EModel  model  = OpticalSurface::Model::kMglisur;
  OpticalSurface::EFinish finish = OpticalSurface::Finish::kFpolished;
  OpticalSurface::EType   type   = OpticalSurface::Type::kTdielectric_metal;
  Double_t value = 1.0;
  if ( e.hasAttr(_U(type))   ) type   = OpticalSurface::stringToType(e.attr<std::string>(_U(type)));
  if ( e.hasAttr(_U(model))  ) model  = OpticalSurface::stringToModel(e.attr<std::string>(_U(model)));
  if ( e.hasAttr(_U(finish)) ) finish = OpticalSurface::stringToFinish(e.attr<std::string>(_U(finish)));
  if ( e.hasAttr(_U(value))  ) value  = e.attr<double>(_U(value));
  OpticalSurface surf(description, sname, model, finish, type, value);
  if ( s_debug.surface )    {
    printout(ALWAYS,"Compact","+++ Reading optical surface %s Typ:%d model:%d finish:%d value: %.3f",
             sname.c_str(), int(type), int(model), int(finish), value);
  }
  for (xml_coll_t props(e, _U(property)); props; ++props)  {
    pname = props.attr<std::string>(_U(name));
    if ( props.hasAttr(_U(ref)) )  {
      bool err = kFALSE;
      ref = props.attr<std::string>(_U(ref));
      mgr.GetProperty(ref.c_str(), &err); /// Check existence
      surf->AddProperty(pname.c_str(), ref.c_str());
      if ( s_debug.surface )  {
        printout(ALWAYS,"Compact","+++ \t\t Property:  %s  -> %s", pname.c_str(), ref.c_str());
      }
      continue;
    }
    std::size_t         cols = props.attr<long>(_U(coldim));
    xml_attr_t          opt  = props.attr_nothrow(_U(option));
    std::stringstream   str(props.attr<std::string>(_U(values))), str_nam;
    std::string         val;
    std::vector<double> values;
    while ( !str.eof() )   {
      val = "";
      str >> val;
      if ( val.empty() && !str.good() ) break;
      values.emplace_back(_toDouble(val));
    }
    /// Create table and register table
    TGDMLMatrix* table = new TGDMLMatrix("",values.size()/cols, cols);
    if ( opt )   {
      std::string tit = e.attr<std::string>(opt);
      str_nam << tit << "|";
    }
    str_nam << pname << "__" << (void*)table;
    table->SetName(str_nam.str().c_str());
    table->SetTitle(pname.c_str());
    for (std::size_t i=0, n=values.size(); i<n; ++i)
      table->Set(i/cols, i%cols, values[i]);
    surf->AddProperty(pname.c_str(), table->GetName());
    description.manager().AddGDMLMatrix(table);
  }
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,31,1)
  //
  // In case there were constant surface properties specified: convert them here
  for(xml_coll_t properties(e, _U(constant)); properties; ++properties) {
    xml_elt_t p = properties;
    pname = p.attr<std::string>(_U(name));
    if ( p.hasAttr(_U(ref)) )   {
      bool err = kFALSE;
      ref = p.attr<std::string>(_U(ref));
      mgr.GetProperty(ref.c_str(), &err); /// Check existence
      if ( err == kFALSE )  {
        surf->AddConstProperty(pname.c_str(), ref.c_str());
        printout(s_debug.surface ? ALWAYS : DEBUG, "Compact",
                 "++            surface  %-16s  add constant property: %s  ->  %s.",
                 surf->GetName(), pname.c_str(), ref.c_str());
        continue;
      }
      // ERROR
      throw_print("Compact2Objects[ERROR]: Converting surface: " + sname +
                  " ConstProperty missing in TGeoManager: " + ref);
    }
    else if ( p.hasAttr(_U(value)) )   {
      std::stringstream str;
      str << pname << "_" << (void*)surf.ptr();
      ref = str.str();
      mgr.AddProperty(ref.c_str(), p.attr<double>(_U(value))); /// Check existence
      surf->AddConstProperty(pname.c_str(), ref.c_str());
      printout(s_debug.surface ? ALWAYS : DEBUG, "Compact",
               "++            surface  %-16s  add constant property: %s  ->  %s.",
               surf->GetName(), pname.c_str(), ref.c_str());
    }
    else if ( p.hasAttr(_U(option)) )   {
      std::string ptyp = p.attr<std::string>(_U(option));
      surf->AddConstProperty(pname.c_str(), ptyp.c_str());
      printout(s_debug.surface ? ALWAYS : DEBUG, "Compact",
               "++            surface  %-16s  add constant property: %s  ->  %s.",
               surf->GetName(), pname.c_str(), ptyp.c_str());
    }
  }
#endif
}

/** Convert compact constant property (Material properties stored in TGeoManager)
 *
 *  <constant name="RINDEX" value="8.123"/>
 *
 */
template <> void Converter<PropertyConstant>::operator()(xml_h e) const    {
  double      value = e.attr<double>(_U(value));
  std::string name  = e.attr<std::string>(_U(name));
  description.manager().AddProperty(name.c_str(), value);
  if ( s_debug.matrix )    {
    printout(ALWAYS,"Compact","+++ Reading property %s : %f",name.c_str(), value);
  }
#if 0
  xml_attr_t opt = e.attr_nothrow(_U(title));
  if ( opt )    {
    std::string  val = e.attr<std::string>(opt);
    TNamed* nam = description.manager().GetProperty(name.c_str());
    if ( !nam )   {
      except("Compact","Failed to access just added manager property: %s",name.c_str());
    }
    nam->SetTitle(val.c_str());
  }
#endif
}

/** Convert compact property table objects (defines)
 *
 *  <matrix coldim="2" name="RINDEX0xf5972d0" values="1.5e-06 1.0013 1. ...."/>
 *
 */
template <> void Converter<PropertyTable>::operator()(xml_h e) const {
  std::vector<double> vals;
  std::size_t         cols = e.attr<unsigned long>(_U(coldim));
  std::stringstream   str(e.attr<std::string>(_U(values)));

  if ( s_debug.matrix )    {
    printout(ALWAYS,"Compact","+++ Reading property table %s with %d columns.",
             e.attr<std::string>(_U(name)).c_str(), cols);
  }
  vals.reserve(1024);
  while ( !str.eof() )   {
    std::string item;
    str >> item;
    if ( item.empty() && !str.good() ) break;
    vals.emplace_back(_toDouble(item));
    if ( s_debug.matrix )    {
      std::cout << " state:" << (str.good() ? "OK " : "BAD") << " '" << item << "'";
      if ( 0 == (vals.size()%cols) ) std::cout << std::endl;
    }
  }
  if ( s_debug.matrix )    {
    std::cout << std::endl;
  }
  /// Create table and register table
  xml_attr_t    opt = e.attr_nothrow(_U(option));
  PropertyTable tab(description,
                    e.attr<std::string>(_U(name)),
                    opt ? e.attr<std::string>(opt).c_str() : "",
                    vals.size()/cols, cols);
  for( std::size_t i=0, n=vals.size(); i < n; ++i )
    tab->Set(i/cols, i%cols, vals[i]);
  //if ( s_debug.matrix ) tab->Print();
}

/** Convert compact visualization attribute to Detector visualization attribute.
 *
 *  <vis name="SiVertexBarrelModuleVis"
 *       alpha="1.0" r="1.0" g="0.75" b="0.76"
 *       drawingStyle="wireframe"
 *       showDaughters="false"
 *       visible="true"/>
 *
 *  Optionally inherit an already defined VisAttr and override other properties.
 *
 *  <vis name="SiVertexEndcapModuleVis"
 *       ref="SiVertexBarrelModuleVis"
 *       alpha="0.5"/>
 */
template <> void Converter<VisAttr>::operator()(xml_h e) const {
  VisAttr attr(e.attr<std::string>(_U(name)));
  float alpha = 1.0;
  float red   = 1.0;
  float green = 1.0;
  float blue  = 1.0;
  bool use_ref = false;
  if(e.hasAttr(_U(ref))) {
    use_ref = true;
    auto refName = e.attr<std::string>(_U(ref));
    const auto refAttr = description.visAttributes(refName);
    if(!refAttr.isValid() )  {
      except("Compact","+++ Reference VisAttr %s does not exist", refName.c_str());
    }
    // Just copying things manually.
    // I think a handle's copy constructor/assignment would reuse the underlying pointer... maybe?
    refAttr.argb(alpha,red,green,blue);
    attr.setColor(alpha,red,green,blue);
    attr.setDrawingStyle( refAttr.drawingStyle());
    attr.setLineStyle( refAttr.lineStyle());
    attr.setShowDaughters(refAttr.showDaughters());
    attr.setVisible(refAttr.visible());
  }
  xml_dim_t dim(e);
  alpha = dim.alpha(alpha);
  red   = dim.r(red  );
  green = dim.g(green);
  blue  = dim.b(blue );

  printout(s_debug.visattr ? ALWAYS : DEBUG, "Compact",
           "++ Converting VisAttr  structure: %-16s. Alpha=%.2f R=%.3f G=%.3f B=%.3f",
           attr.name(), alpha, red, green, blue);
  attr.setColor(alpha, red, green, blue);
  if (e.hasAttr(_U(visible)))
    attr.setVisible(e.attr<bool>(_U(visible)));
  if (e.hasAttr(_U(lineStyle))) {
    std::string ls = e.attr<std::string>(_U(lineStyle));
    if (ls == "unbroken")
      attr.setLineStyle(VisAttr::SOLID);
    else if (ls == "broken")
      attr.setLineStyle(VisAttr::DASHED);
  }
  else {
    if (!use_ref)
      attr.setLineStyle(VisAttr::SOLID);
  }
  if (e.hasAttr(_U(drawingStyle))) {
    std::string ds = e.attr<std::string>(_U(drawingStyle));
    if (ds == "wireframe")
      attr.setDrawingStyle(VisAttr::WIREFRAME);
    else if (ds == "solid")
      attr.setDrawingStyle(VisAttr::SOLID);
  }
  else {
    if (!use_ref)
      attr.setDrawingStyle(VisAttr::SOLID);
  }
  if (e.hasAttr(_U(showDaughters)))
    attr.setShowDaughters(e.attr<bool>(_U(showDaughters)));
  else {
    if (!use_ref)
      attr.setShowDaughters(true);
  }
  description.addVisAttribute(attr);
}

/** Specialized converter for compact region objects.
 *
 */
template <> void Converter<Region>::operator()(xml_h elt) const {
  xml_dim_t       e = elt;
  Region          region(e.nameStr());
  auto&           limits       = region.limits();
  xml_attr_t      cut          = elt.attr_nothrow(_U(cut));
  xml_attr_t      threshold    = elt.attr_nothrow(_U(threshold));
  xml_attr_t store_secondaries = elt.attr_nothrow(_U(store_secondaries));
  double ene = e.eunit(1.0), len = e.lunit(1.0);

  printout(s_debug.regions ? ALWAYS : DEBUG, "Compact",
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
    limits.emplace_back(user_limits.attr<std::string>(_U(name)));
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
  std::string type = seg.attr<std::string>(_U(type));
  std::string name = seg.hasAttr(_U(name)) ? seg.attr<std::string>(_U(name)) : std::string();
  std::pair<Segmentation,IDDescriptor>* opt = _option<std::pair<Segmentation,IDDescriptor> >();

  const BitFieldCoder* bitfield = &opt->second->decoder;
  Segmentation segment(type, name, bitfield);
  if ( segment.isValid() ) {
    const DDSegmentation::Parameters& pars = segment.parameters();
    printout(s_debug.segmentation ? ALWAYS : DEBUG, "Compact",
             "++ Converting segmentation structure: %s of type %s.",name.c_str(),type.c_str());
    for(const auto p : pars )  {
      xml::Strng_t pNam(p->name());
      if ( seg.hasAttr(pNam) ) {
        std::string pType = p->type();
        if ( pType.compare("int") == 0 ) {
          typedef DDSegmentation::TypedSegmentationParameter<int> ParInt;
          static_cast<ParInt*>(p)->setTypedValue(seg.attr<int>(pNam));
        } else if ( pType.compare("float") == 0 ) {
          typedef DDSegmentation::TypedSegmentationParameter<float> ParFloat;
          static_cast<ParFloat*>(p)->setTypedValue(seg.attr<float>(pNam));
        } else if ( pType.compare("doublevec") == 0 ) {
          std::vector<double> valueVector;
          std::string par = seg.attr<std::string>(pNam);
          printout(s_debug.segmentation ? ALWAYS : DEBUG, "Compact",
                   "++ Converting this std::string structure: %s.",par.c_str());
          std::vector<std::string> elts = DDSegmentation::splitString(par);
          for (const std::string& spar : elts )  {
            if ( spar.empty() ) continue;
            valueVector.emplace_back(_toDouble(spar));
          }
          typedef DDSegmentation::TypedSegmentationParameter< std::vector<double> > ParDouVec;
          static_cast<ParDouVec*>(p)->setTypedValue(valueVector);
        } else if ( pType.compare("double" ) == 0) {
          typedef DDSegmentation::TypedSegmentationParameter<double>ParDouble;
          static_cast<ParDouble*>(p)->setTypedValue(seg.attr<double>(pNam));
        } else {
          p->setValue(seg.attr<std::string>(pNam));
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
          std::stringstream tree;
          xml::dump_tree(sub,tree);
          throw_print("Nested segmentations: Invalid key specification:"+tree.str());
        }
        printout(s_debug.segmentation ? ALWAYS : DEBUG,"Compact",
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
  xml_h id  = e.child(_U(id));
  std::string name = e.attr<std::string>(_U(name));
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
  
  printout(s_debug.readout ? ALWAYS : DEBUG,
           "Compact", "++ Converting readout  structure: %-16s. %s%s",
           ro.name(), id ? "ID: " : "", id ? id.text().c_str() : "");
  
  for(xml_coll_t colls(e,_U(hits_collections)); colls; ++colls)   {
    std::string hits_key;
    if ( colls.hasAttr(_U(key)) ) hits_key = colls.attr<std::string>(_U(key));
    for(xml_coll_t coll(colls,_U(hits_collection)); coll; ++coll)   {
      xml_dim_t c(coll);
      std::string coll_name = c.nameStr();
      std::string coll_key  = hits_key;
      long   key_min = 0, key_max = 0;

      if ( c.hasAttr(_U(key)) )   {
        coll_key = c.attr<std::string>(_U(key));
      }
      if ( c.hasAttr(_U(key_value)) )   {
        key_max = key_min = c.key_value();
      }
      else if ( c.hasAttr(_U(key_min)) && c.hasAttr(_U(key_max)) )  {
        key_min = c.key_min();
        key_max = c.key_max();
      }
      else   {
        std::stringstream tree;
        xml::dump_tree(e,tree);
        throw_print("Readout: Invalid specification for multiple hit collections."+tree.str());
      }
      printout(s_debug.readout ? ALWAYS : DEBUG,"Compact",
               "++ Readout[%s]: Add hit collection %s [%s]  %d-%d",
               ro.name(), coll_name.c_str(), coll_key.c_str(), key_min, key_max);
      HitCollection hits(coll_name, coll_key, key_min, key_max);
      ro->hits.emplace_back(hits);
    }
  }
  description.addReadout(ro);
}

static long load_readout(Detector& description, xml_h element) {
  Converter<Readout> converter(description);
  converter(element);
  return 1;
}
DECLARE_XML_DOC_READER(readout,load_readout)


/** Specialized converter for compact LimitSet objects.
 *
 *      <limitset name="....">
 *        <limit name="step_length_max" particles="*" value="5.0" unit="mm" />
 *  ... </limitset>
 */
template <> void Converter<LimitSet>::operator()(xml_h e) const {
  Limit limit;
  LimitSet ls(e.attr<std::string>(_U(name)));
  printout(s_debug.limits ? ALWAYS : DEBUG, "Compact",
           "++ Converting LimitSet structure: %s.",ls.name());
  for (xml_coll_t c(e, _U(limit)); c; ++c) {
    limit.name      = c.attr<std::string>(_U(name));
    limit.particles = c.attr<std::string>(_U(particles));
    limit.content   = c.attr<std::string>(_U(value));
    limit.unit      = c.attr<std::string>(_U(unit));
    limit.value     = _multiply<double>(limit.content, limit.unit);
    ls.addLimit(limit);
    printout(s_debug.limits ? ALWAYS : DEBUG, "Compact",
             "++ %s: add %-6s: [%s] = %s [%s] = %f",
             ls.name(), limit.name.c_str(), limit.particles.c_str(),
             limit.content.c_str(), limit.unit.c_str(), limit.value);
  }
  limit.name      = "cut";
  for (xml_coll_t c(e, _U(cut)); c; ++c) {
    limit.particles = c.attr<std::string>(_U(particles));
    limit.content   = c.attr<std::string>(_U(value));
    limit.unit      = c.attr<std::string>(_U(unit));
    limit.value     = _multiply<double>(limit.content, limit.unit);
    ls.addCut(limit);
    printout(s_debug.limits ? ALWAYS : DEBUG, "Compact",
             "++ %s: add %-6s: [%s] = %s [%s] = %f",
             ls.name(), limit.name.c_str(), limit.particles.c_str(),
             limit.content.c_str(), limit.unit.c_str(), limit.value);
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
  std::string name = e.attr<std::string>(_U(name));
  Detector::Properties& prp  = description.properties();
  if ( name.empty() )
    throw_print("Failed to convert properties. No name given!");

  std::vector<xml_attr_t> a = e.attributes();
  if ( prp.find(name) == prp.end() )
    prp.emplace(name, Detector::PropertyValues());

  for (xml_attr_t i : a )
    prp[name].emplace(xml_tag_t(e.attr_name(i)).str(),e.attr<std::string>(i));
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
  std::string msg  = "updated";
  std::string name = e.attr<std::string>(_U(name));
  std::string type = e.attr<std::string>(_U(type));
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
  // Now update the field structure with the generic part ie. set its properties
  CartesianField::Properties& prp = field.properties();
  for ( xml_coll_t c(e, _U(properties)); c; ++c ) {
    std::string props_name = c.attr<std::string>(_U(name));
    std::vector<xml_attr_t>a = c.attributes();
    if ( prp.find(props_name) == prp.end() ) {
      prp.emplace(props_name, Detector::PropertyValues());
    }
    for ( xml_attr_t i : a )
      prp[props_name].emplace(xml_tag_t(c.attr_name(i)).str(), c.attr<std::string>(i));

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
  std::string name = element.attr<std::string>(_U(name));
  try {
    SensitiveDetector sd = description.sensitiveDetector(name);
    xml_attr_t type = element.attr_nothrow(_U(type));
    if ( type )  {
      sd.setType(element.attr<std::string>(type));
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
      std::string l = element.attr<std::string>(limits);
      LimitSet ls = description.limitSet(l);
      if (!ls.isValid()) {
        throw_print("Converter<SensitiveDetector>: Request for non-existing limitset:" + l);
      }
      sd.setLimitSet(ls);
    }
    xml_attr_t region = element.attr_nothrow(_U(region));
    if ( region ) {
      std::string r = element.attr<std::string>(region);
      Region reg = description.region(r);
      if (!reg.isValid()) {
        throw_print("Converter<SensitiveDetector>: Request for non-existing region:" + r);
      }
      sd.setRegion(reg);
    }
    xml_attr_t hits = element.attr_nothrow(_U(hits_collection));
    if (hits) {
      sd.setHitsCollection(element.attr<std::string>(hits));
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
    printout(DEBUG, "Compact", "SensitiveDetector-update: %-18s %-24s Hits:%-24s Cutoff:%7.3f", sd.name(),
             (" [" + sd.type() + "]").c_str(), sd.hitsCollection().c_str(), sd.energyCutoff());
    xml_attr_t sequence = element.attr_nothrow(_U(sequence));
    if (sequence) {
    }
  }
  catch (const std::exception& e) {
    printout(ERROR, "Compact", "++ FAILED    to convert sensitive detector: %s: %s", name.c_str(), e.what());
  }
  catch (...) {
    printout(ERROR, "Compact", "++ FAILED    to convert sensitive detector: %s: %s", name.c_str(), "UNKNONW Exception");
  }
}

static void setChildTitles(const std::pair<std::string, DetElement>& e) {
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
  std::string type = element.attr<std::string>(_U(type));
  std::string name = element.attr<std::string>(_U(name));
  std::string name_match = ":" + name + ":";
  std::string type_match = ":" + type + ":";

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
    std::string par_name;
    xml_attr_t attr_par = element.attr_nothrow(_U(parent));
    xml_elt_t  elt_par(0);
    if (attr_par)
      par_name = element.attr<std::string>(attr_par);
    else if ( (elt_par=element.child(_U(parent),false)) )
      par_name = elt_par.attr<std::string>(_U(name));
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
    if( s_debug.detelements )  {
      printout(ALWAYS, "Compact","++ Building DetElement %s of type: %s. Parent: %s",
	       name.c_str(), type.c_str(), par_name.c_str());
    }
    
    xml_attr_t attr_ro  = element.attr_nothrow(_U(readout));
    SensitiveDetector sd;
    Segmentation seg;
    if ( attr_ro )   {
      Readout ro = description.readout(element.attr<std::string>(attr_ro));
      if (!ro.isValid()) {
        except("Compact","No Readout structure present for detector:" + name);
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
      setChildTitles(std::make_pair(name, det));
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

    if (!det.isValid())  {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &description, &element, &sens);
      except("Compact","Failed to execute subdetector creation plugin. %s", dbg.missingFactory(type).c_str());
    }
    description.addDetector(det);
    description.surfaceManager().registerSurfaces(det);
    return;
  }
  catch (const std::exception& e)  {
    printout(ERROR, "Compact", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), e.what());
    std::terminate();
  }
  catch (...)  {
    printout(ERROR, "Compact", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), "UNKNONW Exception");
    std::terminate();
  }
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<IncludeFile>::operator()(xml_h element) const   {
  xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
  if ( s_debug.include_guard) {
    // Include guard, we check whether this file was already processed
    if (check_process_file(description, doc.uri()))
      return;
  }
  xml_h root = doc.root();
  if ( s_debug.includes )   {
    printout(ALWAYS, "Compact","++ Processing xml document %s.",doc.uri().c_str());
  }
  if ( root.tag() == "materials" || root.tag() == "elements" )   {
    xml_coll_t(root, _U(isotope)).for_each(Converter<Isotope>(this->description,0,0));
    xml_coll_t(root, _U(element)).for_each(Converter<Atom>(this->description));
    xml_coll_t(root, _U(material)).for_each(Converter<Material>(this->description));
    return;
  }
  this->description.fromXML(doc.uri(), this->description.buildType());
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<JsonFile>::operator()(xml_h element) const {
  std::string base = xml::DocumentHandler::system_directory(element);
  std::string file = element.attr<std::string>(_U(ref));
  std::vector<char*>  argv{&file[0], &base[0]};
  description.apply("DD4hep_JsonProcessor",int(argv.size()), &argv[0]);
}

/// Read alignment entries from a seperate file in one of the include sections of the geometry
template <> void Converter<XMLFile>::operator()(xml_h element) const {
  PrintLevel  level = s_debug.includes ? ALWAYS : DEBUG;
  std::string fname = element.attr<std::string>(_U(ref));
  std::size_t idx   = fname.find("://");
  std::error_code ec;

  if ( idx == std::string::npos && std::filesystem::exists(fname, ec) )  {
    // Regular file without protocol specification
    printout(level, "Compact","++ Processing xml document %s.", fname.c_str());
    this->description.fromXML(fname, this->description.buildType());
  }
  else if ( idx == std::string::npos )  {
    // File relative to location of xml tag (protocol specification not possible)
    std::string location = xml::DocumentHandler::system_path(element, fname);
    printout(level, "Compact","++ Processing xml document %s.", location.c_str());
    this->description.fromXML(location, this->description.buildType());
  }
  else if ( idx > 0 )   {
    // File with protocol specification: must trust the location and the parser capabilities
    printout(level, "Compact","++ Processing xml document %s.", fname.c_str());
    this->description.fromXML(fname, this->description.buildType());
  }
  else  {
    // Are there any other possibilities ?
    printout(level, "Compact","++ Processing xml document %s.", fname.c_str());
    this->description.fromXML(fname, this->description.buildType());
  }
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<World>::operator()(xml_h element) const {
  xml_elt_t  x_world(element);
  xml_comp_t x_shape = x_world.child(_U(shape), false);
  xml_attr_t att = x_world.getAttr(_U(material));
  Material   mat = att ? description.material(x_world.attr<std::string>(att)) : description.air();
  Volume     world_vol;

  /// Create the shape and the corresponding volume
  if ( x_shape )   {
    Solid sol(x_shape.createShape());
    world_vol = Volume("world_volume", sol, mat);
    printout(INFO, "Compact", "++ Created successfully world volume '%s'. shape: %s material:%s.",
             world_vol.name(), sol.type(), mat.name());
    description.manager().SetTopVolume(world_vol.ptr());
  }
  else   {
    world_vol = description.worldVolume();
    if ( !world_vol && att )   {
      /// If we require a user configured world, but no shape is given, define the standard box.
      /// Implicitly assumes that the box dimensions are given in the standard way.
      Box sol("world_x", "world_y", "world_z");
      world_vol = Volume("world_volume", sol, mat);
      printout(INFO, "Compact", "++ Created world volume '%s' as %s (%.2f, %.2f %.2f [cm]) material:%s.",
               world_vol.name(), sol.type(),
               sol.x()/dd4hep::cm, sol.y()/dd4hep::cm, sol.z()/dd4hep::cm,
               mat.name());
      description.manager().SetTopVolume(world_vol.ptr());
    }
    else if ( !world_vol )  {
      except("Compact", "++ Logical error: "
             "You cannot configure the world volume before it is created and not giving creation instructions.");
    }
  }
  // Delegate further configuration o0f the world volume to the xml utilities:
  if ( world_vol.isValid() )   {
    xml::configVolume(description, x_world, world_vol, false, true);
    auto vis = world_vol.visAttributes();
    if ( !vis.isValid() )  {
      vis = description.visAttributes("WorldVis");
      world_vol.setVisAttributes(vis);
    }
  }
}

/// Read material entries from a seperate file in one of the include sections of the geometry
template <> void Converter<Parallelworld_Volume>::operator()(xml_h element) const {
  xml_det_t    parallel(element);
  xml_comp_t   shape  = parallel.child(_U(shape));
  xml_dim_t    pos    = element.child(_U(position),false);
  xml_dim_t    rot    = element.child(_U(rotation),false);
  std::string  name   = element.attr<std::string>(_U(name));
  std::string  path   = element.attr<std::string>(_U(anchor));
  bool         conn   = element.attr<bool>(_U(connected),false);
  DetElement   anchor(detail::tools::findElement(description, path));
  Position     position = pos ? Position(pos.x(), pos.y(), pos.z())    : Position();
  RotationZYX  rotation = rot ? RotationZYX(rot.z(), rot.y(), rot.x()) : RotationZYX();

  Material mat = parallel.hasAttr(_U(material))
    ? description.material(parallel.attr<std::string>(_U(material)))
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

/// Process include statements in various sub-tags of compact
template <> void Converter<DetElementInclude>::operator()(xml_h element) const {
  std::string type = element.hasAttr(_U(type)) ? element.attr<std::string>(_U(type)) : std::string("xml");
  if ( type == "xml" )  {
    xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
    if ( s_debug.include_guard ) {
      // Include guard, we check whether this file was already processed
      if (check_process_file(description, doc.uri()))
        return;
    }
    if ( s_debug.includes )   {
      printout(ALWAYS, "Compact","++ Processing xml document %s.",doc.uri().c_str());
    }
    xml_h node = doc.root();
    std::string tag = node.tag();
    if ( tag == "lccdd" )
      Converter<Compact>(this->description)(node);
    else if ( tag == "define" )
      xml_coll_t(node, _U(constant)).for_each(Converter<Constant>(this->description));
    else if ( tag == "readout" )
      Converter<Readout>(this->description)(node);
    else if ( tag == "readouts" )
      xml_coll_t(node, _U(readout)).for_each(Converter<Readout>(this->description));
    else if ( tag == "region" )
      Converter<Region>(this->description)(node);
    else if ( tag == "regions" )
      xml_coll_t(node, _U(region)).for_each(Converter<Region>(this->description));
    else if ( tag == "limits" || tag == "limitsets" )
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
    Converter<IncludeFile>(this->description)(element);
  }
  else if ( type == "include" )  {
    Converter<IncludeFile>(this->description)(element);
  }
  else if ( type == "xml-extended" )  {
    Converter<XMLFile>(this->description)(element);
  }
  else  {
    except("Compact","++ FAILED    Invalid file type:%s. This cannot be processed!",type.c_str());
  }
}

/// Main compact conversion entry point
template <> void Converter<Compact>::operator()(xml_h element) const {
  static int num_calls = 0;
  char text[32];

  ++num_calls;
  xml_elt_t compact(element);
  bool steer_geometry = compact.hasChild(_U(geometry));
  bool open_geometry  = true;
  bool close_document = true;
  bool close_geometry = true;
  bool build_reflections = false;
  xml_dim_t world = element.child(_U(world), false);


  if (element.hasChild(_U(debug)))
    (Converter<Debug>(description))(xml_h(compact.child(_U(debug))));

  if ( steer_geometry )   {
    xml_elt_t steer = compact.child(_U(geometry));
    if ( steer.hasAttr(_U(open))  )
      open_geometry  = steer.attr<bool>(_U(open));
    if ( steer.hasAttr(_U(close)) )
      close_document = steer.attr<bool>(_U(close));
    if ( steer.hasAttr(_U(reflect)) )
      build_reflections = steer.attr<bool>(_U(reflect));
    for (xml_coll_t clr(steer, _U(clear)); clr; ++clr) {
      std::string nam = clr.hasAttr(_U(name)) ? clr.attr<std::string>(_U(name)) : std::string();
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

  if ( s_debug.materials || s_debug.elements )   {
    printout(INFO,"Compact","+++ UNIT System:");
    printout(INFO,"Compact","+++ Density:    %8.3g  Units:%8.3g",
             xml::_toDouble(_Unicode(gram/cm3)), dd4hep::gram/dd4hep::cm3);
    printout(INFO,"Compact","+++ GeV:        %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(GeV)),dd4hep::GeV);
    printout(INFO,"Compact","+++ sec:        %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(second)),dd4hep::second);
    printout(INFO,"Compact","+++ nanosecond: %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(nanosecond)),dd4hep::nanosecond);
    printout(INFO,"Compact","+++ kilo:       %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(kilogram)),dd4hep::kilogram);
    printout(INFO,"Compact","+++ kilo:       %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(joule*s*s/(m*m))),
             dd4hep::joule*dd4hep::s*dd4hep::s/(dd4hep::meter*dd4hep::meter));
    printout(INFO,"Compact","+++ meter:      %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(meter)),dd4hep::meter);
    printout(INFO,"Compact","+++ ampere:     %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(ampere)),dd4hep::ampere);
    printout(INFO,"Compact","+++ degree:     %8.3g  Units:%8.3g",xml::_toDouble(_Unicode(degree)),dd4hep::degree);
  }
  
  xml_coll_t(compact, _U(define)).for_each(_U(include),    Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(define)).for_each(_U(constant),   Converter<Constant>(description));
  xml_coll_t(compact, _U(std_conditions)).for_each(        Converter<STD_Conditions>(description));
  xml_coll_t(compact, _U(includes)).for_each(_U(gdmlFile), Converter<IncludeFile>(description));
  xml_coll_t(compact, _U(includes)).for_each(_U(file),     Converter<IncludeFile>(description));

  if (element.hasChild(_U(info)))
    (Converter<Header>(description))(xml_h(compact.child(_U(info))));

  /// These two must be parsed early, because they are needed by the detector constructors
  xml_coll_t(compact, _U(properties)).for_each(_U(attributes), Converter<Property>(description));
  xml_coll_t(compact, _U(properties)).for_each(_U(constant), Converter<PropertyConstant>(description));
  xml_coll_t(compact, _U(properties)).for_each(_U(matrix),   Converter<PropertyTable>(description));
  xml_coll_t(compact, _U(properties)).for_each(_U(plugin),   Converter<Plugin> (description));
  xml_coll_t(compact, _U(surfaces)).for_each(_U(opticalsurface), Converter<OpticalSurface>(description));

  xml_coll_t(compact, _U(materials)).for_each(_U(element),  Converter<Atom>(description));
  xml_coll_t(compact, _U(materials)).for_each(_U(material), Converter<Material>(description));
  xml_coll_t(compact, _U(materials)).for_each(_U(plugin),   Converter<Plugin> (description));

  printout(DEBUG, "Compact", "++ Converting visualization attributes...");
  xml_coll_t(compact, _U(display)).for_each(_U(include),    Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(display)).for_each(_U(vis),        Converter<VisAttr>(description));

  printout(DEBUG, "Compact", "++ Converting limitset structures...");
  xml_coll_t(compact, _U(limits)).for_each(_U(include),     Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(limits)).for_each(_U(limitset),    Converter<LimitSet>(description));

  printout(DEBUG, "Compact", "++ Converting region   structures...");
  xml_coll_t(compact, _U(regions)).for_each(_U(include),    Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(regions)).for_each(_U(region),     Converter<Region>(description));
  
  if ( world )  {
    (Converter<World>(description))(world);
  }
  if ( open_geometry ) description.init();
  printout(DEBUG, "Compact", "++ Converting readout  structures...");
  xml_coll_t(compact, _U(readouts)).for_each(_U(include), Converter<DetElementInclude>(description));
  xml_coll_t(compact, _U(readouts)).for_each(_U(readout), Converter<Readout>(description));

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
  if ( build_reflections )   {
    ReflectionBuilder rb(description);
    rb.execute();
  }
  /// Load plugin and process them as indicated
  xml_coll_t(compact, _U(plugins)).for_each(_U(plugin),  Converter<Plugin>  (description));
  xml_coll_t(compact, _U(plugins)).for_each(_U(include), Converter<XMLFile> (description));
  xml_coll_t(compact, _U(plugins)).for_each(_U(xml),     Converter<XMLFile> (description));
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
