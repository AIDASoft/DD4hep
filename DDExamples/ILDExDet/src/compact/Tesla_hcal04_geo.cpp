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
#if 0
namespace DD4hep {
  namespace Geometry {
    struct Hcal04Data : public DetElement::Object {
      typedef Position Dimension;
      Material radiatorMat;
      string model;
      struct Barrel {
	Dimension    bottom, middle, top;
	Dimension    module;
	Dimension    cells;
	double       y1_for_x;
	double       y2_for_x;
	int          numLayer;
      } barrel;
      struct Layer : public Dimension {
	int       id;
        Dimension offset;
      };
      std::vector<Layer> barrel_layers, end_layers, endcap_layers;
      struct Staves {
	int id;
	double phi_start;
	double inner_r, z_offset;
      } stave;
      struct Chambers {
	double x, thickness, z;
      } chamber;
      struct RPC {
	double g10_thickness;
	double glass_thickness;
	double gas_thickness;
	double spacer_thickness;
	double spacer_gap;
      } rpc;
      double fiberGap;
      SensitiveDetector barrelSensitiveDetector;
    };

    struct Hcal04 : public Hcal04Data  {
      Hcal04(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens) : Hcal04Data() {}
      void construct();
      /// Build and place Barrel Regular Modules
      void buildBarrelRegularModules(Volume assembly);
      void buildBarrelEndModules(Volume assembly);
  
      // EndCap Modules
      void buildEndcaps(Volume assembly);

      /// Build Box with RPC1 chamber
      Volume buildRPC1Box(Box box, int layer_id);
    };
  }
}

Hcal04::construct(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  /*
    <constant name="Hcal_spacer_thickness" value="8*mm"/>
    <constant name="Hcal_nlayers" value="38"/>
    <constant name="Hcal_endcap_nlayers" value="38"/>

    <endcap nlayers="Hcal_endcap_nlayers" fiber_gap="Hcal_fiber_gap">
    <barrel sensitive_model="scintillator" nlayers="Hcal_nlayers">
    <top    x=""/>
    <middle x=""/>
    <bottom x=""/>
    <module x=""/>
    <rpc g10_thickness="" glass_thickness="" gas_thickness="" spacer_thickness="Hcal_spacer_thickness" spacer_gap=""/>
    </barrel>
  */
  xml_det_t   x_det = e;
  string      nam   = x_det.nameStr();
  Assembly    assembly(nam+"_assembly");
  xml_comp_t  x_barrel = x_det.child(Unicode("barrel"));
  xml_comp_t  x_endcap = x_det.child(Unicode("endcap"));
  xml_comp_t  x_bottom = x_det.child(Unicode("bottom"));
  xml_comp_t  x_middle = x_det.child(Unicode("middle"));
  xml_comp_t  x_top    = x_det.child(Unicode("top"));
  xml_comp_t  x_cells  = x_det.child(Unicode("cells"));
  xml_comp_t  x_module = x_det.child(Unicode("module"));
  xml_comp_t  x_rpc    = x_det.child(Unicode("rpc"));


  Volume motherVol = lcdd.pickMotherVolume(*this);
  // Visualisation attributes
  VisAttr vis = lcdd.visAttributes(x_det.visStr());

  //--------- BarrelHcal Sensitive detector -----
  model = x_det.attr<string>(Unicode("model"));
  fiberGap = x_det.attr<double>(Unicode("fiber_gap"));
  radiatorMat = lcdd.material(x_radiator.materialStr());

  barrel.numLayer = x_barrel.attr<int>(Unicode("num_layer"));
  barrel.bottom.x = x_bottom.x();
  barrel.middle.x = x_middle.x();
  barrel.top.x    = x_top.x();
  barrel.module.x = x_module.x();
  barrel.y1_for_x = x_module.attr<double>(Unicode("y1_for_x"));
  barrel.y2_for_x = x_module.attr<double>(Unicode("y2_for_x"));
  //double chamber_tickness = x_params.attr<double>(Unicode("chamber_tickness"));
  barrel.cells.x = x_cells.x();
  barrel.cells.z = x_cells.z();

  
  // if RPC1 read the RPC parameters
  if ( model == "RPC1" )   {
    rpc.g10_thickness    =x_rpc.attr<double>(Unicode("g10_thickness"));
    rpc.glass_thickness  =x_rpc.attr<double>(Unicode("glass_thickness"));
    rpc.gas_thickness    =x_rpc.attr<double>(Unicode("gas_thickness"));
    rpc.spacer_thickness =x_rpc.attr<double>(Unicode("spacer_thickness"));
    rpc.spacer_gap       =x_rpc.attr<double>(Unicode("spacer_gap"));
  }
#if 0
  // The cell boundaries does not really exist as G4 volumes. So,
  // to avoid long steps over running  several cells, the 
  // theMaxStepAllowed inside the sensitive material is the
  // pad smaller x or z dimension.
  theMaxStepAllowed= std::min(barrel.cells.x,barrel.cells.z);

  //  Hcal  barrel regular modules
  theBarrilRegSD =     new SD(barrel.cells.x,barrel.cells.z,chamber_tickness,HCALBARREL,"HcalBarrelReg");
  RegisterSensitiveDetector(theBarrilRegSD);
  
  // Hcal  barrel end modules
  theBarrilEndSD = new SD(barrel.cells.x,barrel.cells.z,chamber_tickness,HCALBARREL,"HcalBarrelEnd");
  RegisterSensitiveDetector(theBarrilEndSD);

  // Hcal  endcap modules
  theENDCAPEndSD = new HECSD(barrel.cells.x,barrel.cells.z,chamber_tickness,HCALENDCAPMINUS,"HcalEndCaps");
  RegisterSensitiveDetector(theENDCAPEndSD);
#endif

  // Barrel
  buildBarrelRegularModules(assembly);
  buildBarrelEndModules(assembly);
  
  // EndCap Modules
  buildEndcaps(assembly);

  motherVol.placeVolume(assembly);
}

