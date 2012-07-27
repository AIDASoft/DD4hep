// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_TAGS_H
#define DD4hep_XML_TAGS_H

#include "XML/XMLElements.h"

namespace DD4hep   { namespace XML  {

  extern const Tag_t Attr_id;
  extern const Tag_t Attr_type;
  extern const Tag_t Attr_value;
  extern const Tag_t Attr_formula;
  extern const Tag_t Tag_NULL;
  extern const Tag_t Tag_empty;
  extern const Tag_t Tag_star;
  extern const Tag_t Tag_PI;
  extern const Tag_t Tag_TWOPI;
  extern const Tag_t Tag_radian;
  extern const Tag_t Tag_mm;
  extern const Tag_t Tag_cm;


  extern const Tag_t Attr_overlap;
  extern const Tag_t Attr_check;

  // RefElement parameters
  extern const Tag_t Tag_ref;

  // Polycone parameters

  extern const Tag_t Tag_Air;
  extern const Tag_t Tag_Vacuum;
  extern const Tag_t Tag_D;
  extern const Tag_t Tag_n;
  extern const Tag_t Tag_atom;
  extern const Tag_t Tag_fraction;

  extern const Tag_t Tag_define;
  extern const Tag_t Tag_detector;
  extern const Tag_t Tag_dimensions;
  extern const Tag_t Tag_element;
  extern const Tag_t Tag_id;
  extern const Tag_t Tag_idspecref;

  extern const Tag_t Attr_InvisibleNoDaughters;
  extern const Tag_t Attr_InvisibleWithDaughters;

  extern const Tag_t Attr_Z;

  extern const Tag_t Attr_1;
  extern const Tag_t Attr_barrel;
  extern const Tag_t Attr_beampipe;
  extern const Tag_t Attr_combineHits;
  extern const Tag_t Attr_crossing_angle;
  extern const Tag_t Attr_dimensions;
  extern const Tag_t Attr_ecut;
  extern const Tag_t Attr_false;
  extern const Tag_t Attr_field_name;
  extern const Tag_t Attr_hits_collection;
  extern const Tag_t Attr_incoming_r;
  extern const Tag_t Attr_inner_r;
  extern const Tag_t Attr_inner_z;
  extern const Tag_t Attr_layer;
  extern const Tag_t Attr_limits;
  extern const Tag_t Attr_material;
  extern const Tag_t Attr_n;
  extern const Tag_t Attr_name;
  extern const Tag_t Attr_nphi;
  extern const Tag_t Attr_ntheta;
  
  extern const Tag_t Attr_outer_r;
  extern const Tag_t Attr_outer_z;
  extern const Tag_t Attr_outgoing_r;
  extern const Tag_t Attr_position;
  extern const Tag_t Attr_rbg;
  extern const Tag_t Attr_readout;
  extern const Tag_t Attr_reflect;
  extern const Tag_t Attr_region;
  extern const Tag_t Attr_repeat;
  extern const Tag_t Attr_rotation;
  extern const Tag_t Attr_segmentation;
  extern const Tag_t Attr_sensitive;
  extern const Tag_t Attr_sensitive_detector;
  extern const Tag_t Attr_system;
  extern const Tag_t Attr_thickness;
  extern const Tag_t Attr_true;
  extern const Tag_t Attr_tubs;
  extern const Tag_t Attr_verbose;
  extern const Tag_t Attr_version;
  extern const Tag_t Attr_vis;

  // LCDD
  extern const Tag_t Tag_barrel;
  extern const Tag_t Tag_cartesian_grid_xy;
  extern const Tag_t Tag_global_grid_xy;
  extern const Tag_t Tag_color;
  extern const Tag_t Tag_detectors;
  extern const Tag_t Tag_display;
  extern const Tag_t Tag_fields;
  extern const Tag_t Tag_gdml;
  extern const Tag_t Tag_header;
  extern const Tag_t Tag_iddict;
  extern const Tag_t Tag_lcdd;
  extern const Tag_t Tag_limits;
  extern const Tag_t Tag_materials;
  extern const Tag_t Tag_materials;
  extern const Tag_t Tag_includes;
  extern const Tag_t Tag_gdmlFile;
  extern const Tag_t Tag_readout;
  extern const Tag_t Tag_readouts;
  extern const Tag_t Tag_reflect_rot;
  extern const Tag_t Tag_regions;
  extern const Tag_t Tag_segmentation;
  extern const Tag_t Tag_sensitive_detectors;
  extern const Tag_t Tag_setup;
  extern const Tag_t Tag_structure;
  extern const Tag_t Tag_solids;
  extern const Tag_t Tag_tracker;
  extern const Tag_t Tag_tubs;
  extern const Tag_t Tag_system;
  extern const Tag_t Tag_world;
  extern const Tag_t Tag_world_box;
  extern const Tag_t Tag_world_volume;
  extern const Tag_t Tag_calorimeter;
  extern const Tag_t Tag_cartesian_grid_xy;
  extern const Tag_t Tag_tracking_cylinder;
  extern const Tag_t Tag_tracking_volume;

