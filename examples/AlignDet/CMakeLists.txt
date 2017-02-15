#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#--------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package ( AlignDet MAJOR 0 MINOR 0 PATCH 1
  USES           [ROOT   REQUIRED COMPONENTS Geom GenVector MathCore] 
                 [DD4hep REQUIRED COMPONENTS DDCore DDCond DDAlign]
  OPTIONAL       XERCESC
  INCLUDE_DIRS   include
  )

dd4hep_add_plugin( AlignDetExample SOURCES src/*.cpp  )
dd4hep_install_dir( compact DESTINATION examples/AlignDet )
dd4hep_configure_scripts( AlignDet DEFAULT_SETUP WITH_TESTS)

#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( test_AlignDet_Telescope_dump_geometry
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy
  -compact file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
  -plugin DD4hepDetectorDump
  REGEX_PASS "/world/Telescope/module_9 NumDau\\:1 VolID\\:00000903 Place")
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( test_AlignDet_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_populate
     -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 10
  REGEX_PASS "\\[7438F09CE3AD4ACA\\] -> \\[7438F09CE3AD4ACA\\] /world/Telescope/module_9/sensor#alignment/Transformations")
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( test_AlignDet_Telescope_read_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml
     -input  file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
     -deltas file:${DD4hep_DIR}/examples/Conditions/data/repository.xml
  REGEX_PASS "\\[7438F09CE3AD4ACA\\] -> \\[7438F09CE3AD4ACA\\] /world/Telescope/module_9/sensor#alignment/Transformations")
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_AlignDet_Telescope_stress
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_AlignmentExample_stress
      -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 20 -runs 111
  REGEX_PASS "Summary: # of IOV:  20  # of Runs: 111")
#
#---Testing: Write out alignment file from the alignment data using a detelement scan
dd4hep_add_test_reg( test_AlignDet_Telescope_dump_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml
  -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
  -delta file:${DD4hep_DIR}/examples/Conditions/data/repository.xml
  -plugin DD4hep_ConditionsXMLRepositoryWriter -iov_type run -iov_value 1500
  REGEX_PASS "-- DDCond conditions for DetElement /world/Telescope/module_9/sensor Total of 1 Entries.  -->")
#
#---Testing: Write out alignment file from the alignment data using a detelement scan
dd4hep_add_test_reg( test_AlignDet_Telescope_write_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml
  -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
  -delta file:${DD4hep_DIR}/examples/Conditions/data/repository.xml
  -plugin DD4hep_ConditionsXMLRepositoryWriter -iov_type run -iov_value 1500 -manager -output new_cond.xml
  REGEX_PASS "Successfully wrote conditions file: new_cond.xml")
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( test_AlignDet_Telescope_readback_xml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_read_xml
     -input  file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
     -deltas file:./new_cond.xml
  REGEX_PASS "\\[7438F09CE3AD4ACA\\] -> \\[7438F09CE3AD4ACA\\] /world/Telescope/module_9/sensor#alignment/Transformations")
#
#---Testing: Load Telescope geometry and read and print alignments --------
dd4hep_add_test_reg( test_AlignDet_Telescope_align_new
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_align_telescope
     -input  file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
     -setup  file:${DD4hep_DIR}/examples/Conditions/data/manager.xml 
  REGEX_PASS "World transformation of: /world/Telescope/module_3/sensor  Tr:\\( 3.9e\\+02,       0,     4.9 \\[cm\\]\\)")
#
#---Testing: Extended stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_AlignDet_CLICSiD_stress_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_AlignmentExample_stress
      -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 10 -runs 100
  REGEX_PASS "Summary: # of IOV:  10  # of Runs: 100")
#
#---Testing: Load ALEPH TPC geometry --------------------------------------
dd4hep_add_test_reg( test_AlignDet_AlephTPC_load
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${DD4hep_DIR}/examples/AlignDet/compact/AlephTPC.xml
             -destroy -no-interpreter -plugin DD4hep_GlobalAlignmentInstall
  REGEX_PASS "189 nodes/ 23 volume UID's in LCDD Geometry" )
#
#---Testing: Load and misalign ALEPH TPC geometry -------------------------
dd4hep_add_test_reg( test_AlignDet_Global_AlephTPC_align
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${DD4hep_DIR}/examples/AlignDet/compact/AlephTPC.xml
             -destroy -no-interpreter
             -plugin DD4hep_GlobalAlignmentInstall
             -plugin DD4hepXMLLoader file:${DD4hep_DIR}/examples/AlignDet/compact/AlephTPC_alignment.xml BUILD_DEFAULT
  REGEX_PASS "Successfully parsed XML: AlephTPC_alignment.xml")
#
#---Testing: Load and misalign ALEPH TPC geometry -------------------------
dd4hep_add_test_reg( test_AlignDet_Global_AlephTPC_reset
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_AlignDet.sh"
  EXEC_ARGS  geoPluginRun
             -input file:${DD4hep_DIR}/examples/AlignDet/compact/AlephTPC.xml
             -destroy -no-interpreter
             -plugin DD4hep_GlobalAlignmentInstall
             -plugin DD4hepXMLLoader file:${DD4hep_DIR}/examples/AlignDet/compact/AlephTPC_alignment.xml
             -plugin DD4hepXMLLoader file:${DD4hep_DIR}/examples/AlignDet/compact/AlephTPC_reset.xml
  REGEX_PASS "Successfully parsed XML: AlephTPC_reset.xml" )
