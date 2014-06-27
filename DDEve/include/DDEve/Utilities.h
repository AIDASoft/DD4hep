// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_UTILITIES_H
#define DD4HEP_DDEVE_UTILITIES_H

// Framework include files
#include "DD4hep/LCDD.h"

// ROOT include files
#include "TGeoMatrix.h"
#include "TEveElement.h"
#include "TNamed.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  namespace Utilities {
    bool findNodeWithMatrix(TGeoNode* p, TGeoNode* n, TGeoHMatrix* mat, std::string* sub_path=0);

    std::pair<bool,TEveElement*> 
      createEveShape(int level, int max_level, TEveElement* p, TGeoNode* n, TGeoHMatrix mat);

    std::pair<bool,TEveElement*> 
      LoadDetElement(Geometry::DetElement element,int levels, TEveElement* parent);

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


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_UTILITIES_H */

