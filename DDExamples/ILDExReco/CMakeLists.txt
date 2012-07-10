cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/DDExamples/ILDExDet/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR})

add_executable(ILDExReco ILDExReco.cpp src/GearTPC.cpp)
add_executable(TestTPCPrototype TestTPCPrototype.cpp src/GearTPC.cpp)
add_executable(TPCDigitisation TPCDigitisation.cpp src/GearTPC.cpp)
add_executable(TPCHitReco TPCHitReco.cpp src/GearTPC.cpp)
add_executable(CalculateTPCPadBinning CalculateTPCPadBinning.cpp src/GearTPC.cpp)
add_executable(TPCToySimulation TPCToySimulation.cpp src/GearTPC.cpp)
add_executable(TPCMaterialScan TPCMaterialScan.cpp src/GearTPC.cpp)
target_link_libraries(ILDExReco DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(TestTPCPrototype DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(TPCDigitisation DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(TPCHitReco DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(CalculateTPCPadBinning DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(TPCToySimulation DD4hepCore ILDEx ${ROOT_LIBRARIES})
target_link_libraries(TPCMaterialScan DD4hepCore ILDEx ${ROOT_LIBRARIES})

