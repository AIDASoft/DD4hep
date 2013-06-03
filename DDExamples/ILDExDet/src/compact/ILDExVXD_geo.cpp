// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "VXDData.h"

// -- lcio 
#include <UTIL/BitField64.h>
#include <UTIL/ILDConf.h>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  DetElement  vxd;
  xml_det_t   x_det = e;
  string      name  = x_det.nameStr();

  VXDData* vxd_data = new VXDData();
  vxd.assign(vxd_data,name,x_det.typeStr());
  vxd_data->id = x_det.id();

  Volume      mother = lcdd.pickMotherVolume(vxd);    

  // setup the encoder
  UTIL::BitField64 encoder( ILDCellID0::encoder_string ) ;
  encoder.reset() ;  // reset to 0
  
  encoder[ILDCellID0::subdet] = ILDDetID::VXD ; 
  encoder[ILDCellID0::side] = 0 ;
  encoder[ILDCellID0::sensor] = 0 ;
  

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
      
    sensvol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
    sens.setType("tracker");
    sensvol.setSensitiveDetector(sens);
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

    encoder[ILDCellID0::layer]  = layer_id ; 

    for(int j=0; j<nLadders; ++j) {

      double dj = double(j);      
      double phi = phi0 + dj*dphi  ;

      string laddername = layername + _toString(j,"_ladder%d");

      // Position pos(radius*cos(dj*dphi) - offset*sin(dj*dphi),
      // 		   radius*sin(dj*dphi) - offset*cos(dj*dphi),0.);

      double lthick = sens_thick + supp_thick ;
      
      Position pos( (radius + lthick/2.)*cos(phi)  - offset * sin( phi ) ,
       		    (radius + lthick/2.)*sin(phi)  + offset * cos( phi ) ,
       		    0. ) ;

      Rotation rot( 0, 0 , phi ) ;
      // Rotation rot(0,0,0) ;
      // rot.rotateZ(  phi ) ;
      
      //      if( dj < 3 ) 

      // place the volume and set the cellID0 - will be set to the copyNo in Geant4Converter
      encoder[ILDCellID0::module]  = j  ;
      int cellID0 = encoder.lowWord() ;

      mother.placeVolume(laddervol,pos, rot   ).addPhysVolID("CellID0", cellID0 )  ;
    }
    
    vxd.setVisAttributes(lcdd, x_det.visStr(),laddervol);
  }
  return vxd;
}

DECLARE_DETELEMENT(ILDExVXD,create_element);