/// Build and place Barrel Regular Modules
void Hcal04::buildBarrelRegularModules(Volume parent)   {
  // Attention: on bâtit le module dans la verticale
  // à cause du G4Trd et on le tourne avant de le positioner
  Trd trdBottom(barrel.bottom.x/2, barrel.middle.x/2,barrel.module.z/2, barrel.module.z/2, barrel.y1_for_x/2);
  Trd trdTop   (barrel.middle.x/2, barrel.top.x/2,   barrel.module.z/2, barrel.module.z/2, barrel.y2_for_x/2);

  UnionSolid modSolid (trdBottom, trdTop, Position(0,0,(barrel.y1_for_x+barrel.y2_for_x)/2.));
  Volume     modVolume(name+"_module",modSolid,radiatorMat);

  modVolume.setVisAttributes(moduleVis);
  double chambers_y_off_correction = barrel.y2_for_x/2;
  double bottomDimY = barrel.y1_for_x/2;

  VisAttr chamberVis = lcdd.visAttributes("HcalChamberVis");
  G4VisAttributes *VisAtt = new G4VisAttributes(G4Colour(.2,.8,.2));
  VisAtt->SetForceWireframe(true);

  G4UserLimits* pULimits=    new G4UserLimits(theMaxStepAllowed);

  Material scint_mat = lcdd.material("polystyrene");
  Volume chambers[2000];

  double Xoff = 0;
  double Yoff = x_module.offset();
  for(Layers::const_iterator i=barrel_layers.begin(); i != barrel_layers.end(); ++i)  {
    const Layer& layer = *i;
    double thickness = layer.y/2;
    Box  chamberBox(layer.x/2,tiickness.y/2,layer.z/2);
    PlacedVolume pv;
    Volume chamberVol;
    if ( model == "scintillator")      {	
      //fg: introduce (empty) fiber gap - should be filled with fibres and cables
      Box    scintBox(layer.x/2,layer.z/2,(thickness - fiber_gap)/2);
      Volume scintVol(nam+_toString(i,"_scint%d"),scintBox,scint_mat);
      scintVol.setSensitiveDetector(barrelSensitiveDetector);
      //scintVol.setAttributes(lcdd,"",x_slice.limitsStr(),x_slice.visStr());

      //scintVol.setLimits(pULimits);
      //scintVol->SetSensitiveDetector(theBarrilRegSD);

      chamberVol = Volume(nam+_toString(i,"_chamber%d"),chamberBox,lcdd.air());
      pv = chamberVol.placeVolume(scintVol,Position(0,0,-fiber_gap/2));
    }
    else if (model == "RPC1")	{
      chamberVol = BuildRPC1Box(chamberBox,i);
    }
    else    {
      throw runtime_error("Unknown sensitive model:"+model+" for detector hcal04");
    }
    chamberVol.setVisAttributes(chamberVis);
    const Position& pos = layer.offset;
    pv = parent.placeVolume(chamberVol,Position(pos.x,pos.z,pos.y+chambers_y_off_correction));
    pv.addPhysVolID("layer",layer.id);
  }   
}

#if 0

db->exec("select bottom_dim_x/2 AS BHX,midle_dim_x/2. AS MHX, top_dim_x/2 AS THX, y_dim1_for_x/2. AS YX1H,y_dim2_for_x/2. AS YX2H,module_dim_z/2. AS DHZ from barrel_module,barrel_regular_module;");
db->getTuple();
double BottomDimY = db->fetchDouble("YX1H");
double chambers_y_off_correction = db->fetchDouble("YX2H");
  

// Chambers in the Hcal Barrel 
BarrelRegularChambers(EnvLogHcalModuleBarrel,chambers_y_off_correction);

//   // BarrelStandardModule placements
db->exec("select stave_id,module_id,module_type,stave_phi_offset,inner_radius,module_z_offset from barrel,barrel_stave, barrel_module, barrel_modules where module_type = 1;"); //  un module: AND stave_id=1 AND module_id = 2
db->getTuple();
double Y;
Y = db->fetchDouble("inner_radius")+barrel.y1_for_x/2;

