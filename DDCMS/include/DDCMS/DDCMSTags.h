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
// DDCMS is a detector description convention developed by the CMS experiment.
//
//==========================================================================

// Framework includes
#ifndef DD4HEP_DDCMS_DDCMSTAGS_H
#define DD4HEP_DDCMS_DDCMSTAGS_H

// Framework include files
#include "XML/XMLElements.h"
#ifndef UNICODE 
#define UNICODE(x)  extern const ::dd4hep::xml::Tag_t Unicode_##x 
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of DDCMS conversion namespace
  namespace DDCMS  {
    UNICODE(DDCMS);

    UNICODE(DDDefinition);

    UNICODE(ConstantsSection);
    UNICODE(Constant);
    
    UNICODE(MaterialSection);
    UNICODE(ElementaryMaterial);
    UNICODE(CompositeMaterial);
    UNICODE(atomicWeight);
    UNICODE(density);
    UNICODE(symbol);
    UNICODE(atomicNumber);
    UNICODE(MaterialFraction);


    UNICODE(RotationSection);
    UNICODE(Rotation);
    UNICODE(rRotation);
    UNICODE(thetaX);
    UNICODE(phiX);
    UNICODE(thetaY);
    UNICODE(phiY);
    UNICODE(thetaZ);
    UNICODE(phiZ);

    UNICODE(TransformationSection);
    UNICODE(Transformation);

    UNICODE(SolidSection);

    UNICODE(Box);
    UNICODE(dx);
    UNICODE(dy);
    UNICODE(dz);

    UNICODE(Tubs);
    UNICODE(rMin);
    UNICODE(rMax);
    UNICODE(startPhi);
    UNICODE(deltaPhi);
    
    UNICODE(Polycone);
    UNICODE(ZSection);
    UNICODE(z);
    
    UNICODE(LogicalPartSection);
    UNICODE(LogicalPart);
    UNICODE(rSolid);
    UNICODE(rMaterial);
    
    UNICODE(PosPartSection);
    UNICODE(PosPart);
    UNICODE(copyNumber);
    UNICODE(rParent);
    UNICODE(rChild);
    UNICODE(Translation);

    UNICODE(Algorithm);
    UNICODE(String);
    UNICODE(Numeric);
    UNICODE(Vector);
    UNICODE(nEntries);

    UNICODE(VisSection);
    UNICODE(vis);
    
    /// Debug flags
    UNICODE(debug_constants);
    UNICODE(debug_materials);
    UNICODE(debug_shapes);
    UNICODE(debug_volumes);
    UNICODE(debug_placements);
    UNICODE(debug_namespaces);
    UNICODE(debug_visattr);

    /// DD4hep specific
    UNICODE(IncludeSection);
    UNICODE(Include);
    
  }   /* End namespace DDCMS       */
}     /* End namespace dd4hep     */

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"

#define _CMU(a) ::dd4hep::DDCMS::Unicode_##a

#endif /* DD4HEP_DDCMS_DDCMSTAGS_H  */
