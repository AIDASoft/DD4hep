// ==========================================================================
//   Software for the ALEPH experiment
// --------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
// 
//  For the licensing terms see ALSOFTINSTALL/LICENSE.
//  For the list of contributors see ALSOFTINSTALL/doc/CREDITS.
// 
//  Author     : M.Frank
// 
// ==========================================================================

#include <dd4hep/geant3_geometry.h>

/// Handle the conversion of the Geant3 materials: 
void dd4hep::geant3_geometry_imp::handle_materials()  {        
  /// First add all pure materials:
  this->add_material(15, "AIR", 7.3, 14.61, 0.001, 30400.0, 67500.0); 
  this->add_material(16, "VACUUM", 0.0, 0.0, 0.0, 1e+16, 1e+16); 
  this->add_material(5, "BERILLIUM", 4.0, 9.01, 1.848, 35.3, 36.7); 
  this->add_material(9, "ALUMINIUM", 13.0, 26.98, 2.7, 8.9, 37.2); 
  this->add_material(51, "COMPENSATOR_AVRG", 26.0, 55.85, 1.95, 7.04, 77.0); 
  this->add_material(67, "VDET_AIR", 7.3, 14.61, 0.001, 30400.0, 67500.0); 
  this->add_material(18, "CARBON_FIBER_PLASTIC", 6.0, 12.0, 1.97, 21.6, 43.8); 
  this->add_material(17, "SILICON", 14.0, 28.09, 2.33, 9.36, 10.0); 
  this->add_material(82, "PITCH", 6.0, 12.0, 1.93, 22.0, 51.2); 
  this->add_material(66, "RAIL", 5.992, 11.986, 2.052, 20.7, 48.1); 
  this->add_material(89, "ITC_wires_&_gas", 18.779, 41.452, 0.003, 6750.0, 49500.0); 
  this->add_material(90, "ITC_inner_wall", 6.327, 12.698, 1.989, 20.6, 50.5); 
  this->add_material(91, "ITC_outer_wall", 6.202, 12.432, 1.359, 30.5, 73.4); 
  this->add_material(92, "ITC_cables", 28.905, 63.325, 0.284, 45.4, 547.0); 
  this->add_material(97, "TPC_GAS_mixture_Ar/C", 17.492, 38.774, 0.002, 13300.0, 90400.0); 
  this->add_material(94, "TPC_Inner_cage_matte", 8.869, 18.175, 0.957, 33.8, 111.0); 
  this->add_material(96, "TPC_Outer_cage_matte", 11.09, 22.96, 0.62, 44.1, 190.0); 
  this->add_material(95, "TPC_Rohacell_support", 3.6, 6.7, 0.07, 806.0, 1200.0); 
  this->add_material(27, "G10_PC_BOARD", 7.84, 15.48, 1.7, 19.4, 53.0); 
  this->add_material(46, "TPC_HORIZONTAL_FEET", 13.0, 26.98, 0.529, 45.3, 233.0); 
  this->add_material(13, "LEAD", 82.0, 207.19, 11.35, 0.56, 18.5); 
  this->add_material(106, "LCAL_TUNGSTEN", 72.6, 180.0, 18.3, 0.375, 11.2); 
  this->add_material(20, "INOX", 25.7, 55.15, 8.0, 1.76, 17.1); 
  this->add_material(10, "IRON", 26.0, 55.85, 7.87, 1.76, 17.1); 
  this->add_material(11, "COPPER", 29.0, 63.54, 8.96, 1.43, 14.8); 
  this->add_material(109, "AVG_SAMBA_ELECT.", 12.89, 26.75, 1.22, 22.5, 53.4); 
  this->add_material(12, "TUNGSTEN", 74.0, 183.85, 19.3, 0.35, 10.3); 
  this->add_material(125, "SCAL_G10", 7.84, 15.48, 1.7, 20.1, 62.3); 
  this->add_material(113, "SICAL_AIR", 7.3, 14.61, 0.001, 30400.0, 67500.0); 
  this->add_material(126, "SCAL_KAPTON", 6.36, 12.701, 1.4, 29.0, 71.7); 
  this->add_material(116, "SCAL_ceramics", 11.0, 22.0, 3.86, 7.0, 30.0); 
  this->add_material(115, "SCAL_SILICIUM", 14.0, 28.09, 2.33, 9.36, 53.4); 
  this->add_material(130, "COIL_BODY_AVERAGE", 12.99, 26.98, 1.13, 21.2, 109.0); 
  this->add_material(131, "COIL_END_BODY_AVE", 12.99, 26.98, 0.5, 48.0, 246.0); 
  this->add_material(132, "PUMPING_STATION_AVER", 26.0, 55.85, 0.84, 16.5, 179.0); 
  this->add_material(133, "QUADR_TUBE_MATTER", 27.5, 59.7, 2.37, 5.62, 64.5); 
  this->add_material(135, "HCAL_IRON", 26.0, 55.847, 7.87, 1.76, 17.1); 
  /// Now add all the composite materials:
  this->add_mixture(57, "GLASS_FIBER", 9.311, 18.632, 1.761, 17.2, 62.5, {{5.933, 11.804, 0.3066}, {10.805, 21.652, 0.6934}});
  this->add_mixture(59, "Beam_mask_W", 71.761, 177.793, 18.0, 0.385, 11.3, {{28.0, 58.69, 0.032}, {29.0, 63.54, 0.017}, {74.0, 183.85, 0.9509}, {7.3, 14.61, 0.0}});
  this->add_mixture(58, "Beam_supp_disk", 12.117, 25.074, 0.596, 42.5, 198.0, {{3.6, 6.7, 0.094}, {13.0, 26.98, 0.906}});
  this->add_mixture(80, "CARBON_FIBER_ALU", 7.53, 15.275, 2.058, 17.6, 50.2, {{13.0, 26.98, 0.2186}, {6.0, 12.0, 0.7814}});
  this->add_mixture(81, "CARBON_FIBER_CYL", 6.084, 12.161, 1.801, 23.3, 55.0, {{6.456, 12.878, 0.1837}, {6.0, 12.0, 0.8163}});
  this->add_mixture(26, "MYLAR_OR_EQUI._GLUE", 6.456, 12.878, 1.39, 28.6, 70.0, {{6.0, 12.011, 0.625}, {1.0, 1.008, 0.042}, {8.0, 16.0, 0.333}});
  this->add_mixture(68, "CERAMIC", 10.646, 21.811, 3.965, 7.03, 29.0, {{13.0, 26.98, 0.5292}, {8.0, 16.0, 0.4708}});
  this->add_mixture(69, "QUARTZ", 10.805, 21.652, 2.64, 10.2, 43.4, {{14.0, 28.09, 0.4675}, {8.0, 16.0, 0.5325}});
  this->add_mixture(74, "UPILEXGLUE", 6.424, 12.819, 1.393, 28.9, 72.2, {{6.36, 12.701, 0.3349}, {6.456, 12.878, 0.6651}});
  this->add_mixture(76, "KEVLARGLUE", 6.188, 12.366, 1.626, 25.5, 61.3, {{6.36, 12.701, 0.3691}, {6.0, 12.0, 0.5088}, {6.456, 12.878, 0.1221}});
  this->add_mixture(75, "KEVLAR", 6.151, 12.295, 1.665, 25.0, 59.7, {{6.36, 12.701, 0.4204}, {6.0, 12.0, 0.5796}});
  this->add_mixture(85, "BEOGLUE", 6.558, 13.475, 2.863, 14.3, 35.6, {{6.559, 13.482, 0.9882}, {6.456, 12.878, 0.0118}});
  this->add_mixture(71, "BERILLIUM_OXIDE", 6.559, 13.482, 2.9, 14.1, 34.7, {{4.0, 9.01, 0.3603}, {8.0, 16.0, 0.6397}});
  this->add_mixture(78, "HYBRIDAU31", 20.034, 46.901, 3.135, 6.37, 37.0, {{6.559, 13.482, 0.5198}, {79.0, 196.967, 0.1702}, {10.805, 21.652, 0.2704}, {6.456, 12.878, 0.0395}});
  this->add_mixture(87, "HYBRIDAU13", 13.522, 30.298, 2.867, 9.04, 38.6, {{6.559, 13.482, 0.5674}, {79.0, 196.967, 0.0779}, {10.805, 21.652, 0.3115}, {6.456, 12.878, 0.0432}});
  this->add_mixture(79, "CAMEX", 21.081, 46.488, 2.577, 6.72, 50.6, {{14.0, 28.09, 0.8911}, {79.0, 196.967, 0.1089}});
  this->add_mixture(83, "WATER", 7.215, 14.32, 1.0, 35.8, 94.9, {{1.0, 1.01, 0.1121}, {8.0, 16.0, 0.8879}});
  this->add_mixture(86, "CSCARD", 16.308, 34.713, 2.516, 8.17, 48.3, {{7.84, 15.48, 0.5998}, {29.0, 63.54, 0.4002}});
  this->add_mixture(88, "ALUMINIUMAIR", 12.998, 26.976, 1.418, 16.9, 86.9, {{13.0, 26.98, 0.9997}, {7.3, 14.61, 0.0003}});
  this->add_mixture(93, "ITC_electronics", 13.094, 27.434, 0.235, 103.0, 513.0, {{13.0, 26.98, 0.5735}, {6.36, 12.701, 0.0066}, {29.0, 63.54, 0.0472}, {9.311, 18.632, 0.2971}, {50.0, 118.69, 0.007}, {82.0, 207.19, 0.0049}, {16.361, 35.339, 0.0301}, {6.456, 12.878, 0.0336}});
  this->add_mixture(98, "TPC_res_chain_matter", 26.854, 58.678, 0.426, 32.2, 348.0, {{29.0, 63.54, 0.9}, {7.84, 15.48, 0.08}, {6.36, 12.7, 0.02}});
  this->add_mixture(99, "TPC_prism_matter", 12.127, 24.742, 0.685, 36.5, 172.0, {{29.0, 63.54, 0.08}, {6.36, 12.7, 0.029}, {10.8, 21.65, 0.891}});
  this->add_mixture(100, "TPC_mirror_matter", 12.127, 24.742, 0.285, 87.7, 414.0, {{29.0, 63.54, 0.08}, {6.36, 12.7, 0.029}, {10.8, 21.65, 0.891}});
  this->add_mixture(101, "TPC_ENDPLATE_MATTER", 12.985, 26.947, 0.405, 59.2, 304.0, {{13.0, 26.98, 0.9973}, {7.3, 14.61, 0.0027}});
  this->add_mixture(34, "ECAL_BL_PASSIVE_MAT", 19.377, 41.255, 1.33, 13.3, 99.2, {{20.541, 44.255, 0.2188}, {25.7, 55.15, 0.4851}, {7.84, 15.48, 0.2939}, {50.402, 122.303, 0.0022}});
  this->add_mixture(35, "END_STACK_AVERAGE", 7.967, 15.8, 0.853, 39.8, 124.0, {{7.84, 15.48, 0.997}, {50.402, 122.303, 0.003}});
  this->add_mixture(25, "PVC", 11.998, 24.776, 1.38, 18.4, 79.4, {{1.0, 1.008, 0.0484}, {6.0, 12.011, 0.3844}, {17.0, 35.453, 0.5673}});
  this->add_mixture(32, "EC_LIGHT_AVERAGE_BL", 67.121, 168.351, 3.697, 2.05, 50.0, {{13.051, 27.219, 0.2158}, {82.0, 207.19, 0.7842}});
  this->add_mixture(33, "EC_DENSE_AVERAGE_BL", 73.66, 185.422, 5.254, 1.33, 37.5, {{13.051, 27.219, 0.121}, {82.0, 207.19, 0.879}});
  this->add_mixture(43, "EC_LEFT+RIGHT_AVERAG", 18.131, 41.444, 1.997, 10.0, 68.0, {{10.122, 20.65, 0.2362}, {20.608, 47.875, 0.7638}});
  this->add_mixture(36, "RESIN_PRESSURE_BAG", 6.054, 12.045, 1.2, 34.3, 78.2, {{1.0, 1.008, 0.0718}, {6.0, 12.011, 0.7093}, {7.0, 14.007, 0.0243}, {8.0, 15.999, 0.1945}});
  this->add_mixture(44, "EC_VERY_FIRST_LAYER", 12.24, 25.353, 1.554, 16.2, 77.9, {{11.695, 24.187, 0.5825}, {13.0, 26.98, 0.4175}});
  this->add_mixture(38, "EC_LIGHT_AVERAGE_EC", 64.482, 161.592, 3.634, 2.16, 49.3, {{11.695, 24.187, 0.2492}, {82.0, 207.19, 0.7508}});
  this->add_mixture(39, "EC_DENSE_AVERAGE_EC", 71.995, 181.146, 5.129, 1.39, 37.6, {{11.695, 24.187, 0.1423}, {82.0, 207.19, 0.8577}});
  this->add_mixture(42, "ECAP_BOTTOM_AVERAGE", 76.473, 192.853, 7.74, 0.874, 26.2, {{7.84, 15.48, 0.0427}, {50.402, 122.303, 0.0}, {82.0, 207.19, 0.8584}, {13.0, 26.98, 0.0121}, {64.482, 161.592, 0.0867}});
  this->add_mixture(50, "EC_ELEC._BOX_AVERAGE", 21.745, 46.965, 1.2, 13.6, 116.0, {{29.0, 63.54, 0.5161}, {13.0, 26.98, 0.4193}, {20.541, 44.255, 0.0646}});
  this->add_mixture(47, "TPC_VERTICAL__FEET", 21.854, 46.62, 1.64, 9.74, 85.7, {{13.0, 26.98, 0.3028}, {25.7, 55.15, 0.6972}});
  this->add_mixture(48, "TPC_CABLES_AVERAGE", 17.255, 36.762, 1.395, 14.0, 92.8, {{29.0, 63.54, 0.3092}, {11.998, 24.776, 0.6908}});
  this->add_mixture(49, "ITC_CABLES_AVERAGE", 19.023, 40.793, 1.49, 12.2, 89.1, {{29.0, 63.54, 0.4132}, {11.998, 24.776, 0.5868}});
  this->add_mixture(45, "ECAL_PETAL_SUPPORT", 20.097, 42.846, 2.853, 6.02, 47.7, {{13.0, 26.98, 0.4313}, {25.7, 55.15, 0.4655}, {29.0, 63.54, 0.0706}, {50.0, 118.69, 0.0053}, {7.84, 15.48, 0.0273}});
  this->add_mixture(103, "LCAL_NE110", 5.573, 11.071, 1.032, 42.1, 88.5, {{6.0, 12.011, 0.9146}, {1.0, 1.01, 0.0854}});
  this->add_mixture(105, "LCAL_screws_front_pl", 17.298, 36.514, 3.48, 5.54, 37.6, {{13.0, 26.98, 0.6616}, {25.7, 55.15, 0.3384}});
  this->add_mixture(102, "LCAL_XE-CO2", 50.402, 122.303, 0.005, 1770.0, 34900.0, {{6.0, 12.011, 0.0211}, {8.0, 15.999, 0.0562}, {54.0, 131.3, 0.9227}});
  this->add_mixture(107, "LCAL_LIGHT_AVERAGE", 69.769, 175.326, 4.454, 1.65, 42.4, {{12.155, 25.224, 0.1751}, {82.0, 207.19, 0.8249}});
  this->add_mixture(108, "LCAL_DENSE_AVERAGE", 75.298, 189.729, 6.14, 1.12, 32.5, {{12.155, 25.224, 0.096}, {82.0, 207.19, 0.904}});
  this->add_mixture(112, "Samba_gas_&_wir", 68.395, 169.477, 0.014, 508.0, 13800.0, {{74.0, 183.85, 0.902}, {16.792, 37.151, 0.098}});
  this->add_mixture(129, "SCAL_CABLING_AVG", 16.098, 35.045, 0.151, 140.0, 827.0, {{7.84, 15.48, 0.4441}, {29.0, 63.54, 0.1184}, {13.0, 26.98, 0.0296}, {21.568, 48.661, 0.4079}});
  this->add_mixture(124, "SCAL_COOLING_AVG", 26.835, 58.634, 3.995, 3.44, 37.2, {{7.3, 14.61, 0.0001}, {13.0, 26.98, 0.0336}, {29.0, 63.54, 0.8917}, {7.215, 14.32, 0.0746}});
  this->add_mixture(122, "SCAL_W_THIN__AVG", 73.207, 181.739, 18.07, 0.378, 11.4, {{6.456, 12.878, 0.0024}, {73.368, 182.145, 0.9976}});
  this->add_mixture(121, "SCAL_W_THICK_AVG", 72.142, 178.999, 16.458, 0.42, 12.4, {{6.456, 12.878, 0.0019}, {73.368, 182.145, 0.9799}, {13.0, 26.98, 0.0182}});
  this->add_mixture(123, "SCAL_WVERYTH_AVG", 65.989, 163.38, 8.736, 0.857, 21.4, {{6.456, 12.878, 0.0044}, {73.368, 182.145, 0.8875}, {7.84, 15.48, 0.1081}});
  this->add_mixture(118, "SCAL_PASSIVE_AV1", 9.671, 19.515, 1.442, 20.6, 77.3, {{7.3, 14.61, 0.0002}, {13.0, 26.98, 0.2978}, {6.36, 12.701, 0.0116}, {7.84, 15.48, 0.5919}, {11.0, 22.0, 0.0985}});
  this->add_mixture(127, "SCAL_PASSIVE_AV2", 14.944, 31.482, 1.472, 14.8, 82.5, {{7.3, 14.61, 0.0003}, {13.0, 26.98, 0.2918}, {7.84, 15.48, 0.4076}, {14.0, 28.09, 0.0504}, {29.0, 63.54, 0.2499}});
  this->add_mixture(128, "SCAL_PASSIVE_AV3", 9.748, 19.757, 1.336, 22.1, 83.5, {{7.3, 14.61, 0.0003}, {13.0, 26.98, 0.3214}, {7.84, 15.48, 0.6387}, {11.998, 24.776, 0.0308}, {21.568, 48.661, 0.0089}});
  this->add_mixture(119, "SCAL_PASSIVE_LAS", 16.991, 36.115, 1.402, 14.1, 90.1, {{7.3, 14.61, 0.0005}, {13.0, 26.98, 0.2945}, {6.36, 12.701, 0.0172}, {7.84, 15.48, 0.2028}, {11.0, 22.0, 0.1447}, {29.0, 63.54, 0.3403}});
  this->add_mixture(144, "HCBL_PASSIVE_MATERIA", 25.993, 55.827, 6.795, 2.04, 22.1, {{7.3, 14.61, 0.0}, {25.994, 55.828, 1.0}});
  this->add_mixture(143, "HCAL_SENS._MATERIAL", 12.791, 26.639, 0.617, 39.5, 199.0, {{12.8, 26.658, 0.9978}, {8.558, 17.794, 0.0022}});
  this->add_mixture(146, "HCAP_AVERAGE_MATTER", 25.993, 55.827, 6.831, 2.03, 22.0, {{7.3, 14.61, 0.0}, {25.994, 55.828, 1.0}});
} /// End geant3_geometry_imp::handle_materials  


/// Handle the conversion of the Geant3 media: 
void dd4hep::geant3_geometry_imp::handle_media()  { 

  this->add_medium( 1, "AIR_INSIDE_FIELD", "AIR" );
  this->add_medium( 4, "CENTRAL_DET.REGION", "AIR" );
  this->add_medium( 5, "BEAM_VACUUM__MED", "VACUUM" );
  this->add_medium( 8, "BERIL._PIPE__MED", "BERILLIUM" );
  this->add_medium( 7, "ALU_PARTS____MED", "ALUMINIUM" );
  this->add_medium( 6, "COMPENSATORS_MED", "COMPENSATOR_AVRG" );
  this->add_medium( 9, "GLASS_FIBER__MED", "GLASS_FIBER" );
  this->add_medium( 10, "TUNGSTEN_____MED", "Beam_mask_W" );
  this->add_medium( 11, "Casserole_disk", "Beam_supp_disk" );
  this->add_medium( 12, "VDET_AIR", "VDET_AIR" );
  this->add_medium( 26, "VDET_SHIELDING", "CARBON_FIBER_ALU" );
  this->add_medium( 27, "VDET_CYLINDER", "CARBON_FIBER_CYL" );
  this->add_medium( 25, "VDET_PLASTIC", "CARBON_FIBER_PLASTIC" );
  this->add_medium( 14, "VDET_GLUE", "MYLAR_OR_EQUI._GLUE" );
  this->add_medium( 15, "VDET_CERAMIC", "CERAMIC" );
  this->add_medium( 16, "VDET_QUARTZ", "QUARTZ" );
  this->add_medium( 13, "VDET_SILICON", "SILICON" );
  this->add_medium( 17, "VDET_UPILEXGLUE", "UPILEXGLUE" );
  this->add_medium( 18, "VDET_KEVLARGLUE", "KEVLARGLUE" );
  this->add_medium( 19, "VDET_KEVLAR", "KEVLAR" );
  this->add_medium( 29, "VDET_BEOGLUE", "BEOGLUE" );
  this->add_medium( 20, "VDET_BEO", "BERILLIUM_OXIDE" );
  this->add_medium( 21, "VDET_HYBRID10", "HYBRIDAU31" );
  this->add_medium( 22, "VDET_HYBRID11", "HYBRIDAU13" );
  this->add_medium( 23, "VDET_CAPACITOR", "QUARTZ" );
  this->add_medium( 24, "VDET_MX7", "CAMEX" );
  this->add_medium( 28, "VDET_PITCH", "PITCH" );
  this->add_medium( 32, "VDET_CSFLANGE", "ALUMINIUM" );
  this->add_medium( 30, "VDET_WATER", "WATER" );
  this->add_medium( 33, "VDET_CSCARD", "CSCARD" );
  this->add_medium( 31, "VDET_RAIL", "RAIL" );
  this->add_medium( 34, "VDET_FOOT", "ALUMINIUMAIR" );
  this->add_medium( 35, "VDET_LOCK", "ALUMINIUM" );
  this->add_medium( 36, "ITC_sensitive", "ITC_wires_&_gas" );
  this->add_medium( 37, "ITC_inner_wall", "ITC_inner_wall" );
  this->add_medium( 38, "ITC_outer_wall", "ITC_outer_wall" );
  this->add_medium( 39, "ITC_end_plates", "ALUMINIUM" );
  this->add_medium( 40, "ITC_electronics", "ITC_electronics" );
  this->add_medium( 41, "ITC_cables", "ITC_cables" );
  this->add_medium( 43, "TPC_GAS_medium", "TPC_GAS_mixture_Ar/C" );
  this->add_medium( 42, "TPC_inner_wall_med", "TPC_Inner_cage_matte" );
  this->add_medium( 44, "TPC_OUTER_WALL_med", "TPC_Outer_cage_matte" );
  this->add_medium( 45, "TPC_INNERCAGE_SUPPOR", "ALUMINIUM" );
  this->add_medium( 46, "TPC_Mylar_inner_sup", "TPC_Rohacell_support" );
  this->add_medium( 47, "TPC_Mylar_outer_sup", "ALUMINIUM" );
  this->add_medium( 48, "TPC_MYLAR_MEMBRANE", "MYLAR_OR_EQUI._GLUE" );
  this->add_medium( 49, "TPC_G10_reinf_ring", "G10_PC_BOARD" );
  this->add_medium( 50, "TPC_resistor_chain", "TPC_res_chain_matter" );
  this->add_medium( 51, "TPC_prisms", "TPC_prism_matter" );
  this->add_medium( 52, "TPC_mirrors", "TPC_mirror_matter" );
  this->add_medium( 53, "TP_ENDPLATE_VOLUME", "TPC_ENDPLATE_MATTER" );
  this->add_medium( 54, "EC_BARREL_REGION", "AIR" );
  this->add_medium( 55, "EC_ENDCAP_REGION", "AIR" );
  this->add_medium( 56, "EC_BARREL_VOLUME", "AIR" );
  this->add_medium( 57, "EC_ALU_FRAMES", "ALUMINIUM" );
  this->add_medium( 58, "ECBL_AV._PASSIVE_MED", "ECAL_BL_PASSIVE_MAT" );
  this->add_medium( 59, "EC_BL_ENDPART_MEDIUM", "END_STACK_AVERAGE" );
  this->add_medium( 60, "PVC_STACK_SEPARATOR", "PVC" );
  this->add_medium( 61, "EC_STACK_1+2_BARREL", "EC_LIGHT_AVERAGE_BL" );
  this->add_medium( 62, "EC_STACK_3_BARREL", "EC_DENSE_AVERAGE_BL" );
  this->add_medium( 63, "EC_LEFT_PASSIVE_MIX", "EC_LEFT+RIGHT_AVERAG" );
  this->add_medium( 64, "EC_ENDCAP_RESINE_BAG", "RESIN_PRESSURE_BAG" );
  this->add_medium( 66, "ALU_STACK_SEPARATOR", "ALUMINIUM" );
  this->add_medium( 67, "EC_STACK_0_ENDCAP", "EC_VERY_FIRST_LAYER" );
  this->add_medium( 65, "EC_STACK_1+2_ENDCAP", "EC_LIGHT_AVERAGE_EC" );
  this->add_medium( 68, "EC_STACK_3_ENDCAP", "EC_DENSE_AVERAGE_EC" );
  this->add_medium( 69, "EC_NARROW_PASSIVMIX", "ECAP_BOTTOM_AVERAGE" );
  this->add_medium( 70, "PASS._MAT._BETW.MODU", "AIR" );
  this->add_medium( 71, "ELEC._BOX_ABOVE_ECAL", "EC_ELEC._BOX_AVERAGE" );
  this->add_medium( 72, "ELEC_BOX_IN_NONUNI_B", "EC_ELEC._BOX_AVERAGE" );
  this->add_medium( 73, "TPC_HORIZONTAL_FOOT", "TPC_HORIZONTAL_FEET" );
  this->add_medium( 74, "TPC_VERTICAL_FOOT", "TPC_VERTICAL__FEET" );
  this->add_medium( 75, "TPC_CABLES_IN_UNI._B", "TPC_CABLES_AVERAGE" );
  this->add_medium( 76, "TPC_CABLES_NON_UNI_B", "TPC_CABLES_AVERAGE" );
  this->add_medium( 77, "ITC_CABLES_IN_UNI._B", "ITC_CABLES_AVERAGE" );
  this->add_medium( 78, "ITC_CABLES_NON_UNI_B", "ITC_CABLES_AVERAGE" );
  this->add_medium( 79, "END_CAP_SUPPORT_PLAT", "ECAL_PETAL_SUPPORT" );
  this->add_medium( 80, "LC_CALO_VOLUME", "AIR" );
  this->add_medium( 83, "LC_LEAD_SHIELD", "LEAD" );
  this->add_medium( 84, "LC_W_absorber", "LCAL_TUNGSTEN" );
  this->add_medium( 85, "LC_NE110_Scint.", "LCAL_NE110" );
  this->add_medium( 81, "LC_WALLS_VOLUME", "ALUMINIUM" );
  this->add_medium( 86, "LCAL_INOX_SCREWS_ARE", "LCAL_screws_front_pl" );
  this->add_medium( 87, "LCAL_INOX_SCREWS_MED", "INOX" );
  this->add_medium( 82, "LC_PASSIVE_GAS_MIX", "LCAL_XE-CO2" );
  this->add_medium( 88, "LC_BACK_PLATE_VOLUME", "IRON" );
  this->add_medium( 89, "LC_STACK1+2_VOLUME", "LCAL_LIGHT_AVERAGE" );
  this->add_medium( 90, "LC_STACK3_VOLUME", "LCAL_DENSE_AVERAGE" );
  this->add_medium( 91, "SAMBA_VOLUME", "AIR" );
  this->add_medium( 92, "SAMBA_Vetronite", "G10_PC_BOARD" );
  this->add_medium( 93, "SAMBA_GAS", "Samba_gas_&_wir" );
  this->add_medium( 94, "SAMBA_copper", "COPPER" );
  this->add_medium( 95, "SAMBA_electronic", "AVG_SAMBA_ELECT." );
  this->add_medium( 96, "SCAL_VOLUME", "AIR" );
  this->add_medium( 102, "SCAL_LEAD_TUBE", "LEAD" );
  this->add_medium( 98, "SCAL_AL_PLATE", "ALUMINIUM" );
  this->add_medium( 114, "SCAL_CABLES_AREA", "SCAL_CABLING_AVG" );
  this->add_medium( 109, "SCAL_COOLING_VOL", "SCAL_COOLING_AVG" );
  this->add_medium( 107, "SCAL_THIN__ABSBR", "SCAL_W_THIN__AVG" );
  this->add_medium( 97, "SCAL_THICK_ABSBR", "SCAL_W_THICK_AVG" );
  this->add_medium( 108, "SCAL_UTHIN_ABSBR", "SCAL_WVERYTH_AVG" );
  this->add_medium( 111, "SCAL_FIRST_ABSBR", "TUNGSTEN" );
  this->add_medium( 104, "SCAL_PASSIVE_AV1", "SCAL_PASSIVE_AV1" );
  this->add_medium( 112, "SCAL_PASSIVE_AV2", "SCAL_PASSIVE_AV2" );
  this->add_medium( 113, "SCAL_PASSIVE_AV3", "SCAL_PASSIVE_AV3" );
  this->add_medium( 105, "SCAL_LAST_PASAVG", "SCAL_PASSIVE_LAS" );
  this->add_medium( 110, "SCAL_goupilles", "INOX" );
  this->add_medium( 99, "SCAL_G10_PLATE", "SCAL_G10" );
  this->add_medium( 106, "SCAL_VOLUME_ACT", "SICAL_AIR" );
  this->add_medium( 103, "SCAL_KAPTON-GLUE", "SCAL_KAPTON" );
  this->add_medium( 100, "SCAL_CERAMICS", "SCAL_ceramics" );
  this->add_medium( 101, "SCAL_SI_CRYSTALS", "SCAL_SILICIUM" );
  this->add_medium( 115, "COIL_REGION", "AIR" );
  this->add_medium( 2, "AIR_OUTSIDE_FIELD", "AIR" );
  this->add_medium( 3, "AIR_IN_NON_UNIFORM_B", "AIR" );
  this->add_medium( 116, "COIL_BODY_IN_UNI_BZ", "COIL_BODY_AVERAGE" );
  this->add_medium( 117, "COIL_BODY_OUT_FIELD", "COIL_BODY_AVERAGE" );
  this->add_medium( 118, "COIL_REINFOR_UNI_BZ", "AIR" );
  this->add_medium( 119, "COIL_REINFOR_NO_FIEL", "AIR" );
  this->add_medium( 121, "COIL_RINGS_NO_FIELD", "ALUMINIUM" );
  this->add_medium( 120, "COIL_RINGS_IN_UNI_BZ", "ALUMINIUM" );
  this->add_medium( 122, "COIL_RINGS_NON_UNI_B", "ALUMINIUM" );
  this->add_medium( 123, "COIL_ENDPL_NON_UNI_B", "INOX" );
  this->add_medium( 124, "COIL_BODY_NON_UNI_B", "COIL_END_BODY_AVE" );
  this->add_medium( 125, "PUMPS_AND_VALVES_MED", "PUMPING_STATION_AVER" );
  this->add_medium( 126, "VACUUM_IN_PIPE_MED", "VACUUM" );
  this->add_medium( 127, "QUADR_TUBE_BODY", "QUADR_TUBE_MATTER" );
  this->add_medium( 128, "HADRON_BL__REGION", "AIR" );
  this->add_medium( 129, "HADRON_EC__REGION", "AIR" );
  this->add_medium( 131, "HCBL_NON_SENSITIVE", "HCBL_PASSIVE_MATERIA" );
  this->add_medium( 133, "HCAL_CALO_VOLUME", "AIR" );
  this->add_medium( 132, "LAST_IRON_PLATE_MEDI", "HCAL_IRON" );
  this->add_medium( 134, "CABLES_IN_NOTCHES", "TPC_CABLES_AVERAGE" );
  this->add_medium( 130, "HCAL_HSTREAMER_TUBES", "HCAL_SENS._MATERIAL" );
  this->add_medium( 135, "HCAP_NON_SENSITIVE", "HCAP_AVERAGE_MATTER" );
  this->add_medium( 136, "MUON_______REGION", "AIR" );
  this->add_medium( 137, "MUON_SENSITIVE_GAS", "AIR" );
} /// End geant3_geometry_imp::handle_media  


