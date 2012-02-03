cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DD4hep)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake) 

add_subdirectory(DDCore)
add_subdirectory(DDExamples/ILDExDet)
add_subdirectory(DDExamples/ILDExDisplay)
add_subdirectory(DDExamples/ILDExReco)



