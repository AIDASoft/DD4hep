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
#include "DDDB/DDDBConversion.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <cstring>
#include <map>

#define CHECK_OBJECT(obj)  if ( !obj )  {                             \
    printout(ERROR,"dddb_print",                                      \
             "++ Attempt to print invalid (null) object of type %s.",	\
             typeName(typeid(obj)).c_str());                          \
    return;                                                           \
  }

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {

  using namespace std;
  using namespace DDDB;

  /// Default constructor
  dddb::dddb() : top(0), structure(0), geometry(0)  {
    InstanceCount::increment(this);
  }

  /// Default destructor
  dddb::~dddb()   {
    // need to release heare all allocated resources.
    releaseObjects(isotopes)();
    releaseObjects(elements)();
    releaseObjects(elementPaths)();

    releaseObjects(materials)();
    releaseObjects(materialPaths)();

    releaseObjects(shapes)();

    releaseObjects(volumes)();
    releaseObjects(volumePaths)();

    releaseObjects(placements)();
    releaseObjects(placementPaths)();

    releaseObjects(tabproperties)();
    releaseObjects(tabpropertyPaths)();

    conditions.clear();
    conditionPaths.clear();
    //releaseObjects(conditions)();
    //releaseObjects(conditionPaths)();
    releaseObjects(catalogs)();
    releaseObjects(catalogPaths)();
    releaseObjects(documents)();
    printout(DEBUG,"dddb","++ All intermediate objects deleted!");
    InstanceCount::decrement(this);
  }

  /// Default constructor
  Named::Named()
    : name(), id(), document(0), refCount(0)
  {
  }

  /// Initializing constructor
  Named::Named(const std::string& c)
    : name(c), id(), document(0), refCount(0)
  {
  }

  /// Copy constructor
  Named::Named(const Named& c)
    : name(c.name), id(c.id), document(c.document), refCount(0)
  {
    if ( document ) document->addRef();
  }

  /// Default destructor
  Named::~Named() {
    if ( document ) document->release();
  }

  /// Assignment operator
  Named& Named::operator=(const Named& c) {
    if ( this != &c )  {
      setDocument(c.document);
      name = c.name;
      id = c.id;
    }
    return *this;
  }

  /// Assign document
  void Named::setDocument(Document* doc)   {
    if ( doc ) doc->addRef();
    if ( document ) document->release();
    document = doc;
  }

  /// Default constructor
  Document::Document() : Named(), context()  {
    InstanceCount::increment(this);
  }

  /// Default destructor
  Document::~Document()   {
    //printout(INFO,"Document","Delete doc %s",c_id());
    InstanceCount::decrement(this);
  }

  /// Default constructor
  Isotope::Isotope() : Named() {
    InstanceCount::increment(this);
  }

  /// Default destructor
  Isotope::~Isotope()   {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  Element::Element() : Named(), density(0), ionization(0), state(UNKNOWN) {
    InstanceCount::increment(this);
  }

  /// Copy constructor
  Element::Element(const Element& e) 
    : Named(e), isotopes(e.isotopes), path(e.path), symbol(e.symbol), 
      atom(e.atom), density(e.density), 
      ionization(e.ionization), state(e.state)
  {
    InstanceCount::increment(this);
  }

  /// Default destructor
  Element::~Element()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  Material::Material() : density(0), pressure(-1), temperature(-1), radlen(0), lambda(0) {
    InstanceCount::increment(this);
  }

  /// Default destructor
  Material::~Material()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  LogVol::LogVol() : Named(), material(), shape(), physvols() {
    InstanceCount::increment(this);
  }

  /// Default destructor
  LogVol::~LogVol()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  PhysVol::PhysVol() : type(PHYSVOL_REGULAR), logvol(), path(), trafo() {
    InstanceCount::increment(this);
  }

  /// Copy constructor
  PhysVol::PhysVol(const PhysVol& c) 
    : Named(c), type(c.type), logvol(c.logvol), path(c.path), trafo(c.trafo) {
    InstanceCount::increment(this);
  }

  /// Default destructor
  PhysVol::~PhysVol()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  Shape::Shape() : type(0), zplanes(), boolean_ops() {
    ::memset(&s.box,0,sizeof(s));
    InstanceCount::increment(this);
  }
  
  /// Default destructor
  Shape::~Shape()   {
    if ( type == BooleanUnion::type() ||
         type == BooleanSubtraction::type() ||
         type == BooleanIntersection::type() )   {
      Shape* shape = s.boolean.first;
      if ( shape ) delete shape;
      for(Operations::iterator i=boolean_ops.begin(); i!=boolean_ops.end(); ++i)
        delete (*i).shape;
      boolean_ops.clear();
    }
    InstanceCount::decrement(this);
  }
 
  /// Default constructor
  Catalog::Catalog() : Named(), level(0), typeID(0) {
    InstanceCount::increment(this);
  }

  /// Default destructor
  Catalog::~Catalog()   {
    InstanceCount::decrement(this);
  }


  /// Default constructor
  TabProperty::TabProperty()    {
    InstanceCount::increment(this);
  }

  /// Default destructor
  TabProperty::~TabProperty()    {
    InstanceCount::decrement(this);
  }

  pair<const Catalog*,string> Catalog::parent(const string& nam)  const  {
    const Catalog* cat = this;
    string rest = nam.substr(cat->path.length()+1);
    size_t idx = rest.find('/');
    string sub = rest.substr(0,idx);
    while( idx != string::npos )  {
      dddb::Catalogs::const_iterator ic = cat->catalogs.find(sub);
      if ( ic != cat->catalogs.end() )  {
        rest = rest.substr(idx+1);
        idx = rest.find('/');
        sub = rest.substr(0,idx);
        cat = (*ic).second;
        continue;
      }
      cat = 0;
    }
    return make_pair(cat,sub);
  }

  template <> void dddb_print(const Position* p)   {
    CHECK_OBJECT(p);
    printout(INFO,"PosXYZ","    PosXYZ: x=%f y=%f z=%f",p->X(),p->Y(),p->Z());
  }

  template <> void dddb_print(const RotationZYX* p)   {
    CHECK_OBJECT(p);
    printout(INFO,"RotXYZ","    RotXYZ: z/phi=%f y/theta=%f x/psi=%f",p->Phi(),p->Theta(),p->Psi());
  }

  template <> void dddb_print(const Transform3D* tr)   {
    CHECK_OBJECT(tr);
    Rotation3D rot;
    Position   pos;
    tr->GetDecomposition(rot,pos);
    double dot_pos = pos.mag2();
    printout(INFO,"Transformation"," Position:%s",dot_pos>1e-10 ? "YES" : "NO");
    if ( dot_pos>1e-10 ) dddb_print(&pos);
  }

  template <> void dddb_print(const Isotope* i)   {
    CHECK_OBJECT(i);
    printout(INFO,"Isotope","++ %-20s A=%6.0f Z=%3.0f Density=%8.2g  id=%s",
             i->c_name(), i->A, i->Z, i->density,i->c_id());
  }

  template <> void dddb_print(const TGeoIsotope* i)   {
    CHECK_OBJECT(i);
    printout(INFO,"TGeoIsotope","++ %-20s N=%4d Z=%3d A=%8.2g",
             i->GetName(), i->GetN(), i->GetZ(), i->GetA());
  }

  template <> void dddb_print(const Element* e)   {
    CHECK_OBJECT(e);
    printout(INFO,"Element","++ %-20s A=%6.0f Z=%3.0f Density=%8g ioni:%8.2g (%d isotopes) id=%s",
             (e->name+"/"+e->symbol).c_str(), e->atom.A, e->atom.Zeff, 
             e->density, e->ionization,int(e->isotopes.size()), e->c_id());
  }

  template <> void dddb_print(const TGeoElement* e)   {
    CHECK_OBJECT(e);
    printout(INFO,"TGeoElement","++ %-20s N=%4d Z=%3d A=%8.2g",
             e->GetName(), e->N(), e->Z(), e->A());
  }

  template <> void dddb_print(const Material* m)   {
    CHECK_OBJECT(m);
    printout(INFO,"Material","++ %-20s Density=%8g P=%f T=%f %d components id:%s",
             ("'"+m->path+"'").c_str(), m->density, m->pressure, m->temperature, 
             int(m->components.size()),m->c_id());
  }

  template <> void dddb_print(const TGeoMedium* m)   {
    CHECK_OBJECT(m);
    TGeoMaterial* mat = m->GetMaterial();
    printout(INFO,"TGeoMedium","++ %-20s Material:%-20s A=%4.0f Z=%3.0f "
             "Radlen:%8.2g IntLen:%8.2g components:%d",
             m->GetName(), mat->GetName(), 
             mat->GetA(), mat->GetZ(), 
             mat->GetRadLen(), mat->GetIntLen(),
             mat->GetNelements());
  }

  template <> void dddb_print(const Shape* s)   {
    CHECK_OBJECT(s);
    if (s-> type == Box::type() )  {
      printout(INFO,"Box","++ %3d/%-12s: x=%f y=%f z=%f",
               s->type,s->c_name(),
               s->s.box.x,s->s.box.y,s->s.box.z);
    }
    else if (s-> type == Cons::type() )  {
      printout(INFO,"Cons","++ %3d/%-12s: z=%f innerMZ=%f innerPZ=%f outerMZ=%f outerPZ=%f",
               s->type,s->c_name(),s->s.cons.sizeZ,
               s->s.cons.innerRadiusMZ,s->s.cons.innerRadiusPZ,
               s->s.cons.outerRadiusMZ,s->s.cons.outerRadiusPZ);
    }
    else if (s-> type == Tubs::type() )  {
      printout(INFO,"Tubs","++ %3d/%-12s: z=%f innerR=%f outerR=%f",
               s->type,s->c_name(),s->s.tubs.sizeZ,
               s->s.tubs.innerRadius,s->s.tubs.outerRadius);
    }
    else if (s-> type == Polycone::type() )  {
      printout(INFO,"Polycone","++ %3d/%-12s: %d zplanes",
               s->type,s->c_name(),int(s->zplanes.size()));
    }
    else if (s-> type == BooleanUnion::type() )  {
      printout(INFO,"union","++ %3d/%-12s: %d modifications",
               s->type,s->c_name(),int(s->boolean_ops.size()));
    }
    else if (s-> type == BooleanSubtraction::type() )  {
      printout(INFO,"subtraction","++ %3d/%-12s: %d modifications",
               s->type,s->c_name(),int(s->boolean_ops.size()));
    }
    else if (s-> type == BooleanIntersection::type() )  {
      printout(INFO,"intersection","++ %3d%-12s: %d modifications",
               s->type,s->path.c_str(),int(s->boolean_ops.size()));
    }
    printout(INFO,"Shape","++ %-15s  name:%s id:%s",
             "", s->path.c_str(), s->c_id());
  }

  template <> void dddb_print(const PhysVol* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"PhysVol","++ %-12s: logvol:%s",
             obj->path.c_str(),obj->logvol.c_str());
    printout(INFO,"PhysVol","++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }

  template <> void dddb_print(const LogVol* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"LogVol", "++ %-12s: Material:%s Shape:%s",
             obj->path.c_str(),obj->material.c_str(),
             obj->shape.empty() ? "<INVALID>" : obj->shape.c_str());
    printout(INFO,"LogVol", "++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }

  template <> void dddb_print(const TabProperty* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"Detector", "++ %-12s: [%s] xunit:%s xaxis:%s yunit:%s yaxis:%s siz:%d",
             obj->path.c_str(), obj->type.c_str(), 
	     obj->xunit.c_str(), obj->xaxis.c_str(),
	     obj->yunit.c_str(), obj->yaxis.c_str(),
	     int(obj->data.size()));
  }

  template <> void dddb_print(const Catalog* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"Detector", "++ %-12s: [%s] %d children support:%s geo:%s",
             obj->path.c_str(), obj->type.c_str(), int(obj->catalogrefs.size()),
             obj->support.c_str(), obj->logvol.c_str());
    printout(INFO,"Detector", "++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }
  template <> void dddb_print(const Document* obj)   {
    CHECK_OBJECT(obj);
    char c_since[64], c_until[64], c_evt[64];
    struct tm since, until, evt;
    time_t t_evt   = obj->context.event_time;
    time_t t_since = obj->context.valid_since;
    time_t t_until = obj->context.valid_until;

    ::gmtime_r(&t_evt,&evt);
    ::gmtime_r(&t_since,&since);
    ::gmtime_r(&t_until,&until);
    ::strftime(c_evt,sizeof(c_evt),"%T %F",&evt);
    ::strftime(c_since,sizeof(c_since),"%T %F",&since);
    ::strftime(c_until,sizeof(c_until),"%T %F",&until);
#if 0
    printout(INFO,"Document", "++ %8ld [%8ld - %8ld] %s",
	     long(double(obj->context.event_time)/1e9),
	     long(double(obj->context.valid_since)/1e9),
	     long(double(obj->context.valid_until)/1e9),
	     obj->c_id());
#endif
    printout(INFO,"Document", "++ %s [%s - %s] %s",c_evt,c_since,c_until,obj->c_id());
  }
}
