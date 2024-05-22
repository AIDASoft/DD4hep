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

#ifndef XML_UTILITIES_H
#define XML_UTILITIES_H

// Framework include files
#include <XML/Conversions.h>
#include <XML/XMLElements.h>
#include <XML/VolumeBuilder.h>
#include <DD4hep/Detector.h>

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

    /// Configure volume properties from XML element
    /**
     *   Example:
     *      <volume>
     *         <regionref   name="world_region"/>
     *         <limitsetref name="world_limits"/>
     *         <visref      name="world_vis"/>
     *      </volume>
     *    
     *      with
     *      dd4hep::Detector&          detector:  Handle to dd4hep Detector instance
     *      dd4hep::xml::Handle_t      element:   XML element <volume>
     *      dd4hep::Volume             volume:    the volume to be configured.
     *      bool                       propagate: apply setting also to all children
     *      bool                       ignore_unknown_attr: Ignore unknown attrs
     *
     *   \return Number of properties changed
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    03.03.23
     */
    std::size_t configVolume( dd4hep::Detector& detector,
			      dd4hep::xml::Handle_t element,
			      dd4hep::Volume volume,
			      bool propagate,
			      bool ignore_unknown_attr = false);

    /// Configure sensitive detector from XML element
    /**
     *   Example:
     *   <some-xml-tag debug="true">
     *      <combine_hits    value="true"/>
     *      <verbose         value="true"/>
     *      <type            value="tracker"/>
     *      <ecut            value="5*keV"/>
     *      <hits_collection value="hits_collection_5"/>
     *    </some-xml-tag>
     *    
     *    - The debug flag is optional.
     *    - Not all child nodes are mandatory.
     *      If not present, the attribute stays unchanged.
     *
     *   \return Number of properties changed
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    03.03.23
     */
    std::size_t configSensitiveDetector( dd4hep::Detector& detector,
					 dd4hep::SensitiveDetector sensitive,
					 dd4hep::xml::Handle_t element);

  }       /* End namespace xml              */
}         /* End namespace dd4hep           */
#endif // XML_UTILITIES_H
