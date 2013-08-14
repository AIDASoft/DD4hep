cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/DDExamples/ILDExDet/include
                     ${CMAKE_SOURCE_DIR}/DDExamples/ILDExReco/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR})

add_executable(TestILDTPC TestILDTPC.cpp ${CMAKE_SOURCE_DIR}/DDExamples/ILDExReco/src/GearTPC.cpp)
add_executable(TestTPCPrototype TestTPCPrototype.cpp ${CMAKE_SOURCE_DIR}/DDExamples/ILDExReco/src/GearTPC.cpp)

target_link_libraries(TestILDTPC DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(TestTPCPrototype DD4hepCore ILDEx ${ROOT_LIBRARIES})


ADD_TEST(TestILDTPC TestILDTPC file:${CMAKE_SOURCE_DIR}/DDExamples/ILDExTest/ILDExTPC.xml)
ADD_TEST(TestILDTPC_Output grep PASSED ${CMAKE_CURRENT_BINARY_DIR}/testOutput_TestILCTPC.txt)
ADD_TEST(TestTPCPrototype TestTPCPrototype file:${CMAKE_SOURCE_DIR}/DDExamples/ILDExTest/TPCPrototype.xml)
ADD_TEST(TestTPCPrototype_Output grep PASSED ${CMAKE_CURRENT_BINARY_DIR}/testOutput_TestTPCPrototype.txt)
                                                                         