// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetFactoryHelper.h"
#include "CompactDetectors.h"
#include "XML/Layering.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace DD4hep { namespace Geometry {
  
  template <> Ref_t DetElementFactory<PolyhedraEndcapCalorimeter2>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t   x_det     = e;
    xml_dim_t   dim       = x_det.dimensions();
    int         det_id    = x_det.id();
    string      det_name  = x_det.nameStr();
    string      det_type  = x_det.typeStr();
    bool        reflect   = x_det.reflect(true);
    Material    air       = lcdd.air();
    int         numsides  = dim.numsides();
    double      rmin      = dim.rmin();
    double      rmax      = dim.rmax()*std::cos(M_PI/numsides);
    double      zmin      = dim.zmin();
    Layering    layering(x_det);
    double      totalThickness = layering.totalThickness();

    PolyhedraRegular polyhedra(lcdd,det_name+"_polyhedra",numsides,rmin,rmax,totalThickness);
    Volume           envelopeVol(lcdd,det_name+"_envelope",polyhedra,air);

    int l_num = 0;
    int layerType   = 0;
    double layerZ   = -totalThickness/2;

    for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
      xml_comp_t       x_layer  = c;
      double           l_thick  = layering.layer(l_num)->thickness();
      string           l_name   = det_name + _toString(layerType,"_layer%d");
      int              l_repeat = x_layer.repeat();
      PolyhedraRegular l_solid(lcdd,l_name+"_solid",numsides,rmin,rmax,l_thick);
      Volume           l_vol  (lcdd,l_name+"_volume", l_solid, air);
      
      int s_num = 0;
      double sliceZ = -l_thick/2;
      for(xml_coll_t s(x_layer,_X(slice)); s; ++s)  {
	xml_comp_t x_slice = s;
	string     s_name  = l_name + _toString(s_num,"_slice%d");
	double     s_thick = x_slice.thickness();
	Material   s_mat   = lcdd.material(x_slice.materialStr());
	PolyhedraRegular s_solid(lcdd,s_name+"_solid",numsides,rmin,rmax,s_thick);
	Volume           s_vol  (lcdd,s_name+"_volume",s_solid,s_mat);
	
	if ( x_slice.isSensitive() ) s_vol.setSensitiveDetector(sens);
	s_vol.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));
	sliceZ += s_thick/2;
	PlacedVolume s_phv = l_vol.placeVolume(s_vol,Position(0,0,sliceZ));
	s_phv.addPhysVolID("slice",s_num);
	s_num++;
      }
      l_vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
      if ( l_repeat <= 0 ) throw std::runtime_error(det_name+"> Invalid repeat value");
      for(int j=0; j<l_repeat; ++j) {
	string phys_lay = det_name + _toString(l_num,"_layer%d");
	layerZ += l_thick/2;
	PlacedVolume  phys_vol = envelopeVol.placeVolume(l_vol,Position(0,0,layerZ));
	phys_vol.addPhysVolID("layer", l_num);
	layerZ += l_thick/2;
	++l_num;
      }
      ++layerType;
    }

    envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
    DetElement sdet    (lcdd,det_name,det_type,x_det.id());
    Volume motherVol = lcdd.pickMotherVolume(sdet);
    PlacedVolume  physvol = motherVol.placeVolume(envelopeVol,
						  Position(0,0,zmin+totalThickness/2),
						  Rotation(0,0,M_PI/numsides));
    physvol.addPhysVolID("system",det_id);
    physvol.addPhysVolID("barrel",1);        
    sdet.addPlacement(physvol);
        
    if ( reflect ) {
      physvol = motherVol.placeVolume(envelopeVol,
				      Position(0,0,-(zmin+totalThickness/2)),
				      Rotation(M_PI,0,M_PI/numsides));
      physvol.addPhysVolID("system",det_id);
      physvol.addPhysVolID("barrel",2);
      DetElement rdet(lcdd,det_name+"_reflect",det_type,x_det.id());
      rdet.addPlacement(physvol);
    }
    return sdet;
  }
}}
DECLARE_NAMED_DETELEMENT_FACTORY(DD4hep,PolyhedraEndcapCalorimeter2);