  extern const Tag_t Attr_insideTrackingVolume;
  extern const Tag_t Attr_signed;
  extern const Tag_t Attr_label;
  extern const Tag_t Attr_start;
  extern const Tag_t Attr_width;
  extern const Tag_t Attr_length;
  extern const Tag_t Attr_z_length;
  extern const Tag_t Attr_line_style;
  extern const Tag_t Attr_drawing_style;
  extern const Tag_t Attr_module;
  extern const Tag_t Attr_combine_hits;

  extern const Tag_t Tag_polyhedra;
  extern const Tag_t Attr_numsides;


  /// Compact XML
  extern const Tag_t Attr_gap;
  extern const Tag_t Attr_phiBins;
  extern const Tag_t Attr_thetaBins;
  extern const Tag_t Attr_threshold;
  extern const Tag_t Tag_component;

  extern const Tag_t Attr_r_size;
  extern const Tag_t Attr_phi_size_max;
  extern const Tag_t Tag_beampipe;

  extern const Tag_t Attr_lineStyle;
#if 0  
  extern const Tag_t 
  extern const Tag_t 
  extern const Tag_t 
  extern const Tag_t 
  extern const Tag_t 
  extern const Tag_t 
  extern const Tag_t 

#endif

  // -- RefElement
  extern const Tag_t Attr_name;
  // -- References to refelements:
  extern const Tag_t Attr_ref;

  //================================ Objects: ================================
  // -- Author
  extern const Tag_t Tag_author;
  extern const Tag_t Attr_name;
  extern const Tag_t Attr_email;
  extern const Tag_t Tag_empty;
  // -- Header
  extern const Tag_t Tag_header;
  extern const Tag_t Tag_author;
  extern const Tag_t Tag_detector;
  extern const Tag_t Tag_comment;
  extern const Tag_t Tag_generator;
  extern const Tag_t Attr_name;
  extern const Tag_t Attr_file;
  extern const Tag_t Attr_version;
  extern const Tag_t Attr_checksum;
  extern const Tag_t Tag_info;
  // -- Constant
  extern const Tag_t Tag_constant;
  extern const Tag_t Attr_value;
  // -- XYZ_element
  extern const Tag_t Attr_x;
  extern const Tag_t Attr_y;
  extern const Tag_t Attr_z;
  extern const Tag_t Attr_unit;
  // -- Position
  extern const Tag_t Tag_position;
  // -- Rotation
  extern const Tag_t Tag_rotation;
  // -- Atom
  extern const Tag_t Tag_element;
  // -- Material
  extern const Tag_t Tag_material;
  // -- VisAttr (compact)
  extern const Tag_t Tag_vis;
  extern const Tag_t Attr_showDaughters;
  extern const Tag_t Attr_visible;
  extern const Tag_t Attr_drawingStyle;
  extern const Tag_t Attr_alpha;
  extern const Tag_t Attr_r;
  extern const Tag_t Attr_b;
  extern const Tag_t Attr_g;
  // -- VisAttr (LCDD)
  extern const Tag_t Tag_vis;
  extern const Tag_t Tag_color;
  extern const Tag_t Attr_show_daughters;
  extern const Tag_t Attr_visible;
  extern const Tag_t Attr_line_style;
  extern const Tag_t Attr_drawing_style;
  extern const Tag_t Attr_alpha;
  extern const Tag_t Attr_R;
  extern const Tag_t Attr_B;
  extern const Tag_t Attr_G;
  extern const Tag_t Tag_limit;
  // -- Limit
  extern const Tag_t Attr_value;
  extern const Tag_t Attr_unit;
  extern const Tag_t Attr_particles;
  extern const Tag_t Tag_mm;
  extern const Tag_t Tag_star;
  // -- Limitset
  extern const Tag_t Tag_limitset;
  // -- Region
  extern const Tag_t Tag_region;
  extern const Tag_t Attr_store_secondaries;
  extern const Tag_t Attr_threshold;
  extern const Tag_t Attr_lunit;
  extern const Tag_t Attr_eunit;
  extern const Tag_t Attr_cut;
  extern const Tag_t Tag_mm;
  extern const Tag_t Tag_MeV;
  // -- IDSpec
  extern const Tag_t Tag_idspec;
  extern const Tag_t Attr_length;
  // -- IDSpec.idfield
  extern const Tag_t Tag_idfield;
  extern const Tag_t Attr_signed;
  extern const Tag_t Attr_label;
  extern const Tag_t Attr_start;
  extern const Tag_t Attr_length;
  // -- Alignment
  extern const Tag_t Tag_alignments;
  extern const Tag_t Tag_alignment;

