// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"

// -- lcio 
#include <UTIL/BitField64.h>
#include <UTIL/ILDConf.h>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
  
static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   sit(name,x_det.id());
  Assembly assembly( name + "assembly"  ) ;
  PlacedVolume pv;

  // setup the encoder
  UTIL::BitField64 encoder( ILDCellID0::encoder_string ) ;
  encoder.reset() ;  // reset to 0
  
  encoder[ILDCellID0::subdet] = ILDDetID::SIT ; 
  encoder[ILDCellID0::side] = 0 ;
  encoder[ILDCellID0::module] = 0 ;
  encoder[ILDCellID0::sensor] = 0 ;
  
  for(xml_coll_t c(e,_U(layer)); c; ++c)  {


    xml_comp_t  x_layer   (c);
    xml_comp_t  x_support (x_layer.child(_U(support)));
    xml_comp_t  x_ladder  (x_layer.child(_U(ladder)));
    int         layer_id   = x_layer.id();
    int         nLadders   = x_ladder.number();
    string      layername  = name+_toString(layer_id,"_layer%d");
    double      dphi       = 2.*M_PI/double(nLadders);
    double      zhalf      = x_ladder.zhalf();
    double      sens_radius= x_ladder.radius();
    double      sens_thick = x_ladder.thickness();
    double      supp_thick = x_support.thickness();
    double      width      = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.);
    Box         ladderbox ((sens_thick+supp_thick)/2.,width/2.,zhalf);
    Volume      laddervol (layername+"_ladder",ladderbox,lcdd.material(x_ladder.materialStr()));
    Box         sensbox   (sens_thick/2.,width/2.,zhalf);
    Volume      sensvol   (layername+"_sens",sensbox,lcdd.material(x_ladder.materialStr()));
    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_supp",suppbox,lcdd.material(x_support.materialStr()));
    Position    senspos   (-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0);
    Position    supppos   (-(sens_thick+supp_thick)/2.+sens_thick+supp_thick/2.,0,0);
      
    sensvol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
    sens.setType("tracker");
    sensvol.setSensitiveDetector(sens);

    pv = laddervol.placeVolume(sensvol,senspos) ;
    laddervol.placeVolume(suppvol,supppos);
    sit.setVisAttributes(lcdd, x_det.visStr(),laddervol);
    //    pv.laddPhysVolID( "layer", layer_id ) ;


    encoder[ILDCellID0::layer]  = layer_id ;


    //Assembly layer_assembly = assembly;//( name + _toString( layer_id,"layer_assembly_%d" ) ) ;
    //PlacedVolume layer_physvol = assembly.placeVolume( layer_assembly,IdentityPos() ) ;
    //layer_physvol.addPhysVolID("layer", layer_id );


    for(int j=0; j<nLadders; ++j) {
      
      string laddername = layername + _toString(j,"_ladder%d");
      double radius = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.);
      Position pos(radius*cos(j*dphi),radius*sin(j*dphi),0.);
 
      // place the volume and set the cellID0 - will be set to the copyNo in Geant4Converter
      encoder[ILDCellID0::module]  = j  ;
      int cellID0 = encoder.lowWord() ;

      pv = assembly.placeVolume(laddervol,pos,Rotation(0,0,j*dphi));
      pv.addPhysVolID("layer",layer_id).addPhysVolID("module",j);
   }
  }

  pv = lcdd.pickMotherVolume(sit).placeVolume(assembly)  ;
  pv.addPhysVolID("system", x_det.id());

  sit.setPlacement( pv );

  return sit;
}

DECLARE_DETELEMENT(ILDExSIT,create_element);
