// $Id$
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
#include "DDDB/Dimension.h"
#include "DDDB/DDDBHelper.h"
#include "DDDB/DDDBConversion.h"

// C/C++ include files
#include "boost/filesystem/path.hpp"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Keep all in here anonymous. Does not have to be visible outside.
  namespace {

    /// Main processing context
    /**   \ingroup DD4HEP_DDDB
     */
    struct Context  {

    private:
      template <typename T,typename Q> 
      void collect_id(Q& container, const string& id, T* object)  {
        typename Q::const_iterator i=container.find(id);
        if ( i != container.end() )  {
          if ( object == (*i).second ) return;
          printout(ERROR,"collect","++ Duplicate ID: %s  %p <-> %p",
                   id.c_str(), object, (*i).second);
        }
        object->id = id;
        container[id] = object->addRef();
        print(object);
      }
      template <typename T,typename Q> 
      void collect_p(Q& container, const string& path, T* object) {
        typename Q::const_iterator i=container.find(path);
        if ( i != container.end() )  {
          if ( object == (*i).second ) return;
          printout(ERROR,"collectPath","++ Duplicate ID: %s  %p <-> %p",
                   path.c_str(), object, (*i).second);
        }
        container[path] = object->addRef();
      }

    public:
      typedef set<string> StringSet;

      /// Local processing environment
      /**   \ingroup DD4HEP_DDDB
       */
      struct Locals  {
	string doc_path, obj_path, xml_path;
	Locals() {}
	Locals(const Locals& c) : doc_path(c.doc_path), obj_path(c.obj_path), xml_path(c.xml_path) {}
	Locals& operator=(const Locals& c)  {
	  doc_path = c.doc_path;
	  obj_path = c.obj_path;
	  xml_path = c.xml_path;
	  return *this;
	}
      };
      /// Helper class to preserve local processing environment
      /**   \ingroup DD4HEP_DDDB
       */
      struct PreservedLocals : public Locals {
	Context* context;
	PreservedLocals(Context* c) : Locals(c->locals), context(c) {}
	~PreservedLocals()            { context->locals = *this; }
      };

      lcdd_t&     lcdd;
      DDDBHelper* hlp;
      dddb*       geo;
      StringSet   files;
      Locals      locals;
      bool print_xml;
      bool print_docs;
      bool print_materials;
      bool print_logvol;
      bool print_shapes;
      bool print_physvol;
      bool print_params;
      bool print_detelem;
      bool print_detelem_ref;
      bool print_detelem_xml;
      bool print_condition;
      bool print_condition_ref;
      bool print_catalog;
      bool print_catalog_ref;

      /// Default constructor
      Context(lcdd_t& l) : lcdd(l), hlp(0), geo(0),
			   print_xml(false),
			   print_docs(false),
			   print_materials(false), 
			   print_logvol(false), 
			   print_shapes(false), 
			   print_physvol(false), 
			   print_params(false), 
			   print_detelem(false),
			   print_detelem_ref(false), 
			   print_detelem_xml(false),
			   print_condition(false), 
			   print_condition_ref(false), 
			   print_catalog(false),
			   print_catalog_ref(false)
      {     }
      /// Default destructor
      ~Context()  {      }
      /// Printout helpers
      void print(const Isotope* obj)   const               { if ( print_materials ) dddb_print(obj);    }
      void print(const Element* obj)   const               { if ( print_materials ) dddb_print(obj);    }
      void print(const Material* obj)  const               { if ( print_materials ) dddb_print(obj);    }
      void print(const Shape* obj)     const               { if ( print_shapes )    dddb_print(obj);    }
      void print(const PhysVol* obj)   const               { if ( print_physvol )   dddb_print(obj);    }
      void print(const LogVol* obj)    const               { if ( print_logvol )    dddb_print(obj);    }
      void print(const Catalog* obj)   const               { if ( print_catalog )   dddb_print(obj);    }
      void print(const Condition* obj) const               { if ( print_condition ) dddb_print(obj);    }
      /// Data collection helpers for indexing by object identifier
      void collect(const string& id, Catalog* obj)         { collect_id(geo->catalogs, id, obj);        }
      void collect(const string& id, Shape* obj)           { collect_id(geo->shapes, id, obj);          }
      void collect(const string& id, PhysVol* obj)         { collect_id(geo->placements, id, obj);      }
      void collect(const string& id, LogVol* obj)          { collect_id(geo->volumes, id, obj);         }
      void collect(const string& id, Isotope* obj)         { collect_id(geo->isotopes, id, obj);        }
      void collect(const string& id, Element* obj)         { collect_id(geo->elements, id, obj);        }
      void collect(const string& id, Material* obj)        { collect_id(geo->materials, id, obj);       }
      void collect(const string& id, Condition* obj)       { collect_id(geo->conditions, id, obj);      }
      /// Data collection helpers for indexing by path
      void collectPath(const string& path, Element*   obj) { collect_p(geo->elementPaths,   path, obj); }
      void collectPath(const string& path, Material*  obj) { collect_p(geo->materialPaths,  path, obj); }
      void collectPath(const string& path, PhysVol*   obj) { collect_p(geo->placementPaths, path, obj); }
      void collectPath(const string& path, LogVol*    obj) { collect_p(geo->volumePaths,    path, obj); }
      void collectPath(const string& path, Catalog*   obj) { collect_p(geo->catalogPaths,   path, obj); }
      void collectPath(const string& path, Condition* obj) { collect_p(geo->conditionPaths, path, obj); }
    };

    /// Helper class to count call depths
    /**   \ingroup DD4HEP_DDDB
     */
    template <typename T> struct Increment {
      static int& counter() { static int cnt=0; return cnt; }
      Increment()   { ++counter(); }
      ~Increment()  { --counter(); }
    };

    /// Converter to incept processing exceptions
    /**   \ingroup DD4HEP_DDDB
     */
    template <typename T> struct Conv : protected Converter<T> {
    public:
      /// Initializing constructor of the functor with initialization of the user parameter
      Conv(lcdd_t& l, void* p, void* o=0) : Converter<T>(l,p,o) {}
      void convert(XML::Handle_t element) const;
      void fill(XML::Handle_t, T*) const {}
      void operator()(XML::Handle_t element) const  {
        Increment<T> incr;
        try  {
          convert(element);
        }
        catch(const exception& e)  {
          printout(INFO,typeName(typeid(T)),"Failed to convert XML object: %s", e.what());
          XML::dump_tree(element.parent());
        }
        catch(...)   {
          printout(INFO,typeName(typeid(T)),"Failed to convert XML object.");
          XML::dump_tree(element.parent());
        }
      }
    };

    /// Special shape converter with inception of processing exceptions
    /**   \ingroup DD4HEP_DDDB
     */
    template <typename T> struct ShapeConv : private Converter<T> {
    public:
      /// Initializing constructor of the functor with initialization of the user parameter
      ShapeConv(lcdd_t& l, void* p) : Converter<T>(l,p,0) {}
      void operator()(XML::Handle_t element, Shape*& ptr_shape) const  {
        Increment<T> incr;
        try  {
          convert(element, ptr_shape);
        }
        catch(const exception& e)  {
          printout(INFO,typeName(typeid(T)),"Failed to convert object: %s",e.what());
          XML::dump_tree(element.parent());
        }
        catch(...)   {
          printout(INFO,typeName(typeid(T)),"Failed to convert object.");
          XML::dump_tree(element.parent());
        }
      }
      void convert(XML::Handle_t element, Shape*& ptr_shape) const;
    };

    /// Explicit template instantiations
    template <> void ShapeConv<Shape>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Assembly>::convert(xml_h , Shape*& s) const;
    template <> void ShapeConv<Box>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Cons>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<ConeSegment>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Tubs>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Trap>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Polycone>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Polygon>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<EllipticalTube>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<TRD>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<Sphere>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<BooleanShape>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<BooleanUnion>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<BooleanIntersection>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<BooleanSubtraction>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<BooleanSubtraction>::convert(xml_h element, Shape*& s) const;
    template <> void ShapeConv<BooleanOperation>::convert(xml_h element, Shape*& ptr_shape) const;

    template <> void Conv<ZPlane>::convert(xml_h element) const;
    template <> void Conv<dddb>::convert(xml_h element) const;
    template <> void Conv<LogVol>::convert(xml_h element) const;
    template <> void Conv<LogVolRef>::convert(xml_h element) const;
    template <> void Conv<PhysVol>::convert(xml_h element) const;
    template <> void Conv<ParamPhysVol>::convert(xml_h element) const;
    template <> void Conv<ParamPhysVol2D>::convert(xml_h element) const;
    template <> void Conv<ParamPhysVol3D>::convert(xml_h element) const;
    template <> void Conv<ConditionParam>::convert(xml_h element) const;

    void extract_transformation(lcdd_t& lcdd, void* context, xml_coll_t& collection, Transform3D& tr, int which=-1);
    void build_transformation(lcdd_t& lcdd, void* context, xml_h element, Transform3D& tr, int which=-1)   {
      xml_coll_t p(element,_U(star));
      extract_transformation(lcdd,context,p,tr,which);
    }
    string reference_href(xml_h element, const string& ref);

    string object_path(Context* context, const string& ref)  {
      size_t hash = ref.rfind("#");
      boost::filesystem::path path = hash==0 ? ref.substr(1) : ref;
      if ( ref[0] != '/' )  {
        path = context->locals.obj_path;
        path /= (hash==0 ? ref.substr(1) : ref.substr(hash+1));
      }
      return path.normalize().native();
    }
    string object_href(xml_h element, const string& ref)   {
      string p = XML::DocumentHandler::system_path(element);
      boost::filesystem::path path = p;
      p = path.normalize().native();
      p += '#';
      p += ref;
      return p;
    }
    string reference_path(Context* context, const string& ref)   {
      size_t hash = ref.rfind("#");
      size_t idx  = ref.find(":");
      size_t idq  = ref.find("/");
      boost::filesystem::path path = hash==0 ? ref.substr(1) : ref;
      if ( (idx == string::npos || idq < idx) && ref[0] != '/' )  {
        path = context->locals.doc_path;
        if ( hash != 0 ) path = path.parent_path();
        path /= ref.substr(0,hash);
      }
      return path.normalize().native();
    }
    void print_ref(const char* desc, Context* context, xml_h element, const string& ref, const string& opt="")  {
      size_t hash = ref.find("#");
      string path = reference_path(context,ref);
      string obj  = hash == string::npos ? ref : ref.substr(hash+1);
      string id1  = object_path(context,ref);
      string id2  = reference_href(element,ref);
      printout(INFO, desc, "** %s --> %s  path: %s # %s  %s", 
               id1.c_str(), id2.c_str(), path.c_str(), obj.c_str(), opt.c_str());
    }

    void load_dddb_entity(Context*        context,
			  Catalog*        catalog,
			  xml_h           element,
			  const string&   ref,
			  bool            print=false);

    /// Helper to locate objects in a map using string identifiers
    template <typename Q> bool find(const string& id, const Q& container)  {
      return container.find(id) != container.end();
    }

    bool checkParents(Context* context, Catalog* det)  {
      dddb* geo = context->geo;
      if ( det == geo->top )  {
        return true;
      }
      else if ( det )   {
        Catalog* par = 0;
        string parent_id  = det->support;
        dddb::Catalogs::const_iterator k = geo->catalogPaths.find(parent_id);
        if ( k != geo->catalogPaths.end() )  {
          par = (*k).second;
          par->catalogrefs[det->id] = det;
          return checkParents(context, par);
        }
        boost::filesystem::path p(det->path);
        parent_id  = p.parent_path().native();
        if ( parent_id == "/" )  {
          return true;
        }
        k = geo->catalogPaths.find(parent_id);
        if ( k != geo->catalogPaths.end() )  {
          par = (*k).second;
          par->catalogrefs[det->id] = det;
          return checkParents(context, par);
        }
        printout(ERROR,"checkParents", "++ %s: NO PARENT Detector: '%s'!",
                 det->path.c_str(), parent_id.c_str());
      }
      return false;
    }

    void checkParents(Context* context)  {
      dddb* geo = context->geo;
      for(dddb::Catalogs::iterator i=geo->catalogs.begin(); i!=geo->catalogs.end(); ++i)  {
        Catalog* det = (*i).second;
        checkParents(context,det);
      }
    }

    void fixCatalogs(Context* context)  {
      dddb* geo = context->geo;
      for(dddb::Catalogs::iterator i=geo->catalogs.begin(); i!=geo->catalogs.end(); ++i)  {
        Catalog* det = (*i).second;
        for(dddb::Catalogs::iterator j=det->catalogrefs.begin(); j!=det->catalogrefs.end(); ++j)  {
          const string& child_id = (*j).first;
          dddb::Catalogs::const_iterator k = geo->catalogs.find(child_id);
          if ( k == geo->catalogs.end() )   {
            printout(ERROR,"fixCatalogs","++  MISSING ID: %s child:%s",det->id.c_str(),child_id.c_str());
            continue;
          }
          Catalog* c = (*k).second;
          det->catalogs[c->name] = c;
          if ( 0 == (*j).second )  {
            (*j).second = c;
          }
        }
        for(dddb::Volumes::iterator j=det->logvolrefs.begin(); j!=det->logvolrefs.end(); ++j)  {
          LogVol* c = (*j).second;
          if ( !c )  {
            printout(ERROR,"fixCatalogs","++  MISSING Volume: %s child:%s",det->id.c_str(),c->id.c_str());
            continue;
          }
          dddb::Volumes::const_iterator k = geo->volumes.find(c->id);
          if ( k == geo->volumes.end() )   {
            printout(ERROR,"fixCatalogs","++  MISSING VolID: %s child:%s",det->id.c_str(),c->id.c_str());
          }
          det->logvols[c->name] = c;
          if ( 0 == (*j).second )  {
            (*j).second = c;
          }
        }
      }
    }

    /// Specialized conversion of <author/> entities
    template <> void Conv<Author>::convert(xml_h element) const {
      string* context = _option<string>();
      if ( element.hasAttr(_U(author)) ) *context = element.attr<string>(_U(author));
    }

    /// Specialized conversion of <version/> entities
    template <> void Conv<Version>::convert(xml_h element) const {
      string* context = _option<string>();
      if ( element.hasAttr(_U(version)) ) *context = element.attr<string>(_U(version));
    }

    /// Specialized conversion of <param/> entities
    template <> void Conv<Param>::convert(xml_h element) const {
      Catalog* det   = _option<Catalog>();
      string   name  = element.attr<string>(_U(name));
      string   type  = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("int");
      string   value = element.text();
      det->params[name] = make_pair(type,value);
    }

    /// Specialized conversion of <ConditionInfo/> entities
    template <> void Conv<ConditionInfo>::convert(xml_h element) const {
      Catalog* det  = _option<Catalog>();
      string   name = element.attr<string>(_U(name));
      string   cond = element.attr<string>(_LBU(condition));
      det->conditioninfo[name] = cond;
    }

    /// Specialized conversion of <param/> and <paramVector> entities
    template <> void Conv<ConditionParam>::convert(xml_h element) const {
      Condition::Params*  c = _option<Condition::Params>();
      string            n = element.attr<string>(_U(name));
      ConditionParam* p = new ConditionParam();
      p->type = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("int");
      p->data = element.text();
      c->insert(make_pair(n,p));
    }

    /// Specialized conversion of <condition/> entities
    template <> void Conv<Condition>::convert(xml_h element) const {
      Context*   context = _param<Context>();
      Catalog*   catalog = _option<Catalog>();
      string     name    = element.attr<string>(_U(name));
      string     id      = object_href(element,name);
      string     path    = object_path(context,name);
      Condition* cond    = new Condition();
      static int num_param = 0, num_paramVector = 0;

      cond->id   = id;
      cond->name = name;
      cond->path = path;
      cond->classID = element.attr<string>(_LBU(classID));
      xml_coll_t(element,_U(param)).for_each(Conv<ConditionParam>(lcdd,context,&cond->params));
      xml_coll_t(element,_LBU(paramVector)).for_each(Conv<ConditionParam>(lcdd,context,&cond->paramVectors));
      context->collect(cond->id, cond);
      if ( catalog )  {
        context->collectPath(cond->path, cond);
      }
      num_param += int(cond->params.size());
      num_paramVector += int(cond->paramVectors.size());
      if ( (context->geo->conditions.size()%500) == 0 )  {
        printout(INFO,"Condition","++ Processed %d conditions....last:%s Number of Params: Scalar: %d Vectors: %d", 
                 int(context->geo->conditions.size()), path.c_str(),
                 num_param, num_paramVector);
      }
    }

    /// Specialized conversion of <conditionref/> entities
    template <> void Conv<ConditionRef>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = _option<Catalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);
      string      path;

      load_dddb_entity(context, catalog, element, href);
      dddb::Conditions::const_iterator i=context->geo->conditions.find(refid);
      if ( i == context->geo->conditions.end() )  {
        printout(ERROR,"ConditionRef","++  MISSING ID: %s Failed to convert ref:%s cat:%s",
		 refid.c_str(),path.c_str(), catalog ? catalog->path.c_str() : "???");
	if ( context->print_condition_ref )  {
	  print_ref("ConditionRef", context, element, href, "Path:----");
	}
	return;
      }
      Condition* cond = (*i).second;
      path = object_path(context,cond->name);
      context->collectPath(path, cond);
      if ( context->print_condition_ref )  {
        print_ref("ConditionRef", context, element, href, "Path:"+path);
      }
    }

    /// Specialized conversion of <isotope/> entities
    template <> void Conv<Isotope>::convert(xml_h element) const {
      Context* context = _param<Context>();
      dddb_dim_t x_i = element;
      string name = x_i.nameStr();
      string id = object_path(context,name);
      if ( !find(id, context->geo->isotopes) )  {
        Isotope* i = new Isotope();
        i->name       = name;
        i->A          = x_i.A(-1.0);
        i->Z          = x_i.Z(-1.0);
        i->density    = x_i.density(-1.0);
        context->collect(id, i);
      }
    }

    /// Specialized conversion of <elementref/> entities
    template <> void Conv<ElementRef>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = _option<Catalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);
      load_dddb_entity(context, catalog, element, href);
      dddb::Elements::const_iterator i=context->geo->elements.find(refid);
      if ( i == context->geo->elements.end() )  {
        printout(ERROR,"ElementRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
      }
      Element* e = (*i).second;
      string path = object_path(context,e->name);
      context->collectPath(path, e);
    }

    /// Specialized conversion of <element/> entities
    template <> void Conv<Element>::convert(xml_h element) const {
      Context* context = _param<Context>();
      dddb_dim_t x_elem = element;
      string       name = x_elem.nameStr();
      string         id = object_href(element, name);
      if ( !find(id, context->geo->elements) )  {
        Element* e = new Element();
        dddb_dim_t atom = x_elem.child(_U(atom),false);
        e->id         = id;
        e->name       = name;
        e->path       = object_path(context,name);
        e->density    = x_elem.density(-1.0);
        e->ionization = x_elem.I(-1.0);
        e->symbol     = x_elem.symbol(e->name);
        e->atom.A     = atom ? atom.attr<double>(_U(A)) : -1.0;
        e->atom.Zeff  = atom ? atom.attr<double>(_U(Zeff)) : -1.0;
        string st = x_elem.state();
        if      ( st == "solid"  ) e->state = Element::SOLID;
        else if ( st == "liquid" ) e->state = Element::LIQUID;
        else if ( st == "gas"    ) e->state = Element::GAS;
        else                       e->state = Element::UNKNOWN;

        for(xml_coll_t p(x_elem,_LBU(isotoperef)); p; ++p)   {
          dddb_dim_t isotope = p;
          double frac   = isotope.fractionmass(1.0);
          string iso_id = object_path(context,isotope.href());
          e->isotopes.push_back(make_pair(iso_id,frac));
        }
        context->collect(e->id, e);
        context->collectPath(e->path, e);
      }
    }

    /// Specialized conversion of <material><component/></material> entities
    template <> void Conv<MaterialComponent>::convert(xml_h element) const {
      Material* m = _option<Material>();
      dddb_dim_t x_mat = element;
      m->components.push_back(MaterialComponent());
      MaterialComponent& c = m->components.back();
      c.name = x_mat.nameStr();
      c.natoms = x_mat.natoms(1);
      c.fractionmass = x_mat.fractionmass(1.0);
    }

    /// Specialized conversion of <materialref/> entities
    template <> void Conv<MaterialRef>::convert(xml_h element) const {
      Context*  context = _param<Context>();
      Catalog*  catalog = _option<Catalog>();
      string       href = element.attr<string>(_LBU(href));
      string      refid = reference_href(element,href);
      load_dddb_entity(context, catalog, element, href);
      dddb::Materials::const_iterator i=context->geo->materials.find(refid);
      if ( i == context->geo->materials.end() )  {
        printout(ERROR,"MaterialRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
      }
      if ( catalog )  {
        Material* m = (*i).second;
        string path = object_path(context,m->name);
        context->collectPath(path, m);
      }
    }

    /// Specialized conversion of <material/> entities
    template <> void Conv<Material>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = _option<Catalog>();
      dddb_dim_t x_mat = element;
      string      name = x_mat.nameStr();
      string        id = object_href(element, name);
      if ( !find(id, context->geo->materials) )  {
        Material* m    = new Material();
        m->name        = name;
        m->id          = id;
        m->path        = object_path(context,name);
        m->density     = x_mat.density(-1.0);
        m->pressure    = x_mat.pressure(-1.0);
        m->temperature = x_mat.temperature(-1.0);
        m->radlen      = x_mat.radlen(-1.0);
        m->lambda      = x_mat.lambda(-1.0);
        for(xml_coll_t p(element,_LBU(tabprops)); p; ++p)
          m->properties.push_back(p.attr<string>(_LBU(address)));

        xml_coll_t(element, _U(component)).for_each(Conv<MaterialComponent>(lcdd,context,m));
        context->collect(m->id, m); // We collect materials by NAME!!!
        context->collect(m->name, m);
        if ( catalog ) context->collectPath(m->path, m);
      }
    }

    /// Specialized conversion of <geometryinfo/> entities
    template <> void Conv<GeometryInfo>::convert(xml_h element) const {
      Catalog* context = _option<Catalog>();
      context->logvol   = element.attr<string>(_LBU(lvname));
      if ( element.hasAttr(_LBU(npath)) ) context->npath = element.attr<string>(_LBU(npath));
      if ( element.hasAttr(_LBU(support)) ) context->support = element.attr<string>(_LBU(support));
      if ( element.hasAttr(_LBU(condition)) ) context->condition = element.attr<string>(_LBU(condition));
    }

    /// Specialized conversion of <posXYZ/> entities
    template <> void Conv<Position>::convert(xml_h element) const {
      dddb_dim_t dim = element;
      Position*  pos = _option<Position>();
      pos->SetXYZ(dim.x(0.0), dim.y(0.0), dim.z(0.0));
      //dddb_print(p);
    }

    /// Specialized conversion of <rotXYZ/> entities
    template <> void Conv<RotationZYX>::convert(xml_h element) const {
      dddb_dim_t   dim = element;
      RotationZYX* rot = _option<RotationZYX>();
      rot->SetComponents(dim.rotZ(0.0), dim.rotY(0.0), dim.rotX(0.0));
      //dddb_print(p);
    }

    /// Specialized conversion of <transformation/> entities
    template <> void Conv<Transform3D>::convert(xml_h element) const {
      Transform3D* tr = _option<Transform3D>();
      build_transformation(lcdd, param, element, *tr);
      //dddb_print(tr);
    }

    /// Specialized conversion of <zplane/> entities
    template <> void Conv<ZPlane>::convert(xml_h element) const {
      Shape* s = _param<Shape>();
      dddb_dim_t dim = element;
      ZPlane plane;
      plane.innerRadius = dim.innerRadius(0.0);
      plane.outerRadius = dim.outerRadius(0.0);
      plane.z = dim.z(0.0);
      s->zplanes.push_back(plane);
    }

    /// Specialized common shape conversion
    template <> void ShapeConv<Shape>::convert(xml_h element, Shape*& s) const {
      Context* context = _param<Context>();
      xml_dim_t dim = element;
      s = new Shape;
      s->name = dim.nameStr();
      s->path = object_path(context, s->name);
    }

    /// Specialized conversion of assemblies (logical volumes without shapes)
    template <> void ShapeConv<Assembly>::convert(xml_h , Shape*& s) const {
      s = new Shape();
      s->name = "assembly";
      s->type = to_type::type();
    }

    /// Specialized conversion of Box shapes
    template <> void ShapeConv<Box>::convert(xml_h element, Shape*& s) const {
      ShapeConv<Shape>(lcdd,param)(element,s);
      dddb_dim_t dim = element;
      s->type = to_type::type();
      s->s.box.x = dim.sizeX(0.0)/2e0;
      s->s.box.y = dim.sizeY(0.0)/2e0;
      s->s.box.z = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of Cons shapes
    template <> void ShapeConv<Cons>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.cons.innerRadiusMZ = dim.innerRadiusMZ(0.0);
      s->s.cons.innerRadiusPZ = dim.innerRadiusPZ(0.0);
      s->s.cons.outerRadiusMZ = dim.outerRadiusMZ(0.0);
      s->s.cons.outerRadiusPZ = dim.outerRadiusPZ(0.0);
      s->s.cons.sizeZ         = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of ConeSegment shapes
    template <> void ShapeConv<ConeSegment>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.coneSegment.start         = dim.startPhiAngle(0.0);
      s->s.coneSegment.delta         = dim.deltaPhiAngle(2.0*M_PI);
      s->s.coneSegment.innerRadiusMZ = dim.innerRadiusMZ(0.0);
      s->s.coneSegment.innerRadiusPZ = dim.innerRadiusPZ(0.0);
      s->s.coneSegment.outerRadiusMZ = dim.outerRadiusMZ(0.0);
      s->s.coneSegment.outerRadiusPZ = dim.outerRadiusPZ(0.0);
      s->s.coneSegment.sizeZ = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of Tubs shapes
    template <> void ShapeConv<Tubs>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.tubs.start       = dim.startPhiAngle(0.0);
      s->s.tubs.delta       = dim.deltaPhiAngle(2.0*M_PI);
      s->s.tubs.innerRadius = dim.innerRadius(0.0);
      s->s.tubs.outerRadius = dim.outerRadius(0.0);
      s->s.tubs.sizeZ       = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of Trap shapes
    template <> void ShapeConv<Trap>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      if ( s->name == "Cut_out_right" )  {
        s->type = to_type::type();
      }
      s->type = to_type::type();
      s->s.trap.dz     = dim.sizeZ()/2e0;
      s->s.trap.phi    = dim.phiAngle(0);
      s->s.trap.theta  = dim.thetaAngle(0);
      s->s.trap.h1     = dim.sizeY1(0)/2e0;
      s->s.trap.bl1    = dim.sizeX1(0)/2e0;
      s->s.trap.tl1    = dim.sizeX2(0)/2e0;
      s->s.trap.alpha1 = dim.alp1(0);
      s->s.trap.h2     = dim.sizeY2(0)/2e0;
      s->s.trap.bl2    = dim.sizeX3(0)/2e0;
      s->s.trap.tl2    = dim.sizeX4(0)/2e0;
      s->s.trap.alpha2 = dim.alp2(0);
    }

    /// Specialized conversion of Polycone shapes
    template <> void ShapeConv<Polycone>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.polycone.start = dim.startPhiAngle(0.0);
      s->s.polycone.delta = dim.deltaPhiAngle(2.0*M_PI);
      xml_coll_t(element, _U(zplane)).for_each(Conv<ZPlane>(lcdd,s));
    }

    /// Specialized conversion of Polygon shapes
    template <> void ShapeConv<Polygon>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.polygon.z      = dim.z();
      s->s.polygon.start  = dim.start(0.0);
      s->s.polygon.nsides = dim.nsides();
      s->s.polygon.innerRadius = dim.innerRadius(0.0);
      s->s.polygon.outerRadius = dim.outerRadius(0.0);
      xml_coll_t(element, _U(zplane)).for_each(Conv<ZPlane>(lcdd,s));
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void ShapeConv<EllipticalTube>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.ellipticalTube.a  = dim.a();
      s->s.ellipticalTube.b  = dim.b();
      s->s.ellipticalTube.dz = dim.dz();
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void ShapeConv<TRD>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.trd.x1 = dim.sizeX1()/2e0;
      s->s.trd.x2 = dim.sizeX2()/2e0;
      s->s.trd.y1 = dim.sizeY1()/2e0;
      s->s.trd.y2 = dim.sizeY2()/2e0;
      s->s.trd.z  = dim.sizeZ()/2e0;
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void ShapeConv<Sphere>::convert(xml_h element, Shape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<Shape>(lcdd,param)(element,s);
      s->type = to_type::type();
      s->s.sphere.rmin        = dim.innerRadius(0);
      s->s.sphere.rmax        = dim.outerRadius(0);
      s->s.sphere.theta       = dim.startThetaAngle(0);
      s->s.sphere.delta_theta = dim.deltaThetaAngle(2.0*M_PI);
      s->s.sphere.phi         = dim.startPhiAngle(0);
      s->s.sphere.delta_phi   = dim.deltaPhiAngle(2.0*M_PI);
    }

    /// Specialized common conversion of boolean shapes
    template <> void ShapeConv<BooleanShape>::convert(xml_h element, Shape*& s) const {
      ShapeConv<Shape>(lcdd,param)(element,s);
      Context* context = _param<Context>();
      string        id = object_path(context,s->name);
      Context::PreservedLocals locals(context);
      context->locals.obj_path = id;
      s->s.boolean.first = 0;
      for(xml_coll_t p(element,_U(star)); p; ++p)  {
        if ( p.parent() == element ) { // Only next level is processed here
          if ( 0 == s->s.boolean.first )  {
            ShapeConv<BooleanOperation>(lcdd,context)(p,s->s.boolean.first);
            s->s.boolean.first->id = id + "/" + s->s.boolean.first->name;
            ++p;
          }
          BooleanOperation op;
          ShapeConv<BooleanOperation>(lcdd,context)(p,op.shape);
          if ( 0 == op.shape )  {
            // Error. What to do? 
            // Anyhow: ShapeConv<BooleanOperation> throws exception if the
            //         shape is unknown. We never get here.
          }
          op.shape->id = id + "/" + op.shape->name;
          ++p;
          extract_transformation(lcdd, param, p, op.trafo);
          s->boolean_ops.push_back(op);
        }
      }
    }

    /// Specialized conversion of boolean union shapes
    template <> void ShapeConv<BooleanUnion>::convert(xml_h element, Shape*& s) const {
      ShapeConv<BooleanShape>(lcdd,param)(element,s);
      s->type = to_type::type();
    }

    /// Specialized conversion of boolean intersection shapes
    template <> void ShapeConv<BooleanIntersection>::convert(xml_h element, Shape*& s) const {
      ShapeConv<BooleanShape>(lcdd,param)(element,s);
      s->type = to_type::type();
    }

    /// Specialized conversion of boolean subtraction shapes
    template <> void ShapeConv<BooleanSubtraction>::convert(xml_h element, Shape*& s) const {
      ShapeConv<BooleanShape>(lcdd,param)(element,s);
      s->type = to_type::type();
    }

    /// Specialized conversion of boolean operations
    template <> void ShapeConv<BooleanOperation>::convert(xml_h element, Shape*& s) const {
      xml_h elt = element;
      string tag = element.tag();
      Context* context = _param<Context>();

      // Now convert all possible / supported shapes
      if ( tag == "box" )
        ShapeConv<Box>(lcdd,context)(elt,s);
      else if ( tag == "cons" )
        ShapeConv<Cons>(lcdd,context)(elt,s);
      else if ( tag == "tubs" )
        ShapeConv<Tubs>(lcdd,context)(elt,s);
      else if ( tag == "trap" )
        ShapeConv<Trap>(lcdd,context)(elt,s);
      else if ( tag == "polycone" )
        ShapeConv<Polycone>(lcdd,context)(elt,s);
      else if ( tag == "polygon" )
        ShapeConv<Polygon>(lcdd,context)(elt,s);
      else if ( tag == "trd" )
        ShapeConv<TRD>(lcdd,context)(elt,s);
      else if ( tag == "eltu" )
        ShapeConv<EllipticalTube>(lcdd,context)(elt,s);
      else if ( tag == "sphere" )
        ShapeConv<Sphere>(lcdd,context)(elt,s);
      else if ( tag == "union" )
        ShapeConv<BooleanUnion>(lcdd,context)(elt,s);
      else if ( tag == "subtraction" )
        ShapeConv<BooleanSubtraction>(lcdd,context)(elt,s);
      else if ( tag == "intersection" )
        ShapeConv<BooleanIntersection>(lcdd,context)(elt,s);
      else   {
        XML::dump_tree(element.parent());
        except("BooleanOperation","Unknown shape conversion requested:"+tag);
      }
    }

    /// Specialized conversion of <logvolref/> entities
    template <> void Conv<LogVolRef>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = _option<Catalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);

      load_dddb_entity(context, catalog, element, href);
      dddb::Volumes::const_iterator i=context->geo->volumes.find(refid);
      if ( i == context->geo->volumes.end() )  {
        string r = reference_href(element,href);
        printout(ERROR,"LogVolRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
      }
      LogVol* vol = (*i).second;
      catalog->logvolrefs[vol->id] = vol;
      string path = object_path(context,vol->name);
      context->collectPath(path, vol);
      if ( context->print_logvol )  {
        print_ref("LogVolRef", context, element, href, "Path:"+path);
      }
    }

    /// Specialized conversion of <logvol/> entities
    template <> void Conv<LogVol>::convert(xml_h element) const {
      Context* context = _param<Context>();
      string   name    = element.attr<string>(_U(name));
      string   id      = object_href(element, name);
      if ( !find(id, context->geo->volumes) )  {
        Catalog* catalog = _option<Catalog>();
        string   material;
        LogVol* vol = new LogVol;
        xml_h elt;
        vol->name = name;
        vol->path = object_path(context,name);
        if ( element.hasAttr(_U(material)) )  {
          vol->material = element.attr<string>(_U(material));
        }
        Shape* s = 0;
        // Now convert all possible / supported shapes
        if ( (elt=element.child(_U(box),false)) )
          ShapeConv<Box>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(cons),false)) )
          ShapeConv<Cons>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(tubs),false)) )
          ShapeConv<Tubs>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(trap),false)) )
          ShapeConv<Trap>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(trd),false)) )
          ShapeConv<TRD>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(polycone),false)) )
          ShapeConv<Polycone>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(sphere),false)) )
          ShapeConv<Sphere>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(union),false)) )
          ShapeConv<BooleanUnion>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(subtraction),false)) )
          ShapeConv<BooleanSubtraction>(lcdd,context)(elt,s);
        else if ( (elt=element.child(_U(intersection),false)) )
          ShapeConv<BooleanIntersection>(lcdd,context)(elt,s);
        else  {
          ShapeConv<Assembly>(lcdd,context)(elt,s);
          // Dump element if not known and processed....
          for(xml_coll_t p(element,_U(star)); p; ++p)  {
            if ( p.parent() == element )  {
              string tag = p.tag();
              if ( tag == "physvol" ) continue;
              if ( tag == "paramphysvol" ) continue;
              if ( tag == "paramphysvol2D" ) continue;
              if ( tag == "paramphysvol3D" ) continue;
              printout(WARNING,"LogVol","++ Unknown and not processed tag found: %s",tag.c_str());
              XML::dump_tree(element);
              break;
            }
          }
        }
        // Now collect the information
        vol->shape = id;
        context->collect(id, s);
        context->collect(id, vol);
        if ( catalog ) context->collectPath(vol->path, vol);
        {
          Context::PreservedLocals locals(context);
          context->locals.obj_path = id;
          xml_coll_t(element, _U(physvol)).for_each(Conv<PhysVol>(lcdd,context,vol));
          xml_coll_t(element, _LBU(paramphysvol)).for_each(Conv<ParamPhysVol>(lcdd,context,vol));
          xml_coll_t(element, _LBU(paramphysvol2D)).for_each(Conv<ParamPhysVol2D>(lcdd,context,vol));
        }
      }
    }

    /// Specialized conversion of <physvol/> entities
    template <> void Conv<PhysVol>::fill(xml_h element, PhysVol* pv) const {
      xml_h      elt;
      dddb_dim_t x_vol   = element;
      string     name    = x_vol.nameStr();
      Context*   context = _param<Context>();

      pv->name   = name;
      pv->logvol = x_vol.logvol();
      pv->path   = object_path(context,name);
      build_transformation(lcdd, param, element, pv->trafo, -1);
    }
    template <> void Conv<PhysVol>::convert(xml_h element) const {
      Context* context = _param<Context>();
      dddb_dim_t x_vol = element;
      string   name    = x_vol.nameStr();
      string   id      = object_href(element, name);
      if ( !find(id, context->geo->placements) )  {
        PhysVol* vol = new PhysVol();
        this->fill(element, vol);
        context->collect(id, vol);
        context->collectPath(vol->path,vol);
        if ( optional )  {
          _option<LogVol>()->physvols.push_back(vol);
        }
      }
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void Conv<ParamPhysVol>::fill(xml_h element, ParamPhysVol* pv) const {
      dddb_dim_t x_pv = element.child(_U(physvol));
      dddb_dim_t elt = element;
      Conv<PhysVol>(lcdd,param).fill(x_pv, pv);
      pv->number1 = elt.number(0);
      build_transformation(lcdd, param, element, pv->trafo1, 0);
    }
    template <> void Conv<ParamPhysVol>::convert(xml_h element) const {
      ParamPhysVol* pv = new ParamPhysVol();
      this->fill(element, pv);
      _option<LogVol>()->physvols.push_back(pv);
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void Conv<ParamPhysVol2D>::fill(xml_h element, ParamPhysVol2D* pv) const {
      dddb_dim_t elt = element;
      Conv<ParamPhysVol>(lcdd,param).fill(element, pv);
      pv->number1 = elt.number1();
      pv->number2 = elt.number2();
      build_transformation(lcdd, param, element, pv->trafo2, 1);
    }
    template <> void Conv<ParamPhysVol2D>::convert(xml_h element) const {
      ParamPhysVol2D* pv = new ParamPhysVol2D();
      this->fill(element,pv);
      _option<LogVol>()->physvols.push_back(pv);
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void Conv<ParamPhysVol3D>::fill(xml_h element, ParamPhysVol3D* pv) const {
      dddb_dim_t elt = element;
      Conv<ParamPhysVol2D>(lcdd,param).fill(element, pv);
      pv->number3 = elt.number3();
      build_transformation(lcdd, param, element, pv->trafo3, 2);
    }
    template <> void Conv<ParamPhysVol3D>::convert(xml_h element) const {
      ParamPhysVol3D* pv = new ParamPhysVol3D();
      this->fill(element,pv);
      _option<LogVol>()->physvols.push_back(pv);
    }

    /// Specialized conversion of <parameter/> entities
    template <> void Conv<Parameter>::convert(xml_h element) const {
      Context* context  = _param<Context>();
      string name  = element.attr<string>(_U(name));
      string value = element.attr<string>(_U(value));
      _toDictionary(name,value);
      if ( context->print_params )  {
        printout(INFO,"Parameter","++  %s = %s",name.c_str(),value.c_str());
      }
    }

    /// Specialized conversion of <detelemref/> entities
    template <> void Conv<DetElemRef>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = _option<Catalog>();
      string   href    = element.attr<string>(_LBU(href));
      string   refid   = reference_href(element,href);
      load_dddb_entity(context, catalog, element, href);
      dddb::Catalogs::const_iterator i=context->geo->catalogs.find(refid);
      catalog->catalogrefs[refid] = (i==context->geo->catalogs.end()) ? 0 : (*i).second;
      if ( context->print_detelem_ref )   {
        print_ref("DetElemRef", context, element, href);
      }
    }

    /// Specialized conversion of <detelem/> entities
    template <> void Conv<DetElem>::convert(xml_h element) const {
      Context*   context = _param<Context>();
      dddb*      geo     = context->geo;
      dddb_dim_t x_det   = element;
      string     name    = x_det.nameStr();
      string     id      = object_href(element, name);

      dddb::Catalogs::iterator i = geo->catalogs.find(id);
      if ( i != geo->catalogs.end() ) {
        printout(ERROR,"DetElem", "++ Attempt to add same detector element '%s' twice!",id.c_str());
        return;
      }
      xml_h      elt;
      string     path      = object_path(context, name);
      boost::filesystem::path p(path);
      string     parent    = p.parent_path().native();
      Catalog*   det       = new Catalog();
      det->typeID   = 1;
      det->name     = name;
      det->type     = x_det.typeStr();
      det->path     = path;
      det->id       = id;
      det->support  = parent;
      if ( context->print_detelem )   {
        printout(INFO,"DetElem","  xml:%s id=%s  [%s/%s] doc:%s obj:%s / %s",
                 element.parent().tag().c_str(), 
                 det->id.c_str(),
                 det->name.c_str(),
                 det->type.c_str(),
                 context->locals.doc_path.c_str(),
                 context->locals.obj_path.c_str(),
                 det->name.c_str()
                 );
      }
      // Now extract all availible information from the xml
      if ( (elt=x_det.child(_U(author),false)) )
        Conv<Author>(lcdd,context,&det->author)(elt);
      if ( (elt=x_det.child(_U(version),false)) )
        Conv<Version>(lcdd,context,&det->version)(elt);
      xml_coll_t(element, _U(param)).for_each(Conv<Param>(lcdd,context,det));
      xml_coll_t(element, _LBU(userParameter)).for_each(Conv<Param>(lcdd,context,det));
      xml_coll_t(element, _LBU(conditioninfo)).for_each(Conv<ConditionInfo>(lcdd,context,det));
      {
        Context::PreservedLocals locals(context);
        context->locals.obj_path = det->path;
        xml_coll_t(element, _LBU(geometryinfo)).for_each(Conv<GeometryInfo>(lcdd,context,det));
        xml_coll_t(element, _LBU(detelemref)).for_each(Conv<DetElemRef>(lcdd,context,det));
      }
      det->path = det->support + "/" + det->name;
      context->collect(det->id, det);
      context->collectPath(det->path, det);
      context->print(det);
    }

    /// Specialized conversion of references to catalogs
    template <> void Conv<CatalogRef>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = _option<Catalog>();
      string   href    = element.attr<string>(_LBU(href));
      string   refid   = reference_href(element,href);

      load_dddb_entity(context, catalog, element, href);
      dddb::Catalogs::const_iterator i=context->geo->catalogs.find(refid);
      catalog->catalogrefs[refid] = (i == context->geo->catalogs.end()) ? 0 : (*i).second;
      if ( context->print_catalog_ref )  {
        print_ref("CatalogRef", context, element, href);
      }
    }

    /// Specialized conversion of <catalog/> entities
    template <> void Conv<Catalog>::convert(xml_h e) const {
      Context* context = _param<Context>();
      string   name    = e.attr<string>(_U(name));
      string   id      = object_href(e,name);
      Catalog* catalog = new Catalog();

      catalog->name          = name;
      catalog->path          = object_path(context,name);
      catalog->level         = Increment<Catalog>::counter();
      catalog->type          = "Logical";
      catalog->support       = "";
      context->collect(id, catalog);
      context->collectPath(catalog->path, catalog);
      if ( catalog->path == "/dd" )  {
        catalog->type    = "World";
        context->geo->top = catalog;
      }
      else if ( catalog->path == "/dd/Structure" )  {
        catalog->type    = "Structure";
        context->geo->structure = catalog;
      }
      else if ( catalog->path == "/dd/Geometry" )  {
        catalog->type    = "Geometry";
        context->geo->geometry  = catalog;
      }
      {
        Context::PreservedLocals locals(context);
        context->locals.obj_path = catalog->path;
        xml_coll_t(e, _U(parameter)).for_each(Conv<Parameter>(lcdd,context,catalog));
        xml_coll_t(e, _U(isotope)).for_each(Conv<Isotope>(lcdd,context,catalog));
        xml_coll_t(e, _U(element)).for_each(Conv<Element>(lcdd,context,catalog));
        xml_coll_t(e, _U(material)).for_each(Conv<Material>(lcdd,context,catalog));
        xml_coll_t(e, _U(logvol)).for_each(Conv<LogVol>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(condition)).for_each(Conv<Condition>(lcdd,context,catalog));

        xml_coll_t(e, _LBU(detelem)).for_each(Conv<DetElem>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(catalog)).for_each(Conv<Catalog>(lcdd,context,catalog));

        xml_coll_t(e, _LBU(elementref)).for_each(Conv<ElementRef>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(materialref)).for_each(Conv<MaterialRef>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(logvolref)).for_each(Conv<LogVolRef>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(detelemref)).for_each(Conv<DetElemRef>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(conditionref)).for_each(Conv<ConditionRef>(lcdd,context,catalog));
        xml_coll_t(e, _LBU(catalogref)).for_each(Conv<CatalogRef>(lcdd,context,catalog));
      }
    }

    /// Specialized conversion of <DDDB/> entities
    template <> void Conv<dddb>::convert(xml_h element) const {
      Context* context = _param<Context>();
      Catalog* catalog = 0;
      Context::PreservedLocals locals(context);
      xml_coll_t(element, _U(parameter)).for_each(Conv<Parameter>(lcdd,context,catalog));
      xml_coll_t(element, _U(isotope)).for_each(Conv<Isotope>(lcdd,context,catalog));
      xml_coll_t(element, _U(element)).for_each(Conv<Element>(lcdd,context,catalog));
      xml_coll_t(element, _U(material)).for_each(Conv<Material>(lcdd,context,catalog));
      xml_coll_t(element, _U(logvol)).for_each(Conv<LogVol>(lcdd,context,catalog));
      // catalog = _option<Catalog>();
      xml_coll_t(element, _LBU(condition)).for_each(Conv<Condition>(lcdd,context,catalog));
      catalog = 0;
      xml_coll_t(element, _LBU(detelem)).for_each(Conv<DetElem>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(catalog)).for_each(Conv<Catalog>(lcdd,context,catalog));

      xml_coll_t(element, _LBU(detelemref)).for_each(Conv<DetElemRef>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(elementref)).for_each(Conv<ElementRef>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(materialref)).for_each(Conv<MaterialRef>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(logvolref)).for_each(Conv<LogVolRef>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(conditionref)).for_each(Conv<ConditionRef>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(catalogref)).for_each(Conv<CatalogRef>(lcdd,context,catalog));
      xml_coll_t(element, _LBU(detelemref)).for_each(Conv<DetElemRef>(lcdd,context,catalog));
    }

    void apply_trafo(int apply, Position& pos, RotationZYX& rot, Transform3D& trafo, Transform3D& tr)  {
      switch(apply)   {
      case 99:   {
        tr *= trafo;
        trafo = Transform3D();
        pos = Position();
        rot = RotationZYX();
        break;
      }
      case 2:  {
        Transform3D r(rot);
        trafo = Transform3D(pos);
        trafo = trafo * r;
        tr = trafo * tr;
        trafo = Transform3D();
        pos = Position();
        rot = RotationZYX();
        break;
      }
      case 1:  {
        trafo = Transform3D(pos);
        tr *= trafo;
        trafo = Transform3D();
        pos = Position();
        rot = RotationZYX();
        break;
      }
      case 0:
      default:
        break;
      }
    }

    void extract_transformation(lcdd_t& lcdd, void* context, xml_coll_t& p, Transform3D& tr, int which)   {
      int count = 0;
      int apply = 0;
      Position pos;
      RotationZYX rot;
      Transform3D trafo;
      for(xml_h element = p.parent(); p; ++p )  {
        if ( p.parent() == element ) { // Only next level is processed here
          dddb_dim_t dim = p;
          string tag = p.tag();
          if ( tag == "transformation" && (which<0 || count == which) )  {
            apply_trafo(apply, pos, rot, trafo, tr);
            Conv<Transform3D>(lcdd,context,&trafo)(p);
            apply = 99;
          }
          else if ( tag == "posXYZ" && (which<0 || count == which) )   {
            apply_trafo(apply, pos, rot, trafo, tr);
            pos.SetXYZ(dim.x(0.0), dim.y(0.0), dim.z(0.0));
            apply = 1;
            ++count;
          }
          else if ( tag == "rotXYZ" && (which<0 || count == (which+1)) )  {
            rot.SetComponents(dim.rotZ(0.0), dim.rotY(0.0), dim.rotX(0.0));
            apply = 2;
          }
          else  {
            --p;
            apply_trafo(apply, pos, rot, trafo, tr);
            return;
          }
          if ( apply > 1 )  {
            apply_trafo(apply, pos, rot, trafo, tr);
            apply = 0;
          }
        }
      }
      if ( apply > 0 ) {
        apply_trafo(apply, pos, rot, trafo, tr);
      }
    }
    string reference_href(xml_h element, const string& ref)  {
      size_t hash = ref.rfind("#");
      size_t idx = ref.find(":");
      size_t idq = ref.find("/");

      if ( idx != string::npos && idx < idq )  {
	if ( hash == string::npos && idq != string::npos )   {
	  if ( (idq=ref.rfind("/")) != string::npos )
	    return ref + '#' + ref.substr(idq+1);
	}
	return ref;
      }
      // Resolve relative path
      string p = XML::DocumentHandler::system_path(element);
      if ( hash == 0 )  { // This is a local object. Take the element's path
        p += ref;
        boost::filesystem::path path = p;  // Remote object. Parent + local path
        p = path.normalize().native();      
        return p;
      }
      boost::filesystem::path path = p;  // Remote object. Parent + local path
      path  = path.parent_path();
      path /= ref;
      p  = path.normalize().native();      
      /// Hacky: If no reference object is given, the object name 
      /// is the same as the last item of the path (???)
      /// ==> Local fix for online consitions, which are referenced using:
      /// conddb:/Conditions/Online/L0MUON/Q1/FOI instead of:
      /// conddb:/Conditions/Online/L0MUON/Q1/FOI#FOI ...
      if ( hash == string::npos )   {
	if ( (idq=p.rfind("/")) != string::npos )
	  p += '#' + p.substr(idq+1);
	else if ( (idx=p.find(":")) != string::npos )
	  p += '#' + p.substr(idx+1);
	else
	  p += '#' + ref;
      }
      return p;
    }
    void load_dddb_entity(Context* context,
			  Catalog* catalog, 
			  xml_h element,
			  const string& ref,
			  bool prt)
    {
      size_t hash = ref.find("#");
      if ( hash != 0 )  {
        try {
          string doc_path = reference_href(element,ref);
	  if ( ref == "conddb:/Conditions/Online" )
	    doc_path = reference_href(element,ref);
          hash = doc_path.find('#');
          if ( hash != string::npos ) doc_path = doc_path.substr(0,hash);
#if 0
          if ( doc_path.find("/Conditions/") != string::npos )  {
            printout(INFO, "load_dddb", "SKIPPING DOC %s", doc_path.c_str());
            return;
          }
          if ( doc_path.find("conddb:/TrackfitGeometry") != string::npos )  {
            printout(INFO, "load_dddb", "SKIPPING DOC %s", doc_path.c_str());
            return;
          }
          if ( doc_path.find("conddb:/Rich") != string::npos )  {
            printout(INFO, "load_dddb", "SKIPPING DOC %s", doc_path.c_str());
            return;
          }
#endif
          if ( context->files.find(doc_path) != context->files.end() )
            return;
          context->files.insert(doc_path);
          if ( context->print_docs )  {
            printout(INFO, "load_dddb", "Loading DOC path %s  parent:%s",
                     doc_path.c_str(), context->locals.xml_path.c_str());
          }
          size_t idx = doc_path.find('[');
          size_t idq = doc_path.find(']');
          string key, fp = doc_path;
          if ( idq != string::npos && idx != string::npos )  {
            key = doc_path.substr(idx+1,idq-idx-1);
            fp = doc_path.substr(0,idx);
          }
          xml_doc_holder_t doc(xml_handler_t().load(fp, context->hlp->xmlReader()));
          xml_h e = doc.root();
          if ( e )   {
            if ( !key.empty() )  {
              stringstream str;
              XML::dump_tree(e, str);
              string buffer = str.str();
              while( (idx=buffer.find("-KEY-")) != string::npos )  {
                buffer.replace(idx,5,key);
              }
              doc.assign(xml_handler_t().parse(buffer.c_str(), buffer.length(), doc_path.c_str(), context->hlp->xmlReader()));
              e = doc.root();
            }
            Context::PreservedLocals locals(context);
            context->locals.doc_path = doc_path;
            Conv<dddb> converter(context->lcdd, context, catalog);
	    context->print_condition = prt;
	    if ( prt || context->print_xml )  XML::dump_tree(e);
            converter(e);
          }
        }
        catch(const exception& e)  {
          printout(INFO,"load_dddb","Failed to load %s [%s]",ref.c_str(),e.what());
        }
        catch(...)   {
          printout(INFO,"load_dddb","Failed to load %s",ref.c_str());
        }
      }
    }

    void config_context(Context& context)  {
      context.locals.doc_path     = "conddb://lhcb.xml";
      context.locals.obj_path     = "/";
      context.locals.xml_path     = "/";
      context.print_xml           = false;
      context.print_docs          = false;
      context.print_materials     = false;
      context.print_logvol        = false;
      context.print_shapes        = false;
      context.print_physvol       = false;
      context.print_params        = false;
      context.print_detelem       = false;
      context.print_detelem_ref   = false;
      context.print_detelem_xml   = false;
      context.print_condition     = false;
      context.print_condition_ref = false;
      context.print_catalog       = false;
      context.print_catalog_ref   = false;
    }
  }
}