  //================================ Objects: ================================
  // -- Rotation
  extern const Tag_t Attr_theta;
  extern const Tag_t Attr_phi;
  extern const Tag_t Attr_psi;
  //================================ Volumes: ================================
  // -- Volume
  extern const Tag_t Tag_volume;
  extern const Tag_t Tag_solidref;
  extern const Tag_t Tag_materialref;
  extern const Tag_t Tag_regionref;
  extern const Tag_t Tag_limitsetref;
  extern const Tag_t Tag_sdref;
  extern const Tag_t Tag_visref;
  // -- PhysVol
  extern const Tag_t Tag_physvol;
  extern const Tag_t Tag_volumeref;
  extern const Tag_t Tag_positionref;
  extern const Tag_t Tag_rotationref;
  extern const Tag_t Tag_identity_pos;
  extern const Tag_t Tag_identity_rot;
  // -- PhysVol.PhysVolID
  extern const Tag_t Tag_physvolid;
  extern const Tag_t Attr_field_name;
  extern const Tag_t Attr_value;

  //================================ Shapes:  ================================
  // Solid:
  // No tags
  extern const Tag_t Attr_zmin;

  // -- ZPlane
  extern const Tag_t Tag_zplane;
  extern const Tag_t Attr_rmin;
  extern const Tag_t Attr_rmax;
  extern const Tag_t Attr_z;
  // -- Box
  extern const Tag_t Tag_box;
  extern const Tag_t Attr_x;
  extern const Tag_t Attr_y;
  extern const Tag_t Attr_z;
  // -- Polycone
  extern const Tag_t Tag_polycone;
  extern const Tag_t Attr_startphi;
  extern const Tag_t Attr_deltaphi;
  // -- Tube
  extern const Tag_t Tag_tube;
  extern const Tag_t Attr_deltaphi;
  extern const Tag_t Attr_rmin;
  extern const Tag_t Attr_rmax;
  extern const Tag_t Attr_zhalf;
  // -- Cone
  extern const Tag_t Tag_cone;
  extern const Tag_t Attr_startphi;
  extern const Tag_t Attr_deltaphi;
  extern const Tag_t Attr_rmin1;
  extern const Tag_t Attr_rmin2;
  extern const Tag_t Attr_rmax1;
  extern const Tag_t Attr_rmax2;
  extern const Tag_t Attr_z;
  // -- Trapezoid
  extern const Tag_t Tag_trd;
  extern const Tag_t Attr_X1;
  extern const Tag_t Attr_X2;
  extern const Tag_t Attr_Y1;
  extern const Tag_t Attr_Y2;
  extern const Tag_t Attr_Z;
  // Trap
  extern const Tag_t Tag_trap;
  extern const Tag_t Attr_aunit;
  extern const Tag_t Attr_lunit;
  extern const Tag_t Attr_x1;
  extern const Tag_t Attr_x2;
  extern const Tag_t Attr_x3;
  extern const Tag_t Attr_x4;
  extern const Tag_t Attr_y1;
  extern const Tag_t Attr_y2;
  extern const Tag_t Attr_z;
  extern const Tag_t Attr_alpha1;
  extern const Tag_t Attr_alpha2;
  // -- PolyhedraRegular
  extern const Tag_t Tag_polyhedra;
  extern const Tag_t Attr_startphi;
  extern const Tag_t Attr_deltaphi;
  extern const Tag_t Attr_numsides;
  // -- BooleanSolid
  extern const Tag_t Tag_first;
  extern const Tag_t Tag_second;
  extern const Tag_t Tag_positionref;
  extern const Tag_t Tag_rotationref;
  // -- SubtractionSolid
  extern const Tag_t Tag_subtraction;

