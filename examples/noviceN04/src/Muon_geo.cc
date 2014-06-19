// $Id: $
//====================================================================
//  AIDA Detector description implementation for LCD
//-------------------------------------------------------------------- 
// geant4 example/novice/N04 ported to DD4hep
//
// @author F.Gaede, DESY
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

#include "DD4hep/DD4hepUnits.h"

using namespace std;
using namespace dd4hep ;
using namespace DD4hep;
using namespace DD4hep::Geometry;


static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {


  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   muon( name, x_det.id() );
  
  Volume experimentalHall_log =  lcdd.pickMotherVolume( muon ) ;
  
  xml_comp_t  muonBox ( x_det.child( _U(box) ) );
  
  Box muon_box( muonBox.width(), muonBox.thickness(), muonBox.length() ) ;
  
  
  Volume muon_log( "muonT_L", muon_box, lcdd.material("Scintillator" ) );
  
  muon_log.setSensitiveDetector( sens ) ;

  double deltaphi = muonBox.deltaphi()  ;

  for(int i=0,N = muonBox.repeat() ; i < N ; ++i ){
    
    double radius = x_det.radius() ;
    
    double phi = deltaphi/rad * i ;

    Position trans( radius * sin( phi ) ,
		    radius * cos( phi ) ,
		    0. ) ;

    PlacedVolume muon_phys = experimentalHall_log.placeVolume( muon_log , Transform3D( RotationZ(phi) , trans ) );

    muon_phys.addPhysVolID( "system", x_det.id() )  ;

    muon.setPlacement( muon_phys );

  }
  muon.setVisAttributes( lcdd, x_det.visStr(), muon_log );

  
  return muon;
}

//first argument is the type from the xml file
DECLARE_DETELEMENT( Muon ,create_element )
