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

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Printout.h"
#include "DDEve/Utilities.h"
#include "DDEve/EventHandler.h"
#include "DDEve/ElementList.h"

#include "TGeoNode.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGeoShapeAssembly.h"

#include "TEveGeoShape.h"
#include "TEveGeoNode.h"
#include "TEveElement.h"
#include "TEveTrans.h"

using namespace dd4hep;
using namespace dd4hep::detail;
using namespace std;

/// Set the rendering flags for the object and the next level children
void Utilities::SetRnrChildren(TEveElementList* l, bool b)  {
  l->SetRnrChildren(b);
  for(TEveElementList::List_i i=l->BeginChildren(); i!=l->EndChildren(); ++i)  {
    (*i)->SetRnrChildren(b);
  }
}

/// Recursively set the rendering flags for the object ans its children
void Utilities::SetRnrAll(TEveElementList* l, bool b)  {
  l->SetRnrSelfChildren(b,b);
  for(TEveElementList::List_i i=l->BeginChildren(); i!=l->EndChildren(); ++i)  {
    TEveElement* e = *i;
    TEveElementList* ll = dynamic_cast<TEveElementList*>(e);
    if ( ll ) SetRnrAll(ll, b);
    else  e->SetRnrSelfChildren(b,b);
  }
}

/// Make a set of nodes starting from a top element (in-)visible with a given depth
void Utilities::MakeNodesVisible(TEveElement* e, bool visible, int level)   {
  TEveElementList* lst = dynamic_cast<TEveElementList*>(e);
  if ( !lst )  {
    return;
  }
  else if ( level == 1 )  {
    SetRnrChildren(lst, visible);
    lst->SetRnrSelf(true);
  }
  else if ( level == 0 )  {
    lst->SetRnrSelf(visible);
  }
  else   {
    SetRnrAll(lst, visible);
    lst->SetRnrSelf(visible);
  }
}

std::pair<bool,TEveElement*> 
Utilities::createEveShape(int level,
                          int max_level,
                          TEveElement* p,
                          TGeoNode* n,
                          const TGeoHMatrix& mat,
                          const std::string& nam)
{
  TGeoVolume* vol = n ? n->GetVolume() : 0;
  bool created = false;

  if ( 0 == vol || level > max_level ) return make_pair(created,(TEveElement*)0);

  VisAttr vis(Volume(vol).visAttributes());
  TGeoShape* geoShape = vol->GetShape();
  TEveElement* element = 0;

  if ( p )   {
    TGeoNode* pn = (TGeoNode*)p->GetUserData();
    if ( pn == n )  {
      element = p;
    }
    if ( !element )  {
      element = (TEveGeoShape*)p->FindChild(n->GetName());
    }
    if ( !element && !nam.empty() )  {
      element = (TEveGeoShape*)p->FindChild(nam.c_str());
    }
    if ( element ) goto Daughters;
  }  

  if ( geoShape->IsA() == TGeoShapeAssembly::Class() )  {
    //printout(INFO,"createEveShape","+++ Assembly Shape %s Userdata:%p.",n->GetName(),n);
    ElementList* shape = new ElementList(n->GetName(),n->GetName(),true,true);
    shape->SetUserData(n);
    shape->SetMainTransparency(true);
    shape->SetMainAlpha(0.2);
    shape->SetPickable(kTRUE);
    if ( vis.isValid() )  {
      float red, green, blue;
      vis.rgb(red, green, blue);
      shape->SetMainColorRGB(red, green, blue);
    }
    element = shape;
    created = true;
    goto Daughters;
  }  
  else if ( 0 == element )  {
    TEveGeoShape* shape = new TEveGeoShape(n->GetName());
    //printout(INFO,"createEveShape","+++ Create TEveGeoShape %s [%s] Userdata:%p.",
    //n->GetName(),geoShape->IsA()->GetName(),n);
    created = true;
    if ( vis.isValid() )  {
      float red, green, blue;
      vis.rgb(red, green, blue);
      shape->SetMainColorRGB(red, green, blue);
    }
    shape->SetMainTransparency(true);
    shape->SetMainAlpha(0.2);
    shape->SetPickable(kTRUE);
    shape->RefMainTrans().SetFrom(mat);
    shape->SetShape((TGeoShape*)geoShape->Clone());
    if ( level < max_level ) {
      shape->SetRnrSelfChildren(true,true);
    }
    else if ( level == max_level ) {
      shape->SetRnrSelfChildren(true,false);
    }
    else if ( level > max_level ) {
      shape->SetRnrSelfChildren(false,false);
    }
    shape->SetUserData(n);
    element = shape;
  }

 Daughters:
  for (Int_t idau = 0, ndau = n->GetNdaughters(); idau < ndau; ++idau) {
    TGeoNode* daughter = n->GetDaughter(idau);
    TGeoHMatrix dau_mat(mat);
    TGeoMatrix* matrix = daughter->GetMatrix();
    dau_mat.Multiply(matrix);
    pair<bool,TEveElement*> dau_shape = 
      createEveShape(level+1, max_level, element, daughter, dau_mat, daughter->GetName());
    if ( dau_shape.first )  {
      element->AddElement(dau_shape.second);
    }
  }
  return make_pair(created,element);
}

int Utilities::findNodeWithMatrix(TGeoNode* p, TGeoNode* n, TGeoHMatrix* mat, string* sub_path)  {
  if ( p == n ) return 1;
  TGeoHMatrix dau_mat;
  for (Int_t idau = 0, ndau = p->GetNdaughters(); idau < ndau; ++idau) {
    string spath;
    TGeoNode*   daughter = p->GetDaughter(idau);
    TGeoHMatrix* daughter_matrix = 0;
    if ( mat )  {
      TGeoMatrix* matrix = daughter->GetMatrix();
      dau_mat = *mat;
      dau_mat.Multiply(matrix);
      daughter_matrix = &dau_mat;
    }
    int level = findNodeWithMatrix(daughter,n,daughter_matrix,sub_path ? &spath : 0);
    if ( level>0 )  {
      if ( sub_path )  {
        *sub_path += "/";
        *sub_path += spath;
      }
      if ( mat ) *mat = *daughter_matrix;
      return level+1;
    }
  }
  return 0;
}

std::pair<bool,TEveElement*> Utilities::LoadDetElement(DetElement de,int levels, TEveElement* parent)  {
  if ( !strstr(de.name(),"BeamCal") )   {
    PlacedVolume pv = de.placement();
    if (pv.isValid()) {
      TGeoNode* n = pv.ptr();
      TGeoMatrix* matrix = n->GetMatrix();
      std::pair<bool,TEveElement*> e = createEveShape(0, levels, parent, n, *matrix, de.name());
      TEveElementList* list = dynamic_cast<TEveElementList*>(e.second);
      if ( list )  {
        list->SetName(de.name());
      }
      return e;
    }
  }
  return make_pair(false,(TEveGeoShape*)0);
}
