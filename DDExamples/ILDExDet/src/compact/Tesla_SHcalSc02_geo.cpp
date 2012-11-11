// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  This is not too bad an example of a detector constructor, 
//  which is only driven by global parameters.
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"

namespace DD4hep { namespace Geometry {
  struct SHcalSc02Data : public DetElement::Object  {
    enum { HCALBARREL=1, HCALENDCAPPLUS=2, HCALENDCAPMINUS=3 };
    
    double theMaxStepAllowed;  ///<maximum step allowed in GEANT4

    /* Run time parameters: taken from the data base (default values) 
     * or from the user's steering file (user value).
     */
    double      Hcal_radiator_thickness;          ///<thickness of the HCAL absorber 
    std::string Hcal_radiator_material;           ///<type of the HCAL absorber, i.e. steel or tungsten
    int         Hcal_ring;                        ///<=0 no rings, =1 use rings
    double      Hcal_radial_ring_inner_gap;       ///<inner gap of the radial ring
    std::string Hcal_sensitive_model;             ///<HCAL sensitive mode: 'Scintillator' or 'RPC1'
    double      Hcal_back_plate_thickness;        ///<thickness of the HCAL back plate
    double      Hcal_stave_gaps;                  ///<gapg between HCAL staves
    double      Hcal_modules_gap;                 ///<gap between HCAL modules
    int         Hcal_nlayers;                     ///<number of HCAL layers (default: 48)
    int         Hcal_barrel_end_module_type;      ///<type of the HCAL modules (default: 1)
    double      Hcal_fiber_gap;                   ///<gap between HCAL fibers
    double      Hcal_chamber_tickness;            ///<thickness of the HCAL chambers
    double      Hcal_inner_radius;                ///<inner radius of the HCAL
    double      TPC_Ecal_Hcal_barrel_halfZ;       ///<half-length of the HCAL barrel (including gap between modules)
    double      Hcal_normal_dim_z;                ///<length of the HCAL modules along z
    double      Hcal_top_end_dim_z;               ///<length of the HCAL module's top along z
    double      Hcal_start_z;                     ///<HCAL start position in z
    double      Ecal_endcap_outer_radius;         ///<outer radius of the ECAL endcap
    double      Ecal_endcap_zmin;                 ///<minimum z of the ECAL endcap
    double      Ecal_endcap_zmax;                 ///<maximum z of the ECAL endcap
    double      Hcal_lateral_plate_thickness;     ///<thickness of the HCAL lateral plate
    double      Hcal_cells_size;                  ///<size of the HCAL cell
    double      Hcal_digi_cells_size;             ///<size of the HCAL cell

    double      Hcal_endcap_cables_gap;           ///<cables gap in the endcap
    double      Hcal_endcap_ecal_gap;             ///<gap between ECAL and HCAL
    double      Hcal_endcap_rmax;                 ///<maximum radius of the endcaps
    double      Hcal_endcap_center_box_size;      ///<size of the HCAL endcap center box
    double      Hcal_endcap_sensitive_center_box; ///<siez of the HCAL sensitive center box
    double      Hcal_endcap_radiator_thickness;   ///<thickness of the radiator in the HCAL endcap
    std::string Hcal_endcap_radiator_material;    ///<type of absorber material in the HCAL endcap
    int         Hcal_endcap_nlayers;              ///<number of layers in the HCAL endcap
    double      Hcal_endcap_total_z;              ///<total length along z of the HCAL endcap

    double      Hcal_total_dim_y;                 ///<total dimension of the HCAL detector along the y-axis
    double      Hcal_module_radius;               ///<radius of an HCAL module
    double      Hcal_y_dim2_for_x;                ///<y-dimension of the lower part of the HCAL module
    double      Hcal_y_dim1_for_x;                ///<y-dimension of the upper part of the HCAL module
    double      Hcal_bottom_dim_x;                ///<x-dimension of the bottom part of the HCAL module
    double      Hcal_midle_dim_x;                 ///<x-dimension of the middle part of the HCAL module
    double      Hcal_top_dim_x;                   ///<x-dimension of the top part of the HCAL module
    double      Hcal_regular_chamber_dim_z;       ///<z-dimension of the HCAL chamber
    double      Hcal_cell_dim_x;                  ///<dimension of the HCAL cell along the x-axis
    double      Hcal_cell_dim_z;                  ///<dimension of the HCAL cell along the z-axis
    double      Hcal_digi_cell_dim_x;             ///<x-dimension of the HCAL cell
    double      Hcal_digi_cell_dim_z;             ///<z-dimension of the HCAL cell 

    double      Hcal_layer_support_length;        ///<length of the HCAL layer support
    double      Hcal_layer_air_gap;               ///<air gap in the HCAL layer
    double      Hcal_chamber_thickness;           ///<thickness of the HCAL chamber
    double      Hcal_middle_stave_gaps;           ///<gap in the middle of HCAL staves
    bool        Hcal_apply_Birks_law;             ///<flag for applying (or not) the Birks law
    double      Hcal_scintillator_thickness;      ///<thickness of the HCAL scintillator

    struct {
      SensitiveDetector sensDet;
      VisAttr           radiatorVis;
      Material          radiatorMat;         ///<radiator (i.e. absorber) material in the HCAL
      VisAttr           gapVis;
    } m_barrel, m_endcap;

    Material    m_stainlessSteel;
    Material    m_polystyrene;
    Material    m_aluminum;

    VisAttr     m_moduleVis;
    VisAttr     m_scintillatorVis;
    VisAttr     m_chamberVis;
    VisAttr     m_chamberGapVis;
    VisAttr     m_supportTrapVis;
    LimitSet    m_limits;    

    LCDD*       lcdd;
    std::string name;
    DetElement  self;
  };
  struct SHcalSc02 : public SHcalSc02Data  {
  protected:
    /// Construct the detector structure and geometry
    Assembly constructDetector();
    /// Construct the barrel modules
    void constructBarrel(Assembly assembly);
    /// Construct the endcap modules
    void constructEndcaps(Assembly assembly);
    /// Construct the endcap rings
    void constructEndcapRings(Assembly assembly);
    void constructBarrelChambers(Volume modVolume,double chambers_y_off_correction);

  public:
    /// Standard constructor
    SHcalSc02() : SHcalSc02Data() {}
    /// Detector construction function
    DetElement construct(LCDD& lcdd, xml_det_t e);
    /// Calculate x-length of an HCAL barrel layer
    void calculateXLayer(int layer_id, int &logical_layer_id,
			 double &xOffset, double &x_halfLength, double &xShift);

