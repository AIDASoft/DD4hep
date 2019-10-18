#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================

set(AlignDet_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/AlignDet)
dd4hep_add_plugin(AlignDetExample SOURCES src/*.cpp
  USES DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign
       ROOT::Core ROOT::Geom ROOT::GenVector ROOT::MathCore
  )
dd4hep_install_dir(compact DESTINATION ${AlignDet_INSTALL})
dd4hep_configure_scripts(AlignDet DEFAULT_SETUP WITH_TESTS)
install(TARGETS AlignDetExample LIBRARY DESTINATION lib)

#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( AlignDet_Telescope_dump_geometry
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${AlignDet_INSTALL}/compact/Telescope.xml 
  -plugin DD4hep_DetectorDump
  REGEX_PASS "/world/Telescope/module_9 NumDau\\:1 VolID\\:00000903 Place"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( AlignDet_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_populate 
     -input file:${AlignDet_INSTALL}/compact/Telescope.xml -iovs 10
  REGEX_PASS "Summary          INFO  Processed a total 190 conditions \\(S:190,L:0,C:0,M:0\\) and \\(C:190,M:0\\) alignments. Created:200"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( AlignDet_Telescope_read_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml 
     -input  file:${AlignDet_INSTALL}/compact/Telescope.xml 
     -deltas file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/repository.xml 
  REGEX_PASS "20 conditions in slice. \\(T:20,S:20,L:0,C:0,M:0\\) Alignments accessed: 20 \\(A:19,M:0\\) for IOV:run\\(1\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Write out alignment file from the alignment data using a detelement scan
dd4hep_add_test_reg( AlignDet_Telescope_dump_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml 
  -input file:${AlignDet_INSTALL}/compact/Telescope.xml 
  -delta file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_ConditionsXMLRepositoryWriter -iov_type run -iov_value 1500
  REGEX_PASS "Summary: Converted 33 conditions. 0 conditions without recipe"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Write out alignment file from the alignment data using a detelement scan
dd4hep_add_test_reg( AlignDet_Telescope_write_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml 
  -input file:${AlignDet_INSTALL}/compact/Telescope.xml 
  -delta file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_ConditionsXMLRepositoryWriter -iov_type run -iov_value 1500 -manager -output new_cond.xml
  REGEX_PASS "Successfully wrote 33 conditions \\(0 unconverted\\) to file: new_cond.xml"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( AlignDet_Telescope_stress
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_AlignmentExample_stress 
      -input file:${AlignDet_INSTALL}/compact/Telescope.xml -iovs 20 -runs 111
  REGEX_PASS "Summary: Total 4598 conditions used \\(S:4598,L:0,C:0,M:0\\) \\(A:380,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( AlignDet_Telescope_align_new
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_align_telescope 
     -input  file:${AlignDet_INSTALL}/compact/Telescope.xml 
     -setup  file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/manager.xml 
  REGEX_PASS "World transformation of: /world/Telescope/module_3/sensor  Tr:\\( 3.9e\\+02,       0,     4.9 \\[cm\\]\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( AlignDet_Telescope_align_nominal
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_nominal
     -input  file:${AlignDet_INSTALL}/compact/Telescope.xml 
  REGEX_PASS "Printed 20, scanned 20 and computed a total of 20 alignments \\(C:20,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( AlignDet_Telescope_readback_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml
  -input  file:${AlignDet_INSTALL}/compact/Telescope.xml
  -deltas file:./new_cond.xml
  REGEX_PASS "33 conditions in slice. \\(T:33,S:33,L:0,C:0,M:0\\) Alignments accessed: 20 \\(A:19,M:0\\) for IOV:run\\(1\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Extended stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( AlignDet_CLICSiD_stress_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_AlignmentExample_stress 
      -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 10 -runs 100
  REGEX_PASS "Summary: Total 7372260 conditions used \\(S:7372260,L:0,C:0,M:0\\) \\(A:351060,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( AlignDet_CLICSiD_align_nominal_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_nominal
     -input  file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
  REGEX_PASS "Printed 35107, scanned 35107 and computed a total of 35107 alignments \\(C:35107,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load ALEPH TPC geometry --------------------------------------
dd4hep_add_test_reg( AlignDet_AlephTPC_load
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${AlignDet_INSTALL}/compact/AlephTPC.xml
             -destroy -no-interpreter -plugin DD4hep_GlobalAlignmentInstall
  REGEX_PASS "189 nodes/ 24 volume UID's in Detector Geometry"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load and misalign ALEPH TPC geometry -------------------------
dd4hep_add_test_reg( AlignDet_AlephTPC_global_align
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${AlignDet_INSTALL}/compact/AlephTPC.xml
             -destroy -no-interpreter
             -plugin DD4hep_GlobalAlignmentInstall
             -plugin DD4hep_XMLLoader file:${AlignDet_INSTALL}/compact/AlephTPC_alignment.xml BUILD_DEFAULT
  REGEX_PASS "Successfully parsed XML: AlephTPC_alignment.xml"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load and misalign ALEPH TPC geometry -------------------------
dd4hep_add_test_reg( AlignDet_AlephTPC_global_reset
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${AlignDet_INSTALL}/compact/AlephTPC.xml
             -destroy -no-interpreter
             -plugin DD4hep_GlobalAlignmentInstall
             -plugin DD4hep_XMLLoader file:${AlignDet_INSTALL}/compact/AlephTPC_alignment.xml
             -plugin DD4hep_XMLLoader file:${AlignDet_INSTALL}/compact/AlephTPC_reset.xml
  REGEX_PASS "Successfully parsed XML: AlephTPC_reset.xml"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
