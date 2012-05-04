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

static void placeStaves(double        rmin, 
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
  string nam =   sectVolume.name();
  //numsides = 2;
  for (int module = 0; module < numsides; ++module)  {
    PlacedVolume pv=envelopeVolume.placeVolume(sectVolume,Position(-posX,-posY,0),Rotation(rotX,rotY,0));
    pv.addPhysVolID(_X(stave), 0);
    pv.addPhysVolID(_X(module),module);

    rotY -=  innerRotation;
    posX  = -sectCenterRadius * std::sin(rotY);
    posY  =  sectCenterRadius * std::cos(rotY);
  }
}

static Ref_t create_detector(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det     = e;
  xml_comp_t  staves    = x_det.child(_X(staves));
  xml_dim_t   dim       = x_det.dimensions();

  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  Material    air       = lcdd.air();
  Layering    layering(x_det);
  double      totalThickness = layering.totalThickness();
  int         totalRepeat = 0;
  int         totalSlices = 0;
  double      gap         = xml_dim_t(x_det).gap();
  int         numSides    = dim.numsides();
  double      detZ        = dim.z();
  double      rmin        = dim.rmin();
  DetElement  sdet(det_name,det_type,x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet);

  cout << det_name << "  Gap:" << gap << endl;

  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    int repeat   = x_layer.repeat();
    totalRepeat += repeat;
    totalSlices += x_layer.numChildren(_X(slice));
  }

  PolyhedraRegular polyhedra(lcdd,det_name,numSides,rmin,rmin+totalThickness,detZ);
  Volume           envelopeVol(lcdd,det_name,polyhedra,air);

  // Add the subdetector envelope to the structure.
  double innerAngle     = 2*M_PI/numSides;
  double halfInnerAngle = innerAngle/2;
  double tan_inner      = std::tan(halfInnerAngle) * 2;
  double innerFaceLen   = rmin * tan_inner;
  double outerFaceLen   = (rmin+totalThickness) * tan_inner;
  double staveThickness = totalThickness;

  Trapezoid staveTrdOuter(lcdd,det_name+"_stave_trapezoid_outer",
			  innerFaceLen/2,outerFaceLen/2,detZ/2,detZ/2,staveThickness/2);
  Volume staveOuterVol(lcdd,det_name+"_stave_outer",staveTrdOuter,air);

  Trapezoid staveTrdInner(lcdd,det_name+"_stave_trapezoid_inner",
			  innerFaceLen/2-gap,outerFaceLen/2-gap,detZ/2,detZ/2,staveThickness/2);
  Volume staveInnerVol(lcdd,det_name+"_stave_inner",staveTrdInner,air);

  double layerOuterAngle = (M_PI-innerAngle)/2;
  double layerInnerAngle = (M_PI/2 - layerOuterAngle);
  double layer_pos_z = -(staveThickness / 2);                        
  double layer_dim_x = innerFaceLen/2 - gap * 2;
  int layer_num = 0;

  // Set envelope volume attributes.
  envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t   x_layer = c;
    int          repeat = x_layer.repeat();     // Get number of times to repeat this layer.
    const Layer* lay    = layering.layer(layer_num); // Get the layer from the layering engine.
    // Loop over repeats for this layer.
    for (int j = 0; j < repeat; j++)    {                
      string layer_name      = det_name+_toString(layer_num,"_stave_layer%d");
      double layer_thickness = lay->thickness();
      DetElement  layer(layer_name,det_name+"/Layer",x_det.id());

      // Layer position in Z within the stave.
      layer_pos_z += layer_thickness / 2;
      // Layer box & volume
      Box layer_box(lcdd,layer_name, layer_dim_x, detZ/2, layer_thickness);
      Volume layer_vol(lcdd,layer_name,layer_box,air);

      // Create the slices (sublayers) within the layer.
      double slice_pos_z = -(layer_thickness / 2);
      int slice_number = 0;
      for(xml_coll_t k(x_layer,_X(slice)); k; ++k)  {
	xml_comp_t x_slice = k;
	string   slice_name      = layer_name + _toString(slice_number,"_slice%d");
	double   slice_thickness = x_slice.thickness();
	Material slice_material  = lcdd.material(x_slice.materialStr());
	DetElement slice(slice_name,det_name+"/Layer/Slice",x_det.id());

	slice_pos_z += slice_thickness / 2;
	// Slice box. 
	Box slice_box(lcdd,slice_name,layer_dim_x,detZ/2,slice_thickness);

	// Slice volume.
	Volume slice_vol(lcdd,slice_name,slice_box,slice_material);
	if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
	// Set region, limitset, and vis.
	slice_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
	// slice PlacedVolume
	PlacedVolume slice_phv = layer_vol.placeVolume(slice_vol,Position(0,0,slice_pos_z));
	slice_phv.addPhysVolID(_X(slice),slice_number);

	layer.add(slice);
	// Increment Z position for next slice.
	slice_pos_z += slice_thickness / 2;
	// Increment slice number.
	++slice_number;             
      }
      // Set region, limitset, and vis.
      layer_vol.setAttributes(lcdd,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());

      // Layer physical volume.
      PlacedVolume layer_phv = staveInnerVol.placeVolume(layer_vol,Position(0,0,layer_pos_z));
      layer_phv.addPhysVolID(_X(layer),layer_num);

      sdet.add(layer);

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
  sdet.setVisAttributes(lcdd,staves.visStr(),staveOuterVol);

  // Place the staves.
  placeStaves(rmin,numSides,totalThickness,envelopeVol,innerAngle,staveOuterVol);

  PlacedVolume env_phv = motherVol.placeVolume(envelopeVol,Rotation(0,0,M_PI/numSides));
  env_phv.addPhysVolID(_X(system), sdet.id());
  env_phv.addPhysVolID(_X(barrel), 0);
  sdet.addPlacement(env_phv);
  return sdet;
}

DECLARE_DETELEMENT(PolyhedraBarrelCalorimeter2,create_detector);