    /// Calculate size of the fractional tile
    void calculateFractTileSize(double x_length, 
				double x_integerTileSize, 
				double &x_fractionalTileSize);
  };
}}
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
#define _U(text)  Unicode(#text)

/// Detector construction function
DetElement SHcalSc02::construct(LCDD& l, xml_det_t x_det)  {
  lcdd     = &l;
  name     = x_det.nameStr();
  self.assign(dynamic_cast<Value<TNamed,SHcalSc02>*>(this),name,x_det.typeStr());
  self._data().id = x_det.id();

  xml_comp_t x_barrel             = x_det.child(_X(barrel));
  xml_comp_t x_endcap             = x_det.child(_X(endcap));
  xml_comp_t x_barrel_rad         = x_barrel.child(_U(radiator));
  xml_comp_t x_endcap_rad         = x_endcap.child(_U(radiator));

  m_barrel.radiatorMat            = lcdd->material(x_barrel_rad.materialStr());
  m_barrel.radiatorVis            = lcdd->visAttributes(x_barrel_rad.visStr());

  m_endcap.radiatorMat            = lcdd->material(x_endcap_rad.materialStr());
  m_endcap.radiatorVis            = lcdd->visAttributes(x_endcap_rad.visStr());

  m_aluminum                      = lcdd->material("Aluminum");
  m_stainlessSteel                = lcdd->material("stainless_steel");
  m_polystyrene                   = lcdd->material("Polystyrene");

  m_moduleVis                     = lcdd->visAttributes(xml_comp_t(x_det.child(_U(module))).visStr());
  m_scintillatorVis               = lcdd->visAttributes(xml_comp_t(x_det.child(_U(scintillator))).visStr());
  m_chamberVis                    = lcdd->visAttributes(xml_comp_t(x_det.child(_U(chamber))).visStr());
  m_chamberGapVis                 = lcdd->visAttributes(xml_comp_t(x_det.child(_U(chambergap))).visStr());
  m_barrel.gapVis                 = lcdd->visAttributes(xml_comp_t(x_barrel.child(_U(gap))).visStr());
  m_supportTrapVis                = lcdd->visAttributes(xml_comp_t(x_det.child(_U(supporttrap))).visStr());

  m_limits                        = lcdd->limitSet(x_det.limitsStr());


  Hcal_barrel_end_module_type     = lcdd->constant<int>("Hcal_barrel_end_module_type");
  if( Hcal_barrel_end_module_type != 1) {
    throw runtime_error("SHcalSc02: Sorry, but TESLA like end modules in barrel are not available with this driver.");
  }
  Hcal_layer_support_length       = lcdd->constant<double>("Hcal_layer_support_length");
  Hcal_layer_air_gap              = lcdd->constant<double>("Hcal_layer_air_gap");
  Hcal_chamber_thickness          = lcdd->constant<double>("Hcal_chamber_thickness");
  Hcal_middle_stave_gaps          = lcdd->constant<double>("Hcal_middle_stave_gaps");
  Hcal_apply_Birks_law            = lcdd->constant<int>   ("Hcal_apply_Birks_law");
  Hcal_radiator_thickness         = lcdd->constant<double>("Hcal_radiator_thickness");
  Hcal_radiator_material          = lcdd->constant<string>("Hcal_radiator_material");
  Hcal_ring                       = lcdd->constant<int>   ("Hcal_ring");
  Hcal_radial_ring_inner_gap      = lcdd->constant<int>   ("Hcal_radial_ring_inner_gap");
  Hcal_sensitive_model            = lcdd->constant<string>("Hcal_sensitive_model");
  Hcal_back_plate_thickness       = lcdd->constant<double>("Hcal_back_plate_thickness");
  Hcal_stave_gaps                 = lcdd->constant<double>("Hcal_stave_gaps");
  Hcal_modules_gap                = lcdd->constant<double>("Hcal_modules_gap");
  Hcal_nlayers                    = lcdd->constant<int>   ("Hcal_nlayers");
  Hcal_fiber_gap                  = lcdd->constant<double>("Hcal_fiber_gap");
  Ecal_endcap_zmin                = lcdd->constant<double>("Ecal_endcap_zmin");
  Ecal_endcap_outer_radius        = lcdd->constant<double>("Ecal_endcap_outer_radius");
  Hcal_endcap_radiator_thickness  = lcdd->constant<double>("Hcal_endcap_radiator_thickness");
  Hcal_endcap_radiator_material   = lcdd->constant<string>("Hcal_endcap_radiator_material");
  Hcal_endcap_nlayers             = lcdd->constant<int>   ("Hcal_endcap_nlayers");

  Hcal_inner_radius               = lcdd->constant<double>("Ecal_outer_radius") + lcdd->constant<double>("Hcal_Ecal_gap");
  Hcal_endcap_cables_gap          = lcdd->constant<double>("Hcal_endcap_cables_gap");
  Hcal_endcap_ecal_gap            = lcdd->constant<double>("Hcal_endcap_ecal_gap");

  TPC_Ecal_Hcal_barrel_halfZ      = lcdd->constant<double>("TPC_Ecal_Hcal_barrel_halfZ");

  // just two modules per stave
  Hcal_normal_dim_z               = (2 * TPC_Ecal_Hcal_barrel_halfZ - Hcal_modules_gap)/2;
  Hcal_top_end_dim_z              = 1180.0000;
  Hcal_start_z                    = Hcal_normal_dim_z + Hcal_modules_gap/2 + Hcal_endcap_cables_gap;
 
 
  // Hcal_start_z is the Hcal Endcap boundary coming from the IP
  // Test Hcal_start_z against Ecal_endcap_zmax + Hcal_endcap_ecal_gap
  // to avoid overlap problems with Ecal if scaled.
  Ecal_endcap_zmax = lcdd->constant<double>("Ecal_endcap_zmax");
  if( Hcal_start_z < Ecal_endcap_zmax + Hcal_endcap_ecal_gap ) {
    Hcal_start_z = Ecal_endcap_zmax + Hcal_endcap_ecal_gap;
  }
  Hcal_lateral_plate_thickness     = lcdd->constant<double>("Hcal_lateral_structure_thickness");
  Hcal_cells_size                  = lcdd->constant<double>("Hcal_cells_size");
  Hcal_digi_cells_size             = lcdd->constant<double>("Hcal_digitization_tile_size");
  Hcal_endcap_center_box_size      = lcdd->constant<double>("Hcal_endcap_center_box_size");
  Hcal_endcap_sensitive_center_box = lcdd->constant<double>("Hcal_endcap_sensitive_center_box");

  //=======================================================
  // general calculated parameters                       //
  //=======================================================  
  Hcal_total_dim_y    = Hcal_nlayers * (Hcal_radiator_thickness + Hcal_chamber_thickness) + Hcal_back_plate_thickness;
  Hcal_endcap_total_z = Hcal_endcap_nlayers * (Hcal_endcap_radiator_thickness + Hcal_chamber_thickness) + Hcal_back_plate_thickness;
  Hcal_module_radius  = Hcal_inner_radius + Hcal_total_dim_y;
  Hcal_y_dim2_for_x   = (Hcal_module_radius - Hcal_module_radius*cos(M_PI/8));
  Hcal_y_dim1_for_x   = Hcal_total_dim_y - Hcal_y_dim2_for_x;
  Hcal_bottom_dim_x   = 2.*Hcal_inner_radius*std::tan(M_PI/8.)- Hcal_stave_gaps;
  Hcal_midle_dim_x    = Hcal_bottom_dim_x + 2* Hcal_y_dim1_for_x*std::tan(M_PI/8.);
  Hcal_top_dim_x      = Hcal_midle_dim_x - 2 * Hcal_y_dim2_for_x/std::tan(M_PI/8.);  
  Hcal_endcap_rmax    = Hcal_inner_radius + Hcal_y_dim1_for_x;

  Hcal_regular_chamber_dim_z  = Hcal_normal_dim_z - 2 *Hcal_lateral_plate_thickness;
  Hcal_cell_dim_x             = Hcal_cells_size;
  Hcal_cell_dim_z             = Hcal_regular_chamber_dim_z / floor(Hcal_regular_chamber_dim_z/Hcal_cell_dim_x);
  Hcal_digi_cell_dim_x        = Hcal_digi_cells_size;
  Hcal_digi_cell_dim_z        = Hcal_regular_chamber_dim_z / floor(Hcal_regular_chamber_dim_z/Hcal_digi_cell_dim_x);
  Hcal_scintillator_thickness = Hcal_chamber_thickness - Hcal_fiber_gap;

  if (Hcal_sensitive_model != "scintillator")    {	
    throw runtime_error("SHcalSc02: Invalid sensitive model for the chosen HCAL superdriver!");
  }

  // Hosting volume
  Assembly assembly = constructDetector();
  assembly.setVisAttributes(lcdd->visAttributes(x_det.visStr()));
  PlacedVolume pv = lcdd->pickMotherVolume(self).placeVolume(assembly);
  self.setPlacement(pv);
  return self;
}

