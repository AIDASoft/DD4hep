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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBTags.h"
#include "DDDB/DDDBDimension.h"
#include "DDDB/DDDBHelper.h"
#include "DDDB/DDDBConversion.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Path.h"
//#include "DD4hep/DetConditions.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/InstanceCount.h"

#include "DDCond/ConditionsPool.h"

// ROOT include files
#include "TGeoManager.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <iomanip>
#include <set>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Keep all in here anonymous. Does not have to be visible outside.
  namespace  {

    struct DetElem  {};

    typedef Conditions::Interna::ConditionObject GeoCondition;
    typedef Conditions::ConditionsManager        ConditionsManager;
    typedef Conditions::ConditionsPool           ConditionsPool;
    typedef Conditions::AbstractMap              AbstractMap;
    typedef Geometry::PlacedVolume               GeoPlacement;
    typedef Geometry::Volume                     GeoVolume;
    typedef Geometry::Material                   GeoMaterial;
    typedef Geometry::Solid                      GeoSolid;

    /// Helper class to facilitate conversion. Purely local.
    struct Context  {

      typedef set<string> StringSet;

      Context(LCDD& l, dddb* g) : lcdd(l), geo(g)      {
      }
      ~Context()  {
        //printout(INFO,"Context","Destructor calling....");
      }
      template <typename T, typename Q> 
      static const Q find(const typename std::map<T,Q>& m, const T& match)    {
        typename std::map<T,Q>::const_iterator i = m.find(match);
        return (i != m.end()) ? (*i).second : 0;
      }
      template <typename T> void collect(const string& id, T* s);
      template <typename T,typename Q> void collect(const string& id, T* s, Q* c);
      LCDD&   lcdd;
      DDDB::dddb*       geo = 0;
      DDDB::DDDBHelper* helper = 0;
      typedef std::map<Isotope*,     TGeoIsotope*>   Isotopes;
      typedef std::map<Element*,     TGeoElement*>   Elements;
      typedef std::map<Material*,    TGeoMedium*>    Materials;
      typedef std::map<Shape*,       TGeoShape*>     Shapes;
      typedef std::map<LogVol*,      TGeoVolume*>    Volumes;
      typedef std::map<PhysVol*,     TGeoNode*>      Placements;
      typedef std::map<std::string,  DetElement>     DetectorMap;
      typedef std::map<std::string,  TGeoVolume*>    VolumeMap;
      typedef std::map<DetElement,   Catalog*>       DetectorElements;

      Isotopes          isotopes;
      Elements          elements;
      Materials         materials;
      Shapes            shapes;
      Volumes           volumes;
      VolumeMap         volumePaths;
      Placements        placements;
      DetElement        detectors;
      DetectorMap       catalogPaths;
      DetectorElements  detelements;
      GeoVolume         lvDummy;
      ConditionsManager manager;
      const IOVType*    epoch = 0;
      int               max_volume_depth = 9999;
      int               unmatched_deltas = 0;
      int               delta_conditions = 0;
      int               matched_conditions = 0;
      int               unmatched_conditions = 0;
      int               badassigned_conditions = 0;
      bool              print_materials = false;
      bool              print_volumes = false;
      bool              print_logvol = false;
      bool              print_shapes = false;
      bool              print_physvol = false;
      bool              print_params = false;
      bool              print_detelem = false;
      bool              print_conditions = false;
      bool              print_vis = false;
      bool              conditions_only = false;

      static GeoPlacement placement(DetElement de)   {
        if ( de.isValid() )  {
          GeoPlacement p = de.placement();
          if ( p.isValid() ) return p;
          return placement(de.parent());
        }
        return GeoPlacement(0);
      }
      static bool volumePlaced(GeoVolume par_vol, GeoVolume vol)  {
        if ( par_vol.isValid() && vol.isValid() && par_vol.ptr() == vol.ptr() )   {
          for(int i=0; i < vol->GetNdaughters(); ++i)  {
            TGeoNode* dau = vol->GetNode(i);
            TGeoVolume* v = dau->GetVolume();
            if ( v == vol.ptr() ) return true;
          }
        }
        return false;
      }

      TGeoNode* supportPlacement(TGeoVolume* parent, const string& npath)    {
        if ( !npath.empty() )   {
          size_t idx = npath.find('/');
          if ( idx == string::npos )  {
            for(int i=0; i<parent->GetNdaughters(); ++i)  {
              TGeoNode* dau = parent->GetNode(i);
              if ( npath == dau->GetTitle() )   {
                return dau;
              }
            }
            return 0;
          }
          TGeoNode* dau = supportPlacement(parent, npath.substr(0,idx));
          if ( dau )  {
            return supportPlacement(dau->GetVolume(), npath.substr(idx+1));
          }
        }
        return 0;
      }

      GeoPlacement supportPlacement(DetElement parent, const string& npath)    {
        GeoVolume par_vol = parent.volume();
        return par_vol.isValid() ? supportPlacement(par_vol.ptr(), npath) : 0;
      }
    };

    template <typename T> struct CNV : Converter<T,T*>  {
    public:
      /// Initializing constructor of the functor with initialization of the user parameter
      CNV(LCDD& l, void* p, void* o=0) : Converter<T,T*>(l,p,o) {}
      template<typename Q> CNV<Q> cnv() const {  return CNV<Q>(this->lcdd,this->param,this->optional);   }
      void* convert(T* obj) const;
      template<typename Q> Q get(const string& /* obj */) const  {
        throw runtime_error("NOT implemented virtual call !");
      }
      void operator()(T* obj) const   {	convert(obj);  }
      void operator()(const pair<string,T*>& arg) const  {
        Increment<T> incr;
        try  {
          if ( arg.second )  {
            operator()(arg.second);
            return;
          }
          printout(INFO,typeName(typeid(T)),"SKIP invalid object: %s",arg.first.c_str());
        }
        catch(const exception& e)  {
          printout(INFO,typeName(typeid(T)),"Failed to convert object: %s",e.what());
        }
        catch(...)   {
          printout(INFO,typeName(typeid(T)),"Failed to convert object.");
        }
      }
    };
  }

  namespace {
    template <> void* CNV<GeoCondition>::convert(GeoCondition *object) const;
    template <> void* CNV<Isotope>::convert(Isotope *object) const;
    template <> void* CNV<Element>::convert(Element *object) const;
    template <> void* CNV<Material>::convert(Material *object) const;
    template <> void* CNV<BooleanOperation>::convert(BooleanOperation *object) const;
    template <> void* CNV<Shape>::convert(Shape *object) const;
    template <> void* CNV<DetElem>::convert(DetElem *object) const;
    template <> void* CNV<LogVol>::convert(LogVol *object) const;
    template <> void* CNV<Catalog>::convert(Catalog *object) const;
    template <> void* CNV<dddb>::convert(dddb *obj) const;

    template <> template <> GeoMaterial CNV<Material>::get<GeoMaterial>(const string& material_name) const;
    template <> template <> LogVol*     CNV<LogVol>::get<LogVol*>(const string& obj) const;
    template <> template <> GeoVolume   CNV<LogVol>::get<GeoVolume>(const string& obj) const;

    /// Convert single condition objects
    template <> void* CNV<GeoCondition>::convert(GeoCondition *obj) const   {
      if ( obj )   {
        typedef IOV::Key _K;
        using Conditions::Condition;
        Context* context = _param<Context>();
        Condition cond = obj;
        AbstractMap&        d = cond.get<AbstractMap>();
        Document*         doc = d.option<Document>();
        _K::first_type  since = doc->context.valid_since;
        _K::second_type until = doc->context.valid_until;
        _K iov_key(since,until);
        auto e = context->helper->getConditionEntry(obj->value);
        /// We automatically convert here alignment deltas to the proper format...
        /// This will allow us to re-use all alignment utilities for DDDB.
        if ( obj->testFlag(Condition::ALIGNMENT_DELTA) )  {
          const pair<string,OpaqueDataBlock>& block = *(d.params.begin());
          Alignments::Delta delta = block.second.get<Alignments::Delta>();
          string typ = obj->type, add = obj->address, val = obj->value;
          int flg = obj->flags;
          // In-place replacement with Delta condition...
          obj->~ConditionObject();
          new(obj) Condition::Object(e.second,typ);
          cond.setFlag(flg);
          cond->address = add;
          cond->value   = val;
          cond.bind<Alignments::Delta>() = delta;
          ++context->delta_conditions;
          if ( !e.first.isValid() )
            ++context->unmatched_deltas;
        }
        if ( e.first.isValid() )  {
          cond->SetName(e.second.c_str());
          cond->hash = Conditions::ConditionKey::KeyMaker(e.first.key(),hash32(cond.name())).hash;
          printout(DEBUG,"DDDB","Insert condition: %s # %s --> %16llX",
                   e.first.path().c_str(),cond.name(),cond.key());
          printout(DEBUG,"DDDB","       %s -> %s",cond->value.c_str(),cond->address.c_str());
          ++context->matched_conditions;
        }
        else   {
          //printout(WARNING,"DDDB","Cannot match condition %s to detector element!",cond.name());
          //printout(WARNING,"DDDB","      %s -> %s",obj->value.c_str(),obj->address.c_str());
          ++context->unmatched_conditions;
        }
        ConditionsPool* pool = context->manager.registerIOV(*(context->epoch), iov_key);
        context->manager.registerUnlocked(*pool, cond);
        //context->manager.registerKey(cond->hash, cond->name);
      }
      return obj;
    }

    /// Convert single isotope objects
    template <> void* CNV<Isotope>::convert(Isotope *o) const    {
      Context* context = _param<Context>();
      TGeoIsotope* iso = Context::find(context->isotopes, o);
      if ( !iso )  {
        iso = TGeoIsotope::FindIsotope(o->c_name());
        if ( !iso ) iso = new TGeoIsotope(o->c_name(),o->Z,o->A,o->density);
        context->isotopes.insert(make_pair(o,iso));
        if ( context->print_materials ) dddb_print(iso);
      }
      return iso;
    }

    /// Convert single element objects
    TGeoElement* createElement(const char* nam, Context* context, Element *o)   {
      TGeoElementTable* t = TGeoElement::GetElementTable();
      TGeoElement* e = t->FindElement(nam);
      if ( !e )  {
        e = new TGeoElement(nam,o->symbol.c_str(),o->atom.Zeff,o->atom.A,o->density);
        /// Add the isotopes to the element
        for(auto i = o->isotopes.begin(); i != o->isotopes.end(); ++i)  {
          auto iso = context->geo->isotopes.find((*i).first);
          if ( iso == context->geo->isotopes.end() )  {
            printout(ERROR,"DDDB","++ Invalid isotope: %s [Ignore Isotope]",(*i).first.c_str());
            continue;
          }
          Isotope* isotope = (*iso).second;
          TGeoIsotope* geo_iso = 
            (TGeoIsotope*)CNV<Isotope>(context->lcdd,context).convert(isotope);
          if ( !geo_iso )  {
            printout(ERROR,"DDDB","++ Invalid isotope: %s [Ignore Isotope]",(*iso).first.c_str());
            continue;
          }
          e->AddIsotope(geo_iso, (*i).second);
        }
        t->AddElement(e);
        e = t->FindElement(nam);
        if ( !e ) {
          except("Cnv<Element>","Failed to insert Element: %s into table!", nam);
          return 0;
        }
        if ( context->print_materials ) dddb_print(e);
      }
      Material* material = Context::find(context->geo->materials,string(nam));
      if ( !material )  {
        TGeoManager& mgr = context->lcdd.manager();
        if ( !mgr.GetMaterial(nam) || !mgr.GetMedium(nam) )  {
          MaterialComponent comp;
          material          = new Material;
          material->name    = nam;
          material->density = o->density;
          comp.name         = nam;
          comp.fractionmass = 1.0;
          material->components.push_back(comp);
          context->geo->materials.insert(make_pair(nam,material));
          CNV<Material>(context->lcdd,context)(material);
        }
      }
      return e;
    }

    /// Convert single element objects and the counterpart using the full name
    template <> void* CNV<Element>::convert(Element *object) const    {
      Context* context = _param<Context>();
      TGeoElement* e = Context::find(context->elements, object);
      if ( !e )  {
        e = createElement(object->c_name(), context, object);
        context->elements.insert(make_pair(object,e));
        if ( !object->symbol.empty() && object->symbol != object->name )  {
          Element* esym = Context::find(context->geo->elements, object->symbol);
          if ( !esym )  {
            esym = new Element(*object);
            esym->name = object->symbol;
            context->geo->elements.insert(make_pair(object->symbol,esym));
          }
          TGeoElement* es = Context::find(context->elements, esym);
          if ( !es )  {
            es = createElement(esym->c_name(), context, esym);
            context->elements.insert(make_pair(esym, es));
          }
        }
      }
      return e;
    }

    /// Convert single material objects
    template <> void* CNV<Material>::convert(Material *object) const    {
      Context* context = _param<Context>();
      TGeoMedium* medium = Context::find(context->materials, object);
      if ( !medium )  {
        const char*      name = object->c_name();
        TGeoManager&      mgr = lcdd.manager();
        TGeoElementTable* tab = TGeoElement::GetElementTable();
        TGeoMaterial*     mat = mgr.GetMaterial(name);
        TGeoMixture*      mix = dynamic_cast<TGeoMixture*>(mat);

        medium = mgr.GetMedium(name);
        if ( 0 == mat )  {
          Material::Components& comp = object->components;
          //printout(INFO, "Cnv<Material>", "++ Converting material %s", name);
          mat = mix = new TGeoMixture(name, comp.size(), object->density);
          if ( object->radlen > 0 && object->lambda > 0 )
            mat->SetRadLen(object->radlen, object->lambda);
          else if ( object->radlen > 0 )
            mat->SetRadLen(object->radlen);
          if ( object->pressure > 0 )
            mat->SetPressure(object->pressure);
          if ( object->temperature > 0 )
            mat->SetTemperature(object->temperature);
          for(auto i = comp.begin(); i != comp.end(); ++i)  {
            MaterialComponent& c=(*i);
            TGeoElement* e = tab->FindElement(c.c_name());
            if ( e && c.natoms>0 )  {  // Add simple elements using atomic numbers
              mix->AddElement(e, c.natoms);
              continue;
            }
            else if ( e && c.fractionmass >= 0 )  { // Add other mixtures using fractional weights
              mix->AddElement(e, c.fractionmass);
              continue;
            }
            else if ( c.fractionmass >= 0 )  {
              GeoMaterial mm = this->get<GeoMaterial>(c.name);
              mix->AddElement(mm->GetMaterial(), c.fractionmass);
              continue;
            }
          }
        }
        // Now create the medium
        static UInt_t unique_mat_id = 0xAFFEFEED;
        if (0 == medium) {
          --unique_mat_id;
          medium = new TGeoMedium(name, unique_mat_id, mat);
          medium->SetTitle("material");
          medium->SetUniqueID(unique_mat_id);
        }
        context->materials.insert(make_pair(object,medium));
        if ( context->print_materials ) dddb_print(medium);
      }
      return medium;
    }

    /// Access/on the fly convert material objects by name
    template <> template <>
    GeoMaterial CNV<Material>::get<GeoMaterial>(const string& material_name)  const {
      Context* context = _param<Context>();
      Material* mat = Context::find(context->geo->materials,material_name);
      if ( !mat )  {
        string mat_name = material_name; // HACK! for bad material names in IT
        size_t idx;
        if ( mat_name.empty() )
          mat_name = "Air";
        else if ( (idx=mat_name.rfind('/')) != string::npos ) {
          mat_name = mat_name.substr(idx+1);
        }
        mat = Context::find(context->geo->materials,mat_name);
      }
      if ( !mat )  {
        printout(ERROR,"Cnv<Material>",
                 "++  Failed to find component material: %s "
                 "---> Matrial table dump.", material_name.c_str());
        for(auto im=context->geo->materials.begin(); im != context->geo->materials.end(); ++im)
          dddb_print((*im).second);
        except("Materials","++ Undefined material %s",material_name.c_str());
      }
      TGeoMedium* medium = (TGeoMedium*)CNV<Material>(context->lcdd,context).convert(mat);
      return GeoMaterial::handle_t(medium);
    }

    /// Convert shape entities
    template <> void* CNV<Shape>::convert(Shape *object) const    {
      Context* context = _param<Context>();
      GeoSolid shape = Context::find(context->shapes, object);
      if ( !shape )  {
        if ( object->id == "/dd/Geometry/LHCb/lvLHCb" )  {
          shape = lcdd.worldVolume().solid();
        }
        else if ( object->type == Assembly::type() )  {
          except("Cnv<Shape>","++ Assembly shapes cannot be converted!");
        }
        else if ( object->type == Box::type() )  {
          shape = Geometry::Box(object->s.box.x, object->s.box.y, object->s.box.z).ptr();
        }
        else if ( object->type == Cons::type() )  {
          const Cons& c = object->s.cons;
          shape = Geometry::Cone(c.sizeZ, 
                                 c.innerRadiusMZ, c.outerRadiusMZ,
                                 c.innerRadiusPZ, c.outerRadiusPZ).ptr();
        }
        else if ( object->type == ConeSegment::type() )  {
          const ConeSegment& o = object->s.coneSegment;
          shape = Geometry::ConeSegment(o.sizeZ, 
                                        o.innerRadiusMZ, o.outerRadiusMZ,
                                        o.innerRadiusPZ, o.outerRadiusPZ,
                                        o.start, o.delta).ptr();
        }
        else if ( object->type == EllipticalTube::type() )  {
          const EllipticalTube& o = object->s.ellipticalTube;
          shape = Geometry::EllipticalTube(o.a, o.b, o.dz).ptr();
        }
        else if ( object->type == Tubs::type() )  {
          const Tubs& t = object->s.tubs;
          shape = Geometry::Tube(t.innerRadius, t.outerRadius, t.sizeZ, t.start, t.delta);
        }
        else if ( object->type == Polycone::type() )  {
          Polycone& o = object->s.polycone;
          TGeoPcon* pc = new TGeoPcon(o.start, o.delta, object->zplanes.size());
          for(size_t iz=0; iz< object->zplanes.size(); ++iz)  {
            const ZPlane& plane = object->zplanes[iz];
            pc->DefineSection(iz, plane.z, plane.innerRadius, plane.outerRadius);
          }
          shape = pc;
        }
        else if ( object->type == Polygon::type() )  {
          Polygon& o = object->s.polygon;
          shape = Geometry::PolyhedraRegular(o.nsides, o.start, o.innerRadius, o.outerRadius, o.z);
        }
        else if ( object->type == Sphere::type() )  {
          const Sphere& o = object->s.sphere;
          shape = Geometry::Sphere(o.rmin, o.rmax, o.theta, o.theta+o.delta_theta, o.phi, o.phi+o.delta_phi);
        }
        //else if ( object->type == Ellipsoid::type() )  {
        //  const Ellipsoid& o = object->s.ellipsoid;
        //  shape = Geometry::Ellipsoid(o.rlow, o.rhigh, o.dz).ptr();
        //}
        else if ( object->type == Paraboloid::type() )  {
          const Paraboloid& o = object->s.paraboloid;
          shape = Geometry::Paraboloid(o.rlow, o.rhigh, o.dz).ptr();
        }
        else if ( object->type == Hyperboloid::type() )  {
          const Hyperboloid& o = object->s.hyperboloid;
          shape = Geometry::Hyperboloid(o.rmin, o.rmax, o.stIn, o.stOut, o.dz).ptr();
        }
        else if ( object->type == TRD::type() )  {
          const TRD& o = object->s.trd;
          shape = Geometry::Trapezoid(o.x1, o.x2, o.y1, o.y2, o.z).ptr();
        }
        else if ( object->type == Trap::type() )  {
          const Trap& o = object->s.trap;
          shape = Geometry::Trap(o.dz, o.theta, o.phi,
                                 o.h1, o.bl1, o.tl1, o.alpha1, 
                                 o.h2, o.bl2, o.tl2, o.alpha2);
        }
        else if ( object->type == BooleanUnion::type() ||
                  object->type == BooleanSubtraction::type() ||
                  object->type == BooleanIntersection::type() )   {
          shape = context->lvDummy.solid();
          Shape::Operations::const_iterator i;
          Shape*   left_shape = object->s.boolean.first;
          GeoSolid left_solid = (TGeoShape*)convert(left_shape);
          if ( !left_solid.isValid() )  { // Error ....
            except("Cnv<Shape>","++ %s: Unknown left boolean shape creation:%s -> %d",
                   object->c_name(), left_shape->c_name(), left_shape->type);
          }
          shape = left_solid;
          for(i=object->boolean_ops.begin(); i != object->boolean_ops.end(); ++i)  {
            Shape* right_shape = (*i).shape;
            GeoSolid right_solid = (TGeoShape*)convert(right_shape);
            const Transform3D& trafo = (*i).trafo;
            if ( !right_solid.isValid() )  { // Error ....
              except("Cnv<Shape>","++ %s: Unknown right boolean shape creation:%s -> %d",
                     object->c_name(), right_shape->c_name(), right_shape->type);
            }
            shape = GeoSolid();
            if ( object->type == BooleanUnion::type() )
              shape = Geometry::UnionSolid(left_solid, right_solid, trafo);
            else if ( object->type == BooleanSubtraction::type() )
              shape = Geometry::SubtractionSolid(left_solid, right_solid, trafo);
            else if ( object->type == BooleanIntersection::type() )
              shape = Geometry::IntersectionSolid(left_solid, right_solid, trafo);
            if ( !shape.isValid() )  {
              except("Cnv<Shape>","++ %s: Unknown boolean shape creation:%d",
                     object->c_name(), object->type);
            }
            left_solid = shape;
          }
        }
        if ( !shape )  {
          except("Cnv<Shape>","++ Undefined shape conversion %s [id:%d]",
                 object->c_id(), object->type);
        }
        shape->SetTitle(object->path.c_str());
        if ( context->print_shapes )  {
          printout(INFO,"Cnv<Shape>","++ Converted shape: %s",object->c_id());
        }
        context->shapes.insert(make_pair(object, shape));
      }
      return shape;
    }

    inline GeoPlacement place_daughter(const char* pv, GeoVolume mother, GeoVolume daughter, const Transform3D& tr)  {
      GeoPlacement place = mother.placeVolume(daughter, tr);
      // Use title for user defined physical volume name, since TGeo sets already the name!
      place->SetTitle(pv);
      return place;
    }

    inline void __check_physvol_instances__(int n)   {
      if ( n <= 0 )  {
        printout(WARNING,"Cnv<PhysVol>","++ Invalid replication constant in ParamPhysVolXD:%d",n);
      }
    }

    /// Convert logical volumes
    template <> void* CNV<LogVol>::convert(LogVol *object) const    {
      struct VolumeDepth {};
      Increment<VolumeDepth> depth;
      Context* context = _param<Context>();
      GeoVolume mother = Context::find(context->volumes, object);

      if ( !mother.isValid() )  {
        if ( depth.counter() >= context->max_volume_depth )   {
          mother = context->lvDummy;
          context->volumes.insert(make_pair(object, mother.ptr()));
          context->volumePaths[object->path] = mother.ptr();
          printout(WARNING,"Cnv<LogVol>","++ Ignore placements below level:%d -> %s",
                   depth.counter(), object->path.c_str());
          return mother;
        }
        Shape* shape = Context::find(context->geo->shapes,object->shape);
        if ( object->shape.empty() || shape->type == Assembly::type() )  {
          mother = Geometry::Assembly(object->name);
        }
        else  {
          GeoSolid s = (TGeoShape*)cnv<Shape>().convert(shape);
          if ( s.isValid() )   {
            GeoMaterial m = cnv<Material>().get<GeoMaterial>(object->material);
            mother = GeoVolume(object->name, s, m);
          }
        }
        mother->SetTitle(object->path.c_str());
        Geometry::VisAttr vis = context->helper->visAttr(object->path);
        if ( vis.isValid() )  {
          if ( context->print_vis )  {
            printout(INFO,"Cnv<LogVol>","++ Vol:%s  Vis:%s",mother->GetTitle(), vis.name());
          }
          mother.setVisAttributes(vis);
        }
        else   {
          mother->SetVisibility(kTRUE);
          mother->SetVisLeaves(kTRUE);
          mother->SetVisContainers(kTRUE);
          mother->SetFillColor(kRed);
        }
        context->volumes.insert(make_pair(object, mother.ptr()));
        context->volumePaths[object->path] = mother.ptr();
        // Now place all daughter volumes
        for(auto i=object->physvols.begin(); i!=object->physvols.end(); ++i)  {
          PhysVol* pv = *i;
          GeoPlacement place = context->find(context->placements, pv);
          if ( !place.isValid() )  {
            GeoVolume daughter = this->get<GeoVolume>(pv->logvol);
            if ( !daughter.isValid() )  {
              printout(WARNING,"Cnv<PhysVol>","++ Failed to convert placement: %s."
                       " Unknown daughter vol:%s.",pv->c_id(), pv->logvol.c_str());
              continue;
            }
            int num_places = 0;
            const char* pv_name = pv->name.c_str();
            switch(pv->type)   {
            case PhysVol::PHYSVOL_REGULAR:   {
              place = place_daughter(pv_name, mother, daughter, pv->trafo);
              num_places = 1;
              break;
            }
            case PhysVol::PHYSVOL_PARAM1D:   {
              ParamPhysVol* pDim = (ParamPhysVol*)pv;
              Transform3D   tr;
              Position      pos1, p1;
              RotationZYX   rot1, r1;
              pDim->trafo1.GetDecomposition(r1,p1);
              pDim->trafo.GetDecomposition(rot1,pos1);
              __check_physvol_instances__(pDim->number1);
              for(int k=0; k<pDim->number1; ++k)   {
                pos1 += p1;
                rot1 *= r1;
                tr    = Transform3D(rot1, pos1);
                place = place_daughter(pv_name, mother, daughter, tr);
                ++num_places;
              }
              break;
            }
            case PhysVol::PHYSVOL_PARAM2D:  {
              ParamPhysVol2D* pDim = (ParamPhysVol2D*)pv;
              Position        pos1, pos2, p1, p2;
              RotationZYX     rot1, rot2, r1, r2;
              Transform3D     tr;
              __check_physvol_instances__(pDim->number1);
              __check_physvol_instances__(pDim->number2);
              pDim->trafo1.GetDecomposition(r1, p1);
              pDim->trafo2.GetDecomposition(r2, p2);
              pDim->trafo.GetDecomposition(rot1,pos1);
              for(int k=0; k < pDim->number1; ++k)   {
                pos1 += p1;
                rot1 *= r1;
                pos2  = pos1;
                rot2  = rot1;
                for(int l=0; l < pDim->number2; ++l)   {
                  pos2 += p2;
                  rot2 *= r2;
                  tr    = Transform3D(rot2, pos2);
                  place = place_daughter(pv_name, mother, daughter, tr);
                  ++num_places;
                }
              }
              break;
            }
            case PhysVol::PHYSVOL_PARAM3D:  {
              ParamPhysVol3D* pDim = (ParamPhysVol3D*)pv;
              Position        pos1, pos2, pos3, p1, p2, p3;
              RotationZYX     rot1, rot2, rot3, r1, r2, r3;
              Transform3D     tr;
              __check_physvol_instances__(pDim->number1);
              __check_physvol_instances__(pDim->number2);
              __check_physvol_instances__(pDim->number3);
              pDim->trafo1.GetDecomposition(r1, p1);
              pDim->trafo2.GetDecomposition(r2, p2);
              pDim->trafo2.GetDecomposition(r3, p3);
              pDim->trafo.GetDecomposition(rot1,pos1);
              for(int k=0; k < pDim->number1; ++k)   {
                pos1 += p1;
                rot1 *= r1;
                pos2  = pos1;
                rot2  = rot1;
                for(int l=0; l < pDim->number2; ++l)   {
                  pos2 += p2;
                  rot2 *= r2;
                  pos3  = pos2;
                  rot3  = rot2;
                  for(int m=0; m < pDim->number3; ++m)   {
                    pos3 += p3;
                    rot3 *= r3;
                    tr    = Transform3D(rot3, pos3);
                    place = place_daughter(pv_name, mother, daughter, tr);
                    ++num_places;
                  }
                }
              }
              break;
            }
            default:
              printout(ERROR,"Cnv<PhysVol>","++ \tUnknown ParamPhysVol type: %d",pv->type);
              break;
            }
            context->placements.insert(make_pair(pv, place.ptr()));
            if ( context->print_physvol )    {
              Position pos;
              pv->trafo.GetTranslation(pos);
              printout(INFO,"Cnv<PhysVol>","++ Converted physVol: depth:%d typ:%d places:%d [%p lv:%p] %s",
                       depth.counter(), pv->type, num_places, (void*)place.ptr(), (void*)daughter.ptr(), 
                       mother->GetTitle());
              printout(INFO,"Cnv<PhysVol>","++ \tPosition: x=%f y=%f z=%f %p -> %s",
                       pos.X(), pos.Y(), pos.Z(), (void*)pv, pv->c_name());
            }
          }
        }
        if ( context->print_logvol )  {
          printout(INFO,"Cnv<LogVol>","++ Converted   logVol: [%p -> %p] %s NDau:%d",
                   (void*)object, (void*)mother.ptr(), object->path.c_str(),
                   mother->GetNdaughters());
        }
      }
      return mother.ptr();
    }

    /// Access logical volumes from string name
    template <> template <> 
    LogVol* CNV<LogVol>::get<LogVol*>(const string& nam) const    {
      Context* context = _param<Context>();
      LogVol* lv = Context::find(context->geo->volumes,nam);
      if ( lv )  {
        return lv;
      }
      lv = Context::find(context->geo->volumePaths,nam);
      if ( lv )  {
        return lv;
      }
      pair<const Catalog*,string> cat = context->geo->geometry->parent(nam);
      if ( cat.first )  {
        const Catalog* c = cat.first;
        dddb::Volumes::const_iterator iv = c->logvols.find(cat.second);
        if ( iv != c->logvols.end() )  {
          lv = (*iv).second;
          return lv;
        }
        for(iv = c->logvols.begin(); iv != c->logvols.end(); ++iv)
          printout(WARNING,"Cnv<LogVol>","++ %s --volume--> %s",
                   c->id.c_str(), (*iv).second->name.c_str());
      }
      printout(WARNING,"Cnv<LogVol>","++ Undefined logical volume: %s", nam.c_str());
      return 0;
    }

    /// Convert logical volumes from string
    template <> template <> 
    GeoVolume CNV<LogVol>::get<GeoVolume>(const string& nam) const    {
      LogVol* lv = this->get<LogVol*>(nam);
      if ( lv )  {
        return (TGeoVolume*)this->convert(lv);
      }
      return GeoVolume(0);
    }

    template <> void* CNV<Catalog>::convert(Catalog *object) const    {
      Context* context = _param<Context>();
      dddb*    geo     = context->geo;
      Context::DetectorMap::const_iterator j=context->catalogPaths.find(object->path);
      if ( j != context->catalogPaths.end() )  {
        return (*j).second.ptr();
      }
      GeoVolume  vol;
      Catalog*   support = 0;
      DetElement det(0), parent_element(0);
      if ( context->print_detelem )  {
        printout(INFO,"CNV<Catalog>","++ Starting catalog %p %s [cref:%d/%d lref:%d/%d lv:%s sup:%s np:%s] Cond:%s ",
                 (void*)object,
                 object->path.c_str(),
                 int(object->catalogrefs.size()),
                 int(object->catalogs.size()),
                 int(object->logvolrefs.size()),
                 int(object->logvols.size()),
                 object->logvol.empty()  ? "--" : object->logvol.c_str(),
                 object->support.empty() ? "--" : object->support.c_str(),
                 object->npath.empty()   ? "--" : object->npath.c_str(),
                 object->condition.c_str());
      }
      if ( object->path == "/dd/Structure" )  {
        const Box& o = geo->world;
        printout(WARNING,"World","World: %g %g %g",o.x,o.y,o.z);
        _toDictionary("world_x",_toString(o.x));
        _toDictionary("world_y",_toString(o.y));
        _toDictionary("world_z",_toString(o.z));
        lcdd.init();
        det = lcdd.world();
        vol = lcdd.worldVolume();
        context->detectors = det;
      }
      else if ( object->path.find("/dd/TrackfitGeometry") == 0 )  {
        return 0;
      }
      else if ( object->path.find("/dd/Geometry") == 0 )  {
        context->catalogPaths[object->path] = det;
      }
      else if ( object->path.find("/dd/Structure") == 0 )  {
        det = DetElement(object->name,object->type,0);
        det.addExtension<Catalog>(object->addRef());
        if ( !object->support.empty() )  {
          try  {
            j = context->catalogPaths.find(object->support);
            if ( j != context->catalogPaths.end() )  {
              parent_element = (*j).second;
              parent_element.add(det);
            }
            else  {
              dddb::Catalogs::const_iterator i=geo->catalogPaths.find(object->support);
              if ( i != geo->catalogPaths.end() )  {
                support = (*i).second;
                parent_element = (DetElement::Object*)this->convert(support);
                parent_element.add(det);
              }
              else  {
                pair<const Catalog*,string> cat = context->geo->structure->parent(object->support);
                if ( cat.first )  {
                  const Catalog* c = cat.first;
                  dddb::Catalogs::const_iterator icc = c->catalogs.find(cat.second);
                  if ( icc != c->catalogs.end() )  {
                    support = (*icc).second;
                    parent_element = (DetElement::Object*)this->convert(support);
                    parent_element.add(det);
                  }
                }
              }
            }
          }
          catch(const std::exception& e)   {
            printout(ERROR,"CNV<DetElem>","++ EXCEPTION: %s",e.what());
          }
        }
        if ( !parent_element.isValid() )   {
          printout(ERROR,"CNV<DetElem>","++ Unknown parent: %s",object->support.c_str());
        }
      }
      // Deal with true detector elements from /dd/Structure
      if ( det.isValid() )  { 
        if ( !object->logvol.empty() )  {
          CNV<LogVol> conv(lcdd,param);
          // Convert the logical volume, so that all daughters are present!
          vol = conv.get<GeoVolume>(object->logvol);
          //printout(INFO,"CNV<DetElem>","++ Set Placement: %s :  %s -> %s",
          //         object->path.c_str(), object->logvol.c_str(), object->npath.c_str());
          if ( !object->npath.empty() )  {
            GeoPlacement place = context->supportPlacement(parent_element, object->npath);
            if ( !place.isValid() )   {
              char txt[64];
              ::snprintf(txt,sizeof(txt),"++ %%%lus %%s %%s",object->path.length());
              printout(WARNING,"CNV<DetElem>",txt,
                       object->path.c_str(), "Placement: ", object->logvol.c_str());
              printout(WARNING,"CNV<DetElem>",txt,"    --> INVALID PLACEMENT...",
                       "Vol.N-path:", object->npath.c_str());
            }
            else   {
              det.setPlacement(place);
            }
          }
          else    {
            GeoPlacement par_place = context->placement(parent_element);
            GeoVolume    par_vol   = par_place.volume();
            if ( context->volumePlaced(par_vol, vol) )  {
              printout(WARNING,"CNV<DetElem>","++ %s : Volume already placed %s -> %s",
                       det.path().c_str(), par_vol->GetTitle(), vol->GetTitle());
            }
            GeoPlacement det_place = par_vol.placeVolume(vol);
            det.setPlacement(det_place);
          }
        }
        if ( !det.placement().isValid() )  {
          //printout(ERROR,"CNV<DetElem>","++  DetElement %s has no placement!",det.path().c_str());
        }
      }
      if ( det.isValid() )  {
        context->detelements[det] = object;
        context->catalogPaths[object->path] = det;
      }
      for_each(object->logvolrefs.begin(), object->logvolrefs.end(), CNV<LogVol>(lcdd,param,det.ptr()));
      for_each(object->catalogrefs.begin(), object->catalogrefs.end(), *this);

#if 0
      for(Catalog::LvRefs::const_iterator i=object->logvolrefs.begin(); i!=object->logvolrefs.end(); ++i)   {
        LogVol* lv = (*i).second;
        string  lp = object->path+"/"+lv->name;
        GeoVolume gv = Context::find(context->volumes, lv);
        context->volumePaths[lp] = gv.ptr();
      }
#endif
      /// Attach conditions keys to the detector element if present
      if ( !object->condition.empty() )   {
        bool res;
        char text[64];
        ::snprintf(text,sizeof(text),"              %%%lus -> %%s",object->condition.length());
        printout(DEBUG,"DDDB","+ Match Align %s -> %s",object->condition.c_str(), object->path.c_str());
        printout(DEBUG,"DDDB",text,"",det.path().c_str());
        res = context->helper->addConditionEntry(object->condition,det,"alignment_delta");
        if ( !res )  {
          printout(DEBUG,"DDDB","++ Conditions entry with key:%s already exists!",
                   object->condition.c_str());
          ++context->badassigned_conditions;
        }
      }
      if ( !object->conditioninfo.empty() )  {
        bool res;
        for( const auto& i : object->conditioninfo )   {
          printout(DEBUG,"DDDB","+ Match Cond  %s -> %s",i.second.c_str(), object->path.c_str());
          res = context->helper->addConditionEntry(i.second, det, i.first);
          if ( !res )  {
            printout(DEBUG,"DDDB","++ Conditions entry with key:%s already exists!",
                     i.second.c_str());
            ++context->badassigned_conditions;
          }
        }
      }
      if ( context->print_detelem )  {
        printout(INFO,"CNV<Catalog>","++ Converting catalog %p -> %p [cref:%d/%d lref:%d/%d lv:%s [%p] sup:%s np:%s] %s ",
                 (void*)object, det.ptr(),
                 int(object->catalogrefs.size()),
                 int(object->catalogs.size()),
                 int(object->logvolrefs.size()),
                 int(object->logvols.size()),
                 object->logvol.empty()  ? "--" : object->logvol.c_str(),
                 vol.ptr(),
                 object->support.empty() ? "--" : object->support.c_str(),
                 object->npath.empty()   ? "--" : object->npath.c_str(),
                 object->path.c_str());
        int cnt = 0;
        for( const auto& v : object->catalogrefs )  {
          if ( v.second )
            printout(INFO,"CNV<DE>:cref","++ DE:%s ref[%2d]: %p -> %s",
                     object->path.c_str(), cnt, v.second, v.second->c_name());
          else
            printout(INFO,"CNV<DE>:cref","++ DE:%s ref[%2d]: ??????", object->path.c_str(), cnt);
          ++cnt;
        }
        cnt = 0;
        for( const auto& v : object->logvolrefs )  {
          LogVol* lv = v.second;
          GeoVolume geoVol = Context::find(context->volumes, lv);
          if ( lv )
            printout(INFO,"CNV<DE>:lref","++ DE:%s ref[%2d]: %p / %s  -> %p [%s]",
                     object->path.c_str(), cnt, (void*)lv, lv->c_name(), 
                     (void*)geoVol.ptr(), geoVol.isValid() ? geoVol->GetName() : "????");
          else
            printout(INFO,"CNV<DE>:lref","++ DE:%s ref[%2d]: ??????", object->path.c_str(), cnt);
          ++cnt;
        }
        cnt = 0;
        for( const auto& v : object->logvols )  {
          LogVol* lv = v.second;
          if ( lv )  {
            GeoVolume geoVol = Context::find(context->volumes, lv);
            printout(INFO,"CNV<DE>:lvol","++ DE:%s ref[%2d]: %p / %s  -> %p [%s]",
                     object->path.c_str(), cnt, (void*)lv, lv->c_id(), 
                     (void*)geoVol.ptr(), geoVol.isValid() ? geoVol->GetName() : "????");
          }
          else
            printout(INFO,"CNV<DE>:lvol","++ DE:%s ref[%2d]: ??????", object->path.c_str(), cnt);
          ++cnt;
        }
        if ( vol && !object->npath.empty() )   {
          for(int i=0; i<vol->GetNdaughters(); ++i)  {
            TGeoNode* dau = vol->GetNode(i);
            printout(INFO,"CNV<DE>:npath","++ DE:%s npath:%s Dau[%2d]: %s",
                     object->path.c_str(), object->npath.c_str(),
                     i, dau->GetName());
          }
        }
      }
      return det.ptr();	
    }

    template <> void* CNV<dddb>::convert(dddb *obj) const   {
      Context*  context = _param<Context>();
      if ( !context->conditions_only )  {
        GeoVolume world   = lcdd.worldVolume();

        for_each(obj->isotopes.begin(),  obj->isotopes.end(),   cnv<Isotope>());
        printout(INFO,"DDDB2Object","++ Converted %d isotopes.",int(obj->isotopes.size()));
        for_each(obj->elements.begin(),  obj->elements.end(),   cnv<Element>());
        printout(INFO,"DDDB2Object","++ Converted %d elements.",int(obj->elements.size()));
        //for_each(obj->materials.begin(), obj->materials.end(),  cnv<Material>());
        //printout(INFO,"DDDB2Object","++ Converted %d materials.",int(obj->materials.size()));
        //for_each(obj->shapes.begin(),    obj->shapes.end(),     cnv<Shape>());
        //printout(INFO,"DDDB2Object","++ Converted %d shapes.",int(obj->shapes.size()));
        //for_each(obj->volumes.begin(),   obj->volumes.end(),    cnv<LogVol>());
        //printout(INFO,"DDDB2Object","++ Converted %d volumes.",int(obj->volumes.size()));
        //for_each(obj->placements.begin(),obj->placements.end(), cnv<PhysVol>());
        //printout(INFO,"DDDB2Object","++ Converted %d placements.",int(obj->placements.size()));

        if ( obj->top )   {
          if ( !context->lvDummy.isValid() )   {
            lcdd.manager().SetVisLevel(context->max_volume_depth);
            context->lvDummy = GeoVolume("Dummy",Geometry::Box(0.0001,0.0001, 0.0001),lcdd.vacuum());
            context->lvDummy.setVisAttributes(lcdd.invisible());
          }
          if ( !world.isValid() )  {
            string top = "/dd/Geometry/LHCb/lvLHCb";
            const LogVol* lv = Context::find(obj->volumePaths,top);
            if ( !lv )   {
              except("DDDB2DD4hep","++ No World volume defined.");
            }
            const Shape* s = Context::find(obj->shapes,lv->id);
            if ( !s )  {
              except("DDDB2DD4hep","++ No Shape for the world volume defined.");
            }
            obj->world = s->s.box;
          }
          /// Main detector conversion invokation
          cnv<Catalog>().convert(obj->top);
          if ( !world.isValid() && lcdd.worldVolume().isValid() )  {
            lcdd.endDocument();
          }
          /// Now configure the conditions manager
          if ( !context->manager.isValid() )  {
            Conditions::ConditionsManager manager = Conditions::ConditionsManager::from(lcdd);
            manager["PoolType"]       = "DD4hep_ConditionsLinearPool";
            manager["LoaderType"]     = "dddb";
            manager["UserPoolType"]   = "DD4hep_ConditionsMapUserPool";
            manager["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
            manager.initialize();
            pair<bool,const IOVType*> e = manager.registerIOVType(0, "epoch");
            context->manager = manager;
            context->epoch   = e.second;
          }
        }
      }
      if ( !context->manager.isValid() )  {
        Conditions::ConditionsManager manager = Conditions::ConditionsManager::from(lcdd);
        pair<bool,const IOVType*> e = manager.registerIOVType(0, "epoch");
        context->manager = manager;
        context->epoch   = e.second;
      }
      for_each(obj->conditions.begin(),obj->conditions.end(), cnv<GeoCondition>());
      //printout(INFO,"DDDB2Object","++ Converted %d conditions.",int(obj->conditions.size()));
      return obj;
    }
  }

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace DDDB  {
    long dddb_2_dd4hep(LCDD& lcdd, int , char** ) {
      DDDBHelper* helper = lcdd.extension<DDDBHelper>(false);
      if ( helper )   {
        Context context(lcdd, helper->detectorDescription());
        context.helper              = helper;
        context.print_materials     = false;
        context.print_logvol        = false;
        context.print_shapes        = false;
        context.print_physvol       = false;
        context.print_volumes       = false;
        context.print_params        = false;
        context.print_detelem       = false;
        context.print_conditions    = false;
        context.print_vis           = false;
        context.max_volume_depth    = 11;

        CNV<dddb> cnv(lcdd,&context);
        cnv(make_pair(string("World"),context.geo));
        printout(INFO,"DDDB","+========================= Conversion summary =========================+");
        printout(INFO,"DDDB","++ Converted %8d isotopes.",         int(context.isotopes.size()));
        printout(INFO,"DDDB","++ Converted %8d elements.",         int(context.elements.size()));
        printout(INFO,"DDDB","++ Converted %8d materials.",        int(context.materials.size()));
        printout(INFO,"DDDB","++ Converted %8d shapes.",           int(context.shapes.size()));
        printout(INFO,"DDDB","++ Converted %8d logical  volumes.", int(context.volumes.size()));
        printout(INFO,"DDDB","++ Converted %8d placements.",       int(context.placements.size()));
        printout(INFO,"DDDB","++ Converted %8d detector elements.",int(context.detelements.size()));
        printout(INFO,"DDDB","++ Converted %8d conditions.",
                 context.geo ? int(context.geo->conditions.size()) : 0);
        printout(INFO,"DDDB","++ MATCHED   %8d conditions.",       context.matched_conditions);
        printout(INFO,"DDDB","++ UNMATCHED %8d conditions. [Could not determine DetElement]",
                 context.unmatched_conditions);
        printout(INFO,"DDDB","++ BADASSIGN %8d conditions. [Could not determine DetElement]",
                 context.badassigned_conditions);
        printout(INFO,"DDDB","++ DELTAS    %8d conditions.",       context.delta_conditions);
        printout(INFO,"DDDB","++ DELTAS    %8d (unmatched).",      context.unmatched_deltas);
        printout(INFO,"DDDB","+======================================================================+");
        helper->setDetectorDescription(0);
        return 1;
      }
      except("DDDB","++ No DDDBHelper instance installed. Geometry conversion failed!");
      return 1;
    }
    long dddb_conditions_2_dd4hep(LCDD& lcdd, int , char** ) {
      DDDBHelper* helper = lcdd.extension<DDDBHelper>(false);
      if ( helper )   {
        Context context(lcdd, helper->detectorDescription());
        context.helper              = helper;
        context.print_conditions    = false;
        context.conditions_only     = true;
        CNV<dddb> cnv(lcdd,&context);
        cnv(make_pair(string(),context.geo));
        helper->setDetectorDescription(0);
        return 1;
      }
      except("DDDB","++ No DDDBHelper instance installed. Geometry conversion failed!");
      return 1;
    }

  } /* End namespace DDDB      */
} /* End namespace DD4hep    */
DECLARE_APPLY(DDDB_2DD4hep,dddb_2_dd4hep)
DECLARE_APPLY(DDDB_Conditions2DD4hep,dddb_conditions_2_dd4hep)