/// Plugin entry point.
static long create_dddb(lcdd_t& lcdd, xml_h element) {
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(false);
  if ( helper )   {
    Context context(lcdd);
    context.hlp = helper;
    context.geo = new dddb();
    config_context(context);

    /// Convert the XML information
    Conv<dddb> converter(lcdd, &context);
    converter( element );
    checkParents( &context );
    fixCatalogs( &context );
    /// Transfer ownership from local context to the helper
    helper->setGeometry( context.geo );
    context.geo = 0;
    return 1;
  }
  except("DDDB","+++ Failed to access cool. No DDDBHelper object defined. Run plugin DDDBInstallHelper.");
  return 0;
}
DECLARE_XML_DOC_READER(DDDB,create_dddb)

/// Plugin entry point.
static long create_dddb_file(lcdd_t& lcdd, xml_h element) {
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(false);
  if ( helper )   {
    Context context(lcdd);
    context.hlp = helper;
    context.geo = new dddb();
    config_context(context);
    /// Convert the XML information
    Conv<dddb> converter(lcdd, &context);
    converter(element);
    checkParents( &context );
    fixCatalogs( &context );
    /// Transfer ownership from local context to the helper
    helper->setGeometry(context.geo);
    context.geo = 0;
    return 1;
  }
  except("DDDB","+++ Failed to access cool. No DDDBHelper object defined. Run plugin DDDBInstallHelper.");
  return 0;
}
DECLARE_XML_DOC_READER(DDDB_file,create_dddb_file)

/// Plugin entry point.
static long install_helper(lcdd_t& lcdd, int argc, char** argv) {
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(false);
  if ( !helper )   {
    helper = new DDDBHelper(lcdd);
    if ( argc > 0 )   {
      helper->setXmlReader((XML::UriReader*)argv[0]);
    }
    lcdd.addExtension<DDDBHelper,DDDBHelper>(helper);
  }
  return 1;
}
DECLARE_APPLY(DDDBInstallHelper,install_helper)
