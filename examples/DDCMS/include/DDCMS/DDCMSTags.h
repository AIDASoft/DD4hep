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
#ifndef DDCMS_DDCMSTAGS_H
#define DDCMS_DDCMSTAGS_H

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
    UNICODE(Division);

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

    UNICODE(nReplicas);
    UNICODE(RotationSection);
    UNICODE(ReflectionRotation);
    UNICODE(rReflectionRotation);
    UNICODE(Rotation);
    UNICODE(rRotation);
    UNICODE(thetaX);
    UNICODE(phiX);
    UNICODE(thetaY);
    UNICODE(phiY);
    UNICODE(thetaZ);
    UNICODE(phiZ);

    UNICODE(numSide);
    UNICODE(parent);
    UNICODE(width);
    UNICODE(axis);
    UNICODE(offset);

    UNICODE(TransformationSection);
    UNICODE(Transformation);

    UNICODE(SolidSection);

    UNICODE(PseudoTrap);
    UNICODE(dx1);
    UNICODE(dy1);
    UNICODE(dx2);
    UNICODE(dy2);
    UNICODE(atMinusZ);

    UNICODE(Box);
    UNICODE(dx);
    UNICODE(dy);
    UNICODE(dz);

    UNICODE(Cone);
    UNICODE(rMin1);
    UNICODE(rMax1);
    UNICODE(rMin2);
    UNICODE(rMax2);
    
    UNICODE(Tubs);
    UNICODE(rMin);
    UNICODE(rMax);
    UNICODE(startPhi);
    UNICODE(deltaPhi);
    
    UNICODE(Polycone);
    UNICODE(ZSection);

    UNICODE(ZXYSection);
    UNICODE(XYPoint);
    UNICODE(RZPoint);
    UNICODE(scale);

    UNICODE(CutTubs);
    UNICODE(lx);
    UNICODE(ly);
    UNICODE(lz);
    UNICODE(tx);
    UNICODE(ty);
    UNICODE(tz);

    UNICODE(TruncTubs);
    UNICODE(cutAtStart);
    UNICODE(cutAtDelta);
    UNICODE(cutInside);
    UNICODE(zHalf);
    
    UNICODE(Trapezoid);
    UNICODE(alp1);
    UNICODE(h1);
    UNICODE(bl1);
    UNICODE(tl1);
    UNICODE(alp2);
    UNICODE(h2);
    UNICODE(bl2);
    UNICODE(tl2);

    UNICODE(Sphere);
    UNICODE(startTheta);
    UNICODE(deltaTheta);
    
    UNICODE(Ellipsoid);
    UNICODE(xSemiAxis);
    UNICODE(ySemiAxis);
    UNICODE(zSemiAxis);
    UNICODE(zBottomCut);
    UNICODE(zTopCut);
    
    UNICODE(EllipticalTube);
    UNICODE(zHeight);
    
    UNICODE(Torus);
    UNICODE(torusRadius);
    UNICODE(innerRadius);
    UNICODE(outerRadius);
    
    UNICODE(SubtractionSolid);
    UNICODE(firstSolid);
    UNICODE(secondSolid);

    UNICODE(LogicalPartSection);
    UNICODE(LogicalPart);
    UNICODE(rSolid);
    UNICODE(rMaterial);
    
    UNICODE(PosPartSection);
    UNICODE(PosPart);
    UNICODE(copyNumber);
    UNICODE(rParent);
    UNICODE(ChildName);
    UNICODE(rChild);
    UNICODE(Translation);

    UNICODE(Algorithm);
    UNICODE(String);
    UNICODE(Numeric);
    UNICODE(Vector);
    UNICODE(nEntries);

    UNICODE(VisSection);
    UNICODE(vismaterial);
    UNICODE(vis);

    
    /// Debug flags
    UNICODE(debug_constants);
    UNICODE(debug_materials);
    UNICODE(debug_shapes);
    UNICODE(debug_volumes);
    UNICODE(debug_placements);
    UNICODE(debug_namespaces);
    UNICODE(debug_rotations);
    UNICODE(debug_visattr);
    UNICODE(debug_includes);
    UNICODE(debug_algorithms);

    /// DD4hep specific
    UNICODE(open_geometry);
    UNICODE(close_geometry);
    UNICODE(IncludeSection);
    UNICODE(Include);
    UNICODE(DisabledAlgo);
    
  }   /* End namespace DDCMS       */
}     /* End namespace dd4hep     */

#undef UNICODE // Do not miss this one!
#include "XML/XMLTags.h"

#define _CMU(a) ::dd4hep::DDCMS::Unicode_##a

#endif // DDCMS_DDCMSTAGS_H
