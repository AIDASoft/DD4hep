// $Id: ILDExVXD_geo.cpp 673 2013-08-05 10:01:33Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  Assembly     assembly(name+"_assembly");
  DetElement   vxd(name, x_det.typeStr(), x_det.id());
  PlacedVolume pv;

  assembly.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
  for(xml_coll_t c(e,_U(layer)); c; ++c)  {
    xml_comp_t  x_layer  (c);
    xml_comp_t  x_support (x_layer.child(_U(support)));
    xml_comp_t  x_ladder  (x_layer.child(_U(ladder)));
    int         layer_id   = x_layer.id();
    int         nLadders   = x_ladder.number();
    string      layername  = name+_toString(layer_id,"_layer%d");
    double      dphi       = 2.*M_PI/double(nLadders);
    // --- create an assembly and DetElement for the layer 
    Assembly     layer_assembly(layername);
    //Box          layer_box(1,1,1);
    //Volume       layer_assembly(layername,layer_box,lcdd.air());

    DetElement   layerDE( vxd , _toString(layer_id,"layer_%d"), x_det.id() );
    double      zhalf      = x_ladder.zhalf();
    double      offset     = x_ladder.offset();
    double      sens_radius= x_ladder.radius();
    double      sens_thick = x_ladder.thickness();
    double      supp_thick = x_support.thickness();
    double      radius     = sens_radius  - supp_thick ;
    double      phi0       =  x_layer.phi0() ;
    double      width      =  x_ladder.width();

    //Box         ladderbox ((sens_thick+supp_thick)/2., width/2., zhalf );
    //Volume    laddervol(layername+"_ladder",ladderbox,lcdd.air());
    Assembly    laddervol (layername+"_ladder");

    Box         sensbox   (sens_thick/2.,  width/2.,   zhalf);
    Volume      sensvol   (layername+"_ladder_sens",sensbox,lcdd.material(x_ladder.materialStr()));

    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_ladder_supp",suppbox,lcdd.material(x_support.materialStr()));

    // --- position the sensitive on top of the support !
    Position    senspos   ( (sens_thick+supp_thick)/2. - sens_thick/2., 0, 0 );
    Position    supppos   ( (sens_thick+supp_thick)/2. - sens_thick - supp_thick/2., 0, 0 );
      
    sens.setType("tracker");

    layer_assembly.setAttributes(lcdd,x_layer.regionStr(),x_layer.limitsStr(),"invisible");
    pv = assembly.placeVolume(layer_assembly).addPhysVolID("layer",layer_id);  
    layerDE.setPlacement( pv ) ;

    laddervol.setAttributes(lcdd,x_ladder.regionStr(),x_ladder.limitsStr(),"invisible");
    suppvol.setAttributes(lcdd,x_support.regionStr(),x_support.limitsStr(),x_support.visStr());

    sensvol.setSensitiveDetector(sens);
    sensvol.setAttributes(lcdd,x_ladder.regionStr(),x_ladder.limitsStr(),x_layer.visStr());

    laddervol.placeVolume(sensvol,senspos);
    laddervol.placeVolume(suppvol,supppos);

    for(int j=0; j<nLadders; ++j) {
      double dj = double(j);
      double phi = phi0 + dj*dphi;
      string laddername = layername + _toString(j,"_ladder%d");
      double lthick = sens_thick + supp_thick;
      RotationZYX rot( phi,0,0);
      double pos_x = (radius + lthick/2.)*cos(phi)  - offset * sin( phi );
      double pos_y = (radius + lthick/2.)*sin(phi)  + offset * cos( phi );

      pv = layer_assembly.placeVolume( laddervol,Transform3D(rot, Position(pos_x,pos_y,0.0)));
      pv.addPhysVolID("module",j).addPhysVolID("sensor",0).addPhysVolID("side",1);
      DetElement ladderDE(layerDE, laddername,x_det.id());
      ladderDE.setPlacement(pv);
      DetElement ladderDEposZ(layerDE, laddername+"_posZ", x_det.id() );
      ladderDEposZ.setPlacement( pv ) ;
      pv = layer_assembly.placeVolume(laddervol,Transform3D(rot, Position(pos_x, pos_y, -zhalf)));
      pv.addPhysVolID("module",j).addPhysVolID("sensor",0).addPhysVolID("side",2);

      DetElement ladderDEnegZ(layerDE, laddername+"_negZ" , x_det.id() );
      ladderDEnegZ.setPlacement( pv ) ;
    }
  }
  Volume mother =  lcdd.pickMotherVolume(vxd) ;
  pv = mother.placeVolume(assembly);
  pv.addPhysVolID( "system", x_det.id());
  vxd.setPlacement(pv);
  return vxd;
}

DECLARE_DETELEMENT(VXD_Assembly,create_element)
