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
#include "DDDB/DDDBConversion.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"

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
namespace dd4hep  {

  using namespace std;
  using namespace DDDB;

  /// Default constructor
  dddb::dddb() : world{0E0,0E0,0E0}, top(0), structure(0), geometry(0)  {
    InstanceCount::increment(this);
  }

  /// Default destructor
  dddb::~dddb()   {
    // need to release heare all allocated resources.
    detail::releaseObjects(isotopes);
    detail::releaseObjects(elements);
    detail::releaseObjects(elementPaths);

    detail::releaseObjects(materials);
    detail::releaseObjects(materialPaths);

    detail::releaseObjects(shapes);

    detail::releaseObjects(volumes);
    detail::releaseObjects(volumePaths);

    detail::releaseObjects(placements);
    detail::releaseObjects(placementPaths);

    detail::releaseObjects(tabproperties);
    detail::releaseObjects(tabpropertyPaths);

    conditions.clear();
    conditionPaths.clear();
    //detail::releaseObjects(conditions);
    //detail::releaseObjects(conditionPaths);
    detail::releaseObjects(catalogs);
    detail::releaseObjects(catalogPaths);
    detail::releaseObjects(documents);
    printout(DEBUG,"dddb","++ All intermediate objects deleted!");
    InstanceCount::decrement(this);
  }

  /// Initializing constructor
  DDDBNamed::DDDBNamed(const std::string& c)
    : name(c), id(), document(0), refCount(0)
  {
  }

  /// Copy constructor
  DDDBNamed::DDDBNamed(const DDDBNamed& c)
    : name(c.name), id(c.id), document(c.document), refCount(0)
  {
    if ( document ) document->addRef();
  }

  /// Default destructor
  DDDBNamed::~DDDBNamed() {
    if ( document ) document->release();
  }

  /// Assignment operator
  DDDBNamed& DDDBNamed::operator=(const DDDBNamed& c) {
    if ( this != &c )  {
      setDocument(c.document);
      name = c.name;
      id = c.id;
    }
    return *this;
  }

  /// Assign document
  void DDDBNamed::setDocument(DDDBDocument* doc)   {
    if ( doc ) doc->addRef();
    if ( document ) document->release();
    document = doc;
  }