/// Calculate size of the fractional tile
void SHcalSc02::calculateFractTileSize(double x_length, 
				       double x_integerTileSize, 
				       double &x_fractionalTileSize)
{
  int noOfIntCells = 0; //number of integer cells;
  double temp = x_length/x_integerTileSize;

  //check if x_length (scintillator length) is divisible with x_integerTileSize
  double fracPart, intPart;
  fracPart = modf(temp, &intPart);

  if (fracPart == 0){ //divisible
    noOfIntCells = int(temp);
    x_fractionalTileSize= 0.;
  }
  else if (fracPart>0){
    noOfIntCells = int(temp) -1;
    x_fractionalTileSize = (x_length - noOfIntCells * x_integerTileSize)/2.;
  }
}

/// Calculate x-length of an HCAL barrel layer
void SHcalSc02::calculateXLayer(int layer_id, int &logical_layer_id,
				double &xOffset, double &x_halfLength, double &xShift)
{
  double TanPiDiv8 = std::tan(M_PI/8.);
  double x_total   = 0.;
  double x_length  = 0.;

  if ( (layer_id < Hcal_nlayers) || (layer_id > Hcal_nlayers && layer_id < (2*Hcal_nlayers)) )
    logical_layer_id = layer_id % Hcal_nlayers;
  else if ( (layer_id == Hcal_nlayers) || (layer_id == 2*Hcal_nlayers) ) 
    logical_layer_id = Hcal_nlayers;

  //---- bottom barrel------------------------------------------------------------
  if(logical_layer_id *(Hcal_radiator_thickness + Hcal_chamber_thickness) < Hcal_y_dim1_for_x )  {
    xOffset = (logical_layer_id * Hcal_radiator_thickness 
	       + (logical_layer_id -1) * Hcal_chamber_thickness) * TanPiDiv8;

    x_total  = Hcal_bottom_dim_x/2 - Hcal_middle_stave_gaps/2 + xOffset;
    x_length = x_total - 2*Hcal_layer_support_length - 2*Hcal_layer_air_gap;
    x_halfLength = x_length/2.;

  }
  else {//----- top barrel -------------------------------------------------
    xOffset = (logical_layer_id * Hcal_radiator_thickness +
	       (logical_layer_id - 1) * Hcal_chamber_thickness - Hcal_y_dim1_for_x) / TanPiDiv8
      + Hcal_chamber_thickness / TanPiDiv8;
    x_total  = Hcal_midle_dim_x/2. - Hcal_middle_stave_gaps/2. - xOffset;
    x_length = x_total - 2.*Hcal_layer_support_length  - 2.* Hcal_layer_air_gap;
    x_halfLength = x_length/2.;
  }
  double xAbsShift = (Hcal_middle_stave_gaps/2 + Hcal_layer_support_length + Hcal_layer_air_gap + x_halfLength);

  if (layer_id <= Hcal_nlayers)     xShift = - xAbsShift;
  else if (layer_id > Hcal_nlayers) xShift = xAbsShift;
}

