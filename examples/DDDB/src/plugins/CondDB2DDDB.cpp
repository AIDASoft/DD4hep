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
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/Path.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/OpaqueDataBinder.h"
#include "DDDB/DDDBTags.h"
#include "DDDB/DDDBHelper.h"
#include "DDDB/DDDBReader.h"
#include "DDDB/DDDBDimension.h"
#include "DDDB/DDDBConversion.h"
#include "Math/Polar2D.h"

// C/C++ include files

using namespace std;
using namespace dd4hep;
using namespace dd4hep::DDDB;

#ifdef __GNUC__    // Disable some diagnostics.
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Keep all in here anonymous. Does not have to be visible outside.
  namespace {

    using cond::AbstractMap;

    typedef AbstractMap::Params ConditionParams;
    struct PositionRPhiZ {};
    struct DDDBLogVolRef {};
    struct DDDBElementRef  {};
    struct DDDBMaterialRef  {};
    struct DDDBCatalogRef  {};
    struct DDDBConditionRef  {};
    struct DDDBTabPropertyRef  {};
    struct DDDBDetElemRef  {};

    struct DDDBConditionInfo  {};
    struct DDDBDetElem  {};
    struct DDDBParam {};
    struct DDDBBlock {};
    struct DDDBConfig {};
    struct DDDBParameter  {};
    struct DDDBGeometryInfo  {};
    struct DDDBConditionParam {};
    struct DDDBConditionParamMap {};
    struct DDDBConditionParamVector {};
    struct DDDBConditionParamSpecific {};

    /// C++ version to convert a string to lower case
    std::string str_lower(const std::string& str) {
      std::string res = str.c_str();
      for(char* p=(char*)res.c_str(); *p; ++p) *p = ::tolower(*p);
      return res;
    }

    /// C++ version to convert a string to upper case
    std::string str_upper(const std::string& str) {
      std::string res = str.c_str();
      for(char* p=(char*)res.c_str(); *p; ++p) *p = ::toupper(*p);
      return res;
    }
    
    /// Main processing context
    /**   \ingroup DD4HEP_DDDB
     */
    class DDDBContext  {

    private:
      template <typename T,typename Q> 
      void collect_id(Q& container, const string& id, T* object)  const {
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
      void collect_p(Q& container, const string& path, T* object)   const {
        typename Q::const_iterator i=container.find(path);
        if ( i != container.end() )  {
          if ( object == (*i).second ) return;
          printout(ERROR,"collectPath","++ Duplicate ID: %s  %p <-> %p",
                   path.c_str(), object, (*i).second);
        }
        container[path] = object->addRef();
      }

    public:

      /// Local processing environment
      /**   \ingroup DD4HEP_DDDB
       */
      class Locals  {
      public:
        string obj_path;
        DDDBDocument* xml_doc;
        Locals() : obj_path(), xml_doc(0) {}
        Locals(const Locals& c) : obj_path(c.obj_path), xml_doc(c.xml_doc) {}
        Locals& operator=(const Locals& c)  {
          obj_path = c.obj_path;
          xml_doc  = c.xml_doc;
          return *this;
        }
      };

      /// Helper class to preserve local processing environment
      /**   \ingroup DD4HEP_DDDB
       */
      class PreservedLocals : public Locals {
      public:
        DDDBContext* context;
#ifdef __DEBUG_LOCALS
        void print(const char* opt, bool inc)  const  {
          static int preserv_level = 0;
          int level = inc ? preserv_level++ : --preserv_level;
          if ( xml_doc )
            printout(INFO,"Locals","%s[%d]: %s [%s]", opt, level, obj_path.c_str(), xml_doc->id.c_str());
          else
            printout(INFO,"Locals","%s[%d]: %s ", opt, level, obj_path.c_str());
        }
#else
        inline void print(const char*, bool) const {}
#endif
        PreservedLocals(DDDBContext* c) : Locals(c->locals), context(c) {
          print("PUSH", true);
        }
        ~PreservedLocals()           {
          print("POP ", false);
          context->locals = *this; 
        }
      };

    public:
      Detector*   description = 0;
      DDDBReader* resolver = 0;
      dddb*       geo = 0;
      Locals      locals;
      bool        check = true;
      bool        print_xml = false;
      bool        print_file_load = false;
      bool        print_docs = false;
      bool        print_materials = false;
      bool        print_logvol = false;
      bool        print_shapes = false;
      bool        print_physvol = false;
      bool        print_params = false;
      bool        print_detelem = false;
      bool        print_detelem_ref = false;
      bool        print_detelem_xml = false;
      bool        print_condition = false;
      bool        print_condition_ref = false;
      bool        print_catalog = false;
      bool        print_catalog_ref = false;
      bool        print_tabprop = false;

      /// Default constructor
      DDDBContext() = default;
      /// Initializing constructor
      DDDBContext(Detector* d);
      /// Default destructor
      ~DDDBContext()  {
      }

      /** Printout helpers                                                                             */
      void print(const DDDBIsotope* obj)   const               { if ( print_materials ) dddb_print(obj);    }
      void print(const DDDBElement* obj)   const               { if ( print_materials ) dddb_print(obj);    }
      void print(const DDDBMaterial* obj)  const               { if ( print_materials ) dddb_print(obj);    }
      void print(const DDDBShape* obj)     const               { if ( print_shapes )    dddb_print(obj);    }
      void print(const DDDBPhysVol* obj)   const               { if ( print_physvol )   dddb_print(obj);    }
      void print(const DDDBLogVol* obj)    const               { if ( print_logvol )    dddb_print(obj);    }
      void print(const DDDBCatalog* obj)   const               { if ( print_catalog )   dddb_print(obj);    }
      void print(const DDDBTabProperty* obj)  const            { if ( print_tabprop )   dddb_print(obj);    }
      void print(const DDDBDocument* obj)  const               { if ( print_docs )      dddb_print(obj);    }
      
      /** Data collection helpers for indexing by object identifier                                    */
      void collect(const string& id, DDDBCatalog* obj)         { collect_id(geo->catalogs, id, obj);        }
      void collect(const string& id, DDDBShape* obj)           { collect_id(geo->shapes, id, obj);          }
      void collect(const string& id, DDDBPhysVol* obj)         { collect_id(geo->placements, id, obj);      }
      void collect(const string& id, DDDBLogVol* obj)          { collect_id(geo->volumes, id, obj);         }
      void collect(const string& id, DDDBIsotope* obj)         { collect_id(geo->isotopes, id, obj);        }
      void collect(const string& id, DDDBElement* obj)         { collect_id(geo->elements, id, obj);        }
      void collect(const string& id, DDDBMaterial* obj)        { collect_id(geo->materials, id, obj);       }
      void collect(const string& id, DDDBTabProperty* obj)     { collect_id(geo->tabproperties, id, obj);   }
      void collect(const string& id, Condition& object)  {
        dddb::Conditions::const_iterator i=geo->conditions.find(id);
        if ( i != geo->conditions.end() )  {
          if ( object.ptr() == (*i).second ) return;
          printout(ERROR,"collect","++ Duplicate ID: %s  %p <-> %p",
                   id.c_str(), object.ptr(), (*i).second);
        }
        geo->conditions[id] = object.ptr();
      }

      /** Data collection helpers for indexing by path                                                 */
      void collectPath(const string& path, DDDBElement*   obj) { collect_p(geo->elementPaths,   path, obj); }
      void collectPath(const string& path, DDDBMaterial*  obj) { collect_p(geo->materialPaths,  path, obj); }
      void collectPath(const string& path, DDDBPhysVol*   obj) { collect_p(geo->placementPaths, path, obj); }
      void collectPath(const string& path, DDDBLogVol*    obj) { collect_p(geo->volumePaths,    path, obj); }
      void collectPath(const string& path, DDDBTabProperty*   obj) { collect_p(geo->tabpropertyPaths, path, obj); }
      void collectPath(const string& path, DDDBCatalog*   obj) { collect_p(geo->catalogPaths,   path, obj); }
      void collectPath(const string& path, Condition& object) {
        dddb::Conditions::const_iterator i=geo->conditionPaths.find(path);
        if ( i != geo->conditionPaths.end() )  {
          if ( object.ptr() == (*i).second ) return;
          printout(ERROR,"collectPath","++ Duplicate ID: %s  %p <-> %p",
                   path.c_str(), object.ptr(), (*i).second);
        }
        geo->conditionPaths[path] = object.ptr();
      }
    } s_config;

    /// Initializing constructor
    DDDBContext::DDDBContext(Detector* d)   {
      *this = s_config;
      description = d;
    }

    
    /// Converter to incept processing exceptions
    /**   \ingroup DD4HEP_DDDB
     */
    template <typename T> struct Conv : protected Converter<T> {
    public:
      /// Initializing constructor of the functor with initialization of the user parameter
      Conv(Detector& l, void* p, void* o=0) : Converter<T>(l,p,o) {}
      void convert(xml::Handle_t element) const;
      void fill(xml::Handle_t, T*) const {}
      void operator()(xml::Handle_t element) const  {
        Increment<T> incr;
        try  {
          convert(element);
        }
        catch(const exception& e)  {
          printout(INFO,typeName(typeid(T)),"Failed to convert XML object: %s", e.what());
          xml::dump_tree(element.parent());
        }
        catch(...)   {
          printout(INFO,typeName(typeid(T)),"Failed to convert XML object.");
          xml::dump_tree(element.parent());
        }
      }
    };

    /// Special shape converter with inception of processing exceptions
    /**   \ingroup DD4HEP_DDDB
     */
    template <typename T> struct ShapeConv : private Converter<T> {
    public:
      /// Initializing constructor of the functor with initialization of the user parameter
      ShapeConv(Detector& l, void* p) : Converter<T>(l,p,0) {}
      void operator()(xml::Handle_t element, DDDBShape*& ptr_shape) const  {
        Increment<T> incr;
        try  {
          convert(element, ptr_shape);
        }
        catch(const exception& e)  {
          printout(INFO,typeName(typeid(T)),"Failed to convert object: %s",e.what());
          xml::dump_tree(element.parent());
        }
        catch(...)   {
          printout(INFO,typeName(typeid(T)),"Failed to convert object.");
          xml::dump_tree(element.parent());
        }
      }
      void convert(xml::Handle_t element, DDDBShape*& ptr_shape) const;
    };

    /// Explicit template instantiations
    template <> void ShapeConv<DDDBShape>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBAssembly>::convert(xml_h , DDDBShape*& s) const;
    template <> void ShapeConv<DDDBBox>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBCons>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBConeSegment>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBTubs>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBTrap>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBPolycone>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBPolygon>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBEllipticalTube>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBTRD>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBSphere>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBBooleanShape>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBBooleanUnion>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBBooleanIntersection>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBBooleanSubtraction>::convert(xml_h element, DDDBShape*& s) const;
    template <> void ShapeConv<DDDBBooleanOperation>::convert(xml_h element, DDDBShape*& ptr_shape) const;

    template <> void Conv<DDDBZPlane>::convert(xml_h element) const;
    template <> void Conv<dddb>::convert(xml_h element) const;
    template <> void Conv<DDDBLogVol>::convert(xml_h element) const;
    template <> void Conv<DDDBBlock>::convert(xml_h element) const;
    template <> void Conv<DDDBConfig>::convert(xml_h element) const;
    template <> void Conv<DDDBLogVolRef>::convert(xml_h element) const;
    template <> void Conv<DDDBPhysVol>::convert(xml_h element) const;
    template <> void Conv<DDDBParamPhysVol>::convert(xml_h element) const;
    template <> void Conv<DDDBParamPhysVol2D>::convert(xml_h element) const;
    template <> void Conv<DDDBParamPhysVol3D>::convert(xml_h element) const;
    template <> void Conv<DDDBConditionParam>::convert(xml_h element) const;
    template <> void Conv<Delta>::convert(xml_h element) const;

    template <> void Conv<Position>::convert(xml_h element) const;
    template <> void Conv<PositionRPhiZ>::convert(xml_h element) const;
    template <> void Conv<RotationZYX>::convert(xml_h element) const;
    template <> void Conv<Transform3D>::convert(xml_h element) const;

    void extract_transformation(Detector& description, void* context, xml_coll_t& collection, Transform3D& tr, int which=-1);
    void build_transformation(Detector& description, void* context, xml_h element, Transform3D& tr, int which=-1)   {
      xml_coll_t p(element,_U(star));
      extract_transformation(description,context,p,tr,which);
    }
    string reference_href(xml_h element, const string& ref);

    xml_h find_local_element(xml_elt_t element, const string& ref, const xml_tag_t& tag)  {
      size_t hash = ref.find("#");
      if ( hash == 0 )  {
        string name = ref.substr(1);
        xml_h  root = element.document().root();
        for(xml_coll_t coll(root,tag); coll; ++coll )  {
          string entry = coll.attr<string>(_U(name));
          if ( entry == name )  {
            return coll;
          }
        }
      }
      return xml_h(0);
    }

    string object_path(DDDBContext* context, const string& ref)  {
      size_t hash = ref.rfind("#");
      Path path = hash==0 ? ref.substr(1) : ref;
      if ( ref[0] != '/' )  {
        path = context->locals.obj_path;
        path /= (hash==0 ? ref.substr(1) : ref.substr(hash+1));
      }
      return path.normalize().native();
    }
    string object_href(xml_h element, const string& ref)   {
      string p = xml::DocumentHandler::system_path(element);
      Path path = p;
      p = path.normalize().native();
      p += '#';
      p += ref;
      return p;
    }
    string reference_path(DDDBContext* context, const string& ref)   {
      size_t hash = ref.rfind("#");
      size_t idx  = ref.find(":");
      size_t idq  = ref.find("/");
      Path path = hash==0 ? ref.substr(1) : ref;
      if ( (idx == string::npos || idq < idx) && ref[0] != '/' )  {
        path = context->locals.xml_doc->id;
        if ( hash != 0 ) path = path.parent_path();
        path /= ref.substr(0,hash);
      }
      return path.normalize().native();
    }
    void print_ref(const char* desc, DDDBContext* context, xml_h element, const string& ref, const string& opt="")  {
      size_t hash = ref.find("#");
      string path = reference_path(context,ref);
      string obj  = hash == string::npos ? ref : ref.substr(hash+1);
      string id1  = object_path(context,ref);
      string id2  = reference_href(element,ref);
      printout(INFO, desc, "** %s --> %s  path: %s # %s  %s", 
               id1.c_str(), id2.c_str(), path.c_str(), obj.c_str(), opt.c_str());
    }

    template <typename ACTION=dddb>
    void load_dddb_entity(DDDBContext*        context,
                          DDDBCatalog*    catalog,
                          xml_h           element,
                          const string&   ref,
                          bool            print=false);

    /// Helper to locate objects in a map using string identifiers
    template <typename Q> bool _find(const string& id, const Q& container)  {
      return container.find(id) != container.end();
    }

    bool checkParents(DDDBContext* context, DDDBCatalog* det)  {
      dddb* geo = context->geo;
      if ( det == geo->top )  {
        return true;
      }
      else if ( det )   {
        DDDBCatalog* par = 0;
        string parent_id  = det->support;
        dddb::Catalogs::const_iterator k = geo->catalogPaths.find(parent_id);
        if ( k != geo->catalogPaths.end() )  {
          par = (*k).second;
          par->catalogrefs[det->id] = det;
          return checkParents(context, par);
        }
        Path p(det->path);
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

    void checkParents(DDDBContext* context)  {
      dddb* geo = context->geo;
      if ( context->check )  {
        for(dddb::Catalogs::iterator i=geo->catalogs.begin(); i!=geo->catalogs.end(); ++i)  {
          DDDBCatalog* det = (*i).second;
          checkParents(context,det);
        }
      }
    }

    void fixCatalogs(DDDBContext* context)  {
      dddb*       geo = context->geo;
      DDDBReader* rdr = context->resolver;
      for(dddb::Catalogs::iterator i=geo->catalogs.begin(); i!=geo->catalogs.end(); ++i)  {
        DDDBCatalog* det = (*i).second;
        for(dddb::Catalogs::iterator j=det->catalogrefs.begin(); j!=det->catalogrefs.end(); ++j)  {
          const string& child_id = (*j).first;
          dddb::Catalogs::const_iterator k = geo->catalogs.find(child_id);
          if ( k == geo->catalogs.end() )   {
            if ( !rdr->isBlocked(child_id) )  {
              printout(ERROR,"fixCatalogs","++  MISSING ID: %s child:%s",det->id.c_str(),child_id.c_str());
            }
            continue;
          }
          DDDBCatalog* c = (*k).second;
          det->catalogs[c->name] = c;
          if ( 0 == (*j).second )  {
            (*j).second = c;
          }
        }
        for(dddb::Volumes::iterator j=det->logvolrefs.begin(); j!=det->logvolrefs.end(); ++j)  {
          DDDBLogVol* c = (*j).second;
          if ( !c )  {
            if ( !rdr->isBlocked(det->id) )  {
              printout(ERROR,"fixCatalogs","++  MISSING Volume: %s child:%s",det->id.c_str(),(*j).first.c_str());
            }
            continue;
          }
          dddb::Volumes::const_iterator k = geo->volumes.find(c->id);
          if ( k == geo->volumes.end() )   {
            if ( !rdr->isBlocked(det->id) )  {
              printout(ERROR,"fixCatalogs","++  MISSING VolID: %s child:%s",det->id.c_str(),c->id.c_str());
            }
          }
          det->logvols[c->name] = c;
          if ( 0 == (*j).second )  {
            (*j).second = c;
          }
        }
      }
    }
    string clean_cond_data(char pre, const string& data, char post)  {
      string d = pre+data+" ";
      size_t idx, idq;
      for(idx = d.find_first_not_of(' ',1); idx != string::npos;)  {
        if ( ::isspace(d[idx]) ) d[idx] = ' ';
        idx = d.find_first_not_of(' ',++idx);
      }
      for(idx = d.find_first_not_of(' ',1); idx != string::npos; ++idx)  {
        if ( d[idx] != ' ' && ::isspace(d[idx]) ) d[idx] = ' ';
        idq = d.find_first_of(' ',idx);
        if ( idq != string::npos )  {
          idx = d.find_first_not_of(' ',idq);
          if ( idx == string::npos ) break;
          if ( d[idx] != ' ' && ::isspace(d[idx]) ) d[idx] = ' ';
          d[idq] = ',';
          continue;
        }
        break;
      }
      d[d.length()-1] = post;
      return d;
    }

    /// Specialized conversion of <param/> entities
    template <> void Conv<DDDBConditionParam>::convert(xml_h element) const {
      string          nam = element.attr<string>(_U(name));
      string          typ = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("int");
      string         data = element.text();
      pair<string,OpaqueDataBlock> block;
      block.first = nam;
      try {
        detail::OpaqueDataBinder::bind(detail::ValueBinder(), block.second, typ, data);
      }
      catch(...)  {
        pair<string,OpaqueDataBlock> block1;
        detail::OpaqueDataBinder::bind(detail::ValueBinder(), block1.second, typ, data);
      }
      ConditionParams* par = _option<ConditionParams>();
      pair<ConditionParams::iterator,bool> res = par->insert(block);
      if ( !res.second )  {
        printout(INFO,"ParamVector","++ Failed to insert condition parameter:%s",nam.c_str());
      }
    }

    /// Specialized conversion of <paramVector> entities
    template <> void Conv<DDDBConditionParamVector>::convert(xml_h element) const {
      string  nam = element.attr<string>(_U(name));
      string  typ = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("int");
      string data = clean_cond_data('[',element.text(),']');
      ConditionParams* par = _option<ConditionParams>();
      pair<string,OpaqueDataBlock> block;

      block.first = nam;
      detail::OpaqueDataBinder::bind(detail::VectorBinder(), block.second, typ, data);
      pair<ConditionParams::iterator,bool> res = par->insert(block);
      if ( !res.second )  {
        printout(INFO,"ParamVector","++ Failed to insert condition parameter:%s",nam.c_str());
      }
    }

    /// Specialized conversion of <map/> conditions entities
    template <> void Conv<DDDBConditionParamMap>::convert(xml_h element) const {
      dddb_dim_t e = element;
      string nam = element.attr<string>(_U(name));
      string key_type = e.attr<string>(_LBU(keytype));
      string val_type = e.attr<string>(_LBU(valuetype));
      pair<string,OpaqueDataBlock> block;
      detail::MapBinder binder;

      block.first = nam;
      detail::OpaqueDataBinder::bind_map(binder,block.second, key_type, val_type);
      for(xml_coll_t i(e,_LBU(item)); i; ++i)  {
        string key = i.attr<string>(_LBU(key));
        string val = i.attr<string>(_LBU(value));
        detail::OpaqueDataBinder::insert_map(binder,block.second, key_type, key, val_type, val);
      }
      ConditionParams* par = _option<ConditionParams>();
      pair<ConditionParams::iterator,bool> res = par->insert(block);
      if ( !res.second )
        printout(INFO,"ParamMap","++ Failed to insert map-condition:%s",nam.c_str());
    }

    /// Specialized conversion of <specific/> conditions entities
    template <> void Conv<DDDBConditionParamSpecific>::convert(xml_h element) const {
      string nam = element.parent().attr<string>(_U(name));
      pair<string,OpaqueDataBlock> block;
      stringstream str;

      xml::dump_tree(element, str);
      block.second.bind<string>(str.str());
      block.first = nam;

      ConditionParams* par = _option<ConditionParams>();
      pair<ConditionParams::iterator,bool> res = par->insert(block);
      if ( !res.second )  {
        printout(INFO,"ParamVector","++ Failed to insert condition entry:%s of type <specific/>",nam.c_str());
      }
    }

    /// Specialized conversion of <param/> and <paramVector> entities in alignments
    template <> void Conv<Delta>::convert(xml_h element) const {
      string        nam = element.attr<string>(_U(name));
      string       data = clean_cond_data('(',element.text(),')');
      Delta* a = _option<Delta>();
      Position pos;
      const BasicGrammar& g = BasicGrammar::instance<Position>();

      if ( !g.fromString(&pos,data) ) g.invalidConversion(data, g.type());
      if ( nam == "dPosXYZ" )  {
        a->translation = pos/10.0;
        a->flags |= Delta::HAVE_TRANSLATION;
      }
      else if ( nam == "dRotXYZ" )   {
        a->rotation = RotationZYX(pos.z(),pos.y(),pos.x());
        a->flags |= Delta::HAVE_ROTATION;
      }
      else if ( nam == "pivotXYZ" )   {
        a->pivot = Translation3D(pos.x(),pos.y(),pos.z());
        a->flags |= Delta::HAVE_PIVOT;
      }
      else   {
        printout(ERROR,"Delta","++ Unknown alignment conditions tag: %s",nam.c_str());
      }
    }

    /// Specialized conversion of <condition/> entities
    template <> void Conv<Condition>::convert(xml_h element) const {
      DDDBContext*   context = _param<DDDBContext>();
      string     name    = element.attr<string>(_U(name));
      string     id      = object_href(element,name);
      if ( !_find(id, context->geo->conditions) )  {
        DDDBCatalog*   catalog = _option<DDDBCatalog>();
        DDDBDocument*  doc     = context->locals.xml_doc;
        string     path    = object_path(context,name);
        static int num_param=0, num_vector=0, num_map=0, num_spec=0, num_align=0;
        Condition cond(path,"DDDB");
        cond->address  = doc->name+"@"+id;
        cond->value    = path; // doc->name;
        cond->validity = "";
        cond->hash     = detail::hash64(path);
        if ( element.hasAttr(_U(comment)) )  {
          cond->comment = element.attr<string>(_U(comment));
        }
#if 0
        if ( str_upper(path).find("/VP/") != string::npos )   {
          printout(ALWAYS,"Conditions","Loading condition: %s", path.c_str());
        }
#endif
        
        int cls_id = -1;
        if ( element.hasAttr(_LBU(classID)) )  {
          cls_id = element.attr<int>(_LBU(classID));
        }
#if 0
        if ( cls_id == AbstractMap::ALIGNMENT )   {
          Conv<Delta> conv(description,context,&cond.bind<Delta>());
          xml_coll_t(element,_LBU(paramVector)).for_each(conv);
          cond->setFlag(Condition::ALIGNMENT_DELTA);
          ++num_align;
        }
#endif
        if ( cls_id == AbstractMap::ALIGNMENT || cls_id == 1008106 )   {
          AbstractMap& d = cond.bind<AbstractMap>();
          pair<string,OpaqueDataBlock>  block;
          Delta&  align = block.second.bind<Delta>();
          d.clientData = doc->addRef();
          d.classID    = cls_id;
          block.first  = align::Keys::deltaName;

          Conv<Delta> conv(description,context,&align);
          xml_coll_t(element,_LBU(paramVector)).for_each(conv);
          cond.setFlag(Condition::ALIGNMENT_DELTA);
          pair<ConditionParams::iterator,bool> res = d.params.insert(block);
          if ( !res.second )  {
            printout(INFO,"Condition",
                     "++ Failed to insert condition parameter:%s",
                     name.c_str());
          }
          if ( d.size() > 1 )  {
            printout(WARNING,"Condition",
                     "++ Found ALIGNMENT condition block with MULTIPLE entries [%d]: %s",
                     int(d.size()), name.c_str());
          }
          ++num_align;
        }
        else
        {
          AbstractMap& d = cond.bind<AbstractMap>();
          d.clientData = doc->addRef();
          d.classID    = cls_id;

          Conv<DDDBConditionParam>         object_cnv(description,context,&d.params);
          xml_coll_t(element,_U(param)).for_each(object_cnv);

          Conv<DDDBConditionParamVector>   vector_cnv(description,context,&d.params);
          xml_coll_t(element,_LBU(paramVector)).for_each(vector_cnv);

          Conv<DDDBConditionParamMap>      map_cnv(description,context,&d.params);
          xml_coll_t(element,_LBU(map)).for_each(map_cnv);

          Conv<DDDBConditionParamSpecific> specific_cnv(description,context,&d.params);
          xml_coll_t(element,_LBU(specific)).for_each(specific_cnv);
          for(xml_coll_t iter(element,_U(star)); iter; ++iter)  {
            string tag = iter.tag();
            string nam = iter.hasAttr(_U(name)) ? iter.attr<string>(_U(name)) : string();
            if ( context->print_condition )  {
              printout(INFO,"ParamMap","++ Condition:%s -> %s",path.c_str(),nam.c_str());
            }
            if ( d.classID == AbstractMap::ALIGNMENT ) { continue; }
            if ( tag == "param"       )  { ++num_param;  continue; }
            if ( tag == "paramVector" )  { ++num_vector; continue; }
            if ( tag == "map"         )  { ++num_map;    continue; }
            if ( tag == "specific"    )  { ++num_spec;   continue; }
            printout(INFO,"Condition","++ Unknown conditions tag:%s obj:%s id:%s",
                     tag.c_str(), path.c_str(), id.c_str());
          }
          num_param += int(d.params.size());
        }
        context->collect(id, cond);
        if ( catalog )  {
          context->collectPath(path, cond);
        }
        if ( (context->geo->conditions.size()%500) == 0 )  {
          printout(INFO,"Condition","++ Processed %d conditions....last:%s Number of Params: %d Vec:%d Map:%d Spec:%d Align:%d", 
                   int(context->geo->conditions.size()), path.c_str(), num_param, num_vector, num_map, num_spec, num_align);
        }
      }
    }

    /// Specialized conversion of <conditionref/> entities
    template <> void Conv<DDDBConditionRef>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);
      string      path;

      xml_h target = find_local_element(element, href, _LBU(condition));
      if ( target )
        Conv<Condition>(description,context,catalog)(target);
      else
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
      Condition cond((*i).second);
      path = object_path(context,cond->name);
      context->collectPath(path, cond);
      if ( context->print_condition_ref )  {
        print_ref("ConditionRef", context, element, href, "Path:"+path);
      }
    }

    /// Specialized conversion of <author/> entities
    template <> void Conv<DDDBAuthor>::convert(xml_h element) const {
      string* context = _option<string>();
      if ( element.hasAttr(_U(author)) ) *context = element.attr<string>(_U(author));
    }

    /// Specialized conversion of <version/> entities
    template <> void Conv<DDDBVersion>::convert(xml_h element) const {
      string* context = _option<string>();
      if ( element.hasAttr(_U(version)) ) *context = element.attr<string>(_U(version));
    }

    /// Specialized conversion of <param/> entities
    template <> void Conv<DDDBParam>::convert(xml_h element) const {
      DDDBCatalog* det   = _option<DDDBCatalog>();
      string   name  = element.attr<string>(_U(name));
      string   type  = element.hasAttr(_U(type)) ? element.attr<string>(_U(type)) : string("int");
      string   value = element.text();
      det->params[name] = make_pair(type,value);
    }

    /// Specialized conversion of <param/> entities
    template <> void Conv<DDDBBlock>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      context->resolver->blockPath(element.attr<string>(_U(name)));
    }

    /// Specialized conversion of <param/> entities
    template <> void Conv<DDDBConfig>::convert(xml_h element) const {
      DDDBContext* ctx = &s_config;
      for(xml_coll_t c(element,_U(param)); c; ++c)   {
        xml_dim_t p = c;
        if ( p.nameStr() == "print_file_load" )
          ctx->print_file_load = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_xml" )
          ctx->print_xml = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_docs" )
          ctx->print_docs = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_materials" )
          ctx->print_materials = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_logvol" )
          ctx->print_logvol = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_shapes" )
          ctx->print_shapes = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_physvol" )
          ctx->print_physvol = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_params" )
          ctx->print_params = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_detelem" )
          ctx->print_detelem = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_detelem_ref" )
          ctx->print_detelem_ref = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_detelem_xml" )
          ctx->print_detelem_xml = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_condition" )
          ctx->print_condition = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_condition_ref" )
          ctx->print_condition_ref = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_catalog" )
          ctx->print_catalog = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_catalog_ref" )
          ctx->print_catalog_ref = p.attr<bool>(_U(value));
        else if ( p.nameStr() == "print_tabprop" )
          ctx->print_tabprop = p.attr<bool>(_U(value));
      }
    }

    /// Specialized conversion of <ConditionInfo/> entities
    template <> void Conv<DDDBConditionInfo>::convert(xml_h element) const {
      DDDBCatalog* det  = _option<DDDBCatalog>();
      string   name = element.attr<string>(_U(name));
      string   cond = element.attr<string>(_LBU(condition));
      det->conditioninfo[name] = cond;
    }

    /// Specialized conversion of <isotope/> entities
    template <> void Conv<DDDBIsotope>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      dddb_dim_t x_i = element;
      string name = x_i.nameStr();
      string id = object_path(context,name);
      if ( !_find(id, context->geo->isotopes) )  {
        DDDBIsotope* i = new DDDBIsotope();
        i->name       = name;
        i->A          = x_i.A(-1.0);
        i->Z          = x_i.Z(-1.0);
        i->density    = x_i.density(-1.0);
        context->collect(id, i);
        i->setDocument(context->locals.xml_doc);
      }
    }

    /// Specialized conversion of <elementref/> entities
    template <> void Conv<DDDBElementRef>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);
      load_dddb_entity(context, catalog, element, href);
      dddb::Elements::const_iterator i=context->geo->elements.find(refid);
      if ( i == context->geo->elements.end() )  {
        printout(ERROR,"ElementRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
      }
      DDDBElement* e = (*i).second;
      string path = object_path(context,e->name);
      context->collectPath(path, e);
    }

    /// Specialized conversion of <element/> entities
    template <> void Conv<DDDBElement>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      dddb_dim_t x_elem = element;
      string       name = x_elem.nameStr();
      string         id = object_href(element, name);
      if ( !_find(id, context->geo->elements) )  {
        DDDBElement* e = new DDDBElement();
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
        if      ( st == "solid"  ) e->state = DDDBElement::SOLID;
        else if ( st == "liquid" ) e->state = DDDBElement::LIQUID;
        else if ( st == "gas"    ) e->state = DDDBElement::GAS;
        else                       e->state = DDDBElement::UNKNOWN;

        for(xml_coll_t p(x_elem,_LBU(isotoperef)); p; ++p)   {
          dddb_dim_t isotope = p;
          double frac   = isotope.fractionmass(1.0);
          string iso_id = object_path(context,isotope.href());
          e->isotopes.push_back(make_pair(iso_id,frac));
        }
        context->collect(e->id, e);
        context->collectPath(e->path, e);
        e->setDocument(context->locals.xml_doc);
      }
    }

    /// Specialized conversion of <material><component/></material> entities
    template <> void Conv<DDDBMaterialComponent>::convert(xml_h element) const {
      DDDBMaterial* m = _option<DDDBMaterial>();
      dddb_dim_t x_mat = element;
      m->components.push_back(DDDBMaterialComponent());
      DDDBMaterialComponent& c = m->components.back();
      c.name = x_mat.nameStr();
      c.natoms = x_mat.natoms(1);
      c.fractionmass = x_mat.fractionmass(1.0);
    }

    /// Specialized conversion of <materialref/> entities
    template <> void Conv<DDDBMaterialRef>::convert(xml_h element) const {
      DDDBContext*  context = _param<DDDBContext>();
      DDDBCatalog*  catalog = _option<DDDBCatalog>();
      string       href = element.attr<string>(_LBU(href));
      string      refid = reference_href(element,href);
      load_dddb_entity(context, catalog, element, href);
      dddb::Materials::const_iterator i=context->geo->materials.find(refid);
      if ( i == context->geo->materials.end() )  {
        printout(ERROR,"MaterialRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
      }
      if ( catalog )  {
        DDDBMaterial* m = (*i).second;
        string path = object_path(context,m->name);
        context->collectPath(path, m);
      }
    }

    /// Specialized conversion of <material/> entities
    template <> void Conv<DDDBMaterial>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      dddb_dim_t x_mat = element;
      string      name = x_mat.nameStr();
      string        id = object_href(element, name);
      if ( !_find(id, context->geo->materials) )  {
        DDDBMaterial* m    = new DDDBMaterial();
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

        xml_coll_t(element, _U(component)).for_each(Conv<DDDBMaterialComponent>(description,context,m));
        context->collect(m->id, m); // We collect materials by NAME!!!
        context->collect(m->name, m);
        m->setDocument(context->locals.xml_doc);
        if ( catalog ) context->collectPath(m->path, m);
      }
    }

    /// Specialized conversion of <geometryinfo/> entities
    template <> void Conv<DDDBGeometryInfo>::convert(xml_h element) const {
      DDDBCatalog* context = _option<DDDBCatalog>();
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

    /// Specialized conversion of <posRPhiZ/> entities
    template <> void Conv<PositionRPhiZ>::convert(xml_h element) const {
      dddb_dim_t dim = element;
      ROOT::Math::Polar2D<double> dim2(dim.r(0.0), dim.phi(0.0));
      Position*  pos = _option<Position>();
      pos->SetXYZ(dim2.X(), dim2.Y(), dim.z(0.0));
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
      build_transformation(description, param, element, *tr);
      //dddb_print(tr);
    }

    /// Specialized conversion of <zplane/> entities
    template <> void Conv<DDDBZPlane>::convert(xml_h element) const {
      DDDBShape* s = _param<DDDBShape>();
      dddb_dim_t dim = element;
      DDDBZPlane plane;
      plane.innerRadius = dim.innerRadius(0.0);
      plane.outerRadius = dim.outerRadius(0.0);
      plane.z = dim.z(0.0);
      s->zplanes.push_back(plane);
    }

    /// Specialized common shape conversion
    template <> void ShapeConv<DDDBShape>::convert(xml_h element, DDDBShape*& s) const {
      DDDBContext* context = _param<DDDBContext>();
      xml_dim_t dim = element;
      s = new DDDBShape;
      s->name = dim.nameStr();
      s->path = object_path(context, s->name);
    }

    /// Specialized conversion of assemblies (logical volumes without shapes)
    template <> void ShapeConv<DDDBAssembly>::convert(xml_h , DDDBShape*& s) const {
      s = new DDDBShape();
      s->name = "assembly";
      s->type = to_type::type();
    }

    /// Specialized conversion of Box shapes
    template <> void ShapeConv<DDDBBox>::convert(xml_h element, DDDBShape*& s) const {
      ShapeConv<DDDBShape>(description,param)(element,s);
      dddb_dim_t dim = element;
      s->type = to_type::type();
      s->s.box.x = dim.sizeX(0.0)/2e0;
      s->s.box.y = dim.sizeY(0.0)/2e0;
      s->s.box.z = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of Cons shapes
    template <> void ShapeConv<DDDBCons>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.cons.innerRadiusMZ = dim.innerRadiusMZ(0.0);
      s->s.cons.innerRadiusPZ = dim.innerRadiusPZ(0.0);
      s->s.cons.outerRadiusMZ = dim.outerRadiusMZ(0.0);
      s->s.cons.outerRadiusPZ = dim.outerRadiusPZ(0.0);
      s->s.cons.sizeZ         = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of ConeSegment shapes
    template <> void ShapeConv<DDDBConeSegment>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
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
    template <> void ShapeConv<DDDBTubs>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.tubs.start       = dim.startPhiAngle(0.0);
      s->s.tubs.delta       = dim.deltaPhiAngle(2.0*M_PI);
      s->s.tubs.innerRadius = dim.innerRadius(0.0);
      s->s.tubs.outerRadius = dim.outerRadius(0.0);
      s->s.tubs.sizeZ       = dim.sizeZ(0.0)/2e0;
    }

    /// Specialized conversion of Trap shapes
    template <> void ShapeConv<DDDBTrap>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
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
    template <> void ShapeConv<DDDBPolycone>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.polycone.start = dim.startPhiAngle(0.0);
      s->s.polycone.delta = dim.deltaPhiAngle(2.0*M_PI);
      xml_coll_t(element, _U(zplane)).for_each(Conv<DDDBZPlane>(description,s));
    }

    /// Specialized conversion of Polygon shapes
    template <> void ShapeConv<DDDBPolygon>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.polygon.z      = dim.z();
      s->s.polygon.start  = dim.start(0.0);
      s->s.polygon.nsides = dim.nsides();
      s->s.polygon.innerRadius = dim.innerRadius(0.0);
      s->s.polygon.outerRadius = dim.outerRadius(0.0);
      xml_coll_t(element, _U(zplane)).for_each(Conv<DDDBZPlane>(description,s));
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void ShapeConv<DDDBEllipticalTube>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.ellipticalTube.a  = dim.a();
      s->s.ellipticalTube.b  = dim.b();
      s->s.ellipticalTube.dz = dim.dz();
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void ShapeConv<DDDBTRD>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.trd.x1 = dim.sizeX1()/2e0;
      s->s.trd.x2 = dim.sizeX2()/2e0;
      s->s.trd.y1 = dim.sizeY1()/2e0;
      s->s.trd.y2 = dim.sizeY2()/2e0;
      s->s.trd.z  = dim.sizeZ()/2e0;
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void ShapeConv<DDDBSphere>::convert(xml_h element, DDDBShape*& s) const {
      dddb_dim_t dim = element;
      ShapeConv<DDDBShape>(description,param)(element,s);
      s->type = to_type::type();
      s->s.sphere.rmin        = dim.innerRadius(0);
      s->s.sphere.rmax        = dim.outerRadius(0);
      s->s.sphere.theta       = dim.startThetaAngle(0);
      s->s.sphere.delta_theta = dim.deltaThetaAngle(2.0*M_PI);
      s->s.sphere.phi         = dim.startPhiAngle(0);
      s->s.sphere.delta_phi   = dim.deltaPhiAngle(2.0*M_PI);
    }

    /// Specialized common conversion of boolean shapes
    template <> void ShapeConv<DDDBBooleanShape>::convert(xml_h element, DDDBShape*& s) const {
      ShapeConv<DDDBShape>(description,param)(element,s);
      DDDBContext* context = _param<DDDBContext>();
      string        id = object_path(context,s->name);
      DDDBContext::PreservedLocals locals(context);
      context->locals.obj_path = id;
      s->s.boolean.first = 0;
      for(xml_coll_t p(element,_U(star)); p; ++p)  {
        if ( p.parent() == element ) { // Only next level is processed here
          if ( 0 == s->s.boolean.first )  {
            ShapeConv<DDDBBooleanOperation>(description,context)(p,s->s.boolean.first);
            s->s.boolean.first->id = id + "/" + s->s.boolean.first->name;
            ++p;
          }
          DDDBBooleanOperation op;
          ShapeConv<DDDBBooleanOperation>(description,context)(p,op.shape);
          if ( 0 == op.shape )  {
            // Error. What to do? 
            // Anyhow: ShapeConv<BooleanOperation> throws exception if the
            //         shape is unknown. We never get here.
            printout(ERROR,"ShapeConv","++ Invalid boolean shape operation. [Invalid operand]");
            continue;
          }
          op.shape->id = id + "/" + op.shape->name;
          ++p;
          extract_transformation(description, param, p, op.trafo);
          s->boolean_ops.push_back(op);
        }
      }
    }

    /// Specialized conversion of boolean union shapes
    template <> void ShapeConv<DDDBBooleanUnion>::convert(xml_h element, DDDBShape*& s) const {
      ShapeConv<DDDBBooleanShape>(description,param)(element,s);
      s->type = to_type::type();
    }

    /// Specialized conversion of boolean intersection shapes
    template <> void ShapeConv<DDDBBooleanIntersection>::convert(xml_h element, DDDBShape*& s) const {
      ShapeConv<DDDBBooleanShape>(description,param)(element,s);
      s->type = to_type::type();
    }

    /// Specialized conversion of boolean subtraction shapes
    template <> void ShapeConv<DDDBBooleanSubtraction>::convert(xml_h element, DDDBShape*& s) const {
      ShapeConv<DDDBBooleanShape>(description,param)(element,s);
      s->type = to_type::type();
    }

    /// Specialized conversion of boolean operations
    template <> void ShapeConv<DDDBBooleanOperation>::convert(xml_h element, DDDBShape*& s) const {
      xml_h elt = element;
      string tag = element.tag();
      DDDBContext* context = _param<DDDBContext>();

      // Now convert all possible / supported shapes
      if ( tag == "box" )
        ShapeConv<DDDBBox>(description,context)(elt,s);
      else if ( tag == "cons" )
        ShapeConv<DDDBCons>(description,context)(elt,s);
      else if ( tag == "tubs" )
        ShapeConv<DDDBTubs>(description,context)(elt,s);
      else if ( tag == "trap" )
        ShapeConv<DDDBTrap>(description,context)(elt,s);
      else if ( tag == "polycone" )
        ShapeConv<DDDBPolycone>(description,context)(elt,s);
      else if ( tag == "polygon" )
        ShapeConv<DDDBPolygon>(description,context)(elt,s);
      else if ( tag == "trd" )
        ShapeConv<DDDBTRD>(description,context)(elt,s);
      else if ( tag == "eltu" )
        ShapeConv<DDDBEllipticalTube>(description,context)(elt,s);
      else if ( tag == "sphere" )
        ShapeConv<DDDBSphere>(description,context)(elt,s);
      else if ( tag == "union" )
        ShapeConv<DDDBBooleanUnion>(description,context)(elt,s);
      else if ( tag == "subtraction" )
        ShapeConv<DDDBBooleanSubtraction>(description,context)(elt,s);
      else if ( tag == "intersection" )
        ShapeConv<DDDBBooleanIntersection>(description,context)(elt,s);
      else   {
        xml::dump_tree(element.parent());
        except("BooleanOperation","Unknown shape conversion requested:"+tag);
      }
    }

    /// Specialized conversion of <logvolref/> entities
    template <> void Conv<DDDBLogVolRef>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);

      load_dddb_entity(context, catalog, element, href);
      dddb::Volumes::const_iterator i=context->geo->volumes.find(refid);
      if ( i == context->geo->volumes.end() )  {
        string r = reference_href(element,href);
        printout(ERROR,"LogVolRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
      }
      DDDBLogVol* vol = (*i).second;
      catalog->logvolrefs[vol->id] = vol;
      string path = object_path(context,vol->name);
      context->collectPath(path, vol);
      if ( context->print_logvol )  {
        print_ref("LogVolRef", context, element, href, "Path:"+path);
      }
    }

    /// Specialized conversion of <logvol/> entities
    template <> void Conv<DDDBLogVol>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      string   name    = element.attr<string>(_U(name));
      string   id      = object_href(element, name);
      if ( !_find(id, context->geo->volumes) )  {
        DDDBCatalog* catalog = _option<DDDBCatalog>();
        string   material;
        DDDBLogVol* vol = new DDDBLogVol;
        xml_h elt;
        vol->name = name;
        vol->path = object_path(context,name);
        if ( element.hasAttr(_U(material)) )  {
          vol->material = element.attr<string>(_U(material));
        }
        DDDBShape* s = 0;
        // Now convert all possible / supported shapes
        if ( (elt=element.child(_U(box),false)) )
          ShapeConv<DDDBBox>(description,context)(elt,s);
        else if ( (elt=element.child(_U(cons),false)) )
          ShapeConv<DDDBCons>(description,context)(elt,s);
        else if ( (elt=element.child(_U(tubs),false)) )
          ShapeConv<DDDBTubs>(description,context)(elt,s);
        else if ( (elt=element.child(_U(trap),false)) )
          ShapeConv<DDDBTrap>(description,context)(elt,s);
        else if ( (elt=element.child(_U(trd),false)) )
          ShapeConv<DDDBTRD>(description,context)(elt,s);
        else if ( (elt=element.child(_U(polycone),false)) )
          ShapeConv<DDDBPolycone>(description,context)(elt,s);
        else if ( (elt=element.child(_U(sphere),false)) )
          ShapeConv<DDDBSphere>(description,context)(elt,s);
        else if ( (elt=element.child(_U(union),false)) )
          ShapeConv<DDDBBooleanUnion>(description,context)(elt,s);
        else if ( (elt=element.child(_U(subtraction),false)) )
          ShapeConv<DDDBBooleanSubtraction>(description,context)(elt,s);
        else if ( (elt=element.child(_U(intersection),false)) )
          ShapeConv<DDDBBooleanIntersection>(description,context)(elt,s);
        else  {
          ShapeConv<DDDBAssembly>(description,context)(elt,s);
          // Dump element if not known and processed....
          for(xml_coll_t p(element,_U(star)); p; ++p)  {
            if ( p.parent() == element )  {
              string tag = p.tag();
              if ( tag == "physvol" ) continue;
              if ( tag == "paramphysvol" ) continue;
              if ( tag == "paramphysvol2D" ) continue;
              if ( tag == "paramphysvol3D" ) continue;
              printout(WARNING,"LogVol","++ Unknown and not processed tag found: %s",tag.c_str());
              xml::dump_tree(element);
              break;
            }
          }
        }
        // Now collect the information
        vol->shape = id;
        s->setDocument(context->locals.xml_doc);
        vol->setDocument(context->locals.xml_doc);
        context->collect(id, s);
        context->collect(id, vol);
        if ( catalog ) context->collectPath(vol->path, vol);
        {
          DDDBContext::PreservedLocals locals(context);
          context->locals.obj_path = id;
          xml_coll_t(element, _U(physvol)).for_each(Conv<DDDBPhysVol>(description,context,vol));
          xml_coll_t(element, _LBU(paramphysvol)).for_each(Conv<DDDBParamPhysVol>(description,context,vol));
          xml_coll_t(element, _LBU(paramphysvol2D)).for_each(Conv<DDDBParamPhysVol2D>(description,context,vol));
          xml_coll_t(element, _LBU(paramphysvol3D)).for_each(Conv<DDDBParamPhysVol3D>(description,context,vol));
        }
      }
    }

    /// Specialized conversion of <physvol/> entities
    template <> void Conv<DDDBPhysVol>::fill(xml_h element, DDDBPhysVol* pv) const {
      xml_h      elt;
      dddb_dim_t x_vol   = element;
      string     name    = x_vol.nameStr();
      DDDBContext*   context = _param<DDDBContext>();

      pv->name   = name;
      pv->logvol = x_vol.logvol();
      pv->path   = object_path(context,name);
      build_transformation(description, param, element, pv->trafo, -1);
    }
    template <> void Conv<DDDBPhysVol>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      dddb_dim_t x_vol = element;
      string   name    = x_vol.nameStr();
      string   id      = object_href(element, name);
      if ( !_find(id, context->geo->placements) )  {
        DDDBPhysVol* vol = new DDDBPhysVol();
        this->fill(element, vol);
        context->collect(id, vol);
        context->collectPath(vol->path,vol);
        vol->setDocument(context->locals.xml_doc);
        if ( optional )  {
          _option<DDDBLogVol>()->physvols.push_back(vol);
        }
      }
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void Conv<DDDBParamPhysVol>::fill(xml_h element, DDDBParamPhysVol* pv) const {
      dddb_dim_t x_pv = element.child(_U(physvol));
      dddb_dim_t elt = element;
      Conv<DDDBPhysVol>(description,param).fill(x_pv, pv);
      pv->number1 = elt.number(0);
      build_transformation(description, param, element, pv->trafo1, 0);
    }
    template <> void Conv<DDDBParamPhysVol>::convert(xml_h element) const {
      DDDBParamPhysVol* pv = new DDDBParamPhysVol();
      this->fill(element, pv);
      _option<DDDBLogVol>()->physvols.push_back(pv);
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void Conv<DDDBParamPhysVol2D>::fill(xml_h element, DDDBParamPhysVol2D* pv) const {
      dddb_dim_t elt = element;
      Conv<DDDBParamPhysVol>(description,param).fill(element, pv);
      pv->number1 = elt.number1();
      pv->number2 = elt.number2();
      build_transformation(description, param, element, pv->trafo2, 1);
    }
    template <> void Conv<DDDBParamPhysVol2D>::convert(xml_h element) const {
      DDDBParamPhysVol2D* pv = new DDDBParamPhysVol2D();
      this->fill(element,pv);
      _option<DDDBLogVol>()->physvols.push_back(pv);
    }

    /// Specialized conversion of EllipticalTube shapes
    template <> void Conv<DDDBParamPhysVol3D>::fill(xml_h element, DDDBParamPhysVol3D* pv) const {
      dddb_dim_t elt = element;
      Conv<DDDBParamPhysVol2D>(description,param).fill(element, pv);
      pv->number3 = elt.number3();
      build_transformation(description, param, element, pv->trafo3, 2);
    }
    template <> void Conv<DDDBParamPhysVol3D>::convert(xml_h element) const {
      DDDBParamPhysVol3D* pv = new DDDBParamPhysVol3D();
      this->fill(element,pv);
      _option<DDDBLogVol>()->physvols.push_back(pv);
    }

    /// Specialized conversion of <geometryinfo/> entities
    template <> void Conv<DDDBTabProperty>::convert(xml_h element) const {
      DDDBContext*   context = _param<DDDBContext>();
      dddb_dim_t x_dim   = element;
      string     name    = x_dim.nameStr();
      string     id      = object_href(element, name);
      if ( !_find(id, context->geo->tabproperties) )  {
        DDDBTabProperty* p = new DDDBTabProperty();
        p->name        = name;
        p->path        = object_path(context,name);
        if ( element.hasAttr(_U(type))    ) p->type = element.attr<string>(_U(type));
        if ( element.hasAttr(_LBU(xunit)) ) p->xunit = element.attr<string>(_LBU(xunit));
        if ( element.hasAttr(_LBU(yunit)) ) p->yunit = element.attr<string>(_LBU(yunit));
        if ( element.hasAttr(_LBU(xaxis)) ) p->xaxis = element.attr<string>(_LBU(xaxis));
        if ( element.hasAttr(_LBU(yaxis)) ) p->yaxis = element.attr<string>(_LBU(yaxis));
        context->collect(id, p);
        context->collectPath(p->path, p);
        p->setDocument(context->locals.xml_doc);
      }
    }

    /// Specialized conversion of <tabpropertyref/> entities
    template <> void Conv<DDDBTabPropertyRef>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      string      href = element.attr<string>(_LBU(href));
      string     refid = reference_href(element,href);

      load_dddb_entity(context, catalog, element, href);
      dddb::TabProperties::const_iterator i=context->geo->tabproperties.find(refid);
      if ( i == context->geo->tabproperties.end() )  {
        string r = reference_href(element,href);
        printout(ERROR,"TabPropertyRef","++  MISSING ID: %s Failed to convert ref:%s",refid.c_str(),href.c_str());
        return;
      }
      DDDBTabProperty* p = (*i).second;
      catalog->tabpropertyrefs[p->id] = p;
      string path = object_path(context,p->name);
      context->collectPath(path, p);
      if ( context->print_tabprop )  {
        print_ref("TabPropertyRef", context, element, href, "Path:"+path);
      }
    }

    /// Specialized conversion of <parameter/> entities
    template <> void Conv<DDDBParameter>::convert(xml_h element) const {
      DDDBContext* context  = _param<DDDBContext>();
      string name  = element.attr<string>(_U(name));
      string value = element.attr<string>(_U(value));
      _toDictionary(name,value);
      if ( context->print_params )  {
        printout(INFO,"Parameter","++  %s = %s",name.c_str(),value.c_str());
      }
    }

    /// Specialized conversion of <detelemref/> entities
    template <> void Conv<DDDBDetElemRef>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      string   href    = element.attr<string>(_LBU(href));
      string   refid   = reference_href(element, href);

      xml_h target = find_local_element(element, href, _LBU(detelem));
      if ( target )
        Conv<DDDBDetElem>(description,context,catalog)(target);
      else
        load_dddb_entity(context, catalog, element, href);
      dddb::Catalogs::const_iterator i=context->geo->catalogs.find(refid);
      catalog->catalogrefs[refid] = (i==context->geo->catalogs.end()) ? 0 : (*i).second;
      if ( context->print_detelem_ref )   {
        print_ref("DetElemRef", context, element, href);
      }
    }

    /// Specialized conversion of <detelem/> entities
    template <> void Conv<DDDBDetElem>::convert(xml_h element) const {
      DDDBContext*   context = _param<DDDBContext>();
      dddb*      geo     = context->geo;
      dddb_dim_t x_det   = element;
      string     name    = x_det.nameStr();
      string     id      = object_href(element, name);

      dddb::Catalogs::iterator idet = geo->catalogs.find(id);
      if ( idet == geo->catalogs.end() ) {
        xml_h      elt;
        string     path   = object_path(context, name);
        Path p(path);
        string     parent = p.parent_path().native();
        DDDBCatalog* det  = new DDDBCatalog();
        det->typeID   = 1;
        det->name     = name;
        det->type     = x_det.typeStr();
        det->path     = path;
        det->id       = id;
        det->support  = parent;
        det->setDocument(context->locals.xml_doc);
        if ( context->print_detelem )   {
          printout(INFO,"DetElem","  xml:%s id=%s  [%s/%s] doc:%s obj:%s / %s",
                   element.parent().tag().c_str(), 
                   det->id.c_str(),
                   det->name.c_str(),
                   det->type.c_str(),
                   context->locals.xml_doc->id.c_str(),
                   context->locals.obj_path.c_str(),
                   det->name.c_str()
                   );
        }
        if ( x_det.hasAttr(_LBU(classID)) )  {
          det->classID = element.attr<int>(_LBU(classID));
        }
        // Now extract all availible information from the xml
        if ( (elt=x_det.child(_U(author),false)) )
          Conv<DDDBAuthor>(description,context,&det->author)(elt);
        if ( (elt=x_det.child(_U(version),false)) )
          Conv<DDDBVersion>(description,context,&det->version)(elt);
        xml_coll_t(element, _U(param)).for_each(Conv<DDDBParam>(description,context,det));
        xml_coll_t(element, _LBU(userParameter)).for_each(Conv<DDDBParam>(description,context,det));
        xml_coll_t(element, _LBU(conditioninfo)).for_each(Conv<DDDBConditionInfo>(description,context,det));
        {
          DDDBContext::PreservedLocals locals(context);
          context->locals.obj_path = det->path;
          xml_coll_t(element, _LBU(geometryinfo)).for_each(Conv<DDDBGeometryInfo>(description,context,det));
          xml_coll_t(element, _LBU(detelem)).for_each(Conv<DDDBDetElem>(description,context,det));
          xml_coll_t(element, _LBU(detelemref)).for_each(Conv<DDDBDetElemRef>(description,context,det));
        }
        det->path = det->support + "/" + det->name;
        context->collect(det->id, det);
        context->collectPath(det->path, det);
        context->print(det);
        return;
      }
      printout(DEBUG,"DetElem","ALREADY PROCESSED: xid:%s -> %s",id.c_str(),(*idet).second->path.c_str());
    }

    /// Specialized conversion of references to catalogs
    template <> void Conv<DDDBCatalogRef>::convert(xml_h element) const {
      DDDBContext* context = _param<DDDBContext>();
      DDDBCatalog* catalog = _option<DDDBCatalog>();
      string   href    = element.attr<string>(_LBU(href));
      string   refid   = reference_href(element,href);
#if 0
      if ( href.find("@ChamAlign.xml#") != string::npos )  {
        print_ref("CatalogRef", context, element, href);
      }
#endif
      xml_h target = find_local_element(element, href,_LBU(catalog));
      if ( target )  {
        Conv<DDDBCatalog>(description,context,catalog)(target);
      }
      else  {
        load_dddb_entity(context, catalog, element, href);
      }
      dddb::Catalogs::const_iterator i=context->geo->catalogs.find(refid);
      catalog->catalogrefs[refid] = (i == context->geo->catalogs.end()) ? 0 : (*i).second;
      if ( context->print_catalog_ref )  {
        print_ref("CatalogRef", context, element, href);
      }
    }

    /// Specialized conversion of <catalog/> entities
    template <> void Conv<DDDBCatalog>::convert(xml_h e) const {
      DDDBContext* context = _param<DDDBContext>();
      string   name    = e.attr<string>(_U(name));
      string   id      = object_href(e,name);
      dddb::Catalogs::const_iterator icat = context->geo->catalogs.find(id);

      if ( icat == context->geo->catalogs.end() )  {
        DDDBCatalog* catalog = new DDDBCatalog();
        catalog->name          = name;
        catalog->path          = object_path(context,name);
        printout(DEBUG,"Catalog","PROCESSING:        xid:%s -> %s",id.c_str(),catalog->path.c_str());
        catalog->level         = Increment<DDDBCatalog>::counter();
        catalog->type          = "Logical";
        catalog->support       = "";
        catalog->setDocument(context->locals.xml_doc);
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
          DDDBContext::PreservedLocals locals(context);
          context->locals.obj_path = catalog->path;
          xml_coll_t(e, _U(parameter)).for_each(Conv<DDDBParameter>(description,context,catalog));
          xml_coll_t(e, _U(isotope)).for_each(Conv<DDDBIsotope>(description,context,catalog));
          xml_coll_t(e, _U(element)).for_each(Conv<DDDBElement>(description,context,catalog));
          xml_coll_t(e, _U(material)).for_each(Conv<DDDBMaterial>(description,context,catalog));
          xml_coll_t(e, _U(logvol)).for_each(Conv<DDDBLogVol>(description,context,catalog));

          xml_coll_t(e, _LBU(tabproperty)).for_each(Conv<DDDBTabProperty>(description,context,catalog));

          //xml_coll_t(e, _LBU(condition)).for_each(Conv<DDDBCondition>(description,context,catalog));
          //xml_coll_t(e, _LBU(detelem)).for_each(Conv<DDDBDetElem>(description,context,catalog));
          //xml_coll_t(e, _LBU(catalog)).for_each(Conv<DDDBCatalog>(description,context,catalog));

          xml_coll_t(e, _LBU(elementref)).for_each(Conv<DDDBElementRef>(description,context,catalog));
          xml_coll_t(e, _LBU(materialref)).for_each(Conv<DDDBMaterialRef>(description,context,catalog));
          xml_coll_t(e, _LBU(logvolref)).for_each(Conv<DDDBLogVolRef>(description,context,catalog));
          xml_coll_t(e, _LBU(tabpropertyref)).for_each(Conv<DDDBTabPropertyRef>(description,context,catalog));

          xml_coll_t(e, _LBU(conditionref)).for_each(Conv<DDDBConditionRef>(description,context,catalog));
          xml_coll_t(e, _LBU(catalogref)).for_each(Conv<DDDBCatalogRef>(description,context,catalog));
          xml_coll_t(e, _LBU(detelemref)).for_each(Conv<DDDBDetElemRef>(description,context,catalog));

          xml_coll_t(e, _LBU(catalog)).for_each(Conv<DDDBCatalog>(description,context,catalog));
          xml_coll_t(e, _LBU(detelem)).for_each(Conv<DDDBDetElem>(description,context,catalog));
          xml_coll_t(e, _LBU(condition)).for_each(Conv<Condition>(description,context,catalog));
        }
        return;
      }
      printout(DEBUG,"Catalog","ALREADY PROCESSED: xid:%s -> %s",id.c_str(),(*icat).second->path.c_str());
    }

    /// Specialized conversion of <DDDB/> entities
    template <> void Conv<dddb>::convert(xml_h e) const {
      DDDBCatalog* catalog = 0;
      DDDBContext* context = _param<DDDBContext>();
      xml_coll_t(e, _LBU(block)).for_each(Conv<DDDBBlock>(description,context,catalog));
      xml_coll_t(e, _U(config)).for_each(Conv<DDDBConfig>(description,context,catalog));
      xml_coll_t(e, _U(parameter)).for_each(Conv<DDDBParameter>(description,context,catalog));
      xml_coll_t(e, _U(isotope)).for_each(Conv<DDDBIsotope>(description,context,catalog));
      xml_coll_t(e, _U(element)).for_each(Conv<DDDBElement>(description,context,catalog));
      xml_coll_t(e, _U(material)).for_each(Conv<DDDBMaterial>(description,context,catalog));
      xml_coll_t(e, _U(logvol)).for_each(Conv<DDDBLogVol>(description,context,catalog));
      xml_coll_t(e, _LBU(tabproperty)).for_each(Conv<DDDBTabProperty>(description,context,catalog));

      //xml_coll_t(e, _LBU(condition)).for_each(Conv<Condition>(description,context,catalog));
      //xml_coll_t(e, _LBU(detelem)).for_each(Conv<DDDBDetElem>(description,context,catalog));
      //xml_coll_t(e, _LBU(catalog)).for_each(Conv<DDDBCatalog>(description,context,catalog));

      xml_coll_t(e, _LBU(elementref)).for_each(Conv<DDDBElementRef>(description,context,catalog));
      xml_coll_t(e, _LBU(materialref)).for_each(Conv<DDDBMaterialRef>(description,context,catalog));
      xml_coll_t(e, _LBU(logvolref)).for_each(Conv<DDDBLogVolRef>(description,context,catalog));
      xml_coll_t(e, _LBU(tabpropertyref)).for_each(Conv<DDDBTabPropertyRef>(description,context,catalog));

      xml_coll_t(e, _LBU(conditionref)).for_each(Conv<DDDBConditionRef>(description,context,catalog));
      xml_coll_t(e, _LBU(catalogref)).for_each(Conv<DDDBCatalogRef>(description,context,catalog));
      xml_coll_t(e, _LBU(detelemref)).for_each(Conv<DDDBDetElemRef>(description,context,catalog));

      xml_coll_t(e, _LBU(detelem)).for_each(Conv<DDDBDetElem>(description,context,catalog));
      // First handle the catalog. It may refer to local conditions!
      xml_coll_t(e, _LBU(catalog)).for_each(Conv<DDDBCatalog>(description,context,catalog));
      xml_coll_t(e, _LBU(condition)).for_each(Conv<Condition>(description,context,catalog));
    }

    /// Specialized conversion of <DDDB/> entities
    template <> void Conv<dddb_conditions>::convert(xml_h e) const {
      DDDBCatalog* catalog = 0;
      DDDBContext* context = _param<DDDBContext>();
      context->check = false;
      xml_coll_t(e, _LBU(conditionref)).for_each(Conv<DDDBConditionRef>(description,context,catalog));
      xml_coll_t(e, _LBU(catalogref)).for_each(Conv<DDDBCatalogRef>(description,context,catalog));
      {
        DDDBContext::PreservedLocals locals(context);
        // First handle the catalog. It may refer to local conditions!
        xml_coll_t(e, _LBU(catalog)).for_each(Conv<DDDBCatalog>(description,context,catalog));
        xml_coll_t(e, _LBU(condition)).for_each(Conv<Condition>(description,context,catalog));
      }
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

    void extract_transformation(Detector& description, void* context, xml_coll_t& p, Transform3D& tr, int which)   {
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
            Conv<Transform3D>(description,context,&trafo)(p);
            apply = 99;
          }
          else if ( tag == "posXYZ" && (which<0 || count == which) )   {
            apply_trafo(apply, pos, rot, trafo, tr);
            pos.SetXYZ(dim.x(0.0), dim.y(0.0), dim.z(0.0));
            apply = 1;
            ++count;
          }
          else if ( tag == "posRPhiZ" && (which<0 || count == which) )   {
            ROOT::Math::Polar2D<double> dim2(dim.r(0.0), dim.phi(0.0));
            apply_trafo(apply, pos, rot, trafo, tr);
            pos.SetXYZ(dim2.X(), dim2.Y(), dim.z(0.0));
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
      string p = xml::DocumentHandler::system_path(element);
      if ( hash == 0 )  { // This is a local object. Take the element's path
        p += ref;
        Path path = p;  // Remote object. Parent + local path
        p = path.normalize().native();      
        return p;
      }
      Path path = p;  // Remote object. Parent + local path
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

    template <typename ACTION>
    void load_dddb_entity(DDDBContext*      context,
                          DDDBCatalog*      catalog, 
                          xml_h         element,
                          const string& ref,
                          bool prt)
    {
      size_t hash = ref.find("#");
      if ( hash != 0 )  {
        try {
          DDDBReader*        rdr       = context->resolver;
          DDDBReaderContext* ctx       = (DDDBReaderContext*)rdr->context();
          string doc_path = element.ptr() ? reference_href(element,ref) : ref;
          if ( ref == ctx->match+"/Conditions/Online" )
            doc_path = reference_href(element,ref);
          hash = doc_path.find('#');
          if ( hash != string::npos ) doc_path = doc_path.substr(0,hash);
#if 0
          if ( doc_path.find("conddb:/Rich") != string::npos )  {
            printout(INFO, "load_dddb", "SKIPPING DOC %s", doc_path.c_str());
            return;
          }
#endif
          dddb::Documents& docs = context->geo->documents;
          if ( _find(doc_path,docs) )
            return;

          size_t idx = doc_path.find('[');
          size_t idq = doc_path.find(']');
          string key, fp = doc_path;
          if ( idq != string::npos && idx != string::npos )  {
            key = doc_path.substr(idx+1,idq-idx-1);
            fp = doc_path.substr(0,idx);
          }
          DDDBDocument*      xml_doc   = new DDDBDocument();
          xml_doc->id                  = doc_path;
          xml_doc->name                = context->locals.obj_path;
          xml_doc->context.doc         = xml_doc->id;
          xml_doc->context.event_time  = ctx->event_time;
          xml_doc->context.valid_since = 0;//ctx->valid_since;
          xml_doc->context.valid_until = 0;//ctx->valid_until;
          docs.insert(make_pair(doc_path,xml_doc->addRef()));

          if ( !rdr->isBlocked(doc_path) )   {
            xml::UriContextReader reader(rdr, &xml_doc->context);
            xml_doc_holder_t doc(xml_handler_t().load(fp, &reader));
            xml_h e = doc.root();
            context->print(xml_doc);
            printout(context->print_file_load ? INFO : DEBUG,
                     "load_dddb","Loading document: %s IOV: %ld [%ld,%ld]",
                     xml_doc->id.c_str(), 
                     xml_doc->context.event_time,
                     xml_doc->context.valid_since,
                     xml_doc->context.valid_until);

            if ( e )   {
              if ( !key.empty() )  {
                stringstream str;
                xml::dump_tree(e, str);
                string buffer = str.str();
                while( (idx=buffer.find("-KEY-")) != string::npos )
                  buffer.replace(idx,5,key);
                doc.assign(xml_handler_t().parse(buffer.c_str(),
                                                 buffer.length(),
                                                 doc_path.c_str(),
                                                 &reader));
                e = doc.root();
              }
              DDDBContext::PreservedLocals locals(context);
              context->locals.xml_doc  = xml_doc;
              Conv<ACTION> converter(*context->description, context, catalog);
              context->print_condition = prt;
              if ( prt || context->print_xml )  xml::dump_tree(e);
              converter(e);
            }
            return;
          }
          printout(context->print_file_load ? INFO : DEBUG,
                   "load_dddb","Ignore BLOCKED load %s",ref.c_str());
        }
        catch(const exception& e)  {
          printout(INFO,"load_dddb","Failed to load %s [%s]",ref.c_str(),e.what()); 
       }
        catch(...)   {
          printout(INFO,"load_dddb","Failed to load %s",ref.c_str());
        }
      }
    }

    void config_context(DDDBContext& context, 
                        DDDBHelper*  hlp, 
                        const std::string& doc_path,
                        const std::string& obj_path)  {
      DDDBReader*         rdr     = hlp->reader<DDDBReader>();
      DDDBReaderContext*  ctx     = (DDDBReaderContext*)rdr->context();
      DDDBDocument*       doc     = new DDDBDocument();
      doc->name                   = obj_path;
      doc->id                     = doc_path;
      doc->context.event_time     = ctx->event_time;
      doc->context.valid_since    = ctx->valid_since;
      doc->context.valid_until    = ctx->valid_until;
      context.resolver            = rdr;
      context.geo                 = new dddb();
      context.locals.obj_path     = obj_path;
      context.locals.xml_doc      = doc;
      context.geo->documents.insert(make_pair("Initial_dummy_doc",doc->addRef()));
    }

    /// Plugin entry point.
    template <typename ACTION>
    long load_dddb_objects(Detector& description, int argc, char** argv) {
      DDDBHelper* hlp = description.extension<DDDBHelper>(false);
      if ( hlp )   {
        xml::UriReader*    rdr = hlp->xmlReader();
        DDDBReaderContext* ctx = (DDDBReaderContext*)rdr->context();
        DDDBContext ctxt(&description);
        string sys_id = ctx->match+"//lhcb.xml";
        string obj_path = "/";
        if ( argc == 0 )   {
          return 0;
        }
        if ( argc >= 1 && argv[0] != 0 )  {
          rdr = (xml::UriReader*)argv[0];
        }
        if ( argc >= 2 && argv[1] != 0 )  {
          sys_id = argv[1];
        }
        if ( argc >= 3 && argv[2] != 0 )  {
          obj_path = argv[2];
        }
        if ( argc == 4 && argv[3] != 0 )  {
          long long int evt_time = *(long long int*)argv[3];
          ctx->event_time  = evt_time;
        }
        if ( argc == 5 && argv[3] != 0 && argv[4] != 0)  {
          long long int iov_start = *(long long int*)argv[3];
          long long int iov_end   = *(long long int*)argv[4];
          ctx->event_time  = (iov_start+iov_end)/2;
          ctx->valid_since = iov_start;
          ctx->valid_until = iov_end;
        }
        config_context(ctxt, hlp, sys_id, obj_path);
        load_dddb_entity<ACTION>(&ctxt,0,0,ctxt.locals.xml_doc->id);
        checkParents( &ctxt );
        fixCatalogs( &ctxt );
        /// Transfer ownership from local context to the helper
        hlp->setDetectorDescription( ctxt.geo );
        ctxt.geo = 0;
        return 1;
      }
      except("DDDB","+++ Failed to access cool. No DDDBHelper object defined. Run plugin DDDBInstallHelper.");
      return 0;
    }
  }

  namespace DDDB  {
    long load_dddb_from_uri(Detector& description, int argc, char** argv) {
      return load_dddb_objects<dddb>(description,argc,argv);
    }
    long load_dddb_conditions_from_uri(Detector& description, int argc, char** argv) {
      return load_dddb_objects<dddb_conditions>(description,argc,argv);
    }
    /// Plugin entry point.
    long load_dddb_from_handle(Detector& description, xml_h element) {
      DDDBHelper* helper = description.extension<DDDBHelper>(false);
      if ( helper )   {
        DDDBContext context(&description);
        xml::UriReader* rdr = helper->xmlReader();
        DDDBReaderContext*  ctx = (DDDBReaderContext*)rdr->context();
        config_context(context, helper, ctx->match+"//lhcb.xml", "/");
        /// Convert the XML information
        Conv<dddb> converter(description, &context);
        converter( element );
        checkParents( &context );
        fixCatalogs( &context );
        /// Transfer ownership from local context to the helper
        helper->setDetectorDescription( context.geo );
        context.geo = 0;
        return 1;
      }
      except("DDDB","+++ Failed to access cool. No DDDBHelper object defined. Run plugin DDDBInstallHelper.");
      return 0;
    }
  }
}
DECLARE_XML_DOC_READER(DDDB,load_dddb_from_handle)
DECLARE_APPLY(DDDB_Loader,load_dddb_from_uri)
DECLARE_APPLY(DDDB_ConditionsLoader,load_dddb_conditions_from_uri)

/// Plugin entry point.
static long install_helper(Detector& description, int argc, char** argv) {
  DDDBHelper* helper = description.extension<DDDBHelper>(false);
  if ( !helper )   {
    helper = new DDDBHelper(description);
    if ( argc > 0 )   {
      helper->setXmlReader((xml::UriReader*)argv[0]);
    }
    description.addExtension<DDDBHelper,DDDBHelper>(helper);
  }
  return 1;
}
DECLARE_APPLY(DDDB_InstallHelper,install_helper)
