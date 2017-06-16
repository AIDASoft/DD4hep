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

#ifndef dd4hep_XML_XMLUTILITIES_H
#define dd4hep_XML_XMLUTILITIES_H

// Framework include files
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {

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

  }  /* End namespace xml              */
}    /* End namespace dd4hep           */
#endif    /* dd4hep_XML_XMLUTILITIES_H */