/// Construct the detector structure and geometry
Assembly SHcalSc02::constructDetector() {
  Assembly  assembly(name+"_assembly");
  /*--------- BarrelHcal Sensitive detector -----*/
  /* The cell boundaries does not exist as  volumes. So,
   * to avoid long steps over running  several cells, the 
   * theMaxStepAllowed inside the sensitive material is the
   * minimum between x- and z-dimension of the cell
   */
  theMaxStepAllowed = std::min(Hcal_cell_dim_x, Hcal_cell_dim_z);

  //=====================================================
  //  HCAL barrel regular modules                      //
  //=====================================================
#if 0
  m_barrel.sensDet = new SDHcalBarrel(Hcal_cell_dim_x, Hcal_cell_dim_z, Hcal_scintillator_thickness, HCALBARREL,"HcalBarrelReg",
				    (Hcal_middle_stave_gaps/2 + Hcal_layer_support_length + Hcal_layer_air_gap),
				    Hcal_apply_Birks_law);
  RegisterSensitiveDetector(m_barrel.sensDet);
#endif
  constructBarrel(assembly);
  
  //====================================================
  // HCAL endcap modules                              //
  //====================================================
#if 0
  theENDCAPEndSD = new SDHcalEndCapTesla(Hcal_cell_dim_x,
					 Hcal_scintillator_thickness,//inverse, due to definition in SD (which is made for barrel, ie. layers perpendicular on z)
					 Hcal_cell_dim_x,//really Hcal_cell_dim_x !!! cell should be a square in the endcaps
					 HCALENDCAPMINUS,
					 "HcalEndCaps",
					 Hcal_apply_Birks_law
					 );
  RegisterSensitiveDetector(theENDCAPEndSD);
#endif
  constructEndcaps(assembly);
  
  //====================================================
  // HCAL endcap rings                                //
  //====================================================
  if(Hcal_ring > 0 )    {
#if 0
    theENDCAPRingSD = new SDHcalEndCap(Hcal_cell_dim_x,
				       Hcal_cell_dim_x,//really Hcal_cell_dim_x !!! cell should be a square in the endcaps
				       Hcal_scintillator_thickness,
				       HCALENDCAPMINUS,
				       "HcalEndCapRings",
				       Hcal_stave_gaps, 
				       Hcal_endcap_sensitive_center_box,
				       Hcal_apply_Birks_law
				       );
    RegisterSensitiveDetector(theENDCAPRingSD);
#endif
    //draw the HCAL endcap rings
    constructEndcapRings(assembly);         
  }
  return assembly;
}

/// Construct the barrel modules
void SHcalSc02::constructBarrel(Assembly assembly) {
  double BHX  = Hcal_bottom_dim_x /2.;
  double MHX  = Hcal_midle_dim_x / 2.;
  double THX  = Hcal_top_dim_x / 2.;
  double YX1H = Hcal_y_dim1_for_x / 2.;
  double YX2H = Hcal_y_dim2_for_x / 2.;
  double DHZ  = Hcal_normal_dim_z / 2.;
  double chambers_y_off_correction = YX2H;

  // Attention: on bâtit le module dans la verticale à cause du Trd et on le tourne avant de le positioner
  Trapezoid trdBottom(BHX, MHX, DHZ, DHZ, YX1H);
  Trapezoid trdTop   (MHX, THX, DHZ, DHZ, YX2H);
  UnionSolid modSolid (trdBottom,trdTop,Position(0,0,YX1H + YX2H));
  Volume     modVolume(name+"_barrel",modSolid,m_barrel.radiatorMat);
  modVolume.setVisAttributes(m_moduleVis);

  // Chambers in the HCAL BARREL
  //
  // build a box filled with air in the middle of the HCAL barrel
  // to simulate the gap between the two real halves of a module
  //
  Box    gapBox(Hcal_middle_stave_gaps/2,Hcal_normal_dim_z/2,Hcal_total_dim_y/2);
  Volume gapVol(name+"_barrel_gap",gapBox,m_stainlessSteel);
  gapVol.setVisAttributes(m_barrel.gapVis);
  modVolume.placeVolume(gapVol,Position(0,0,Hcal_y_dim2_for_x/2)).addPhysVolID("barrel",HCALBARREL);

  constructBarrelChambers(modVolume,chambers_y_off_correction);

  // BarrelStandardModule placements
  double Y = Hcal_inner_radius + YX1H;
  //-------- start loop over HyCAL BARREL staves ----------------------------
  for (int stave_id = 1; stave_id <= 8; stave_id++)    {
    double module_z_offset = - (Hcal_normal_dim_z + Hcal_modules_gap)/2.;
    for (int module_id = 1; module_id <=2; module_id++) {
      double phi = (stave_id-1) * M_PI/4;
      Position pos(0,-Y,module_z_offset);
      Rotation rot(M_PI/2,phi,0);
      PlacedVolume pv = assembly.placeVolume(modVolume,pos.rotateZ(phi),rot);
      pv.addPhysVolID("stave",HCALBARREL*100 + stave_id*10 + module_id);
      //for (int j = 1; j >= 0; j--) m_barrel.sensDet->SetStaveRotationMatrix(2*stave_id - j, phi);
      //m_barrel.sensDet->SetModuleZOffset(module_id,module_z_offset);
      module_z_offset = - module_z_offset;
    }
  }
}

