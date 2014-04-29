
// $Id: ILDExTPC_geo.cpp 680 2013-08-06 15:07:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Detector.h"

//fixme: TPCData should not depend on this internal header  
#include "DD4hep/objects/DetectorInterna.h"

#include "DD4hep/TGeoUnits.h"
#include "DDRec/Surface.h"

#include "TPCData.h"

#include "DDRec/DDGear.h"

#include "gearimpl/TPCParametersImpl.h"
#include "gearimpl/FixedPadSizeDiskLayout.h"

using namespace std;
//using namespace tgeo ;
using namespace DD4hep;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec ;
using namespace DDSurfaces ;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det = e;
  xml_comp_t  x_tube (x_det.child(_U(tubs)));
  string      name  = x_det.nameStr();

  Material envmat = lcdd.material("Air" ) ; // x_det.materialStr()));

  //if data is needed do this
  TPCData* tpcData = new TPCData();
  DetElement tpc(tpcData, name, x_det.typeStr());
  tpcData->id = x_det.id();
  //else do this
  //DetElement    tpc  (name,x_det.typeStr(),x_det.id());
  Tube        tpc_tub(x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  Volume      tpc_vol(name+"_envelope_volume", tpc_tub, envmat);

  Readout     readout(sens.readout());

  //--- some parameters needed for gear: 
  double g_driftlength  ;
  double g_rMin, g_rMax, g_padHeight, g_padWidth, g_maxRow, g_padGap, g_phiMax ;
  double g_inner_r, g_outer_r, g_inner_wt, g_outer_wt ;

  xml_comp_t x_global( x_det.child( _Unicode( global ) ) );

  //-------- global gear parameters ----------------

  g_driftlength = x_global.attr<double>("driftLength") ;
  g_padWidth = x_global.attr<double>("padWidth") ;

  //-------- global gear parameters ----------------


  for(xml_coll_t c(e,_U(detector)); c; ++c)  {

    xml_comp_t  px_det  (c);
    xml_comp_t  px_tube (px_det.child(_U(tubs)));
    xml_dim_t   px_pos  (px_det.child(_U(position)));
    xml_dim_t   px_rot  (px_det.child(_U(rotation)));
    xml_comp_t  px_mat  (px_det.child(_U(material)));

    

    string      part_nam(px_det.nameStr());

    Material    part_mat(lcdd.material(px_mat.nameStr()));
    DetElement  part_det(part_nam,px_det.typeStr(),px_det.id());

    Tube        part_tub(px_tube.rmin(),px_tube.rmax(),px_tube.zhalf());
    Volume      part_vol(part_nam,part_tub,part_mat);

    Position    part_pos(px_pos.x(),px_pos.y(),px_pos.z());
    RotationZYX part_rot(px_rot.z(),px_rot.y(),px_rot.x());
    bool        reflect = px_det.reflect();
    
    sens.setType("tracker");

    part_vol.setVisAttributes(lcdd,px_det.visStr());

    // vectors for measurement plane 
    Vector3D u( 0. , 1. , 0. ) ;
    Vector3D v( 0. , 0. , 1. ) ;
    Vector3D n( 1. , 0. , 0. ) ;

    
    double  rcyl = ( px_tube.rmax() + px_tube.rmin() ) * 0.5  ;
    double drcyl =   px_tube.rmax() - px_tube.rmin() ;
    Vector3D ocyl( rcyl , 0. , 0. ) ;
	

    //cache the important volumes in TPCData for later use without having to know their name
    switch(part_det.id())
      {
      case 2: {
        tpcData->innerWall=part_det;
	// add a surface to the det element
	VolCylinder surf( part_vol , SurfaceType( SurfaceType::Helper ) , drcyl*.5 , drcyl*.5 , u,v,n , ocyl ) ;
	volSurfaceList( part_det )->push_back( surf ) ;
      }
	break;

      case 3: {
        tpcData->outerWall=part_det;
	// add a surface to the det element
	VolCylinder surf( part_vol , SurfaceType( SurfaceType::Helper ) , drcyl*.5 , drcyl*.5 , u,v,n , ocyl ) ;
	volSurfaceList( part_det )->push_back( surf ) ;
      }
	break;

      case 4:
	{
	  tpcData->gasVolume=part_det;

	  xml_comp_t  px_lay(px_det.child(_U(layer)));
	  int  nTPClayer( px_lay.attr<int>(_U(number)) );

	  double r0 = px_tube.rmin() +1.e-3 ;
	  double r1 = px_tube.rmax() -1e-3  ;
	  double zh = px_tube.zhalf() - 1e-3 ;
	  double dR = ( r1 - r0 ) / ( 2.* nTPClayer ) ;  


	  //---------------------------- gear stuff --------------
	  g_driftlength = zh / tgeo::mm ;
	  g_rMin = r0 /  tgeo::mm ;
	  g_rMax = r1 /  tgeo::mm ;
	  g_padHeight =  2. * dR / tgeo::mm  ;
	  g_maxRow = nTPClayer ;
	  g_padGap = 0. ;
	  g_phiMax = 6.283185307e+00 ; // FIXME: where to define ? is it allways 2PI ?
	  //------------------------------------------------------

	  
	  for(int i=0 ; i < nTPClayer ; ++i){

	    Tube    gas_tubL( r0 + (2*i) * dR , r0 + (2*i+1) * dR , zh );
	    Volume  gas_volL(  _toString( i, "tpc_row_lower_%03d") , gas_tubL, part_mat);
	    part_vol.placeVolume( gas_volL, RotationZYX(0,0,0) );

	    Tube    gas_tubU( r0 + (2*i+1) * dR , r0 + (2*i+2) * dR , zh );
	    Volume  gas_volU( _toString( i, "tpc_row_upper_%03d")  , gas_tubU, part_mat);

	    gas_volU.setSensitiveDetector( sens );

	    PlacedVolume pv = part_vol.placeVolume( gas_volU, RotationZYX(0,0,0) ) ;
	    pv.addPhysVolID("layer",i) ;

	    DetElement   layerDE( tpc ,   _toString( i, "tpc_row_upper_%03d") , x_det.id() );
	    layerDE.setPlacement( pv ) ;

	    Vector3D o( r0 + (2*i+1) * dR , 0. , 0. ) ;

	    VolCylinder surf( gas_volU , SurfaceType(SurfaceType::Sensitive, SurfaceType::Invisible ) , dR , dR , u,v,n ,o ) ;

	    volSurfaceList( layerDE )->push_back( surf ) ;
	  }
	  
	 
	  break;
	}
      case 5:
        tpcData->cathode=part_det;
	break;
      }
    //Endplate
    if(part_det.id()== 0){
      tpcData->endplate=part_det;

      // add a plane to the endcap volume 
      // note: u and v are exchanged: normal is along z ...
      // Vector3D u( 0. , 1. , 0. ) ;
      // Vector3D v( 0. , 0. , 1. ) ;
      // Vector3D n( 1. , 0. , 0. ) ;
      
      VolPlane surf( part_vol , SurfaceType( SurfaceType::Helper ) , px_tube.zhalf() , x_tube.zhalf(), u , n , v ) ;
      volSurfaceList( part_det )->push_back( surf ) ;

      //modules
      int mdcount=0;
      for(xml_coll_t m(px_det,_U(modules)); m; ++m)  {
        xml_comp_t  modules  (m);
        string      m_name  = modules.nameStr();
        for(xml_coll_t r(modules,_U(row)); r; ++r)  {
          xml_comp_t  row(r);
          int nmodules = row.nModules();
          int rowID=row.RowID();
          //shape of module
          double pitch=row.rowPitch();
          double rmin=px_tube.rmin()+pitch/2+rowID*row.moduleHeight()+rowID*pitch/2;
          double rmax=rmin+row.moduleHeight();
          double DeltaPhi=(2*M_PI-nmodules*(row.modulePitch()/(rmin+(rmax-rmin)/2)))/nmodules;
          double zhalf=px_tube.zhalf();
          string      mr_nam=m_name+_toString(rowID,"_Row%d");
          Volume      mr_vol(mr_nam,Tube(rmin,rmax,zhalf,DeltaPhi),part_mat);
          Material    mr_mat(lcdd.material(px_mat.nameStr()));
          Readout     xml_pads(lcdd.readout(row.padType()));
          
          //placing modules
          for(int md=0;md<nmodules;md++){
            string      m_nam=m_name+_toString(rowID,"_Row%d")+_toString(md,"_M%d");
            
            DetElement  module(part_det,m_nam,mdcount);
            mdcount++;
            double rotz=md*2*M_PI/nmodules+row.modulePitch()/(rmin+(rmax-rmin))/2;
            PlacedVolume m_phv = part_vol.placeVolume(mr_vol,RotationZYX(rotz,0,0));
            m_phv.addPhysVolID("module",md);
            module.setPlacement(m_phv);
            // Readout and placement must be present before adding extension,
            // since they are aquired internally for optimisation reasons. (MF)
	    // module.addExtension<PadLayout>(new FixedPadAngleDiskLayout(module,readout));

          }//modules
        }//rows
      }//module groups
    }//endplate
    
    PlacedVolume part_phv = tpc_vol.placeVolume(part_vol,Transform3D(Rotation3D(part_rot),part_pos));
    //part_phv.addPhysVolID(part_nam,px_det.id());
    part_phv.addPhysVolID("side",0);
    part_det.setPlacement(part_phv);
    tpc.add(part_det);

    //now reflect it
    if(reflect){
      Position r_pos(px_pos.x(),px_pos.y(),-px_pos.z());
      //Attention: rotation is given in euler angles
      RotationZYX r_rot(0,M_PI,M_PI);
      // Volume      part_vol_r(lcdd,part_nam+"_negativ",part_tub,part_mat);
      PlacedVolume part_phv2 = tpc_vol.placeVolume(part_vol,Transform3D(Rotation3D(r_rot),r_pos));
      //part_phv2.addPhysVolID(part_nam+"_negativ",px_det.id()+1);
      part_phv2.addPhysVolID("side",1);
      // needs a copy function for DetElement
      // DetElement rdet(lcdd,part_nam+"_negativ",px_det.typeStr(),px_det.id()+1);
      DetElement rdet = part_det.clone(part_nam+"_negativ",px_det.id()+1);
      rdet.setPlacement(part_phv2);
      tpcData->endplate2 = rdet;
      tpc.add(rdet);
    }
  }//subdetectors
  tpc_vol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
  
  //--------------- create gear::TPCParameters and add them as Extension
  //  GearTPCParameters* gearTPC = new GearTPCParameters( g_driftlength , gear::PadRowLayout2D::POLAR ) ;
  gear::TPCParametersImpl* gearTPC = new gear::TPCParametersImpl( g_driftlength , gear::PadRowLayout2D::POLAR ) ;
  
  gearTPC->setPadLayout( new gear::FixedPadSizeDiskLayout( g_rMin, g_rMax, g_padHeight, g_padWidth, g_maxRow, g_padGap, g_phiMax ) ) ;

  Tube t =  DetElement(tpcData->innerWall).volume().solid() ;
  g_inner_r  = t->GetRmin() / tgeo::mm   ;
  g_inner_wt = ( t->GetRmax() - t->GetRmin() ) / tgeo::mm ;

  t =  DetElement(tpcData->outerWall).volume().solid() ;
  g_outer_r  = t->GetRmax()  / tgeo::mm   ;
  g_outer_wt = ( t->GetRmax() - t->GetRmin() )   / tgeo::mm  ;
  
  gearTPC->setDoubleVal("tpcInnerRadius", g_inner_r )  ; // inner r of support tube
  gearTPC->setDoubleVal("tpcOuterRadius", g_outer_r )  ; // outer radius of TPC
  gearTPC->setDoubleVal("tpcInnerWallThickness", g_inner_wt)  ;   // thickness of inner shell
  gearTPC->setDoubleVal("tpcOuterWallThickness", g_outer_wt)  ;   // thickness of outer shell
  
  //  tpc.addExtension<GearTPCParameters>( gearTPC ) ;

  //  tpc.addExtension<GearHandle<gear::TPCParametersImpl> >( new GearHandle<gear::TPCParametersImpl>( gearTPC ) ) ;

  tpc.addExtension< GearHandle >( new GearHandle( gearTPC, "TPCParameters" ) ) ;
  
  //--------------------------------------------------------------------
  
  PlacedVolume phv = lcdd.pickMotherVolume(tpc).placeVolume(tpc_vol);
  phv.addPhysVolID("system",x_det.id());
  tpc.setPlacement(phv);
  return tpc;
}

//first argument is the type from the xml file
DECLARE_DETELEMENT(ILDExTPC,create_element)
