// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include <map>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det     = e;
  Material    air       = lcdd.air();
  Material    vacuum    = lcdd.vacuum();
  int         det_id    = x_det.id();
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  bool        reflect   = x_det.reflect();
  DetElement  sdet       (det_name,det_type,det_id);
  Volume      motherVol = lcdd.trackingVolume();
  int         m_id=0, c_id=0, n_sensor=0;
  double      posY;
  map<string,Volume> modules;
    
  for(xml_coll_t mi(x_det,_X(module)); mi; ++mi, ++m_id)  {
    xml_comp_t x_mod   = mi;
    string     m_nam   = x_mod.nameStr();
    string     vol_nam = m_nam+"Volume";
    xml_comp_t trd     = x_mod.child(_X(trd));
    double     x1      = trd.x1();
    double     x2      = trd.x2();
    double     z       = trd.z();
    double     y1, y2, total_thickness=0.;
    xml_coll_t ci(x_mod,_X(module_component));
    for(ci.reset(), total_thickness=0.0; ci; ++ci)
      total_thickness += xml_comp_t(ci).thickness();
      
    y1 = y2 = total_thickness / 2;
    Trapezoid m_envelope(lcdd, m_nam+"Trd", x1, x2, y1, y2, z);
    Volume    m_volume(lcdd, vol_nam, m_envelope, vacuum);
      
    m_volume.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
      
    for(ci.reset(), n_sensor=0, c_id=0, posY=-y1; ci; ++ci, ++c_id)  {
      xml_comp_t c       = ci;
      double     c_thick = c.thickness();
      Material   c_mat   = lcdd.material(c.materialStr());
      string     c_name  = m_nam + _toString(c_id,"_component%d");
      Trapezoid trd(lcdd, c_name, x1, x2, c_thick/2e0, c_thick/2e0, z);
      Volume    vol(lcdd, c_name, trd, c_mat);
        
      vol.setVisAttributes(lcdd.visAttributes(c.visStr()));
        
      PlacedVolume phv = m_volume.placeVolume(vol,Position(0,posY+c_thick/2,0));
      phv.addPhysVolID(_X(component),c_id);
      if ( c.isSensitive() ) {
	sdet.check(n_sensor > 1,"SiTrackerEndcap2::fromCompact: "+c_name+" Max of 2 modules allowed!");
	phv.addPhysVolID(_X(sensor),c_id);
	vol.setSensitiveDetector(sens);
	++n_sensor;
      }
      posY += c_thick;
    }
    modules[m_nam] = m_volume;
  }
  for(xml_coll_t li(x_det,_X(layer)); li; ++li)  {
    xml_comp_t x_layer(li);
    int l_id = x_layer.id();
    int mod_num = 0;
    for(xml_coll_t ri(x_layer,_X(ring)); ri; ++ri)  {
      xml_comp_t x_ring = ri;
      double r        = x_ring.r();
      double phi0     = x_ring.phi0(0);
      double zstart   = x_ring.zstart();
      double dz       = x_ring.dz(0);
      int    nmodules = x_ring.attr<int>(_A(nmodules));
      string m_nam    = x_ring.moduleStr();
      Volume m_vol    = modules[m_nam];
        
      double iphi     = 2*M_PI/nmodules;
      double phi = phi0;
      for(int k=0; k<nmodules; ++k) {
	string m_base = det_name + _toString(l_id,"_layer%d") + _toString(mod_num,"_module%d");
	double x = r*std::cos(phi);
	double y = r*std::sin(phi);
	PlacedVolume pv = motherVol.placeVolume(m_vol,
						Position(x,y,zstart+dz),
						Rotation(-M_PI/2,-M_PI/2-phi,0));
	pv.addPhysVolID("system",det_id).addPhysVolID("barrel",1);
	pv.addPhysVolID("layer", l_id).addPhysVolID("module",mod_num);
	DetElement module (m_base,det_type+"/Module",det_id);
	module.addPlacement(pv);
	sdet.add(module);
          
	if ( reflect ) {
	  pv = motherVol.placeVolume(m_vol,
				     Position(x,y,-zstart-dz),
				     Rotation(-M_PI/2,-M_PI/2-phi,M_PI));
	  pv.addPhysVolID("system",det_id).addPhysVolID("barrel",2);
	  pv.addPhysVolID("layer", l_id).addPhysVolID("module",mod_num);
	  DetElement r_module (m_base+"_reflect",det_type+"/Module",det_id);
	  r_module.addPlacement(pv);
	  sdet.add(r_module);
	}
	dz = -dz;
	phi += iphi;
	++mod_num;
      }
    }
  }
  return sdet;
}

DECLARE_DETELEMENT(SiTrackerEndcap2,create_detector);