void SHcalSc02::constructBarrelChambers(Volume modVol,double chambers_y_off_correction) {
  PlacedVolume pv;
  const double tan8   = std::tan(M_PI/8.);
  double x_length     = 0.;
  double y_height     = Hcal_chamber_thickness/2;
  double z_width_trap = Hcal_regular_chamber_dim_z/2;
  double z_width_gap  = Hcal_normal_dim_z/2 - Hcal_lateral_plate_thickness;
  double z_width_supp = Hcal_normal_dim_z/2 - Hcal_lateral_plate_thickness;

  double xOffset = 0.;//the x_length of a barrel layer is calculated as a
  //barrel x-dimension plus (bottom barrel) or minus
  //(top barrel) an x-offset, which depends on the angle M_PI/8

  double xShift = 0.;//Geant4 draws everything in the barrel related to the 
  //center of the bottom barrel, so we need to shift the layers to
  //the left (or to the right) with the quantity xShift

  // the scintillator width is the chamber width - fiber gap 
  double scintHalfWidth = Hcal_scintillator_thickness/2;
  // fiber gap can't be larger than total chamber
  if (scintHalfWidth <= 0.)    {
    throw runtime_error("SHcalSc02::BuildLefBarrelChambers() - scintHalfWidth invalid!");
  }

  // --- build the air gap between chambers and layer support structure
  Box    gapBox(Hcal_layer_air_gap/2.,z_width_gap,y_height);
  Volume gapVol(name+"_gap_vol",gapBox,m_stainlessSteel);
  gapVol.setVisAttributes(m_chamberGapVis);

  // --- build the layer supports
  Trap supportTrap1(2*z_width_supp,2*y_height,
		    Hcal_layer_support_length + 2*y_height*tan8,
		    Hcal_layer_support_length);
  Volume supportTrapVol1(name+"_trap",supportTrap1,m_aluminum);
  supportTrapVol1.setVisAttributes(m_supportTrapVis);

  Trap supportTrap2(2*z_width_supp,2*y_height,
		    Hcal_layer_support_length + 2*y_height/tan8,
		    Hcal_layer_support_length);
  Volume supportTrapVol2(name+"_trap",supportTrap1,m_aluminum);
  supportTrapVol2.setVisAttributes(m_supportTrapVis);

  Box    supportBox(Hcal_layer_support_length/2, z_width_supp, y_height);
  Volume supportBoxVol(name+"_support",supportBox,m_aluminum);
  supportBoxVol.setVisAttributes(m_supportTrapVis);

  for(int id = 1, logical_id = 0; id <= (2*Hcal_nlayers); id++)    {
    Volume supportTrapVol;
    const double multiply = (id <= Hcal_nlayers) ? -1 : 1;
    string l_nam = name+_toString(id,"_layer%d");
    calculateXLayer(id, logical_id, xOffset, x_length, xShift);

    // --- build chamber box, with the calculated dimensions
    // -------------------------------------------------------------------------
    Box    chamberBox(x_length,z_width_trap,y_height);
    // fg: introduce (empty) fiber gap - should be filled with fibres and cables - so far we fill it  with air ...
    Volume chamberVol(l_nam,chamberBox,lcdd->air());
    chamberVol.setVisAttributes(m_chamberVis);

    Box    scintBox(x_length, z_width_trap, scintHalfWidth);
    Volume scintVol(l_nam+"_scintillator",scintBox,m_polystyrene);
    scintVol.setVisAttributes(m_scintillatorVis);
    scintVol.setLimitSet(m_limits);
    scintVol.setSensitiveDetector(m_barrel.sensDet);

    chamberVol.placeVolume(scintVol,Position(0,0,-Hcal_fiber_gap/2)).addPhysVolID("layer",id);

#if 0
    double fract_cell_dim_x = 0.;
    this->calculateFractTileSize(2*x_length, Hcal_cell_dim_x, fract_cell_dim_x);      
    ThreeVector newFractCellDim(fract_cell_dim_x, Hcal_chamber_thickness, Hcal_cell_dim_z);
    m_barrel.sensDet->SetFractCellDimPerLayer(id, newFractCellDim);
    // module x and y offsets (needed for the SD)
    double Xoff,Yoff;
    Xoff = 0.;
    Yoff = Hcal_inner_radius + Hcal_total_dim_y/2.;      
    m_barrel.sensDet->AddLayer(id,
			     //chamber_x_offset + 
			     Xoff - 
			     2*chamberBox->GetDx(),
			     chamber_y_offset + Yoff,
			     chamber_z_offset - chamberBox->GetDy());
#endif
      
    // --- Place Chamber
    Position pos(xShift,0,0);
    pos.z = -Hcal_total_dim_y/2. 
      + (logical_id-1) * (Hcal_chamber_thickness + Hcal_radiator_thickness)
      + Hcal_radiator_thickness + Hcal_chamber_thickness/2
      + chambers_y_off_correction;

    modVol.placeVolume(chamberVol,pos).addPhysVolID("layer",id);

    // ---   Place gap
    double gap_leftEdge  = (Hcal_middle_stave_gaps/2. + Hcal_layer_support_length + Hcal_layer_air_gap/2.);
    double gap_rightEdge = (gap_leftEdge + 2.*x_length + Hcal_layer_air_gap);
    // right side, right edge
    pos.x = multiply*gap_rightEdge;
    modVol.placeVolume(gapVol,pos).addPhysVolID("layer",id);    
    // right side, left edge
    pos.x = multiply*gap_leftEdge;
    modVol.placeVolume(gapVol,pos).addPhysVolID("layer",id);

    // --- Place layer supports
    Rotation rotation;
    bool   isOutsideCorner = false;
    double temp = Hcal_middle_stave_gaps/2 + 3*Hcal_layer_support_length/2 + 2*x_length + 2*Hcal_layer_air_gap;
    //---- bottom barrel --------------------------------------------------------------
    if (logical_id *(Hcal_radiator_thickness + Hcal_chamber_thickness) < Hcal_y_dim1_for_x ){
      supportTrapVol = supportTrapVol1;
      // bottom barrel, right side (id > Hcal_nlayers) and bottom barrel, left side
      rotation = Rotation(M_PI/2, 0, ((id > Hcal_nlayers) ? 3*M_PI/2 : M_PI/2));
      pos.x = multiply * (temp + y_height*tan8/2);
    }
    else {//------- top barrel --------------------------------------------------
      double xAbsOutsideCorner = (temp + Hcal_layer_support_length/2 + 2*y_height/tan8);
      supportTrapVol = supportTrapVol2;
      pos.x = multiply * (temp + y_height/tan8/2);
      // check if the corner of the right angular wedge is outside the volume
      isOutsideCorner = (xAbsOutsideCorner > Hcal_midle_dim_x/2);
      // top barrel, right side (id > Hcal_nlayers) vs. top barrel, left side
      rotation = Rotation(M_PI/2, 0, ((id > Hcal_nlayers) ? 3*M_PI/2 : M_PI/2));
    }
    if ( !isOutsideCorner )   {
      modVol.placeVolume(supportTrapVol,pos,rotation).addPhysVolID("layer",id);
    }
    if ( isOutsideCorner )  {
      // ---  draw a support box instead of the right angular wedge, if the
      //      corner of the wedge is outside the mother volume.
      cout << "Placing support box...." << endl;
      pos.x = multiply * temp;
      modVol.placeVolume(supportBoxVol,pos).addPhysVolID("layer",id);
    }
    //--- draw support boxes in the middle -------------------------------------------
    pos.x = multiply * (Hcal_middle_stave_gaps/2 + Hcal_layer_support_length/2);
    modVol.placeVolume(supportBoxVol,pos).addPhysVolID("layer",id);

    //break; // Debug: only one layer...
  }  //end loop over HCAL nlayers;
}