/// Handle the conversion of the Geant3 rotations: 
void dd4hep::geant3_geometry_imp::handle_rotations()  {
  this->add_rotation( 1, dd4hep::Rotation3D(1.0, -0.0, 0.0, 0.0, 1.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 2, dd4hep::Rotation3D(-1.0, -0.0, -0.0, -0.0, 1.0, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 3, dd4hep::Rotation3D(-1.0, 0.0, 0.0, -0.0, -1.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 4, dd4hep::Rotation3D(-0.0, -1.0, 0.0, 1.0, -0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 5, dd4hep::Rotation3D(1.0, -0.0, -0.0, 0.0, -0.0, 1.0, -0.0, -1.0, -0.0) );
  this->add_rotation( 6, dd4hep::Rotation3D(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 7, dd4hep::Rotation3D(1.0, 0.0, -0.0, 0.0, -1.0, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 8, dd4hep::Rotation3D(-1.0, 0.0, 0.0, -0.0, -1.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 9, dd4hep::Rotation3D(-0.0, 0.0, 1.0, 1.0, 0.0, 0.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 10, dd4hep::Rotation3D(0.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.0, -0.0, -0.0) );
  this->add_rotation( 11, dd4hep::Rotation3D(0.0, 1.0, 0.0, -1.0, 0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 12, dd4hep::Rotation3D(0.219, -0.9757, 0.0, 0.9757, 0.219, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 13, dd4hep::Rotation3D(0.3687, 0.9296, 0.0, -0.9296, 0.3687, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 14, dd4hep::Rotation3D(0.219, 0.9757, -0.0, 0.9757, -0.219, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 15, dd4hep::Rotation3D(0.3687, -0.9296, -0.0, -0.9296, -0.3687, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 16, dd4hep::Rotation3D(-0.2175, -0.9761, 0.0, 0.9761, -0.2175, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 17, dd4hep::Rotation3D(0.0476, 0.9989, 0.0, -0.9989, 0.0476, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 18, dd4hep::Rotation3D(-0.2175, 0.9761, -0.0, 0.9761, 0.2175, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 19, dd4hep::Rotation3D(0.0476, -0.9989, -0.0, -0.9989, -0.0476, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 20, dd4hep::Rotation3D(-0.9304, 0.3665, 0.0, -0.3665, -0.9304, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 21, dd4hep::Rotation3D(-0.9304, -0.3665, -0.0, -0.3665, 0.9304, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 22, dd4hep::Rotation3D(-0.4772, 0.8788, 0.0, -0.8788, -0.4772, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 23, dd4hep::Rotation3D(-0.4772, -0.8788, -0.0, -0.8788, 0.4772, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 24, dd4hep::Rotation3D(0.1994, 0.9799, 0.0, -0.9799, 0.1994, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 25, dd4hep::Rotation3D(0.1994, -0.9799, -0.0, -0.9799, -0.1994, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 26, dd4hep::Rotation3D(0.7826, 0.6225, 0.0, -0.6225, 0.7826, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 27, dd4hep::Rotation3D(0.7826, -0.6225, -0.0, -0.6225, -0.7826, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 28, dd4hep::Rotation3D(0.9997, -0.0262, 0.0, 0.0262, 0.9997, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 29, dd4hep::Rotation3D(0.9997, 0.0262, -0.0, 0.0262, -0.9997, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 30, dd4hep::Rotation3D(0.749, -0.6626, 0.0, 0.6626, 0.749, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 31, dd4hep::Rotation3D(0.749, 0.6626, -0.0, 0.6626, -0.749, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 32, dd4hep::Rotation3D(0.1478, -0.989, 0.0, 0.989, 0.1478, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 33, dd4hep::Rotation3D(0.1478, 0.989, -0.0, 0.989, -0.1478, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 34, dd4hep::Rotation3D(-0.5225, -0.8526, 0.0, 0.8526, -0.5225, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 35, dd4hep::Rotation3D(-0.5225, 0.8526, -0.0, 0.8526, 0.5225, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 36, dd4hep::Rotation3D(-0.9483, -0.3173, 0.0, 0.3173, -0.9483, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 37, dd4hep::Rotation3D(-0.9483, 0.3173, -0.0, 0.3173, 0.9483, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 38, dd4hep::Rotation3D(0.8788, -0.4772, 0.0, 0.4772, 0.8788, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 39, dd4hep::Rotation3D(0.8788, 0.4772, -0.0, 0.4772, -0.8788, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 40, dd4hep::Rotation3D(0.6088, -0.7934, 0.0, 0.7934, 0.6088, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 41, dd4hep::Rotation3D(0.6088, 0.7934, -0.0, 0.7934, -0.6088, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 42, dd4hep::Rotation3D(0.2334, -0.9724, 0.0, 0.9724, 0.2334, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 43, dd4hep::Rotation3D(0.2334, 0.9724, -0.0, 0.9724, -0.2334, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 44, dd4hep::Rotation3D(-0.1822, -0.9833, 0.0, 0.9833, -0.1822, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 45, dd4hep::Rotation3D(-0.1822, 0.9833, -0.0, 0.9833, 0.1822, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 46, dd4hep::Rotation3D(-0.5664, -0.8241, 0.0, 0.8241, -0.5664, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 47, dd4hep::Rotation3D(-0.5664, 0.8241, -0.0, 0.8241, 0.5664, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 48, dd4hep::Rotation3D(-0.8526, -0.5225, 0.0, 0.5225, -0.8526, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 49, dd4hep::Rotation3D(-0.8526, 0.5225, -0.0, 0.5225, 0.8526, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 50, dd4hep::Rotation3D(-0.9914, -0.1305, 0.0, 0.1305, -0.9914, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 51, dd4hep::Rotation3D(-0.9914, 0.1305, -0.0, 0.1305, 0.9914, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 52, dd4hep::Rotation3D(-0.9588, 0.284, 0.0, -0.284, -0.9588, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 53, dd4hep::Rotation3D(-0.9588, -0.284, -0.0, -0.284, 0.9588, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 54, dd4hep::Rotation3D(-0.7604, 0.6494, 0.0, -0.6494, -0.7604, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 55, dd4hep::Rotation3D(-0.7604, -0.6494, -0.0, -0.6494, 0.7604, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 56, dd4hep::Rotation3D(-0.4305, 0.9026, 0.0, -0.9026, -0.4305, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 57, dd4hep::Rotation3D(-0.4305, -0.9026, -0.0, -0.9026, 0.4305, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 58, dd4hep::Rotation3D(-0.0262, 0.9997, 0.0, -0.9997, -0.0262, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 59, dd4hep::Rotation3D(-0.0262, -0.9997, -0.0, -0.9997, 0.0262, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 60, dd4hep::Rotation3D(0.3827, 0.9239, 0.0, -0.9239, 0.3827, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 61, dd4hep::Rotation3D(0.3827, -0.9239, -0.0, -0.9239, -0.3827, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 62, dd4hep::Rotation3D(0.7254, 0.6884, 0.0, -0.6884, 0.7254, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 63, dd4hep::Rotation3D(0.7254, -0.6884, -0.0, -0.6884, -0.7254, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 64, dd4hep::Rotation3D(0.9426, 0.3338, 0.0, -0.3338, 0.9426, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 65, dd4hep::Rotation3D(0.9426, -0.3338, -0.0, -0.3338, -0.9426, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 66, dd4hep::Rotation3D(0.9969, -0.0785, 0.0, 0.0785, 0.9969, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 67, dd4hep::Rotation3D(0.9969, 0.0785, -0.0, 0.0785, -0.9969, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 68, dd4hep::Rotation3D(-0.0, 0.0, 1.0, 1.0, 0.0, 0.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 69, dd4hep::Rotation3D(0.9569, 0.2903, 0.0, -0.2903, 0.9569, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 70, dd4hep::Rotation3D(0.9739, 0.2271, 0.0, -0.2271, 0.9739, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 71, dd4hep::Rotation3D(-0.0, 0.0, 1.0, 1.0, 0.0, 0.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 72, dd4hep::Rotation3D(1.0, 0.0, -0.0, 0.0, -1.0, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 73, dd4hep::Rotation3D(0.0, 1.0, 0.0, -1.0, 0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 74, dd4hep::Rotation3D(-0.0, 1.0, -0.0, 1.0, 0.0, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 75, dd4hep::Rotation3D(0.9979, -0.0654, 0.0, 0.0654, 0.9979, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 76, dd4hep::Rotation3D(0.9979, 0.0654, 0.0, -0.0654, 0.9979, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 77, dd4hep::Rotation3D(0.9239, -0.3827, 0.0, 0.3827, 0.9239, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 78, dd4hep::Rotation3D(0.7071, -0.7071, 0.0, 0.7071, 0.7071, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 79, dd4hep::Rotation3D(0.3827, -0.9239, 0.0, 0.9239, 0.3827, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 80, dd4hep::Rotation3D(-0.0, -1.0, 0.0, 1.0, -0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 81, dd4hep::Rotation3D(-0.3827, -0.9239, 0.0, 0.9239, -0.3827, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 82, dd4hep::Rotation3D(-0.7071, -0.7071, 0.0, 0.7071, -0.7071, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 83, dd4hep::Rotation3D(-0.9239, -0.3827, 0.0, 0.3827, -0.9239, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 84, dd4hep::Rotation3D(-1.0, 0.0, 0.0, -0.0, -1.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 85, dd4hep::Rotation3D(-0.9239, 0.3827, 0.0, -0.3827, -0.9239, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 86, dd4hep::Rotation3D(-0.7071, 0.7071, 0.0, -0.7071, -0.7071, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 87, dd4hep::Rotation3D(-0.3827, 0.9239, 0.0, -0.9239, -0.3827, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 88, dd4hep::Rotation3D(0.0, 1.0, 0.0, -1.0, 0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 89, dd4hep::Rotation3D(0.3827, 0.9239, 0.0, -0.9239, 0.3827, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 90, dd4hep::Rotation3D(0.7071, 0.7071, 0.0, -0.7071, 0.7071, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 91, dd4hep::Rotation3D(0.9239, 0.3827, 0.0, -0.3827, 0.9239, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 92, dd4hep::Rotation3D(-0.0, 1.0, 0.0, -1.0, 0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 93, dd4hep::Rotation3D(-0.0, 1.0, -0.0, 1.0, 0.0, -0.0, -0.0, -0.0, -1.0) );
  this->add_rotation( 94, dd4hep::Rotation3D(0.866, 0.5, 0.0, -0.5, 0.866, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 95, dd4hep::Rotation3D(-0.0, 0.0, 1.0, 1.0, 0.0, 0.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 96, dd4hep::Rotation3D(-0.0, 0.0, 1.0, 1.0, 0.0, 0.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 97, dd4hep::Rotation3D(-0.5, 0.0, 0.866, 0.866, 0.0, 0.5, -0.0, 1.0, -0.0) );
  this->add_rotation( 98, dd4hep::Rotation3D(-0.866, 0.0, 0.5, 0.5, 0.0, 0.866, -0.0, 1.0, -0.0) );
  this->add_rotation( 99, dd4hep::Rotation3D(-1.0, 0.0, -0.0, -0.0, 0.0, 1.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 100, dd4hep::Rotation3D(-0.866, 0.0, -0.5, -0.5, 0.0, 0.866, -0.0, 1.0, -0.0) );
  this->add_rotation( 101, dd4hep::Rotation3D(-0.5, 0.0, -0.866, -0.866, 0.0, 0.5, -0.0, 1.0, -0.0) );
  this->add_rotation( 102, dd4hep::Rotation3D(0.0, 0.0, -1.0, -1.0, 0.0, 0.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 103, dd4hep::Rotation3D(0.5, 0.0, -0.866, -0.866, 0.0, -0.5, -0.0, 1.0, -0.0) );
  this->add_rotation( 104, dd4hep::Rotation3D(0.866, 0.0, -0.5, -0.5, 0.0, -0.866, -0.0, 1.0, -0.0) );
  this->add_rotation( 105, dd4hep::Rotation3D(1.0, 0.0, -0.0, -0.0, 0.0, -1.0, -0.0, 1.0, -0.0) );
  this->add_rotation( 106, dd4hep::Rotation3D(0.866, 0.0, 0.5, 0.5, 0.0, -0.866, -0.0, 1.0, -0.0) );
  this->add_rotation( 107, dd4hep::Rotation3D(0.5, 0.0, 0.866, 0.866, 0.0, -0.5, -0.0, 1.0, -0.0) );
  this->add_rotation( 108, dd4hep::Rotation3D(0.0, -0.0, 1.0, 0.0, -1.0, 0.0, 1.0, -0.0, -0.0) );
  this->add_rotation( 109, dd4hep::Rotation3D(0.0, 0.5, 0.866, 0.0, -0.866, 0.5, 1.0, -0.0, -0.0) );
  this->add_rotation( 110, dd4hep::Rotation3D(0.0, 0.866, 0.5, 0.0, -0.5, 0.866, 1.0, -0.0, -0.0) );
  this->add_rotation( 111, dd4hep::Rotation3D(0.0, 1.0, -0.0, 0.0, 0.0, 1.0, 1.0, -0.0, -0.0) );
  this->add_rotation( 112, dd4hep::Rotation3D(0.0, 0.866, -0.5, 0.0, 0.5, 0.866, 1.0, -0.0, -0.0) );
  this->add_rotation( 113, dd4hep::Rotation3D(0.0, 0.5, -0.866, 0.0, 0.866, 0.5, 1.0, -0.0, -0.0) );
  this->add_rotation( 114, dd4hep::Rotation3D(0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, -0.0, -0.0) );
  this->add_rotation( 115, dd4hep::Rotation3D(0.0, -0.5, -0.866, 0.0, 0.866, -0.5, 1.0, -0.0, -0.0) );
  this->add_rotation( 116, dd4hep::Rotation3D(0.0, -0.866, -0.5, 0.0, 0.5, -0.866, 1.0, -0.0, -0.0) );
  this->add_rotation( 117, dd4hep::Rotation3D(0.0, -1.0, -0.0, 0.0, 0.0, -1.0, 1.0, -0.0, -0.0) );
  this->add_rotation( 118, dd4hep::Rotation3D(0.0, -0.866, 0.5, 0.0, -0.5, -0.866, 1.0, -0.0, -0.0) );
  this->add_rotation( 119, dd4hep::Rotation3D(0.0, -0.5, 0.866, 0.0, -0.866, -0.5, 1.0, -0.0, -0.0) );
  this->add_rotation( 120, dd4hep::Rotation3D(-0.0, -1.0, 0.0, 1.0, -0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 121, dd4hep::Rotation3D(0.0, 1.0, 0.0, -1.0, 0.0, 0.0, -0.0, -0.0, 1.0) );
  this->add_rotation( 333, dd4hep::Rotation3D(1.0, -0.0, 0.0, 0.0, 1.0, 0.0, -0.0, -0.0, 1.0) );
} /// End geant3_geometry_imp::handle_rotations


/// Handle the conversion of the Geant3 volumes: 
void dd4hep::geant3_geometry_imp::handle_volumes()  { 
  dd4hep::Solid    solid;    


  solid = dd4hep::Tube( std::string("ALEF_solid"), 0.0*units::cm, 650.0*units::cm, 600.0*units::cm ); 
  this->add_volume( 1, "ALEF", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("CDET_solid"), 5.65*units::cm, 184.7*units::cm, 240.0*units::cm ); 
  this->add_volume( 2, "CDET", "CENTRAL_DET.REGION", solid, "ONLY");

  solid = dd4hep::Tube( std::string("PASV_solid"), 5.65*units::cm, 31.0*units::cm, 51.95*units::cm ); 
  this->add_volume( 3, "PASV", "CENTRAL_DET.REGION", solid, "ONLY");

  solid = dd4hep::Tube( std::string("PASW_solid"), 5.65*units::cm, 31.0*units::cm, 51.95*units::cm ); 
  this->add_volume( 4, "PASW", "CENTRAL_DET.REGION", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BTUB_solid"), 0.0*units::cm, 5.65*units::cm, 262.5*units::cm ); 
  this->add_volume( 5, "BTUB", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BTUP_solid"), 0.0*units::cm, 8.5*units::cm, 26.25*units::cm ); 
  this->add_volume( 6, "BTUP", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BVAC_solid"), 0.0*units::cm, 5.3*units::cm, 262.5*units::cm ); 
  this->add_volume( 7, "BVAC", "BEAM_VACUUM__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPIP_solid"), 5.3*units::cm, 5.65*units::cm, 44.0*units::cm ); 
  this->add_volume( 8, "BPIP", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPBY_solid"), 5.3*units::cm, 5.65*units::cm, 38.0*units::cm ); 
  this->add_volume( 9, "BPBY", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPBE_solid"), 5.3*units::cm, 5.41*units::cm, 38.0*units::cm ); 
  this->add_volume( 10, "BPBE", "BERIL._PIPE__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPSO_solid"), 5.41*units::cm, 5.57*units::cm, 38.0*units::cm, 268.835*units::degree, 271.1648*units::degree ); 
  this->add_volume( 11, "BPSO", "BERIL._PIPE__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BBAL_solid"), 5.41*units::cm, 5.65*units::cm, 0.125*units::cm, 1.165*units::degree, 358.835*units::degree ); 
  this->add_volume( 12, "BBAL", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPND_solid"), 5.3*units::cm, 5.65*units::cm, 3.0*units::cm ); 
  this->add_volume( 13, "BPND", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BAIR_solid"), 5.45*units::cm, 5.65*units::cm, 1.25*units::cm ); 
  this->add_volume( 14, "BAIR", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BXAL_solid"), 5.2*units::cm, 5.3*units::cm, 1.2*units::cm ); 
  this->add_volume( 15, "BXAL", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPAL_solid"), 5.3*units::cm, 5.45*units::cm, 109.5*units::cm ); 
  this->add_volume( 16, "BPAL", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BSAL_solid"), 5.45*units::cm, 5.5*units::cm, 17.575*units::cm ); 
  this->add_volume( 17, "BSAL", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRIB_solid"), 5.45*units::cm, 5.65*units::cm, 0.75*units::cm ); 
  this->add_volume( 18, "BRIB", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRIA_solid"), 5.5*units::cm, 5.65*units::cm, 0.75*units::cm ); 
  this->add_volume( 19, "BRIA", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BCOM_solid"), 0.0*units::cm, 8.5*units::cm, 24.0*units::cm ); 
  this->add_volume( 20, "BCOM", "COMPENSATORS_MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BVCO_solid"), 0.0*units::cm, 6.0*units::cm, 24.0*units::cm ); 
  this->add_volume( 21, "BVCO", "BEAM_VACUUM__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BEND_solid"), 5.3*units::cm, 7.3*units::cm, 2.25*units::cm ); 
  this->add_volume( 22, "BEND", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRID_solid"), 5.3*units::cm, 5.5*units::cm, 1.15*units::cm ); 
  this->add_volume( 23, "BRID", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRIN_solid"), 5.3*units::cm, 7.3*units::cm, 1.1*units::cm ); 
  this->add_volume( 24, "BRIN", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BVCI_solid"), 0.0*units::cm, 5.3*units::cm, 2.25*units::cm ); 
  this->add_volume( 25, "BVCI", "BEAM_VACUUM__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPSP_solid"), 5.65*units::cm, 6.55*units::cm, 0.55*units::cm ); 
  this->add_volume( 26, "BPSP", "GLASS_FIBER__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPSA_solid"), 5.65*units::cm, 7.9*units::cm, 0.55*units::cm ); 
  this->add_volume( 27, "BPSA", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("BMSK_solid"), 0.0*units::degree, 360.0*units::degree, {4.95*units::cm, 4.25*units::cm, 4.1*units::cm, 5.0*units::cm}, {5.2*units::cm, 5.2*units::cm, 5.2*units::cm, 5.2*units::cm}, {230.25*units::cm, 232.25*units::cm, 242.25*units::cm, 243.15*units::cm} ); 
  this->add_volume( 28, "BMSK", "TUNGSTEN_____MED", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("BSH1_solid"), 0.0*units::degree, 360.0*units::degree, {6.0*units::cm, 6.0*units::cm}, {6.3*units::cm, 7.9*units::cm}, {0.0*units::cm, 37.3*units::cm} ); 
  this->add_volume( 29, "BSH1", "TUNGSTEN_____MED", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("BSH2_solid"), 0.0*units::degree, 360.0*units::degree, {6.0*units::cm, 6.0*units::cm}, {8.02*units::cm, 8.3*units::cm}, {0.0*units::cm, 7.5*units::cm} ); 
  this->add_volume( 30, "BSH2", "TUNGSTEN_____MED", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("BAL1_solid"), 0.0*units::degree, 360.0*units::degree, {5.3*units::cm, 5.2*units::cm, 5.2*units::cm, 5.3*units::cm}, {5.3*units::cm, 5.3*units::cm, 5.3*units::cm, 5.3*units::cm}, {229.45*units::cm, 230.25*units::cm, 231.55*units::cm, 231.85*units::cm} ); 
  this->add_volume( 31, "BAL1", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("BAL2_solid"), 0.0*units::degree, 360.0*units::degree, {5.3*units::cm, 5.2*units::cm, 5.2*units::cm, 5.3*units::cm}, {5.3*units::cm, 5.3*units::cm, 5.3*units::cm, 5.3*units::cm}, {241.42*units::cm, 241.99*units::cm, 243.34*units::cm, 243.82*units::cm} ); 
  this->add_volume( 32, "BAL2", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRH1_solid"), 7.0*units::cm, 19.0*units::cm, 0.25*units::cm ); 
  this->add_volume( 33, "BRH1", "Casserole_disk", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRH2_solid"), 7.6*units::cm, 19.5*units::cm, 0.25*units::cm ); 
  this->add_volume( 34, "BRH2", "Casserole_disk", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRH3_solid"), 8.1*units::cm, 20.55*units::cm, 0.25*units::cm ); 
  this->add_volume( 35, "BRH3", "Casserole_disk", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRH4_solid"), 8.3*units::cm, 20.55*units::cm, 0.25*units::cm ); 
  this->add_volume( 36, "BRH4", "Casserole_disk", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRA1_solid"), 16.8*units::cm, 19.0*units::cm, 0.25*units::cm ); 
  this->add_volume( 37, "BRA1", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRA2_solid"), 17.55*units::cm, 19.5*units::cm, 0.25*units::cm ); 
  this->add_volume( 38, "BRA2", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BRA3_solid"), 19.65*units::cm, 20.55*units::cm, 0.25*units::cm ); 
  this->add_volume( 39, "BRA3", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPN1_solid"), 16.2*units::cm, 20.15*units::cm, 0.25*units::cm ); 
  this->add_volume( 40, "BPN1", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPN2_solid"), 17.7*units::cm, 20.15*units::cm, 0.25*units::cm ); 
  this->add_volume( 41, "BPN2", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPAN_solid"), 20.15*units::cm, 20.55*units::cm, 12.45*units::cm ); 
  this->add_volume( 42, "BPAN", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPAM_solid"), 20.55*units::cm, 21.2*units::cm, 3.25*units::cm ); 
  this->add_volume( 43, "BPAM", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPAO_solid"), 21.2*units::cm, 25.4*units::cm, 1.5*units::cm ); 
  this->add_volume( 44, "BPAO", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPA1_solid"), 21.9*units::cm, 31.0*units::cm, 1.65*units::cm, -57.5*units::degree, -50.5*units::degree ); 
  this->add_volume( 45, "BPA1", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPA2_solid"), 21.9*units::cm, 31.0*units::cm, 1.65*units::cm, -21.5*units::degree, -14.5*units::degree ); 
  this->add_volume( 46, "BPA2", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BPA3_solid"), 21.9*units::cm, 31.0*units::cm, 1.65*units::cm, 50.5*units::degree, 57.5*units::degree ); 
  this->add_volume( 47, "BPA3", "ALU_PARTS____MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BHOL_solid"), 0.0*units::cm, 2.5*units::cm, 0.25*units::cm ); 
  this->add_volume( 48, "BHOL", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BHOB_solid"), 0.0*units::cm, 0.8*units::cm, 0.25*units::cm ); 
  this->add_volume( 49, "BHOB", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("BHOD_solid"), 0.0*units::cm, 0.75*units::cm, 0.25*units::cm ); 
  this->add_volume( 50, "BHOD", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDET_solid"), 5.65*units::cm, 12.8*units::cm, 30.945*units::cm ); 
  this->add_volume( 51, "VDET", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VTUI_solid"), 5.9*units::cm, 5.91*units::cm, 30.945*units::cm ); 
  this->add_volume( 52, "VTUI", "VDET_SHIELDING", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VTUO_solid"), 11.99*units::cm, 12.0*units::cm, 30.945*units::cm ); 
  this->add_volume( 53, "VTUO", "VDET_SHIELDING", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VTUM_solid"), 9.0*units::cm, 9.18*units::cm, 20.0*units::cm ); 
  this->add_volume( 54, "VTUM", "VDET_CYLINDER", solid, "ONLY");

  solid = dd4hep::Box( std::string("VMBA_solid"), 0.015*units::cm, 2.63*units::cm, 12.5737*units::cm ); 
  this->add_volume( 55, "VMBA", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VMSI_solid"), 0.085*units::cm, 2.63*units::cm, 12.5737*units::cm ); 
  this->add_volume( 56, "VMSI", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VFSI_solid"), 0.085*units::cm, 2.63*units::cm, 25.1475*units::cm ); 
  this->add_volume( 57, "VFSI", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VFPB_solid"), 0.175*units::cm, 2.33*units::cm, 0.1*units::cm ); 
  this->add_volume( 58, "VFPB", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VPBS_solid"), 0.0*units::cm, 0.1*units::cm, 0.175*units::cm ); 
  this->add_volume( 59, "VPBS", "VDET_PLASTIC", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VPBO_solid"), 0.0*units::cm, 0.1*units::cm, 0.1*units::cm ); 
  this->add_volume( 60, "VPBO", "VDET_PLASTIC", solid, "ONLY");

  solid = dd4hep::Box( std::string("VBMG_solid"), 0.015*units::cm, 2.63*units::cm, 0.0037*units::cm ); 
  this->add_volume( 61, "VBMG", "VDET_GLUE", solid, "ONLY");

  solid = dd4hep::Box( std::string("VBGL_solid"), 0.015*units::cm, 2.63*units::cm, 0.0025*units::cm ); 
  this->add_volume( 62, "VBGL", "VDET_GLUE", solid, "ONLY");

  solid = dd4hep::Box( std::string("VBCE_solid"), 0.015*units::cm, 2.63*units::cm, 2.6*units::cm ); 
  this->add_volume( 63, "VBCE", "VDET_CERAMIC", solid, "ONLY");

  solid = dd4hep::Box( std::string("VBQU_solid"), 0.015*units::cm, 2.63*units::cm, 0.15*units::cm ); 
  this->add_volume( 64, "VBQU", "VDET_QUARTZ", solid, "ONLY");

  solid = dd4hep::Box( std::string("VBSI_solid"), 0.015*units::cm, 2.63*units::cm, 9.8175*units::cm ); 
  this->add_volume( 65, "VBSI", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VSWA_solid"), 0.015*units::cm, 2.63*units::cm, 3.27*units::cm ); 
  this->add_volume( 66, "VSWA", "VDET_SILICON", solid, "ONLY");

  solid = dd4hep::Box( std::string("VMES_solid"), 0.07*units::cm, 2.63*units::cm, 2.0*units::cm ); 
  this->add_volume( 67, "VMES", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VMEO_solid"), 0.17*units::cm, 2.63*units::cm, 2.0*units::cm ); 
  this->add_volume( 68, "VMEO", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VOUP_solid"), 0.0075*units::cm, 2.595*units::cm, 21.04*units::cm ); 
  this->add_volume( 69, "VOUP", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VUPC_solid"), 0.0075*units::cm, 2.355*units::cm, 19.57*units::cm ); 
  this->add_volume( 70, "VUPC", "VDET_UPILEXGLUE", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("VUPM_solid"), 2.595*units::cm, 2.355*units::cm, 0.0075*units::cm, 0.275*units::cm ); 
  this->add_volume( 71, "VUPM", "VDET_UPILEXGLUE", solid, "ONLY");

  solid = dd4hep::Box( std::string("VUPE_solid"), 0.0075*units::cm, 2.595*units::cm, 0.46*units::cm ); 
  this->add_volume( 72, "VUPE", "VDET_UPILEXGLUE", solid, "ONLY");

  solid = dd4hep::Box( std::string("VOKE_solid"), 0.37*units::cm, 1.95*units::cm, 20.04*units::cm ); 
  this->add_volume( 73, "VOKE", "VDET_AIR", solid, "MANY");

  solid = dd4hep::Box( std::string("VKGL_solid"), 0.035*units::cm, 0.34*units::cm, 20.04*units::cm ); 
  this->add_volume( 74, "VKGL", "VDET_KEVLARGLUE", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("VKEV_solid"), 0.64*units::cm, 1.35*units::cm, 20.04*units::cm, 0.37*units::cm ); 
  this->add_volume( 75, "VKEV", "VDET_KEVLAR", solid, "MANY");

  solid = dd4hep::Trd1( std::string("VKAI_solid"), 0.6*units::cm, 1.27*units::cm, 20.04*units::cm, 0.34*units::cm ); 
  this->add_volume( 76, "VKAI", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VOPI_solid"), 0.3925*units::cm, 2.43*units::cm, 0.35*units::cm ); 
  this->add_volume( 77, "VOPI", "VDET_AIR", solid, "MANY");

  solid = dd4hep::Box( std::string("VPSU_solid"), 0.1025*units::cm, 2.43*units::cm, 0.35*units::cm ); 
  this->add_volume( 78, "VPSU", "VDET_BEOGLUE", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("VPGZ_solid"), 1.4*units::cm, 1.65*units::cm, 0.35*units::cm, 0.29*units::cm ); 
  this->add_volume( 79, "VPGZ", "VDET_BEO", solid, "ONLY");

  solid = dd4hep::Box( std::string("VEH1_solid"), 0.045*units::cm, 2.63*units::cm, 1.4*units::cm ); 
  this->add_volume( 80, "VEH1", "VDET_HYBRID10", solid, "ONLY");

  solid = dd4hep::Box( std::string("VEH2_solid"), 0.045*units::cm, 2.63*units::cm, 0.6*units::cm ); 
  this->add_volume( 81, "VEH2", "VDET_HYBRID11", solid, "ONLY");

  solid = dd4hep::Box( std::string("VECA_solid"), 0.025*units::cm, 2.63*units::cm, 0.65*units::cm ); 
  this->add_volume( 82, "VECA", "VDET_CAPACITOR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VEMX_solid"), 0.015*units::cm, 2.63*units::cm, 0.3215*units::cm ); 
  this->add_volume( 83, "VEMX", "VDET_MX7", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VEPO_solid"), 0.0*units::cm, 0.1*units::cm, 0.125*units::cm ); 
  this->add_volume( 84, "VEPO", "VDET_PLASTIC", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VEPS_solid"), 0.0*units::cm, 0.1*units::cm, 0.025*units::cm ); 
  this->add_volume( 85, "VEPS", "VDET_PLASTIC", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDAI_solid"), 7.2*units::cm, 9.58*units::cm, 0.3*units::cm ); 
  this->add_volume( 86, "VDAI", "VDET_AIR", solid, "MANY");

  solid = dd4hep::Tube( std::string("VDKI_solid"), 7.2*units::cm, 9.58*units::cm, 0.3*units::cm ); 
  this->add_volume( 87, "VDKI", "VDET_PITCH", solid, "MANY");

  solid = dd4hep::Tube( std::string("VDF1_solid"), 7.5*units::cm, 9.6*units::cm, 0.1*units::cm ); 
  this->add_volume( 88, "VDF1", "VDET_CSFLANGE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDF2_solid"), 11.7*units::cm, 12.0*units::cm, 0.2*units::cm ); 
  this->add_volume( 89, "VDF2", "VDET_CSFLANGE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDF3_solid"), 5.9*units::cm, 6.2*units::cm, 0.2*units::cm ); 
  this->add_volume( 90, "VDF3", "VDET_CSFLANGE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDTU_solid"), 8.35*units::cm, 9.1*units::cm, 0.25*units::cm ); 
  this->add_volume( 91, "VDTU", "VDET_WATER", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDC1_solid"), 7.0*units::cm, 7.2*units::cm, 1.2*units::cm ); 
  this->add_volume( 92, "VDC1", "VDET_CSCARD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("VDC2_solid"), 9.1*units::cm, 9.3*units::cm, 1.2*units::cm ); 
  this->add_volume( 93, "VDC2", "VDET_CSCARD", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("VRAO_solid"), 0.4423*units::cm, 0.05*units::cm, 30.945*units::cm, 0.1962*units::cm ); 
  this->add_volume( 94, "VRAO", "VDET_RAIL", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("VRAI_solid"), 0.3009*units::cm, 0.0*units::cm, 30.945*units::cm, 0.1505*units::cm ); 
  this->add_volume( 95, "VRAI", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Box( std::string("VWSS_solid"), 0.0015*units::cm, 2.63*units::cm, 3.27*units::cm ); 
  this->add_volume( 96, "VWSS", "VDET_SILICON", solid, "ONLY");

  solid = dd4hep::Box( std::string("VLOC_solid"), 1.25*units::cm, 0.4*units::cm, 3.6*units::cm ); 
  this->add_volume( 97, "VLOC", "VDET_AIR", solid, "ONLY");

  solid = dd4hep::Trap( std::string("VLTR_solid"), 3.6*units::cm, 0.0868*units::degree, 0.0*units::degree, 0.4*units::degree, 1.25*units::cm, 1.25*units::cm, 0.0*units::degree, 0.4*units::degree, 0.625*units::cm, 0.625*units::cm, 0.0*units::degree ); 
  this->add_volume( 98, "VLTR", "VDET_FOOT", solid, "ONLY");

  solid = dd4hep::Box( std::string("VLBX_solid"), 0.5*units::cm, 1.0*units::cm, 0.25*units::cm ); 
  this->add_volume( 99, "VLBX", "VDET_LOCK", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITCR_solid"), 12.8*units::cm, 31.0*units::cm, 136.1*units::cm ); 
  this->add_volume( 100, "ITCR", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITND_solid"), 12.8*units::cm, 31.0*units::cm, 18.05*units::cm ); 
  this->add_volume( 101, "ITND", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITC_solid"), 12.8*units::cm, 28.819*units::cm, 100.0*units::cm ); 
  this->add_volume( 102, "ITC", "ITC_sensitive", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITWI_solid"), 12.8*units::cm, 12.8695*units::cm, 100.0*units::cm ); 
  this->add_volume( 103, "ITWI", "ITC_inner_wall", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITWO_solid"), 28.6*units::cm, 28.819*units::cm, 100.0*units::cm ); 
  this->add_volume( 104, "ITWO", "ITC_outer_wall", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITEP_solid"), 12.8*units::cm, 28.8*units::cm, 1.25*units::cm ); 
  this->add_volume( 105, "ITEP", "ITC_end_plates", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITRG_solid"), 28.8*units::cm, 30.3*units::cm, 2.75*units::cm ); 
  this->add_volume( 106, "ITRG", "ITC_end_plates", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITEL_solid"), 14.35*units::cm, 28.5*units::cm, 16.8*units::cm ); 
  this->add_volume( 107, "ITEL", "ITC_electronics", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ITSP_solid"), 14.2*units::cm, 28.8*units::cm, 16.8*units::cm ); 
  this->add_volume( 108, "ITSP", "ITC_end_plates", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ITCA_solid"), 0.0*units::degree, 360.0*units::degree, {14.35*units::cm, 25.0*units::cm, 25.0*units::cm}, {28.5*units::cm, 28.5*units::cm, 28.5*units::cm}, {136.1*units::cm, 146.1*units::cm, 232.0*units::cm} ); 
  this->add_volume( 109, "ITCA", "ITC_cables", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ITSU_solid"), 0.0*units::degree, 360.0*units::degree, {14.2*units::cm, 24.85*units::cm, 24.85*units::cm}, {28.8*units::cm, 28.8*units::cm, 28.8*units::cm}, {136.1*units::cm, 146.1*units::cm, 232.0*units::cm} ); 
  this->add_volume( 110, "ITSU", "ITC_end_plates", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPCR_solid"), 31.0*units::cm, 184.7*units::cm, 240.0*units::cm ); 
  this->add_volume( 111, "TPCR", "AIR_INSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPC_solid"), 31.68*units::cm, 177.9*units::cm, 220.0*units::cm ); 
  this->add_volume( 112, "TPC", "TPC_GAS_medium", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPHF_solid"), 31.68*units::cm, 177.9*units::cm, 110.0*units::cm ); 
  this->add_volume( 113, "TPHF", "TPC_GAS_medium", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPWI_solid"), 31.0*units::cm, 31.68*units::cm, 220.0*units::cm ); 
  this->add_volume( 114, "TPWI", "TPC_inner_wall_med", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPWO_solid"), 177.9*units::cm, 180.1*units::cm, 220.0*units::cm ); 
  this->add_volume( 115, "TPWO", "TPC_OUTER_WALL_med", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPIS_solid"), 31.68*units::cm, 31.78*units::cm, 9.85*units::cm ); 
  this->add_volume( 116, "TPIS", "TPC_INNERCAGE_SUPPOR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPMI_solid"), 31.68*units::cm, 33.68*units::cm, 0.2*units::cm ); 
  this->add_volume( 117, "TPMI", "TPC_Mylar_inner_sup", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPMO_solid"), 176.9*units::cm, 177.9*units::cm, 0.2*units::cm ); 
  this->add_volume( 118, "TPMO", "TPC_Mylar_outer_sup", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPMB_solid"), 36.4*units::cm, 175.4*units::cm, 0.0006*units::cm ); 
  this->add_volume( 119, "TPMB", "TPC_MYLAR_MEMBRANE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPMG_solid"), 31.68*units::cm, 33.68*units::cm, 0.059*units::cm ); 
  this->add_volume( 120, "TPMG", "TPC_G10_reinf_ring", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPRC_solid"), 31.68*units::cm, 33.03*units::cm, 99.891*units::cm, 266.0014*units::degree, 273.9998*units::degree ); 
  this->add_volume( 121, "TPRC", "TPC_resistor_chain", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPR1_solid"), 31.68*units::cm, 34.68*units::cm, 1.5*units::cm, 80.0021*units::degree, 88.0005*units::degree ); 
  this->add_volume( 122, "TPR1", "TPC_prisms", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPR2_solid"), 31.68*units::cm, 34.68*units::cm, 1.5*units::cm, 200.0021*units::degree, 208.0005*units::degree ); 
  this->add_volume( 123, "TPR2", "TPC_prisms", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPR3_solid"), 31.68*units::cm, 34.68*units::cm, 1.5*units::cm, 320.002*units::degree, 328.0005*units::degree ); 
  this->add_volume( 124, "TPR3", "TPC_prisms", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM11_solid"), 31.68*units::cm, 34.68*units::cm, 4.0*units::cm, 80.0021*units::degree, 88.0005*units::degree ); 
  this->add_volume( 125, "TM11", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM12_solid"), 31.68*units::cm, 34.68*units::cm, 4.0*units::cm, 200.0021*units::degree, 208.0005*units::degree ); 
  this->add_volume( 126, "TM12", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM13_solid"), 31.68*units::cm, 34.68*units::cm, 4.0*units::cm, 320.002*units::degree, 328.0005*units::degree ); 
  this->add_volume( 127, "TM13", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM21_solid"), 31.68*units::cm, 34.68*units::cm, 4.5*units::cm, 80.0021*units::degree, 88.0005*units::degree ); 
  this->add_volume( 128, "TM21", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM22_solid"), 31.68*units::cm, 34.68*units::cm, 4.5*units::cm, 200.0021*units::degree, 208.0005*units::degree ); 
  this->add_volume( 129, "TM22", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM23_solid"), 31.68*units::cm, 34.68*units::cm, 4.5*units::cm, 320.002*units::degree, 328.0005*units::degree ); 
  this->add_volume( 130, "TM23", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM31_solid"), 31.68*units::cm, 34.68*units::cm, 5.0*units::cm, 80.0021*units::degree, 88.0005*units::degree ); 
  this->add_volume( 131, "TM31", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM32_solid"), 31.68*units::cm, 34.68*units::cm, 5.0*units::cm, 200.0021*units::degree, 208.0005*units::degree ); 
  this->add_volume( 132, "TM32", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM33_solid"), 31.68*units::cm, 34.68*units::cm, 5.0*units::cm, 320.002*units::degree, 328.0005*units::degree ); 
  this->add_volume( 133, "TM33", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM41_solid"), 31.68*units::cm, 34.68*units::cm, 6.0*units::cm, 80.0021*units::degree, 88.0005*units::degree ); 
  this->add_volume( 134, "TM41", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM42_solid"), 31.68*units::cm, 34.68*units::cm, 6.0*units::cm, 200.0021*units::degree, 208.0005*units::degree ); 
  this->add_volume( 135, "TM42", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TM43_solid"), 31.68*units::cm, 34.68*units::cm, 6.0*units::cm, 320.002*units::degree, 328.0005*units::degree ); 
  this->add_volume( 136, "TM43", "TPC_mirrors", solid, "ONLY");

  solid = dd4hep::Tube( std::string("TPEP_solid"), 31.0*units::cm, 180.1*units::cm, 10.0*units::cm ); 
  this->add_volume( 137, "TPEP", "TP_ENDPLATE_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ECBL_solid"), 184.7*units::cm, 248.0*units::cm, 240.0*units::cm ); 
  this->add_volume( 138, "ECBL", "EC_BARREL_REGION", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ECEA_solid"), 0.0*units::degree, 360.0*units::degree, {5.65*units::cm, 5.65*units::cm, 8.5*units::cm, 8.5*units::cm}, {248.0*units::cm, 248.0*units::cm, 248.0*units::cm, 248.0*units::cm}, {240.0*units::cm, 269.5*units::cm, 269.501*units::cm, 315.0*units::cm} ); 
  this->add_volume( 139, "ECEA", "EC_ENDCAP_REGION", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ECEB_solid"), 0.0*units::degree, 360.0*units::degree, {5.65*units::cm, 5.65*units::cm, 8.5*units::cm, 8.5*units::cm}, {248.0*units::cm, 248.0*units::cm, 248.0*units::cm, 248.0*units::cm}, {240.0*units::cm, 269.5*units::cm, 269.501*units::cm, 315.0*units::cm} ); 
  this->add_volume( 140, "ECEB", "EC_ENDCAP_REGION", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EBAL_solid"), 184.7*units::cm, 248.0*units::cm, 238.7*units::cm ); 
  this->add_volume( 141, "EBAL", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EBAR_solid"), 184.7*units::cm, 248.0*units::cm, 238.7*units::cm, -15.0*units::degree, 15.0*units::degree ); 
  this->add_volume( 142, "EBAR", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBMO_solid"), 49.2314*units::cm, 61.2194*units::cm, 238.7*units::cm, 22.37*units::cm ); 
  this->add_volume( 143, "EBMO", "EC_ALU_FRAMES", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBRA_solid"), 10.3696*units::cm, 9.0345*units::cm, 212.0*units::cm, 9.28*units::cm ); 
  this->add_volume( 144, "EBRA", "EC_ALU_FRAMES", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBIN_solid"), 49.4835*units::cm, 59.837*units::cm, 238.4*units::cm, 19.32*units::cm ); 
  this->add_volume( 145, "EBIN", "ECBL_AV._PASSIVE_MED", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBND_solid"), 49.4835*units::cm, 59.837*units::cm, 3.45*units::cm, 19.32*units::cm ); 
  this->add_volume( 146, "EBND", "EC_BL_ENDPART_MEDIUM", solid, "ONLY");

  solid = dd4hep::Box( std::string("EBEP_solid"), 32.5*units::cm, 1.1*units::cm, 19.32*units::cm ); 
  this->add_volume( 147, "EBEP", "EC_ALU_FRAMES", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBSP_solid"), 50.8051*units::cm, 50.939*units::cm, 231.5*units::cm, 0.25*units::cm ); 
  this->add_volume( 148, "EBSP", "PVC_STACK_SEPARATOR", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBS1_solid"), 48.707*units::cm, 50.8051*units::cm, 231.5*units::cm, 3.915*units::cm ); 
  this->add_volume( 149, "EBS1", "EC_STACK_1+2_BARREL", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBS2_solid"), 50.939*units::cm, 55.7648*units::cm, 231.5*units::cm, 9.005*units::cm ); 
  this->add_volume( 150, "EBS2", "EC_STACK_1+2_BARREL", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EBS3_solid"), 55.9312*units::cm, 58.543*units::cm, 231.5*units::cm, 5.9*units::cm ); 
  this->add_volume( 151, "EBS3", "EC_STACK_3_BARREL", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ECPA_solid"), 0.0*units::degree, 360.0*units::degree, {54.0*units::cm, 54.0*units::cm}, {243.0829*units::cm, 243.0829*units::cm}, {250.5*units::cm, 306.75*units::cm} ); 
  this->add_volume( 152, "ECPA", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ECPB_solid"), 0.0*units::degree, 360.0*units::degree, {54.0*units::cm, 54.0*units::cm}, {243.0829*units::cm, 243.0829*units::cm}, {250.5*units::cm, 306.75*units::cm} ); 
  this->add_volume( 153, "ECPB", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ECMA_solid"), -15.0*units::degree, 30.0*units::degree, {54.0*units::cm, 54.0*units::cm}, {243.0829*units::cm, 243.0829*units::cm}, {250.5*units::cm, 306.75*units::cm} ); 
  this->add_volume( 154, "ECMA", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("ECMB_solid"), -15.0*units::degree, 30.0*units::degree, {54.0*units::cm, 54.0*units::cm}, {243.0829*units::cm, 243.0829*units::cm}, {250.5*units::cm, 306.75*units::cm} ); 
  this->add_volume( 155, "ECMB", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECMO_solid"), 14.2622*units::cm, 62.7074*units::cm, 28.125*units::cm, 90.4*units::cm ); 
  this->add_volume( 156, "ECMO", "EC_ALU_FRAMES", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECFX_solid"), 11.9602*units::cm, 58.7977*units::cm, 1.12*units::cm, 87.4*units::cm ); 
  this->add_volume( 157, "ECFX", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECBX_solid"), 9.6582*units::cm, 54.888*units::cm, 1.75*units::cm, 84.4*units::cm ); 
  this->add_volume( 158, "ECBX", "EC_BARREL_VOLUME", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECIN_solid"), 13.7749*units::cm, 61.8798*units::cm, 20.805*units::cm, 89.765*units::cm ); 
  this->add_volume( 159, "ECIN", "EC_LEFT_PASSIVE_MIX", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECRB_solid"), 13.7749*units::cm, 61.8798*units::cm, 0.25*units::cm, 89.765*units::cm ); 
  this->add_volume( 160, "ECRB", "EC_ENDCAP_RESINE_BAG", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECSP_solid"), 12.476*units::cm, 56.0628*units::cm, 0.3175*units::cm, 81.334*units::cm ); 
  this->add_volume( 161, "ECSP", "ALU_STACK_SEPARATOR", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECS0_solid"), 12.476*units::cm, 56.0628*units::cm, 0.416*units::cm, 81.334*units::cm ); 
  this->add_volume( 162, "ECS0", "EC_STACK_0_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EDS0_solid"), 56.0628*units::cm, 26.0871*units::cm, 0.416*units::cm, 4.016*units::cm ); 
  this->add_volume( 163, "EDS0", "EC_STACK_0_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECS1_solid"), 12.476*units::cm, 56.0628*units::cm, 3.744*units::cm, 81.334*units::cm ); 
  this->add_volume( 164, "ECS1", "EC_STACK_1+2_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EDS1_solid"), 56.0628*units::cm, 26.0871*units::cm, 3.744*units::cm, 4.016*units::cm ); 
  this->add_volume( 165, "EDS1", "EC_STACK_1+2_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECS2_solid"), 12.476*units::cm, 56.0628*units::cm, 9.568*units::cm, 81.334*units::cm ); 
  this->add_volume( 166, "ECS2", "EC_STACK_1+2_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EDS2_solid"), 56.0628*units::cm, 26.0871*units::cm, 9.568*units::cm, 4.016*units::cm ); 
  this->add_volume( 167, "EDS2", "EC_STACK_1+2_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECS3_solid"), 12.476*units::cm, 56.0628*units::cm, 6.192*units::cm, 81.334*units::cm ); 
  this->add_volume( 168, "ECS3", "EC_STACK_3_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("EDS3_solid"), 56.0628*units::cm, 26.0871*units::cm, 6.192*units::cm, 4.016*units::cm ); 
  this->add_volume( 169, "EDS3", "EC_STACK_3_ENDCAP", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECNE_solid"), 13.7749*units::cm, 13.7749*units::cm, 20.555*units::cm, 1.0825*units::cm ); 
  this->add_volume( 170, "ECNE", "EC_NARROW_PASSIVMIX", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPBA_solid"), 180.0*units::cm, 248.0*units::cm, 4.4*units::cm ); 
  this->add_volume( 171, "EPBA", "PASS._MAT._BETW.MODU", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPBB_solid"), 180.0*units::cm, 248.0*units::cm, 4.4*units::cm ); 
  this->add_volume( 172, "EPBB", "PASS._MAT._BETW.MODU", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPCA_solid"), 243.0829*units::cm, 248.0*units::cm, 33.1*units::cm, 12.6899*units::degree, 17.3102*units::degree ); 
  this->add_volume( 173, "EPCA", "PASS._MAT._BETW.MODU", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPAA_solid"), 243.0829*units::cm, 248.0*units::cm, 33.1*units::cm ); 
  this->add_volume( 174, "EPAA", "ELEC._BOX_ABOVE_ECAL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPAB_solid"), 243.0829*units::cm, 248.0*units::cm, 33.1*units::cm ); 
  this->add_volume( 175, "EPAB", "ELEC._BOX_ABOVE_ECAL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPBX_solid"), 210.0*units::cm, 248.0*units::cm, 25.2*units::cm ); 
  this->add_volume( 176, "EPBX", "ELEC_BOX_IN_NONUNI_B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ETHF_solid"), 180.0*units::cm, 248.0*units::cm, 4.25*units::cm, -4.3522*units::degree, 4.3522*units::degree ); 
  this->add_volume( 177, "ETHF", "TPC_HORIZONTAL_FOOT", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ETVF_solid"), 180.0*units::cm, 248.0*units::cm, 4.25*units::cm, 85.6474*units::degree, 94.3524*units::degree ); 
  this->add_volume( 178, "ETVF", "TPC_VERTICAL_FOOT", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ETCA_solid"), 180.0*units::cm, 248.0*units::cm, 1.65*units::cm, 4.3522*units::degree, 9.9998*units::degree ); 
  this->add_volume( 179, "ETCA", "TPC_CABLES_IN_UNI._B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("ETCB_solid"), 233.0*units::cm, 248.0*units::cm, 25.2*units::cm, 24.7478*units::degree, 35.2518*units::degree ); 
  this->add_volume( 180, "ETCB", "TPC_CABLES_NON_UNI_B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EICA_solid"), 243.0829*units::cm, 248.0*units::cm, 33.1*units::cm, 144.748*units::degree, 155.252*units::degree ); 
  this->add_volume( 181, "EICA", "ITC_CABLES_IN_UNI._B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EICB_solid"), 233.0*units::cm, 248.0*units::cm, 25.2*units::cm, 144.748*units::degree, 155.252*units::degree ); 
  this->add_volume( 182, "EICB", "ITC_CABLES_NON_UNI_B", solid, "ONLY");

  solid = dd4hep::Trd1( std::string("ECSU_solid"), 11.4611*units::cm, 35.9701*units::cm, 1.75*units::cm, 69.5*units::cm ); 
  this->add_volume( 183, "ECSU", "END_CAP_SUPPORT_PLAT", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCEA_solid"), 8.5*units::cm, 52.4*units::cm, 22.65*units::cm ); 
  this->add_volume( 184, "LCEA", "LC_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCEB_solid"), 8.5*units::cm, 52.4*units::cm, 22.65*units::cm ); 
  this->add_volume( 185, "LCEB", "LC_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCSH_solid"), 9.8*units::cm, 10.0*units::cm, 22.65*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 186, "LCSH", "LC_LEAD_SHIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("LSCP_solid"), 2.5*units::cm, 12.85*units::cm, 0.55*units::cm ); 
  this->add_volume( 187, "LSCP", "LC_W_absorber", solid, "ONLY");

  solid = dd4hep::Box( std::string("LSCI_solid"), 2.3*units::cm, 13.3*units::cm, 0.5*units::cm ); 
  this->add_volume( 188, "LSCI", "LC_NE110_Scint.", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCMO_solid"), 10.0*units::cm, 52.0*units::cm, 22.65*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 189, "LCMO", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Box( std::string("LSBR_solid"), 0.3*units::cm, 15.165*units::cm, 1.85*units::cm ); 
  this->add_volume( 190, "LSBR", "LCAL_INOX_SCREWS_ARE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LSFR_solid"), 13.7*units::cm, 14.3*units::cm, 1.85*units::cm, -65.426*units::degree, 65.426*units::degree ); 
  this->add_volume( 191, "LSFR", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LSCR_solid"), 0.0*units::cm, 0.3*units::cm, 1.85*units::cm ); 
  this->add_volume( 192, "LSCR", "LCAL_INOX_SCREWS_MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCIN_solid"), 11.0*units::cm, 51.2*units::cm, 19.0*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 193, "LCIN", "LC_PASSIVE_GAS_MIX", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCBP_solid"), 10.0*units::cm, 52.0*units::cm, 1.5*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 194, "LCBP", "LC_BACK_PLATE_VOLUME", solid, "ONLY");

  solid = dd4hep::Box( std::string("LCSI_solid"), 0.5*units::cm, 0.25*units::cm, 19.0*units::cm ); 
  this->add_volume( 195, "LCSI", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Box( std::string("LCSO_solid"), 0.5*units::cm, 0.3*units::cm, 19.0*units::cm ); 
  this->add_volume( 196, "LCSO", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Box( std::string("LCSW_solid"), 0.5*units::cm, 19.55*units::cm, 12.6585*units::cm ); 
  this->add_volume( 197, "LCSW", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Box( std::string("LCSG_solid"), 0.25*units::cm, 19.55*units::cm, 12.6585*units::cm ); 
  this->add_volume( 198, "LCSG", "LC_PASSIVE_GAS_MIX", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCDL_solid"), 0.0*units::cm, 2.0*units::cm, 12.6585*units::cm ); 
  this->add_volume( 199, "LCDL", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Trap( std::string("LEAD_solid"), 12.6585*units::cm, 0.0*units::degree, 0.0*units::degree, 3.125*units::degree, 1.0*units::cm, 0.0001*units::cm, 0.16*units::degree, 3.125*units::degree, 1.0*units::cm, 0.0001*units::cm, 0.16*units::degree ); 
  this->add_volume( 200, "LEAD", "LC_PASSIVE_GAS_MIX", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LC12_solid"), 11.5*units::cm, 50.6*units::cm, 12.6585*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 201, "LC12", "LC_STACK1+2_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCS3_solid"), 11.5*units::cm, 50.6*units::cm, 5.1885*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 202, "LCS3", "LC_STACK3_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("LCCA_solid"), 11.5*units::cm, 50.6*units::cm, 1.155*units::cm, -90.0*units::degree, 90.0*units::degree ); 
  this->add_volume( 203, "LCCA", "LC_WALLS_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMBA_solid"), 5.85*units::cm, 21.0*units::cm, 1.6*units::cm ); 
  this->add_volume( 204, "SMBA", "SAMBA_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMCH_solid"), 5.85*units::cm, 21.0*units::cm, 0.7845*units::cm ); 
  this->add_volume( 205, "SMCH", "SAMBA_Vetronite", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMIN_solid"), 5.85*units::cm, 13.2*units::cm, 0.48*units::cm ); 
  this->add_volume( 206, "SMIN", "SAMBA_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMGA_solid"), 6.3*units::cm, 13.2*units::cm, 0.25*units::cm ); 
  this->add_volume( 207, "SMGA", "SAMBA_GAS", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMCU_solid"), 6.3*units::cm, 13.2*units::cm, 0.0015*units::cm ); 
  this->add_volume( 208, "SMCU", "SAMBA_copper", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMCP_solid"), 6.3*units::cm, 21.0*units::cm, 0.0015*units::cm ); 
  this->add_volume( 209, "SMCP", "SAMBA_copper", solid, "ONLY");

  solid = dd4hep::Box( std::string("SMEL_solid"), 0.8*units::cm, 5.0*units::cm, 6.5*units::cm ); 
  this->add_volume( 210, "SMEL", "SAMBA_electronic", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCAL_solid"), 5.76*units::cm, 52.0*units::cm, 8.75*units::cm ); 
  this->add_volume( 211, "SCAL", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("STUB_solid"), 5.76*units::cm, 6.0*units::cm, 7.17*units::cm ); 
  this->add_volume( 212, "STUB", "SCAL_LEAD_TUBE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("STOP_solid"), 6.0*units::cm, 52.0*units::cm, 8.75*units::cm ); 
  this->add_volume( 213, "STOP", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SABP_solid"), 6.0*units::cm, 45.0*units::cm, 0.29*units::cm ); 
  this->add_volume( 214, "SABP", "SCAL_AL_PLATE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SBOX_solid"), 6.0*units::cm, 45.0*units::cm, 6.206*units::cm ); 
  this->add_volume( 215, "SBOX", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SAFR_solid"), 6.0*units::cm, 45.0*units::cm, 2.254*units::cm ); 
  this->add_volume( 216, "SAFR", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SLUM_solid"), 6.0*units::cm, 25.4*units::cm, 6.206*units::cm ); 
  this->add_volume( 217, "SLUM", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SSUP_solid"), 25.4*units::cm, 45.0*units::cm, 6.206*units::cm ); 
  this->add_volume( 218, "SSUP", "SCAL_CABLES_AREA", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCOO_solid"), 25.9*units::cm, 26.7*units::cm, 6.056*units::cm, 127.0*units::degree, 233.0*units::degree ); 
  this->add_volume( 219, "SCOO", "SCAL_COOLING_VOL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCO1_solid"), 26.7*units::cm, 33.3*units::cm, 6.056*units::cm, 51.7*units::degree, 54.3*units::degree ); 
  this->add_volume( 220, "SCO1", "SCAL_COOLING_VOL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCO2_solid"), 26.7*units::cm, 33.3*units::cm, 6.056*units::cm, 125.7*units::degree, 128.3*units::degree ); 
  this->add_volume( 221, "SCO2", "SCAL_COOLING_VOL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCO3_solid"), 26.7*units::cm, 33.3*units::cm, 6.056*units::cm, 231.7*units::degree, 234.3*units::degree ); 
  this->add_volume( 222, "SCO3", "SCAL_COOLING_VOL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCO4_solid"), 26.7*units::cm, 33.3*units::cm, 6.066*units::cm, 305.7*units::degree, 308.2998*units::degree ); 
  this->add_volume( 223, "SCO4", "SCAL_COOLING_VOL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCPF_solid"), 25.4*units::cm, 42.5*units::cm, 0.15*units::cm ); 
  this->add_volume( 224, "SCPF", "SCAL_AL_PLATE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCPS_solid"), 42.2*units::cm, 42.45*units::cm, 5.721*units::cm ); 
  this->add_volume( 225, "SCPS", "SCAL_AL_PLATE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCPB_solid"), 42.2*units::cm, 44.0*units::cm, 0.335*units::cm ); 
  this->add_volume( 226, "SCPB", "SCAL_AL_PLATE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SACT_solid"), 6.0*units::cm, 14.6*units::cm, 6.206*units::cm ); 
  this->add_volume( 227, "SACT", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SPAS_solid"), 14.6*units::cm, 25.4*units::cm, 6.206*units::cm ); 
  this->add_volume( 228, "SPAS", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SFIR_solid"), 6.0*units::cm, 14.6*units::cm, 0.16*units::cm ); 
  this->add_volume( 229, "SFIR", "SCAL_THIN__ABSBR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SLAS_solid"), 6.0*units::cm, 14.6*units::cm, 0.225*units::cm ); 
  this->add_volume( 230, "SLAS", "SCAL_THICK_ABSBR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SLLA_solid"), 6.0*units::cm, 14.6*units::cm, 0.18*units::cm ); 
  this->add_volume( 231, "SLLA", "SCAL_UTHIN_ABSBR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SWFR_solid"), 6.0*units::cm, 14.6*units::cm, 0.15*units::cm ); 
  this->add_volume( 232, "SWFR", "SCAL_FIRST_ABSBR", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMOD_solid"), 6.0*units::cm, 14.6*units::cm, 0.503*units::cm ); 
  this->add_volume( 233, "SMOD", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMOL_solid"), 6.0*units::cm, 14.6*units::cm, 0.523*units::cm ); 
  this->add_volume( 234, "SMOL", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMDX_solid"), 14.6*units::cm, 25.4*units::cm, 5.533*units::cm ); 
  this->add_volume( 235, "SMDX", "SCAL_PASSIVE_AV1", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMDA_solid"), 17.0*units::cm, 22.8*units::cm, 5.533*units::cm ); 
  this->add_volume( 236, "SMDA", "SCAL_PASSIVE_AV2", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMDC_solid"), 22.8*units::cm, 25.4*units::cm, 5.533*units::cm ); 
  this->add_volume( 237, "SMDC", "SCAL_PASSIVE_AV3", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SMDL_solid"), 14.6*units::cm, 25.4*units::cm, 0.523*units::cm ); 
  this->add_volume( 238, "SMDL", "SCAL_LAST_PASAVG", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SINX_solid"), 0.0*units::cm, 0.825*units::cm, 5.533*units::cm ); 
  this->add_volume( 239, "SINX", "SCAL_goupilles", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SINO_solid"), 0.0*units::cm, 0.825*units::cm, 0.523*units::cm ); 
  this->add_volume( 240, "SINO", "SCAL_goupilles", solid, "ONLY");

  solid = dd4hep::Box( std::string("SEQU_solid"), 0.05*units::cm, 9.5*units::cm, 0.3*units::cm ); 
  this->add_volume( 241, "SEQU", "SCAL_goupilles", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCAP_solid"), 15.6*units::cm, 25.4*units::cm, 0.15*units::cm ); 
  this->add_volume( 242, "SCAP", "SCAL_AL_PLATE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SSEN_solid"), 6.0*units::cm, 14.6*units::cm, 0.118*units::cm ); 
  this->add_volume( 243, "SSEN", "SCAL_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SSN3_solid"), 6.0*units::cm, 14.6*units::cm, 0.037*units::cm ); 
  this->add_volume( 244, "SSN3", "SCAL_G10_PLATE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SSN2_solid"), 6.0*units::cm, 14.6*units::cm, 0.037*units::cm ); 
  this->add_volume( 245, "SSN2", "SCAL_VOLUME_ACT", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SSN1_solid"), 6.0*units::cm, 14.6*units::cm, 0.044*units::cm ); 
  this->add_volume( 246, "SSN1", "SCAL_VOLUME_ACT", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SKAP_solid"), 6.7689*units::cm, 15.3689*units::cm, 0.0275*units::cm, 0.0*units::degree, 22.5*units::degree ); 
  this->add_volume( 247, "SKAP", "SCAL_KAPTON-GLUE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SCRM_solid"), 6.0*units::cm, 14.6*units::cm, 0.037*units::cm, 3.75*units::degree, 18.75*units::degree ); 
  this->add_volume( 248, "SCRM", "SCAL_CERAMICS", solid, "ONLY");

  solid = dd4hep::Tube( std::string("SSIL_solid"), 6.7689*units::cm, 15.3689*units::cm, 0.015*units::cm, 0.0*units::degree, 22.5*units::degree ); 
  this->add_volume( 249, "SSIL", "SCAL_SI_CRYSTALS", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COIL_solid"), 248.0*units::cm, 297.2998*units::cm, 365.3999*units::cm ); 
  this->add_volume( 250, "COIL", "COIL_REGION", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COBY_solid"), 248.0*units::cm, 297.2998*units::cm, 315.0*units::cm ); 
  this->add_volume( 251, "COBY", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COEN_solid"), 248.0*units::cm, 297.2998*units::cm, 25.2*units::cm ); 
  this->add_volume( 252, "COEN", "AIR_IN_NON_UNIFORM_B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COBI_solid"), 254.0*units::cm, 264.7*units::cm, 315.0*units::cm ); 
  this->add_volume( 253, "COBI", "COIL_BODY_IN_UNI_BZ", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COBO_solid"), 264.7*units::cm, 286.0*units::cm, 315.0*units::cm ); 
  this->add_volume( 254, "COBO", "COIL_BODY_OUT_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COIN_solid"), 248.0*units::cm, 254.0*units::cm, 315.0*units::cm ); 
  this->add_volume( 255, "COIN", "COIL_REINFOR_UNI_BZ", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COUT_solid"), 286.0*units::cm, 292.0*units::cm, 315.0*units::cm ); 
  this->add_volume( 256, "COUT", "COIL_REINFOR_NO_FIEL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COCO_solid"), 286.0*units::cm, 292.0*units::cm, 10.0*units::cm ); 
  this->add_volume( 257, "COCO", "COIL_RINGS_NO_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COCI_solid"), 248.0*units::cm, 254.0*units::cm, 10.0*units::cm ); 
  this->add_volume( 258, "COCI", "COIL_RINGS_IN_UNI_BZ", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COEO_solid"), 286.0*units::cm, 292.0*units::cm, 5.0*units::cm ); 
  this->add_volume( 259, "COEO", "COIL_RINGS_NON_UNI_B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COEI_solid"), 248.0*units::cm, 254.0*units::cm, 5.0*units::cm ); 
  this->add_volume( 260, "COEI", "COIL_RINGS_NON_UNI_B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COMI_solid"), 248.0*units::cm, 254.0*units::cm, 10.0*units::cm ); 
  this->add_volume( 261, "COMI", "COIL_RINGS_IN_UNI_BZ", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COMO_solid"), 286.0*units::cm, 292.0*units::cm, 10.0*units::cm ); 
  this->add_volume( 262, "COMO", "COIL_RINGS_NO_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COII_solid"), 249.5*units::cm, 254.0*units::cm, 8.5*units::cm ); 
  this->add_volume( 263, "COII", "COIL_REINFOR_UNI_BZ", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COIO_solid"), 286.0*units::cm, 290.5*units::cm, 8.5*units::cm ); 
  this->add_volume( 264, "COIO", "COIL_REINFOR_NO_FIEL", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COEP_solid"), 248.0*units::cm, 292.0*units::cm, 4.5*units::cm ); 
  this->add_volume( 265, "COEP", "COIL_ENDPL_NON_UNI_B", solid, "ONLY");

  solid = dd4hep::Tube( std::string("COBE_solid"), 254.0*units::cm, 286.0*units::cm, 13.0*units::cm ); 
  this->add_volume( 266, "COBE", "COIL_BODY_NON_UNI_B", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("QUEA_solid"), 0.0*units::degree, 360.0*units::degree, {0.0*units::cm, 0.0*units::cm}, {45.0*units::cm, 45.0*units::cm}, {315.0*units::cm, 600.0*units::cm} ); 
  this->add_volume( 267, "QUEA", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("QUEB_solid"), 0.0*units::degree, 360.0*units::degree, {0.0*units::cm, 0.0*units::cm}, {45.0*units::cm, 45.0*units::cm}, {315.0*units::cm, 600.0*units::cm} ); 
  this->add_volume( 268, "QUEB", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("QUPU_solid"), 0.0*units::cm, 28.0*units::cm, 17.5*units::cm ); 
  this->add_volume( 269, "QUPU", "PUMPS_AND_VALVES_MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("QUVA_solid"), 0.0*units::cm, 6.0*units::cm, 17.5*units::cm ); 
  this->add_volume( 270, "QUVA", "VACUUM_IN_PIPE_MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("QUVI_solid"), 0.0*units::cm, 6.0*units::cm, 125.0*units::cm ); 
  this->add_volume( 271, "QUVI", "BEAM_VACUUM__MED", solid, "ONLY");

  solid = dd4hep::Tube( std::string("QUAA_solid"), 0.0*units::cm, 27.5*units::cm, 125.0*units::cm ); 
  this->add_volume( 272, "QUAA", "QUADR_TUBE_BODY", solid, "ONLY");

  solid = dd4hep::Tube( std::string("QUAB_solid"), 0.0*units::cm, 27.5*units::cm, 125.0*units::cm ); 
  this->add_volume( 273, "QUAB", "QUADR_TUBE_BODY", solid, "ONLY");

  solid = dd4hep::Tube( std::string("HCBL_solid"), 297.2998*units::cm, 495.5999*units::cm, 365.3999*units::cm ); 
  this->add_volume( 274, "HCBL", "HADRON_BL__REGION", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCEA_solid"), 0.0*units::degree, 360.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {248.0*units::cm, 248.0*units::cm, 495.5999*units::cm, 495.5999*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.3999*units::cm, 496.7998*units::cm} ); 
  this->add_volume( 275, "HCEA", "HADRON_EC__REGION", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCEB_solid"), 0.0*units::degree, 360.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {248.0*units::cm, 248.0*units::cm, 495.5999*units::cm, 495.5999*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.3999*units::cm, 496.7998*units::cm} ); 
  this->add_volume( 276, "HCEB", "HADRON_EC__REGION", solid, "ONLY");

  solid = dd4hep::Trap( std::string("HBMO_solid"), 365.3999*units::cm, 0.0*units::degree, 0.0*units::degree, 80.55*units::degree, 39.8306*units::cm, 61.4139*units::cm, 0.1317*units::degree, 80.55*units::degree, 39.8306*units::cm, 61.4139*units::cm, 0.1317*units::degree ); 
  this->add_volume( 277, "HBMO", "HCBL_NON_SENSITIVE", solid, "ONLY");

  solid = dd4hep::Tube( std::string("HBAL_solid"), 297.2998*units::cm, 495.5999*units::cm, 365.3999*units::cm ); 
  this->add_volume( 278, "HBAL", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Tube( std::string("HBAR_solid"), 297.2998*units::cm, 495.5999*units::cm, 365.3999*units::cm, -15.0*units::degree, 15.0*units::degree ); 
  this->add_volume( 279, "HBAR", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Trap( std::string("HBME_solid"), 365.3999*units::cm, 0.0*units::degree, 0.0*units::degree, 5.0*units::degree, 61.4139*units::cm, 62.7537*units::cm, 0.1317*units::degree, 5.0*units::degree, 61.4139*units::cm, 62.7537*units::cm, 0.1317*units::degree ); 
  this->add_volume( 280, "HBME", "LAST_IRON_PLATE_MEDI", solid, "ONLY");

  solid = dd4hep::Box( std::string("HBN1_solid"), 16.5*units::cm, 80.55*units::cm, 6.5*units::cm ); 
  this->add_volume( 281, "HBN1", "CABLES_IN_NOTCHES", solid, "ONLY");

  solid = dd4hep::Solid(new TGeoPara( "HBN2_solid", 3.0*units::cm, 80.55*units::cm, 6.5*units::cm, 0.2679, 0.0, 0.0 )); 
  this->add_volume( 282, "HBN2", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Box( std::string("HBLA_solid"), 39.9512*units::cm, 0.7*units::cm, 352.3999*units::cm ); 
  this->add_volume( 283, "HBLA", "HCAL_HSTREAMER_TUBES", solid, "ONLY");

  solid = dd4hep::Box( std::string("HBL1_solid"), 20.4512*units::cm, 0.7*units::cm, 6.5*units::cm ); 
  this->add_volume( 284, "HBL1", "HCAL_HSTREAMER_TUBES", solid, "ONLY");

  solid = dd4hep::Box( std::string("HBL2_solid"), 20.4512*units::cm, 0.7*units::cm, 6.5*units::cm ); 
  this->add_volume( 285, "HBL2", "HCAL_HSTREAMER_TUBES", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCPA_solid"), 0.0*units::degree, 360.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {210.0*units::cm, 210.0*units::cm, 450.3452*units::cm, 450.3452*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.4009*units::cm, 483.3999*units::cm} ); 
  this->add_volume( 286, "HCPA", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCPB_solid"), 0.0*units::degree, 360.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {210.0*units::cm, 210.0*units::cm, 450.3452*units::cm, 450.3452*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.4009*units::cm, 483.3999*units::cm} ); 
  this->add_volume( 287, "HCPB", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCMA_solid"), -30.0*units::degree, 60.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {210.0*units::cm, 210.0*units::cm, 450.3452*units::cm, 450.3452*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.4009*units::cm, 483.3999*units::cm} ); 
  this->add_volume( 288, "HCMA", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCMB_solid"), -30.0*units::degree, 60.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {210.0*units::cm, 210.0*units::cm, 450.3452*units::cm, 450.3452*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.4009*units::cm, 483.3999*units::cm} ); 
  this->add_volume( 289, "HCMB", "HCAL_CALO_VOLUME", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCMO_solid"), 0.0*units::degree, 60.0*units::degree, {45.0*units::cm, 45.0*units::cm, 45.0*units::cm, 45.0*units::cm}, {210.0*units::cm, 210.0*units::cm, 450.3452*units::cm, 450.3452*units::cm}, {315.0*units::cm, 365.3989*units::cm, 365.4009*units::cm, 483.3999*units::cm} ); 
  this->add_volume( 290, "HCMO", "HCAP_NON_SENSITIVE", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("HCLA_solid"), 0.0*units::degree, 60.0*units::degree, {45.0*units::cm, 45.0*units::cm}, {210.0*units::cm, 210.0*units::cm}, {-0.7*units::cm, 0.7*units::cm} ); 
  this->add_volume( 291, "HCLA", "HCAL_HSTREAMER_TUBES", solid, "ONLY");

  solid = dd4hep::Tube( std::string("HCME_solid"), 45.0*units::cm, 450.3452*units::cm, 5.0*units::cm, 0.0*units::degree, 60.0*units::degree ); 
  this->add_volume( 292, "HCME", "LAST_IRON_PLATE_MEDI", solid, "ONLY");

  solid = dd4hep::Tube( std::string("EPCB_solid"), 233.0*units::cm, 248.0*units::cm, 25.2*units::cm, 12.6899*units::degree, 17.3102*units::degree ); 
  this->add_volume( 293, "EPCB", "AIR_IN_NON_UNIFORM_B", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("MUON_solid"), 0.0*units::degree, 360.0*units::degree, {45.0*units::cm, 45.0*units::cm, 495.5999*units::cm, 495.5999*units::cm, 45.0*units::cm, 45.0*units::cm}, {650.0*units::cm, 650.0*units::cm, 650.0*units::cm, 650.0*units::cm, 650.0*units::cm, 650.0*units::cm}, {-600.0*units::cm, -496.7998*units::cm, -496.7898*units::cm, 496.7898*units::cm, 496.7998*units::cm, 600.0*units::cm} ); 
  this->add_volume( 294, "MUON", "MUON_______REGION", solid, "ONLY");

  solid = dd4hep::Box( std::string("MUB1_solid"), 123.85*units::cm, 348.2999*units::cm, 4.65*units::cm ); 
  this->add_volume( 295, "MUB1", "MUON_SENSITIVE_GAS", solid, "ONLY");

  solid = dd4hep::Tube( std::string("MUBO_solid"), 495.5999*units::cm, 650.0*units::cm, 348.2999*units::cm ); 
  this->add_volume( 296, "MUBO", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("MUB2_solid"), 127.0*units::cm, 348.2999*units::cm, 4.65*units::cm ); 
  this->add_volume( 297, "MUB2", "MUON_SENSITIVE_GAS", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("MUEA_solid"), 0.0*units::degree, 360.0*units::degree, {495.5999*units::cm, 495.5999*units::cm, 45.0*units::cm, 45.0*units::cm}, {650.0*units::cm, 650.0*units::cm, 650.0*units::cm, 650.0*units::cm}, {348.2999*units::cm, 496.7898*units::cm, 496.7998*units::cm, 600.0*units::cm} ); 
  this->add_volume( 298, "MUEA", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Polycone( std::string("MUEB_solid"), 0.0*units::degree, 360.0*units::degree, {495.5999*units::cm, 495.5999*units::cm, 45.0*units::cm, 45.0*units::cm}, {650.0*units::cm, 650.0*units::cm, 650.0*units::cm, 650.0*units::cm}, {348.2999*units::cm, 496.7898*units::cm, 496.7998*units::cm, 600.0*units::cm} ); 
  this->add_volume( 299, "MUEB", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("MMIA_solid"), 495.5999*units::cm, 543.9999*units::cm, 73.8*units::cm, -55.0*units::degree, 235.0*units::degree ); 
  this->add_volume( 300, "MMIA", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("MMOA_solid"), 543.9999*units::cm, 650.0*units::cm, 91.0499*units::cm, -55.0*units::degree, 235.0*units::degree ); 
  this->add_volume( 301, "MMOA", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("MMIB_solid"), 495.5999*units::cm, 543.9999*units::cm, 73.8*units::cm, -55.0*units::degree, 235.0*units::degree ); 
  this->add_volume( 302, "MMIB", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Tube( std::string("MMOB_solid"), 543.9999*units::cm, 650.0*units::cm, 91.0499*units::cm, -55.0*units::degree, 235.0*units::degree ); 
  this->add_volume( 303, "MMOB", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("MUM1_solid"), 73.8*units::cm, 122.9*units::cm, 4.7*units::cm ); 
  this->add_volume( 304, "MUM1", "MUON_SENSITIVE_GAS", solid, "ONLY");

  solid = dd4hep::Box( std::string("MUM2_solid"), 90.65*units::cm, 131.25*units::cm, 4.7*units::cm ); 
  this->add_volume( 305, "MUM2", "MUON_SENSITIVE_GAS", solid, "ONLY");

  solid = dd4hep::Box( std::string("MMBA_solid"), 83.0*units::cm, 204.0*units::cm, 6.0*units::cm ); 
  this->add_volume( 306, "MMBA", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("MMBB_solid"), 83.0*units::cm, 204.0*units::cm, 6.0*units::cm ); 
  this->add_volume( 307, "MMBB", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("MCIA_solid"), 543.9999*units::cm, 518.1998*units::cm, 16.8*units::cm ); 
  this->add_volume( 308, "MCIA", "AIR_OUTSIDE_FIELD", solid, "MANY");

  solid = dd4hep::Box( std::string("MCOA_solid"), 650.0*units::cm, 571.2*units::cm, 34.8001*units::cm ); 
  this->add_volume( 309, "MCOA", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("MCIB_solid"), 543.9999*units::cm, 518.1998*units::cm, 16.8*units::cm ); 
  this->add_volume( 310, "MCIB", "AIR_OUTSIDE_FIELD", solid, "MANY");

  solid = dd4hep::Box( std::string("MCOB_solid"), 650.0*units::cm, 571.2*units::cm, 34.8001*units::cm ); 
  this->add_volume( 311, "MCOB", "AIR_OUTSIDE_FIELD", solid, "ONLY");

  solid = dd4hep::Box( std::string("MUC1_solid"), 300.6*units::cm, 237.1*units::cm, 4.7*units::cm ); 
  this->add_volume( 312, "MUC1", "MUON_SENSITIVE_GAS", solid, "ONLY");

  solid = dd4hep::Box( std::string("MUC2_solid"), 323.2999*units::cm, 259.35*units::cm, 4.7*units::cm ); 
  this->add_volume( 313, "MUC2", "MUON_SENSITIVE_GAS", solid, "ONLY");
} /// End geant3_geometry_imp::handle_volumes  


/// Handle the conversion of the Geant3 volume divisions: 
void dd4hep::geant3_geometry_imp::handle_divisions()  { 
  this->add_division("EBAR", "EBAL",  2, 12,  -1.874*units::degree, 0.0*units::degree,  56, "NX");
  this->add_division("ECMA", "ECPA",  2, 12,  0.0*units::degree, 0.0*units::degree,  0, "N");
  this->add_division("ECMB", "ECPB",  2, 12,  0.0*units::degree, 0.0*units::degree,  0, "N");
  this->add_division("HBAR", "HBAL",  2, 12,  -15.0*units::degree, 0.0*units::degree,  133, "NX");
  this->add_division("HCMA", "HCPA",  2, 6,  0.0*units::degree, 0.0*units::degree,  0, "N");
  this->add_division("HCMB", "HCPB",  2, 6,  0.0*units::degree, 0.0*units::degree,  0, "N");

} /// End geant3_geometry_imp::handle_divisions  


/// Handle the conversion of the Geant3 volumes: 
void dd4hep::geant3_geometry_imp::handle_placements()  { 
  std::vector<double> params; 

  params.clear();
  this->add_placement(1, "ALEF", "CDET", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(2, "CDET", "PASV", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,188.05*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(3, "CDET", "PASW", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-188.05*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(4, "ALEF", "BTUB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(5, "ALEF", "BTUP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,288.75*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(6, "ALEF", "BTUP", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-288.75*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(7, "BTUB", "BPIP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(8, "BPIP", "BPBY", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(9, "BPBY", "BPBE", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(10, "BPBY", "BPSO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(11, "BPBY", "BBAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,37.875*units::cm), 4, params, "ONLY");
  params.clear();
  this->add_placement(12, "BPBY", "BBAL", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-37.875*units::cm), 4, params, "ONLY");
  params.clear();
  this->add_placement(13, "BPIP", "BPND", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,41.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(14, "BPIP", "BPND", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-41.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(15, "BPND", "BAIR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,1.75*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(16, "BTUB", "BPAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,153.25*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(17, "BTUB", "BPAL", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-153.25*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(18, "BTUB", "BSAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,244.925*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(19, "BTUB", "BSAL", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-244.925*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(20, "BTUB", "BRIA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,231.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(21, "BTUB", "BRIB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-156.45*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(22, "BTUB", "BRIA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-231.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(23, "BTUB", "BVAC", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(24, "BVAC", "BXAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,39.2*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(25, "BVAC", "BXAL", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-39.2*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(26, "BVAC", "BAL1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(27, "BVAC", "BAL1", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(28, "BVAC", "BAL2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(29, "BVAC", "BAL2", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(30, "BTUP", "BCOM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,2.25*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(31, "BCOM", "BVCO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(32, "BTUP", "BEND", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-24.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(33, "BEND", "BRID", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-1.1*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(34, "BEND", "BRIN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,1.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(35, "BEND", "BVCI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(36, "PASV", "BPSA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,43.65*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(37, "PASW", "BPSA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,43.65*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(38, "PASW", "BPSP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-31.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(39, "BVAC", "BMSK", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(40, "BVAC", "BMSK", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(41, "PASV", "BSH1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,5.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(42, "PASW", "BSH1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,5.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(43, "PASV", "BSH2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,44.45*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(44, "PASW", "BSH2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,44.45*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(45, "PASV", "BRH1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,22.05*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(46, "PASW", "BRH1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,22.05*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(47, "PASV", "BRH2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,34.85*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(48, "PASW", "BRH2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,34.85*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(49, "PASV", "BRH3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.45*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(50, "PASW", "BRH3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.45*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(51, "PASV", "BRH4", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,50.95*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(52, "PASW", "BRH4", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,50.95*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(53, "BRH1", "BRA1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(54, "BRH2", "BRA2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(55, "BRH3", "BRA3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(56, "BRH4", "BRA3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(57, "PASV", "BPN1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,21.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(58, "PASW", "BPN1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,21.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(59, "PASV", "BPN2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,34.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(60, "PASW", "BPN2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,34.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(61, "PASV", "BPAN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,33.75*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(62, "PASW", "BPAN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,33.75*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(63, "PASV", "BPAM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,47.95*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(64, "PASW", "BPAM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,47.95*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(65, "PASV", "BPAO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,49.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(66, "PASW", "BPAO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,49.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(67, "PASV", "BPA1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(68, "PASW", "BPA1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(69, "PASV", "BPA1", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(70, "PASW", "BPA1", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(71, "PASV", "BPA2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(72, "PASW", "BPA2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(73, "PASV", "BPA2", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(74, "PASW", "BPA2", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(75, "PASV", "BPA3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(76, "PASW", "BPA3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(77, "PASV", "BPA3", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(78, "PASW", "BPA3", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,46.55*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(79, "BRH1", "BHOL", 1, dd4hep::Position(11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(80, "BRH1", "BHOL", 2, dd4hep::Position(5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(81, "BRH1", "BHOL", 3, dd4hep::Position(-5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(82, "BRH1", "BHOL", 4, dd4hep::Position(-11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(83, "BRH2", "BHOL", 1, dd4hep::Position(11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(84, "BRH2", "BHOL", 2, dd4hep::Position(5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(85, "BRH2", "BHOL", 3, dd4hep::Position(-5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(86, "BRH2", "BHOL", 4, dd4hep::Position(-11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(87, "BRH3", "BHOL", 1, dd4hep::Position(11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(88, "BRH3", "BHOL", 2, dd4hep::Position(5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(89, "BRH3", "BHOL", 3, dd4hep::Position(-5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(90, "BRH3", "BHOL", 4, dd4hep::Position(-11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(91, "BRH4", "BHOL", 1, dd4hep::Position(11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(92, "BRH4", "BHOL", 2, dd4hep::Position(5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(93, "BRH4", "BHOL", 3, dd4hep::Position(-5.494*units::cm,11.782*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(94, "BRH4", "BHOL", 4, dd4hep::Position(-11.258*units::cm,6.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(95, "BRH1", "BHOB", 1, dd4hep::Position(0.0*units::cm,-15.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(96, "BRH2", "BHOB", 1, dd4hep::Position(0.0*units::cm,-15.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(97, "BRH3", "BHOB", 1, dd4hep::Position(0.0*units::cm,-15.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(98, "BRH4", "BHOB", 1, dd4hep::Position(0.0*units::cm,-15.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(99, "BRH1", "BHOD", 1, dd4hep::Position(8.132*units::cm,-8.132*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(100, "BRH1", "BHOD", 2, dd4hep::Position(-8.132*units::cm,-8.132*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(101, "BRH2", "BHOD", 1, dd4hep::Position(8.839*units::cm,-8.839*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(102, "BRH2", "BHOD", 2, dd4hep::Position(-8.839*units::cm,-8.839*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(103, "BRH3", "BHOD", 1, dd4hep::Position(9.546*units::cm,-9.546*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(104, "BRH3", "BHOD", 2, dd4hep::Position(-9.546*units::cm,-9.546*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(105, "BRH4", "BHOD", 1, dd4hep::Position(9.899*units::cm,-9.899*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(106, "BRH4", "BHOD", 2, dd4hep::Position(-9.899*units::cm,-9.899*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(107, "BRH1", "BHOD", 3, dd4hep::Position(-10.422*units::cm,-4.86*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(108, "BRH2", "BHOD", 3, dd4hep::Position(-11.329*units::cm,-5.283*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(109, "BRH3", "BHOD", 3, dd4hep::Position(-12.235*units::cm,-5.705*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(110, "BRH4", "BHOD", 3, dd4hep::Position(-12.688*units::cm,-5.917*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(111, "VFPB", "VPBS", 1, dd4hep::Position(0.0*units::cm,-2.23*units::cm,0.0*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(112, "VFPB", "VPBS", 2, dd4hep::Position(0.0*units::cm,2.23*units::cm,0.0*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(113, "VFSI", "VMSI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-12.5725*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(114, "VFSI", "VMSI", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,12.5725*units::cm), 7, params, "ONLY");
  params.clear();
  this->add_placement(115, "VMSI", "VMBA", 1, dd4hep::Position(-0.07*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(116, "VMSI", "VMES", 1, dd4hep::Position(0.015*units::cm,0.0*units::cm,-9.1675*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(117, "VMBA", "VBCE", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-9.9738*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(118, "VMBA", "VBGL", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,-7.3713*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(119, "VMBA", "VBQU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-7.2188*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(120, "VMBA", "VBSI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,2.7487*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(121, "VMBA", "VBMG", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,12.57*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(122, "VBSI", "VBGL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-9.815*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(123, "VBSI", "VSWA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,-6.5425*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(124, "VBSI", "VBGL", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-3.27*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(125, "VBSI", "VSWA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0025*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(126, "VBSI", "VBGL", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,3.275*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(127, "VBSI", "VSWA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,6.5475*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(128, "VMEO", "VEH1", 1, dd4hep::Position(0.125*units::cm,0.0*units::cm,-0.6*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(129, "VMEO", "VEH2", 1, dd4hep::Position(0.125*units::cm,0.0*units::cm,1.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(130, "VMEO", "VECA", 1, dd4hep::Position(0.055*units::cm,0.0*units::cm,1.3*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(131, "VMEO", "VEMX", 1, dd4hep::Position(0.065*units::cm,0.0*units::cm,0.2285*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(132, "VMEO", "VEPO", 1, dd4hep::Position(-0.045*units::cm,2.23*units::cm,-0.5*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(133, "VMEO", "VEPO", 2, dd4hep::Position(-0.045*units::cm,-2.23*units::cm,-0.5*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(134, "VOUP", "VUPC", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.735*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(135, "VOUP", "VUPM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-9.325*units::cm), 11, params, "ONLY");
  params.clear();
  this->add_placement(136, "VOUP", "VUPE", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-10.06*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(137, "VOPI", "VPSU", 1, dd4hep::Position(0.29*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(138, "VOPI", "VPGZ", 1, dd4hep::Position(-0.1025*units::cm,0.0*units::cm,0.0*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(139, "VOKE", "VKGL", 1, dd4hep::Position(0.335*units::cm,-1.61*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(140, "VOKE", "VKGL", 2, dd4hep::Position(0.335*units::cm,1.61*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(141, "VOKE", "VKEV", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 9, params, "MANY");
  params.clear();
  this->add_placement(142, "VKEV", "VKAI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.03*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(143, "VMES", "VEH1", 2, dd4hep::Position(-0.025*units::cm,0.0*units::cm,-0.6*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(144, "VMES", "VEH2", 2, dd4hep::Position(-0.025*units::cm,0.0*units::cm,1.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(145, "VMES", "VECA", 2, dd4hep::Position(0.045*units::cm,0.0*units::cm,1.3*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(146, "VMES", "VEMX", 2, dd4hep::Position(0.035*units::cm,0.0*units::cm,0.2285*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(147, "VMES", "VEPS", 1, dd4hep::Position(0.045*units::cm,2.23*units::cm,-0.5*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(148, "VMES", "VEPS", 2, dd4hep::Position(0.045*units::cm,-2.23*units::cm,-0.5*units::cm), 9, params, "ONLY");
  params.clear();
  this->add_placement(149, "VDET", "VTUI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(150, "VDET", "VTUM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(151, "VDET", "VTUO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(152, "VDET", "VDAI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-20.445*units::cm), 0, params, "MANY");
  params.clear();
  this->add_placement(153, "VDET", "VDAI", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,20.445*units::cm), 7, params, "MANY");
  params.clear();
  this->add_placement(154, "VDAI", "VDKI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "MANY");
  params.clear();
  this->add_placement(155, "VDKI", "VDTU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.25*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(156, "VDET", "VDF1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-30.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(157, "VDET", "VDF1", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,30.7*units::cm), 7, params, "ONLY");
  params.clear();
  this->add_placement(158, "VDET", "VDF2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-30.645*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(159, "VDET", "VDF2", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,30.645*units::cm), 7, params, "ONLY");
  params.clear();
  this->add_placement(160, "VDET", "VDF3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-30.645*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(161, "VDET", "VDF3", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,30.645*units::cm), 7, params, "ONLY");
  params.clear();
  this->add_placement(162, "VDET", "VDC1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-29.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(163, "VDET", "VDC1", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,29.4*units::cm), 7, params, "ONLY");
  params.clear();
  this->add_placement(164, "VDET", "VDC2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-29.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(165, "VDET", "VDC2", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,29.4*units::cm), 7, params, "ONLY");
  params.clear();
  this->add_placement(166, "VRAO", "VRAI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.0457*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(167, "VDET", "VRAO", 1, dd4hep::Position(0.0*units::cm,-12.6038*units::cm,0.0*units::cm), 5, params, "ONLY");
  params.clear();
  this->add_placement(168, "VDET", "VRAO", 2, dd4hep::Position(0.0*units::cm,12.6038*units::cm,0.0*units::cm), 6, params, "ONLY");
  params.clear();
  this->add_placement(169, "CDET", "VDET", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(170, "VSWA", "VWSS", 1, dd4hep::Position(0.0135*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(171, "VSWA", "VWSS", 2, dd4hep::Position(0.0105*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(172, "VSWA", "VWSS", 3, dd4hep::Position(0.0075*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(173, "VSWA", "VWSS", 4, dd4hep::Position(0.0045*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(174, "VSWA", "VWSS", 5, dd4hep::Position(0.0015*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(175, "VSWA", "VWSS", 6, dd4hep::Position(-0.0015*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(176, "VSWA", "VWSS", 7, dd4hep::Position(-0.0045*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(177, "VSWA", "VWSS", 8, dd4hep::Position(-0.0075*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(178, "VSWA", "VWSS", 9, dd4hep::Position(-0.0105*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(179, "VSWA", "VWSS", 10, dd4hep::Position(-0.0135*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(180, "VDET", "VLOC", 1, dd4hep::Position(1.9053*units::cm,8.4888*units::cm,24.365*units::cm), 12, params, "ONLY");
  params.clear();
  this->add_placement(181, "VDET", "VLOC", 2, dd4hep::Position(3.2076*units::cm,-8.0871*units::cm,24.365*units::cm), 13, params, "ONLY");
  params.clear();
  this->add_placement(182, "VDET", "VLOC", 3, dd4hep::Position(1.9053*units::cm,8.4888*units::cm,-24.365*units::cm), 14, params, "ONLY");
  params.clear();
  this->add_placement(183, "VDET", "VLOC", 4, dd4hep::Position(3.2076*units::cm,-8.0871*units::cm,-24.365*units::cm), 15, params, "ONLY");
  params.clear();
  this->add_placement(184, "VLOC", "VLTR", 1, dd4hep::Position(0.3125*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(185, "VDET", "VLBX", 1, dd4hep::Position(-1.8919*units::cm,8.4918*units::cm,21.015*units::cm), 16, params, "ONLY");
  params.clear();
  this->add_placement(186, "VDET", "VLBX", 2, dd4hep::Position(0.4144*units::cm,-8.6901*units::cm,21.015*units::cm), 17, params, "ONLY");
  params.clear();
  this->add_placement(187, "VDET", "VLBX", 3, dd4hep::Position(-1.8919*units::cm,8.4918*units::cm,-21.015*units::cm), 18, params, "ONLY");
  params.clear();
  this->add_placement(188, "VDET", "VLBX", 4, dd4hep::Position(0.4144*units::cm,-8.6901*units::cm,-21.015*units::cm), 19, params, "ONLY");
  params.clear();
  this->add_placement(189, "VDET", "VFSI", 1, dd4hep::Position(6.2346*units::cm,1.0892*units::cm,0.0*units::cm), 20, params, "ONLY");
  params.clear();
  this->add_placement(190, "VDET", "VFPB", 1, dd4hep::Position(5.9927*units::cm,0.9939*units::cm,-22.24*units::cm), 20, params, "ONLY");
  params.clear();
  this->add_placement(191, "VDET", "VFPB", 2, dd4hep::Position(5.9927*units::cm,0.9939*units::cm,22.24*units::cm), 20, params, "ONLY");
  params.clear();
  this->add_placement(192, "VDET", "VOUP", 1, dd4hep::Position(6.3207*units::cm,1.1231*units::cm,0.0*units::cm), 20, params, "ONLY");
  params.clear();
  this->add_placement(193, "VDET", "VOKE", 1, dd4hep::Position(6.6719*units::cm,1.2614*units::cm,0.0*units::cm), 20, params, "MANY");
  params.clear();
  this->add_placement(194, "VDET", "VOPI", 1, dd4hep::Position(6.6929*units::cm,1.2697*units::cm,-20.39*units::cm), 20, params, "MANY");
  params.clear();
  this->add_placement(195, "VDET", "VOPI", 2, dd4hep::Position(6.6929*units::cm,1.2697*units::cm,20.39*units::cm), 20, params, "MANY");
  params.clear();
  this->add_placement(196, "VDET", "VMEO", 1, dd4hep::Position(6.4719*units::cm,1.1826*units::cm,-23.1475*units::cm), 20, params, "ONLY");
  params.clear();
  this->add_placement(197, "VDET", "VMEO", 2, dd4hep::Position(6.4719*units::cm,1.1826*units::cm,23.1475*units::cm), 21, params, "ONLY");
  params.clear();
  this->add_placement(198, "VDET", "VFSI", 2, dd4hep::Position(4.0759*units::cm,4.8419*units::cm,0.0*units::cm), 22, params, "ONLY");
  params.clear();
  this->add_placement(199, "VDET", "VFPB", 3, dd4hep::Position(3.9518*units::cm,4.6134*units::cm,-22.24*units::cm), 22, params, "ONLY");
  params.clear();
  this->add_placement(200, "VDET", "VFPB", 4, dd4hep::Position(3.9518*units::cm,4.6134*units::cm,22.24*units::cm), 22, params, "ONLY");
  params.clear();
  this->add_placement(201, "VDET", "VOUP", 2, dd4hep::Position(4.12*units::cm,4.9232*units::cm,0.0*units::cm), 22, params, "ONLY");
  params.clear();
  this->add_placement(202, "VDET", "VOKE", 2, dd4hep::Position(4.3002*units::cm,5.255*units::cm,0.0*units::cm), 22, params, "MANY");
  params.clear();
  this->add_placement(203, "VDET", "VOPI", 3, dd4hep::Position(4.3109*units::cm,5.2747*units::cm,-20.39*units::cm), 22, params, "MANY");
  params.clear();
  this->add_placement(204, "VDET", "VOPI", 4, dd4hep::Position(4.3109*units::cm,5.2747*units::cm,20.39*units::cm), 22, params, "MANY");
  params.clear();
  this->add_placement(205, "VDET", "VMEO", 3, dd4hep::Position(4.1976*units::cm,5.066*units::cm,-23.1475*units::cm), 22, params, "ONLY");
  params.clear();
  this->add_placement(206, "VDET", "VMEO", 4, dd4hep::Position(4.1976*units::cm,5.066*units::cm,23.1475*units::cm), 23, params, "ONLY");
  params.clear();
  this->add_placement(207, "VDET", "VFSI", 3, dd4hep::Position(0.01*units::cm,6.3291*units::cm,0.0*units::cm), 24, params, "ONLY");
  params.clear();
  this->add_placement(208, "VDET", "VFPB", 5, dd4hep::Position(0.0618*units::cm,6.0743*units::cm,-22.24*units::cm), 24, params, "ONLY");
  params.clear();
  this->add_placement(209, "VDET", "VFPB", 6, dd4hep::Position(0.0618*units::cm,6.0743*units::cm,22.24*units::cm), 24, params, "ONLY");
  params.clear();
  this->add_placement(210, "VDET", "VOUP", 3, dd4hep::Position(-0.0084*units::cm,6.4197*units::cm,0.0*units::cm), 24, params, "ONLY");
  params.clear();
  this->add_placement(211, "VDET", "VOKE", 3, dd4hep::Position(-0.0837*units::cm,6.7896*units::cm,0.0*units::cm), 24, params, "MANY");
  params.clear();
  this->add_placement(212, "VDET", "VOPI", 5, dd4hep::Position(-0.0882*units::cm,6.8117*units::cm,-20.39*units::cm), 24, params, "MANY");
  params.clear();
  this->add_placement(213, "VDET", "VOPI", 6, dd4hep::Position(-0.0882*units::cm,6.8117*units::cm,20.39*units::cm), 24, params, "MANY");
  params.clear();
  this->add_placement(214, "VDET", "VMEO", 5, dd4hep::Position(-0.0408*units::cm,6.5789*units::cm,-23.1475*units::cm), 24, params, "ONLY");
  params.clear();
  this->add_placement(215, "VDET", "VMEO", 6, dd4hep::Position(-0.0408*units::cm,6.5789*units::cm,23.1475*units::cm), 25, params, "ONLY");
  params.clear();
  this->add_placement(216, "VDET", "VFSI", 4, dd4hep::Position(-4.0606*units::cm,4.8548*units::cm,0.0*units::cm), 26, params, "ONLY");
  params.clear();
  this->add_placement(217, "VDET", "VFPB", 7, dd4hep::Position(-3.8571*units::cm,4.6929*units::cm,-22.24*units::cm), 26, params, "ONLY");
  params.clear();
  this->add_placement(218, "VDET", "VFPB", 8, dd4hep::Position(-3.8571*units::cm,4.6929*units::cm,22.24*units::cm), 26, params, "ONLY");
  params.clear();
  this->add_placement(219, "VDET", "VOUP", 4, dd4hep::Position(-4.133*units::cm,4.9124*units::cm,0.0*units::cm), 26, params, "ONLY");
  params.clear();
  this->add_placement(220, "VDET", "VOKE", 4, dd4hep::Position(-4.4284*units::cm,5.1474*units::cm,0.0*units::cm), 26, params, "MANY");
  params.clear();
  this->add_placement(221, "VDET", "VOPI", 7, dd4hep::Position(-4.446*units::cm,5.1614*units::cm,-20.39*units::cm), 26, params, "MANY");
  params.clear();
  this->add_placement(222, "VDET", "VOPI", 8, dd4hep::Position(-4.446*units::cm,5.1614*units::cm,20.39*units::cm), 26, params, "MANY");
  params.clear();
  this->add_placement(223, "VDET", "VMEO", 7, dd4hep::Position(-4.2601*units::cm,5.0135*units::cm,-23.1475*units::cm), 26, params, "ONLY");
  params.clear();
  this->add_placement(224, "VDET", "VMEO", 8, dd4hep::Position(-4.2601*units::cm,5.0135*units::cm,23.1475*units::cm), 27, params, "ONLY");
  params.clear();
  this->add_placement(225, "VDET", "VFSI", 5, dd4hep::Position(-6.2312*units::cm,1.1089*units::cm,0.0*units::cm), 28, params, "ONLY");
  params.clear();
  this->add_placement(226, "VDET", "VFPB", 9, dd4hep::Position(-5.9712*units::cm,1.1157*units::cm,-22.24*units::cm), 28, params, "ONLY");
  params.clear();
  this->add_placement(227, "VDET", "VFPB", 10, dd4hep::Position(-5.9712*units::cm,1.1157*units::cm,22.24*units::cm), 28, params, "ONLY");
  params.clear();
  this->add_placement(228, "VDET", "VOUP", 5, dd4hep::Position(-6.3236*units::cm,1.1065*units::cm,0.0*units::cm), 28, params, "ONLY");
  params.clear();
  this->add_placement(229, "VDET", "VOKE", 5, dd4hep::Position(-6.701*units::cm,1.0966*units::cm,0.0*units::cm), 28, params, "MANY");
  params.clear();
  this->add_placement(230, "VDET", "VOPI", 9, dd4hep::Position(-6.7235*units::cm,1.096*units::cm,-20.39*units::cm), 28, params, "MANY");
  params.clear();
  this->add_placement(231, "VDET", "VOPI", 10, dd4hep::Position(-6.7235*units::cm,1.096*units::cm,20.39*units::cm), 28, params, "MANY");
  params.clear();
  this->add_placement(232, "VDET", "VMEO", 9, dd4hep::Position(-6.4861*units::cm,1.1022*units::cm,-23.1475*units::cm), 28, params, "ONLY");
  params.clear();
  this->add_placement(233, "VDET", "VMEO", 10, dd4hep::Position(-6.4861*units::cm,1.1022*units::cm,23.1475*units::cm), 29, params, "ONLY");
  params.clear();
  this->add_placement(234, "VDET", "VFSI", 6, dd4hep::Position(-5.4861*units::cm,-3.1559*units::cm,0.0*units::cm), 30, params, "ONLY");
  params.clear();
  this->add_placement(235, "VDET", "VFPB", 11, dd4hep::Position(-5.2914*units::cm,-2.9836*units::cm,-22.24*units::cm), 30, params, "ONLY");
  params.clear();
  this->add_placement(236, "VDET", "VFPB", 12, dd4hep::Position(-5.2914*units::cm,-2.9836*units::cm,22.24*units::cm), 30, params, "ONLY");
  params.clear();
  this->add_placement(237, "VDET", "VOUP", 6, dd4hep::Position(-5.5554*units::cm,-3.2172*units::cm,0.0*units::cm), 30, params, "ONLY");
  params.clear();
  this->add_placement(238, "VDET", "VOKE", 6, dd4hep::Position(-5.8381*units::cm,-3.4673*units::cm,0.0*units::cm), 30, params, "MANY");
  params.clear();
  this->add_placement(239, "VDET", "VOPI", 11, dd4hep::Position(-5.855*units::cm,-3.4822*units::cm,-20.39*units::cm), 30, params, "MANY");
  params.clear();
  this->add_placement(240, "VDET", "VOPI", 12, dd4hep::Position(-5.855*units::cm,-3.4822*units::cm,20.39*units::cm), 30, params, "MANY");
  params.clear();
  this->add_placement(241, "VDET", "VMEO", 11, dd4hep::Position(-5.6771*units::cm,-3.3249*units::cm,-23.1475*units::cm), 30, params, "ONLY");
  params.clear();
  this->add_placement(242, "VDET", "VMEO", 12, dd4hep::Position(-5.6771*units::cm,-3.3249*units::cm,23.1475*units::cm), 31, params, "ONLY");
  params.clear();
  this->add_placement(243, "VDET", "VFSI", 7, dd4hep::Position(-2.174*units::cm,-5.944*units::cm,0.0*units::cm), 32, params, "ONLY");
  params.clear();
  this->add_placement(244, "VDET", "VFPB", 13, dd4hep::Position(-2.1356*units::cm,-5.6868*units::cm,-22.24*units::cm), 32, params, "ONLY");
  params.clear();
  this->add_placement(245, "VDET", "VFPB", 14, dd4hep::Position(-2.1356*units::cm,-5.6868*units::cm,22.24*units::cm), 32, params, "ONLY");
  params.clear();
  this->add_placement(246, "VDET", "VOUP", 7, dd4hep::Position(-2.1877*units::cm,-6.0354*units::cm,0.0*units::cm), 32, params, "ONLY");
  params.clear();
  this->add_placement(247, "VDET", "VOKE", 7, dd4hep::Position(-2.2435*units::cm,-6.4088*units::cm,0.0*units::cm), 32, params, "MANY");
  params.clear();
  this->add_placement(248, "VDET", "VOPI", 13, dd4hep::Position(-2.2468*units::cm,-6.431*units::cm,-20.39*units::cm), 32, params, "MANY");
  params.clear();
  this->add_placement(249, "VDET", "VOPI", 14, dd4hep::Position(-2.2468*units::cm,-6.431*units::cm,20.39*units::cm), 32, params, "MANY");
  params.clear();
  this->add_placement(250, "VDET", "VMEO", 13, dd4hep::Position(-2.2117*units::cm,-6.1961*units::cm,-23.1475*units::cm), 32, params, "ONLY");
  params.clear();
  this->add_placement(251, "VDET", "VMEO", 14, dd4hep::Position(-2.2117*units::cm,-6.1961*units::cm,23.1475*units::cm), 33, params, "ONLY");
  params.clear();
  this->add_placement(252, "VDET", "VFSI", 8, dd4hep::Position(2.1553*units::cm,-5.9508*units::cm,0.0*units::cm), 34, params, "ONLY");
  params.clear();
  this->add_placement(253, "VDET", "VFPB", 15, dd4hep::Position(2.0194*units::cm,-5.7291*units::cm,-22.24*units::cm), 34, params, "ONLY");
  params.clear();
  this->add_placement(254, "VDET", "VFPB", 16, dd4hep::Position(2.0194*units::cm,-5.7291*units::cm,22.24*units::cm), 34, params, "ONLY");
  params.clear();
  this->add_placement(255, "VDET", "VOUP", 8, dd4hep::Position(2.2036*units::cm,-6.0297*units::cm,0.0*units::cm), 34, params, "ONLY");
  params.clear();
  this->add_placement(256, "VDET", "VOKE", 8, dd4hep::Position(2.4008*units::cm,-6.3515*units::cm,0.0*units::cm), 34, params, "MANY");
  params.clear();
  this->add_placement(257, "VDET", "VOPI", 15, dd4hep::Position(2.4126*units::cm,-6.3707*units::cm,-20.39*units::cm), 34, params, "MANY");
  params.clear();
  this->add_placement(258, "VDET", "VOPI", 16, dd4hep::Position(2.4126*units::cm,-6.3707*units::cm,20.39*units::cm), 34, params, "MANY");
  params.clear();
  this->add_placement(259, "VDET", "VMEO", 15, dd4hep::Position(2.2885*units::cm,-6.1682*units::cm,-23.1475*units::cm), 34, params, "ONLY");
  params.clear();
  this->add_placement(260, "VDET", "VMEO", 16, dd4hep::Position(2.2885*units::cm,-6.1682*units::cm,23.1475*units::cm), 35, params, "ONLY");
  params.clear();
  this->add_placement(261, "VDET", "VFSI", 9, dd4hep::Position(5.4761*units::cm,-3.1732*units::cm,0.0*units::cm), 36, params, "ONLY");
  params.clear();
  this->add_placement(262, "VDET", "VFPB", 17, dd4hep::Position(5.2296*units::cm,-3.0907*units::cm,-22.24*units::cm), 36, params, "ONLY");
  params.clear();
  this->add_placement(263, "VDET", "VFPB", 18, dd4hep::Position(5.2296*units::cm,-3.0907*units::cm,22.24*units::cm), 36, params, "ONLY");
  params.clear();
  this->add_placement(264, "VDET", "VOUP", 9, dd4hep::Position(5.5638*units::cm,-3.2025*units::cm,0.0*units::cm), 36, params, "ONLY");
  params.clear();
  this->add_placement(265, "VDET", "VOKE", 9, dd4hep::Position(5.9218*units::cm,-3.3223*units::cm,0.0*units::cm), 36, params, "MANY");
  params.clear();
  this->add_placement(266, "VDET", "VOPI", 17, dd4hep::Position(5.9432*units::cm,-3.3295*units::cm,-20.39*units::cm), 36, params, "MANY");
  params.clear();
  this->add_placement(267, "VDET", "VOPI", 18, dd4hep::Position(5.9432*units::cm,-3.3295*units::cm,20.39*units::cm), 36, params, "MANY");
  params.clear();
  this->add_placement(268, "VDET", "VMEO", 17, dd4hep::Position(5.7179*units::cm,-3.2541*units::cm,-23.1475*units::cm), 36, params, "ONLY");
  params.clear();
  this->add_placement(269, "VDET", "VMEO", 18, dd4hep::Position(5.7179*units::cm,-3.2541*units::cm,23.1475*units::cm), 37, params, "ONLY");
  params.clear();
  this->add_placement(270, "VDET", "VFSI", 10, dd4hep::Position(10.2162*units::cm,3.3291*units::cm,0.0*units::cm), 38, params, "ONLY");
  params.clear();
  this->add_placement(271, "VDET", "VFPB", 19, dd4hep::Position(10.4447*units::cm,3.4532*units::cm,-22.24*units::cm), 38, params, "ONLY");
  params.clear();
  this->add_placement(272, "VDET", "VFPB", 20, dd4hep::Position(10.4447*units::cm,3.4532*units::cm,22.24*units::cm), 38, params, "ONLY");
  params.clear();
  this->add_placement(273, "VDET", "VOUP", 10, dd4hep::Position(10.1349*units::cm,3.285*units::cm,0.0*units::cm), 38, params, "ONLY");
  params.clear();
  this->add_placement(274, "VDET", "VOKE", 10, dd4hep::Position(9.8032*units::cm,3.1048*units::cm,0.0*units::cm), 38, params, "MANY");
  params.clear();
  this->add_placement(275, "VDET", "VOPI", 19, dd4hep::Position(9.7834*units::cm,3.0941*units::cm,-20.39*units::cm), 38, params, "MANY");
  params.clear();
  this->add_placement(276, "VDET", "VOPI", 20, dd4hep::Position(9.7834*units::cm,3.0941*units::cm,20.39*units::cm), 38, params, "MANY");
  params.clear();
  this->add_placement(277, "VDET", "VMEO", 19, dd4hep::Position(9.9921*units::cm,3.2074*units::cm,-23.1475*units::cm), 38, params, "ONLY");
  params.clear();
  this->add_placement(278, "VDET", "VMEO", 20, dd4hep::Position(9.9921*units::cm,3.2074*units::cm,23.1475*units::cm), 39, params, "ONLY");
  params.clear();
  this->add_placement(279, "VDET", "VFSI", 11, dd4hep::Position(7.9789*units::cm,7.1966*units::cm,0.0*units::cm), 40, params, "ONLY");
  params.clear();
  this->add_placement(280, "VDET", "VFPB", 21, dd4hep::Position(8.1372*units::cm,7.4029*units::cm,-22.24*units::cm), 40, params, "ONLY");
  params.clear();
  this->add_placement(281, "VDET", "VFPB", 22, dd4hep::Position(8.1372*units::cm,7.4029*units::cm,22.24*units::cm), 40, params, "ONLY");
  params.clear();
  this->add_placement(282, "VDET", "VOUP", 11, dd4hep::Position(7.9226*units::cm,7.1232*units::cm,0.0*units::cm), 40, params, "ONLY");
  params.clear();
  this->add_placement(283, "VDET", "VOKE", 11, dd4hep::Position(7.6928*units::cm,6.8237*units::cm,0.0*units::cm), 40, params, "MANY");
  params.clear();
  this->add_placement(284, "VDET", "VOPI", 21, dd4hep::Position(7.6791*units::cm,6.8059*units::cm,-20.39*units::cm), 40, params, "MANY");
  params.clear();
  this->add_placement(285, "VDET", "VOPI", 22, dd4hep::Position(7.6791*units::cm,6.8059*units::cm,20.39*units::cm), 40, params, "MANY");
  params.clear();
  this->add_placement(286, "VDET", "VMEO", 21, dd4hep::Position(7.8237*units::cm,6.9943*units::cm,-23.1475*units::cm), 40, params, "ONLY");
  params.clear();
  this->add_placement(287, "VDET", "VMEO", 22, dd4hep::Position(7.8237*units::cm,6.9943*units::cm,23.1475*units::cm), 41, params, "ONLY");
  params.clear();
  this->add_placement(288, "VDET", "VFSI", 12, dd4hep::Position(4.362*units::cm,9.8197*units::cm,0.0*units::cm), 42, params, "ONLY");
  params.clear();
  this->add_placement(289, "VDET", "VFPB", 23, dd4hep::Position(4.4227*units::cm,10.0726*units::cm,-22.24*units::cm), 42, params, "ONLY");
  params.clear();
  this->add_placement(290, "VDET", "VFPB", 24, dd4hep::Position(4.4227*units::cm,10.0726*units::cm,22.24*units::cm), 42, params, "ONLY");
  params.clear();
  this->add_placement(291, "VDET", "VOUP", 12, dd4hep::Position(4.3404*units::cm,9.7298*units::cm,0.0*units::cm), 42, params, "ONLY");
  params.clear();
  this->add_placement(292, "VDET", "VOKE", 12, dd4hep::Position(4.2523*units::cm,9.3627*units::cm,0.0*units::cm), 42, params, "MANY");
  params.clear();
  this->add_placement(293, "VDET", "VOPI", 23, dd4hep::Position(4.247*units::cm,9.3409*units::cm,-20.39*units::cm), 42, params, "MANY");
  params.clear();
  this->add_placement(294, "VDET", "VOPI", 24, dd4hep::Position(4.247*units::cm,9.3409*units::cm,20.39*units::cm), 42, params, "MANY");
  params.clear();
  this->add_placement(295, "VDET", "VMEO", 23, dd4hep::Position(4.3024*units::cm,9.5718*units::cm,-23.1475*units::cm), 42, params, "ONLY");
  params.clear();
  this->add_placement(296, "VDET", "VMEO", 24, dd4hep::Position(4.3024*units::cm,9.5718*units::cm,23.1475*units::cm), 43, params, "ONLY");
  params.clear();
  this->add_placement(297, "VDET", "VFSI", 13, dd4hep::Position(-0.0092*units::cm,10.745*units::cm,0.0*units::cm), 44, params, "ONLY");
  params.clear();
  this->add_placement(298, "VDET", "VFPB", 25, dd4hep::Position(-0.0566*units::cm,11.0006*units::cm,-22.24*units::cm), 44, params, "ONLY");
  params.clear();
  this->add_placement(299, "VDET", "VFPB", 26, dd4hep::Position(-0.0566*units::cm,11.0006*units::cm,22.24*units::cm), 44, params, "ONLY");
  params.clear();
  this->add_placement(300, "VDET", "VOUP", 13, dd4hep::Position(0.0077*units::cm,10.654*units::cm,0.0*units::cm), 44, params, "ONLY");
  params.clear();
  this->add_placement(301, "VDET", "VOKE", 13, dd4hep::Position(0.0764*units::cm,10.2828*units::cm,0.0*units::cm), 44, params, "MANY");
  params.clear();
  this->add_placement(302, "VDET", "VOPI", 25, dd4hep::Position(0.0805*units::cm,10.2607*units::cm,-20.39*units::cm), 44, params, "MANY");
  params.clear();
  this->add_placement(303, "VDET", "VOPI", 26, dd4hep::Position(0.0805*units::cm,10.2607*units::cm,20.39*units::cm), 44, params, "MANY");
  params.clear();
  this->add_placement(304, "VDET", "VMEO", 25, dd4hep::Position(0.0373*units::cm,10.4942*units::cm,-23.1475*units::cm), 44, params, "ONLY");
  params.clear();
  this->add_placement(305, "VDET", "VMEO", 26, dd4hep::Position(0.0373*units::cm,10.4942*units::cm,23.1475*units::cm), 45, params, "ONLY");
  params.clear();
  this->add_placement(306, "VDET", "VFSI", 14, dd4hep::Position(-4.3788*units::cm,9.8123*units::cm,0.0*units::cm), 46, params, "ONLY");
  params.clear();
  this->add_placement(307, "VDET", "VFPB", 27, dd4hep::Position(-4.526*units::cm,10.0265*units::cm,-22.24*units::cm), 46, params, "ONLY");
  params.clear();
  this->add_placement(308, "VDET", "VFPB", 28, dd4hep::Position(-4.526*units::cm,10.0265*units::cm,22.24*units::cm), 46, params, "ONLY");
  params.clear();
  this->add_placement(309, "VDET", "VOUP", 14, dd4hep::Position(-4.3264*units::cm,9.736*units::cm,0.0*units::cm), 46, params, "ONLY");
  params.clear();
  this->add_placement(310, "VDET", "VOKE", 14, dd4hep::Position(-4.1126*units::cm,9.4249*units::cm,0.0*units::cm), 46, params, "MANY");
  params.clear();
  this->add_placement(311, "VDET", "VOPI", 27, dd4hep::Position(-4.0998*units::cm,9.4064*units::cm,-20.39*units::cm), 46, params, "MANY");
  params.clear();
  this->add_placement(312, "VDET", "VOPI", 28, dd4hep::Position(-4.0998*units::cm,9.4064*units::cm,20.39*units::cm), 46, params, "MANY");
  params.clear();
  this->add_placement(313, "VDET", "VMEO", 27, dd4hep::Position(-4.2343*units::cm,9.6021*units::cm,-23.1475*units::cm), 46, params, "ONLY");
  params.clear();
  this->add_placement(314, "VDET", "VMEO", 28, dd4hep::Position(-4.2343*units::cm,9.6021*units::cm,23.1475*units::cm), 47, params, "ONLY");
  params.clear();
  this->add_placement(315, "VDET", "VFSI", 15, dd4hep::Position(-7.9912*units::cm,7.183*units::cm,0.0*units::cm), 48, params, "ONLY");
  params.clear();
  this->add_placement(316, "VDET", "VFPB", 29, dd4hep::Position(-8.2128*units::cm,7.3189*units::cm,-22.24*units::cm), 48, params, "ONLY");
  params.clear();
  this->add_placement(317, "VDET", "VFPB", 30, dd4hep::Position(-8.2128*units::cm,7.3189*units::cm,22.24*units::cm), 48, params, "ONLY");
  params.clear();
  this->add_placement(318, "VDET", "VOUP", 15, dd4hep::Position(-7.9123*units::cm,7.1347*units::cm,0.0*units::cm), 48, params, "ONLY");
  params.clear();
  this->add_placement(319, "VDET", "VOKE", 15, dd4hep::Position(-7.5904*units::cm,6.9374*units::cm,0.0*units::cm), 48, params, "MANY");
  params.clear();
  this->add_placement(320, "VDET", "VOPI", 29, dd4hep::Position(-7.5712*units::cm,6.9257*units::cm,-20.39*units::cm), 48, params, "MANY");
  params.clear();
  this->add_placement(321, "VDET", "VOPI", 30, dd4hep::Position(-7.5712*units::cm,6.9257*units::cm,20.39*units::cm), 48, params, "MANY");
  params.clear();
  this->add_placement(322, "VDET", "VMEO", 29, dd4hep::Position(-7.7737*units::cm,7.0498*units::cm,-23.1475*units::cm), 48, params, "ONLY");
  params.clear();
  this->add_placement(323, "VDET", "VMEO", 30, dd4hep::Position(-7.7737*units::cm,7.0498*units::cm,23.1475*units::cm), 49, params, "ONLY");
  params.clear();
  this->add_placement(324, "VDET", "VFSI", 16, dd4hep::Position(-10.2219*units::cm,3.3117*units::cm,0.0*units::cm), 50, params, "ONLY");
  params.clear();
  this->add_placement(325, "VDET", "VFPB", 31, dd4hep::Position(-10.4797*units::cm,3.3456*units::cm,-22.24*units::cm), 50, params, "ONLY");
  params.clear();
  this->add_placement(326, "VDET", "VFPB", 32, dd4hep::Position(-10.4797*units::cm,3.3456*units::cm,22.24*units::cm), 50, params, "ONLY");
  params.clear();
  this->add_placement(327, "VDET", "VOUP", 16, dd4hep::Position(-10.1302*units::cm,3.2996*units::cm,0.0*units::cm), 50, params, "ONLY");
  params.clear();
  this->add_placement(328, "VDET", "VOKE", 16, dd4hep::Position(-9.7559*units::cm,3.2503*units::cm,0.0*units::cm), 50, params, "MANY");
  params.clear();
  this->add_placement(329, "VDET", "VOPI", 31, dd4hep::Position(-9.7336*units::cm,3.2474*units::cm,-20.39*units::cm), 50, params, "MANY");
  params.clear();
  this->add_placement(330, "VDET", "VOPI", 32, dd4hep::Position(-9.7336*units::cm,3.2474*units::cm,20.39*units::cm), 50, params, "MANY");
  params.clear();
  this->add_placement(331, "VDET", "VMEO", 31, dd4hep::Position(-9.9691*units::cm,3.2784*units::cm,-23.1475*units::cm), 50, params, "ONLY");
  params.clear();
  this->add_placement(332, "VDET", "VMEO", 32, dd4hep::Position(-9.9691*units::cm,3.2784*units::cm,23.1475*units::cm), 51, params, "ONLY");
  params.clear();
  this->add_placement(333, "VDET", "VFSI", 17, dd4hep::Position(-10.6851*units::cm,-1.1322*units::cm,0.0*units::cm), 52, params, "ONLY");
  params.clear();
  this->add_placement(334, "VDET", "VFPB", 33, dd4hep::Position(-10.9344*units::cm,-1.2061*units::cm,-22.24*units::cm), 52, params, "ONLY");
  params.clear();
  this->add_placement(335, "VDET", "VFPB", 34, dd4hep::Position(-10.9344*units::cm,-1.2061*units::cm,22.24*units::cm), 52, params, "ONLY");
  params.clear();
  this->add_placement(336, "VDET", "VOUP", 17, dd4hep::Position(-10.5964*units::cm,-1.106*units::cm,0.0*units::cm), 52, params, "ONLY");
  params.clear();
  this->add_placement(337, "VDET", "VOKE", 17, dd4hep::Position(-10.2345*units::cm,-0.9988*units::cm,0.0*units::cm), 52, params, "MANY");
  params.clear();
  this->add_placement(338, "VDET", "VOPI", 33, dd4hep::Position(-10.2129*units::cm,-0.9924*units::cm,-20.39*units::cm), 52, params, "MANY");
  params.clear();
  this->add_placement(339, "VDET", "VOPI", 34, dd4hep::Position(-10.2129*units::cm,-0.9924*units::cm,20.39*units::cm), 52, params, "MANY");
  params.clear();
  this->add_placement(340, "VDET", "VMEO", 33, dd4hep::Position(-10.4406*units::cm,-1.0598*units::cm,-23.1475*units::cm), 52, params, "ONLY");
  params.clear();
  this->add_placement(341, "VDET", "VMEO", 34, dd4hep::Position(-10.4406*units::cm,-1.0598*units::cm,23.1475*units::cm), 53, params, "ONLY");
  params.clear();
  this->add_placement(342, "VDET", "VFSI", 18, dd4hep::Position(-9.3008*units::cm,-5.3804*units::cm,0.0*units::cm), 54, params, "ONLY");
  params.clear();
  this->add_placement(343, "VDET", "VFPB", 35, dd4hep::Position(-9.4985*units::cm,-5.5493*units::cm,-22.24*units::cm), 54, params, "ONLY");
  params.clear();
  this->add_placement(344, "VDET", "VFPB", 36, dd4hep::Position(-9.4985*units::cm,-5.5493*units::cm,22.24*units::cm), 54, params, "ONLY");
  params.clear();
  this->add_placement(345, "VDET", "VOUP", 18, dd4hep::Position(-9.2305*units::cm,-5.3203*units::cm,0.0*units::cm), 54, params, "ONLY");
  params.clear();
  this->add_placement(346, "VDET", "VOKE", 18, dd4hep::Position(-8.9434*units::cm,-5.0752*units::cm,0.0*units::cm), 54, params, "MANY");
  params.clear();
  this->add_placement(347, "VDET", "VOPI", 35, dd4hep::Position(-8.9263*units::cm,-5.0605*units::cm,-20.39*units::cm), 54, params, "MANY");
  params.clear();
  this->add_placement(348, "VDET", "VOPI", 36, dd4hep::Position(-8.9263*units::cm,-5.0605*units::cm,20.39*units::cm), 54, params, "MANY");
  params.clear();
  this->add_placement(349, "VDET", "VMEO", 35, dd4hep::Position(-9.1069*units::cm,-5.2148*units::cm,-23.1475*units::cm), 54, params, "ONLY");
  params.clear();
  this->add_placement(350, "VDET", "VMEO", 36, dd4hep::Position(-9.1069*units::cm,-5.2148*units::cm,23.1475*units::cm), 55, params, "ONLY");
  params.clear();
  this->add_placement(351, "VDET", "VFSI", 19, dd4hep::Position(-6.3083*units::cm,-8.6982*units::cm,0.0*units::cm), 56, params, "ONLY");
  params.clear();
  this->add_placement(352, "VDET", "VFPB", 37, dd4hep::Position(-6.4202*units::cm,-8.9329*units::cm,-22.24*units::cm), 56, params, "ONLY");
  params.clear();
  this->add_placement(353, "VDET", "VFPB", 38, dd4hep::Position(-6.4202*units::cm,-8.9329*units::cm,22.24*units::cm), 56, params, "ONLY");
  params.clear();
  this->add_placement(354, "VDET", "VOUP", 19, dd4hep::Position(-6.2685*units::cm,-8.6147*units::cm,0.0*units::cm), 56, params, "ONLY");
  params.clear();
  this->add_placement(355, "VDET", "VOKE", 19, dd4hep::Position(-6.106*units::cm,-8.274*units::cm,0.0*units::cm), 56, params, "MANY");
  params.clear();
  this->add_placement(356, "VDET", "VOPI", 37, dd4hep::Position(-6.0963*units::cm,-8.2537*units::cm,-20.39*units::cm), 56, params, "MANY");
  params.clear();
  this->add_placement(357, "VDET", "VOPI", 38, dd4hep::Position(-6.0963*units::cm,-8.2537*units::cm,20.39*units::cm), 56, params, "MANY");
  params.clear();
  this->add_placement(358, "VDET", "VMEO", 37, dd4hep::Position(-6.1985*units::cm,-8.4681*units::cm,-23.1475*units::cm), 56, params, "ONLY");
  params.clear();
  this->add_placement(359, "VDET", "VMEO", 38, dd4hep::Position(-6.1985*units::cm,-8.4681*units::cm,23.1475*units::cm), 57, params, "ONLY");
  params.clear();
  this->add_placement(360, "VDET", "VFSI", 20, dd4hep::Position(-2.225*units::cm,-10.5121*units::cm,0.0*units::cm), 58, params, "ONLY");
  params.clear();
  this->add_placement(361, "VDET", "VFPB", 39, dd4hep::Position(-2.2318*units::cm,-10.772*units::cm,-22.24*units::cm), 58, params, "ONLY");
  params.clear();
  this->add_placement(362, "VDET", "VFPB", 40, dd4hep::Position(-2.2318*units::cm,-10.772*units::cm,22.24*units::cm), 58, params, "ONLY");
  params.clear();
  this->add_placement(363, "VDET", "VOUP", 20, dd4hep::Position(-2.2226*units::cm,-10.4196*units::cm,0.0*units::cm), 58, params, "ONLY");
  params.clear();
  this->add_placement(364, "VDET", "VOKE", 20, dd4hep::Position(-2.2127*units::cm,-10.0422*units::cm,0.0*units::cm), 58, params, "MANY");
  params.clear();
  this->add_placement(365, "VDET", "VOPI", 39, dd4hep::Position(-2.2121*units::cm,-10.0197*units::cm,-20.39*units::cm), 58, params, "MANY");
  params.clear();
  this->add_placement(366, "VDET", "VOPI", 40, dd4hep::Position(-2.2121*units::cm,-10.0197*units::cm,20.39*units::cm), 58, params, "MANY");
  params.clear();
  this->add_placement(367, "VDET", "VMEO", 39, dd4hep::Position(-2.2184*units::cm,-10.2571*units::cm,-23.1475*units::cm), 58, params, "ONLY");
  params.clear();
  this->add_placement(368, "VDET", "VMEO", 40, dd4hep::Position(-2.2184*units::cm,-10.2571*units::cm,23.1475*units::cm), 59, params, "ONLY");
  params.clear();
  this->add_placement(369, "VDET", "VFSI", 21, dd4hep::Position(2.243*units::cm,-10.5082*units::cm,0.0*units::cm), 60, params, "ONLY");
  params.clear();
  this->add_placement(370, "VDET", "VFPB", 41, dd4hep::Position(2.3425*units::cm,-10.7485*units::cm,-22.24*units::cm), 60, params, "ONLY");
  params.clear();
  this->add_placement(371, "VDET", "VFPB", 42, dd4hep::Position(2.3425*units::cm,-10.7485*units::cm,22.24*units::cm), 60, params, "ONLY");
  params.clear();
  this->add_placement(372, "VDET", "VOUP", 21, dd4hep::Position(2.2076*units::cm,-10.4228*units::cm,0.0*units::cm), 60, params, "ONLY");
  params.clear();
  this->add_placement(373, "VDET", "VOKE", 21, dd4hep::Position(2.0631*units::cm,-10.074*units::cm,0.0*units::cm), 60, params, "MANY");
  params.clear();
  this->add_placement(374, "VDET", "VOPI", 41, dd4hep::Position(2.0545*units::cm,-10.0532*units::cm,-20.39*units::cm), 60, params, "MANY");
  params.clear();
  this->add_placement(375, "VDET", "VOPI", 42, dd4hep::Position(2.0545*units::cm,-10.0532*units::cm,20.39*units::cm), 60, params, "MANY");
  params.clear();
  this->add_placement(376, "VDET", "VMEO", 41, dd4hep::Position(2.1454*units::cm,-10.2727*units::cm,-23.1475*units::cm), 60, params, "ONLY");
  params.clear();
  this->add_placement(377, "VDET", "VMEO", 42, dd4hep::Position(2.1454*units::cm,-10.2727*units::cm,23.1475*units::cm), 61, params, "ONLY");
  params.clear();
  this->add_placement(378, "VDET", "VFSI", 22, dd4hep::Position(6.3232*units::cm,-8.6874*units::cm,0.0*units::cm), 62, params, "ONLY");
  params.clear();
  this->add_placement(379, "VDET", "VFPB", 43, dd4hep::Position(6.5118*units::cm,-8.8664*units::cm,-22.24*units::cm), 62, params, "ONLY");
  params.clear();
  this->add_placement(380, "VDET", "VFPB", 44, dd4hep::Position(6.5118*units::cm,-8.8664*units::cm,22.24*units::cm), 62, params, "ONLY");
  params.clear();
  this->add_placement(381, "VDET", "VOUP", 22, dd4hep::Position(6.2561*units::cm,-8.6238*units::cm,0.0*units::cm), 62, params, "ONLY");
  params.clear();
  this->add_placement(382, "VDET", "VOKE", 22, dd4hep::Position(5.9822*units::cm,-8.3639*units::cm,0.0*units::cm), 62, params, "MANY");
  params.clear();
  this->add_placement(383, "VDET", "VOPI", 43, dd4hep::Position(5.9659*units::cm,-8.3484*units::cm,-20.39*units::cm), 62, params, "MANY");
  params.clear();
  this->add_placement(384, "VDET", "VOPI", 44, dd4hep::Position(5.9659*units::cm,-8.3484*units::cm,20.39*units::cm), 62, params, "MANY");
  params.clear();
  this->add_placement(385, "VDET", "VMEO", 43, dd4hep::Position(6.1382*units::cm,-8.5119*units::cm,-23.1475*units::cm), 62, params, "ONLY");
  params.clear();
  this->add_placement(386, "VDET", "VMEO", 44, dd4hep::Position(6.1382*units::cm,-8.5119*units::cm,23.1475*units::cm), 63, params, "ONLY");
  params.clear();
  this->add_placement(387, "VDET", "VFSI", 23, dd4hep::Position(9.31*units::cm,-5.3645*units::cm,0.0*units::cm), 64, params, "ONLY");
  params.clear();
  this->add_placement(388, "VDET", "VFPB", 45, dd4hep::Position(9.5551*units::cm,-5.4513*units::cm,-22.24*units::cm), 64, params, "ONLY");
  params.clear();
  this->add_placement(389, "VDET", "VFPB", 46, dd4hep::Position(9.5551*units::cm,-5.4513*units::cm,22.24*units::cm), 64, params, "ONLY");
  params.clear();
  this->add_placement(390, "VDET", "VOUP", 23, dd4hep::Position(9.2228*units::cm,-5.3336*units::cm,0.0*units::cm), 64, params, "ONLY");
  params.clear();
  this->add_placement(391, "VDET", "VOKE", 23, dd4hep::Position(8.867*units::cm,-5.2076*units::cm,0.0*units::cm), 64, params, "MANY");
  params.clear();
  this->add_placement(392, "VDET", "VOPI", 45, dd4hep::Position(8.8458*units::cm,-5.2001*units::cm,-20.39*units::cm), 64, params, "MANY");
  params.clear();
  this->add_placement(393, "VDET", "VOPI", 46, dd4hep::Position(8.8458*units::cm,-5.2001*units::cm,20.39*units::cm), 64, params, "MANY");
  params.clear();
  this->add_placement(394, "VDET", "VMEO", 45, dd4hep::Position(9.0696*units::cm,-5.2794*units::cm,-23.1475*units::cm), 64, params, "ONLY");
  params.clear();
  this->add_placement(395, "VDET", "VMEO", 46, dd4hep::Position(9.0696*units::cm,-5.2794*units::cm,23.1475*units::cm), 65, params, "ONLY");
  params.clear();
  this->add_placement(396, "VDET", "VFSI", 24, dd4hep::Position(10.6871*units::cm,-1.114*units::cm,0.0*units::cm), 66, params, "ONLY");
  params.clear();
  this->add_placement(397, "VDET", "VFPB", 47, dd4hep::Position(10.9462*units::cm,-1.0936*units::cm,-22.24*units::cm), 66, params, "ONLY");
  params.clear();
  this->add_placement(398, "VDET", "VFPB", 48, dd4hep::Position(10.9462*units::cm,-1.0936*units::cm,22.24*units::cm), 66, params, "ONLY");
  params.clear();
  this->add_placement(399, "VDET", "VOUP", 24, dd4hep::Position(10.5948*units::cm,-1.1213*units::cm,0.0*units::cm), 66, params, "ONLY");
  params.clear();
  this->add_placement(400, "VDET", "VOKE", 24, dd4hep::Position(10.2185*units::cm,-1.1509*units::cm,0.0*units::cm), 66, params, "MANY");
  params.clear();
  this->add_placement(401, "VDET", "VOPI", 47, dd4hep::Position(10.1961*units::cm,-1.1527*units::cm,-20.39*units::cm), 66, params, "MANY");
  params.clear();
  this->add_placement(402, "VDET", "VOPI", 48, dd4hep::Position(10.1961*units::cm,-1.1527*units::cm,20.39*units::cm), 66, params, "MANY");
  params.clear();
  this->add_placement(403, "VDET", "VMEO", 47, dd4hep::Position(10.4328*units::cm,-1.134*units::cm,-23.1475*units::cm), 66, params, "ONLY");
  params.clear();
  this->add_placement(404, "VDET", "VMEO", 48, dd4hep::Position(10.4328*units::cm,-1.134*units::cm,23.1475*units::cm), 67, params, "ONLY");
  params.clear();
  this->add_placement(405, "CDET", "ITCR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(406, "ITCR", "ITC", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(407, "ITCR", "ITND", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,118.05*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(408, "ITCR", "ITND", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-118.05*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(409, "ITC", "ITWI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(410, "ITC", "ITWO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(411, "ITND", "ITEP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-16.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(412, "ITND", "ITRG", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-14.1*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(413, "ITND", "ITSP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,1.25*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(414, "ITSP", "ITEL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(415, "ITSU", "ITCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(416, "PASV", "ITSU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-188.05*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(417, "PASW", "ITSU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-188.05*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(418, "CDET", "TPCR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(419, "TPC", "TPHF", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,110.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(420, "TPC", "TPHF", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-110.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(421, "TPCR", "TPC", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(422, "TPCR", "TPWI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(423, "TPCR", "TPWO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(424, "TPHF", "TPIS", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,100.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(425, "TPHF", "TPMI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-109.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(426, "TPHF", "TPMO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-109.8*units::cm), 0, params, "ONLY");
  params = { 36.4, 175.4, 0.0006 };
  this->add_placement(427, "TPHF", "TPMB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-109.9994*units::cm), 0, params, "ONLY");
  params = { 34.3, 36.4, 0.0156 };
  this->add_placement(428, "TPHF", "TPMB", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-109.9844*units::cm), 0, params, "ONLY");
  params = { 175.4, 176.9, 0.0156 };
  this->add_placement(429, "TPHF", "TPMB", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,-109.9844*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(430, "TPHF", "TPMG", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-109.541*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(431, "TPHF", "TPRC", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-9.591*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(432, "TPHF", "TPR1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-106.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(433, "TPHF", "TPR2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-106.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(434, "TPHF", "TPR3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-106.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(435, "TPHF", "TM11", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-96.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(436, "TPHF", "TM12", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-96.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(437, "TPHF", "TM13", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-96.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(438, "TPHF", "TM21", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-70.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(439, "TPHF", "TM22", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-70.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(440, "TPHF", "TM23", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-70.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(441, "TPHF", "TM31", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-54.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(442, "TPHF", "TM32", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-54.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(443, "TPHF", "TM33", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-54.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(444, "TPHF", "TM41", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-28.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(445, "TPHF", "TM42", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-28.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(446, "TPHF", "TM43", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-28.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(447, "TPCR", "TPEP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,230.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(448, "TPCR", "TPEP", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-230.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(449, "ALEF", "ECBL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(450, "ALEF", "ECEA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 1, params, "ONLY");
  params.clear();
  this->add_placement(451, "ALEF", "ECEB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(452, "ECBL", "EBAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(453, "EBAR", "EBRA", 1, dd4hep::Position(238.72*units::cm,0.0*units::cm,0.0*units::cm), 68, params, "ONLY");
  params.clear();
  this->add_placement(454, "EBAR", "EBMO", 1, dd4hep::Position(207.07*units::cm,0.0*units::cm,0.0*units::cm), 68, params, "ONLY");
  params.clear();
  this->add_placement(455, "EBMO", "EBIN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.95*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(456, "EBIN", "EBND", 1, dd4hep::Position(0.0*units::cm,234.95*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(457, "EBIN", "EBND", 2, dd4hep::Position(0.0*units::cm,-234.95*units::cm,0.0*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(458, "EBND", "EBEP", 1, dd4hep::Position(0.0*units::cm,2.35*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(459, "EBIN", "EBS1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-15.405*units::cm), 0, params, "ONLY");
  params = { 50.8051, 50.939, 231.5, 0.25 };
  this->add_placement(460, "EBIN", "EBSP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-11.24*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(461, "EBIN", "EBS2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-1.985*units::cm), 0, params, "ONLY");
  params = { 55.7648, 55.8988, 231.5, 0.25 };
  this->add_placement(462, "EBIN", "EBSP", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,7.27*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(463, "EBIN", "EBS3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,13.42*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(464, "ECEA", "ECPA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 69, params, "ONLY");
  params.clear();
  this->add_placement(465, "ECEB", "ECPB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 70, params, "ONLY");
  params.clear();
  this->add_placement(466, "ECMA", "ECMO", 1, dd4hep::Position(144.4*units::cm,0.0*units::cm,278.625*units::cm), 71, params, "ONLY");
  params.clear();
  this->add_placement(467, "ECMB", "ECMO", 1, dd4hep::Position(144.4*units::cm,0.0*units::cm,278.625*units::cm), 71, params, "ONLY");
  params = { 11.9602, 58.7977, 1.12, 87.4 };
  this->add_placement(468, "ECMO", "ECFX", 1, dd4hep::Position(0.0*units::cm,-27.005*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 9.6582, 54.888, 1.75, 84.4 };
  this->add_placement(469, "ECMO", "ECBX", 1, dd4hep::Position(0.0*units::cm,26.375*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(470, "ECMO", "ECIN", 1, dd4hep::Position(0.0*units::cm,-3.18*units::cm,-0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(471, "ECIN", "ECRB", 1, dd4hep::Position(0.0*units::cm,-20.555*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 12.476, 56.0628, 0.416, 81.334 };
  this->add_placement(472, "ECIN", "ECS0", 1, dd4hep::Position(-0.88*units::cm,-19.889*units::cm,-6.266*units::cm), 0, params, "ONLY");
  params = { 56.0628, 26.0871, 0.416, 4.016 };
  this->add_placement(473, "ECIN", "EDS0", 1, dd4hep::Position(-0.88*units::cm,-19.889*units::cm,79.084*units::cm), 0, params, "ONLY");
  params = { 12.476, 56.0628, 3.744, 81.334 };
  this->add_placement(474, "ECIN", "ECS1", 1, dd4hep::Position(-0.88*units::cm,-15.729*units::cm,-6.266*units::cm), 0, params, "ONLY");
  params = { 56.0628, 26.0871, 3.744, 4.016 };
  this->add_placement(475, "ECIN", "EDS1", 1, dd4hep::Position(-0.88*units::cm,-15.729*units::cm,79.084*units::cm), 0, params, "ONLY");
  params = { 12.476, 56.0628, 0.3175, 81.334 };
  this->add_placement(476, "ECIN", "ECSP", 1, dd4hep::Position(-0.88*units::cm,-11.6675*units::cm,-6.266*units::cm), 0, params, "ONLY");
  params = { 56.0628, 26.0871, 0.3175, 4.016 };
  this->add_placement(477, "ECIN", "ECSP", 2, dd4hep::Position(-0.88*units::cm,-11.6675*units::cm,79.084*units::cm), 0, params, "ONLY");
  params = { 12.476, 56.0628, 9.568, 81.334 };
  this->add_placement(478, "ECIN", "ECS2", 1, dd4hep::Position(-0.88*units::cm,-1.782*units::cm,-6.266*units::cm), 0, params, "ONLY");
  params = { 56.0628, 26.0871, 9.568, 4.016 };
  this->add_placement(479, "ECIN", "EDS2", 1, dd4hep::Position(-0.88*units::cm,-1.782*units::cm,79.084*units::cm), 0, params, "ONLY");
  params = { 12.476, 56.0628, 0.3175, 81.334 };
  this->add_placement(480, "ECIN", "ECSP", 3, dd4hep::Position(-0.88*units::cm,8.1035*units::cm,-6.266*units::cm), 0, params, "ONLY");
  params = { 56.0628, 26.0871, 0.3175, 4.016 };
  this->add_placement(481, "ECIN", "ECSP", 4, dd4hep::Position(-0.88*units::cm,8.1035*units::cm,79.084*units::cm), 0, params, "ONLY");
  params = { 12.476, 56.0628, 6.192, 81.334 };
  this->add_placement(482, "ECIN", "ECS3", 1, dd4hep::Position(-0.88*units::cm,14.613*units::cm,-6.266*units::cm), 0, params, "ONLY");
  params = { 56.0628, 26.0871, 6.192, 4.016 };
  this->add_placement(483, "ECIN", "EDS3", 1, dd4hep::Position(-0.88*units::cm,14.613*units::cm,79.084*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(484, "ECIN", "ECNE", 1, dd4hep::Position(0.0*units::cm,0.25*units::cm,-88.6825*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(485, "ECEA", "EPBA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,244.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(486, "ECEB", "EPBB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,244.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(487, "ECEA", "EPAA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,281.9*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(488, "ECEB", "EPAB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,281.9*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 12.6899, 17.3102 };
  this->add_placement(489, "EPAA", "EPCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 12.6899, 17.3102 };
  this->add_placement(490, "EPAB", "EPCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 42.6899, 47.3102 };
  this->add_placement(491, "EPAA", "EPCA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 42.6899, 47.3102 };
  this->add_placement(492, "EPAB", "EPCA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 72.6899, 77.3102 };
  this->add_placement(493, "EPAA", "EPCA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 72.6899, 77.3102 };
  this->add_placement(494, "EPAB", "EPCA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 102.6899, 107.3102 };
  this->add_placement(495, "EPAA", "EPCA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 102.6899, 107.3102 };
  this->add_placement(496, "EPAB", "EPCA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 132.6899, 137.3102 };
  this->add_placement(497, "EPAA", "EPCA", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 132.6899, 137.3102 };
  this->add_placement(498, "EPAB", "EPCA", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 162.6899, 167.3102 };
  this->add_placement(499, "EPAA", "EPCA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 162.6899, 167.3102 };
  this->add_placement(500, "EPAB", "EPCA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 192.6899, 197.3102 };
  this->add_placement(501, "EPAA", "EPCA", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 192.6899, 197.3102 };
  this->add_placement(502, "EPAB", "EPCA", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 222.6899, 227.3102 };
  this->add_placement(503, "EPAA", "EPCA", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 222.6899, 227.3102 };
  this->add_placement(504, "EPAB", "EPCA", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 252.6899, 257.3102 };
  this->add_placement(505, "EPAA", "EPCA", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 252.6899, 257.3102 };
  this->add_placement(506, "EPAB", "EPCA", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 282.6899, 287.3102 };
  this->add_placement(507, "EPAA", "EPCA", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 282.6899, 287.3102 };
  this->add_placement(508, "EPAB", "EPCA", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 312.6899, 317.3102 };
  this->add_placement(509, "EPAA", "EPCA", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 312.6899, 317.3102 };
  this->add_placement(510, "EPAB", "EPCA", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 342.6899, 347.3102 };
  this->add_placement(511, "EPAA", "EPCA", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 6.0, 342.6899, 347.3102 };
  this->add_placement(512, "EPAB", "EPCA", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,27.1*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, -4.3522, 4.3522 };
  this->add_placement(513, "EPBA", "ETHF", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, -4.3522, 4.3522 };
  this->add_placement(514, "EPBB", "ETHF", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, 175.6476, 184.352 };
  this->add_placement(515, "EPBA", "ETHF", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, 175.6476, 184.352 };
  this->add_placement(516, "EPBB", "ETHF", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, 85.6474, 94.3524 };
  this->add_placement(517, "EPBA", "ETVF", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, 85.6474, 94.3524 };
  this->add_placement(518, "EPBB", "ETVF", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, 265.6472, 274.3522 };
  this->add_placement(519, "EPBA", "ETVF", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 4.25, 265.6472, 274.3522 };
  this->add_placement(520, "EPBB", "ETVF", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 4.3522, 9.9998 };
  this->add_placement(521, "EPBA", "ETCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 0.55, 9.9998, 20.0002 };
  this->add_placement(522, "EPBA", "ETCA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.475, 20.0002, 30.0001 };
  this->add_placement(523, "EPBA", "ETCA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.475, 30.0001, 39.9999 };
  this->add_placement(524, "EPBA", "ETCA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 49.9997, 60.0001 };
  this->add_placement(525, "EPBA", "ETCA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 60.0001, 69.9999 };
  this->add_placement(526, "EPBA", "ETCA", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 69.9999, 79.9998 };
  this->add_placement(527, "EPBA", "ETCA", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 79.9998, 85.6474 };
  this->add_placement(528, "EPBA", "ETCA", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 0.55, 94.3524, 100.0 };
  this->add_placement(529, "EPBA", "ETCA", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 109.9999, 120.0002 };
  this->add_placement(530, "EPBA", "ETCA", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 120.0002, 130.0001 };
  this->add_placement(531, "EPBA", "ETCA", 13, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 130.0001, 139.9999 };
  this->add_placement(532, "EPBA", "ETCA", 14, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 139.9999, 149.9997 };
  this->add_placement(533, "EPBA", "ETCA", 15, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 149.9997, 164.9998 };
  this->add_placement(534, "EPBA", "ETCA", 16, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 164.9998, 175.6476 };
  this->add_placement(535, "EPBA", "ETCA", 17, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 184.352, 194.9999 };
  this->add_placement(536, "EPBA", "ETCA", 18, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 194.9999, 209.9999 };
  this->add_placement(537, "EPBA", "ETCA", 19, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 209.9999, 219.9997 };
  this->add_placement(538, "EPBA", "ETCA", 20, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 230.0001, 239.9999 };
  this->add_placement(539, "EPBA", "ETCA", 22, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 239.9999, 255.0 };
  this->add_placement(540, "EPBA", "ETCA", 23, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 3.3, 255.0, 265.6472 };
  this->add_placement(541, "EPBA", "ETCA", 24, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 274.3522, 279.9999 };
  this->add_placement(542, "EPBA", "ETCA", 25, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 279.9999, 290.0002 };
  this->add_placement(543, "EPBA", "ETCA", 26, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 290.0002, 300.0001 };
  this->add_placement(544, "EPBA", "ETCA", 27, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 300.0001, 309.9999 };
  this->add_placement(545, "EPBA", "ETCA", 28, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 309.9999, 319.9998 };
  this->add_placement(546, "EPBA", "ETCA", 29, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 319.9998, 330.0002 };
  this->add_placement(547, "EPBA", "ETCA", 30, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 330.0002, 340.0 };
  this->add_placement(548, "EPBA", "ETCA", 31, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 4.3522, 9.9998 };
  this->add_placement(549, "EPBB", "ETCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 9.9998, 20.0002 };
  this->add_placement(550, "EPBB", "ETCA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 20.0002, 30.0001 };
  this->add_placement(551, "EPBB", "ETCA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 30.0001, 39.9999 };
  this->add_placement(552, "EPBB", "ETCA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 0.55, 39.9999, 49.9997 };
  this->add_placement(553, "EPBB", "ETCA", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 49.9997, 60.0001 };
  this->add_placement(554, "EPBB", "ETCA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 60.0001, 69.9999 };
  this->add_placement(555, "EPBB", "ETCA", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 69.9999, 79.9998 };
  this->add_placement(556, "EPBB", "ETCA", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 79.9998, 85.6474 };
  this->add_placement(557, "EPBB", "ETCA", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 94.3524, 100.0 };
  this->add_placement(558, "EPBB", "ETCA", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 109.9999, 120.0002 };
  this->add_placement(559, "EPBB", "ETCA", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 120.0002, 130.0001 };
  this->add_placement(560, "EPBB", "ETCA", 13, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 130.0001, 139.9999 };
  this->add_placement(561, "EPBB", "ETCA", 14, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.75, 139.9999, 149.9997 };
  this->add_placement(562, "EPBB", "ETCA", 15, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 149.9997, 160.0002 };
  this->add_placement(563, "EPBB", "ETCA", 16, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 160.0002, 170.0 };
  this->add_placement(564, "EPBB", "ETCA", 17, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 0.55, 170.0, 175.6476 };
  this->add_placement(565, "EPBB", "ETCA", 18, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.375, 184.352, 190.0002 };
  this->add_placement(566, "EPBB", "ETCA", 19, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.375, 190.0002, 200.0001 };
  this->add_placement(567, "EPBB", "ETCA", 20, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.375, 200.0001, 209.9999 };
  this->add_placement(568, "EPBB", "ETCA", 21, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 2.2, 209.9999, 219.9997 };
  this->add_placement(569, "EPBB", "ETCA", 22, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 230.0001, 239.9999 };
  this->add_placement(570, "EPBB", "ETCA", 24, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 239.9999, 249.9998 };
  this->add_placement(571, "EPBB", "ETCA", 25, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 249.9998, 260.0002 };
  this->add_placement(572, "EPBB", "ETCA", 26, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 260.0002, 265.6472 };
  this->add_placement(573, "EPBB", "ETCA", 27, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.1, 274.3522, 279.9999 };
  this->add_placement(574, "EPBB", "ETCA", 28, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 0.55, 279.9999, 290.0002 };
  this->add_placement(575, "EPBB", "ETCA", 29, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 290.0002, 300.0001 };
  this->add_placement(576, "EPBB", "ETCA", 30, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 300.0001, 309.9999 };
  this->add_placement(577, "EPBB", "ETCA", 31, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 309.9999, 319.9998 };
  this->add_placement(578, "EPBB", "ETCA", 32, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 319.9998, 330.0002 };
  this->add_placement(579, "EPBB", "ETCA", 33, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 1.65, 330.0002, 340.0 };
  this->add_placement(580, "EPBB", "ETCA", 34, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 180.0, 248.0, 0.55, 349.9998, 355.648 };
  this->add_placement(581, "EPBB", "ETCA", 36, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 24.7478, 35.2518 };
  this->add_placement(582, "EPAA", "ETCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 24.7478, 35.2518 };
  this->add_placement(583, "EPAB", "ETCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 54.7478, 65.2518 };
  this->add_placement(584, "EPAA", "ETCA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 54.7478, 65.2518 };
  this->add_placement(585, "EPAB", "ETCA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 114.7478, 125.2518 };
  this->add_placement(586, "EPAA", "ETCA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 114.7478, 125.2518 };
  this->add_placement(587, "EPAB", "ETCA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 204.7478, 215.2518 };
  this->add_placement(588, "EPAA", "ETCA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 204.7478, 215.2518 };
  this->add_placement(589, "EPAB", "ETCA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 234.7478, 245.2518 };
  this->add_placement(590, "EPAA", "ETCA", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 234.7478, 245.2518 };
  this->add_placement(591, "EPAB", "ETCA", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 324.7478, 335.2518 };
  this->add_placement(592, "EPAA", "ETCA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 324.7478, 335.2518 };
  this->add_placement(593, "EPAB", "ETCA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 144.748, 155.252 };
  this->add_placement(594, "EPAA", "EICA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 144.748, 155.252 };
  this->add_placement(595, "EPAB", "EICA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 294.748, 305.252 };
  this->add_placement(596, "EPAA", "EICA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 243.0829, 248.0, 33.1, 294.748, 305.252 };
  this->add_placement(597, "EPAB", "EICA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(598, "ECBX", "ECSU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(599, "ECEA", "LCEA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,285.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(600, "ECEB", "LCEB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,285.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(601, "LCEA", "LCMO", 1, dd4hep::Position(0.4*units::cm,0.1*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(602, "LCEA", "LCSH", 1, dd4hep::Position(0.4*units::cm,0.1*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(603, "LCEA", "LCMO", 2, dd4hep::Position(-0.0*units::cm,0.1*units::cm,0.0*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(604, "LCEA", "LCSH", 2, dd4hep::Position(-0.0*units::cm,0.1*units::cm,0.0*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(605, "LCEB", "LCMO", 1, dd4hep::Position(-0.2*units::cm,0.2*units::cm,0.0*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(606, "LCEB", "LCSH", 1, dd4hep::Position(-0.2*units::cm,0.2*units::cm,0.0*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(607, "LCEB", "LCMO", 2, dd4hep::Position(0.2*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(608, "LCEB", "LCSH", 2, dd4hep::Position(0.2*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(609, "ECEA", "LSCP", 1, dd4hep::Position(0.0*units::cm,30.75*units::cm,308.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(610, "ECEA", "LSCP", 2, dd4hep::Position(0.0*units::cm,-30.75*units::cm,308.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(611, "ECEB", "LSCP", 1, dd4hep::Position(0.0*units::cm,30.75*units::cm,308.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(612, "ECEB", "LSCP", 2, dd4hep::Position(0.0*units::cm,-30.75*units::cm,308.35*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(613, "ECEA", "LSCI", 1, dd4hep::Position(0.0*units::cm,30.6*units::cm,309.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(614, "ECEA", "LSCI", 2, dd4hep::Position(0.0*units::cm,-30.6*units::cm,309.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(615, "ECEB", "LSCI", 1, dd4hep::Position(0.0*units::cm,30.6*units::cm,309.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(616, "ECEB", "LSCI", 2, dd4hep::Position(0.0*units::cm,-30.6*units::cm,309.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(617, "LCMO", "LSBR", 1, dd4hep::Position(4.4*units::cm,28.835*units::cm,-20.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(618, "LCMO", "LSBR", 2, dd4hep::Position(4.4*units::cm,-28.835*units::cm,-20.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(619, "LCMO", "LSFR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-20.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(620, "LSFR", "LSCR", 1, dd4hep::Position(7.0*units::cm,-12.1244*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(621, "LSFR", "LSCR", 2, dd4hep::Position(9.8995*units::cm,-9.8995*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(622, "LSFR", "LSCR", 3, dd4hep::Position(12.1243*units::cm,-7.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(623, "LSFR", "LSCR", 4, dd4hep::Position(13.523*units::cm,-3.6235*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(624, "LSFR", "LSCR", 5, dd4hep::Position(14.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(625, "LSFR", "LSCR", 6, dd4hep::Position(13.523*units::cm,3.6235*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(626, "LSFR", "LSCR", 7, dd4hep::Position(12.1243*units::cm,7.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(627, "LSFR", "LSCR", 8, dd4hep::Position(9.8995*units::cm,9.8995*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(628, "LSFR", "LSCR", 9, dd4hep::Position(7.0*units::cm,12.1244*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(629, "LCMO", "LCIN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.65*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(630, "LCMO", "LCBP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,21.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(631, "LCIN", "LCSI", 1, dd4hep::Position(0.5*units::cm,11.25*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(632, "LCIN", "LCSI", 2, dd4hep::Position(0.5*units::cm,-11.25*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(633, "LCIN", "LCSO", 1, dd4hep::Position(0.5*units::cm,50.9*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(634, "LCIN", "LCSO", 2, dd4hep::Position(0.5*units::cm,-50.9*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(635, "LCIN", "LC12", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-6.3415*units::cm), 0, params, "ONLY");
  params = { 0.5, 19.55, 12.6585 };
  this->add_placement(636, "LC12", "LCSW", 1, dd4hep::Position(0.5*units::cm,31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.5, 19.55, 12.6585 };
  this->add_placement(637, "LC12", "LCSW", 2, dd4hep::Position(0.5*units::cm,-31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.25, 19.55, 12.6585 };
  this->add_placement(638, "LC12", "LCSG", 1, dd4hep::Position(1.25*units::cm,31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.25, 19.55, 12.6585 };
  this->add_placement(639, "LC12", "LCSG", 2, dd4hep::Position(1.25*units::cm,-31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 2.0, 12.6585 };
  this->add_placement(640, "LC12", "LCDL", 1, dd4hep::Position(4.7*units::cm,47.3*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 2.0, 12.6585 };
  this->add_placement(641, "LC12", "LCDL", 2, dd4hep::Position(4.7*units::cm,-47.3*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 12.6585, 0.0, 0.0, 3.125, 1.0, 0.0001, 0.16, 3.125, 1.0, 0.0001, 0.16, 0.0, 0.0, -1.0, -12.6585, 0.0, 0.0, 1.0, -12.6585, 0.0, -1.0, 0.0, -3.125, 0.0, 1.0, 0.0, -3.125, -0.9524, 0.3048, 0.0, -0.4763, 1.0, -0.0, 0.0, -0.5001 };
  this->add_placement(642, "LC12", "LEAD", 1, dd4hep::Position(11.0*units::cm,-3.125*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 12.6585, 0.0, 0.0, 3.125, 1.0, 0.0001, 0.16, 3.125, 1.0, 0.0001, 0.16, 0.0, 0.0, -1.0, -12.6585, 0.0, 0.0, 1.0, -12.6585, 0.0, -1.0, 0.0, -3.125, 0.0, 1.0, 0.0, -3.125, -0.9524, 0.3048, 0.0, -0.4763, 1.0, -0.0, 0.0, -0.5001 };
  this->add_placement(643, "LC12", "LEAD", 2, dd4hep::Position(11.0*units::cm,3.125*units::cm,0.0*units::cm), 72, params, "ONLY");
  params = { 12.6585, 0.0, 0.0, 1.5, 1.1, 0.0001, 0.6333, 1.5, 1.1, 0.0001, 0.6333, 0.0, 0.0, -1.0, -12.6585, 0.0, 0.0, 1.0, -12.6585, 0.0, -1.0, 0.0, -1.5, 0.0, 1.0, 0.0, -1.5, -0.7071, 0.7071, 0.0, -0.389, 0.9662, -0.2577, 0.0, -0.5315 };
  this->add_placement(644, "LC12", "LEAD", 3, dd4hep::Position(3.0*units::cm,11.5*units::cm,0.0*units::cm), 73, params, "ONLY");
  params = { 12.6585, 0.0, 0.0, 1.5, 1.1, 0.0001, 0.6333, 1.5, 1.1, 0.0001, 0.6333, 0.0, 0.0, -1.0, -12.6585, 0.0, 0.0, 1.0, -12.6585, 0.0, -1.0, 0.0, -1.5, 0.0, 1.0, 0.0, -1.5, -0.7071, 0.7071, 0.0, -0.389, 0.9662, -0.2577, 0.0, -0.5315 };
  this->add_placement(645, "LC12", "LEAD", 4, dd4hep::Position(3.0*units::cm,-11.5*units::cm,0.0*units::cm), 74, params, "ONLY");
  params.clear();
  this->add_placement(646, "LCIN", "LCS3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,11.5055*units::cm), 0, params, "ONLY");
  params = { 0.5, 19.55, 5.1885 };
  this->add_placement(647, "LCS3", "LCSW", 1, dd4hep::Position(0.5*units::cm,31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.5, 19.55, 5.1885 };
  this->add_placement(648, "LCS3", "LCSW", 2, dd4hep::Position(0.5*units::cm,-31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.25, 19.55, 5.1885 };
  this->add_placement(649, "LCS3", "LCSG", 1, dd4hep::Position(1.25*units::cm,31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.25, 19.55, 5.1885 };
  this->add_placement(650, "LCS3", "LCSG", 2, dd4hep::Position(1.25*units::cm,-31.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 2.0, 5.1885 };
  this->add_placement(651, "LCS3", "LCDL", 1, dd4hep::Position(4.7*units::cm,47.3*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 2.0, 5.1885 };
  this->add_placement(652, "LCS3", "LCDL", 2, dd4hep::Position(4.7*units::cm,-47.3*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 5.1885, 0.0, 0.0, 3.125, 1.0, 0.0001, 0.16, 3.125, 1.0, 0.0001, 0.16, 0.0, 0.0, -1.0, -5.1885, 0.0, 0.0, 1.0, -5.1885, 0.0, -1.0, 0.0, -3.125, 0.0, 1.0, 0.0, -3.125, -0.9524, 0.3048, 0.0, -0.4763, 1.0, -0.0, 0.0, -0.5001 };
  this->add_placement(653, "LCS3", "LEAD", 1, dd4hep::Position(11.0*units::cm,-3.125*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 5.1885, 0.0, 0.0, 3.125, 1.0, 0.0001, 0.16, 3.125, 1.0, 0.0001, 0.16, 0.0, 0.0, -1.0, -5.1885, 0.0, 0.0, 1.0, -5.1885, 0.0, -1.0, 0.0, -3.125, 0.0, 1.0, 0.0, -3.125, -0.9524, 0.3048, 0.0, -0.4763, 1.0, -0.0, 0.0, -0.5001 };
  this->add_placement(654, "LCS3", "LEAD", 2, dd4hep::Position(11.0*units::cm,3.125*units::cm,0.0*units::cm), 72, params, "ONLY");
  params = { 5.1885, 0.0, 0.0, 1.5, 1.1, 0.0001, 0.6333, 1.5, 1.1, 0.0001, 0.6333, 0.0, 0.0, -1.0, -5.1885, 0.0, 0.0, 1.0, -5.1885, 0.0, -1.0, 0.0, -1.5, 0.0, 1.0, 0.0, -1.5, -0.7071, 0.7071, 0.0, -0.389, 0.9662, -0.2577, 0.0, -0.5315 };
  this->add_placement(655, "LCS3", "LEAD", 3, dd4hep::Position(3.0*units::cm,11.5*units::cm,0.0*units::cm), 73, params, "ONLY");
  params = { 5.1885, 0.0, 0.0, 1.5, 1.1, 0.0001, 0.6333, 1.5, 1.1, 0.0001, 0.6333, 0.0, 0.0, -1.0, -5.1885, 0.0, 0.0, 1.0, -5.1885, 0.0, -1.0, 0.0, -1.5, 0.0, 1.0, 0.0, -1.5, -0.7071, 0.7071, 0.0, -0.389, 0.9662, -0.2577, 0.0, -0.5315 };
  this->add_placement(656, "LCS3", "LEAD", 4, dd4hep::Position(3.0*units::cm,-11.5*units::cm,0.0*units::cm), 74, params, "ONLY");
  params.clear();
  this->add_placement(657, "LCIN", "LCCA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,17.845*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(658, "PASV", "SMBA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-1.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(659, "PASW", "SMBA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-1.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(660, "SMBA", "SMCH", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.7845*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(661, "SMBA", "SMCH", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.7845*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(662, "SMCH", "SMIN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.3045*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(663, "SMCH", "SMGA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.4815*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(664, "SMCH", "SMCP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.733*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(665, "SMCH", "SMCU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.23*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(666, "SMGA", "SMCU", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.177*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(667, "PASV", "SMEL", 1, dd4hep::Position(-15.0*units::cm,-5.5*units::cm,6.6*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(668, "PASW", "SMEL", 1, dd4hep::Position(-15.0*units::cm,-5.5*units::cm,6.6*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(669, "PASV", "SMEL", 2, dd4hep::Position(-5.5*units::cm,-15.0*units::cm,6.6*units::cm), 4, params, "ONLY");
  params.clear();
  this->add_placement(670, "PASW", "SMEL", 2, dd4hep::Position(-5.5*units::cm,-15.0*units::cm,6.6*units::cm), 4, params, "ONLY");
  params.clear();
  this->add_placement(671, "ECEA", "SCAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,253.75*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(672, "ECEB", "SCAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,253.75*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(673, "SCAL", "STOP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(674, "STOP", "SABP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,8.46*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(675, "STOP", "SBOX", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,1.964*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(676, "STOP", "SAFR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-6.496*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(677, "SBOX", "SLUM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(678, "SBOX", "SSUP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(679, "SLUM", "SACT", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(680, "SLUM", "SPAS", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(681, "SSUP", "SCPF", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-6.056*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(682, "SSUP", "SCPS", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.185*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(683, "SSUP", "SCPB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,5.871*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(684, "SSUP", "SCOO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(685, "SSUP", "SCOO", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.15*units::cm), 3, params, "ONLY");
  params.clear();
  this->add_placement(686, "SSUP", "SCO1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(687, "SSUP", "SCO2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(688, "SSUP", "SCO3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(689, "SSUP", "SCO4", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.15*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(690, "SACT", "SWFR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-6.056*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(691, "SACT", "SMOL", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,5.683*units::cm), 76, params, "ONLY");
  params.clear();
  this->add_placement(692, "SPAS", "SCAP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-6.056*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(693, "SPAS", "SMDX", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.373*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(694, "SPAS", "SMDL", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,5.683*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(695, "SMDX", "SMDA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(696, "SMDX", "SMDC", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(697, "SMDX", "SINX", 1, dd4hep::Position(1.4*units::cm,24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(698, "SMDX", "SINX", 2, dd4hep::Position(-1.4*units::cm,24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(699, "SMDX", "SINX", 3, dd4hep::Position(-1.4*units::cm,-24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(700, "SMDX", "SINX", 4, dd4hep::Position(1.4*units::cm,-24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(701, "SMDL", "SINO", 1, dd4hep::Position(1.4*units::cm,24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(702, "SMDL", "SINO", 2, dd4hep::Position(-1.4*units::cm,24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(703, "SMDL", "SINO", 3, dd4hep::Position(-1.4*units::cm,-24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(704, "SMDL", "SINO", 4, dd4hep::Position(1.4*units::cm,-24.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(705, "SMOD", "SLAS", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.278*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(706, "SMOD", "SSEN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.065*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(707, "SMOD", "SFIR", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.343*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(708, "SMOL", "SLAS", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.298*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(709, "SMOL", "SSEN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.045*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(710, "SMOL", "SLLA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.343*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(711, "SSEN", "SSN3", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.081*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(712, "SSEN", "SSN2", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.007*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(713, "SSEN", "SSN1", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.074*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(714, "SKAP", "SSIL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0125*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(715, "SACT", "SMOD", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,4.657*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(716, "SACT", "SMOD", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,3.651*units::cm), 75, params, "ONLY");
  params.clear();
  this->add_placement(717, "SACT", "SMOD", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,2.645*units::cm), 76, params, "ONLY");
  params.clear();
  this->add_placement(718, "SACT", "SMOD", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,1.639*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(719, "SACT", "SMOD", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.633*units::cm), 75, params, "ONLY");
  params.clear();
  this->add_placement(720, "SACT", "SMOD", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,-0.373*units::cm), 76, params, "ONLY");
  params.clear();
  this->add_placement(721, "SACT", "SMOD", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,-1.379*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(722, "SACT", "SMOD", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,-2.385*units::cm), 75, params, "ONLY");
  params.clear();
  this->add_placement(723, "SACT", "SMOD", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,-3.391*units::cm), 76, params, "ONLY");
  params.clear();
  this->add_placement(724, "SACT", "SMOD", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-4.397*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(725, "SACT", "SMOD", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-5.403*units::cm), 75, params, "ONLY");
  params.clear();
  this->add_placement(726, "SSN1", "SKAP", 1, dd4hep::Position(-0.7541*units::cm,-0.15*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(727, "SSN2", "SCRM", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(728, "SSN2", "SKAP", 2, dd4hep::Position(-0.6393*units::cm,-0.4272*units::cm,0.0*units::cm), 77, params, "ONLY");
  params.clear();
  this->add_placement(729, "SSN3", "SCRM", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 77, params, "ONLY");
  params.clear();
  this->add_placement(730, "SSN1", "SKAP", 3, dd4hep::Position(-0.4272*units::cm,-0.6393*units::cm,0.0*units::cm), 78, params, "ONLY");
  params.clear();
  this->add_placement(731, "SSN2", "SCRM", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 78, params, "ONLY");
  params.clear();
  this->add_placement(732, "SSN2", "SKAP", 4, dd4hep::Position(-0.15*units::cm,-0.7541*units::cm,0.0*units::cm), 79, params, "ONLY");
  params.clear();
  this->add_placement(733, "SSN3", "SCRM", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 79, params, "ONLY");
  params.clear();
  this->add_placement(734, "SSN1", "SKAP", 5, dd4hep::Position(0.15*units::cm,-0.7541*units::cm,0.0*units::cm), 80, params, "ONLY");
  params.clear();
  this->add_placement(735, "SSN2", "SCRM", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 80, params, "ONLY");
  params.clear();
  this->add_placement(736, "SSN2", "SKAP", 6, dd4hep::Position(0.4272*units::cm,-0.6393*units::cm,0.0*units::cm), 81, params, "ONLY");
  params.clear();
  this->add_placement(737, "SSN3", "SCRM", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 81, params, "ONLY");
  params.clear();
  this->add_placement(738, "SSN1", "SKAP", 7, dd4hep::Position(0.6393*units::cm,-0.4272*units::cm,0.0*units::cm), 82, params, "ONLY");
  params.clear();
  this->add_placement(739, "SSN2", "SCRM", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 82, params, "ONLY");
  params.clear();
  this->add_placement(740, "SSN2", "SKAP", 8, dd4hep::Position(0.7541*units::cm,-0.15*units::cm,0.0*units::cm), 83, params, "ONLY");
  params.clear();
  this->add_placement(741, "SSN3", "SCRM", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 83, params, "ONLY");
  params.clear();
  this->add_placement(742, "SSN1", "SKAP", 9, dd4hep::Position(0.7541*units::cm,0.15*units::cm,0.0*units::cm), 84, params, "ONLY");
  params.clear();
  this->add_placement(743, "SSN2", "SCRM", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 84, params, "ONLY");
  params.clear();
  this->add_placement(744, "SSN2", "SKAP", 10, dd4hep::Position(0.6393*units::cm,0.4272*units::cm,0.0*units::cm), 85, params, "ONLY");
  params.clear();
  this->add_placement(745, "SSN3", "SCRM", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 85, params, "ONLY");
  params.clear();
  this->add_placement(746, "SSN1", "SKAP", 11, dd4hep::Position(0.4272*units::cm,0.6393*units::cm,0.0*units::cm), 86, params, "ONLY");
  params.clear();
  this->add_placement(747, "SSN2", "SCRM", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 86, params, "ONLY");
  params.clear();
  this->add_placement(748, "SSN2", "SKAP", 12, dd4hep::Position(0.15*units::cm,0.7541*units::cm,0.0*units::cm), 87, params, "ONLY");
  params.clear();
  this->add_placement(749, "SSN3", "SCRM", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 87, params, "ONLY");
  params.clear();
  this->add_placement(750, "SSN1", "SKAP", 13, dd4hep::Position(-0.15*units::cm,0.7541*units::cm,0.0*units::cm), 88, params, "ONLY");
  params.clear();
  this->add_placement(751, "SSN2", "SCRM", 13, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 88, params, "ONLY");
  params.clear();
  this->add_placement(752, "SSN2", "SKAP", 14, dd4hep::Position(-0.4272*units::cm,0.6393*units::cm,0.0*units::cm), 89, params, "ONLY");
  params.clear();
  this->add_placement(753, "SSN3", "SCRM", 14, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 89, params, "ONLY");
  params.clear();
  this->add_placement(754, "SSN1", "SKAP", 15, dd4hep::Position(-0.6393*units::cm,0.4272*units::cm,0.0*units::cm), 90, params, "ONLY");
  params.clear();
  this->add_placement(755, "SSN2", "SCRM", 15, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 90, params, "ONLY");
  params.clear();
  this->add_placement(756, "SSN2", "SKAP", 16, dd4hep::Position(-0.7541*units::cm,0.15*units::cm,0.0*units::cm), 91, params, "ONLY");
  params.clear();
  this->add_placement(757, "SSN3", "SCRM", 16, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 91, params, "ONLY");
  params.clear();
  this->add_placement(758, "ALEF", "COIL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(759, "COIL", "COBY", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(760, "COIL", "COEN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,340.2*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(761, "COIL", "COEN", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-340.2*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(762, "COBY", "COBI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(763, "COBY", "COBO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(764, "COBY", "COIN", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(765, "COBY", "COUT", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(766, "COUT", "COCO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(767, "COIN", "COCI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(768, "COEN", "COEO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-4.2*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(769, "COEN", "COEI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-4.2*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(770, "COIN", "COMI", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,55.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(771, "COIN", "COMI", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-55.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(772, "COUT", "COMO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,55.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(773, "COUT", "COMO", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,-55.8*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(774, "COIN", "COMI", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,102.1*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(775, "COIN", "COMI", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,-102.1*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(776, "COUT", "COMO", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,102.1*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(777, "COUT", "COMO", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,-102.1*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(778, "COIN", "COMI", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,148.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(779, "COIN", "COMI", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,-148.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(780, "COUT", "COMO", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,148.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(781, "COUT", "COMO", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,-148.4*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(782, "COIN", "COMI", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,194.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(783, "COIN", "COMI", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,-194.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(784, "COUT", "COMO", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,194.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(785, "COUT", "COMO", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,-194.7*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(786, "COIN", "COMI", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,241.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(787, "COIN", "COMI", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,-241.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(788, "COUT", "COMO", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,241.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(789, "COUT", "COMO", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,-241.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(790, "COIN", "COMI", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,287.2998*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(791, "COIN", "COMI", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,-287.2998*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(792, "COUT", "COMO", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,287.2998*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(793, "COUT", "COMO", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,-287.2998*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(794, "COMI", "COII", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(795, "COMO", "COIO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(796, "COEN", "COEP", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,5.3*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(797, "COEN", "COBE", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,-12.2*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(798, "ALEF", "QUEA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(799, "ALEF", "QUEB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(800, "QUEA", "QUAA", 1, dd4hep::Position(0.0*units::cm,1.5*units::cm,475.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 6.0, 125.0 };
  this->add_placement(801, "QUAA", "QUVI", 1, dd4hep::Position(0.0*units::cm,-1.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 28.0, 17.5 };
  this->add_placement(802, "QUEA", "QUPU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,332.5*units::cm), 0, params, "ONLY");
  params = { 0.0, 6.0, 17.5 };
  this->add_placement(803, "QUPU", "QUVA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(804, "QUEB", "QUAB", 1, dd4hep::Position(0.0*units::cm,1.5*units::cm,475.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 6.0, 125.0 };
  this->add_placement(805, "QUAB", "QUVI", 1, dd4hep::Position(0.0*units::cm,-1.5*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 28.0, 17.5 };
  this->add_placement(806, "QUEB", "QUPU", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,332.5*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(807, "ALEF", "HCBL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(808, "ALEF", "HCEA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 1, params, "ONLY");
  params.clear();
  this->add_placement(809, "ALEF", "HCEB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(810, "HCBL", "HBAL", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(811, "HBAR", "HBMO", 1, dd4hep::Position(377.8499*units::cm,-50.6223*units::cm,0.0*units::cm), 92, params, "ONLY");
  params.clear();
  this->add_placement(812, "HBAR", "HBME", 1, dd4hep::Position(463.3999*units::cm,-62.0838*units::cm,0.0*units::cm), 92, params, "ONLY");
  params.clear();
  this->add_placement(813, "HBAR", "HBMO", 2, dd4hep::Position(377.8499*units::cm,50.6223*units::cm,0.0*units::cm), 93, params, "ONLY");
  params.clear();
  this->add_placement(814, "HBAR", "HBME", 2, dd4hep::Position(463.3999*units::cm,62.0838*units::cm,0.0*units::cm), 93, params, "ONLY");
  params = { 16.5, 80.55, 6.5 };
  this->add_placement(815, "HBMO", "HBN1", 1, dd4hep::Position(-34.1223*units::cm,0.0*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 16.5, 80.55, 6.5 };
  this->add_placement(816, "HBMO", "HBN1", 2, dd4hep::Position(-34.1223*units::cm,0.0*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 16.5, 5.0, 6.5 };
  this->add_placement(817, "HBME", "HBN1", 1, dd4hep::Position(-45.5838*units::cm,0.0*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 16.5, 5.0, 6.5 };
  this->add_placement(818, "HBME", "HBN1", 2, dd4hep::Position(-45.5838*units::cm,0.0*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 3.0, 80.55, 6.5, 0.2679, 0.0, 0.0 };
  this->add_placement(819, "HBMO", "HBN2", 1, dd4hep::Position(47.6223*units::cm,0.0*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 3.0, 80.55, 6.5, 0.2679, 0.0, 0.0 };
  this->add_placement(820, "HBMO", "HBN2", 2, dd4hep::Position(47.6223*units::cm,0.0*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 3.0, 5.0, 6.5, 0.2679, 0.0, 0.0 };
  this->add_placement(821, "HBME", "HBN2", 1, dd4hep::Position(59.0838*units::cm,0.0*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 3.0, 5.0, 6.5, 0.2679, 0.0, 0.0 };
  this->add_placement(822, "HBME", "HBN2", 2, dd4hep::Position(59.0838*units::cm,0.0*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 39.9512, 0.7, 352.3999 };
  this->add_placement(823, "HBMO", "HBLA", 1, dd4hep::Position(-10.6711*units::cm,-78.95*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 20.4512, 0.7, 6.5 };
  this->add_placement(824, "HBMO", "HBL1", 1, dd4hep::Position(2.8289*units::cm,-78.95*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 20.4512, 0.7, 6.5 };
  this->add_placement(825, "HBMO", "HBL2", 1, dd4hep::Position(2.8289*units::cm,-78.95*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 40.9158, 0.7, 352.3999 };
  this->add_placement(826, "HBMO", "HBLA", 2, dd4hep::Position(-9.7064*units::cm,-71.7499*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 21.4158, 0.7, 6.5 };
  this->add_placement(827, "HBMO", "HBL1", 2, dd4hep::Position(3.7936*units::cm,-71.7499*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 21.4158, 0.7, 6.5 };
  this->add_placement(828, "HBMO", "HBL2", 2, dd4hep::Position(3.7936*units::cm,-71.7499*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 41.8804, 0.7, 352.3999 };
  this->add_placement(829, "HBMO", "HBLA", 3, dd4hep::Position(-8.7418*units::cm,-64.55*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 22.3804, 0.7, 6.5 };
  this->add_placement(830, "HBMO", "HBL1", 3, dd4hep::Position(4.7582*units::cm,-64.55*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 22.3804, 0.7, 6.5 };
  this->add_placement(831, "HBMO", "HBL2", 3, dd4hep::Position(4.7582*units::cm,-64.55*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 42.8451, 0.7, 352.3999 };
  this->add_placement(832, "HBMO", "HBLA", 4, dd4hep::Position(-7.7772*units::cm,-57.3499*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 23.3451, 0.7, 6.5 };
  this->add_placement(833, "HBMO", "HBL1", 4, dd4hep::Position(5.7228*units::cm,-57.3499*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 23.3451, 0.7, 6.5 };
  this->add_placement(834, "HBMO", "HBL2", 4, dd4hep::Position(5.7228*units::cm,-57.3499*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 43.8097, 0.7, 352.3999 };
  this->add_placement(835, "HBMO", "HBLA", 5, dd4hep::Position(-6.8126*units::cm,-50.15*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 24.3097, 0.7, 6.5 };
  this->add_placement(836, "HBMO", "HBL1", 5, dd4hep::Position(6.6874*units::cm,-50.15*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 24.3097, 0.7, 6.5 };
  this->add_placement(837, "HBMO", "HBL2", 5, dd4hep::Position(6.6874*units::cm,-50.15*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 44.7743, 0.7, 352.3999 };
  this->add_placement(838, "HBMO", "HBLA", 6, dd4hep::Position(-5.848*units::cm,-42.95*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 25.2743, 0.7, 6.5 };
  this->add_placement(839, "HBMO", "HBL1", 6, dd4hep::Position(7.652*units::cm,-42.95*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 25.2743, 0.7, 6.5 };
  this->add_placement(840, "HBMO", "HBL2", 6, dd4hep::Position(7.652*units::cm,-42.95*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 45.7389, 0.7, 352.3999 };
  this->add_placement(841, "HBMO", "HBLA", 7, dd4hep::Position(-4.8834*units::cm,-35.7499*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 26.2389, 0.7, 6.5 };
  this->add_placement(842, "HBMO", "HBL1", 7, dd4hep::Position(8.6166*units::cm,-35.7499*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 26.2389, 0.7, 6.5 };
  this->add_placement(843, "HBMO", "HBL2", 7, dd4hep::Position(8.6166*units::cm,-35.7499*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 46.7035, 0.7, 352.3999 };
  this->add_placement(844, "HBMO", "HBLA", 8, dd4hep::Position(-3.9187*units::cm,-28.55*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 27.2035, 0.7, 6.5 };
  this->add_placement(845, "HBMO", "HBL1", 8, dd4hep::Position(9.5813*units::cm,-28.55*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 27.2035, 0.7, 6.5 };
  this->add_placement(846, "HBMO", "HBL2", 8, dd4hep::Position(9.5813*units::cm,-28.55*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 47.6682, 0.7, 352.3999 };
  this->add_placement(847, "HBMO", "HBLA", 9, dd4hep::Position(-2.9541*units::cm,-21.3499*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 28.1681, 0.7, 6.5 };
  this->add_placement(848, "HBMO", "HBL1", 9, dd4hep::Position(10.5459*units::cm,-21.3499*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 28.1681, 0.7, 6.5 };
  this->add_placement(849, "HBMO", "HBL2", 9, dd4hep::Position(10.5459*units::cm,-21.3499*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 48.6328, 0.7, 352.3999 };
  this->add_placement(850, "HBMO", "HBLA", 10, dd4hep::Position(-1.9895*units::cm,-14.15*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 29.1328, 0.7, 6.5 };
  this->add_placement(851, "HBMO", "HBL1", 10, dd4hep::Position(11.5105*units::cm,-14.15*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 29.1328, 0.7, 6.5 };
  this->add_placement(852, "HBMO", "HBL2", 10, dd4hep::Position(11.5105*units::cm,-14.15*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 49.5974, 0.7, 352.3999 };
  this->add_placement(853, "HBMO", "HBLA", 11, dd4hep::Position(-1.0249*units::cm,-6.95*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 30.0974, 0.7, 6.5 };
  this->add_placement(854, "HBMO", "HBL1", 11, dd4hep::Position(12.4751*units::cm,-6.95*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 30.0974, 0.7, 6.5 };
  this->add_placement(855, "HBMO", "HBL2", 11, dd4hep::Position(12.4751*units::cm,-6.95*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 50.562, 0.7, 352.3999 };
  this->add_placement(856, "HBMO", "HBLA", 12, dd4hep::Position(-0.0603*units::cm,0.2501*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 31.062, 0.7, 6.5 };
  this->add_placement(857, "HBMO", "HBL1", 12, dd4hep::Position(13.4397*units::cm,0.2501*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 31.062, 0.7, 6.5 };
  this->add_placement(858, "HBMO", "HBL2", 12, dd4hep::Position(13.4397*units::cm,0.2501*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 51.5266, 0.7, 352.3999 };
  this->add_placement(859, "HBMO", "HBLA", 13, dd4hep::Position(0.9043*units::cm,7.45*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 32.0266, 0.7, 6.5 };
  this->add_placement(860, "HBMO", "HBL1", 13, dd4hep::Position(14.4043*units::cm,7.45*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 32.0266, 0.7, 6.5 };
  this->add_placement(861, "HBMO", "HBL2", 13, dd4hep::Position(14.4043*units::cm,7.45*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 52.4912, 0.7, 352.3999 };
  this->add_placement(862, "HBMO", "HBLA", 14, dd4hep::Position(1.869*units::cm,14.6501*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 32.9912, 0.7, 6.5 };
  this->add_placement(863, "HBMO", "HBL1", 14, dd4hep::Position(15.369*units::cm,14.6501*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 32.9912, 0.7, 6.5 };
  this->add_placement(864, "HBMO", "HBL2", 14, dd4hep::Position(15.369*units::cm,14.6501*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 53.4559, 0.7, 352.3999 };
  this->add_placement(865, "HBMO", "HBLA", 15, dd4hep::Position(2.8336*units::cm,21.85*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 33.9559, 0.7, 6.5 };
  this->add_placement(866, "HBMO", "HBL1", 15, dd4hep::Position(16.3336*units::cm,21.85*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 33.9559, 0.7, 6.5 };
  this->add_placement(867, "HBMO", "HBL2", 15, dd4hep::Position(16.3336*units::cm,21.85*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 54.4205, 0.7, 352.3999 };
  this->add_placement(868, "HBMO", "HBLA", 16, dd4hep::Position(3.7982*units::cm,29.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 34.9205, 0.7, 6.5 };
  this->add_placement(869, "HBMO", "HBL1", 16, dd4hep::Position(17.2982*units::cm,29.05*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 34.9205, 0.7, 6.5 };
  this->add_placement(870, "HBMO", "HBL2", 16, dd4hep::Position(17.2982*units::cm,29.05*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 55.3851, 0.7, 352.3999 };
  this->add_placement(871, "HBMO", "HBLA", 17, dd4hep::Position(4.7628*units::cm,36.2501*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 35.8851, 0.7, 6.5 };
  this->add_placement(872, "HBMO", "HBL1", 17, dd4hep::Position(18.2628*units::cm,36.2501*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 35.8851, 0.7, 6.5 };
  this->add_placement(873, "HBMO", "HBL2", 17, dd4hep::Position(18.2628*units::cm,36.2501*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 56.3497, 0.7, 352.3999 };
  this->add_placement(874, "HBMO", "HBLA", 18, dd4hep::Position(5.7274*units::cm,43.45*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 36.8497, 0.7, 6.5 };
  this->add_placement(875, "HBMO", "HBL1", 18, dd4hep::Position(19.2274*units::cm,43.45*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 36.8497, 0.7, 6.5 };
  this->add_placement(876, "HBMO", "HBL2", 18, dd4hep::Position(19.2274*units::cm,43.45*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 57.3143, 0.7, 352.3999 };
  this->add_placement(877, "HBMO", "HBLA", 19, dd4hep::Position(6.692*units::cm,50.6501*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 37.8143, 0.7, 6.5 };
  this->add_placement(878, "HBMO", "HBL1", 19, dd4hep::Position(20.192*units::cm,50.6501*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 37.8143, 0.7, 6.5 };
  this->add_placement(879, "HBMO", "HBL2", 19, dd4hep::Position(20.192*units::cm,50.6501*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 58.2789, 0.7, 352.3999 };
  this->add_placement(880, "HBMO", "HBLA", 20, dd4hep::Position(7.6567*units::cm,57.85*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 38.7789, 0.7, 6.5 };
  this->add_placement(881, "HBMO", "HBL1", 20, dd4hep::Position(21.1567*units::cm,57.85*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 38.7789, 0.7, 6.5 };
  this->add_placement(882, "HBMO", "HBL2", 20, dd4hep::Position(21.1567*units::cm,57.85*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 59.2436, 0.7, 352.3999 };
  this->add_placement(883, "HBMO", "HBLA", 21, dd4hep::Position(8.6213*units::cm,65.05*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 39.7436, 0.7, 6.5 };
  this->add_placement(884, "HBMO", "HBL1", 21, dd4hep::Position(22.1213*units::cm,65.05*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 39.7436, 0.7, 6.5 };
  this->add_placement(885, "HBMO", "HBL2", 21, dd4hep::Position(22.1213*units::cm,65.05*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 60.2082, 0.7, 352.3999 };
  this->add_placement(886, "HBMO", "HBLA", 22, dd4hep::Position(9.5859*units::cm,72.25*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 40.7082, 0.7, 6.5 };
  this->add_placement(887, "HBMO", "HBL1", 22, dd4hep::Position(23.0859*units::cm,72.25*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 40.7082, 0.7, 6.5 };
  this->add_placement(888, "HBMO", "HBL2", 22, dd4hep::Position(23.0859*units::cm,72.25*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params = { 61.1728, 0.7, 352.3999 };
  this->add_placement(889, "HBMO", "HBLA", 23, dd4hep::Position(10.5505*units::cm,79.45*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 41.6728, 0.7, 6.5 };
  this->add_placement(890, "HBMO", "HBL1", 23, dd4hep::Position(24.0505*units::cm,79.45*units::cm,358.8999*units::cm), 0, params, "ONLY");
  params = { 41.6728, 0.7, 6.5 };
  this->add_placement(891, "HBMO", "HBL2", 23, dd4hep::Position(24.0505*units::cm,79.45*units::cm,-358.8999*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(892, "HCEA", "HCPA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(893, "HCEB", "HCPB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(894, "HCMA", "HCMO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 94, params, "ONLY");
  params.clear();
  this->add_placement(895, "HCMB", "HCMO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 94, params, "ONLY");
  params.clear();
  this->add_placement(896, "HCMO", "HCME", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,478.3999*units::cm), 0, params, "ONLY");
  params = { 210.0, 248.0, 25.2 };
  this->add_placement(897, "HCEA", "EPBX", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,340.2*units::cm), 0, params, "ONLY");
  params = { 210.0, 248.0, 25.2 };
  this->add_placement(898, "HCEB", "EPBX", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,340.2*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 24.7478, 35.2518 };
  this->add_placement(899, "EPBX", "ETCB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 54.7478, 65.2518 };
  this->add_placement(900, "EPBX", "ETCB", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 114.7478, 125.2518 };
  this->add_placement(901, "EPBX", "ETCB", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 204.7478, 215.2518 };
  this->add_placement(902, "EPBX", "ETCB", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 234.7478, 245.2518 };
  this->add_placement(903, "EPBX", "ETCB", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 324.7478, 335.2518 };
  this->add_placement(904, "EPBX", "ETCB", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 144.748, 155.252 };
  this->add_placement(905, "EPBX", "EICB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 294.748, 305.252 };
  this->add_placement(906, "EPBX", "EICB", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 12.6899, 17.3102 };
  this->add_placement(907, "EPBX", "EPCB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 42.6899, 47.3102 };
  this->add_placement(908, "EPBX", "EPCB", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 72.6899, 77.3102 };
  this->add_placement(909, "EPBX", "EPCB", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 102.6899, 107.3102 };
  this->add_placement(910, "EPBX", "EPCB", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 132.6899, 137.3102 };
  this->add_placement(911, "EPBX", "EPCB", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 162.6899, 167.3102 };
  this->add_placement(912, "EPBX", "EPCB", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 192.6899, 197.3102 };
  this->add_placement(913, "EPBX", "EPCB", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 222.6899, 227.3102 };
  this->add_placement(914, "EPBX", "EPCB", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 252.6899, 257.3102 };
  this->add_placement(915, "EPBX", "EPCB", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 282.6899, 287.3102 };
  this->add_placement(916, "EPBX", "EPCB", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 312.6899, 317.3102 };
  this->add_placement(917, "EPBX", "EPCB", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 233.0, 248.0, 25.2, 342.6899, 347.3102 };
  this->add_placement(918, "EPBX", "EPCB", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(919, "HCMO", "HCLA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,321.1*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(920, "HCMO", "HCLA", 2, dd4hep::Position(0.0*units::cm,0.0*units::cm,328.3*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(921, "HCMO", "HCLA", 3, dd4hep::Position(0.0*units::cm,0.0*units::cm,335.5*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(922, "HCMO", "HCLA", 4, dd4hep::Position(0.0*units::cm,0.0*units::cm,342.7*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(923, "HCMO", "HCLA", 5, dd4hep::Position(0.0*units::cm,0.0*units::cm,349.9001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(924, "HCMO", "HCLA", 6, dd4hep::Position(0.0*units::cm,0.0*units::cm,357.1001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 210.0, 0.7, 45.0, 210.0 };
  this->add_placement(925, "HCMO", "HCLA", 7, dd4hep::Position(0.0*units::cm,0.0*units::cm,364.3001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(926, "HCMO", "HCLA", 8, dd4hep::Position(0.0*units::cm,0.0*units::cm,371.5001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(927, "HCMO", "HCLA", 9, dd4hep::Position(0.0*units::cm,0.0*units::cm,378.7001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(928, "HCMO", "HCLA", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,385.9001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(929, "HCMO", "HCLA", 11, dd4hep::Position(0.0*units::cm,0.0*units::cm,393.1001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(930, "HCMO", "HCLA", 12, dd4hep::Position(0.0*units::cm,0.0*units::cm,400.3001*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(931, "HCMO", "HCLA", 13, dd4hep::Position(0.0*units::cm,0.0*units::cm,407.5002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(932, "HCMO", "HCLA", 14, dd4hep::Position(0.0*units::cm,0.0*units::cm,414.7002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(933, "HCMO", "HCLA", 15, dd4hep::Position(0.0*units::cm,0.0*units::cm,421.9002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(934, "HCMO", "HCLA", 16, dd4hep::Position(0.0*units::cm,0.0*units::cm,429.1002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(935, "HCMO", "HCLA", 17, dd4hep::Position(0.0*units::cm,0.0*units::cm,436.3002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(936, "HCMO", "HCLA", 18, dd4hep::Position(0.0*units::cm,0.0*units::cm,443.5002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(937, "HCMO", "HCLA", 19, dd4hep::Position(0.0*units::cm,0.0*units::cm,450.7002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(938, "HCMO", "HCLA", 20, dd4hep::Position(0.0*units::cm,0.0*units::cm,457.9002*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(939, "HCMO", "HCLA", 21, dd4hep::Position(0.0*units::cm,0.0*units::cm,465.1003*units::cm), 0, params, "ONLY");
  params = { 0.0, 60.0, 2.0, -0.7, 45.0, 450.3452, 0.7, 45.0, 450.3452 };
  this->add_placement(940, "HCMO", "HCLA", 22, dd4hep::Position(0.0*units::cm,0.0*units::cm,472.3003*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(941, "ALEF", "MUON", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(942, "HBAR", "MUB1", 1, dd4hep::Position(474.5999*units::cm,0.0*units::cm,0.0*units::cm), 95, params, "ONLY");
  params.clear();
  this->add_placement(943, "MUON", "MUBO", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 127.0, 348.2999, 4.65 };
  this->add_placement(944, "MUBO", "MUB2", 1, dd4hep::Position(526.5999*units::cm,-10.8*units::cm,0.0*units::cm), 96, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(945, "MUBO", "MUB2", 2, dd4hep::Position(456.0488*units::cm,263.2999*units::cm,0.0*units::cm), 97, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(946, "MUBO", "MUB2", 3, dd4hep::Position(263.2999*units::cm,456.0489*units::cm,0.0*units::cm), 98, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(947, "MUBO", "MUB2", 4, dd4hep::Position(-0.0*units::cm,526.5999*units::cm,0.0*units::cm), 99, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(948, "MUBO", "MUB2", 5, dd4hep::Position(-263.3*units::cm,456.0488*units::cm,0.0*units::cm), 100, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(949, "MUBO", "MUB2", 6, dd4hep::Position(-456.0488*units::cm,263.3*units::cm,0.0*units::cm), 101, params, "ONLY");
  params = { 127.0, 348.2999, 4.65 };
  this->add_placement(950, "MUBO", "MUB2", 7, dd4hep::Position(-526.5999*units::cm,-10.8*units::cm,0.0*units::cm), 102, params, "ONLY");
  params = { 127.0, 348.2999, 4.65 };
  this->add_placement(951, "MUBO", "MUB2", 8, dd4hep::Position(-450.6488*units::cm,-272.6531*units::cm,0.0*units::cm), 103, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(952, "MUBO", "MUB2", 9, dd4hep::Position(-263.2999*units::cm,-456.0489*units::cm,0.0*units::cm), 104, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(953, "MUBO", "MUB2", 10, dd4hep::Position(0.0*units::cm,-526.5999*units::cm,0.0*units::cm), 105, params, "ONLY");
  params = { 137.75, 348.2999, 4.65 };
  this->add_placement(954, "MUBO", "MUB2", 11, dd4hep::Position(263.2999*units::cm,-456.0489*units::cm,0.0*units::cm), 106, params, "ONLY");
  params = { 127.0, 348.2999, 4.65 };
  this->add_placement(955, "MUBO", "MUB2", 12, dd4hep::Position(450.6489*units::cm,-272.6529*units::cm,0.0*units::cm), 107, params, "ONLY");
  params.clear();
  this->add_placement(956, "MUON", "MUEA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(957, "MUON", "MUEB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 2, params, "ONLY");
  params.clear();
  this->add_placement(958, "MUEA", "MMIA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,422.0999*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(959, "MUEA", "MMOA", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,439.3499*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(960, "MUEB", "MMIB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,422.0999*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(961, "MUEB", "MMOB", 1, dd4hep::Position(0.0*units::cm,0.0*units::cm,439.3499*units::cm), 0, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(962, "MMIA", "MUM1", 1, dd4hep::Position(498.6999*units::cm,0.0*units::cm,-1.7*units::cm), 108, params, "ONLY");
  params = { 69.5, 164.75, 4.7 };
  this->add_placement(963, "MMIA", "MUM1", 2, dd4hep::Position(442.279*units::cm,255.3499*units::cm,2.6*units::cm), 109, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(964, "MMIA", "MUM1", 3, dd4hep::Position(249.8499*units::cm,432.7528*units::cm,-1.7*units::cm), 110, params, "ONLY");
  params = { 69.5, 164.75, 4.7 };
  this->add_placement(965, "MMIA", "MUM1", 4, dd4hep::Position(-0.0*units::cm,510.6998*units::cm,2.6*units::cm), 111, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(966, "MMIA", "MUM1", 5, dd4hep::Position(-249.8499*units::cm,432.7527*units::cm,-1.7*units::cm), 112, params, "ONLY");
  params = { 69.5, 164.75, 4.7 };
  this->add_placement(967, "MMIA", "MUM1", 6, dd4hep::Position(-442.279*units::cm,255.3499*units::cm,2.6*units::cm), 113, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(968, "MMIA", "MUM1", 7, dd4hep::Position(-498.6999*units::cm,-0.0*units::cm,-1.7*units::cm), 114, params, "ONLY");
  params = { 69.45, 181.35, 4.7 };
  this->add_placement(969, "MMIA", "MUM1", 8, dd4hep::Position(-433.063*units::cm,-269.3127*units::cm,2.6*units::cm), 115, params, "ONLY");
  params = { 69.45, 181.35, 4.7 };
  this->add_placement(970, "MMIA", "MUM1", 12, dd4hep::Position(433.0631*units::cm,-269.3124*units::cm,2.6*units::cm), 119, params, "ONLY");
  params.clear();
  this->add_placement(971, "MUEA", "MMBA", 1, dd4hep::Position(0.0*units::cm,-498.3998*units::cm,431.2998*units::cm), 117, params, "ONLY");
  params = { 83.0, 204.0, 6.0 };
  this->add_placement(972, "MMBA", "MUM1", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(973, "MMOA", "MUM2", 1, dd4hep::Position(548.6999*units::cm,0.0*units::cm,-0.6499*units::cm), 108, params, "ONLY");
  params = { 90.65, 194.5, 4.7 };
  this->add_placement(974, "MMOA", "MUM2", 2, dd4hep::Position(485.5803*units::cm,280.3499*units::cm,-0.6499*units::cm), 109, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(975, "MMOA", "MUM2", 3, dd4hep::Position(274.8499*units::cm,476.054*units::cm,-0.6499*units::cm), 110, params, "ONLY");
  params = { 82.25, 194.5, 4.7 };
  this->add_placement(976, "MMOA", "MUM2", 4, dd4hep::Position(-0.0*units::cm,560.6998*units::cm,7.75*units::cm), 111, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(977, "MMOA", "MUM2", 5, dd4hep::Position(-274.8499*units::cm,476.054*units::cm,-0.6499*units::cm), 112, params, "ONLY");
  params = { 90.65, 194.5, 4.7 };
  this->add_placement(978, "MMOA", "MUM2", 6, dd4hep::Position(-485.5803*units::cm,280.3499*units::cm,-0.6499*units::cm), 113, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(979, "MMOA", "MUM2", 7, dd4hep::Position(-548.6999*units::cm,-0.0*units::cm,-0.6499*units::cm), 114, params, "ONLY");
  params = { 90.65, 202.2999, 4.7 };
  this->add_placement(980, "MMOA", "MUM2", 8, dd4hep::Position(-480.8142*units::cm,-286.605*units::cm,-0.6499*units::cm), 115, params, "ONLY");
  params = { 90.65, 202.2999, 4.7 };
  this->add_placement(981, "MMOA", "MUM2", 12, dd4hep::Position(480.8144*units::cm,-286.6048*units::cm,-0.6499*units::cm), 119, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(982, "MMIB", "MUM1", 1, dd4hep::Position(-498.6999*units::cm,-0.0*units::cm,-1.7*units::cm), 114, params, "ONLY");
  params = { 69.5, 164.75, 4.7 };
  this->add_placement(983, "MMIB", "MUM1", 2, dd4hep::Position(-442.279*units::cm,255.3499*units::cm,2.6*units::cm), 113, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(984, "MMIB", "MUM1", 3, dd4hep::Position(-249.8499*units::cm,432.7527*units::cm,-1.7*units::cm), 112, params, "ONLY");
  params = { 69.5, 164.75, 4.7 };
  this->add_placement(985, "MMIB", "MUM1", 4, dd4hep::Position(-0.0*units::cm,510.6998*units::cm,2.6*units::cm), 111, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(986, "MMIB", "MUM1", 5, dd4hep::Position(249.8499*units::cm,432.7528*units::cm,-1.7*units::cm), 110, params, "ONLY");
  params = { 69.5, 164.75, 4.7 };
  this->add_placement(987, "MMIB", "MUM1", 6, dd4hep::Position(442.279*units::cm,255.3499*units::cm,2.6*units::cm), 109, params, "ONLY");
  params = { 73.8, 122.9, 4.7 };
  this->add_placement(988, "MMIB", "MUM1", 7, dd4hep::Position(498.6999*units::cm,0.0*units::cm,-1.7*units::cm), 108, params, "ONLY");
  params = { 69.45, 181.35, 4.7 };
  this->add_placement(989, "MMIB", "MUM1", 8, dd4hep::Position(433.0631*units::cm,-269.3124*units::cm,2.6*units::cm), 119, params, "ONLY");
  params = { 69.45, 181.35, 4.7 };
  this->add_placement(990, "MMIB", "MUM1", 12, dd4hep::Position(-433.063*units::cm,-269.3127*units::cm,2.6*units::cm), 115, params, "ONLY");
  params.clear();
  this->add_placement(991, "MUEB", "MMBB", 1, dd4hep::Position(0.0*units::cm,-498.3998*units::cm,431.2998*units::cm), 117, params, "ONLY");
  params = { 83.0, 204.0, 6.0 };
  this->add_placement(992, "MMBB", "MUM1", 10, dd4hep::Position(0.0*units::cm,0.0*units::cm,0.0*units::cm), 0, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(993, "MMOB", "MUM2", 1, dd4hep::Position(-548.6999*units::cm,-0.0*units::cm,-0.6499*units::cm), 114, params, "ONLY");
  params = { 90.65, 194.5, 4.7 };
  this->add_placement(994, "MMOB", "MUM2", 2, dd4hep::Position(-485.5803*units::cm,280.3499*units::cm,-0.6499*units::cm), 113, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(995, "MMOB", "MUM2", 3, dd4hep::Position(-274.8499*units::cm,476.054*units::cm,-0.6499*units::cm), 112, params, "ONLY");
  params = { 82.25, 194.5, 4.7 };
  this->add_placement(996, "MMOB", "MUM2", 4, dd4hep::Position(-0.0*units::cm,560.6998*units::cm,7.75*units::cm), 111, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(997, "MMOB", "MUM2", 5, dd4hep::Position(274.8499*units::cm,476.054*units::cm,-0.6499*units::cm), 110, params, "ONLY");
  params = { 90.65, 194.5, 4.7 };
  this->add_placement(998, "MMOB", "MUM2", 6, dd4hep::Position(485.5803*units::cm,280.3499*units::cm,-0.6499*units::cm), 109, params, "ONLY");
  params = { 90.65, 131.25, 4.7 };
  this->add_placement(999, "MMOB", "MUM2", 7, dd4hep::Position(548.6999*units::cm,0.0*units::cm,-0.6499*units::cm), 108, params, "ONLY");
  params = { 90.65, 202.2999, 4.7 };
  this->add_placement(1000, "MMOB", "MUM2", 8, dd4hep::Position(480.8144*units::cm,-286.6048*units::cm,-0.6499*units::cm), 119, params, "ONLY");
  params = { 90.65, 202.2999, 4.7 };
  this->add_placement(1001, "MMOB", "MUM2", 12, dd4hep::Position(-480.8142*units::cm,-286.605*units::cm,-0.6499*units::cm), 115, params, "ONLY");
  params.clear();
  this->add_placement(1002, "MUEA", "MCIA", 1, dd4hep::Position(0.0*units::cm,25.8*units::cm,513.5998*units::cm), 0, params, "MANY");
  params.clear();
  this->add_placement(1003, "MUEA", "MCOA", 1, dd4hep::Position(0.0*units::cm,78.8001*units::cm,565.1999*units::cm), 0, params, "ONLY");
  params.clear();
  this->add_placement(1004, "MUEB", "MCIB", 1, dd4hep::Position(0.0*units::cm,25.8*units::cm,513.5998*units::cm), 0, params, "MANY");
  params.clear();
  this->add_placement(1005, "MUEB", "MCOB", 1, dd4hep::Position(0.0*units::cm,78.8001*units::cm,565.1999*units::cm), 0, params, "ONLY");
  params = { 300.6, 237.1, 4.7 };
  this->add_placement(1006, "MCIA", "MUC1", 1, dd4hep::Position(303.8999*units::cm,208.0*units::cm,-12.1*units::cm), 121, params, "ONLY");
  params = { 233.7999, 301.45, 4.7 };
  this->add_placement(1007, "MCIA", "MUC1", 2, dd4hep::Position(-234.7*units::cm,274.7998*units::cm,-12.1*units::cm), 120, params, "ONLY");
  params = { 239.6, 233.7, 4.7 };
  this->add_placement(1008, "MCIA", "MUC1", 3, dd4hep::Position(-300.5*units::cm,-198.6*units::cm,-12.1*units::cm), 120, params, "ONLY");
  params = { 172.7999, 303.8999, 4.7 };
  this->add_placement(1009, "MCIA", "MUC1", 4, dd4hep::Position(237.1*units::cm,-265.4*units::cm,-12.1*units::cm), 121, params, "ONLY");
  params = { 323.2999, 259.35, 4.7 };
  this->add_placement(1010, "MCOA", "MUC2", 1, dd4hep::Position(326.2*units::cm,177.6999*units::cm,-30.1001*units::cm), 121, params, "ONLY");
  params = { 256.8999, 326.1499, 4.7 };
  this->add_placement(1011, "MCOA", "MUC2", 2, dd4hep::Position(-259.3999*units::cm,244.8999*units::cm,-30.1001*units::cm), 120, params, "ONLY");
  params = { 265.0499, 260.0499, 4.7 };
  this->add_placement(1012, "MCOA", "MUC2", 3, dd4hep::Position(-326.8999*units::cm,-277.1001*units::cm,-30.1001*units::cm), 120, params, "ONLY");
  params = { 198.25, 326.85, 4.7 };
  this->add_placement(1013, "MCOA", "MUC2", 4, dd4hep::Position(260.0999*units::cm,-343.8999*units::cm,-30.1001*units::cm), 121, params, "ONLY");
  params = { 300.6, 237.1, 4.7 };
  this->add_placement(1014, "MCIB", "MUC1", 1, dd4hep::Position(-303.8999*units::cm,208.0*units::cm,-12.1*units::cm), 120, params, "ONLY");
  params = { 233.7999, 301.45, 4.7 };
  this->add_placement(1015, "MCIB", "MUC1", 2, dd4hep::Position(234.7*units::cm,274.7998*units::cm,-12.1*units::cm), 121, params, "ONLY");
  params = { 239.6, 233.7, 4.7 };
  this->add_placement(1016, "MCIB", "MUC1", 3, dd4hep::Position(300.5*units::cm,-198.6*units::cm,-12.1*units::cm), 121, params, "ONLY");
  params = { 172.7999, 303.8999, 4.7 };
  this->add_placement(1017, "MCIB", "MUC1", 4, dd4hep::Position(-237.1*units::cm,-265.4*units::cm,-12.1*units::cm), 120, params, "ONLY");
  params = { 323.2999, 259.35, 4.7 };
  this->add_placement(1018, "MCOB", "MUC2", 1, dd4hep::Position(-326.2*units::cm,177.6999*units::cm,-30.1001*units::cm), 120, params, "ONLY");
  params = { 256.8999, 326.1499, 4.7 };
  this->add_placement(1019, "MCOB", "MUC2", 2, dd4hep::Position(259.3999*units::cm,244.8999*units::cm,-30.1001*units::cm), 121, params, "ONLY");
  params = { 265.0499, 260.0499, 4.7 };
  this->add_placement(1020, "MCOB", "MUC2", 3, dd4hep::Position(326.8999*units::cm,-277.1001*units::cm,-30.1001*units::cm), 121, params, "ONLY");
  params = { 198.25, 326.85, 4.7 };
  this->add_placement(1021, "MCOB", "MUC2", 4, dd4hep::Position(-260.0999*units::cm,-343.8999*units::cm,-30.1001*units::cm), 120, params, "ONLY");
} /// End geant3_geometry_imp::handle_placements  


