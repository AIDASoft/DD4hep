// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Detector.h"
#include "TPCData.h"
#include "TPCModuleData.h"
#include "TPCModule.h"
#include "VersatileDiskRowLayout.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det = e;
  xml_comp_t  x_tube (x_det.child(_X(tubs)));
  string      name  = x_det.nameStr();
  Material    mat    (lcdd.material(x_det.materialStr()));
  //if data is needed do this  
  Value<TNamed,TPCData>* tpcData = new Value<TNamed,TPCData>();
  DetElement tpc(tpcData, name, x_det.typeStr());
  tpcData->id = x_det.id();
  //else do this
  //DetElement    tpc  (name,x_det.typeStr(),x_det.id());
  Tube        tpc_tub(x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  Volume      tpc_vol(name+"_envelope_volume", tpc_tub, mat);

  for(xml_coll_t c(e,_X(detector)); c; ++c)  {
    xml_comp_t  px_det  (c);
    xml_comp_t  px_tube (px_det.child(_X(tubs)));
    xml_dim_t   px_pos  (px_det.child(_X(position)));
    xml_dim_t   px_rot  (px_det.child(_X(rotation)));
    xml_comp_t  px_mat  (px_det.child(_X(material)));
    string      part_nam(px_det.nameStr());
    Material    part_mat(lcdd.material(px_mat.nameStr()));
    DetElement  part_det(part_nam,px_det.typeStr(),px_det.id());
    Tube        part_tub(px_tube.rmin(),px_tube.rmax(),px_tube.zhalf());
    Volume      part_vol(part_nam,part_tub,part_mat);
    Position    part_pos(px_pos.x(),px_pos.y(),px_pos.z());
    Rotation    part_rot(px_rot.x(),px_rot.y(),px_rot.z());
    bool        reflect   = px_det.reflect();

    part_vol.setVisAttributes(lcdd,px_det.visStr());
    //cache the important volumes in TPCData for later use without having to know their name
    switch(part_det.id())
      {
      case 2:
	tpcData->innerWall=part_det;
      case 3:
	tpcData->outerWall=part_det;
      case 4:
	tpcData->gasVolume=part_det;
      case 5:
	tpcData->cathode=part_det;
      }
    //Endplate
    if(part_det.id()== 0){
      tpcData->endplate=part_det;
      //modules
      int mdcount=0;
      for(xml_coll_t m(px_det,_X(modules)); m; ++m)  {
	xml_comp_t  modules  (m);
	string      m_name  = modules.nameStr();
	for(xml_coll_t mr(modules,_X(module)); mr; ++mr)  {
	  xml_comp_t  mod(mr);
	  int modID=mod.id();
	  //shape of module
	  double rmin=mod.rmin();
	  double rmax=mod.rmax();
	  double DeltaPhi=mod.phi0();
	  double zhalf=px_tube.zhalf();
	  string      m_nam=m_name+_toString(modID,"_M%d");
	  Volume      mr_vol(m_nam,Tube(rmin,rmax,zhalf,DeltaPhi),part_mat);
	  Material    mr_mat(lcdd.material(px_mat.nameStr()));
	  Readout     xml_pads(lcdd.readout(mod.padType()));
	  //placing modules
	  DetElement  module(part_det,m_nam,modID);
	  xml_dim_t   px_modpos  (mod.child(_X(position)));
	  Position mod_pos(px_modpos.x(),px_modpos.y(),px_modpos.z());
	  xml_dim_t   px_modrot  (mod.child(_X(rotation)));
	  Rotation    mod_rot(px_modrot.x(),px_modrot.y(),px_modrot.z()*M_PI/180);
	  PlacedVolume m_phv = part_vol.placeVolume(mr_vol,mod_pos,mod_rot);
	  m_phv.addPhysVolID("module",modID);
	  module.setPlacement(m_phv);
	  //data for VersatileDiskRowLayout is added as user segmentataion	  
	  module.setReadout(xml_pads);
	  // Readout and placement must be present before adding extension,
	  // since they are aquired internally for optimisation reasons. (MF)
	  module.addExtension<PadLayout>(new VersatileDiskRowLayout(module));
	}//modules
      }//module groups
    }//endplate
    
    PlacedVolume part_phv = tpc_vol.placeVolume(part_vol,part_pos,part_rot);
    part_phv.addPhysVolID(part_nam,px_det.id());
    part_det.setPlacement(part_phv);
    tpc.add(part_det);
    //now reflect it
    if(reflect){
      Position r_pos(px_pos.x(),px_pos.y(),-px_pos.z());
      //Attention: rotation is given in euler angles
      Rotation r_rot(M_PI,0,M_PI);
      // Volume      part_vol_r(lcdd,part_nam+"_negativ",part_tub,part_mat);
      PlacedVolume part_phv2 = tpc_vol.placeVolume(part_vol,r_pos,r_rot);
      part_phv2.addPhysVolID(part_nam+"_negativ",px_det.id()+1);
      // needs a copy function for DetElement
      // DetElement rdet(lcdd,part_nam+"_negativ",px_det.typeStr(),px_det.id()+1);
      DetElement rdet = part_det.clone(part_nam+"_negativ",px_det.id()+1); 
      rdet.setPlacement(part_phv2);
      tpcData->endplate2=rdet;
      tpc.add(rdet);
    }
  }//subdetectors
  tpc_vol.setVisAttributes(lcdd, x_det.visStr());
  PlacedVolume phv = lcdd.pickMotherVolume(tpc).placeVolume(tpc_vol);
  tpc.setPlacement(phv);
  return tpc;
}

//first argument is the type from the xml file
DECLARE_DETELEMENT(LP1,create_element)
