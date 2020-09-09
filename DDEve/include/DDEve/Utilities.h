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
#ifndef DDEVE_UTILITIES_H
#define DDEVE_UTILITIES_H

// Framework include files
#include "DD4hep/Detector.h"

// ROOT include files
#include "TGeoMatrix.h"
#include "TEveElement.h"
#include "TNamed.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  namespace Utilities {
    int findNodeWithMatrix(TGeoNode* p, TGeoNode* n, TGeoHMatrix* mat, std::string* sub_path=0);

    std::pair<bool,TEveElement*> 
    createEveShape(int level, int max_level, TEveElement* p, TGeoNode* n, const
                   TGeoHMatrix& mat, const std::string& node_name);

    std::pair<bool,TEveElement*> 
    LoadDetElement(DetElement element,int levels, TEveElement* parent);

    /// Make a set of nodes starting from a top element (in-)visible with a given depth
    void MakeNodesVisible(TEveElement* e, bool visible, int level);

    /// Set the rendering flags for the object and the next level children
    void SetRnrChildren(TEveElementList* l, bool b);
    /// Recursively set the rendering flags for the object ans its children
    void SetRnrAll(TEveElementList* l, bool b);

    template <typename T> inline const char* GetName(T* p)   {
      TNamed *n = dynamic_cast<TNamed*>(p);
      return n ? n->GetName() : "???";
    }
  }
}      /* End namespace dd4hep     */
#endif // DDEVE_UTILITIES_H

