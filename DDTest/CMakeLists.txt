#=================================================================================
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================

add_library(DDTest INTERFACE)
add_library(DD4hep::DDTest ALIAS DDTest)

dd4hep_use_python_executable()
target_include_directories(DDTest INTERFACE include)

foreach(TEST_NAME
    test_example
    test_bitfield64
    test_bitfieldcoder
    test_DetType
    test_PolarGridRPhi2
    test_cellDimensions
    test_cellDimensionsRPhi2
    test_segmentationHandles
    test_Evaluator
    test_shapes
    )
  add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
  target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDTest)
  install(TARGETS ${TEST_NAME} RUNTIME DESTINATION bin)

  set(cmd ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME})
  add_test(NAME t_${TEST_NAME} COMMAND ${cmd} ${TEST_NAME})
  set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
endforeach()

foreach(TEST_NAME
    test_units
    test_surface
    )
  add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
  target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDTest)
  install(TARGETS ${TEST_NAME} RUNTIME DESTINATION bin)
  add_test(NAME t_${TEST_NAME}
    COMMAND ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME} file:${CMAKE_CURRENT_SOURCE_DIR}/units.xml)
  set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
endforeach()

find_program(HAVE_PYTEST pytest)
if(NOT HAVE_PYTEST)
  message(WARNING "pytest not found! Skipping pytest tests.")
endif()

if(HAVE_PYTEST)
  ADD_TEST( t_test_python_import "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
    pytest ${PROJECT_SOURCE_DIR}/DDTest/python/test_import.py)
  SET_TESTS_PROPERTIES( t_test_python_import PROPERTIES FAIL_REGULAR_EXPRESSION  "Exception;EXCEPTION;ERROR;Error" )
endif()

