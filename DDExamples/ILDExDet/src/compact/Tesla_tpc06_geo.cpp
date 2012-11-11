// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
#define _U(text)  Unicode(#text)

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens_det)  {
  PlacedVolume pv;
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  DetElement  sdet(name,x_det.id());
  Volume      motherVol   = lcdd.pickMotherVolume(sdet);
  xml_comp_t  x_envelope  = x_det.child(_U(envelope));
  xml_comp_t  x_endplates = x_det.child(_U(endplates));
  xml_comp_t  x_barrel    = x_det.child(_U(barrel));
  xml_comp_t  x_inner     = x_det.child(_U(inner_wall));
  xml_comp_t  x_outer     = x_det.child(_U(outer_wall));
  xml_comp_t  x_pads      = x_det.child(_U(pads));
  xml_comp_t  x_cathode   = x_det.child(_U(cathode));

  Material    gasMat      = lcdd.material(x_barrel.attr<string>(_U(chamberGas)));
  Material    mixMat      = lcdd.material(x_endplates.materialStr());
  Material    mylarMat    = lcdd.material("Mylar");

  double      dzEndPlate     = x_endplates.attr<double>(_U(electronics_backend_thickness));
  double      dzCathode      = x_barrel.attr<double>(_U(dzCathode));
  double      cathode_mylar  = x_barrel.attr<double>(_U(cathode_mylar));
  double      cathode_copper = x_barrel.attr<double>(_U(cathode_cupper));

  struct      cylinder_t { double inner_r, outer_r, z; Material mat; };
  cylinder_t  env  = { x_envelope.inner_r(),  x_envelope.outer_r(),  x_envelope.zhalf() };
  cylinder_t  wall = { x_inner.thickness(),   x_outer.thickness(),   0.0};
  cylinder_t  gas  = { env.inner_r + wall.inner_r, env.outer_r - wall.outer_r, env.z};
  cylinder_t  delta_sens = { x_barrel.attr<double>(_U(drInnerInsensitive)),
                             x_barrel.attr<double>(_U(drOuterInsensitive)),
                             env.z - dzEndPlate - dzCathode/2};
  cylinder_t  sens   = { env.inner_r + delta_sens.inner_r,
		 	 env.outer_r - delta_sens.outer_r,
			 delta_sens.z/2 + dzCathode/2};
  double      padHeight = x_pads.height();
  double      padWidth  = x_pads.width();
  double      zEndplate = env.z - dzEndPlate / 2.0;

  // Simple calculations and some named constants
  int         numberPadRows = (int)((sens.outer_r - (sens.inner_r) )/padHeight) ;

  // Volumes for the whole TPC, Walls, Cathode, and Endplate
  Volume assemblyVol(name+"_assembly",Tube(env.inner_r,env.outer_r,env.z),gasMat);
  assemblyVol.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  
  Tube   innerTube(env.inner_r, gas.inner_r, env.z);
  Volume innerVol(name+"_assembly",innerTube,lcdd.material(x_inner.materialStr()));
  innerVol.setVisAttributes(lcdd.visAttributes(x_inner.visStr()));
  assemblyVol.placeVolume(innerVol);

  Tube   outerTube(gas.outer_r, env.outer_r, env.z);
  Volume outerVol(name+"_assembly",outerTube,lcdd.material(x_outer.materialStr()));
  outerVol.setVisAttributes(lcdd.visAttributes(x_outer.visStr()));
  assemblyVol.placeVolume(outerVol);

  //-------------------------------- cathode construction ----------------------------------------//
  Tube   cathodeTube(gas.inner_r, gas.outer_r, dzCathode/2);
  Volume cathodeVol (name+"_cathode",cathodeTube,lcdd.air());
  cathodeVol.setVisAttributes(lcdd.visAttributes(x_cathode.visStr()));
  assemblyVol.placeVolume(cathodeVol);

  Tube   cuTube(gas.inner_r, gas.outer_r, cathode_copper/2);
  Volume cuVol (name+"_cathode_cu",cuTube,lcdd.material(x_cathode.materialStr()));
  cathodeVol.placeVolume(cuVol,Position(0,0, (dzCathode-cathode_copper)/2));
  cathodeVol.placeVolume(cuVol,Position(0,0,-(dzCathode-cathode_copper)/2));

  Tube   mylarTub(gas.inner_r, gas.outer_r,cathode_mylar/2);
  Volume mylarVol(name+"_cathode_mylar",mylarTub,mylarMat);
  cathodeVol.placeVolume(cuVol,Position(0,0, (dzCathode-cathode_mylar)/2-cathode_copper));
  cathodeVol.placeVolume(cuVol,Position(0,0,-(dzCathode-cathode_mylar)/2+cathode_copper));

  //-----------------------------------------------------------------------------------------------//
  Rotation reflect_rot(M_PI,0,M_PI);
  Tube   endplateSolid(gas.inner_r, gas.outer_r, dzEndPlate / 2);
  Volume endplateVol  (name+"_endplate",endplateSolid,lcdd.air());
  endplateVol.setVisAttributes(lcdd.visAttributes(x_endplates.visStr()));
  pv = assemblyVol.placeVolume(endplateVol,Position(0,0,+zEndplate));
  pv.addPhysVolID("size",0);
  pv = assemblyVol.placeVolume(endplateVol,Position(0,0,-zEndplate),reflect_rot);
  pv.addPhysVolID("size",1);

  // TPC Sensitive Detector
  LimitSet limits = lcdd.limitSet(x_det.limitsStr());
  Tube   sensTub(sens.inner_r, sens.outer_r, delta_sens.z / 2);
  Volume sensVol(name+"_gas",sensTub,gasMat);
  sensVol.setVisAttributes(lcdd.invisible());
  sensVol.setLimitSet(limits);
  assemblyVol.placeVolume(sensVol,Position(0,0,+sens.z)).addPhysVolID("side",0);
  assemblyVol.placeVolume(sensVol,Position(0,0,-sens.z),reflect_rot).addPhysVolID("side",1);

  // SJA lets have a go at putting in the pad rows
  for (int layer = 0; layer < numberPadRows; layer++) {
    // create twice the number of rings as there are pads, producing an lower and upper part 
    // of the pad with the boundry between them the pad-ring centre
    cylinder_t lower, upper;
    lower.inner_r = sens.inner_r + (layer * padHeight);
    lower.outer_r = lower.inner_r + padHeight/2;
    upper.inner_r = lower.outer_r;
    upper.outer_r = lower.outer_r + padHeight/2;
    Tube   lowerTub(lower.inner_r,lower.outer_r,delta_sens.z / 2);
    Volume lowerVol(name+_toString(layer,"_lower_layer%d"),lowerTub,gasMat);
    lowerVol.setVisAttributes(lcdd.invisible());
    lowerVol.setSensitiveDetector(sens_det);
    lowerVol.setLimitSet(limits);
    sensVol.placeVolume(lowerVol).addPhysVolID("layer",layer+1);

    Tube   upperTub(upper.inner_r,upper.outer_r,delta_sens.z / 2);
    Volume upperVol(name+_toString(layer,"_upper_layer%d"),upperTub,gasMat);
    upperVol.setVisAttributes(lcdd.invisible());
    upperVol.setSensitiveDetector(sens_det);
    upperVol.setLimitSet(limits);
    sensVol.placeVolume(upperVol).addPhysVolID("layer",layer+1);
  }
  // SJA end of pad rows 

  // Assembly of the TPC Endplate
  double fracRadLength = 0;
  double zCursor = -dzEndPlate / 2;
  int np = 0;
  for(xml_coll_t c(x_endplates,_U(endplate)); c; ++c, ++np) {
    xml_comp_t p(c);
    double dz = p.dz();
    string vis = p.visStr();
    Material m = lcdd.material(p.materialStr());
    Volume   v(name+_toString(np,"_piece%d"),Tube(gas.inner_r,gas.outer_r,dz/2),m);
    if ( vis.empty() )
      v.setVisAttributes(lcdd.invisible());
    else
      v.setVisAttributes(lcdd.visAttributes(vis));
    endplateVol.placeVolume(v,Position(0,0,zCursor+dz/2)).addPhysVolID("piece",np);
    fracRadLength += dz / m.radLength();
    zCursor += dz;
    if (zCursor > +dzEndPlate/2)  {
      throw runtime_error("TPC06: Overfull TPC endplate.");
    }
  }

  double dzPiece = dzEndPlate/2-zCursor;
  Volume endVol(name+"_end_vol",Tube(gas.inner_r,gas.outer_r,dzPiece/2),mixMat);
  endVol.setVisAttributes(lcdd.invisible());
  endplateVol.placeVolume(endVol,Position(0,0,zCursor+dzPiece/2)).addPhysVolID("piece",0);
  fracRadLength += dzPiece / mixMat.radLength();

  pv = motherVol.placeVolume(assemblyVol);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_tpc06,create_element);
