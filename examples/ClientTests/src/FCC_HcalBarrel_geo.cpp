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
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  //XML detector object: DDCore/XML/XMLDetector.h
  xml_dim_t x_det = e;  
  //Create the DetElement for dd4hep
  DetElement d_det(x_det.nameStr(),x_det.id());

  //XML dimension object: DDCore/XML/XMLDimension.h
  xml_dim_t x_det_dim(x_det.dimensions());
  //double inner_r = x_det_dim.rmin();
  //double outer_r = x_det_dim.rmax();
  Assembly calo_vol(x_det.nameStr()+"_envelope");
  PlacedVolume pv;

  //Set envelope volume attributes
  calo_vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

#if 0

  //Declare this sensitive detector as a calorimeter
  Tube tub(inner_r,outer_r,x_det_dim.z()/2.0,0.0,2*M_PI);
  //Volume tub_vol(x_det.nameStr()+"_tube",tub,description.material("PyrexGlass"));
  Volume tub_vol(x_det.nameStr()+"_tube",tub,description.material("Iron"));
  calo_vol.placeVolume(tub_vol);
  sens.setType("calorimeter");
  tub_vol.setSensitiveDetector(sens);
  d_det.setAttributes(description,tub_vol,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
#endif

#if 1

  int layer_num = 0;  
  float layer_pos_z = 0;
  double tile_phi = 2*M_PI/x_det_dim.phiBins();
  float r = x_det_dim.rmin();
  
  bool debug = true;

  Assembly stave_vol(x_det.nameStr()+"_stave_0");


  //Repeat layers until we reach the rmax
  while(r<x_det_dim.rmax()){

    //Loop over layers of type: XML Collection_t object: DDCore/XML/XMLElements.h
    for(dd4hep::xml::Collection_t layerIt(x_det,_U(layer));layerIt; ++layerIt, ++layer_num)   {
      
      //Build a layer volume
      xml_comp_t x_det_layer = layerIt;
      
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
      Volume layer_vol(layer_name, layer_shape, description.air());
      layer_vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det_layer.visStr());
      
      //DetElement layer(layer_name,_toString(layer_num,"layer%d"),x_det.id());
      
      //Fill the volume with tiles
      vector<Volume> tiles;

      //Assembly tile_seq(layer_name+"_seq");
      Trapezoid tile_seq_shape(x1,x2,x_det_layer.dz(),x_det_layer.dz(),x_det_layer.dr());
      Volume tile_seq(layer_name + "_seq",tile_seq_shape,description.air());
      double total_thickness = 0;
      //Repeat slices until we reach the end of the calorimeter
      int slice_num = 0, tile_number = 0;  

      tile_seq.setVisAttributes(description.visAttributes("VisibleGreen"));
      for(xml_coll_t k(x_det_layer,_U(slice)); k; ++k, ++slice_num)  {	
        xml_comp_t tile_xml       = k;
        string     tile_name      = layer_name + _toString(tile_number,"_slice%d");
        Material   tile_material  = description.material(tile_xml.materialStr());
        float      tile_thickness = tile_xml.dz();
        float      tile_y1        = tile_thickness;
        float      tile_y2        = tile_thickness;
        float      tile_z         = x_det_layer.dr();
	
        Trapezoid tile_shape(x1,x2,tile_y1,tile_y2,tile_z);
        Volume tile_vol(tile_name,tile_shape,tile_material);
        pv = tile_seq.placeVolume(tile_vol,Position(0,total_thickness,0));
        pv.addPhysVolID("slice",slice_num);
        total_thickness += tile_thickness;
        if ( tile_xml.isSensitive() ) {
          cout << "Set volume " << tile_name << " sensitive...." << endl;
          tile_vol.setSensitiveDetector(sens);
        }
	
        // Set region, limitset, and visibility settings
        tile_vol.setAttributes(description,tile_xml.regionStr(),tile_xml.limitsStr(),tile_xml.visStr());	
        tiles.push_back(tile_vol);
        tile_number++;
      }
      
      // Place the same volumes inside the envelope
      float tile_pos_z = -x_det_dim.z()/2.;
      int   tile_num = 0;
      while(tile_pos_z<x_det_dim.z()/2.){
        pv = layer_vol.placeVolume(tile_seq,Position(0,tile_pos_z,0));
        pv.addPhysVolID("tile",tile_num);
        tile_pos_z += total_thickness;
        tile_num++;
      }
      
      // Place the same layer around the beam axis phiBins times
      Transform3D tr(RotationZYX(M_PI*0.5,M_PI*0.5,0),Translation3D(r,0,layer_pos_z));
      pv = stave_vol.placeVolume(layer_vol,tr);
      pv.addPhysVolID("layer",layer_num);
      r += dr;
      cout << "+++ R=" << r << endl;
    }
  }
  //double mod_x_off = outer_r - (outer_r-inner_r)/2.0;
  //double mod_y_off = 0;
  int nphi_bins = x_det_dim.phiBins();
  for(int i=0;i<nphi_bins;i++){
    if(debug) cout << "Layer:" << i << " phi:" << tile_phi << " rotz:" << (tile_phi*i) << endl;
    double phi = tile_phi*i;
    //double pos_x = mod_x_off * cos(phi) - mod_y_off * sin(phi);
    //double pos_y = mod_x_off * sin(phi) + mod_y_off * cos(phi);
    Transform3D tr(RotationZYX(phi,0,0),Translation3D(0,0,0));
    pv = calo_vol.placeVolume(stave_vol,tr);
    pv.addPhysVolID("stave",i+1);
  }

  cout << "Number of layers: " << layer_num << endl;
#endif
  //Place the calo inside the world
  PlacedVolume  calo_plv = description.pickMotherVolume(d_det).placeVolume(calo_vol);
  calo_plv.addPhysVolID("system",x_det.id());
  calo_plv.addPhysVolID("barrel",0);
  d_det.setPlacement(calo_plv);

  return d_det;
}

DECLARE_DETELEMENT(FCC_HcalBarrel,create_detector)

