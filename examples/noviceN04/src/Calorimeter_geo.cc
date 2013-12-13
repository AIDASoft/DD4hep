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
  DetElement   calorimeter( name, x_det.id() );
  
  Volume experimentalHall_log =  lcdd.pickMotherVolume( calorimeter ) ;
  
  xml_comp_t  calTubs ( x_det.child( _U(tubs) ) );
  
  Tube calorimeter_tubs( calTubs.rmin(),calTubs.rmax(),calTubs.dz(), calTubs.phi0(), calTubs.deltaphi() );   
  // fixme: Tgeo documentation has phi1, phi2 !!!???
  
  Volume calorimeter_log( "calorimeterT_L", calorimeter_tubs, lcdd.air() ) ;
  
  PlacedVolume calorimeter_phys = experimentalHall_log.placeVolume( calorimeter_log );
  
  calorimeter_phys.addPhysVolID( "system", x_det.id() )  ;

  calorimeter.setPlacement( calorimeter_phys );

  calorimeter.setVisAttributes( lcdd, x_det.visStr(), calorimeter_log );

  double calo_layer_ri =  calTubs.rmin() ;
  double calo_layer_ro =  0. ;

  // DD4hep does not handle parameterized volumes - so we
  // specify the layers in the xml file (using "repeat")

  int l=0 ;
  for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
    
    xml_comp_t x_layer = li;
    int repeat = x_layer.repeat();

    // Loop over number of repeats for this layer.
    for (int j=0; j<repeat; j++)    {

      // slices per layer
      for(xml_coll_t si(x_layer,_U(slice)); si; ++si)  {

	xml_comp_t x_slice = si;

	double s_thick = x_slice.thickness();

	calo_layer_ro =  calo_layer_ri + s_thick ;

	Tube layer_tubs( calo_layer_ri ,calo_layer_ro ,calTubs.dz(), calTubs.phi0(), calTubs.deltaphi() );

	calo_layer_ri = calo_layer_ro  ;
	
	Volume layer_log( "layer_L", layer_tubs, lcdd.material( x_slice.attr<string>( _U(material) ) ));
    
	PlacedVolume layer_phys = calorimeter_log.placeVolume( layer_log );

	if(  x_slice.isSensitive() ) {

	  layer_log.setSensitiveDetector( sens) ;

	  layer_phys.addPhysVolID( "layer", l++ )  ;
	}

	calorimeter.setVisAttributes( lcdd, x_layer.visStr(), layer_log );
      } // slice
    }   // repeat
  }     // layer

  
  return calorimeter;
}

//first argument is the type from the xml file
DECLARE_DETELEMENT( Calorimeter ,create_element )
