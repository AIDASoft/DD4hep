cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

find_package(ROOT)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/DDExamples/ILDExDet/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR})

add_executable(ILDExReco src/recons.cpp src/GearTPC.cpp)
target_link_libraries(ILDExReco DetDesc ILDEx ${ROOT_LIBRARIES})

