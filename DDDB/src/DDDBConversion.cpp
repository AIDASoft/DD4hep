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

  /// Default constructor
  DDDB::dddb::dddb() : top(0), structure(0), geometry(0)  {
  }

  /// Default destructor
  DDDB::dddb::~dddb()   {
    // need to release heare all allocated resources.
    destroyObjects(catalogs)();
  }

  /// Default constructor
  DDDB::Shape::Shape() : type(0), zplanes(), boolean_ops() {
    ::memset(&s.box,0,sizeof(s));
  }

  pair<const DDDB::Catalog*,string> DDDB::Catalog::parent(const string& nam)  const  {
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

  template <> void dddb_print(const DDDB::Position* p)   {
    CHECK_OBJECT(p);
    printout(INFO,"PosXYZ","    PosXYZ: x=%f y=%f z=%f",p->X(),p->Y(),p->Z());
  }

  template <> void dddb_print(const DDDB::RotationZYX* p)   {
    CHECK_OBJECT(p);
    printout(INFO,"RotXYZ","    RotXYZ: z/phi=%f y/theta=%f x/psi=%f",p->Phi(),p->Theta(),p->Psi());
  }

  template <> void dddb_print(const DDDB::Transform3D* tr)   {
    CHECK_OBJECT(tr);
    DDDB::Rotation3D rot;
    DDDB::Position   pos;
    tr->GetDecomposition(rot,pos);
    double dot_pos = pos.mag2();
    printout(INFO,"Transformation"," Position:%s",dot_pos>1e-10 ? "YES" : "NO");
    if ( dot_pos>1e-10 ) dddb_print(&pos);
  }

  template <> void dddb_print(const DDDB::Isotope* i)   {
    CHECK_OBJECT(i);
    printout(INFO,"Isotope","++ %-20s A=%6.0f Z=%3.0f Density=%8.2g  id=%s",
             i->c_name(), i->A, i->Z, i->density,i->c_id());
  }

  template <> void dddb_print(const TGeoIsotope* i)   {
    CHECK_OBJECT(i);
    printout(INFO,"TGeoIsotope","++ %-20s N=%4d Z=%3d A=%8.2g",
             i->GetName(), i->GetN(), i->GetZ(), i->GetA());
  }

  template <> void dddb_print(const DDDB::Element* e)   {
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

  template <> void dddb_print(const DDDB::Material* m)   {
    CHECK_OBJECT(m);
    printout(INFO,"Material","++ %-20s Density=%8g P=%f T=%f %d components id:%s",
             ("'"+m->name+"'").c_str(), m->density, m->pressure, m->temperature, 
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

  template <> void dddb_print(const DDDB::Shape* s)   {
    CHECK_OBJECT(s);
    if (s-> type == DDDB::Box::type() )  {
      printout(INFO,"Box","++ %3d/%-12s: x=%f y=%f z=%f",
               s->type,s->c_name(),
               s->s.box.x,s->s.box.y,s->s.box.z);
    }
    else if (s-> type == DDDB::Cons::type() )  {
      printout(INFO,"Cons","++ %3d/%-12s: z=%f innerMZ=%f innerPZ=%f outerMZ=%f outerPZ=%f",
               s->type,s->c_name(),s->s.cons.sizeZ,
               s->s.cons.innerRadiusMZ,s->s.cons.innerRadiusPZ,
               s->s.cons.outerRadiusMZ,s->s.cons.outerRadiusPZ);
    }
    else if (s-> type == DDDB::Tubs::type() )  {
      printout(INFO,"Tubs","++ %3d/%-12s: z=%f innerR=%f outerR=%f",
               s->type,s->c_name(),s->s.tubs.sizeZ,
               s->s.tubs.innerRadius,s->s.tubs.outerRadius);
    }
    else if (s-> type == DDDB::Polycone::type() )  {
      printout(INFO,"Polycone","++ %3d/%-12s: %d zplanes",
               s->type,s->c_name(),int(s->zplanes.size()));
    }
    else if (s-> type == DDDB::BooleanUnion::type() )  {
      printout(INFO,"union","++ %3d/%-12s: %d modifications",
               s->type,s->c_name(),int(s->boolean_ops.size()));
    }
    else if (s-> type == DDDB::BooleanSubtraction::type() )  {
      printout(INFO,"subtraction","++ %3d/%-12s: %d modifications",
               s->type,s->c_name(),int(s->boolean_ops.size()));
    }
    else if (s-> type == DDDB::BooleanIntersection::type() )  {
      printout(INFO,"intersection","++ %3d%-12s: %d modifications",
               s->type,s->path.c_str(),int(s->boolean_ops.size()));
    }
    printout(INFO,"Shape","++ %-15s  name:%s id:%s",
             "",s->c_name(),s->c_id());
  }

  template <> void dddb_print(const DDDB::PhysVol* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"PhysVol","++ %-12s: logvol:%s",
             obj->path.c_str(),obj->logvol.c_str());
    printout(INFO,"PhysVol","++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }

  template <> void dddb_print(const DDDB::LogVol* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"LogVol", "++ %-12s: Material:%s Shape:%s",
             obj->path.c_str(),obj->material.c_str(),
             obj->shape.empty() ? "<INVALID>" : obj->shape.c_str());
    printout(INFO,"LogVol", "++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }

  template <> void dddb_print(const DDDB::Catalog* obj)   {
    CHECK_OBJECT(obj);
    printout(INFO,"Detector", "++ %-12s: [%s] %d children support:%s geo:%s",
             obj->path.c_str(), obj->type.c_str(), int(obj->catalogrefs.size()),
             obj->support.c_str(), obj->logvol.c_str());
    printout(INFO,"Detector", "++ %-12s  name:%s id:%s",
             "",obj->c_name(),obj->c_id());
  }
}
