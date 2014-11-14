// $Id: ForwardDetector_geo.cpp 1306 2014-08-22 12:29:38Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "XML/Layering.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det      = e;
  xml_dim_t   dim        = x_det.dimensions();
  bool        reflect    = x_det.reflect();
  xml_comp_t  beam       = x_det.beampipe();
  string      det_name   = x_det.nameStr();
  int         id         = x_det.id();
  Material    air        = lcdd.air();
  DetElement  sdet       (det_name,id);
  Layering    layering   (x_det);

  Volume      motherVol  = lcdd.pickMotherVolume(sdet);

  double      rmax       = dim.outer_r();
  double      rmin       = dim.inner_r();
  double      zinner     = dim.inner_z();
  double      outgoingR  = beam.outgoing_r();
  double      incomingR  = beam.incoming_r();
  double      xangle     = beam.crossing_angle();
  double      xangleHalf = xangle/2;
  double      thickness  = layering.totalThickness();
  double      zpos       = zinner + thickness/2;
  // Beampipe position in envelope.
  double      beamPosX   = std::tan(xangleHalf) * zpos;

  // Detector envelope solid. 
  Tube envelopeTube(rmin,rmax,thickness/2);

  // First envelope bool subtracion of outgoing beampipe.
  // Incoming beampipe solid.
  Tube beamInTube(0,outgoingR,thickness);
  // Position of incoming beampipe.
  Position beamInPos(beamPosX,0,0);
  /// Rotation of incoming beampipe.
  Rotation3D  beamInRot(RotationY(1.*xangleHalf));
  Transform3D beamInTrans(beamInRot,beamInPos);

  // Second envelope bool subtracion of outgoing beampipe.
  // Outgoing beampipe solid.
  Tube     beamOutTube(0,incomingR,thickness);
  // Position of outgoing beampipe.
  Position beamOutPos(-beamPosX,0,0);
  // Rotation of outgoing beampipe.
  Rotation3D  beamOutRot(RotationY(-xangleHalf));
  Transform3D beamOutTrans(beamOutRot,beamOutPos);

  // First envelope bool subtraction of incoming beampipe.
  SubtractionSolid envelopeSubtraction1(envelopeTube,beamInTube,beamInTrans);
  SubtractionSolid envelopeSubtraction2(envelopeSubtraction1,beamOutTube,beamOutTrans);

  // Final envelope bool volume.
  Volume envelopeVol(det_name+"_envelope", envelopeSubtraction2, air);

  // Process each layer element.
  double layerPosZ   = -thickness / 2;
  double layerDisplZ = 0;

  int layerCount = 1;
  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    double layerThickness = layering.singleLayerThickness(x_layer);

    // Create tube envelope for this layer, which can be reused in bool definition
    // in the repeat loop below.
    Tube layerTube(rmin,rmax,layerThickness);

    for(int i=0, repeat=x_layer.repeat(); i<repeat; ++i)  {
      string layer_nam = _toString(layerCount,"layer%d");
      // Increment to new layer position.
      layerDisplZ += layerThickness / 2;
      layerPosZ   += layerThickness / 2;

      // First layer subtraction solid.
      DetElement  layer(sdet,layer_nam,sdet.id());
      double      layerGlobalZ = zinner + layerDisplZ;
      double      layerPosX    = std::tan(xangleHalf) * layerGlobalZ;
      Position    layer1SubPos( layerPosX,0,0);
      Position    layer2SubPos(-layerPosX,0,0);

      SubtractionSolid layerSubtraction1(layerTube,beamInTube,Transform3D(beamInRot,layer1SubPos));
      // Second layer subtraction solid.
      SubtractionSolid layerSubtraction2(layerSubtraction1,beamOutTube,Transform3D(beamOutRot,layer2SubPos));
      // Layer LV.
      Volume layerVol(det_name+"_"+layer_nam,layerSubtraction2,air);
      
      // Slice loop.
      int sliceCount = 1;
      double slicePosZ = -layerThickness / 2;
      double sliceDisplZ = 0;
      for(xml_coll_t l(x_layer,_U(slice)); l; ++l)  {
	xml_comp_t x_slice = l;
	string slice_nam = _toString(sliceCount,"slice%d");
	/** Get slice parameters. */
	double sliceThickness = x_slice.thickness();
	Material slice_mat = lcdd.material(x_slice.materialStr());

	// Go to mid of this slice.
	sliceDisplZ += sliceThickness / 2;
	slicePosZ   += sliceThickness / 2;

	// Slice's basic tube.
	Tube sliceTube(rmin,rmax,sliceThickness);
	DetElement slice(layer,slice_nam,sdet.id());
	double sliceGlobalZ = zinner + (layerDisplZ - layerThickness / 2) + sliceDisplZ;
	double slicePosX    = std::tan(xangleHalf) * sliceGlobalZ;

	// First slice subtraction solid.
	SubtractionSolid sliceSubtraction1(sliceTube,beamInTube,Transform3D(beamInRot,Position(slicePosX,0,0)));
	// Second slice subtraction solid.
	SubtractionSolid sliceSubtraction2(sliceSubtraction1,beamOutTube,Transform3D(beamOutRot,Position(-slicePosX,0,0))); 
	// Slice LV.
	Volume sliceVol(det_name+"_"+layer_nam+"_"+slice_nam, sliceSubtraction2, slice_mat);

	if ( x_slice.isSensitive() ) {
	  sens.setType("calorimeter");
	  sliceVol.setSensitiveDetector(sens);
	}
	// Set attributes of slice
	slice.setAttributes(lcdd, sliceVol, x_slice.regionStr(), x_slice.limitsStr(), x_slice.visStr());

	// Place volume in layer
	PlacedVolume pv = layerVol.placeVolume(sliceVol,Position(0,0,slicePosZ));
	pv.addPhysVolID("slice",sliceCount);
	slice.setPlacement(pv);

	// Start of next slice.
	sliceDisplZ += sliceThickness / 2;
	slicePosZ   += sliceThickness / 2;
	++sliceCount;
      }
      // Set attributes of slice
      layer.setAttributes(lcdd, layerVol, x_layer.regionStr(), x_layer.limitsStr(), x_layer.visStr());

      // Layer PV.
      PlacedVolume layerPV = envelopeVol.placeVolume(layerVol,Position(0,0,layerPosZ));
      layerPV.addPhysVolID("layer", layerCount);
      layer.setPlacement(layerPV);

      // Increment to start of next layer.
      layerDisplZ += layerThickness / 2;
      layerPosZ   += layerThickness / 2;
      ++layerCount;
    }
  }
  sdet.setVisAttributes(lcdd, x_det.visStr(), envelopeVol);
  
  // Reflect it.
  if ( reflect )  {
    Assembly assembly(det_name+"_assembly");
    PlacedVolume pv = motherVol.placeVolume(assembly);
    pv.addPhysVolID("system", id);
    sdet.setPlacement(pv);

    DetElement   sdetA(sdet,det_name+"_A",x_det.id());
    pv = assembly.placeVolume(envelopeVol,Transform3D(RotationZ(M_PI),Position(0,0,zpos)));
    pv.addPhysVolID("barrel", 1);
    sdetA.setPlacement(pv);

    DetElement   sdetB = sdetA.clone(det_name+"_B",x_det.id());
    sdet.add(sdetB);
    pv = assembly.placeVolume(envelopeVol,Transform3D(RotationY(M_PI),Position(0,0,-zpos)));
    pv.addPhysVolID("barrel", 2);
    sdetB.setPlacement(pv);
  }
  else  {
    PlacedVolume pv = motherVol.placeVolume(envelopeVol,Transform3D(RotationZ(M_PI),Position(0,0,zpos)));
    pv.addPhysVolID("system", id);
    pv.addPhysVolID("barrel", 1);
    sdet.setPlacement(pv);
  }
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_ForwardDetector,create_detector)
DECLARE_DEPRECATED_DETELEMENT(ForwardDetector,create_detector)