do {
  double phirot = db->fetchDouble("stave_phi_offset")*pi/180;
  G4RotationMatrix *rot=new G4RotationMatrix();
  rot->rotateX(pi*0.5); // on couche le module.
  rot->rotateY(phirot);
  new MyPlacement(rot,
		  G4ThreeVector(-Y*sin(phirot),
				Y*cos(phirot),
				db->fetchDouble("module_z_offset")),
		  EnvLogHcalModuleBarrel,
		  "BarrelHcalModule",
		  MotherLog,
		  false,
		  HCALBARREL*100+db->fetchInt("stave_id")*10+
		  db->fetchInt("module_id"));
  theBarrilRegSD->SetStaveRotationMatrix(db->fetchInt("stave_id"),phirot);
  theBarrilRegSD->
    SetModuleZOffset(db->fetchInt("module_id"),
		     db->fetchDouble("module_z_offset"));
 } while(db->getTuple()!=NULL);
}

void Hcal04::BarrelRegularChambers(G4LogicalVolume* MotherLog,double chambers_y_off_correction)  {
  
  G4LogicalVolume * ChamberLog[200];
  G4Box * ChamberSolid;
  
  G4VisAttributes *VisAtt = new G4VisAttributes(G4Colour(.2,.8,.2));
  VisAtt->SetForceWireframe(true);
  //VisAtt->SetForceSolid(true);
  
  db->exec("select layer_id,chamber_dim_x/2. AS xdh,chamber_tickness/2. AS ydh,chamber_dim_z/2. AS zdh, fiber_gap from hcal,barrel_regular_layer,barrel_regular_module;");
  
  G4UserLimits* pULimits=
    new G4UserLimits(theMaxStepAllowed);

  // the scintillator width is the chamber width - fiber gap 
  double fiber_gap = 0.0;
  double dx = chamber_dim_x;
  double dy = chamber_dim_y;
  double dz = chamber_dim_z;
  Material polystyrene = lcdd.material("polystyrene");
  for(size_t i=0; i<num_Layers; ++i)    {
    Box  chamberBox(dx,dy,dz);
    if(SensitiveModel == "scintillator")      {
      //fg: introduce (empty) fiber gap - should be filled with fibres and cables - so far we fill it  with air ...
      Volume chamberVol(name+"_chamber",chamberBox,lcdd.air());
      double scintHalfWidth =  dy - fiber_gap / 2. ;
      // fiber gap can't be larger than total chamber
      assert( scintHalfWidth > 0. ) ;
      
      Box    scintillatorBox(dx,dz,scintHalfWidth);
      Volume scintillatorVol(name+"_scintillator",scintillatorBox,polystyrene);
      // only scinitllator is sensitive
      //ScintLog->SetSensitiveDetector(theBarrilRegSD);
      // Whatever the MyPlacement shit does...
      PlacedVolume pv=chamberVol.placeVolume(scintillatorVol,Position(0,0,- fiber_gap / 2.));
      pv.addPhysVolId("layer",i);
      ChamberLog [i] = chamberVol;
    }
    else if (SensitiveModel == "RPC1")   {
      G4int layer_id = i;
      ChamberLog [layer_id] =
	BuildRPC1Box(ChamberSolid,
		     theBarrilRegSD,
		     layer_id,
		     pULimits);
    }
    else  {
      Control::Abort("Invalid sensitive model in the dababase!",MOKKA_ERROR_BAD_DATABASE_PARAMETERS);
    }

    ChamberLog[db->fetchInt("layer_id")]->SetVisAttributes(VisAtt);
  }   
  
  // Chamber Placements
  // module x and y offsets (needed for the SD)
  db->exec("select 0 AS module_x_offset, module_y_offset from barrel_module;");
  db->getTuple();
  
  double Xoff,Yoff;
  Xoff = db->fetchDouble("module_x_offset");
  Yoff = db->fetchDouble("module_y_offset");
  
  db->exec("select layer_id, 0. as chamber_x_offset,chamber_y_offset,0. as chamber_z_offset from barrel_regular_layer;");
   
  while(db->getTuple()!=NULL){    
    G4int layer_id = db->fetchInt("layer_id");
    new MyPlacement(0,
		    G4ThreeVector(db->fetchDouble("chamber_x_offset"),
				  db->fetchDouble("chamber_z_offset"),
				  db->fetchDouble("chamber_y_offset")+
				  chambers_y_off_correction),
		    //!!attention!! y<->z
		    ChamberLog [layer_id],
		    "ChamberBarrel",
		    MotherLog,false,layer_id);
    theBarrilRegSD->
      AddLayer(layer_id,
	       db->fetchDouble("chamber_x_offset") + Xoff - 
	       ((G4Box *)ChamberLog[layer_id]->GetSolid())->GetXHalfLength(),
	       db->fetchDouble("chamber_y_offset") + Yoff,
	       // - ((G4Box *)ChamberLog [layer_id]->GetSolid())->GetZHalfLength(),
	       db->fetchDouble("chamber_z_offset") - 
	       ((G4Box *)ChamberLog[layer_id]->GetSolid())->GetYHalfLength());    
  }
  helpBarrel.layerPos.push_back( db->fetchDouble("chamber_y_offset") + solidOfLog->GetZHalfLength() );
}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~              Barrel End Modules                 ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Hcal04::BarrelEndModules(G4LogicalVolume* MotherLog)
{
  // End modules
  // Get parameters
  db->exec("select y_dim1_for_z/2. AS YZ1H, y_dim2_for_z/2. AS YZ2H, y_dim3_for_z/2. AS YZ3H, top_end_dim_z/2.  AS TZ from barrel_end_module;");
  db->getTuple();
  double YZ1H = db->fetchDouble("YZ1H");
  double YZ2H = db->fetchDouble("YZ2H");
  double YZ3H = db->fetchDouble("YZ3H");
  double TZ = db->fetchDouble("TZ");
  double chambers_y_off_correction = YZ3H;

  db->exec("select bottom_dim_x/2 AS BHX,midle_dim_x/2. AS MHX, top_dim_x/2 AS THX, y_dim1_for_x/2. AS YX1H,y_dim2_for_x/2. AS YX2H,module_dim_z/2. AS DHZ from barrel_module,barrel_regular_module;");
  db->getTuple();
  double BHX = db->fetchDouble("BHX");
  double MHX = db->fetchDouble("MHX");
  double THX = db->fetchDouble("THX");
  double YX1H = db->fetchDouble("YX1H");
  double YX2H = db->fetchDouble("YX2H");
  double DHZ = db->fetchDouble("DHZ");

  // Attention: on bâtit le module dans la verticale
  // à cause des G4Trd et on le tourne avant de le positioner
  //
  // Base

  double MHXZ = BHX+(YZ1H+YZ2H)*(MHX-BHX)/YX1H;
  G4Trd * Base1 = new G4Trd("Base1_Barrel_End_Module",
			    BHX, // dx1 
			    MHXZ, // dx2
			    DHZ, // dy1
			    DHZ, // dy2
			    YZ1H+YZ2H); // dz
  
  G4Trd * Base2 = new G4Trd("Base2_Barrel_End_Module",
			    MHXZ,  
			    MHX,
			    TZ,
			    TZ,
			    YX1H-(YZ1H+YZ2H));

  G4UnionSolid* Base = 
    new G4UnionSolid("BasesEndSolid",
		     Base1,
		     Base2,
		     0,
		     G4ThreeVector(0,
				   TZ-DHZ,
				   YX1H));
  
  G4Trd * Top = new G4Trd("Top_Barrel_End_Module",
			  MHX, 
			  THX,
			  TZ,
			  TZ,
			  YX2H);
  
  G4UnionSolid* Ensemble1 = 
    new G4UnionSolid("EndSolid",
		     Base,
		     Top,
		     0,
		     G4ThreeVector(0,
				   TZ-DHZ,
				   2*YX1H-(YZ1H+YZ2H)+YX2H));

  double MHXZ1 = BHX+((YZ1H+YZ2H)-(TZ-DHZ))*(MHX-BHX)/YX1H;

  G4RotationMatrix *rot = new G4RotationMatrix();
  rot->rotateZ(pi/2.);


  double pDX = TZ-DHZ;
  double pDz = sqrt(4*YZ2H*YZ2H+pDX*pDX)/2.;  
  double pTheta = pi/2-atan(2*pDz/pDX);

  G4Trap* detail = new G4Trap("Trap",
			      pDz, //pDz
			      -pTheta,    //pTheta
			      0.,       //pPhi
			      MHXZ1, //pDy1
			      G4GeometryTolerance::GetInstance()->GetSurfaceTolerance(), //pDx1  
			      G4GeometryTolerance::GetInstance()->GetSurfaceTolerance(), //pDx2
			      0.,      //pAlp1
			      MHXZ, //pDy2,
			      pDX, //pDx3
			      pDX, //pDx4
			      0.);      //pAlp2
  
    
  G4UnionSolid* Ensemble2 = 
    new G4UnionSolid("EndSolid",
		     Ensemble1,
		     detail,
		     rot,
		     G4ThreeVector(0,
				   DHZ+pDX-pDz*tan(pTheta),
				   YZ1H+YZ2H-pDz));  //-pDX/2/tan(pTheta)
  
  EnvLogHcalModuleBarrel  = new G4LogicalVolume(Ensemble2,
 						RadiatorMaterial,
 						"barrelHcalModule", 
 						0, 0, 0);

  G4VisAttributes * VisAtt = new G4VisAttributes(G4Colour(.8,.8,.2));
  VisAtt->SetForceWireframe(true);
  VisAtt->SetDaughtersInvisible(true);
  //VisAtt->SetForceSolid(true);
  EnvLogHcalModuleBarrel->SetVisAttributes(VisAtt);


  //----------------------------------------------------
  // Chambers in the Hcal Barrel 
  //----------------------------------------------------
  BarrelEndChambers(EnvLogHcalModuleBarrel,chambers_y_off_correction);

  //   // Barrel End Module placements
  db->exec("select stave_id,module_id,module_type,stave_phi_offset,inner_radius,module_z_offset from barrel,barrel_stave, barrel_module, barrel_modules where module_type = 2;");  // un module:  AND module_id = 1 AND stave_id = 1


  // Take care of this return here: if is possible when building
  // the Hcal prototype single module, where there isn't end modules
  // at all !!!
  if(db->getTuple()==NULL) return;

  double Y;
  Y = db->fetchDouble("inner_radius")+YZ1H+YZ2H;
  G4int stave_inv [8] = {1,8,7,6,5,4,3,2};
  do {
    double phirot = db->fetchDouble("stave_phi_offset")*pi/180;
    G4RotationMatrix *rot=new G4RotationMatrix();
    double Z = db->fetchDouble("module_z_offset");
    double Xplace = -Y*sin(phirot);
    double Yplace = Y*cos(phirot);
    G4int stave_number = db->fetchInt("stave_id");
    rot->rotateX(pi*0.5); // on couche le module.

    if(Z>0) {
      rot->rotateZ(pi);
      Xplace = - Xplace;
      stave_number = stave_inv[stave_number-1];
    }
    
    rot->rotateY(phirot);
    new MyPlacement(rot,
		    G4ThreeVector(Xplace,
				  Yplace,
				  Z),
		    EnvLogHcalModuleBarrel,
		    "BarrelHcalModule",
		    MotherLog,
		    false,
		    HCALBARREL*100+stave_number*10+db->fetchInt("module_id"));
    theBarrilEndSD->SetStaveRotationMatrix(db->fetchInt("stave_id"),phirot);
    theBarrilEndSD->
      SetModuleZOffset(db->fetchInt("module_id"),
		       db->fetchDouble("module_z_offset"));
  } while(db->getTuple()!=NULL);
}

