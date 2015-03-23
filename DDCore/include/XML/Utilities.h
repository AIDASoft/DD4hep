// $Id: Conversions.h 1479 2014-12-18 16:00:45Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_XMLUTILITIES_H
#define DD4hep_XML_XMLUTILITIES_H

#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {

    /// Create a solid shape using the plugin mechanism from the attributes of the XML element
    Geometry::Solid createShape(Geometry::LCDD& lcdd, const std::string& shape_type, XML::Element element);


    /** Create an envelope volume that is placed into the world volume ( the parent volume of sdet) from an xml
     *  element <envelope/> with child nodes <shape/> and optionally <position/> and <rotation/>.
     *  Example: <br>
     *  <p>
     *  <envelope vis="ILD_ECALVis">
     *    <shape type="PolyhedraRegular" numsides="8"  rmin="TPC_outer_radius+Ecal_Tpc_gap" rmax="Ecal_outer_radius"  
     *	         dz="2.*TPC_Ecal_Hcal_barrel_halfZ"  material = "Air" />
     *    <rotation x="0*deg" y="0*deg" z="90*deg-180*deg/8"/>
     * </envelope>
     * 
     *  @author S.Lu DESY, F. Gaede CERN/DESY 
     *  @version $Id:$
     */
    Geometry::Volume createPlacedEnvelope( DD4hep::Geometry::LCDD& lcdd, DD4hep::XML::Handle_t e , 
					   DD4hep::Geometry::DetElement sdet ) ;
    
  }  /* End namespace XML              */
}    /* End namespace DD4hep           */
#endif    /* DD4hep_XML_XMLUTILITIES_H */