/// Construct the endcap modules
void SHcalSc02::constructEndcaps(Assembly assembly) {
  double tan8 = std::tan(M_PI/8.);
  double tan4 = std::tan(M_PI/4.);
  // First: dimensions of the trapezoid half length of a hexagon side
  double half_length  = Hcal_endcap_rmax * tan8;
  double trap_small_x = half_length + Hcal_endcap_center_box_size/2. - Hcal_lateral_plate_thickness * tan8;
  double trap_x       = Hcal_endcap_rmax + Hcal_endcap_center_box_size/2 - Hcal_lateral_plate_thickness;
  double trap_y       = Hcal_endcap_total_z;
  double trap_z       = (Hcal_endcap_rmax + Hcal_endcap_center_box_size/2 - trap_small_x - Hcal_lateral_plate_thickness)/tan4;
  Trap   ecTop(trap_y,trap_z,trap_x,trap_small_x);

  // Second: dimensions of the box (box expects half side length as an input)
  double box_half_x = trap_x/2.;
  double box_half_y = trap_y/2;
  double box_half_z = (Hcal_endcap_rmax - trap_z - Hcal_endcap_center_box_size/2 - Hcal_stave_gaps - Hcal_lateral_plate_thickness)/2.;
  Box    ecBottom(box_half_x,box_half_z,box_half_y);

  // --- x-dimension of the trapezoid center of gravity
  double trap_center_of_grav_half_x = (trap_small_x + trap_z/2 * tan4)/2;

  //shift the top trapezoidal part with respect to the bottom part to get the union
  double shift_x = -std::abs(box_half_x - trap_center_of_grav_half_x);
  double shift_y = box_half_z + trap_z/2.;
  UnionSolid ecStave(ecBottom,ecTop,Position(shift_x,shift_y,0));

  //Create the endcap logical volume
  Volume ecStaveVol(name+"_endcap_stave",ecStave,m_endcap.radiatorMat);
  ecStaveVol.setVisAttributes(m_moduleVis);
#if 0
  // --- Build the chambers 
  // --- Build first the scintillators (polystyrene)
  Box  bottomScintSolid(box_half_x,box_half_z,Hcal_scintillator_thickness/2);  
  Trap topScintSolid(Hcal_scintillator_thickness,trap_z,trap_x,trap_small_x);

  // --- shift the top trapezoidal part with respect to the bottom part to get the union
  UnionSolid scintStave(bottomScintSolid,topScintSolid,Position(shift_x,shift_y,0));
  Volume     scintVol  (name+"_endcap_scintillator",scintStave,m_polystyrene);
  scintVol.setVisAttributes(m_scintillatorVis);
  scintVol.setLimitSet(m_limits);

  // --- Build the air gap (for cables)                   //
  Box  bottomFiberGap(box_half_x,box_half_z,Hcal_fiber_gap/2);
  Trap topFiberGap(Hcal_fiber_gap,trap_z,trap_x,trap_small_x);
  UnionSolid  gapStave(bottomFiberGap,topFiberGap,Position(shift_x,shift_y,0));
  Volume      gapVol(  name+"_endcap_gap",gapStave,lcdd->air());
  gapVol.setVisAttributes(m_chamberGapVis);

  // Place these Hcal_endcap_nlayers times...
  double offset_x = 0;
  double offset_y = 0;
  double offset_z = 0;

  //------ start loop over HCAL layers ----------------------
  Position pos, offset;
  for(int id = 1; id <= Hcal_endcap_nlayers; id++) {
    // --- Place the scintillator
    pos.z = - Hcal_endcap_total_z/2
      + (id-1) *(Hcal_chamber_thickness + Hcal_endcap_radiator_thickness)
      + Hcal_endcap_radiator_thickness
      + Hcal_scintillator_thickness/2.;
    //ecStaveVol.placeVolume(scintVol,pos).addPhysVolID("layer",id);
#if 0
    //center of coordinate in the middle of the box
    offset.x = - box_half_x;
    offset.y = - box_half_z;
    offset.z = Hcal_scintillator_thickness/2.;//????
    theSD->AddLayer(id, offset.x, offset.y, offset.z);     
    //-------------------------------------------------------------
    // Very important: DON'T FORGET to set the sensitive detector
    // only scintillator is sensitive
    EndcapScintillatorLogical->SetSensitiveDetector( theENDCAPEndSD );
#endif
    // ---  Place the fiber gap
    pos.z = - Hcal_endcap_total_z/2
      + id * (Hcal_endcap_radiator_thickness + Hcal_scintillator_thickness)
      + (id-1) * Hcal_fiber_gap + Hcal_fiber_gap/2.;
    //ecStaveVol.placeVolume(gapVol,pos).addPhysVolID("layer",id);
  }
#endif
  //==================================================
  //Place....
  double endcap_z_offset = Hcal_start_z + Hcal_endcap_total_z/2. ;
  double a=0, b=0, c=0, d=0, e=0; //helper variables
  //------------------------
  //endcapId=1: staveId=1
  //            x_shift = box_half_x - Hcal_endcap_center_box_size/2;
  //            y_shift = box_half_z + Hcal_endcap_center_box_size/2 + Hcal_stave_gaps;
  //endcapId=2  x_shift = - x_shift
  //            y_shift = y_shift
  //
  //endcapId=1: staveId=2
  //            x_shift = box_half_z + Hcal_endcap_center_box_size/2 + Hcal_stave_gaps;
  //            y_shift = - (box_half_x - Hcal_endcap_center_box_size/2);
  //endcapId=2  x_shift = - x_shift
  //            y_shift = y_shift
  //
  //endcapId=1: staveId=3
  //            x_shift = - (box_half_x - Hcal_endcap_center_box_size/2)
  //            y_shift = - (Hcal_endcap_center_box_size/2 + box_half_z + Hcal_stave_gaps);
  //endcapId=2  x_shift = - x_shift
  //            y_shift = y_shift
  //
  //endcapId=1: staveId=4
  //            x_shift = - (box_half_z + Hcal_endcap_center_box_size/2 + Hcal_stave_gaps)
  //            y_shift = box_half_x - Hcal_endcap_center_box_size/2 
  //endcapId=2  x_shift = - x_shift
  //            y_shift = y_shift
  //
  //--------- loop over endcap id -----------------------
  for(int ec_id=1; ec_id <= 2; ++ec_id)    {
    //--------- loop over endcap stave id ---------------
    for(int stave_id=2; stave_id < 4; ++stave_id)    {
      Position pos(0,0,endcap_z_offset);
      Rotation rot(0,0,0);//,0);
      int mod_id = (ec_id==1 ? HCALENDCAPPLUS : HCALENDCAPMINUS)*10 + stave_id;
      rot.rotateZ((stave_id-1)*M_PI/2);//,(ec_id-1)*M_PI);

      a = (stave_id+2)%2 * std::pow(-1.,((stave_id+2)%4)%3 );
      b = (stave_id+1)%2 * std::pow(-1.,((stave_id+1)%4)%3 );
      c = std::pow(-1.,(stave_id%3)%2);
      pos.x = a * box_half_x + b * box_half_z + c * Hcal_endcap_center_box_size/2. + b * Hcal_stave_gaps;
      if (ec_id == 2) pos.x = -pos.x;

      d = (stave_id+3)%2 * std::pow(-1., ((stave_id+3)%4)%3 );
      e = std::pow(-1.,int((stave_id-1)/2.) );
      pos.y = d * box_half_x + a * box_half_z + e * Hcal_endcap_center_box_size/2 + a * Hcal_stave_gaps; 
      assembly.placeVolume(ecStaveVol,pos,rot).addPhysVolID("endcap",mod_id);
    }
    //--------- end loop over endcap stave id -----------
    endcap_z_offset = - endcap_z_offset;
#if 0
    theENDCAPEndSD->SetStaveRotationMatrix(stave_id,rot.phi);
    if (ec_id == 1)      theENDCAPEndSD->SetModuleZOffset(HCALENDCAPPLUS, fabs(Z1));
    else if (ec_id == 2) theENDCAPEndSD->SetModuleZOffset(HCALENDCAPMINUS, fabs(Z1));
#endif
  }
}