void Hcal04::BarrelEndChambers(G4LogicalVolume* MotherLog, 
			       double chambers_y_off_correction)
{
  G4LogicalVolume * ChamberLog[200];
  G4Box * ChamberSolid;

  G4VisAttributes *VisAtt = new G4VisAttributes(G4Colour(.2,.8,.2));
  VisAtt->SetForceWireframe(true);

  db->exec("select barrel_regular_layer.layer_id,chamber_dim_x/2. AS xdh,chamber_tickness/2. AS ydh,chamber_dim_z/2. AS zdh, fiber_gap from hcal,barrel_regular_layer,barrel_end_layer where barrel_regular_layer.layer_id = barrel_end_layer.layer_id ;");

  G4UserLimits* pULimits=
    new G4UserLimits(theMaxStepAllowed);
  
  while(db->getTuple()!=NULL)
    {
      ChamberSolid = 
	new G4Box("ChamberSolid", 
		  db->fetchDouble("xdh"),  //hx
		  db->fetchDouble("zdh"),  //hz attention!
		  db->fetchDouble("ydh")); //hy attention!
      
      if(SensitiveModel == "scintillator")
	{
	  //fg: introduce (empty) fiber gap - should be filled with fibres and cables
	  // - so far we fill it  with air ...
	  G4LogicalVolume *ChamberLogical =
	    new G4LogicalVolume(ChamberSolid,
				CGAGeometryManager::GetMaterial("air"), 
				"ChamberLogical", 
				0, 0, 0);
	   
	  double fiber_gap = db->fetchDouble("fiber_gap")  ;
	  double scintHalfWidth =  db->fetchDouble("ydh") - fiber_gap  / 2. ;

	  // fiber gap can't be larger than total chamber
	  assert( scintHalfWidth > 0. ) ;

	   
	  G4Box * ScintSolid = 
	    new G4Box("ScintSolid", 
		      db->fetchDouble("xdh"),  //hx
		      db->fetchDouble("zdh"),  //hz attention!
		      scintHalfWidth ); //hy attention!
	   
	  G4LogicalVolume* ScintLog =
	    new G4LogicalVolume(ScintSolid,
				CGAGeometryManager::GetMaterial("polystyrene"),
				"ScintLogical", 
				0, 0, pULimits);  
	   
	  // only scinitllator is sensitive
	  ScintLog->SetSensitiveDetector(theBarrilEndSD);
	   
	  int layer_id = db->fetchInt("layer_id") ;
	   
	  new MyPlacement(0, G4ThreeVector( 0,0,  - fiber_gap / 2. ), ScintLog,
			  "Scintillator", ChamberLogical, false, layer_id);   

	  ChamberLog [ layer_id ] = ChamberLogical ;
	   
	  // 	   ChamberLog [db->fetchInt("layer_id")] = 
	  // 	     new G4LogicalVolume(ChamberSolid,
	  // 				 CGAGeometryManager::GetMaterial("polystyrene"),
	  // 				 "ChamberLogical", 
	  // 				 0, 0, pULimits);  
	  // 	   ChamberLog[db->fetchInt("layer_id")]->SetSensitiveDetector(theBarrilEndSD);
	}
      else 
	if (SensitiveModel == "RPC1")
	  {
	    G4int layer_id = db->fetchInt("layer_id");
	    ChamberLog [layer_id] =
	      BuildRPC1Box(ChamberSolid,theBarrilEndSD,layer_id,pULimits);
	  }
	else Control::Abort("Invalid sensitive model in the dababase!",MOKKA_ERROR_BAD_DATABASE_PARAMETERS);
      ChamberLog[db->fetchInt("layer_id")]->SetVisAttributes(VisAtt);
    }
   
   
  // End Chamber Placements
  // module x and y offsets (needed for the SD)
  db->exec("select 0 AS module_x_offset, module_y_offset from barrel_module;");
  db->getTuple();
  double Xoff,Yoff;
  Xoff = db->fetchDouble("module_x_offset");
  Yoff = db->fetchDouble("module_y_offset");
   
  db->exec("select barrel_regular_layer.layer_id, 0. as chamber_x_offset,chamber_y_offset,chamber_z_offset as chamber_z_offset,chamber_dim_z/2 AS YHALF,chamber_dim_x/2. as XHALF from barrel_regular_layer,barrel_end_layer where barrel_regular_layer.layer_id = barrel_end_layer.layer_id;");
   
  while(db->getTuple()!=NULL){
    G4int layer_id = db->fetchInt("layer_id");
    new MyPlacement(0,
		    G4ThreeVector(db->fetchDouble("chamber_x_offset"),
				  db->fetchDouble("chamber_z_offset"),
				  db->fetchDouble("chamber_y_offset")+
				  chambers_y_off_correction),
		    //!!attention!! y<->z
		    ChamberLog [db->fetchInt("layer_id")],
		    "ChamberBarrel",
		    MotherLog,false,layer_id);
    theBarrilEndSD->
      AddLayer(layer_id,
	       db->fetchDouble("chamber_x_offset") + 
	       Xoff - db->fetchDouble("XHALF"),
	       db->fetchDouble("chamber_y_offset") + Yoff,
	       - (db->fetchDouble("chamber_z_offset")+
		  db->fetchDouble("YHALF")));
  }
}


