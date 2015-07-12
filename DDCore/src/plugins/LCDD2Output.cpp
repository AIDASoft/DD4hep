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

#include "XML/Conversions.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DD4hep/IDDescriptor.h"

#include "TMap.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include <iostream>
#include <iomanip>

using namespace std;
namespace DD4hep {
  using namespace Geometry;

  void dumpNode(TGeoNode* n, int level) {
    TGeoMatrix* mat = n->GetMatrix();
    TGeoVolume* vol = n->GetVolume();
    TGeoMedium* med = vol->GetMedium();
    TGeoShape* shape = vol->GetShape();
    TObjArray* nodes = vol->GetNodes();
    for (int i = 0; i < level; ++i)
      cout << " ";
    cout << " ++Node:|" << n->GetName() << "| ";
    cout << " Volume: " << vol->GetName() << " material:" << med->GetName() << " shape:" << shape->GetName() << endl;
    for (int i = 0; i < level; ++i)
      cout << " ";
    const Double_t* tr = mat->GetTranslation();
    cout << "         matrix:|" << mat->GetName() << "|" << mat->IsTranslation() << mat->IsRotation() << mat->IsScale()
         << " tr:x=" << tr[0] << " y=" << tr[1] << " z=" << tr[2];
    if (mat->IsRotation()) {
      Double_t theta, phi, psi;
      TGeoRotation rot(*mat);
      rot.GetAngles(phi, theta, psi);
      cout << " rot: theta:" << theta << " phi:" << phi << " psi:" << psi;
    }
    cout << endl;
    PlacedVolume plv(n);
    for (int i = 0; i < level; ++i)
      cout << " ";
    cout << "         volume:" << plv.toString();
    cout << endl;
    TIter next(nodes);
    TGeoNode *geoNode;
    while ((geoNode = (TGeoNode *) next())) {
      dumpNode(geoNode, level + 1);
    }
  }

  void dumpVolume(TGeoVolume* vol, int level) {
    TObjArray* nodes = vol->GetNodes();
    TGeoMedium* med = vol->GetMedium();
    TGeoShape* shape = vol->GetShape();

    for (int i = 0; i < level; ++i)
      cout << " ";
    cout << "++Volume: " << vol->GetName() << " material:" << med->GetName() << " shape:" << shape->GetName() << endl;
    TIter next(nodes);
    TGeoNode *geoNode;
    while ((geoNode = (TGeoNode *) next())) {
      dumpNode(geoNode, level + 1);
    }
  }

  void dumpTopVolume(const LCDD& lcdd) {
    dumpVolume(lcdd.manager().GetTopVolume(), 0);
  }
}