  /// Default constructor
  DDDBDocument::DDDBDocument() : DDDBNamed(), context()  {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBDocument::~DDDBDocument()   {
    //printout(INFO,"Document","Delete doc %s",c_id());
    InstanceCount::decrement(this);
  }

  /// Default constructor
  DDDBIsotope::DDDBIsotope() : DDDBNamed() {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBIsotope::~DDDBIsotope()   {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  DDDBElement::DDDBElement() : DDDBNamed(), density(0), ionization(0), state(UNKNOWN) {
    InstanceCount::increment(this);
  }

  /// Copy constructor
  DDDBElement::DDDBElement(const DDDBElement& e) 
    : DDDBNamed(e), isotopes(e.isotopes), path(e.path), symbol(e.symbol), 
      atom(e.atom), density(e.density), 
      ionization(e.ionization), state(e.state)
  {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBElement::~DDDBElement()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  DDDBMaterial::DDDBMaterial() : density(0), pressure(-1), temperature(-1), radlen(0), lambda(0) {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBMaterial::~DDDBMaterial()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  DDDBLogVol::DDDBLogVol() : DDDBNamed(), material(), shape(), physvols() {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBLogVol::~DDDBLogVol()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  DDDBPhysVol::DDDBPhysVol() : type(PHYSVOL_REGULAR), logvol(), path(), trafo() {
    InstanceCount::increment(this);
  }

  /// Copy constructor
  DDDBPhysVol::DDDBPhysVol(const DDDBPhysVol& c) 
    : DDDBNamed(c), type(c.type), logvol(c.logvol), path(c.path), trafo(c.trafo) {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBPhysVol::~DDDBPhysVol()  {
    InstanceCount::decrement(this);
  }

  /// Default constructor
  DDDBShape::DDDBShape() : type(0), zplanes(), boolean_ops() {
    ::memset(&s,0,sizeof(s));
    InstanceCount::increment(this);
  }
  
  /// Default destructor
  DDDBShape::~DDDBShape()   {
    if ( type == DDDBBooleanUnion::type() ||
         type == DDDBBooleanSubtraction::type() ||
         type == DDDBBooleanIntersection::type() )   {
      DDDBShape* shape = s.boolean.first;
      if ( shape ) delete shape;
      for(Operations::iterator i=boolean_ops.begin(); i!=boolean_ops.end(); ++i)
        delete (*i).shape;
      boolean_ops.clear();
    }
    InstanceCount::decrement(this);
  }
 
  /// Default constructor
  DDDBCatalog::DDDBCatalog() {
    InstanceCount::increment(this);
  }

  /// Extension constructor
  DDDBCatalog::DDDBCatalog(const DDDBCatalog& copy, const DetElement&) {
    *this = copy;
  }

  /// Default destructor
  DDDBCatalog::~DDDBCatalog()   {
    InstanceCount::decrement(this);
  }


  /// Default constructor
  DDDBTabProperty::DDDBTabProperty()    {
    InstanceCount::increment(this);
  }

  /// Default destructor
  DDDBTabProperty::~DDDBTabProperty()    {
    InstanceCount::decrement(this);
  }

  pair<const DDDBCatalog*,string> DDDBCatalog::parent(const string& nam)  const  {
    const DDDBCatalog* cat = this;
    string rest = nam.substr(cat->path.length()+1);
    size_t idx = rest.find('/');
    string sub = rest.substr(0,idx);
    while( cat && idx != string::npos )  {
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

  template <> void dddb_print(const DDDBIsotope* i)   {
    CHECK_OBJECT(i);
    printout(INFO,"Isotope","++ %-20s A=%6.0f Z=%3.0f Density=%8.2g  id=%s",
             i->c_name(), i->A, i->Z, i->density,i->c_id());
  }

  template <> void dddb_print(const TGeoIsotope* i)   {
    CHECK_OBJECT(i);
    printout(INFO,"TGeoIsotope","++ %-20s N=%4d Z=%3d A=%8.2g",
             i->GetName(), i->GetN(), i->GetZ(), i->GetA());
  }

  template <> void dddb_print(const DDDBElement* e)   {
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

  template <> void dddb_print(const DDDBMaterial* m)   {
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

  template <> void dddb_print(const DDDBShape* s)   {
    CHECK_OBJECT(s);
    if (s-> type == DDDBBox::type() )  {
      printout(INFO,"Box","++ %3d/%-12s: x=%f y=%f z=%f",
               s->type,s->c_name(),
               s->s.box.x,s->s.box.y,s->s.box.z);
    }
    else if (s-> type == DDDBCons::type() )  {
      printout(INFO,"Cons","++ %3d/%-12s: z=%f innerMZ=%f innerPZ=%f outerMZ=%f outerPZ=%f",
               s->type,s->c_name(),s->s.cons.sizeZ,
               s->s.cons.innerRadiusMZ,s->s.cons.innerRadiusPZ,
               s->s.cons.outerRadiusMZ,s->s.cons.outerRadiusPZ);
    }
    else if (s-> type == DDDBTubs::type() )  {
      printout(INFO,"Tubs","++ %3d/%-12s: z=%f innerR=%f outerR=%f",
               s->type,s->c_name(),s->s.tubs.sizeZ,
               s->s.tubs.innerRadius,s->s.tubs.outerRadius);
    }
    else if (s-> type == DDDBPolycone::type() )  {
      printout(INFO,"Polycone","++ %3d/%-12s: %d zplanes",
               s->type,s->c_name(),int(s->zplanes.size()));
    }
    else if (s-> type == DDDBBooleanUnion::type() )  {
      printout(INFO,"union","++ %3d/%-12s: %d modifications",
               s->type,s->c_name(),int(s->boolean_ops.size()));
    }
    else if (s-> type == DDDBBooleanSubtraction::type() )  {
      printout(INFO,"subtraction","++ %3d/%-12s: %d modifications",
               s->type,s->c_name(),int(s->boolean_ops.size()));
    }
    else if (s-> type == DDDBBooleanIntersection::type() )  {
      printout(INFO,"intersection","++ %3d%-12s: %d modifications",
               s->type,s->path.c_str(),int(s->boolean_ops.size()));
    }
    printout(INFO,"Shape","++ %-15s  name:%s id:%s",
             "", s->path.c_str(), s->c_id());
  }

  template <> void dddb_print(const DDDBPhysVol* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"PhysVol","++ %-12s: logvol:%s",
             obj->path.c_str(),obj->logvol.c_str());
    printout(INFO,"PhysVol","++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }

  template <> void dddb_print(const DDDBLogVol* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"LogVol", "++ %-12s: Material:%s Shape:%s",
             obj->path.c_str(),obj->material.c_str(),
             obj->shape.empty() ? "<INVALID>" : obj->shape.c_str());
    printout(INFO,"LogVol", "++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }

  template <> void dddb_print(const DDDBTabProperty* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"Detector", "++ %-12s: [%s] xunit:%s xaxis:%s yunit:%s yaxis:%s siz:%d",
             obj->path.c_str(), obj->type.c_str(), 
	     obj->xunit.c_str(), obj->xaxis.c_str(),
	     obj->yunit.c_str(), obj->yaxis.c_str(),
	     int(obj->data.size()));
  }

  template <> void dddb_print(const DDDBCatalog* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"Detector", "++ %-12s: [%s] %d children support:%s geo:%s",
             obj->path.c_str(), obj->type.c_str(), int(obj->catalogrefs.size()),
             obj->support.c_str(), obj->logvol.c_str());
    printout(INFO,"Detector", "++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }
  template <> void dddb_print(const DDDBDocument* obj)   {
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
