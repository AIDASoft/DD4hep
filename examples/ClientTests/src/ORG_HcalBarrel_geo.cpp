/*********************************
 * HcalBarrel_geo
 * Implementing a detector
 *
 * Carlos.Solans@cern.ch
 *********************************/

#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

#include <vector>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {

  //XML detector object: DDCore/XML/XMLDetector.h
  DD4hep::XML::DetElement x_det = e;
  
  //Create the DetElement for DD4hep
  DetElement d_det(x_det.nameStr(),x_det.id());
  
  //Pick the mothervolume
  Volume det_vol = lcdd.pickMotherVolume(d_det);

  //XML dimension object: DDCore/XML/XMLDimension.h
  DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
  
  //Tube: DDCore/DD4hep/Shapes.h
  Tube calo_shape(x_det_dim.rmin(),x_det_dim.rmax(),x_det_dim.z(),2*M_PI/x_det_dim.phiBins());
  
  //Create the detector mother volume
  Volume calo_vol(x_det.nameStr()+"_envelope",calo_shape,lcdd.air());

  //Set envelope volume attributes
  calo_vol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  //Place inside the mother volume
  PlacedVolume  calo_plv = det_vol.placeVolume(calo_vol);

  calo_plv.addPhysVolID("system",x_det.id());
  calo_plv.addPhysVolID("barrel",0);
  d_det.setPlacement(calo_plv);

  //Declare this sensitive detector as a calorimeter
  sens.setType("calorimeter");


  int layer_num = 0;  
  float layer_pos_z = 0;
  double tile_phi = 2*M_PI/x_det_dim.phiBins();
  float r = x_det_dim.rmin();
  
  bool debug = true;

  //Repeat layers until we reach the rmax
  while(r<x_det_dim.rmax()){

    //Loop over layers of type: XML Collection_t object: DDCore/XML/XMLElements.h
    for(DD4hep::XML::Collection_t layerIt(x_det,_U(layer));layerIt; ++layerIt){
      
      //Build a layer volume
      DD4hep::XML::Component x_det_layer = layerIt;
      
      float dr = x_det_layer.dr();
      
      string layer_name =  x_det.nameStr()+_toString(layer_num,"_layer%d");
      
      float    x1  = r * tan(tile_phi/2.);
      float    x2  = (r+dr) * tan(tile_phi/2.);
      float    y1  = x_det_dim.z();
      float    y2  = x_det_dim.z();
      float    z   = x_det_layer.dr();
      
      if(debug){
	cout << " r:" << r 
	     << " dr:" << dr 
	     << " x1:" << x1 
	     << " x2:" << x2
	     << " y1:" << y1 
	     << " y2:" << y2 
	     << " z:" << z 
	     << endl;
      }
      
      //Shape a Trapezoid (tile): DDCore/DD4hep/Shapes.h
      Trapezoid layer_shape(x1,x2,y1,y2,z);
      
      //Create a volume with trapezoid shape
      Volume layer_vol(layer_name, layer_shape, lcdd.air());
      layer_vol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det_layer.visStr());
      
      //DetElement layer(layer_name,_toString(layer_num,"layer%d"),x_det.id());
      
      //Fill the volume with tiles
      
      
      int tile_number = 0;  
      vector<Volume> tiles;
      
      //Repeat slices until we reach the end of the calorimeter
      for(xml_coll_t k(x_det_layer,_U(slice)); k; ++k)  {
	
	DD4hep::XML::Component tile_xml       = k;
	string                 tile_name      = layer_name + _toString(tile_number,"_slice%d");
	Material               tile_material  = lcdd.material(tile_xml.materialStr());
	float                  tile_thickness = tile_xml.dz();
	float                  tile_y1        = tile_thickness;
	float                  tile_y2        = tile_thickness;
	float                  tile_z         = x_det_layer.dr();
	
	//Shape a Trapezoid (tile): DDCore/DD4hep/Shapes.h
	Trapezoid tile_shape(x1,x2,tile_y1,tile_y2,tile_z);
	
	//Create a volume with trapezoid shape
	Volume tile_vol(tile_name,tile_shape,tile_material);
	
	if ( tile_xml.isSensitive() ) {
	  tile_vol.setSensitiveDetector(sens);
	}
	
	//Set region, limitset, and visibility settings
	tile_vol.setAttributes(lcdd,tile_xml.regionStr(),tile_xml.limitsStr(),tile_xml.visStr());
	
	tiles.push_back(tile_vol);
	tile_number++;
      }

      //Place the same volumes inside the envelope
      float tile_pos_z = -x_det_dim.z()/2.;
      while(tile_pos_z<x_det_dim.z()/2.){
	tile_number=0;
	for(xml_coll_t k(x_det_layer,_U(slice)); k; ++k)  {
	  
	  DD4hep::XML::Component tile_xml       = k;
	  float                  tile_thickness = tile_xml.dz();
	  
	  //Place the tile inside the layer
	  // PlacedVolume tile_plv = 
	    layer_vol.placeVolume(tiles.at(tile_number),Position(0,tile_pos_z,0));
	  
	  //Increment the z pos of the tile
	  tile_pos_z += tile_thickness;
	  tile_number++;
	  
	}
      }
      
      //Place the same layer around the beam axis phiBins times
      double mod_x_off = r;  
      double mod_y_off = 0;  
      for(int i=0;i<x_det_dim.phiBins();i++){
	if(debug) cout << "Layer:" << i << " phi:" << tile_phi << " rotz:" << (tile_phi*i) << endl;
	double layer_pos_x = mod_x_off * cos(tile_phi*i) - mod_y_off * sin(tile_phi*i);
	double layer_pos_y = mod_x_off * sin(tile_phi*i) + mod_y_off * cos(tile_phi*i);
	Transform3D tr(RotationZYX(M_PI*0.5,M_PI*0.5,0)*RotationZYX(0,tile_phi*i,0),
		       Translation3D(layer_pos_x,layer_pos_y,layer_pos_z));
	PlacedVolume pv = calo_vol.placeVolume(layer_vol,tr);
	pv.addPhysVolID("system",x_det.id());
	pv.addPhysVolID("barrel",0);
	pv.addPhysVolID("layer",i+1);
	//DetElement sd = i==0 ? stave_det : stave_det.clone(_toString(i,"stave%d"));
      }
      
      r += dr;
    }
  }
  
  //Place the calo inside the world
  
  return d_det;
}

DECLARE_DETELEMENT(ORG_HcalBarrel,create_detector)

