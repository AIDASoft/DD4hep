// $Id: $
//====================================================================
//  AIDA Detector description implementation for LCD
//-------------------------------------------------------------------- 
// geant4 example/novice/N04 ported to DD4hep
//
// @author F.Gaede, DESY
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {


  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   tracker( name, x_det.id() );
  
  Volume experimentalHall_log =  lcdd.pickMotherVolume( tracker ) ;
  
  xml_comp_t  trkTubs ( x_det.child( _U(tubs) ) );
  
  // G4VSolid * tracker_tubs
  //   = new G4Tubs("trkTubs_tubs",trkTubs_rmin,trkTubs_rmax,trkTubs_dz,
  //                trkTubs_sphi,trkTubs_dphi);
  
  Tube tracker_tubs( trkTubs.rmin(),trkTubs.rmax(),trkTubs.dz(), trkTubs.phi0(), trkTubs.deltaphi() ); 
  // fixme: Tgeo documentation has phi1, phi2 !!!???
  
  // G4LogicalVolume * tracker_log
  //   = new G4LogicalVolume(tracker_tubs,Ar,"trackerT_L",0,0,0);
  
  Volume tracker_log( "trackerT_L", tracker_tubs, lcdd.material("ArgonGas") );
  
  

  // G4VPhysicalVolume * tracker_phys =
  //     new G4PVPlacement(0,G4ThreeVector(),tracker_log,"tracker_phys",
  // 			experimentalHall_log,false,0);

  PlacedVolume tracker_phys = experimentalHall_log.placeVolume( tracker_log );
  
  tracker_phys.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;


  tracker.setPlacement( tracker_phys );

  // G4VisAttributes* tracker_logVisAtt
  //   = new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  // tracker_log->SetVisAttributes(tracker_logVisAtt);

  tracker.setVisAttributes( lcdd, x_det.visStr(), tracker_log );


  // DD4hep does not handle prarmetrized volumes - so we
  // have to specify every layer by hand in the xml file
  //
  int layerID = 0 ;
  for(xml_coll_t c(e,_U(layer)); c; ++c, ++layerID)  {
    
    xml_comp_t  x_layer(c);
    
    Tube layer_tubs( x_layer.rmin(),x_layer.rmax(),x_layer.dz(), x_layer.phi0(), x_layer.deltaphi() );   // fixme: Tgeo TubeSegment has phi1, phi2 !!!
    
    Volume layer_log( "layer_L", layer_tubs, lcdd.material("Silicon") );
    
    layer_log.setSensitiveDetector( sens ) ;


    PlacedVolume layer_phys = tracker_log.placeVolume( layer_log );

    layer_phys.addPhysVolID( "layer",  layerID ) ;

    tracker.setVisAttributes( lcdd, x_layer.visStr(), layer_log );
  }
  
  return tracker;
}

//first argument is the type from the xml file
DECLARE_DETELEMENT( Tracker ,create_element )