//~              Endcaps                 ~
void Hcal04::Endcaps(G4LogicalVolume* MotherLog)   {
  db->exec("select module_radius AS pRMax, module_dim_z/2. AS pDz, center_box_size/2. AS pRMin from endcap_standard_module;");
  db->getTuple();

  double zPlane[2];
  zPlane[0]=-db->fetchDouble("pDz");
  zPlane[1]=-zPlane[0];

  double rInner[2],rOuter[2];
  rInner[0]=rInner[1]=db->fetchDouble("pRMin");
  rOuter[0]=rOuter[1]=db->fetchDouble("pRMax");
  G4Polyhedra *EndCapSolid=
    new G4Polyhedra("HcalEndCapSolid",
		    0.,
		    360.,
		    32,
		    2,
		    zPlane,
		    rInner,
		    rOuter);
		    

  G4VisAttributes *VisAtt = new G4VisAttributes(G4Colour(.8,.8,.2));
  VisAtt->SetForceWireframe(true);
  VisAtt->SetDaughtersInvisible(true);
  //VisAtt->SetForceSolid(true);

  G4LogicalVolume* EndCapLogical =
    new G4LogicalVolume(EndCapSolid,
			RadiatorMaterial,
			"EndCapLogical",
			0, 0, 0);
  EndCapLogical->SetVisAttributes(VisAtt);

  // build and place the chambers in the Hcal Endcaps
  EndcapChambers(EndCapLogical);

  // Placements
   
  db->exec("select endcap_id,endcap_z_offset from endcap;");
   
  G4RotationMatrix *rotEffect=new G4RotationMatrix();
  rotEffect->rotateZ(pi/8.);
  G4int ModuleNumber = HCALENDCAPPLUS*100+16;
  double Z1=0;
  while(db->getTuple()!=NULL){    
    Z1=db->fetchDouble("endcap_z_offset");
    new MyPlacement(rotEffect,
		    G4ThreeVector(0.,
				  0.,
				  Z1),
		    EndCapLogical,
		    "EndCapPhys",
		    MotherLog,
		    false,
		    ModuleNumber);
    rotEffect=new G4RotationMatrix();
    rotEffect->rotateZ(pi/8.);
    rotEffect->rotateY(pi); // On inverse les endcaps 
    ModuleNumber -= (HCALENDCAPPLUS-HCALENDCAPMINUS)*100 + 6;
  }
  theENDCAPEndSD->
    SetModuleZOffset(0,
		     fabs(Z1));
  theENDCAPEndSD->
    SetModuleZOffset(6,
		     fabs(Z1));
}

