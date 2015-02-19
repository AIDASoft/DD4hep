###################################################
#
# Simple CMakeList.txt file that allows to
# build all DD4hep examples in one go, e.g.
#
# . ./bin/thisDD4hep.sh
# cd examples ;
# mkdir build ; cd build
# cmake ..
#
# F.Gaede, DESY, 2013
#
##################################################
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
 	
project(DD4hepExample)
 	
option(BUILD_TESTING "Enable and build tests" ON)

#---Testing-------------------------------------------------------------------------
if(BUILD_TESTING)
include(CTest)
enable_testing()

set(BUILDNAME "${CMAKE_SYSTEM}-${CMAKE_CXX_COMPILER}-${CMAKE_BUILD_TYPE}" CACHE STRING "set build string for cdash")

endif(BUILD_TESTING)

#----------------------------------------------------------------------------------
#
# (un)comment as needed
#
 	
add_subdirectory(AlignDet)

add_subdirectory(CLICSiD)
 	
add_subdirectory(ClientTests)

add_subdirectory(SimpleDetector)


