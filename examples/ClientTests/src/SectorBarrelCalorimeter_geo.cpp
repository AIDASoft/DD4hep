//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include <limits>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static void placeStaves(DetElement&   parent,
			DetElement&   stave,
			double        rmin, 
			int           numsides, 
			double        total_thickness, 
			Volume        envelopeVolume, 
			double        innerAngle, 
			Volume        sectVolume)
{
  double innerRotation    = innerAngle;
  double offsetRotation   = -innerRotation / 2;
  double sectCenterRadius = rmin + total_thickness / 2;
  double rotX =  M_PI / 2;
  double rotY = -offsetRotation;
  double posX = -sectCenterRadius  * std::sin(rotY);
  double posY =  sectCenterRadius  * std::cos(rotY);

  for (int module = 1; module <= numsides; ++module)  {
    DetElement det  = module>1 ? stave.clone(_toString(module,"stave%d")) : stave;
    PlacedVolume pv = envelopeVolume.placeVolume(sectVolume,Transform3D(RotationZYX(0,rotY,rotX),
									Translation3D(-posX,-posY,0)));
    // Not a valid volID: pv.addPhysVolID("stave", 0);
    pv.addPhysVolID("module",module);
    det.setPlacement(pv);
    parent.add(det);
    rotY -=  innerRotation;
    posX  = -sectCenterRadius * std::sin(rotY);
    posY  =  sectCenterRadius * std::cos(rotY);
  }
}

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det     = e;
  Layering    layering(x_det);
  //xml_comp_t  staves      = x_det.staves();
  xml_dim_t   dim         = x_det.dimensions();
  string      det_name    = x_det.nameStr();
  Material    air         = description.air();
  double      totalThickness = layering.totalThickness();
  int         numSides    = dim.numsides();
  double      detZ        = dim.z();
  double      rmin        = dim.rmin();
  double      rmax        = dim.rmax();
  DetElement  sdet(det_name,x_det.id());
  DetElement  stave("stave1",x_det.id());
  Volume      motherVol = description.pickMotherVolume(sdet);
  PolyhedraRegular polyhedron(numSides,0.,rmin,detZ+10);
  Tube tube(0.,rmin+totalThickness,detZ/2,0,2*M_PI);
  SubtractionSolid sub(tube,polyhedron);  
  Volume           envelopeVol(det_name+"_envelope",sub,air);

  // Add the subdetector envelope to the structure.
  double externalAngle     = 2*M_PI/numSides;
  double halfExternalAngle = externalAngle/2;
  double tan_external      = std::tan(externalAngle);
  double tan_half      = std::tan(halfExternalAngle);
  
  double half_polyFace = rmin * tan_half;
  
  double innerFaceLen   = std::sqrt(rmax*rmax - rmin*rmin)+half_polyFace;
  //double outerFaceLen   = (rmin+totalThickness) * tan_external;
  double staveThickness = totalThickness;

//   Trapezoid staveTrdOuter(innerFaceLen/2,outerFaceLen/2,detZ/2,detZ/2,staveThickness/2);
//   Volume    staveOuterVol(det_name+"_stave",staveTrdOuter,air);
  Assembly    staveOuterVol(det_name+"_stave");
  
//   Trapezoid staveTrdInner(innerFaceLen/2-gap,outerFaceLen/2-gap,detZ/2,detZ/2,staveThickness/2);
//   Volume    staveInnerVol(det_name+"_inner",staveTrdInner,air);
  
  //double layerOuterAngle = (M_PI-externalAngle)/2;
  //double layerexternalAngle = (M_PI/2 - layerOuterAngle);
  double layer_pos_z = -(staveThickness / 2);    
  double layer_pos_x = 0; 
  
  double layer_dim_x = innerFaceLen/2;
  int layer_num = 1;
  
  double layerR = rmin;

  // Set envelope volume attributes.
  envelopeVol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t   x_layer = c;
    int          repeat = x_layer.repeat();            // Get number of times to repeat this layer.
    const Layer* lay    = layering.layer(layer_num-1); // Get the layer from the layering engine.
    // Loop over repeats for this layer.
    for (int j = 0; j < repeat; j++)    {
      string     layer_name      = det_name+_toString(layer_num,"_layer%d");
      double     layer_thickness = lay->thickness();
      DetElement layer(stave,_toString(layer_num,"layer%d"),x_det.id());

      // Layer position in Z within the stave.
      layer_pos_z += layer_thickness / 2;
      // Layer box & volume
      Volume layer_vol(layer_name, Box(layer_dim_x,detZ/2,layer_thickness/2), air);

      // Create the slices (sublayers) within the layer.
      double slice_pos_z = -(layer_thickness / 2);
      int slice_number = 1;
      for(xml_coll_t k(x_layer,_U(slice)); k; ++k)  {
	xml_comp_t x_slice = k;
	string   slice_name      = layer_name + _toString(slice_number,"_slice%d");
	double   slice_thickness = x_slice.thickness();
	Material slice_material  = description.material(x_slice.materialStr());
	DetElement slice(layer,_toString(slice_number,"slice%d"),x_det.id());

	slice_pos_z += slice_thickness / 2;
	// Slice volume & box
	Volume slice_vol(slice_name,Box(layer_dim_x,detZ/2,slice_thickness/2),slice_material);

	if ( x_slice.isSensitive() ) {
	  sens.setType("calorimeter");
	  slice_vol.setSensitiveDetector(sens);
	}
	// Set region, limitset, and vis.
	slice_vol.setAttributes(description,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
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
      layer_vol.setAttributes(description,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());

      // Layer physical volume.
      PlacedVolume layer_phv = staveOuterVol.placeVolume(layer_vol,Position((layer_dim_x+layer_pos_x-half_polyFace),0,layer_pos_z));
      layer_phv.addPhysVolID("layer",layer_num);
      layer.setPlacement(layer_phv);

      layerR += layer_thickness;
      // Increment the layer X dimension.
      layer_pos_x = (layerR-rmin)/tan_external;
      layer_dim_x =  (std::sqrt(rmax*rmax - layerR*layerR)+half_polyFace - layer_pos_x)/2.0;
      cout<<"Rmin: "<< rmin<<" Rmax: "<<rmax<<" half_polyFace: "<<half_polyFace<<" Layer " <<layer_num<<" layerR: "<<layerR<<" layer_dim_x:" <<layer_dim_x<<endl;
      // Increment the layer Z position.
      layer_pos_z += layer_thickness / 2;
      // Increment the layer number.
      ++layer_num;
    }
  }

  // Add stave inner physical volume to outer stave volume.
//   staveOuterVol.placeVolume(staveInnerVol); //not needed
  // Set the vis attributes of the outer stave section.
//   stave.setVisAttributes(description,staves.visStr(),staveOuterVol); //not applicable for Assembly
  // Place the staves.
  placeStaves(sdet,stave,rmin,numSides,totalThickness,envelopeVol,externalAngle,staveOuterVol);

  double z_offset = dim.hasAttr(_U(z_offset)) ? dim.z_offset() : 0.0;
  Transform3D transform(RotationZ(M_PI-(M_PI/numSides)),Translation3D(0,0,z_offset));
  PlacedVolume env_phv = motherVol.placeVolume(envelopeVol,transform);
  env_phv.addPhysVolID("system", sdet.id());
  env_phv.addPhysVolID("barrel", 0);
  sdet.setPlacement(env_phv);

  //sdet.addExtension<LayerStack>(new PolyhedralCalorimeterLayerStack(sdet));
  return sdet;
}

DECLARE_DETELEMENT(SectorBarrelCalorimeter,create_detector)
