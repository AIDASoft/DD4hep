// $Id: ILDExVXD_geo.cpp 673 2013-08-05 10:01:33Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "VXDData.h"

//#include "GearWrapper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  DetElement   vxd;
  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();

  //  Assembly     assembly(name+"_assembly");

  // replace assembly with cylinder of air:
  xml_comp_t  x_tube (x_det.child(_U(tubs)));
  Tube        envelope_cylinder(x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  Volume      assembly ("vxd_envelope_cyl", envelope_cylinder ,lcdd.air());

  PlacedVolume pv;

  VXDData* vxd_data = new VXDData();
  vxd.assign(vxd_data,name,x_det.typeStr());
  vxd_data->id = x_det.id();


  // //--------------- create gear::ZPlanarParameters and add them as Extension
  // GearZPlanarParameters* gearZPlanar = new GearZPlanarParameters ;
  // vxd.addExtension<GearZPlanarParameters>( gearZPlanar ) ;
  // //--------------------------------------------------------------------

  for(xml_coll_t c(e,_U(layer)); c; ++c)  {

    xml_comp_t  x_layer  (c);

    xml_comp_t  x_support (x_layer.child(_U(support)));
    xml_comp_t  x_ladder  (x_layer.child(_U(ladder)));

    int         layer_id   = x_layer.id();
    int         nLadders   = x_ladder.number();
    string      layername  = name+_toString(layer_id,"_layer%d");
    double      dphi       = 2.*M_PI/double(nLadders);
    double      zhalf      = x_ladder.zhalf();
    double      offset     = x_ladder.offset();
    double      sens_radius= x_ladder.radius();
    double      sens_thick = x_ladder.thickness();
    double      supp_thick = x_support.thickness();
    double      radius     = sens_radius + ((sens_thick+supp_thick)/2. - sens_thick/2.);
    double      phi0       =  x_layer.phi0() ;

    //    double      width      = 2.*tan(dphi/2.)*(sens_radius-sens_thick/2.);
    double      width      =  x_ladder.width();

    Box         ladderbox ( (sens_thick+supp_thick)/2., width/2., zhalf );

    Volume      laddervol (layername+"_ladder",ladderbox,lcdd.air());

    Material    sensmat    = lcdd.material(x_ladder.materialStr());
    Box         sensbox   (sens_thick/2.,  width/2.,   zhalf);
    Volume      sensvol   (layername+"_sens",sensbox,sensmat);

    Material    suppmat  = lcdd.material(x_support.materialStr());
    Box         suppbox   (supp_thick/2.,width/2.,zhalf);
    Volume      suppvol   (layername+"_supp",suppbox,suppmat);

    //Position    senspos   (0,0,0);
    //Position    supppos   (0,0,0);
    Position    senspos   (-(sens_thick+supp_thick)/2.+sens_thick/2.,0,0);
    Position    supppos   (-(sens_thick+supp_thick)/2.+sens_thick+supp_thick/2.,0,0);
      
    sens.setType("tracker");
    sensvol.setSensitiveDetector(sens);
    sensvol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_layer.visStr());

    laddervol.placeVolume(sensvol,senspos);
    laddervol.placeVolume(suppvol,supppos);

    VXDData::Layer layer;
    // Add ladder layer
    layer.NLadders     = nLadders ;
    layer.internalPhi0 = phi0 ; //M_PI/2. - dphi;   // phi0 given wrt x-axis  - internalPhi0 is wrt yaxis , negative orientation
    layer.Distance     = 0.0; // What is this really (?) : ladderDistance;
    layer.Offset       = offset;              // offset given in direction of positive rotation (increasing phi)
    layer.Thickness    = supp_thick;
    layer.Length       = suppbox.x();
    layer.Width        = suppbox.y();
    layer.RadLength    = suppmat->GetMaterial()->GetRadLen();
    vxd_data->_lVec.push_back(layer);

    // Now add sensitive ladder layer
    layer.NLadders     = nLadders ;
    layer.internalPhi0 = phi0 ;  //M_PI/2. - dphi;  // phi0 given wrt x-axis  - internalPhi0 is wrt yaxis , negative orientation
    layer.Distance     = 0.0;    // What is this really (?) : sensitiveDistance ;
    layer.Offset       = offset;  // offset given in direction of positive rotation (increasing phi)
    layer.Thickness    = sens_thick;
    layer.Length       = sensbox.x();
    layer.Width        = sensbox.y();
    layer.RadLength    = sensmat->GetMaterial()->GetRadLen();
    vxd_data->_sVec.push_back(layer);


    // Assembly layer_assembly( name + _toString( layer_id,"layer_assembly_%d" ) ) ;
    // PlacedVolume layer_physvol = assembly.placeVolume( layer_assembly,IdentityPos() ) ;
    // layer_physvol.addPhysVolID("layer", layer_id );

    for(int j=0; j<nLadders; ++j) {

      double dj = double(j);      
      double phi = phi0 + dj*dphi  ;

      string laddername = layername + _toString(j,"_ladder%d");

      // Position pos(radius*cos(dj*dphi) - offset*sin(dj*dphi),
      // 		   radius*sin(dj*dphi) - offset*cos(dj*dphi),0.);

      double lthick = sens_thick + supp_thick ;
      
      RotationZYX rot( phi , 0, 0  ) ;


#if 0
      pv = assembly.placeVolume( laddervol,Transform3D( rot, Position( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
									  (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
									  0. ) ));

      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 )   ;

#else

      // put one wafer at plus z and one at minus z
      
      pv = assembly.placeVolume( laddervol, Transform3D( rot ,  Position( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
									  (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
									  zhalf ) ) );

      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 ).addPhysVolID("side", 1 )   ;


      pv = assembly.placeVolume( laddervol, Transform3D( rot ,  Position( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
									  (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
									  -zhalf ) ) );

      pv.addPhysVolID("layer", layer_id ).addPhysVolID( "module" , j ).addPhysVolID("sensor", 0 ).addPhysVolID("side", -1 )   ;  ;

#endif

      //pv = assembly.placeVolume( sensvol, pos, rot ) ;

    }
    vxd.setVisAttributes(lcdd, x_det.visStr(),laddervol);
  }
  Volume mother =  lcdd.pickMotherVolume(vxd) ;

  pv = mother.placeVolume(assembly);

  pv.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;
  
  vxd.setPlacement(pv);
  return vxd;
}

DECLARE_DETELEMENT(ILDExVXD,create_element);
