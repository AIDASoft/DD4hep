//====================================================================
//  AIDA Detector description implementation
//  for LDC AHCAL Endcap
//--------------------------------------------------------------------
//
//  Author     : S.Lu
//
// Basic idea:
// 1. Create the Hcal Endcap module envelope (16 modules).
//    Note: with default material Steel235.
//    
// 2. Create the Hcal Endcap Chamber(i.e. Layer) for each module.
//    Create the Layer with slices (Polystyrene,Cu,FR4,air).
//    Place each slice into the chamber with the right position,
//    And registry the IDs for slice
//
// 3. Place the same Layer into the endcap module envelope.
//    It will be repeated repeat 48 times.
//    And registry the IDs for layer, and endcapID.
//
// 4. Place the endcap module into the world volume,
//    with the right position and rotation.
//    And registry the IDs for stave,module and endcapID.
//
// 5. Customer material FR4 and Steel235 defined in materials.xml
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
  //  int         numSides    = dim.numsides();
  double      detZ              = dim.z();    // Hcal endcap modules z start
  double      Hcal_inner_radius = dim.rmin(); // Endcap cernter box hole half length: 362.0
  
  // The way to reaad constant from XML/LCDD file.
  //double      Hcal_outer_radius                = lcdd.constant<double>("Hcal_outer_radius");       // 3395.46 mm
  double      Hcal_radiator_thickness          = lcdd.constant<double>("Hcal_radiator_thickness"); // 20.0mm
  //double      Hcal_chamber_thickness           = lcdd.constant<double>("Hcal_chamber_thickness");  // 6.5mm
  //int         Hcal_endcap_layers               = lcdd.constant<int>("Hcal_endcap_layers");         // 48
  // ========  Note============================================================
  // In the XML/LCDD file, make sure the above constant parameters 
  // are agree with the layering/slices in the detector Ahcal. 
  // ==========================================================================
  
  
  double      Hcal_lateral_structure_thickness = lcdd.constant<double>("Hcal_lateral_structure_thickness");
  double      Hcal_layer_air_gap               = lcdd.constant<double>("Hcal_layer_air_gap");
  double      Hcal_endcap_zmin                 = lcdd.constant<double>("Hcal_endcap_zmin");
  //double      Hcal_back_plate_thickness        = lcdd.constant<double>("Hcal_back_plate_thickness");
  
  // ========= Create Hcal Endcap Modules envelope ============================
  //  Read the Endcap Modules dimensions,
  //  Which defined in the compact XML files. 
  // ==========================================================================
  double dim_x[16];
  double dim_y[16];
  double dim_z[16];
  double pos_x[16];
  double pos_y[16];
  double pos_z[16];
  
  int endcapID =0;
  for(xml_coll_t c(x_det.child(_U(dimensions)),_U(dimensions)); c; ++c) {
    
    xml_comp_t l(c);
    
    dim_x[endcapID] = l.attr<double>(_Unicode(dim_x));
    dim_y[endcapID] = l.attr<double>(_Unicode(dim_y));
    dim_z[endcapID] = l.attr<double>(_Unicode(dim_z));
    pos_y[endcapID] = l.attr<double>(_Unicode(y_offset));
    //std::cout<<" dim_z["<<endcapID<<"]: "<< dim_z[endcapID]/2.0<<std::endl;
    endcapID++;
  }
  
  
  DetElement   sdet(det_name,x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet); 
  
  // ========= Create Hcal Endcap Modules envelope ============================
  //  They will be the volume for palcing the Hcal Chamber(i.e. Layers).
  //  Themself will be placed into the world volume.
  // ==========================================================================
  
  // build 16 Endcap Modules
  for(int ecID=0; ecID<endcapID; ecID++)
    {
      // Hcal Endcap module shape
      double box_half_x= dim_x[ecID]/2.0; // module width, all are same
      double box_half_y= dim_y[ecID]/2.0; // total thickness, all are same
      double box_half_z= dim_z[ecID]/2.0; // module length, changed, 
      
      double x_offset = box_half_x*endcapID-box_half_x*ecID*2.0-box_half_x;
      double y_offset = pos_y[ecID];
      
      Box    EndcapModule(box_half_x,box_half_y,box_half_z);
      
      // define the name of each endcap Module
      string envelopeVol_name   = det_name+_toString(ecID,"_EndcapModule%d");
      
      Volume envelopeVol(envelopeVol_name,EndcapModule,Steel235);
      
      // Set envelope volume attributes.
      envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
      
      
      // ========= Create Hcal Chamber (i.e. Layers) ==============================
      // It will be the sub volume for placing the slices.
      // Itself will be placed into the Hcal Endcap modules envelope.
      // ==========================================================================
      
      // create Layer (air) and place the slices (Polystyrene,Cu,FR4,air) into it. 
      // place the Layer into the Hcal Endcap Modules envelope (Steel235).
      
      // First Hcal Chamber position, start after first radiator
      double layer_pos_y     = - box_half_y + Hcal_radiator_thickness;                      
      
      // Create Hcal Endcap Chamber without radiator
      // Place into the Hcal Encap module envelope, after each radiator 
      int layer_num = 0;
      for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
	xml_comp_t   x_layer = c;
	int          repeat = x_layer.repeat();          // Get number of layers.
	const Layer* lay    = layering.layer(layer_num); // Get the layer from the layering engine.
	
	string layer_name      = envelopeVol_name+"_layer";
	double layer_thickness = lay->thickness();
	DetElement  layer(layer_name,layer_name,x_det.id());
	
	// Active Layer box & volume
	double active_layer_dim_x = box_half_x - Hcal_lateral_structure_thickness;
	double active_layer_dim_y = layer_thickness/2.0;
	double active_layer_dim_z = box_half_z;
	
	// Build chamber including air gap
	// The Layer will be filled with slices, 
	Volume layer_vol(layer_name, Box((active_layer_dim_x + Hcal_layer_air_gap),
					 active_layer_dim_y,active_layer_dim_z), air);
	
	
	// ========= Create sublayer slices =========================================
	// Create and place the slices into Layer
	// ==========================================================================
	
	// Create the slices (sublayers) within the Hcal Chamber.
	double slice_pos_y = -(layer_thickness / 2.0);
	int slice_number = 0;
	for(xml_coll_t k(x_layer,_U(slice)); k; ++k)  {
	  xml_comp_t x_slice = k;
	  string   slice_name      = layer_name + _toString(slice_number,"_slice%d");
	  double   slice_thickness = x_slice.thickness();
	  Material slice_material  = lcdd.material(x_slice.materialStr());
	  DetElement slice(layer,_toString(slice_number,"slice%d"),x_det.id());
	  
	  slice_pos_y += slice_thickness / 2.0;
	  
	  // Slice volume & box
	  Volume slice_vol(slice_name,Box(active_layer_dim_x,slice_thickness/2.0,active_layer_dim_z),slice_material);
	  
	  if ( x_slice.isSensitive() ) {
	    sens.setType("calorimeter");
	    slice_vol.setSensitiveDetector(sens);
	  }
	  // Set region, limitset, and vis.
	  slice_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
	  // slice PlacedVolume
	  PlacedVolume slice_phv = layer_vol.placeVolume(slice_vol,Position(0,slice_pos_y,0));
	  slice_phv.addPhysVolID("slice",slice_number);
	  
	  slice.setPlacement(slice_phv);
	  // Increment Z position for next slice.
	  slice_pos_y += slice_thickness / 2.0;
	  // Increment slice number.
	  ++slice_number;             
	}
	// Set region, limitset, and vis.
	layer_vol.setAttributes(lcdd,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());
	
	
	// ========= Place the Layer (i.e. Chamber) =================================
	// Place the Layer into the Hcal Endcap module envelope.
	// with the right position and rotation.
	// Registry the IDs (layer, stave, module).
	// Place the same layer 48 times into Endcap module
	// ==========================================================================
	
	for (int j = 0; j < repeat; j++)    {
	  
	  // Layer position in y within the Endcap Modules.
	  layer_pos_y += layer_thickness / 2.0;
	  
	  PlacedVolume layer_phv = envelopeVol.placeVolume(layer_vol,
							   Position(0,layer_pos_y,0));
	  // registry the ID of Layer, stave and module
	  layer_phv.addPhysVolID("layer",layer_num);
	  layer_phv.addPhysVolID("endcapID",ecID);
	  // then setPlacement for it.
	  layer.setPlacement(layer_phv);
	  
	  
	  // ===== Prepare for next layer (i.e. next Chamber) =========================
	  // Prepare the chamber placement position and the chamber dimension
	  // ==========================================================================
	  
	  // Increment the layer_pos_y
	  // Place Hcal Chamber after each radiator 
	  layer_pos_y += layer_thickness / 2.0;
	  layer_pos_y += Hcal_radiator_thickness;
	  ++layer_num;         
	}
	
	
      }
      
      
      // =========== Place Hcal Endcap envelope ===================================
      // Finally place the Hcal Endcap envelope into the world volume.
      // Registry the stave(up/down), module(left/right) and endcapID.
      // ==========================================================================
      
      // Acording to the number of staves and modules,
      // Place the same Hcal Endcap module volume into the world volume
      // with the right position and rotation.
      for(int stave_num=0;stave_num<2;stave_num++){
	
	double EndcapModule_pos_x;
	double EndcapModule_pos_y;
	double EndcapModule_pos_z = Hcal_endcap_zmin + box_half_y;
	double rot_EM = 0;
	
	switch (stave_num)
	  {
	  case 0 : 
	    EndcapModule_pos_x = x_offset;
	    EndcapModule_pos_y = y_offset;
	    break;
	  case 1 : 
	    EndcapModule_pos_x = -x_offset;
	    EndcapModule_pos_y = -y_offset;
	    break;
	  }
	
	for(int module_num=0;module_num<2;module_num++) {
	  
	  rot_EM = (module_num==0)?(M_PI/2.0):(-M_PI/2.0);
	  
	  EndcapModule_pos_z = (module_num==0)?EndcapModule_pos_z:-EndcapModule_pos_z;

	  PlacedVolume env_phv = motherVol.placeVolume(envelopeVol,
						       Transform3D(Rotation(0.,0.,rot_EM),
								   Translation3D(EndcapModule_pos_x,
										 EndcapModule_pos_y,
										 EndcapModule_pos_z)));
	  
	  env_phv.addPhysVolID("stave",stave_num);   // y: up /down
	  env_phv.addPhysVolID("module",module_num); // z: +/-
	  env_phv.addPhysVolID("system",x_det.id());
	  sdet.setPlacement(env_phv);
	  
	}
	
      }
      
    }
  
  
  
  return sdet;
}




DECLARE_DETELEMENT(AhcalEndcapCalorimeter, create_detector);
