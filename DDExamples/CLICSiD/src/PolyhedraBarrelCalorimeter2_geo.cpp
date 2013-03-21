// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

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
  double rotY =  M_PI / 2;
  double rotZ = -offsetRotation;
  double posX =  sectCenterRadius  * std::sin(rotZ);
  double posY =  sectCenterRadius  * std::cos(rotZ);
  for (int module = 0; module < numsides; ++module)  {
    DetElement det  = module>0 ? stave.clone(_toString(module,"stave%d")) : stave;
    PlacedVolume pv = envelopeVolume.placeVolume(sectVolume,Position(posX,-posY,0),Rotation(rotZ,rotY,0));
    pv.addPhysVolID("stave", 0);
    pv.addPhysVolID("module",module);
    det.setPlacement(pv);
    parent.add(det);
    rotZ -= innerRotation;
    posX  = sectCenterRadius * std::sin(rotZ);
    posY  = sectCenterRadius * std::cos(rotZ);
  }
}

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det     = e;
  Layering    layering(x_det);
  xml_comp_t  staves      = x_det.staves();
  xml_dim_t   dim         = x_det.dimensions();
  string      det_name    = x_det.nameStr();
  string      det_type    = x_det.typeStr();
  Material    air         = lcdd.air();
  double      totalThickness = layering.totalThickness();
  int         totalRepeat = 0;
  int         totalSlices = 0;
  double      gap         = xml_dim_t(x_det).gap();
  int         numSides    = dim.numsides();
  double      detZ        = dim.z();
  double      rmin        = dim.rmin();
  DetElement  sdet(det_name,x_det.id());
  DetElement  stave("stave0",x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet);

  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    int repeat   = x_layer.repeat();
    totalRepeat += repeat;
    totalSlices += x_layer.numChildren(_U(slice));
  }

  PolyhedraRegular polyhedra(numSides,rmin,rmin+totalThickness,detZ);
  Volume           envelopeVol(det_name+"_envelope",polyhedra,air);

  // Add the subdetector envelope to the structure.
  double innerAngle     = 2*M_PI/numSides;
  double halfInnerAngle = innerAngle/2;
  double tan_inner      = std::tan(halfInnerAngle) * 2;
  double innerFaceLen   = rmin * tan_inner;
  double outerFaceLen   = (rmin+totalThickness) * tan_inner;
  double staveThickness = totalThickness;

  Trapezoid staveTrdOuter(innerFaceLen/2,outerFaceLen/2,detZ/2,detZ/2,staveThickness/2);
  Volume staveOuterVol(det_name+"_stave",staveTrdOuter,air);

  Trapezoid staveTrdInner(innerFaceLen/2-gap,outerFaceLen/2-gap,detZ/2,detZ/2,staveThickness/2);
  Volume staveInnerVol(det_name+"_inner",staveTrdInner,air);

  double layerOuterAngle = (M_PI-innerAngle)/2;
  double layerInnerAngle = (M_PI/2 - layerOuterAngle);
  double layer_pos_z = -(staveThickness / 2);                        
  double layer_dim_x = innerFaceLen/2 - gap * 2;
  int layer_num = 0;

  // Set envelope volume attributes.
  envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t   x_layer = c;
    int          repeat = x_layer.repeat();          // Get number of times to repeat this layer.
    const Layer* lay    = layering.layer(layer_num); // Get the layer from the layering engine.
    // Loop over repeats for this layer.
    for (int j = 0; j < repeat; j++)    {
      string layer_name      = det_name+_toString(layer_num,"_layer%d");
      double layer_thickness = lay->thickness();
      DetElement  layer(stave,_toString(layer_num,"layer%d"),x_det.id());

      // Layer position in Z within the stave.
      layer_pos_z += layer_thickness / 2;
      // Layer box & volume
      Volume layer_vol(layer_name, Box(layer_dim_x,detZ/2,layer_thickness), air);

      // Create the slices (sublayers) within the layer.
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
	Volume slice_vol(slice_name,Box(layer_dim_x,detZ/2,slice_thickness),slice_material);

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

      // Layer physical volume.
      PlacedVolume layer_phv = staveInnerVol.placeVolume(layer_vol,Position(0,0,layer_pos_z));
      layer_phv.addPhysVolID("layer",layer_num);
      layer.setPlacement(layer_phv);

      // Increment the layer X dimension.
      layer_dim_x += layer_thickness * std::tan(layerInnerAngle);// * 2;
      // Increment the layer Z position.
      layer_pos_z += layer_thickness / 2;
      // Increment the layer number.
      ++layer_num;         
    }
  }

  // Add stave inner physical volume to outer stave volume.
  staveOuterVol.placeVolume(staveInnerVol,IdentityPos());
  // Set the vis attributes of the outer stave section.
  stave.setVisAttributes(lcdd,staves.visStr(),staveOuterVol);
  // Place the staves.
  placeStaves(sdet,stave,rmin,numSides,totalThickness,envelopeVol,innerAngle,staveOuterVol);

  PlacedVolume env_phv = motherVol.placeVolume(envelopeVol,Rotation(0,0,M_PI/numSides));
  env_phv.addPhysVolID("system", sdet.id());
  env_phv.addPhysVolID("barrel", 0);
  sdet.setPlacement(env_phv);
  return sdet;
}

DECLARE_DETELEMENT(PolyhedraBarrelCalorimeter2,create_detector);
