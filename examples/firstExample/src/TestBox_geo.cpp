// $Id: Module_geo.cpp 784 2013-09-19 20:05:24Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for Testing purposes
//--------------------------------------------------------------------
//
//  Author     : C.Hombach, A. Karachaliou
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"


using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens) {
  xml_det_t x_det = e;
 	string det_name = x_det.nameStr();
 	string det_type = x_det.typeStr();
 	Material air = lcdd.air();
  Assembly assembly (det_name+"_assembly");
 	DetElement sdet(det_name,x_det.id());
  Volume motherVol = lcdd.pickMotherVolume(sdet);

  int m_id=0;
  
  PlacedVolume pv;

  for(xml_coll_t mi(x_det,_U(module)); mi; ++mi, ++m_id) {
    xml_comp_t x_mod = mi;
    string m_nam = x_mod.nameStr();
    double x = x_mod.x();
    double y = x_mod.y();
    double z = x_mod.z();

    // a simple box 
    Box box( 10, 10, 1) ;

#define debug_hole 1
#ifdef debug_hole 
// -------- debug code for subtraction solid -----------------
      
#if 0    // cut out a box 
      
      Box hole( 3, 3, 1.01 ) ; // not z slightly larger than original 
#else
#if franks_trap
      // This trap is ill defined. and does not work!
      // cut out a trap
      Trap hole( 4.2, 
		 0., 0.,
		 2., 4.,  3., 
		 0. , 0. ,
		 2., 4., 3.) ;
#else
      // Fixed version from Andrei. This works
      // cut out a trap
      Trap hole( 4.2, 
		 0., 0.,
		 2., 4.,  3., 
		 0. ,
		 2., 4., 3., 0) ;
#endif
#endif
      
      SubtractionSolid solid(  box, hole , Transform3D() ) ;
      
      Volume m_volume(det_name+"_"+m_nam, solid , air);

#else 
// -------- end debug code for subtraction solid -----------------
      

      Volume m_volume(det_name+"_"+m_nam, box , air);
      
#endif
      


    m_volume.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
    pv = motherVol.placeVolume(m_volume,Transform3D(RotationZYX(0,0,0),Position(x,y,z)));
  }
  sdet.setPlacement(pv);
  return sdet;
}
DECLARE_DETELEMENT(TestBox,create_detector);