  //================================ Detectors: ================================
  // -- Calorimeter
  extern const Tag_t Tag_Air;
  extern const Tag_t Tag_layer;
  extern const Tag_t Tag_slice;
  extern const Tag_t Tag_slices;
  extern const Tag_t Tag_reflect;
  extern const Tag_t Tag_reflect_rot;
  extern const Tag_t Tag_stave;
  extern const Tag_t Tag_staves;
  extern const Tag_t Tag_system;
  extern const Tag_t Tag_barrel;
  extern const Tag_t Tag_limit;
  extern const Tag_t Tag_module;
  extern const Tag_t Attr_system;
  extern const Tag_t Attr_barrel;
  extern const Tag_t Attr_layer;
  // -- SiTrackers
  extern const Tag_t Tag_Air;
  extern const Tag_t Tag_Vacuum;
  extern const Tag_t Tag_module;
  extern const Tag_t Tag_layer;
  extern const Tag_t Tag_position;
  extern const Tag_t Tag_rotation;
  extern const Tag_t Tag_sensor;
  extern const Tag_t Tag_wedge;
  extern const Tag_t Tag_system;
  extern const Tag_t Tag_barrel;
  extern const Tag_t Tag_component;
  extern const Tag_t Tag_sensor;
  extern const Tag_t Tag_trd;
  extern const Tag_t Tag_module_component;
  extern const Tag_t Attr_system;
  extern const Tag_t Attr_barrel;
  extern const Tag_t Attr_InvisibleWithDaughters;
  //================================ Segmentation: =============================
  extern const Tag_t Tag_nonprojective_cylinder;
  extern const Tag_t Tag_projective_cylinder;
  extern const Tag_t Tag_projective_zplane;
  extern const Tag_t Tag_grid_xyz;
  extern const Tag_t Tag_NULL;
  extern const Tag_t Attr_thetaBins;
  extern const Tag_t Attr_phiBins;
  extern const Tag_t Attr_gridSizeX;
  extern const Tag_t Attr_gridSizeY;
  extern const Tag_t Attr_gridSizeZ;
  extern const Tag_t Attr_gridSizePhi;
  extern const Tag_t Attr_useForHitPosition;
  extern const Tag_t Attr_type;
  extern const Tag_t Attr_ntheta;
  extern const Tag_t Attr_nphi;
  extern const Tag_t Attr_thetaBins;
  extern const Tag_t Attr_phiBins;
  extern const Tag_t Attr_grid_size_phi;
  extern const Tag_t Attr_grid_size_x;
  extern const Tag_t Attr_grid_size_y;
  extern const Tag_t Attr_grid_size_z;

  extern const Tag_t Tag_barrel_envelope;
  extern const Tag_t Tag_module_envelope;
  extern const Tag_t Tag_rphi_layout;
  extern const Tag_t Tag_z_layout;
  extern const Tag_t Attr_phi0;
  extern const Tag_t Attr_phi_tilt;
  extern const Tag_t Attr_rc;
  extern const Tag_t Attr_dr;
  extern const Tag_t Attr_z0;
  extern const Tag_t Attr_nz;
  extern const Tag_t Attr_dz;
  extern const Tag_t Attr_nmodules;
  extern const Tag_t Attr_zstart;
  extern const Tag_t Tag_ring;

  // ILCEx
  extern const Tag_t Tag_support;
  extern const Tag_t Tag_ladder;
  extern const Tag_t Attr_number;
  extern const Tag_t Attr_offset;
  extern const Tag_t Attr_radius;
  // -- TPC, modules for endplate
  extern const Tag_t Tag_modules;
  extern const Tag_t Tag_row;
  extern const Tag_t Attr_nModules;
  extern const Tag_t Attr_modulePosX;
  extern const Tag_t Attr_modulePosY;
  extern const Tag_t Attr_moduleHeight;
  extern const Tag_t Attr_moduleWidth;
  extern const Tag_t Attr_modulePitch;
  extern const Tag_t Attr_rowPitch;
  extern const Tag_t Attr_pads;
  extern const Tag_t Attr_RowID;

}}
#endif // DD4hep_XML_TAGS_H