void Hcal04::EndcapChambers(G4LogicalVolume* MotherLog)    {
  // Chambers in the Hcal04::Endcaps
  // standard endcap chamber solid:
  db->exec("select chamber_radius AS pRMax, chamber_tickness/2. AS pDz, fiber_gap, center_box_size/2. AS pRMin from endcap_standard_module,hcal;");
  db->getTuple();
  
  // G4Polyhedra Envelope parameters
  double phiStart = 0.;
  double phiTotal = 360.;
  G4int numSide = 32;
  G4int numZPlanes = 2;

  double zPlane[2];
  zPlane[0]=-db->fetchDouble("pDz");
  zPlane[1]=-zPlane[0];

  double rInner[2],rOuter[2];
  rInner[0]=rInner[1]=db->fetchDouble("pRMin");
  rOuter[0]=rOuter[1]=db->fetchDouble("pRMax");

  PolyHedraRegular hedra(2,endcap.rmin,endcap.rmax,endcap.z);
		    
  // G4UserLimits* pULimits=new G4UserLimits(theMaxStepAllowed);
  // standard endcap chamber logical


  G4LogicalVolume* EndCapChamberLogical=0;

  if(SensitiveModel == "scintillator")    {
    //fg: introduce (empty) fiber gap - should be filled with fibres and cables
    // - so far we fill it  with air ...
    EndCapChamberLogical =
      new G4LogicalVolume(EndCapChamberSolid,
			  CGAGeometryManager::GetMaterial("air"), 
			  "EndCapChamberLogical", 
			  0, 0, 0);
	   
    double fiber_gap = db->fetchDouble("fiber_gap")  ;
    double scintHalfWidth =  db->fetchDouble("pDz") - fiber_gap  / 2. ;

    // fiber gap can't be larger than total chamber
    assert( scintHalfWidth > 0. ) ;

	   
    double zPlaneScint[2];
    zPlaneScint[0]=-scintHalfWidth ;
    zPlaneScint[1]=-zPlaneScint[0];

    G4Polyhedra *EndCapScintSolid=
      new G4Polyhedra("EndCapScintSolid",
		      phiStart,
		      phiTotal,
		      numSide,
		      numZPlanes,
		      zPlaneScint,
		      rInner,
		      rOuter);

    G4LogicalVolume* ScintLog =
      new G4LogicalVolume(EndCapScintSolid,
			  CGAGeometryManager::GetMaterial("polystyrene"),
			  "EndCapScintLogical", 
			  0, 0, pULimits);  
	   
    // only scinitllator is sensitive
    ScintLog->SetSensitiveDetector(theENDCAPEndSD);
    new MyPlacement(0, G4ThreeVector( 0,0,  - fiber_gap / 2.  ), ScintLog,
		    "EndCapScintillator", EndCapChamberLogical, false, 0 );   
  }
  else     if (SensitiveModel == "RPC1")      {
    EndCapChamberLogical =
      BuildRPC1Polyhedra(EndCapChamberSolid,
			 theENDCAPEndSD,
			 phiStart,
			 phiTotal,
			 numSide,
			 numZPlanes,
			 zPlane,
			 rInner,
			 rOuter,
			 pULimits);
  }
  else Control::Abort("Invalid sensitive model in the dababase!",MOKKA_ERROR_BAD_DATABASE_PARAMETERS);
  
  G4VisAttributes *VisAtt = new G4VisAttributes(G4Colour(1.,1.,1.));
  EndCapChamberLogical->SetVisAttributes(VisAtt);


  // standard endcap chamber placements
  db->exec("select layer_id,chamber_z_offset AS Zoff from endcap_layer;");
  
  G4int layer_id;
  while(db->getTuple()!=NULL){
    layer_id=db->fetchInt("layer_id");
    new MyPlacement(0,
		    G4ThreeVector(0.,
				  0.,
				  db->fetchDouble("Zoff")),
		    EndCapChamberLogical,
		    "EndCapChamberPhys",
		    MotherLog,false,layer_id);
    theENDCAPEndSD->
      AddLayer(layer_id,
	       0,
	       0,
	       db->fetchDouble("Zoff"));
  }  
}


