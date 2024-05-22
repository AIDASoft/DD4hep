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

// Framework includes
#include <XML/Conversions.h>
#include <DD4hep/Detector.h>
#include <DD4hep/Objects.h>
#include <DD4hep/Printout.h>
#include <DD4hep/IDDescriptor.h>

// ROOT includes
#include <TMap.h>
#include <TROOT.h>
#include <TColor.h>
#include <TGeoMatrix.h>
#include <TGeoManager.h>

/// C/C++ include files
#include <iostream>
#include <iomanip>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  void dumpNode(TGeoNode* n, int level) {
    TGeoMatrix* mat = n->GetMatrix();
    TGeoVolume* vol = n->GetVolume();
    TGeoMedium* med = vol->GetMedium();
    TGeoShape*  shape = vol->GetShape();
    TObjArray*  nodes = vol->GetNodes();
    for (int i = 0; i < level; ++i)
      std::cout << " ";
    std::cout << " ++Node:|" << n->GetName() << "| ";
    std::cout << " Volume: " << vol->GetName() << " material:" << med->GetName()
              << " shape:" << shape->GetName() << std::endl;
    for (int i = 0; i < level; ++i)
      std::cout << " ";
    const Double_t* tr = mat->GetTranslation();
    std::cout << "         matrix:|" << mat->GetName()
              << "|" << mat->IsTranslation() << mat->IsRotation() << mat->IsScale()
              << " tr:x=" << tr[0] << " y=" << tr[1] << " z=" << tr[2];
    if (mat->IsRotation()) {
      Double_t theta, phi, psi;
      TGeoRotation rot(*mat);
      rot.GetAngles(phi, theta, psi);
      std::cout << " rot: theta:" << theta << " phi:" << phi << " psi:" << psi;
    }
    std::cout << std::endl;
    PlacedVolume plv(n);
    for (int i = 0; i < level; ++i)
      std::cout << " ";
    std::cout << "         volume:" << plv.toString();
    std::cout << std::endl;
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
      std::cout << " ";
    std::cout << "++Volume: " << vol->GetName() << " material:" << med->GetName() << " shape:" << shape->GetName() << std::endl;
    TIter next(nodes);
    TGeoNode *geoNode;
    while ((geoNode = (TGeoNode *) next())) {
      dumpNode(geoNode, level + 1);
    }
  }

  void dumpTopVolume(const Detector& description) {
    dumpVolume(description.manager().GetTopVolume(), 0);
  }
}
