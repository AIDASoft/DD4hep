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

#ifndef DD4HEP_XML_XMLUTILITIES_H
#define DD4HEP_XML_XMLUTILITIES_H

// Framework include files
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/VolumeBuilder.h"
#include "DD4hep/Detector.h"

// C/C++ include files
#include <set>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {

    /// Create layered transformation from xml information
    /** The xml content of the information supposedly looks like this:
     *      <transformation>
     *          <position/>
     *          <rotation x="-90*degree"/>
     *          <position/>
     *          <rotation y="-90*degree"/>
     *          <position x="CutX"
     *                    y=" 0.5*YokeHorHeight"
     *                    z="-0.5*(YokeHorD1+YokeHorA1)"/>
     *          <rotation x="-HoleAngle"/>
     *          <position/>
     *          <rotation z="CutTrapTheta"/>
     *      </transformation>
     *
     *   Every pair position, rotation will be combined to a Transform3D 
     *   and combined to the resulting transformation.
     */
    Transform3D createTransformation(xml::Element element);

    /// Create a simple volume using the shape plugin mechanism from the attributes of the XML element
    Volume createStdVolume(Detector& description, xml::Element element);

    /// Create a volume using the plugin mechanism from the attributes of the XML element
    Volume createVolume(Detector& description, const std::string& type, xml::Element element);


    /// Create a solid shape using the plugin mechanism from the attributes of the XML element
    Solid createShape(Detector& description, const std::string& shape_type, xml::Element element);


    /** Create an envelope volume that is placed into the world volume (the parent volume of sdet) from an xml
     *  element \<envelope/\> with child nodes \<shape/\> and optionally \<position/\> and \<rotation/\>. For special cases
     *  no volume but an assembly can be created with \<shape type="Assembly"/\>.
     *  Example: <br>
     @verbatim
     <envelope vis="ILD_ECALVis">
     <shape type="PolyhedraRegular" numsides="8"  rmin="TPC_outer_radius+Ecal_Tpc_gap" rmax="Ecal_outer_radius"
     dz="2.*TPC_Ecal_Hcal_barrel_halfZ"  material = "Air" />
     <rotation x="0*deg" y="0*deg" z="90*deg-180*deg/8"/>
     </envelope>
     @endverbatim
     *  @author S.Lu DESY, F. Gaede CERN/DESY 
     */
    Volume createPlacedEnvelope( dd4hep::Detector& description, dd4hep::xml::Handle_t e , 
                                 dd4hep::DetElement sdet ) ;
 
    /** Sets the type flag specified in the \<type_flags\> element for the given DetElement, example: <br>
	@verbatim
	<type_flags type=" DetType_TRACKER + DetType_PIXEL + DetType_VERTEX "/>
	@endverbatim
     *  @author F.Gaede, DESY
     */
    void setDetectorTypeFlag( dd4hep::xml::Handle_t e, dd4hep::DetElement sdet ) ; 
  }       /* End namespace xml              */
}         /* End namespace dd4hep           */
#endif    /* dd4hep_XML_XMLUTILITIES_H */