G4LogicalVolume * 
Hcal04::BuildRPC1Polyhedra(G4Polyhedra* ChamberSolid, 
			   SD* theSD,
			   double phiStart,
			   double phiTotal,
			   G4int numSide,
			   G4int numZPlanes,
			   const double zPlane[],
			   const double rInner[],
			   const double rOuter[],
			   G4UserLimits* pULimits)
{
  // fill the Chamber Envelope with G10
  G4LogicalVolume *ChamberLog =
    new G4LogicalVolume(ChamberSolid,
			CGAGeometryManager::GetMaterial("g10"),
			"RPC1", 
			0, 0, 0);
  //
  // build the RPC glass 
      
  double NewZPlane[2];
  NewZPlane[0] = glass_thickness/2.;
  NewZPlane[1] = -NewZPlane[0];

  G4Polyhedra * GlassSolid =
    new G4Polyhedra("RPC1Glass", 
		    phiStart,
		    phiTotal,
		    numSide,
		    numZPlanes,
		    NewZPlane,
		    rInner,
		    rOuter);	
      
  G4LogicalVolume *GlassLogical =
    new G4LogicalVolume(GlassSolid,
			CGAGeometryManager::GetMaterial("pyrex"),
			"RPC1glass", 
			0, 0, 0);
      
  G4VisAttributes * VisAtt = new G4VisAttributes(G4Colour(.8,0,.2));
  VisAtt->SetForceWireframe(true);
  //VisAtt->SetForceSolid(true);
  GlassLogical->SetVisAttributes(VisAtt);

  //
  // build the gas gap
  //
  NewZPlane[0] = gas_thickness/2.;
  NewZPlane[1] = -NewZPlane[0];
  G4Polyhedra * GasSolid =
    new G4Polyhedra("RPC1Gas", 
		    phiStart,
		    phiTotal,
		    numSide,
		    numZPlanes,
		    NewZPlane,
		    rInner,
		    rOuter);
      
  G4LogicalVolume *GasLogical =
    new G4LogicalVolume(GasSolid,
			CGAGeometryManager::GetMaterial("RPCGAS1"),
			"RPC1gas", 
			0, 0, pULimits);
      
  VisAtt = new G4VisAttributes(G4Colour(.1,0,.8));
  VisAtt->SetForceWireframe(true);
  //VisAtt->SetForceSolid(true);
  GasLogical->SetVisAttributes(VisAtt);

  // PLugs the sensitive detector HERE!
  GasLogical->SetSensitiveDetector(theSD);
  // placing the all. 
  // ZOffset starts pointing to the chamber border.
  double ZOffset = zPlane[0];
      
  // first glass after the g10_thickness
  ZOffset += g10_thickness + glass_thickness/2.;
  new MyPlacement(0,
		  G4ThreeVector(0,
				0,
				ZOffset),
		  GlassLogical,
		  "RPCGlass",
		  ChamberLog,
		  false,
		  0);
      
  // set ZOffset to the next first glass border
  ZOffset += glass_thickness/2.;
      
  // gas gap placing 
  ZOffset += gas_thickness/2.; // center !
  new MyPlacement(0,
		  G4ThreeVector(0,
				0,
				ZOffset),
		  GasLogical,
		  "RPCGas",
		  ChamberLog,
		  false,
		  0);

  // set ZOffset to the next gas gap border
  ZOffset += gas_thickness/2.;
      
  // second glass, after the g10_thickness, the first glass
  // and the gas gap.
  ZOffset += glass_thickness/2.; // center !
  new MyPlacement(0,
		  G4ThreeVector(0,
				0,
				ZOffset),
		  GlassLogical,
		  "RPCGlass",
		  ChamberLog,
		  false,
		  0);
  return ChamberLog;      
}

