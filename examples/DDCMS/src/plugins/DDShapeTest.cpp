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
// Specialized generic detector constructor
// 
//==========================================================================
#include <DD4hep/World.h>
#include <DD4hep/Detector.h>
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/GeoHandler.h>
#include <DD4hep/Printout.h>
#include <DDCMS/DDShapes.h>

#include "TClass.h"

#include <sstream>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cms  {

    /// Geometry to screen dump action
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    class ShapeDump: public detail::GeoHandler {
    public:
      ShapeDump() = default;
      /// Standard destructor
      virtual ~ShapeDump() = default;
      /// Dump solid in GDML format to output stream
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;
    };
  }    // End namespace detail
}      // End namespace dd4hep

/// Dump solid in GDML format to output stream
void* dd4hep::cms::ShapeDump::handleSolid(const std::string& name, const TGeoShape* shape) const {
  if (shape) {
    Solid solid(shape);
    Box   box = solid; // Every shape implements a TGeoBBox
    if (shape->IsA() == TGeoCompositeShape::Class()) {
    }
    else  {
      const TClass* isa = shape->IsA();
      cms::DDBox ddbox = solid;

      if (isa == TGeoBBox::Class()) {
        printout(INFO, "ShapeDump","Got shape '%s' of type:%-16s -> DDBox:%p  dx:%7.2f [cm] dy:%7.2f [cm] dz:%7.2f [cm]",
                 name.c_str(), isa->GetName(), ddbox.ptr(), ddbox.halfX(), ddbox.halfY(), ddbox.halfZ());
      }
      else if (isa == TGeoTube::Class()) {
      }
      else if (isa == TGeoTubeSeg::Class()) {
      }
      else if (isa == TGeoTrd1::Class()) {
      }
      else if (isa == TGeoTrd2::Class()) {
      }
      else if (isa == TGeoPgon::Class()) {
        printout(INFO, "ShapeDump","Got shape '%s' of type:%-16s -> DDBox:%p  dx:%7.2f [cm] dy:%7.2f [cm] dz:%7.2f [cm]",
                 name.c_str(), isa->GetName(), ddbox.ptr(), ddbox.halfX(), ddbox.halfY(), ddbox.halfZ());
      }
      else if (isa == TGeoPcon::Class()) {
        cms::DDPolycone pcon = box;
        printout(INFO, "ShapeDump","Got shape '%s' of type:%-16s", name.c_str(), isa->GetName());
        printout(INFO, "ShapeDump","          DDBox:     %p dx:%7.2f [cm] dy:%7.2f [cm] dz:%7.2f [cm]",
                 ddbox.ptr(), ddbox.halfX(), ddbox.halfY(), ddbox.halfZ());
        printout(INFO, "ShapeDump","          DDPolycone:%p phi-start: %7.2f [rad] delta:%7.2f [rad] zplanes: N_z:%ld N_rmin:%ld N_rmax:%ld",
                 pcon.ptr(), pcon.startPhi(), pcon.deltaPhi(), pcon.zVec().size(), pcon.rMinVec().size(), pcon.rMaxVec().size());
      }
    }
  }
  return 0;
}

static long exec_ShapeDump(dd4hep::Detector& description, int, char**) {
  dd4hep::cms::ShapeDump dmp;
  dd4hep::detail::GeoHandlerTypes::GeometryInfo geo;
  dmp.collect(description.world(), geo);
  for (const auto s : geo.solids )
    dmp.handleSolid(s->GetName(), s);
  return 1;
}
DECLARE_APPLY(DDCMS_DDShapeDump,exec_ShapeDump)