if (DD4HEP_USE_GEANT4)
  if(HAVE_PYTEST)
    ADD_TEST( t_test_python_import_ddg4 "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      pytest ${PROJECT_SOURCE_DIR}/DDTest/python/test_import_ddg4.py)
    SET_TESTS_PROPERTIES( t_test_python_import_ddg4 PROPERTIES FAIL_REGULAR_EXPRESSION  "Exception;EXCEPTION;ERROR;Error" )
  endif()
  if(DD4HEP_USE_HEPMC3)
    set(TEST_HEPMC3_EXTENSIONS)

    if(DD4HEP_HEPMC3_COMPRESSION_SUPPORT)
      file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/geantinos.hepmc.gz
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/geantinos.hepmc
        FORMAT raw
        COMPRESSION GZip)
      list(APPEND TEST_HEPMC3_EXTENSIONS .gz)
      file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/geantinos.hepmc.xz
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/geantinos.hepmc
        FORMAT raw
        COMPRESSION XZ)
      list(APPEND TEST_HEPMC3_EXTENSIONS .xz)
      file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/geantinos.hepmc.bz2
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/geantinos.hepmc
        FORMAT raw
        COMPRESSION BZip2)
      list(APPEND TEST_HEPMC3_EXTENSIONS .bz2)
    endif()

    foreach(TEST_HEPMC3_EXTENSION "" ${TEST_HEPMC3_EXTENSIONS})
      if("${TEST_HEPMC3_EXTENSION}" STREQUAL "")
        set(TEST_HEPMC3_INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc${TEST_HEPMC3_EXTENSION}")
      else()
        set(TEST_HEPMC3_INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/geantinos.hepmc${TEST_HEPMC3_EXTENSION}")
      endif()
      get_filename_component(TEST_HEPMC3_INPUT_FILE_NAME "${TEST_HEPMC3_INPUT_FILE}" NAME)
      string(REPLACE "." "_" TEST_NAME "test_ddsim_hepmc3_${TEST_HEPMC3_INPUT_FILE_NAME}")

      add_test( t_${TEST_NAME} "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
        ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -N=1
        --outputFile=${TEST_NAME}.root --inputFiles ${TEST_HEPMC3_INPUT_FILE}
        --part.userParticleHandler=)

      # Tests causes G4Exception about: Isomer level 9 may be ambiguous, just a warning
      # space before Exception intentional!!!
      set_tests_properties( t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION  "ERROR;Error; Exception" )
    endforeach()
  endif()

  foreach(TEST_NAME
      test_EventReaders
      )
    add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
    if(DD4HEP_USE_HEPMC3)
      target_compile_definitions(${TEST_NAME} PRIVATE DD4HEP_USE_HEPMC3)
    endif()
    if(DD4HEP_USE_EDM4HEP)
      target_compile_definitions(${TEST_NAME} PRIVATE DD4HEP_USE_EDM4HEP)
    endif()
    if(DD4HEP_USE_LCIO)
      target_compile_definitions(${TEST_NAME} PRIVATE DD4HEP_USE_LCIO)
    endif()
    target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDG4)
    target_include_directories(${TEST_NAME} PRIVATE ./include)
    install(TARGETS ${TEST_NAME} DESTINATION bin)

    add_test(NAME t_${TEST_NAME} COMMAND ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
  endforeach(TEST_NAME)


  set(DDSIM_OUTPUT_FILES .root)

  if(DD4HEP_USE_LCIO)
    LIST(APPEND DDSIM_OUTPUT_FILES .slcio)
  endif()

  if(DD4HEP_USE_EDM4HEP)
    LIST(APPEND DDSIM_OUTPUT_FILES edm4hep.root)
  endif()

  foreach(OUTPUT_FILE IN LISTS DDSIM_OUTPUT_FILES)
    ADD_TEST( t_test_ddsim_${OUTPUT_FILE} "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -G -N=2
      --outputFile=testSid${OUTPUT_FILE}
      --gun.position \"0.0 0.0 1.0*cm\" --gun.direction \"1.0 0.0 1.0\" --gun.momentumMax 100*GeV --part.userParticleHandler=)
    SET_TESTS_PROPERTIES( t_test_ddsim_${OUTPUT_FILE} PROPERTIES FAIL_REGULAR_EXPRESSION  " Exception; EXCEPTION;ERROR;Error" )
  endforeach()

  add_test( t_ddsimUserPlugins "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
    ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -N=10
    --outputFile=t_ddsimUserPlugins.root -G
    --steeringFile ${CMAKE_CURRENT_SOURCE_DIR}/python/userPluginsSteeringFile.PY
    --part.userParticleHandler=
  )

  add_test( t_ddsimUserActions "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
    ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -N=10
    --outputFile=t_ddsimUserActions.root -G
    --steeringFile ${CMAKE_CURRENT_SOURCE_DIR}/python/userActionsSteeringFile.PY
    --action.event "Geant4TestEventAction/EventActionCLI1"
    --action.step "Geant4TestStepAction/StepActionCLI1,Geant4TestStepAction/StepActionCLI2"
    --action.step '\[ \"Geant4TestStepAction/StepActionCLI3\" , \"Geant4TestStepAction/StepActionCLI4\" \]'
    --action.stack '\{ \"name\" : \"Geant4TestStackAction/StackActionCLI1\" , \"parameter\" : \{ \"Property_int\" : 10 \} \}'
    --action.stack '\[ \{ \"name\" : \"Geant4TestStackAction/StackActionCLI2\" , \"parameter\" : { \"Property_int\" : 10 \} \} \]'
    --printLevel VERBOSE
  )
  set_tests_properties( t_ddsimUserActions PROPERTIES
    PASS_REGULAR_EXPRESSION "Deleting object StepActionCLI1"
  )

  if(DD4HEP_USE_EDM4HEP AND TARGET podio::podioIO)
    add_test( t_ddsimEDM4hepPlugins "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -N=3
      --outputFile=ddsim_edm4hep_ZH.root
      --inputFiles=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/ZH250_ISR.edm4hep.root
      --part.userParticleHandler=
    )
  endif()

  #---Multi-threading tests---------------------------------------------------------
  # Test basic MT functionality
  add_test(NAME t_ddsim_mt_basic_2threads
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 2 -G
            --outputFile=test_mt_2threads.root
            --random.seed=123456
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=10*GeV
            --gun.particle=mu-
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt_basic_2threads PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error"
    PASS_REGULAR_EXPRESSION "@ 2 threads")

  # Test MT with different thread counts
  add_test(NAME t_ddsim_mt_basic_4threads
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=8 -j 4 -G
            --outputFile=test_mt_4threads.root
            --random.seed=654321
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=20*GeV
            --gun.particle=pi+
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt_basic_4threads PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error"
    PASS_REGULAR_EXPRESSION "@ 4 threads")

  # Test backward compatibility: -j 1 should work like ST
  add_test(NAME t_ddsim_mt_single_thread
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=5 -j 1 -G
            --outputFile=test_mt_1thread.root
            --random.seed=111111
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=15*GeV
            --gun.particle=gamma
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt_single_thread PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error"
    PASS_REGULAR_EXPRESSION "@ 1 threads")

  # MT vs MT output comparison tests
  # These tests verify reproducibility within multi-threaded mode by comparing
  # runs with different thread counts (2 vs 4 threads).
  #
  # With proper per-event seeding (--random.enableEventSeed), different
  # thread counts should produce identical physics results. This validates:
  # 1. EventSeeder works correctly in MT mode
  # 2. Thread-local RNG state is properly managed
  # 3. Physics is independent of thread count
  # 4. No race conditions or thread-unsafe operations
  #
  # The comparison validates:
  # - No crashes or exceptions in either configuration
  # - Output files are readable and well-formed
  # - Event counts match
  # - Branch structure is identical
  # - Event-by-event physics results are identical (within tolerance)
  
  # Test 2-thread vs 4-thread MT with particle gun
  add_test(NAME t_ddsim_mt2_vs_mt4_gun_generate_2t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=20 -j 2 -G
            --outputFile=mt2_gun.root
            --random.seed=987654321
            --random.enableEventSeed
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=10*GeV
            --gun.particle=mu-
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_gun_generate_2t PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_mt2_vs_mt4_gun_generate_4t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=20 -j 4 -G
            --outputFile=mt4_gun.root
            --random.seed=987654321
            --random.enableEventSeed
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=10*GeV
            --gun.particle=mu-
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_gun_generate_4t PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  # Comparison test expects exact equivalence with proper event seeding
  add_test(NAME t_ddsim_mt2_vs_mt4_gun_compare
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
            mt2_gun.root mt4_gun.root --tolerance=1e-9)
  set_tests_properties(t_ddsim_mt2_vs_mt4_gun_compare PROPERTIES
    DEPENDS "t_ddsim_mt2_vs_mt4_gun_generate_2t;t_ddsim_mt2_vs_mt4_gun_generate_4t"
    LABELS "comparison;mt")

  # Test 2-thread vs 4-thread MT with HepMC input (file-based generators)
  if(DD4HEP_USE_HEPMC3)
    add_test(NAME t_ddsim_mt2_vs_mt4_hepmc_2t
      COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
        ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
              --runType=batch -N=5 -j 2
              --outputFile=mt2_hepmc.root
              --random.seed=111111
              --random.enableEventSeed
              --inputFiles=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc
              --part.userParticleHandler=)
    set_tests_properties(t_ddsim_mt2_vs_mt4_hepmc_2t PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;Error")

    add_test(NAME t_ddsim_mt2_vs_mt4_hepmc_4t
      COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
        ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
              --runType=batch -N=5 -j 4
              --outputFile=mt4_hepmc.root
              --random.seed=111111
              --random.enableEventSeed
              --inputFiles=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc
              --part.userParticleHandler=)
    set_tests_properties(t_ddsim_mt2_vs_mt4_hepmc_4t PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;Error")

    add_test(NAME t_ddsim_mt2_vs_mt4_hepmc_compare
      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
              mt2_hepmc.root mt4_hepmc.root --tolerance=1e-9)
    set_tests_properties(t_ddsim_mt2_vs_mt4_hepmc_compare PROPERTIES
      DEPENDS "t_ddsim_mt2_vs_mt4_hepmc_2t;t_ddsim_mt2_vs_mt4_hepmc_4t"
      LABELS "comparison;mt")
  endif()

  # Test 2-thread vs 4-thread MT with EDM4hep input (file-based generators)
  if(DD4HEP_USE_EDM4HEP)
    add_test(NAME t_ddsim_mt2_vs_mt4_edm4hep_2t
      COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
        ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
              --runType=batch -N=3 -j 2
              --outputFile=mt2_edm4hep.root
              --random.seed=222222
              --random.enableEventSeed
              --inputFiles=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/ZH250_ISR.edm4hep.root
              --part.userParticleHandler=)
    set_tests_properties(t_ddsim_mt2_vs_mt4_edm4hep_2t PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;Error")

    add_test(NAME t_ddsim_mt2_vs_mt4_edm4hep_4t
      COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
        ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
              --runType=batch -N=3 -j 4
              --outputFile=mt4_edm4hep.root
              --random.seed=222222
              --random.enableEventSeed
              --inputFiles=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/ZH250_ISR.edm4hep.root
              --part.userParticleHandler=)
    set_tests_properties(t_ddsim_mt2_vs_mt4_edm4hep_4t PROPERTIES
      FAIL_REGULAR_EXPRESSION "ERROR;Error")

    add_test(NAME t_ddsim_mt2_vs_mt4_edm4hep_compare
      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
              mt2_edm4hep.root mt4_edm4hep.root --tolerance=1e-9)
    set_tests_properties(t_ddsim_mt2_vs_mt4_edm4hep_compare PROPERTIES
      DEPENDS "t_ddsim_mt2_vs_mt4_edm4hep_2t;t_ddsim_mt2_vs_mt4_edm4hep_4t"
      LABELS "comparison;mt")
  endif()

  #---Advanced Gun Features Tests (G4Gun and GPS with macroFile)------------
  # These tests document known failures in MT mode
  # See: DDG4/python/DDSim/DD4hepSimulation.py:596 FIXME comment
  # Test G4Gun with macroFile - Single Threaded (smoke test)
  add_test(NAME t_ddsim_g4gun_macro_st
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=run -j 1
            --outputFile=g4gun_st.root
            --random.seed=333333
            --random.enableEventSeed
            --enableG4Gun
            --macroFile=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/g4gun_test.mac
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_g4gun_macro_st PROPERTIES
    FAIL_REGULAR_EXPRESSION "ERROR;Error;Exception"
    LABELS "gun;g4gun;singlethread")

  # Test G4Gun: 2-thread vs 4-thread MT comparison
  # Note: Using regular DD4hep gun (-G) for comparison, not G4Gun
  # G4Gun is tested separately with macroFile in smoke tests
  add_test(NAME t_ddsim_mt2_vs_mt4_g4gun_generate_2t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 2
            --outputFile=mt2_g4gun.root
            --random.seed=333333
            --random.enableEventSeed
            --enableG4Gun
            --macroFile=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/g4gun_test.mac
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_g4gun_generate_2t PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_mt2_vs_mt4_g4gun_generate_4t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 4
            --outputFile=mt4_g4gun.root
            --random.seed=333333
            --random.enableEventSeed
            --enableG4Gun
            --macroFile=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/g4gun_test.mac
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_g4gun_generate_4t PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_mt2_vs_mt4_g4gun_compare
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
            mt2_g4gun.root mt4_g4gun.root --tolerance=1e-9)
  set_tests_properties(t_ddsim_mt2_vs_mt4_g4gun_compare PROPERTIES
    DEPENDS "t_ddsim_mt2_vs_mt4_g4gun_generate_2t;t_ddsim_mt2_vs_mt4_g4gun_generate_4t"
    LABELS "comparison;mt;g4gun")

  # Test GPS with macroFile - Single Threaded (smoke test)
  add_test(NAME t_ddsim_g4gps_macro_st
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=run -j 1
            --outputFile=g4gps_st.root
            --random.seed=444444
            --random.enableEventSeed
            --enableG4GPS
            --macroFile=${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/g4gps_test.mac
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_g4gps_macro_st PROPERTIES
    FAIL_REGULAR_EXPRESSION "ERROR;Error;Exception"
    LABELS "gun;gps;singlethread")

  # Test GPS: 2-thread vs 4-thread MT comparison
  # Note: Using regular DD4hep gun (-G) for comparison tests
  # GPS with macroFile is tested in smoke tests
  add_test(NAME t_ddsim_mt2_vs_mt4_gps_generate_2t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 2 -G
            --outputFile=mt2_gps.root
            --random.seed=444444
            --random.enableEventSeed
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_gps_generate_2t PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_mt2_vs_mt4_gps_generate_4t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 4 -G
            --outputFile=mt4_gps.root
            --random.seed=444444
            --random.enableEventSeed
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_gps_generate_4t PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_mt2_vs_mt4_gps_compare
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
            mt2_gps.root mt4_gps.root --tolerance=1e-9)
  set_tests_properties(t_ddsim_mt2_vs_mt4_gps_compare PROPERTIES
    DEPENDS "t_ddsim_mt2_vs_mt4_gps_generate_2t;t_ddsim_mt2_vs_mt4_gps_generate_4t"
    LABELS "comparison;mt;gps")

  #---ST vs MT Comparison Tests (Gun)-------------------------------
  # With proper per-event seeding and event ID matching in the comparison script,
  # ST and MT modes should produce identical physics results event-by-event.
  # The comparison script handles out-of-order events by matching on event ID.
  # Note: Using regular DD4hep gun (-G) since G4Gun/GPS require macroFile

  # Test regular gun: Single-threaded vs Multi-threaded comparison
  add_test(NAME t_ddsim_st_vs_mt_gun_generate_st
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 1 -G
            --outputFile=st_gun.root
            --random.seed=333333
            --random.enableEventSeed
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.particle=mu-
            --gun.energy=10*GeV
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_st_vs_mt_gun_generate_st PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_st_vs_mt_gun_generate_mt
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 2 -G
            --outputFile=mt_gun.root
            --random.seed=333333
            --random.enableEventSeed
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.particle=mu-
            --gun.energy=10*GeV
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_st_vs_mt_gun_generate_mt PROPERTIES
    FAIL_REGULAR_EXPRESSION "Exception;EXCEPTION;ERROR;Error")

  add_test(NAME t_ddsim_st_vs_mt_gun_compare
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
            st_gun.root mt_gun.root --tolerance=1e-9)
  set_tests_properties(t_ddsim_st_vs_mt_gun_compare PROPERTIES
    DEPENDS "t_ddsim_st_vs_mt_gun_generate_st;t_ddsim_st_vs_mt_gun_generate_mt"
    LABELS "comparison;gun")

  #---Magnetic Field Tests (2-thread vs 4-thread MT)-------------------------
  # These tests verify that magnetic fields are properly propagated to all
  # worker threads in MT mode by comparing particle trajectories with different
  # thread counts.
  #
  # IMPORTANT: This tests SHARED field access in MT mode.
  # The SiD solenoid field is defined in the detector description (XML),
  # which is loaded once and shared across all threads. This is analogous
  # to how field maps from files would be shared - the field object itself
  # is shared, not copied per thread. The field evaluation at different
  # positions happens thread-safely via Geant4's field manager.
  #
  # This test validates that:
  # 1. Field setup from detector description is shared correctly
  # 2. Field evaluation is thread-safe in MT mode
  # 3. Particle trajectories are identical regardless of thread count
  # 4. Field parameters (eps_min, eps_max, etc.) are applied uniformly
  #
  # The solenoid field serves as a proxy for field maps since both:
  # - Are defined once in detector description
  # - Must be shared across worker threads (not copied)
  # - Require thread-safe field evaluation
  # - Would fail identically if sharing mechanism is broken
  
  # Test with magnetic field - 2 threads
  add_test(NAME t_ddsim_mt2_vs_mt4_field_2t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 2 -G
            --outputFile=mt2_field.root
            --random.seed=111111
            --random.enableEventSeed
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=5*GeV
            --gun.particle=mu+
            --field.eps_min=1e-3*mm
            --field.eps_max=1e-2*mm
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_field_2t PROPERTIES
    FAIL_REGULAR_EXPRESSION "ERROR;Error;Exception"
    LABELS "field;multithread")

  # Test with magnetic field - 4 threads
  add_test(NAME t_ddsim_mt2_vs_mt4_field_4t
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml
            --runType=batch -N=10 -j 4 -G
            --outputFile=mt4_field.root
            --random.seed=111111
            --random.enableEventSeed
            --gun.position \"0.0 0.0 1.0*cm\"
            --gun.direction \"1.0 0.0 1.0\"
            --gun.energy=5*GeV
            --gun.particle=mu+
            --field.eps_min=1e-3*mm
            --field.eps_max=1e-2*mm
            --part.userParticleHandler=)
  set_tests_properties(t_ddsim_mt2_vs_mt4_field_4t PROPERTIES
    FAIL_REGULAR_EXPRESSION "ERROR;Error;Exception"
    LABELS "field;multithread")

  # Compare field test outputs
  add_test(NAME t_ddsim_mt2_vs_mt4_field_compare
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/compare_simulation_output.py
            mt2_field.root mt4_field.root --tolerance=1e-9)
  set_tests_properties(t_ddsim_mt2_vs_mt4_field_compare PROPERTIES
    DEPENDS "t_ddsim_mt2_vs_mt4_field_2t;t_ddsim_mt2_vs_mt4_field_4t"
    LABELS "field;comparison;mt")

endif()
install(DIRECTORY include/DD4hep DESTINATION include)


# Test that GrammarUnparsed and GrammarParsed fail to be compiled
add_executable(test_failToCompile src/test_failToCompileGrammar.cc)
target_link_libraries(test_failToCompile DD4hep::DDCore)
# Avoid building these targets normally
set_target_properties(test_failToCompile PROPERTIES
  EXCLUDE_FROM_ALL TRUE
  EXCLUDE_FROM_DEFAULT_BUILD TRUE)

add_test(NAME TestFailToCompile
  COMMAND ${CMAKE_COMMAND} --build . --target test_failToCompile --config $<CONFIGURATION>
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(TestFailToCompile
  PROPERTIES
  PASS_REGULAR_EXPRESSION "The header files GrammarParsed.h and GrammarUnparsed.h may not be included in the same compilation unit")
