
#--------------------------------------------------

include_directories( 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIR}
  )

link_libraries(DD4hepG4 DD4hepCore DDSegmentation ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})
#--------------------------------------------------

SET( test_name "test_example" )
ADD_EXECUTABLE( ${test_name} ./src/${test_name}.cc )
ADD_TEST( t_${test_name} "${EXECUTABLE_OUTPUT_PATH}/${test_name}" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED" )

SET( test_name "test_units" )
ADD_EXECUTABLE( ${test_name} ./src/${test_name}.cc )
ADD_TEST( t_${test_name} "${EXECUTABLE_OUTPUT_PATH}/${test_name}" ${CMAKE_CURRENT_SOURCE_DIR}/units.xml )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED" )

#--------------------------------------------------


install(FILES ./include/DD4hep/DDTest.h
  DESTINATION include/DD4hep
  )