/// Construct the endcap rings
void SHcalSc02::constructEndcapRings(Assembly assembly) {
}

#if 0
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~              Barrel Regular Modules               ~
void SHcalSc02::constructEndcapChambers(LogicalVolume* modVol, SDHcalEndCap* theSD, bool rings)  {
  // Chambers in the SHcalSc02::Endcaps
  // standard endcap chamber solid:
  Polyhedra *motherSolid = (Polyhedra*) modVol->GetSolid();

  PolyhedraHistorical* motherPolyhedraParameters = motherSolid->GetOriginalParameters();

  double pRMax, pDz, fiber_gap, pRMin;

  pRMax = (*(motherPolyhedraParameters->Rmax) * cos(M_PI/motherPolyhedraParameters->numSide))
    - (Hcal_lateral_plate_thickness);

  pDz = Hcal_chamber_thickness / 2.;

  pRMin = ( *(motherPolyhedraParameters->Rmin) * cos(M_PI/motherPolyhedraParameters->numSide))
    + (Hcal_lateral_plate_thickness);

  fiber_gap = Hcal_fiber_gap;

  // Polyhedra Envelope parameters
  double phiStart = 0.;
  double phiTotal = 360.;
  int numSide     = motherPolyhedraParameters->numSide;
  int numZPlanes  = 2;

  double zPlane[2];
  zPlane[0] = - pDz;
  zPlane[1] = - zPlane[0];

  double rInner[2],rOuter[2];
  rInner[0] = rInner[1] = pRMin;
  rOuter[0] = rOuter[1] = pRMax;


  Polyhedra *EndCapChamberSolid = new Polyhedra("EndCapChamberSolid",
						phiStart,
						phiTotal,
						numSide,
						numZPlanes,
						zPlane,
						rInner,
						rOuter);
  Box *IntersectionStaveBox = new Box("IntersectionStaveBox",
				      pRMax,
				      pRMax,
				      Hcal_total_dim_y);

  // set up the translation and rotation for the intersection process 
  // this happens in the mother volume coordinate system, so a coordinate transformation is needed
  ThreeVector IntersectXYZtrans((pRMax + (Hcal_stave_gaps/2.))*(cos(M_PI/numSide) - sin(M_PI/numSide)),
				(pRMax + (Hcal_stave_gaps/2.))*(cos(M_PI/numSide) + sin(M_PI/numSide)),
				(Hcal_total_dim_y/2.));
  Rotation rot;
  rot->rotateZ(-(M_PI/motherPolyhedraParameters->numSide));
  // intersect the octagonal layer with a square to get only one quadrant
  IntersectionSolid  *EndCapStaveSolid = new IntersectionSolid( "EndCapStaveSolid",
								EndCapChamberSolid,
								IntersectionStaveBox,
								rot, 
								IntersectXYZtrans); 

  UserLimits* pULimits = new UserLimits(theMaxStepAllowed);

  // standard endcap chamber logical
  LogicalVolume* EndCapStaveLogical = 0;

  if(Hcal_sensitive_model == "scintillator")
    {
      //fg: introduce (empty) fiber gap - should be filled with fibres and cables
      // - so far we fill it  with air ...
      EndCapStaveLogical = new LogicalVolume(EndCapStaveSolid,
					     lcdd->material("air"), 
					     "EndCapChamberLogical", 
					     0, 0, 0);

      double scintHalfWidth = pDz - fiber_gap  / 2. ;

      // fiber gap can't be larger than total chamber
      assert( scintHalfWidth > 0. ) ;

      double zPlaneScint[2];
      zPlaneScint[0] = - scintHalfWidth ;
      zPlaneScint[1] = - zPlaneScint[0];

      Polyhedra *EndCapScintSolid = new Polyhedra("EndCapScintSolid",
						  phiStart,
						  phiTotal,
						  numSide,
						  numZPlanes,
						  zPlaneScint,
						  rInner,
						  rOuter);
      IntersectionSolid  *EndCapScintStaveSolid = new IntersectionSolid( "EndcapScintStaveSolid",
									 EndCapScintSolid,
									 IntersectionStaveBox,
									 rot, 
									 IntersectXYZtrans);

      LogicalVolume* ScintLog = new LogicalVolume(EndCapScintStaveSolid,
						  lcdd->material("polystyrene"),
						  "EndCapScintLogical", 
						  0, 0, pULimits);  
      VisAttributes *VisAtt = new VisAttributes(Colour::Yellow());
      VisAtt->SetForceSolid(true);
      ScintLog->SetVisAttributes(VisAtt);

      // only scintillator is sensitive
      ScintLog->SetSensitiveDetector(theSD);

      new MyPlacement(0, 
		      ThreeVector( 0, 0,  - fiber_gap / 2.), 
		      ScintLog,
		      "EndCapScintillator", 
		      EndCapStaveLogical, 
		      false, 
		      0);   
    }
  else Control::Abort("SHcalSc02: Invalid sensitive model parameter!",MOKKA_ERROR_BAD_GLOBAL_PARAMETERS);

  VisAttributes *VisAtt = new VisAttributes(Colour::Blue());
  EndCapStaveLogical->SetVisAttributes(VisAtt);

  // chamber placements
  int number_of_chambers = Hcal_endcap_nlayers;
  int possible_number_of_chambers = (int) floor(2*abs(*(motherPolyhedraParameters->Z_values)) /
						(Hcal_chamber_thickness + Hcal_endcap_radiator_thickness));
  if(possible_number_of_chambers < number_of_chambers)
    number_of_chambers = possible_number_of_chambers;

  for (int layer_id = 1;
       layer_id <= number_of_chambers;
       layer_id++)
    {
      double Zoff = - abs(*(motherPolyhedraParameters->Z_values))
	+ (layer_id-1) *(Hcal_chamber_thickness + Hcal_endcap_radiator_thickness)
	+ Hcal_endcap_radiator_thickness 
        + (Hcal_chamber_thickness - Hcal_fiber_gap)/2.;

      //place the four staves in their right positions
      for (int stave_id = 1;
	   stave_id <= 4;
	   stave_id++)
	{
	  RotationMatrix *rotEffect = new RotationMatrix();
	  rotEffect->rotateZ(((stave_id-1)*M_PI/2.));
	  new MyPlacement(rotEffect,
			  ThreeVector(0.,0.,Zoff),
			  EndCapStaveLogical,
			  "EndCapStavePhys",
			  modVol,
			  false,
			  layer_id*10 + stave_id);
	}

      theSD->AddLayer(layer_id,
		      0,
		      0,
		      Zoff);

    }  
}

