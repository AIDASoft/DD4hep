#include "DD4hep/Objects.h"
#include "DD4hep/Detector.h"
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

#include "TEveGeoNode.h"
#include "TEveElement.h"
#include "TEveTrans.h"

using namespace DD4hep;
using namespace DD4hep::Geometry;
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
  TEveElementList* s = dynamic_cast<TEveElementList*>(e);
  if ( !s )  {
    return;
  }
  else if ( level == 1 )  {
    SetRnrChildren(s, visible);
    s->SetRnrSelf(true);
  }
  else if ( level == 0 )  {
    s->SetRnrSelf(visible);
  }
  else   {
    SetRnrAll(s, visible);
    s->SetRnrSelf(visible);
  }
}

std::pair<bool,TEveElement*> 
Utilities::createEveShape(int level, int max_level, TEveElement* p, TGeoNode* n, TGeoHMatrix mat)  {
  TGeoVolume* vol = n ? n->GetVolume() : 0;
  bool created = false;

  if ( 0 == vol || level >= max_level ) return make_pair(created,(TEveElement*)0);

  VisAttr vis(Volume(vol).visAttributes());
  TGeoShape* geoShape = vol->GetShape();
  TEveElement* element = 0;

  if ( p )   {
    element = (TEveGeoShape*)p->FindChild(n->GetName());
    if ( element ) goto Daughters;
  }  

  if ( geoShape->IsA() == TGeoShapeAssembly::Class() )  {
    //printout(INFO,"createEveShape","+++ SKIP assembly Shape %s.",n->GetName());
    ElementList* shape = new ElementList(n->GetName(),n->GetName(),true,true);
    shape->SetUserData(n);
    shape->SetMainTransparency(true);
    shape->SetMainAlpha(0.5);
    shape->SetPickable(kTRUE);
    if ( vis.isValid() )  {
      float r,g,b;
      vis.rgb(r,g,b);
      shape->SetMainColorRGB(r,g,b);
    }
    element = shape;
    created = true;
    goto Daughters;
  }  
  else if ( 0 == element )  {
    TEveGeoShape* shape = new TEveGeoShape(n->GetName());
    //printout(INFO,"createEveShape","+++ Create TEveGeoShape %s [%s].",
    //n->GetName(),geoShape->IsA()->GetName());
    created = true;
    if ( vis.isValid() )  {
      float r,g,b;
      vis.rgb(r,g,b);
      shape->SetMainColorRGB(r,g,b);
    }
    shape->SetUserData(n);
    shape->SetMainTransparency(true);
    shape->SetMainAlpha(0.5);
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
    TGeoHMatrix dau_mat = mat;
    TGeoMatrix* matrix = daughter->GetMatrix();
    dau_mat.Multiply(matrix);
    pair<bool,TEveElement*> dau_shape = 
      createEveShape(level+1, max_level, element, daughter, dau_mat);
    if ( dau_shape.first )  {
      element->AddElement(dau_shape.second);
    }
  }
  return make_pair(created,element);
}

bool Utilities::findNodeWithMatrix(TGeoNode* p, TGeoNode* n, TGeoHMatrix* mat, string* sub_path)  {
  if ( p == n ) return true;
  for (Int_t idau = 0, ndau = p->GetNdaughters(); idau < ndau; ++idau) {
    string spath;
    TGeoNode*   daughter = p->GetDaughter(idau);
    TGeoHMatrix* daughter_matrix = 0;
    if ( mat )  {
      TGeoHMatrix dau_mat  = *mat;
      TGeoMatrix* matrix   = daughter->GetMatrix();
      dau_mat.Multiply(matrix);
      daughter_matrix = &dau_mat;
    }
    bool found = findNodeWithMatrix(daughter,n,daughter_matrix,sub_path ? &spath : 0);
    if ( found )  {
      if ( sub_path )  {
	*sub_path += "/";
	*sub_path += spath;
      }
      if ( mat ) *mat = *daughter_matrix;
      return true;
    }
  }
  return false;
}

std::pair<bool,TEveElement*> Utilities::LoadDetElement(Geometry::DetElement de,int levels, TEveElement* parent)  {
  if ( !strstr(de.name(),"BeamCal") )   {
    PlacedVolume pv = de.placement();
    if (pv.isValid()) {
      TGeoNode* n = pv.ptr();
      TGeoMatrix* matrix = n->GetMatrix();
      gGeoManager = 0;
      gGeoManager = new TGeoManager();
      std::pair<bool,TEveElement*> e = createEveShape(0, levels, parent, n, *matrix);
      TEveElementList* list = dynamic_cast<TEveElementList*>(e.second);
      if ( list )  {
	list->SetName(de.name());
      }
      return e;
    }
  }
  return make_pair(false,(TEveGeoShape*)0);
}
