
#--------------------------------------------------

include_directories( ./include )

#--------------------------------------------------

SET( test_name "example_test" )

ADD_EXECUTABLE( ${test_name} ./src/${test_name}.cc )

ADD_TEST( t_${test_name} "${EXECUTABLE_OUTPUT_PATH}/${test_name}" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED" )

#--------------------------------------------------


install(FILES ./include/DDTest.h
  DESTINATION include/DD4hep
  )