/// EndcapRings
void SHcalSc02::EndcapRings(LogicalVolume* modVol)   {
  // old parameters from database
  double pRMax, pDz, pRMin;
  pRMax = Hcal_endcap_rmax;

  // The rings start from inner Ecal endcap boundary
  // and finish at inner Hcal endcap one.
  double start_z, stop_z;
  start_z = Ecal_endcap_zmin;
  double SpaceForLayers = Hcal_start_z - Hcal_endcap_ecal_gap
    - Ecal_endcap_zmin - Hcal_back_plate_thickness;
  int MaxNumberOfLayers = (int) (SpaceForLayers /
				 (Hcal_chamber_thickness + Hcal_endcap_radiator_thickness));
  stop_z = start_z + MaxNumberOfLayers * (Hcal_chamber_thickness + Hcal_endcap_radiator_thickness)
    + Hcal_back_plate_thickness;
  pDz = (stop_z - start_z) / 2.;
  pRMin = Ecal_endcap_outer_radius    + Hcal_radial_ring_inner_gap;
  double zPlane[2];
  zPlane[0]=-pDz;
  zPlane[1]=-zPlane[0];

  double rInner[2],rOuter[2];
  rInner[0]=rInner[1]=pRMin;
  rOuter[0]=rOuter[1]=pRMax;

  if (rOuter[0] <= rInner[0]) 
    Exception("SHcalSc02::EndcapRings() - not enough place for endcap rings (try a larger Hcal_nlayers number)!");

  Polyhedra *EndCapSolid = new Polyhedra("HcalEndCapRingSolid",
					 0.,
					 360.,
					 //32,
					 8,
					 2,
					 zPlane,
					 rInner,
					 rOuter);

  VisAttributes *VisAtt = new VisAttributes(Colour(.8,.8,.2));
  VisAtt->SetForceWireframe(true);
  VisAtt->SetDaughtersInvisible(true);
  LogicalVolume* EndCapLogical = new LogicalVolume(EndCapSolid,
						   m_endcap.radiatorMat,
						   "EndCapRingLogical",
						   0, 0, 0);
  EndCapLogical->SetVisAttributes(VisAtt);
  //------------------------------------------------------
  // build and place the chambers in the Hcal EndcapRings
  EndcapChambers(EndCapLogical,theENDCAPRingSD, true);
  //------------------------------------------------------

  // Placements
  double endcap_z_offset = Ecal_endcap_zmin + pDz;
  RotationMatrix *rotEffect = new RotationMatrix();
  rotEffect->rotateZ(M_PI/8.);

  int ModuleNumber = HCALENDCAPPLUS*100 + 16;
  double Z1 = 0; 

  for (int endcap_id = 1;
       endcap_id <= 2;
       endcap_id++)
    {
      Z1 = endcap_z_offset;
      new MyPlacement(rotEffect,
		      ThreeVector(0.,
				  0.,
				  Z1),
		      EndCapLogical,
		      "EndCapPhys",
		      modVol,
		      false,
		      ModuleNumber);
      rotEffect = new RotationMatrix();
      rotEffect->rotateZ(-M_PI/8.);
      rotEffect->rotateY(M_PI);  // inverse the endcaps
      ModuleNumber -= (HCALENDCAPPLUS-HCALENDCAPMINUS)*100 + 6;

      endcap_z_offset = - endcap_z_offset;
    }

  theENDCAPRingSD->SetModuleZOffset(0, fabs(Z1));
  theENDCAPRingSD->SetModuleZOffset(6, fabs(Z1));
}

//~           Post construct action                   ~
bool SHcalSc02::PostConstructAction(CGAGeometryEnvironment& )   {
  //
  // Propagates the changes to Coil, if any. The SHcal has also the responsability 
  // to change the calorimeter region parameters.
  //
  double Hcal_R_max = (Hcal_y_dim1_for_x +  Hcal_y_dim2_for_x + Hcal_inner_radius)/cos(M_PI/16);
  std::ostringstream oss1;
  oss1 << Hcal_R_max;
  (*Control::globalModelParameters)["Hcal_R_max"] = oss1.str();
  (*Control::globalModelParameters)["calorimeter_region_rmax"] = oss1.str();

  std::ostringstream oss2;
  oss2 << Hcal_start_z;
  (*Control::globalModelParameters)["Hcal_endcap_zmin"] = oss2.str();
  
  double Hcal_outer_radius = Hcal_inner_radius + Hcal_total_dim_y;

  double calorimeter_region_zmax = Hcal_start_z + Hcal_endcap_total_z;
  std::ostringstream oss3;  
  oss3 << calorimeter_region_zmax;
  (*Control::globalModelParameters)["calorimeter_region_zmax"] = oss3.str();

  return true;    
}
#endif


static Ref_t create_detector(LCDD& lcdd, const xml_h& element)  {
  return (new Value<TNamed,SHcalSc02>())->construct(lcdd,element);
}

DECLARE_SUBDETECTOR(Tesla_SHcalSc02,create_detector);