#endif

/// Build Box with RPC1 chamber
Volume Hcal04::buildRPC1Box(Box box, int layer_id) {
  //		     SD* theSD, 
  //		     G4UserLimits* pULimits)
{
  double glass_thickness = x_glass.thickness();
  double spacer_thickness = ;
  double gas_thickness = ;
  string nam = x_det.name();
  Material g10_mat = lcdd.material("G10");

  // fill the Chamber Envelope with G10
  Volume chamberVol(name+"_rpc",box,g10_mat);
	     
  // build the RPC glass !!attention!! y<->z
  Box    glassBox(box.x(),box.y(),glass_thickness/2.0);
  Volume glassVol(nam+"_RPC_glass",glassBox,lcdd.material("pyrex"));
  glassVol.setVisAttributes(lcdd.visAttributes("HcalRpcGlassVis"));

  // build the standard spacer !!attention!! y<->z
  Box    spacerBox(box.x(),spacer_thickness/2,gas_thickness/2);
  Volume spacerVol(nam+"_spacer",spacerBox,g10_mat);
  spacerVol.setVisAttributes(lcdd.visAttributes("HcalRpcSpacerVis"));

  Box    gasBox(box.x(),box.y(),gas_thickness/2.0);
  Volume gasVol(nam+"_RPC_glass",glassBox,lcdd.material("RPCGAS1"));
  gasVol.setVisAttributes(lcdd.visAttributes("HcalRpcGasVis"));

  // PLugs the sensitive detector HERE!
  // gasVol->SetSensitiveDetector(theSD);
      
  // placing the spacers inside the gas gap
  double MaxY  =  box.y()-spacer_thickness/2;
  for(double ypos=-box.y() + spacer_thickness/2; yypos < MaxY; ypos += spacer_gap)
    gasVol.placeVolume(spacerVol,Position(0,NextY,0));

  // placing the all. ZOffset starts pointing to the chamber border.
  double zpos = -box.z();

  // first glass border after the g10_thickness
  zpos += g10_thickness + glass_thickness/2.;
  chamberVol.placeVolume(glassVol,Position(0,0,zpos));

  // set zpos to the next first glass border + gas gap placing 
  zpos += glass_thickness/2. + gas_thickness/2.;
  PlacedVolume pv  = chamberVol.placeVolume(gasVol,Position(0,0,zpos));
  pv.addPhysVolID("layer",layer_id);

  // set zpos to the next gas gap border + second glass
  zpos += gas_thickness/2. + glass_thickness/2.;
  pv  = chamberVol.placeVolume(glassVol,Position(0,0,zpos));
  return chamberVol;
}

static Ref_t create(LCDD& lcdd, const xml_h& elt, SensitiveDetector& sens)  {
  xml_det_t   x_det = e;
  DetElement  sdet;
  Value<TNamed,Hcal04>* ptr = new Value<TNamed,Hcal04>();
  sdet.assign(ptr,x_det.nameStr(),x_det.typeStr());
  ptr->construct(lcdd,e,sens);
  return sdet;
}
DECLARE_DETELEMENT(Tesla_hcal04,create);
#endif
