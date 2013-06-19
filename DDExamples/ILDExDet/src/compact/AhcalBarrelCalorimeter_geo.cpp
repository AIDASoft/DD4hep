//====================================================================
//  AIDA Detector description implementation
//  for LDC AHCAL Barrel
//--------------------------------------------------------------------
//
//  Author     : S.Lu
//
// Basic idea:
// 1. Create the Hcal Barrel module envelope by two shapers.
//    Note: with default material Steel235.
//    
// 2. Create the Hcal Barrel Chamber(i.e. Layer)
//    Create the Layer with slices (Polystyrene,Cu,FR4,air).
//    Note: there is NO radiator (Steel235) in Layer/Chamber.
//
// 3. Place the Layer into the Hcal Barrel envelope,
//    with the right position and rotation.
//    And registry the IDs for slice,layer,stave,module and system.
//
// 4. Customer material FR4 and Steel235 defined in materials.xml
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h element, SensitiveDetector sens)  {
  xml_det_t   x_det     = element;
  Layering    layering(x_det);
  xml_dim_t   dim         = x_det.dimensions();
  string      det_name    = x_det.nameStr();
  string      det_type    = x_det.typeStr();
  Material    air         = lcdd.air();
  Material    Steel235    = lcdd.material(x_det.materialStr());
  double      gap         = xml_dim_t(x_det).gap();
  int         numSides    = dim.numsides();

  // Hcal Barrel module shapers' parameters
  double      detZ              = dim.z();
  double      Hcal_inner_radius = dim.rmin();

  // The way to reaad constant from XML/LCDD file.
  double      Hcal_outer_radius                = lcdd.constant<double>("Hcal_outer_radius");       // 3395.46 mm
  double      Hcal_radiator_thickness          = lcdd.constant<double>("Hcal_radiator_thickness"); // 20.0mm
  double      Hcal_chamber_thickness           = lcdd.constant<double>("Hcal_chamber_thickness");  // 6.5mm
  int         HcalBarrel_layers                = lcdd.constant<int>("HcalBarrel_layers");          // 48
  // ========  Note============================================================
  // In the XML/LCDD file, make sure the above constant parameters 
  // are agree with the layering/slices in the detector AhcalBarrelCalorimeter. 
  // ==========================================================================


  double      Hcal_lateral_structure_thickness = lcdd.constant<double>("Hcal_lateral_structure_thickness");
  double      Hcal_layer_air_gap               = lcdd.constant<double>("Hcal_layer_air_gap");
  double      Hcal_back_plate_thickness        = lcdd.constant<double>("Hcal_back_plate_thickness");
  double      totalThickness_Hcal_Barrel = (Hcal_radiator_thickness + Hcal_chamber_thickness) 
                                           * HcalBarrel_layers + Hcal_back_plate_thickness;
  double      Hcal_module_radius         = Hcal_inner_radius + totalThickness_Hcal_Barrel;

  double      Hcal_Barrel_rotation             = lcdd.constant<double>("Hcal_Barrel_rotation");

  DetElement  sdet(det_name,x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet);


  // ========= Create Hcal Barrel envelope ====================================
  //  It will be the volume for palcing the Hcal Barrel Chamber(i.e. Layers).
  //  Itself will be placed into the world volume.
  // ==========================================================================

  // Hcal Barrel module shapers
  PolyhedraRegular polyhedra_shaper("polyhedra",numSides,Hcal_inner_radius,Hcal_module_radius,detZ*2);
  Tube             tube_shaper(0.0,Hcal_outer_radius, detZ, 0.0, 2.0*M_PI);

  Position pos(0, 0, 0);
  Rotation rot(M_PI/8.,0,0); // Check the rotation with ECAL and HCAL technical design

  // Create Hcal Barrel volume with material Steel235 
  IntersectionSolid barrelModuleSolid(det_name+"ModuleSolid",
				tube_shaper,polyhedra_shaper,
				pos, rot);
  
  Volume           envelopeVol(det_name+"_envelope",barrelModuleSolid,Steel235);

  // Set envelope volume attributes.
  envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());


  // ========= Create Hcal Barrel Chmaber (i.e. Layers) =======================
  // It will be the sub volume for placing the slices.
  // Itself will be placed into the Hcal Barrel envelope.
  // ==========================================================================

  // create Layer (air) and place the slices (Polystyrene,Cu,FR4,air) into it. 
  // place the Layer into the HcalBarrel envelope (Steel235).
  double innerAngle      = 2*M_PI/numSides;
  double halfInnerAngle  = innerAngle/2;
  double tan_inner       = std::tan(halfInnerAngle) * 2;
  double cos_inner       = std::cos(halfInnerAngle);

  double innerFaceLen    = (Hcal_inner_radius + Hcal_radiator_thickness) * tan_inner;

  double layerOuterAngle = (M_PI-innerAngle)/2;
  double layerInnerAngle = (M_PI/2 - layerOuterAngle);

  // First Hcal Barrel Chamber position, start after first radiator
  double layer_pos_y     = Hcal_inner_radius + Hcal_radiator_thickness;                      
  double layer_dim_x     = innerFaceLen/2 - gap/2;

  // Create Hcal Barrel Chamber without radiator
  // Place into the Hcal Barrel envelope, after each radiator 
  int layer_num = 0;
  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t   x_layer = c;
    int          repeat = x_layer.repeat();          // Get number of layers.
    const Layer* lay    = layering.layer(layer_num); // Get the layer from the layering engine.
    // Loop over repeats for this layer.
    for (int j = 0; j < repeat; j++)    {
      string layer_name      = det_name+_toString(layer_num,"_layer%d");
      double layer_thickness = lay->thickness();
      DetElement  layer(layer_name,_toString(layer_num,"layer%d"),x_det.id());

      // Layer position in Z within the stave.
      layer_pos_y += layer_thickness / 2.0;
      // Active Layer box & volume
      double active_layer_dim_x = layer_dim_x;
      double active_layer_dim_y = (detZ-Hcal_lateral_structure_thickness)/2.;
      double active_layer_dim_z = layer_thickness/2.0;

      // The same Layer will be filled with skices, 
      // and placed into the HcalBarrel 16 times: 8 Staves * 2 modules: TODO 32 times.
      Volume layer_vol(layer_name, Box(active_layer_dim_x,active_layer_dim_y,active_layer_dim_z), air);


      // ========= Create sublayer slices =========================================
      // Create and place the slices into Layer
      // ==========================================================================

      // Create the slices (sublayers) within the Hcal Barrel Chamber.
      double slice_pos_z = -(layer_thickness / 2);
      int slice_number = 0;
      for(xml_coll_t k(x_layer,_U(slice)); k; ++k)  {
	xml_comp_t x_slice = k;
	string   slice_name      = layer_name + _toString(slice_number,"_slice%d");
	double   slice_thickness = x_slice.thickness();
	Material slice_material  = lcdd.material(x_slice.materialStr());
	DetElement slice(layer,_toString(slice_number,"slice%d"),x_det.id());

	slice_pos_z += slice_thickness / 2;

	// Slice volume & box
	double slice_dim_x = active_layer_dim_x - Hcal_layer_air_gap ;
	Volume slice_vol(slice_name,Box(slice_dim_x,active_layer_dim_y,slice_thickness/2.0),slice_material);

	if ( x_slice.isSensitive() ) {
	  sens.setType("calorimeter");
	  slice_vol.setSensitiveDetector(sens);
	}
	// Set region, limitset, and vis.
	slice_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
	// slice PlacedVolume
	PlacedVolume slice_phv = layer_vol.placeVolume(slice_vol,Position(0,0,slice_pos_z));
	slice_phv.addPhysVolID("slice",slice_number);

	slice.setPlacement(slice_phv);
	// Increment Z position for next slice.
	slice_pos_z += slice_thickness / 2;
	// Increment slice number.
	++slice_number;             
      }
      // Set region, limitset, and vis.
      layer_vol.setAttributes(lcdd,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());


      // ========= Place the Layer (i.e. Chamber) =================================
      // Place the Layer into the Hcal Barrel envelope.
      // with the right position and rotation.
      // Registry the IDs (layer, stave, module).
      // ==========================================================================

      // Acording to the number of staves and modules,
      // Place the same Layer into the HcalBarrel envelope
      // with the right position and rotation.
      for(int stave_num=0;stave_num<8;stave_num++){
	double delte_phi = M_PI/4.0*(stave_num);
	string stave_layer = _toString(stave_num,"stave%d_layer");
	// Layer physical volume.
	double layer_pos_z = active_layer_dim_y + Hcal_lateral_structure_thickness;
	for(int module_num=0;module_num<2;module_num++)
	  {
	    layer_pos_z = (module_num==0)?layer_pos_z:-layer_pos_z;
	    PlacedVolume layer_phv = envelopeVol.placeVolume(layer_vol,
							     Position(layer_pos_y*std::sin(delte_phi),
								      layer_pos_y*std::cos(delte_phi),
								      layer_pos_z),
							     Rotation(-delte_phi, M_PI/2.,0));
	    // registry the ID of Layer, stave and module
	    layer_phv.addPhysVolID("layer",layer_num);
	    layer_phv.addPhysVolID("stave",stave_num);
	    layer_phv.addPhysVolID("module",module_num);
	    // then setPlacement for it.
	    layer.setPlacement(layer_phv);

	  }
      }


      // ===== Prepare for next layer (i.e. next Chamber) =========================
      // Prepare the chamber placement position and the chamber dimension
      // ==========================================================================

      // Prepare for next Layer
      // Increment the layer_pos_y, and calculate the next layer_dim_x
      // Place Hcal Barrel Chamber after each radiator 
      layer_pos_y += layer_thickness / 2;
      layer_pos_y += Hcal_radiator_thickness;
      // Increment the layer X dimension.
      if((layer_pos_y + layer_thickness ) < (Hcal_outer_radius - Hcal_back_plate_thickness)* cos_inner)
	layer_dim_x += (layer_thickness + Hcal_radiator_thickness) * std::tan(layerInnerAngle);
      else
	layer_dim_x = std::sqrt((Hcal_outer_radius-Hcal_radiator_thickness)
				*(Hcal_outer_radius-Hcal_radiator_thickness)
				- layer_pos_y*layer_pos_y) - gap/2;
      // Increment the layer number.
      ++layer_num;         
    }
  }


  // =========== Place Hcal Barrel envelope ===================================
  // Finally place the Hcal Barrel envelope into the world volume.
  // Registry the system ID.
  // ==========================================================================

  // Place Hcal Barrel volume into the world volume
  PlacedVolume env_phv = motherVol.placeVolume(envelopeVol,Rotation(0,0,Hcal_Barrel_rotation));

  // registry the system id
  env_phv.addPhysVolID("system", sdet.id());
  sdet.setPlacement(env_phv);
  return sdet;
}




DECLARE_DETELEMENT(AhcalBarrelCalorimeter, create_detector);
